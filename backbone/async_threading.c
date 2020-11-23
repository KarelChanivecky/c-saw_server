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
#include <stdio.h>
#include <stdarg.h>
#include <dc/pthread.h>
#include "stdint.h"

#define VOIDP_TO_INT(v) (int)(uintptr_t)(v)

// credits: https://stackoverflow.com/questions/22751762/how-to-make-compiler-not-show-int-to-void-pointer-cast-warnings
#define INT_TO_VOIDP(i) (void*)(uintptr_t)(i)

void thread_put_req_fd( int conn_fd, ...) {
    va_list args;
    dlinked_list * req_queue;

    va_start(args, conn_fd);
    req_queue = va_arg (args, dlinked_list *);

    dlinked_push( req_queue, INT_TO_VOIDP( conn_fd) );
}

int thread_get_conn_fd(int argc, ...) {
    va_list args;
    async_handler_args * handler_args;
    va_start(args, argc);
    handler_args = va_arg (args, async_handler_args *);
    return VOIDP_TO_INT( dlinked_pop_head( handler_args->req_queue));
}

async_handler_args * make_thread( async_configs * async_cfg, async_func_t async_func) {
    async_handler_args * args = (async_handler_args*) dc_malloc( sizeof(async_handler_args));
    args->get_req_fd = async_cfg->get_req_fd;
    args->free_threads = async_cfg->free_threads;
    args->concurrent_conn_sem = async_cfg->concurrent_conn_sem;
    args->req_sem = async_cfg->req_sem;
    args->server_cfg = async_cfg->server_cfg;
    args->req_queue = NULL;
    dc_pthread_create(&args->id.t, 0, async_func, args);
    return args;
}
