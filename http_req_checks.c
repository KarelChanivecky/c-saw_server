/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.c
 */

#include "http_req_checks.h"
#include "string.h"


#define PROTOCOL_NAME_LEN 4

bool check_protocol( http_req_t req) {
    char * http_version = req.protocol_version;
    return strncmp(http_version, SUPPORTED_PROTOCOL, PROTOCOL_NAME_LEN) == 0
    || strncmp(http_version, SUPPORTED_PROTOCOL_CAPS, PROTOCOL_NAME_LEN) == 0;
}

int check_method(http_req_t req) {
    char * method = req.method;
    if (strncmp(method, "GET", 3) == 0) {
        return METHOD_GET;
    }
    if (strncmp(method, "HEAD", 3) == 0) {
        return METHOD_HEAD;
    }
    return METHOD_OTHER;
}
