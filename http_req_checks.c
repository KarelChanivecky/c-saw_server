/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.c
 */

// required for strptime
#define _XOPEN_SOURCE

#include <sys/stat.h>
#include "stdio.h"
#include "time.h"
#include "http_req_checks.h"
#include "string.h"
#include "http_res_field_builders.h"
#include "return_codes.h"


#define PROTOCOL_NAME_LEN 4

bool check_protocol( http_req_t req ) {
    char * http_version = req.protocol_version;
    return strncmp( http_version, SUPPORTED_PROTOCOL, PROTOCOL_NAME_LEN ) == 0
           || strncmp( http_version, SUPPORTED_PROTOCOL_CAPS, PROTOCOL_NAME_LEN ) == 0;
}

int check_method( http_req_t req ) {
    char * method = req.method;
    if ( strncmp( method, "GET", 3 ) == 0 ) {
        return METHOD_GET;
    }
    if ( strncmp( method, "HEAD", 3 ) == 0 ) {
        return METHOD_HEAD;
    }
    return METHOD_OTHER;
}

bool check_modified_since( http_req_t req ) {
    struct stat file_stat;
    stat( req.request_URI, &file_stat );

    struct tm since_time_tm;
    char * since_matched = strptime(req.if_modified_since, HTTP_TIME_FORMAT, &since_time_tm);
    if (since_matched == NULL) {
        fprintf( stderr, "Time parsing error at check_modified_since\n");
        exit(EXIT_TIME);
    }
    time_t since_time = mktime(&since_time_tm);

    if (since_time < file_stat.st_mtime) {
        return true;
    }
    return false;
}
