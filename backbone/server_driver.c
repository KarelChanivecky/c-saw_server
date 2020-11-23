/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * server_driver.c
 */


#include "server_driver.h"

// changing this include should change the behaviour of the serve method
//#include "async_serving_v1.h"
#include "async_serving_v2.h"


#include "../handle_req.h"

#include <dc/sys/socket.h>
#include "logging.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

int get_socket( server_config_t * server_cfg ) {
    int listen_socket_fd;
    struct sockaddr_in addr;
    listen_socket_fd = dc_socket( server_cfg->sin_family, SOCK_STREAM, 0 );

    int option = 1;
    int sock_opt_status = setsockopt( listen_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                      ( char * ) &option, sizeof( option ));
    if ( sock_opt_status == -1 ) {
        fprintf( stderr, "Error setting socket options!: %s", strerror(errno));
        exit( EXIT_FAILURE );
    }

    memset( &addr, 0, sizeof( struct sockaddr_in ));
    addr.sin_family = server_cfg->sin_family;
    addr.sin_port = htons( server_cfg->port );
    addr.sin_addr.s_addr = htonl( server_cfg->addr );
    dc_bind( listen_socket_fd, ( struct sockaddr * ) &addr, sizeof( struct sockaddr_in ));
    return listen_socket_fd;
}

void start_server( server_config_t * server_cfg ) {
    printf( "Opening listener socket\n" );
    int listen_socket_fd = get_socket( server_cfg );
    printf( "Listener socket opened\n" );
    dc_listen( listen_socket_fd, server_cfg->max_open_conn );
    printf( "Listening....\n" );
    serve(server_cfg, listen_socket_fd);
}

