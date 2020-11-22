//
// Created by root on 2020-11-21.
//

#ifndef C_SAW_SERVER_HTTP_RES_TYPE_HPP
#define C_SAW_SERVER_HTTP_RES_TYPE_HPP

// Keep this macro updated
#define HTTP_RES_T_FIELD_COUNT 9
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

#endif //C_SAW_SERVER_HTTP_RES_TYPE_HPP
