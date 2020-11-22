/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * handle_req.h
 */

#ifndef C_SAW_SERVER_HANDLE_REQ_H
#define C_SAW_SERVER_HANDLE_REQ_H

#include "http_req_parser.h"

struct http_res_t {
    char * body;
    char * status_line;
    char * date;
    char * server;
    char * content_type;
    char * allow;
    char * expires;
    char * content_length;
    char * last_modified;
};
typedef struct http_res_t http_res_t;

/**
 * Prepares a response for a given http request.
 * @param req the request information
 * @param res the un-encoded response including header and body
 * @return SUCCESS or an error code
 */
int handle_req(http_req_t * req, http_res_t * res);

#endif //C_SAW_SERVER_HANDLE_REQ_H
