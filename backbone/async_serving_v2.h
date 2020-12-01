/**
 * @since 2020-11-22
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_serving_v2.h
 */

#ifndef C_SAW_SERVER_ASYNC_SERVING_H
#define C_SAW_SERVER_ASYNC_SERVING_H

#include <dc_utils/dlinked_list.h>
#include <semaphore.h>
#include "../server_config.h"

typedef union handler_id {
    pthread_t t;
    pid_t p;
} handler_id;

struct async_handler_args {
    handler_id id;
    int (*get_req_fd)( struct async_handler_args * handler_args);
//    int * free_threads; to be used in elastic pool expansion
    sem_t * concurrent_conn_sem;
    sem_t * req_avail_sem;
    sem_t * listening_pass_fd_sem;
    dlinked_list * req_queue;
    server_config_t * server_cfg;
};
typedef struct async_handler_args async_handler_args;

struct async_configs {
    int (*get_req_fd)( async_handler_args * handler_args );
    int * free_threads;
    sem_t * req_avail_sem;
    sem_t * concurrent_conn_sem;
    sem_t * listening_pass_fd_sem;
    dlinked_list * req_queue;
    server_config_t * server_cfg;
};
typedef struct async_configs async_configs;

typedef void*(*async_func_t)(void*);
typedef async_handler_args * (async_func_maker_t)( async_configs * async_cfg, async_func_t) ;

_Noreturn void serve( server_config_t * server_cfg, int listen_socket_fd );

#endif //C_SAW_SERVER_ASYNC_SERVING_H
