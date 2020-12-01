//
//  config.c
//  
//
//  Created by Gurden Angi on 2020-11-07.
//

#include "config.h"
#include "read_args.h"
#include "read_config_file.h"
#include "read_defaults.h"
#include "server_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

int get_config( config_t* cfg, server_config_t * server_cfgs, int argc, char ** argv ) {
    set_defaults( server_cfgs );
    
    read_confg_file( cfg, server_cfgs );

    char ** args = get_args( argc, argv );
    for ( int i = 0; i < 13; i++ )
        if ( args[i] != NULL) {
            switch ( i ) {
                case 0:
                    server_cfgs->port = atoi( args[i] );
                    break;
                case 1:
                    server_cfgs->sin_family = atoi( args[i] );
                    break;
                case 2:
                    server_cfgs->addr = atoi( args[i] );
                    break;
                case 3:
                    server_cfgs->max_concurrent_conn = atoi( args[i] );
                    break;
                case 4:
                    server_cfgs->max_open_conn = atoi( args[i] );
                    break;
                case 5:
                    server_cfgs->read_buffer_size = atoi( args[i] );
                    break;
                case 6:
                    server_cfgs->log_connections = atoi( args[i] );
                    break;
                case 7:
                    server_cfgs->concurrency_model = atoi( args[i] );
                    break;
                case 8:
                    server_cfgs->content_root_dir_path = args[i];
                    break;
                case 9:
                    server_cfgs->page_404_path = args[i];
                    break;
                case 10:
                    server_cfgs->write_buffer_size = atoi( args[i] );
                    break;
                case 11:
                    server_cfgs->page_expiration_time_mins = atoi( args[i] );
                    break;
                case 12:
                    server_cfgs->pooled = atoi( args[i] );
                    break;

                default:
                    break;
            }
        }
    
    int dir_path_length = strlen(server_cfgs->content_root_dir_path);
    char c = (server_cfgs->content_root_dir_path)[dir_path_length-1];
    if (c != '/') {
        char* path = malloc((dir_path_length + 2)*sizeof(char));
        strcpy(path, server_cfgs->content_root_dir_path);
        path[dir_path_length] = '/';
        path[dir_path_length+1] = '\0';
        strcpy(server_cfgs->content_root_dir_path, path);
        free(path);
    }
    free( args );
    
    return EXIT_SUCCESS;
}


int main(int argc, char** argv) {
    struct server_config_t* server_cfgs = malloc(sizeof(server_config_t));
    config_t cfg;
    get_config(&cfg, server_cfgs, argc, argv);
    printf("%d\n", server_cfgs->port);
    printf("%d\n", server_cfgs->sin_family);
    printf("%d\n", server_cfgs->addr);
    printf("%d\n", server_cfgs->max_concurrent_conn);
    printf("%d\n", server_cfgs->max_open_conn);
    printf("%d\n", server_cfgs->read_buffer_size);
    printf("%d\n", server_cfgs->write_buffer_size);
    printf("%d\n", server_cfgs->page_expiration_time_mins);
    printf("%d\n", server_cfgs->log_connections);
    printf("%d\n", server_cfgs->pooled);
    printf("%d\n", server_cfgs->concurrency_model);
    printf("%s\n", server_cfgs->content_root_dir_path);
    printf("%s\n", server_cfgs->page_404_path);
    free(server_cfgs);
    config_destroy(&cfg);
    return EXIT_SUCCESS;
}
