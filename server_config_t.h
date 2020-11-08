/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * server_config_t.h
 */

#ifndef C_SAW_SERVER_SERVER_CONFIG_T_H
#define C_SAW_SERVER_SERVER_CONFIG_T_H

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

#define FORK true
#define THREAD false

struct server_config_t {
    uint32_t port;
    uint32_t sin_family; // TODO AF_INET or AF_UNIX
    uint32_t addr; // TODO INADDR_ANY in darcy's
    uint32_t max_concurrent_conn;
    size_t read_buffer_size;
    bool log_connections;
    char * content_root_dir_path;
    char * page_404_path;
    bool concurrency_model;
};
typedef struct server_config_t server_config_t;

#endif //C_SAW_SERVER_SERVER_CONFIG_T_H
