//
//  read_defaults.c
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_defaults.h"
#include "server_config.h"
#include <stdio.h>
#include <stdlib.h>

void set_defaults(server_config_t * server_cfgs){
    server_cfgs->port = 8080;
    server_cfgs->sin_family = 30;
    server_cfgs->addr = 2020;
    server_cfgs->max_concurrent_conn = 10;
    server_cfgs->max_open_conn = 40;
    server_cfgs->log_connections = 1;
    server_cfgs->concurrency_model = 1;
    server_cfgs->content_root_dir_path = "c-saw-server.git";
    server_cfgs->page_404_path = "myerrorpage.html";
    
}
