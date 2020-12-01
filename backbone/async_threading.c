/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_threading.c
 */


#include "async_threading.h"
#include "logging.h"
#include "serve_request.h"
#include <dc_utils/dlinked_list.h>
#include <dc/stdlib.h>
#include <dc/pthread.h>
#include <dc/semaphore.h>


void thread_put_req_fd( int conn_fd, async_configs * async_cfg ) {
    dlinked_list * req_queue = async_cfg->req_queue;
    int * fd = ( int * ) dc_malloc( sizeof( int ));
    *fd = conn_fd;
    dlinked_push( req_queue, fd );
    dc_sem_post( async_cfg->req_avail_sem );
}

int thread_get_conn_fd( async_handler_args * handler_args ) {
    dc_sem_wait(handler_args->req_avail_sem);
    int * fd_p = ( int * ) dlinked_pop_head( handler_args->req_queue );
    int fd = *fd_p;
    free( fd_p );
    return fd;
}

async_handler_args * make_thread( async_configs * async_cfg, async_func_t async_func ) {
    async_handler_args * args = ( async_handler_args * ) dc_malloc( sizeof( async_handler_args ));
    args->get_req_fd = async_cfg->get_req_fd;
//    args->free_threads = async_cfg->free_threads;
    args->concurrent_conn_sem = async_cfg->concurrent_conn_sem;
    args->req_avail_sem = async_cfg->req_avail_sem;
    args->server_cfg = async_cfg->server_cfg;
    args->req_queue = async_cfg->req_queue;
    args->listening_pass_fd_sem = NULL;
    dc_pthread_create( &args->id.t, 0, async_func, args );
    return args;
}
