/**
 * @since 2020-11-23
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_process.c
 */
//
// Created by karel on 2020-11-23.
//

#include "async_process.h"

#include "logging.h"
#include "serve_request.h"
#include "../return_codes.h"
#include <dc_utils/dlinked_list.h>
#include <dc/stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "unistd.h"

async_handler_args * make_process( async_configs * async_cfg, async_func_t async_func) {
    async_handler_args * args = (async_handler_args*) dc_malloc( sizeof(async_handler_args));
    args->get_req_fd = async_cfg->get_req_fd;
    args->free_threads = async_cfg->free_threads;
    args->concurrent_conn_sem = async_cfg->concurrent_conn_sem;
    args->req_sem = async_cfg->req_sem;
    args->server_cfg = async_cfg->server_cfg;
    args->req_queue = NULL;
    args->id.p = fork();
    if (args->id.p == 0) {
        async_func(args);
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
int process_get_req_fd( int argc, ...) {
    va_list args;
    int socket;
    va_start(args, argc);
    socket = va_arg (args, int);
    int * fds = malloc (sizeof(int));
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = { .iov_base = &dup, .iov_len = sizeof(dup) };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);
    if (recvmsg (socket, &msg, 0) < 0) {
        perror("Failed to receive message");
        exit(EXIT_PROC_MSG);
    }
    cmsg = CMSG_FIRSTHDR(&msg);
    memcpy (fds, (int *) CMSG_DATA(cmsg), sizeof(int));
    return *fds;
}


void process_put_req_fd( int conn_fd, ...) {
    va_list args;
    int socket;

    va_start(args, conn_fd);
    socket = va_arg (args, int);

    struct msghdr msg = {0};
    struct cmsghdr * cmsg;
    char buf[CMSG_SPACE( sizeof(int))], dup[256];
    memset(buf, '\0', sizeof(buf));
    struct iovec io = { .iov_base = &dup, .iov_len = sizeof(dup) };

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    memcpy ((int *) CMSG_DATA(cmsg), &conn_fd, sizeof (int));

    if (sendmsg (socket, &msg, 0) < 0){
        perror("Failed to send message");
        exit(EXIT_PROC_MSG);
    }
}
