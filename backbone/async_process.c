/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_process.c
 */


#include "async_process.h"

#include "logging.h"
#include "serve_request.h"
#include "../return_codes.h"
#include <dc_utils/dlinked_list.h>
#include <dc/stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <dc/unistd.h>
#include <errno.h>
#include <dc/semaphore.h>
#include <dc/sys/socket.h>
#include "unistd.h"


#define FD_PASS_SOCKET_NAME "/tmp/fd-pass.socket01vxdf"


async_handler_args * make_process( async_configs * async_cfg, async_func_t async_func ) {
    async_handler_args * args = ( async_handler_args * ) dc_malloc( sizeof( async_handler_args ));
    args->get_req_fd = async_cfg->get_req_fd;
//    args->free_threads = async_cfg->free_threads;
    args->concurrent_conn_sem = async_cfg->concurrent_conn_sem;
    args->req_avail_sem = async_cfg->req_avail_sem;
    args->server_cfg = async_cfg->server_cfg;
    args->req_queue = NULL;
    args->id.p = fork();
    args->listening_pass_fd_sem = async_cfg->listening_pass_fd_sem;
    if ( args->id.p == 0 ) {
        async_func( args );
    }
    return args;
}

/*
 * CREDITS
 *
 * Anoop C S
 * August 7, 2016
 * https://openforums.wordpress.com/2016/08/07/open-file-descriptor-passing-over-unix-domain-sockets/
 */
int process_get_req_fd( async_handler_args * handler_args ) {

    struct sockaddr_un addr;
    int sfd = dc_socket( AF_UNIX, SOCK_STREAM, 0 );

    sem_wait( handler_args->req_avail_sem );

    if ( unlink( FD_PASS_SOCKET_NAME ) == -1 && errno != ENOENT ) {
        perror( "Removing socket file failed" );
        exit( EXIT_SOCK_UNLINK );
    }
    memset( &addr, 0, sizeof( struct sockaddr_un ));
    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, FD_PASS_SOCKET_NAME, sizeof( addr.sun_path ) - 1 );
    dc_bind( sfd, ( struct sockaddr * ) &addr, sizeof( struct sockaddr_un ));
    dc_listen( sfd, 5 );


    dc_sem_post( handler_args->listening_pass_fd_sem );

    int cfd = dc_accept( sfd, NULL, NULL);


    int * fds = malloc( sizeof( int ));
    struct msghdr msg = { 0 };
    struct cmsghdr * cmsg;
    char buf[CMSG_SPACE( sizeof( int ))], dup[256];
    memset( buf, '\0', sizeof( buf ));
    struct iovec io = { .iov_base = &dup, .iov_len = sizeof( dup ) };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof( buf );
    if ( recvmsg( cfd, &msg, 0 ) < 0 ) {
        perror( "Failed to receive message" );
        exit( EXIT_PROC_MSG );
    }
    cmsg = CMSG_FIRSTHDR( &msg );
    memcpy( fds, ( int * ) CMSG_DATA( cmsg ), sizeof( int ));
    dc_close( cfd );
    dc_close( sfd );

    int client_fd = *fds;
    free(fds);
    return client_fd;
}

/*
 * CREDITS
 *
 * Anoop C S
 * August 7, 2016
 * https://openforums.wordpress.com/2016/08/07/open-file-descriptor-passing-over-unix-domain-sockets/
 */
void process_put_req_fd( int req_fd, async_configs * async_cfg ) {
    sem_post( async_cfg->req_avail_sem );

    struct sockaddr_un addr;
    int sfd = dc_socket( AF_UNIX, SOCK_STREAM, 0 );
    memset( &addr, 0, sizeof( struct sockaddr_un ));
    addr.sun_family = AF_UNIX;
    strncpy( addr.sun_path, FD_PASS_SOCKET_NAME, sizeof( addr.sun_path ) - 1 );

    dc_sem_wait( async_cfg->listening_pass_fd_sem );

    dc_connect( sfd, ( struct sockaddr * ) &addr, sizeof( struct sockaddr_un ));

    struct msghdr msg = { 0 };
    struct cmsghdr * cmsg;
    char buf[CMSG_SPACE( sizeof( int ))], dup[256];
    memset( buf, '\0', sizeof( buf ));
    struct iovec io = { .iov_base = &dup, .iov_len = sizeof( dup ) };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof( buf );

    cmsg = CMSG_FIRSTHDR( &msg );
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN( sizeof( int ));

    memcpy(( int * ) CMSG_DATA( cmsg ), &req_fd, sizeof( int ));

    if ( sendmsg( sfd, &msg, 0 ) < 0 ) {
        perror( "Failed to send message" );
        exit( EXIT_PROC_MSG );
    }
    dc_close( sfd );
    dc_close( req_fd );
}
