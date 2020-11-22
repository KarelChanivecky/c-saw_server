/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */

#include <dc/stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <dc/unistd.h>
#include "http_res_field_builders.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "unistd.h"
#include "return_codes.h"

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
    char code_str[4];
    sprintf( code_str, "%d", code );

    strncat( status_line, HTTP_VERSION, strlen( HTTP_VERSION ) + 1);
    strncat( status_line, SP, 2 );
    strncat( status_line, code_str, 4 );
    strncat( status_line, SP, 2 );
    strncat( status_line, reason, strlen( reason ) + 1);
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
    strncat( last_modified_str, LAST_MODIFIED_FIELD_NAME, strlen( LAST_MODIFIED_FIELD_NAME ) + 1);
    strncat( last_modified_str, SEPARATOR, 3 );
    strncat( last_modified_str, time_str, time_len + 1 );
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
    strncat( content_length_str, CONTENT_LENGTH_FIELD_NAME, strlen( CONTENT_LENGTH_FIELD_NAME ) + 1);
    strncat( content_length_str, SEPARATOR, 3 );
    strncat( content_length_str, file_size, digits_in_size + 1);
    return content_length_str;
}

#define CONTENT_TYPE_FIELD_NAME "Content-Type"
#define FILE_COMMAND_CALL "file"
#define FILE_COMMAND_OPTIONS { "--mime-type", "-b", NULL, NULL }
#define MAX_CONTENT_TYPE_LENGTH 255

char * execute_file( int out, char * path ) {
    char * options[] = FILE_COMMAND_OPTIONS;
    options[2] = path;
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        dup2( out, STDOUT_FILENO );
        execvp( FILE_COMMAND_CALL, options );
        fprintf( stderr, "Execute error at make content-type: %s", strerror(errno));
        exit( EXIT_EXEC );
    }
    if ( child_pid == -1 ) {
        fprintf( stderr, "Fork error at make content-type: %s", strerror(errno));
        exit( EXIT_FORK );
    }
    int status;
    int wait_status = waitpid( child_pid, &status, 0 );
    if ( wait_status == -1 ) {
        fprintf( stderr, "Wait error at make content-type: %s", strerror(errno));
        exit( EXIT_WAITPID );
    }
    if ( !WIFEXITED( status )) {
        fprintf( stderr, "Child error at make content-type: %d", status );
        exit( EXIT_CHILD );
    }
}

char * get_content_type( int src) {
    char content_type_buf[MAX_CONTENT_TYPE_LENGTH];
    size_t bytes_read = dc_read( src, content_type_buf, MAX_CONTENT_TYPE_LENGTH );
    char * content_type_str = ( char * ) dc_malloc( sizeof( char ) * bytes_read + 1 );
    strncpy( content_type_str, content_type_buf, bytes_read );
    content_type_str[ bytes_read ] = 0;
    return content_type_str;

}

char * get_with_PIPE(char * path) {
    int fds[2];
    dc_pipe( fds );
    execute_file( fds[ 1 ], path);
    char * content_type = get_content_type( fds[ 0 ] );
    dc_close( fds[ 0 ] );
    dc_close( fds[ 1 ] );
    return content_type;
}


char * make_content_type( server_config_t server_cfg, char * path ) {
    char * content_type;
    size_t content_type_field_len = strlen(content_type) + strlen(CONTENT_TYPE_FIELD_NAME) + 2; // separator
    char * content_type_field = (char *) dc_malloc(sizeof(char ) * content_type_field_len + 1);
    strncat(content_type_field, CONTENT_TYPE_FIELD_NAME, strlen(CONTENT_TYPE_FIELD_NAME) + 1);
    strncat(content_type_field, SEPARATOR, 3);
    strncat(content_type_field, content_type, strlen(content_type) + 1);
    free(content_type);
    return content_type_field;
}

