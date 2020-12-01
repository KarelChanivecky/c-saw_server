//
// Created by root on 2020-11-17.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "logging.h"

void log_req( http_req_t * req ) {
    printf( "\nREQUEST:\n" );
    printf( "from: %s\n", req->from ? req->from : "NULL" );
    printf( "uri: %s\n", req->request_URI ? req->request_URI : "NULL" );
    printf( "type: %s\n", req->request_type ? req->request_type : "NULL" );
    printf( "method: %s\n", req->method ? req->method : "NULL" );
    printf( "protocol version: %s\n", req->protocol_version ? req->protocol_version : "NULL" );
    printf( "user agent: %s\n", req->user_agent ? req->user_agent : "NULL" );
    printf( "referer: %s\n", req->referer ? req->referer : "NULL" );
    printf( "\n" );
}

void log_res( http_res_t * res ) {
    printf( "\nRESPONSE:\n" );
    printf("status_line: %s\n", res->status_line ? res->status_line : "NULL");
    printf("date: %s\n", res->date ? res->date : "NULL");
    printf("server: %s\n", res->server ? res->server : "NULL");
    printf("content_type: %s\n", res->content_type ? res->content_type : "NULL");
    printf("allow: %s\n", res->allow ? res->allow : "NULL");
    printf("expires: %s\n", res->expires ? res->expires : "NULL");
    printf("content_length: %s\n", res->content_length ? res->content_length : "NULL");
    printf("last_modified: %s\n", res->last_modified ? res->last_modified : "NULL");
    printf("body: %s\n", res->body ? res->body : "NULL");
    printf( "\n" );
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
    printf( "\nREQUESTER:\nip: %s\nport: %d", client_ip, client.sin_port );
}

void log_action( int client_fd, http_req_t * req, http_res_t * res ) {
    log_requester( client_fd );
    log_req( req );
    log_res( res );
}
