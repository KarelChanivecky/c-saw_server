//
// Created by root on 2020-11-17.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "logging.h"

void log_req( http_req_t req ) {
    printf( "REQUEST:\n" );
    printf( "from: %s\n", req.from ? req.from : "NULL" );
    printf( "uri: %s\n", req.request_URI ? req.request_URI : "NULL" );
    printf( "type: %s\n", req.request_type ? req.request_type : "NULL" );
    printf( "method: %s\n", req.method ? req.method : "NULL" );
    printf( "protocol version: %s\n", req.protocol_version ? req.protocol_version : "NULL" );
    printf( "user agent: %s\n", req.user_agent ? req.user_agent : "NULL" );
    printf( "referer: %s\n", req.referer ? req.referer : "NULL" );
    printf( "\n" );
}

void log_res( http_res_t res ) {
    printf( "RESPONSE:\n" );
    printf( "body: %s\n", res.body ? res.body : NULL);
}

void log_requester( int client_fd ) {
    struct sockaddr_in client;
    socklen_t addr_size = sizeof( client );
    int status = getpeername( client_fd, ( struct sockaddr * ) &client, &addr_size );
    if ( status == -1 ) {
        fprintf( stderr, "Logging error. Could not get peer info\n" );
        return;
    }
    char client_ip[20];
    strcpy( client_ip, inet_ntoa( client.sin_addr ));
    printf( "REQUESTER:\nip: %s\nport: %d", client_ip, client.sin_port );
}

void log_action( int client_fd, http_req_t req, http_res_t res ) {
    log_requester( client_fd );
    log_req( req );
    log_res( res );
}
