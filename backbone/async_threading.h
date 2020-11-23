/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_threading.h
 */
//
// Created by karel on 2020-11-23.
//

#ifndef C_SAW_SERVER_ASYNC_THREADING_H
#define C_SAW_SERVER_ASYNC_THREADING_H

#include "async_serving_v2.h"

void thread_put_req_fd( int conn_fd, ...);

int thread_get_conn_fd(int argc, ...);

async_handler_args * make_thread( async_configs * async_cfg, async_func_t);

#endif //C_SAW_SERVER_ASYNC_THREADING_H
