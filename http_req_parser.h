/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_req_parser.h
 */

#ifndef C_SAW_SERVER_HTTP_REQ_PARSER_H
#define C_SAW_SERVER_HTTP_REQ_PARSER_H

struct http_req_t {

};
typedef struct http_req_t http_req_t;

int parse_http_req(http_req_t * req, char * req_string);

#endif //C_SAW_SERVER_HTTP_REQ_PARSER_H
