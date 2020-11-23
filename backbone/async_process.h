/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_process.h
 */

#ifndef C_SAW_SERVER_ASYNC_PROCESS_H
#define C_SAW_SERVER_ASYNC_PROCESS_H

#include "async_serving_v2.h"

async_handler_args * make_process( async_configs * async_cfg, async_func_t);

int process_get_req_fd( int argc, ...);

void process_put_req_fd( int conn_fd, ...);


#endif //C_SAW_SERVER_ASYNC_PROCESS_H
