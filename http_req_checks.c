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

bool check_protocol( http_req_t * req ) {
    char * http_version = req->protocol_version;
    return strncmp( http_version, SUPPORTED_PROTOCOL, PROTOCOL_NAME_LEN ) == 0
           || strncmp( http_version, SUPPORTED_PROTOCOL_CAPS, PROTOCOL_NAME_LEN ) == 0;
}

int check_method( http_req_t * req ) {
    char * method = req->method;
    if ( strncmp( method, "GET", 3 ) == 0 ) {
        return METHOD_GET;
    }
    if ( strncmp( method, "HEAD", 3 ) == 0 ) {
        return METHOD_HEAD;
    }
    return METHOD_OTHER;
}


#define DATE_FORMAT_OTH_1 "%A, %d-%b-%y %T"
#define DATE_FORMAT_OTH_2 "%a %b  %d %T %Y"

#define HTTP_TIME 0
#define OTH_1_TIME 1
#define OTH_2_time 2

/*
 * HTTP_TIME represents Sun, 06 Nov 1994 08:49:37 GMT
 * OTH_1 represents Sunday, 06-Nov-94 08:49:37 GMT
 * OTH_2 represents Sun Nov  6 08:49:37 1994
 */
size_t get_type_of_date( char * date ) {
    date = delete_leading_whitespaces(date);
    size_t line_len = strcspn( date, "," );
    if ( line_len == 3 ) {
        return HTTP_TIME;
    }
    if (( line_len > 3 ) && ( line_len <= 10 )) {
        return OTH_1_TIME;
    }
    return OTH_2_time;
}



bool check_modified_since( http_req_t * req ) {
    if (!req->if_modified_since) {
        return true;
    }
    struct stat file_stat;
    stat( req->request_URI, &file_stat );

    struct tm since_time_tm;

    char * date_format;
    char* absolute_date = delete_leading_whitespaces(req->if_modified_since);
    switch ( get_type_of_date(req->if_modified_since)) {
        case HTTP_TIME:
            date_format = HTTP_TIME_FORMAT;
            break;
        case OTH_1_TIME:
            date_format = DATE_FORMAT_OTH_1;
            break;
        case OTH_2_time:
            date_format = DATE_FORMAT_OTH_2;
            break;
    }
    char * since_matched = strptime(absolute_date, date_format, &since_time_tm);
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
