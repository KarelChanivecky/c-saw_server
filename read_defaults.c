//
//  read_defaults.c
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_defaults.h"
#include "server_config.h"

void set_defaults(server_config_t * server_cfgs){
    server_cfgs->port = 80;
    server_cfgs->sin_family = 2;
    server_cfgs->addr = 0;
    server_cfgs->max_concurrent_conn = 10;
    server_cfgs->max_open_conn = 40;
    server_cfgs->read_buffer_size = 1024;
    server_cfgs->write_buffer_size = 1024;
    server_cfgs->page_expiration_time_mins = 5;
    server_cfgs->log_connections = 1;
    server_cfgs->concurrency_model = 1;
    server_cfgs->pooled = 1;
    server_cfgs->content_root_dir_path = "static_html/";
    server_cfgs->page_404_path = "static_html/404.html";
}
