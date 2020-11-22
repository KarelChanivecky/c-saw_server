//
// Created by root on 2020-11-21.
//

#ifndef C_SAW_SERVER_HTTP_RES_TYPE_H
#define C_SAW_SERVER_HTTP_RES_TYPE_H

#include "../../../../../../usr/include/glob.h"
#include "stdlib.h"

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

/**
 * Gets the offsets for each field in struct http_res_t
 *
 * Guaranteed that index 0 contains the offset for status line
 *
 * Guaranteed that the last index contains the offset for the body
 *
 * @param offsets must be sized to the value of HTTP_RES_T_FIELD_COUNT
 */
void get_http_res_offsets(size_t offsets[]);

#endif //C_SAW_SERVER_HTTP_RES_TYPE_H
