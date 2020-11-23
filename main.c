#include "server_config.h"
#include "backbone/server_driver.h"
#include "stdio.h"
#include "config.h"

int main( int argc, char ** argv ) {
    printf("C-SAW /. SERVER\n");
    printf("Loading configs\n");
    server_config_t server_cfg;
    get_config(&server_cfg, argc, argv);
    printf("Configs loaded. Starting server.\n");
    start_server(&server_cfg);
}
