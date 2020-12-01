/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * serve_request.h
 */

#include <semaphore.h>
#include "../server_config.h"

#ifndef C_SAW_SERVER_SERVE_REQUEST_H
#define C_SAW_SERVER_SERVE_REQUEST_H

#define BYTES_WRITTEN_MISS_MATCH -1
#define READ_ALLOC_COEFF 8

struct req_thread_args_t {
    sem_t * sem;
    int conn_fd;
    server_config_t server_cfg;
};
typedef struct req_thread_args_t req_thread_args_t;

void * serve_request( void * v_args );

#endif //C_SAW_SERVER_SERVE_REQUEST_H
