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

/**
 * Receive req_fd socket from socket
 *
 * This process is called within the child process
 *
 * @param argc argument count
 * @param ...
 * @return
 */
int process_get_req_fd( async_handler_args * handler_args );

/**
 * Send req_fd over socket.
 *
 * This function occurs in the parent process.
 * @param req_fd the client's fd
 * @param async_cfg struct async_configs
 */
void process_put_req_fd( int req_fd, async_configs * async_cfg);


#endif //C_SAW_SERVER_ASYNC_PROCESS_H
