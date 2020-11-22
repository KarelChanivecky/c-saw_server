/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.c
 */

#ifndef C_SAW_SERVER_HTTP_REQ_CHECKS_H
#define C_SAW_SERVER_HTTP_REQ_CHECKS_H


#include "stdbool.h"
#include "http_req_parser.h"

#define SUPPORTED_PROTOCOL "http"
#define SUPPORTED_PROTOCOL_CAPS "HTTP"
#define METHOD_GET 1
#define METHOD_HEAD 0
#define METHOD_OTHER -1

bool check_protocol( http_req_t req );

int check_method( http_req_t req );

bool check_modified_since( http_req_t req );


#endif //C_SAW_SERVER_HTTP_REQ_CHECKS_H
