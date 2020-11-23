//
//  read_config_file.c
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_config_file.h"
#include "server_config.h"
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

    config_lookup_int(&cfg, "port", &(server_cfgs->port));
    config_lookup_int(&cfg, "sin_family", &(server_cfgs->sin_family));
    config_lookup_int(&cfg, "addr", &(server_cfgs->addr));
    config_lookup_int(&cfg, "max_concurrent_conn", &(server_cfgs->max_concurrent_conn));
    config_lookup_int(&cfg, "max_open_conn", &(server_cfgs->max_open_conn));
    config_lookup_int(&cfg, "read_buffer_size", &(server_cfgs->read_buffer_size));
    config_lookup_int(&cfg, "write_buffer_size", &(server_cfgs->write_buffer_size));
    config_lookup_bool(&cfg, "log_connections", &(server_cfgs->log_connections));
    config_lookup_bool(&cfg, "concurrency_model", &(server_cfgs->concurrency_model));
    config_lookup_string(&cfg, "content_root_dir_path", &(server_cfgs->content_root_dir_path));
    config_lookup_string(&cfg, "page_404_path", &(server_cfgs->page_404_path));
    config_lookup_string(&cfg, "expiration_time", &(server_cfgs->expiration));
    config_lookup_string(&cfg, "pooled", &(server_cfgs->pooled));

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
