/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * server_config.h
 */

#ifndef C_SAW_SERVER_SERVER_CONFIG_H
#define C_SAW_SERVER_SERVER_CONFIG_H

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

// possible concurrency models
#define FORK true
#define THREAD false
#define PIPE 0
#define TEMP_FILE 1

struct server_config_t {
    uint16_t port;
    uint32_t sin_family; // TODO AF_INET or AF_UNIX
    uint32_t addr; // TODO INADDR_ANY in darcy's
    uint32_t max_concurrent_conn;
    uint32_t max_open_conn;
    size_t read_buffer_size;
    size_t write_buffer_size;
    int IPC_method;
    bool log_connections;
    bool pooled;
    char * content_root_dir_path;
    char * page_404_path;
    bool concurrency_model;
    size_t page_expiration_time_mins;
};

typedef struct server_config_t server_config_t;

//int get_config(server_config_t * s_cfg, int argc, char ** argv);

#endif //C_SAW_SERVER_SERVER_CONFIG_H
