//
//  read_config_file.c
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_config_file.h"
#include "server_config.h"
#include "string_allocation.h"
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

int read_confg_file(server_config_t * server_cfgs){
    config_t cfg;
    
    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if(!config_read_file(&cfg, "../example.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    char * root_dir_path;
    char * path_404;

    config_lookup_int( &cfg, "port", ( int * ) &( server_cfgs->port ));
    config_lookup_int(&cfg, "sin_family", (int *)&(server_cfgs->sin_family));
    config_lookup_int(&cfg, "addr", (int *)&(server_cfgs->addr));
    config_lookup_int(&cfg, "max_concurrent_conn", (int *)&(server_cfgs->max_concurrent_conn));
    config_lookup_int(&cfg, "max_open_conn", (int *)&(server_cfgs->max_open_conn));
    config_lookup_int(&cfg, "read_buffer_size", (int *)&(server_cfgs->read_buffer_size));
    config_lookup_int(&cfg, "write_buffer_size", (int *)&(server_cfgs->write_buffer_size));
    config_lookup_int(&cfg, "page_expiration_time_mins", (int *)&(server_cfgs->page_expiration_time_mins));
    config_lookup_bool(&cfg, "log_connections", (int *)&(server_cfgs->log_connections));
    config_lookup_bool(&cfg, "concurrency_model", (int *)&(server_cfgs->concurrency_model));
    config_lookup_bool(&cfg, "pooled", (int *)&(server_cfgs->pooled));
    config_lookup_string(&cfg, "content_root_dir_path", &root_dir_path);
    config_lookup_string(&cfg, "page_404_path", &path_404);

    server_cfgs->content_root_dir_path = alloc_string(root_dir_path);
    server_cfgs->page_404_path = alloc_string(path_404);

    config_destroy(&cfg);
    return EXIT_SUCCESS;
}

//int main(int argc, char** argv) {
//    struct server_config_t* server_cfgs = malloc(sizeof(server_config_t));
//    printf("Before getting configs\n");
//    read_confg_file(server_cfgs);
//    printf("After getting configs\n");
//
//    free(server_cfgs);
//    return EXIT_SUCCESS;
//}
