/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * serve_request.c
 */

#include "serve_request.h"
#include "logging.h"
#include "../return_codes.h"
#include "../http_res_field_builders.h"

#include <dc/unistd.h>
#include <dc/semaphore.h>
#include <dc/stdlib.h>

#include <string.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>



char * get_request_string( int conn_fd, size_t buffer_size ) {
    size_t allocated_space = buffer_size * READ_ALLOC_COEFF;
    char * req_string = dc_malloc( sizeof( char ) * buffer_size * READ_ALLOC_COEFF );
    char buffer[buffer_size];
    size_t total_bytes_read = 0;

    bool reading = true;
    while ( reading ) {
        size_t bytes_read = dc_read( conn_fd, buffer, buffer_size );
        total_bytes_read += bytes_read;

        if ( (long long)allocated_space - (long long)total_bytes_read < 0 ) {
            req_string = dc_realloc( req_string,
                                     sizeof( char ) * allocated_space + ( buffer_size * READ_ALLOC_COEFF ));
        }

        strncat( req_string, buffer, bytes_read );

        char * last_chars = req_string + total_bytes_read - 2;
        if ( strncmp( last_chars, REQ_END, 2 ) == 0 ) {
            reading = false;
        }
    }
    req_string = dc_realloc( req_string, sizeof( char ) * ( total_bytes_read + 1 ));
    req_string[total_bytes_read] = 0;
    return req_string;
}

int write_res_string( int conn_fd, char * res_string, size_t write_buffer_size ) {
    size_t response_len = strlen( res_string );
    size_t bytes_to_write = write_buffer_size < response_len ? write_buffer_size : response_len;
    while ( bytes_to_write ) {
        size_t bytes_written = dc_write( conn_fd, res_string, bytes_to_write );
        if ( bytes_written != bytes_to_write ) {
            return BYTES_WRITTEN_MISS_MATCH;
        }
        response_len -= bytes_written;
        res_string += bytes_written;
        bytes_to_write = write_buffer_size < response_len ? write_buffer_size : response_len;
    }
    return SUCCESS;
}

void * serve_request( void * v_args ) {
    req_thread_args_t args = *( req_thread_args_t * ) v_args;
    sem_t * concurrent_conn_sem = args.sem;
    server_config_t server_cfg = args.server_cfg;

    int conn_fd = args.conn_fd;
    dc_sem_wait( concurrent_conn_sem );
    char * req_string = get_request_string( conn_fd, server_cfg.read_buffer_size );
    log_requester(conn_fd);
    printf("REQUEST\n%s", req_string);
    http_req_t req;
    int req_parse_status = parse_http_req( &req, req_string );
//     possibly error check?
//    http_res_t res;
//    int res_handle_status = handle_req( &req, &res );
//     possibly error check?
//    char * res_string = http_res_encode( &res );
//    if ( server_cfg.log_connections ) {
//        log_action( conn_fd, req, res );
//    }
//    write_res_string(conn_fd, res_string, server_cfg.write_buffer_size);

    if (write_res_string( conn_fd, req_string, server_cfg.write_buffer_size )
        ==
        BYTES_WRITTEN_MISS_MATCH
        &&
        server_cfg.log_connections) {

        fprintf(stderr, "Error writing to socket while responding %s", strerror(errno));
    }

    //    free(res_string);
    free( req_string );
    free(v_args);
    dc_sem_post( concurrent_conn_sem );
    dc_close( args.conn_fd );
    return NULL;
}
