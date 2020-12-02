/**
 * @since 2020-11-22
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_serving_v2.c
 */

#include "async_serving_v2.h"

#include "logging.h"
#include "serve_request.h"
#include "async_threading.h"
#include "async_process.h"

#include <dc_utils/dlinked_list.h>
#include <dc/semaphore.h>
#include <dc/stdlib.h>
#include <dc/sys/socket.h>

#include <string.h>
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dc/unistd.h>

#define BYTES_WRITTEN_MISS_MATCH -1
#define READ_ALLOC_COEFF 8
#define CONCURRENT_CONN_SEM "/queue_sem"
#define REQ_AVAILABLE_SEM "/req_avail_sem"
#define LISTENING_FD_PASS_SEM_NAME "/listening_fd_pass_sem"
#define SERVING true
#define POOL_INCREASE_COUNT 20


_Noreturn void * async_handling_procedure( void * vargs ) {
    async_handler_args * args = ( async_handler_args * ) vargs;
    while ( SERVING ) {
        int req_fd = args->get_req_fd( args );
        req_thread_args_t * serve_args = dc_malloc( sizeof( req_thread_args_t ));
        serve_args->conn_fd = req_fd;
        serve_args->sem = args->concurrent_conn_sem;
        serve_args->server_cfg = *args->server_cfg;
        serve_request( serve_args );
    }
}


void increase_handler_pool( dlinked_list * pool,
                            async_configs * async_cfg,
                            async_func_maker_t async_func_maker, async_func_t async_func ) {
    for ( int i = 0; i < POOL_INCREASE_COUNT; i++ ) {
        dlinked_push( pool, async_func_maker( async_cfg, async_func ));
    }
}



void get_async_cfg( async_configs * async_cfg, server_config_t * server_cfg ) {
    if ( sem_unlink( REQ_AVAILABLE_SEM ) == 1 && errno != ENOENT ) {
        fprintf( stderr, "Error unlinking request available semaphore! %s", strerror(errno));
        exit( EXIT_FAILURE );
    }

    if ( sem_unlink( CONCURRENT_CONN_SEM ) == 1 && errno != ENOENT ) {
        fprintf( stderr, "Error unlinking concurrent connections semaphore! %s", strerror(errno));
        exit( EXIT_FAILURE );
    }

    if ( sem_unlink( LISTENING_FD_PASS_SEM_NAME ) == 1 && errno != ENOENT ) {
        fprintf( stderr, "Error unlinking request available semaphore! %s", strerror(errno));
        exit( EXIT_FAILURE );
    }

    dlinked_list * conn_queue;
    if ( server_cfg->concurrency_model == THREAD ) {
        conn_queue = dlinked_create_list();
        async_cfg->req_queue = conn_queue;
        async_cfg->get_req_fd = thread_get_conn_fd;
    } else {
        async_cfg->get_req_fd = process_get_req_fd;
    }
    async_cfg->server_cfg = server_cfg;
    async_cfg->listening_pass_fd_sem = dc_sem_open( LISTENING_FD_PASS_SEM_NAME, O_CREAT, 0640, 0 );
    async_cfg->req_avail_sem = dc_sem_open( REQ_AVAILABLE_SEM, O_CREAT, 0640, 0 );
    async_cfg->concurrent_conn_sem = dc_sem_open( CONCURRENT_CONN_SEM, O_CREAT, 0640, server_cfg->max_concurrent_conn );
}

_Noreturn void serve( server_config_t * server_cfg, int listen_socket_fd ) {
    printf("Serving model: pool\n");
    async_configs async_cfg;
    get_async_cfg( &async_cfg, server_cfg );

    void (* put_req_fd)( int, async_configs * );
    async_func_maker_t * async_func_maker;
    if ( server_cfg->concurrency_model == THREAD ) {
        put_req_fd = thread_put_req_fd;
        async_func_maker = make_thread;
    } else {
        put_req_fd = process_put_req_fd;
        async_func_maker = make_process;
    }

    dlinked_list * handler_pool = dlinked_create_list();
    increase_handler_pool( handler_pool, &async_cfg, async_func_maker, async_handling_procedure );

    while ( SERVING ) {
        sem_wait( async_cfg.concurrent_conn_sem );

        int new_conn = dc_accept( listen_socket_fd, NULL, NULL);
        put_req_fd( new_conn, &async_cfg );

    }
}
