/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */

#include "http_res_type.h"
#include "stddef.h"

void get_http_res_offsets(size_t offsets[]) {
    offsets[0] = offsetof(http_res_t, status_line);
    offsets[1] = offsetof(http_res_t, date);
    offsets[2] = offsetof(http_res_t, server);
    offsets[3] = offsetof(http_res_t, content_type);
    offsets[4] = offsetof(http_res_t, allow);
    offsets[5] = offsetof(http_res_t, expires);
    offsets[6] = offsetof(http_res_t, content_length);
    offsets[7] = offsetof(http_res_t, last_modified);
    offsets[8] = offsetof(http_res_t, body);
}

