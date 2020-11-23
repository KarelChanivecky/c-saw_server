/**
 * @since 2020-11-22
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_serving_v1.c
 */


#include "async_serving_v1.h"

#include "logging.h"
#include "serve_request.h"
#include <dc/semaphore.h>
#include <dc/stdlib.h>
#include <dc/pthread.h>
#include <dc/sys/socket.h>

#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


#define CONCURRENT_CONN_SEM "/queue_sem"
#define SERVING true



void server_loop( server_config_t * server_cfg, int new_conn, sem_t * concurrent_conn_sem ) {
    req_thread_args_t * serve_args = dc_malloc(sizeof(req_thread_args_t));
    serve_args->conn_fd = new_conn;
    serve_args->sem = concurrent_conn_sem;
    serve_args->server_cfg = *server_cfg;

    if ( server_cfg->concurrency_model == THREAD ) {
        pthread_t thread;
        dc_pthread_create( &thread, NULL, serve_request, serve_args );
        dc_pthread_detach(thread);
    } else {
        pid_t pid = fork();
        if ( pid == -1 ) {
            perror( "Could not create request handling process!" );
        }
        if ( pid == 0 ) {
            serve_request( &serve_args );
            exit( EXIT_SUCCESS ); // CHILD EXITS!
        }
        waitpid( pid, NULL, WNOHANG | WUNTRACED | WCONTINUED );
    }
}

_Noreturn void serve(server_config_t * server_cfg, int listen_socket_fd) {
    if ( sem_unlink( CONCURRENT_CONN_SEM ) == 1 && errno != ENOENT ) {
        fprintf( stderr, "Error unlinking semaphore! %s", strerror(errno));
        exit( EXIT_FAILURE );
    }
    sem_t * concurrent_conn_sem = dc_sem_open( CONCURRENT_CONN_SEM, O_CREAT, 0640, server_cfg->max_concurrent_conn );
    while ( SERVING ) {
        int new_conn = dc_accept( listen_socket_fd, NULL, NULL);
        server_loop( server_cfg, new_conn, concurrent_conn_sem );
    }
}
