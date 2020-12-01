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
#include "../handle_req.h"

#include <dc/unistd.h>
#include <dc/semaphore.h>
#include <dc/stdlib.h>

#include <string.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include "unistd.h"


void free_http_req( http_req_t * req ) {
    if ( req->if_modified_since ) {
        free( req->if_modified_since );
    }
    if ( req->request_URI ) {
        free( req->request_URI );
    }
    if ( req->request_type ) {
        free( req->request_type );
    }
    if ( req->method ) {
        free( req->method );
    }
    if ( req->protocol_version ) {
        free( req->protocol_version );
    }
    if ( req->referer ) {
        free( req->referer );
    }
    if ( req->user_agent ) {
        free( req->user_agent );
    }
    if ( req->from ) {
        free( req->from );
    }
    if ( req->authorization ) {
        free( req->authorization );
    }
}

char * get_request_string( int conn_fd, size_t buffer_size ) {
    size_t allocated_space = buffer_size * READ_ALLOC_COEFF;
    char * req_string = dc_malloc( sizeof( char ) * buffer_size * READ_ALLOC_COEFF );
    char buffer[buffer_size];
    size_t total_bytes_read = 0;

    bool reading = true;
    while ( reading ) {
        size_t bytes_read = dc_read( conn_fd, buffer, buffer_size );
        total_bytes_read += bytes_read;

        if (( long long ) allocated_space - ( long long ) total_bytes_read < 0 ) {
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

int write_res_string( int conn_fd, uint8_t * res_string, size_t response_len, size_t write_buffer_size ) {
    size_t bytes_written = write( conn_fd, res_string, response_len + 1 );
    if ( bytes_written != response_len + 1 ) {
        return BYTES_WRITTEN_MISS_MATCH;
    }
    return SUCCESS;
}

void * serve_request( void * v_args ) {
    req_thread_args_t args = *( req_thread_args_t * ) v_args;
    sem_t * concurrent_conn_sem = args.sem;
    server_config_t server_cfg = args.server_cfg;

    int conn_fd = args.conn_fd;
    char * req_string = get_request_string( conn_fd, server_cfg.read_buffer_size );
    log_requester( conn_fd );
    http_req_t req;
    parse_http_req( &req, req_string, &server_cfg );
//    possibly error check ?
    size_t body_len = 0;
    http_res_t res;

    handle_req( &req, &res, &server_cfg, &body_len );
//    possibly error check ?

    uint8_t * res_string = http_res_encode( &res, &body_len );
    if ( server_cfg.log_connections ) {
        printf( "\nREQUEST\n%s\n", req_string );
        printf( "\nRESPONSE\n%s\n", res_string );
//        log_action( conn_fd, &req, &res );
    }


    if ( write_res_string( conn_fd, res_string, body_len, server_cfg.write_buffer_size )

         ==
         BYTES_WRITTEN_MISS_MATCH
         &&
         server_cfg.log_connections ) {
        fprintf( stderr, "Error writing to socket while responding %s", strerror(errno));
    }
    free_http_req( &req );
    free( res_string );
    dc_close( args.conn_fd );
    dc_sem_post( concurrent_conn_sem );
    free( req_string );
    free( v_args );
    return NULL;
}
