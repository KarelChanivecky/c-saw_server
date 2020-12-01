//
// Created by root on 2020-11-17.
//

#include "../http_req_parser.h"
#include "../handle_req.h"

#ifndef C_SAW_SERVER_LOGGING_H
#define C_SAW_SERVER_LOGGING_H

#endif //C_SAW_SERVER_LOGGING_H

void log_action( int client_fd, http_req_t * req, http_res_t * res );

void log_requester( int client_fd );

void log_req( http_req_t * req );

void log_res( http_res_t * res );

