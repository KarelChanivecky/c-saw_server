/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * server_driver.c
 */

#include "server_driver.h"
#include "http_req_parser.h"
#include "handle_req.h"
#include "http_res_encoder.h"

#include <dc/unistd.h>
#include <dc/sys/socket.h>
#include <dc/semaphore.h>
#include "dc/pthread.h"
#include "dc/stdlib.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

#define CONCURRENT_CONN_SEM "/queue_sem"
#define READ_ALLOC_COEFF 8

struct req_thread_args_t {
    sem_t * sem;
    int conn_fd;
    server_config_t server_cfg;
};
typedef struct req_thread_args_t req_thread_args_t;

int get_socket( server_config_t * server_cfg ) {
    int listen_socket_fd;
    struct sockaddr_in addr;
    listen_socket_fd = dc_socket( server_cfg->sin_family, SOCK_STREAM, 0 );
    memset( &addr, 0, sizeof( struct sockaddr_in ));
    addr.sin_family = server_cfg->sin_family;
    addr.sin_port = htons( server_cfg->port );
    addr.sin_addr.s_addr = htonl( server_cfg->addr );
    dc_bind( listen_socket_fd, ( struct sockaddr * ) &addr, sizeof( struct sockaddr_in ));
    return listen_socket_fd;
}

char * get_request_string( int conn_fd, size_t buffer_size ) {
    size_t allocated_space = buffer_size * READ_ALLOC_COEFF;
    char * req_string = malloc(sizeof(char) * buffer_size * READ_ALLOC_COEFF);
    char buffer[buffer_size];
    size_t bytes_read = read(conn_fd, buffer, buffer_size);
    size_t total_bytes_read = 0;
    while (bytes_read != 0) {
        total_bytes_read += bytes_read;
        if ( 0 < allocated_space - total_bytes_read ) {
            req_string = dc_realloc(&req_string,
                                    sizeof(char) * allocated_space + (buffer_size * READ_ALLOC_COEFF));
        }
        strncat(req_string, buffer, bytes_read);
        bytes_read = read(conn_fd, buffer, buffer_size);
    }
    return req_string;
}

void write_res_string(int conn_fd, char * res_string, size_t write_buffer_size ) {
    size_t response_len = strlen(res_string);
    while ( 0 < response_len ) {
        size_t bytes_written = dc_write(conn_fd, &res_string, write_buffer_size);
        response_len -= bytes_written;
        res_string += bytes_written;
    }
}

void log_action(http_req_t req, http_res_t res ) {
    printf("REQUEST:\n"); // TODO decide format of logging
    printf("RESPONSE:\n");
}

void * serve_request( void * v_args ) {
    req_thread_args_t args = *(req_thread_args_t *) v_args;
    sem_t * concurrent_conn_sem = args.sem;
    server_config_t server_cfg = args.server_cfg;

    int conn_fd = args.conn_fd;
    dc_sem_wait( concurrent_conn_sem );
    char * req_string = get_request_string( conn_fd, server_cfg.read_buffer_size);
    http_req_t req;
    int req_parse_status = parse_http_req( &req, req_string );
    // possibly error check?
    http_res_t res;
    int res_handle_status = handle_req( &req, &res );
    // possibly error check?
    char * res_string = http_res_encode(&res);
    if (server_cfg.log_connections) {
        log_action(req, res);
    }
    write_res_string(conn_fd, res_string, server_cfg.write_buffer_size)
    dc_free((void*)&res_string);
    dc_free((void*)&req_string);
    dc_sem_post(concurrent_conn_sem);
    return NULL;
}

void server_loop( server_config_t * server_cfg, int new_conn, sem_t * concurrent_conn_sem ) {
     req_thread_args_t serve_args = {
            concurrent_conn_sem,
            new_conn,
            *server_cfg
    };
    if (server_cfg->concurrency_model == THREAD) {
        pthread_t thread;
        dc_pthread_create( &thread, NULL, serve_request, &serve_args);
    } else {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Could not create request handling process!");
        }
        if (pid == 0) {
            serve_request(&serve_args);
            exit(EXIT_SUCCESS); // CHILD EXITS!
        }
    }
}

_Noreturn int start_server( server_config_t * server_cfg ) {
    printf("Opening listener socket\n");
    int listen_socket_fd = get_socket( server_cfg );
    printf("Listener socket opened\n");
    bool serving = true;
    sem_t * concurrent_conn_sem = dc_sem_open( CONCURRENT_CONN_SEM, O_CREAT, 0640, server_cfg->max_concurrent_conn );
    dc_listen( listen_socket_fd, server_cfg->max_open_conn );
    printf("Listening....\n");
    while ( serving ) {
//        struct sockaddr requester;
//        socklen_t requester_len;
        int new_conn = accept(listen_socket_fd, NULL, NULL);
        server_loop( server_cfg, listen_socket_fd, concurrent_conn_sem);
    }
}

#include "server_driver.h"
