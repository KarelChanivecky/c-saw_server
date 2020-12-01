/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_req_parser.h
 */

#ifndef C_SAW_SERVER_HTTP_REQ_PARSER_H
#define C_SAW_SERVER_HTTP_REQ_PARSER_H

#define DEFAULT_ABS_PATH /

struct http_req_t {
    char * request_type;
    char * method;
    char * request_URI;
    char * protocol_version;
    char * authorization;
    char * from;
    char * if_modified_since;
    char * referer;
    char * user_agent;
};
typedef struct http_req_t http_req_t;

/*
   difference between a Simple-Request and the Request-
   Line of a Full-Request is the presence of the HTTP-Version field and
   the availability of methods other than GET.
 */
int parse_http_req(http_req_t * req, char * req_string);
char **tokenize_string( char * line, const char *delim, int i);
char **dynamic_tokenize_req(char * req, int delimeter_len);

#endif //C_SAW_SERVER_HTTP_REQ_PARSER_H
