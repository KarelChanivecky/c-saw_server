/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */

#include <dc/stdlib.h>
#include <sys/stat.h>
#include "http_res_field_builders.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"

#define SP " "
#define SEPARATOR ": "

// make status field
#define HTTP_VERSION "HTTP/1.0"
#define REASON_200 "OK"
#define REASON_304 "Not modified since cached time"
#define REASON_403 "Path points outside of public directory"
#define REASON_404 "Page not found"
#define REASON_405 "Only "
#define REASON_505 "Internal server error"

char * make_status_field( int code ) {
    size_t field_length = strlen( HTTP_VERSION ) + 8; // 2 spaces, crlf, code, terminating null char
    char * reason;
    switch ( code ) {
        case 200:
            reason = REASON_200;
            field_length += strlen( reason );
            break;
        case 304:
            reason = REASON_304;
            field_length += strlen( reason );
            break;
        case 403:
            reason = REASON_403;
            field_length += strlen( reason );
            break;
        case 404:
            reason = REASON_404;
            field_length += strlen( reason );
            break;
        case 405:
            reason = REASON_405;
            field_length += strlen( reason );
            break;
        case 505:
            reason = REASON_505;
            field_length += strlen( reason );
            break;
    }
    char * status_line = dc_malloc( sizeof( char ) * field_length );
    strncat( status_line, HTTP_VERSION, strlen( HTTP_VERSION ));
    strncat( status_line, SP, 1 );
    char code_str[4];
    sprintf( code_str, "%d", code );
    strncat( status_line, code_str, 3 );
    strncat( status_line, SP, 1 );
    strncat( status_line, reason, strlen( reason ));
    status_line[field_length - 1] = 0;
    return status_line;
}



// make last modified
#define LAST_MODIFIED_FIELD_NAME "Last-Modified"
#define MAX_TIME_STR_LEN 30

char * make_last_modified( char * path ) {
    struct stat file_stat;
    stat( path, &file_stat );
    struct tm last_modified_tm;
    gmtime_r( &file_stat.st_mtim.tv_sec, &last_modified_tm );
    char time_str[MAX_TIME_STR_LEN];
    size_t time_len = strftime( time_str, MAX_TIME_STR_LEN, HTTP_TIME_FORMAT, &last_modified_tm );
    size_t last_modified_len = strlen( LAST_MODIFIED_FIELD_NAME ) + time_len + 3; // : SP and null byte
    char * last_modified_str = ( char * ) dc_malloc( last_modified_len );
    strncat( last_modified_str, LAST_MODIFIED_FIELD_NAME, strlen( LAST_MODIFIED_FIELD_NAME ));
    strncat( last_modified_str, SEPARATOR, 2 );
    strncat( last_modified_str, time_str, time_len );
    last_modified_str[last_modified_len - 1] = 0;
    return last_modified_str;
}

#define CONTENT_LENGTH_FIELD_NAME "Content-Length"

char * make_content_length( char * path ) {
    struct stat file_stat;
    stat( path, &file_stat );
    int digits_in_size = ( int ) floor( log10( file_stat.st_size )) + 1;
    char file_size[digits_in_size + 1];
    sprintf( file_size, "%ld", file_stat.st_size );
    size_t content_length_str_len =
            strlen( CONTENT_LENGTH_FIELD_NAME ) + digits_in_size + 3; // separator, null byte
    char * content_length_str = ( char * ) dc_malloc( sizeof( char ) * content_length_str_len );
    strncat(content_length_str, CONTENT_LENGTH_FIELD_NAME, strlen(CONTENT_LENGTH_FIELD_NAME));
    strncat(content_length_str, SEPARATOR, 2);
    strncat(content_length_str, file_size, digits_in_size );
    content_length_str[content_length_str_len - 1] = 0;
    return content_length_str;
}
