/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */
#define _XOPEN_SOURCE



#include "http_res_field_builders.h"
#include "return_codes.h"
#include "http_req_parser.h"

#include <dc/stdlib.h>
#include <dc/unistd.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "unistd.h"
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include "ctype.h"


#define SP " "
#define SEPARATOR ": "


// make status field
#define HTTP_VERSION "HTTP/1.0"
#define REASON_200 "STATUS_OK"
#define REASON_304 "Not modified since cached time"
#define REASON_403 "Path points outside of public directory"
#define REASON_404 "Page not found"
#define REASON_405 "Only "
#define REASON_505 "Internal server error"
#define REASON_501 "Not Implemented"

struct http_date {
    int month;
    int date;
    int year;
    int hours;
    int minutes;
    int seconds;
};
typedef struct http_date http_date;

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
        case 501:
            reason = REASON_501;
            field_length += strlen( reason );
            break;
    }
    char * status_line = dc_malloc( sizeof( char ) * field_length );
    char code_str[4];
    sprintf( code_str, "%d", code );

    strncat( status_line, HTTP_VERSION, strlen( HTTP_VERSION ) + 1 );
    strncat( status_line, SP, 2 );
    strncat( status_line, code_str, 4 );
    strncat( status_line, SP, 2 );
    strncat( status_line, reason, strlen( reason ) + 1 );
    return status_line;
}



// make last modified
#define LAST_MODIFIED_FIELD_NAME "Last-Modified"
#define MAX_TIME_STR_LEN 30

char * make_last_modified( char * path ) {
    struct stat file_stat;
    stat( path, &file_stat );
    struct tm last_modified_tm;
    gmtime_r( &file_stat.st_mtime, &last_modified_tm );
    char time_str[MAX_TIME_STR_LEN];
    strftime( time_str, MAX_TIME_STR_LEN, HTTP_TIME_FORMAT, &last_modified_tm );
    size_t time_len = strlen(time_str);
    size_t last_modified_len = strlen( LAST_MODIFIED_FIELD_NAME ) + time_len + 3; // : SP and null byte
    char * last_modified_str = ( char * ) dc_malloc( last_modified_len );
    strncat( last_modified_str, LAST_MODIFIED_FIELD_NAME, strlen( LAST_MODIFIED_FIELD_NAME ) + 1 );
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
    strncat( content_length_str, CONTENT_LENGTH_FIELD_NAME, strlen( CONTENT_LENGTH_FIELD_NAME ) + 1 );
    strncat( content_length_str, SEPARATOR, 3 );
    strncat( content_length_str, file_size, digits_in_size + 1 );
    return content_length_str;
}

#define CONTENT_TYPE_FIELD_NAME "Content-Type"
#define FILE_COMMAND_CALL "file"
#define FILE_COMMAND_OPTIONS { "--mime-type", "-b", NULL, NULL }
#define MAX_CONTENT_TYPE_LENGTH 255

void execute_file( int out, char * path ) {
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

char * read_type( int src ) {
    char content_type_buf[MAX_CONTENT_TYPE_LENGTH];
    size_t bytes_read = dc_read( src, content_type_buf, MAX_CONTENT_TYPE_LENGTH );
    char * content_type_str = ( char * ) dc_malloc( sizeof( char ) * bytes_read + 1 );
    strncpy( content_type_str, content_type_buf, bytes_read );
    content_type_str[bytes_read] = 0;
    return content_type_str;

}

char * get_content_type( char * path ) {
    int fds[2];
    dc_pipe( fds );
    execute_file( fds[1], path );
    char * content_type = read_type( fds[0] );
    dc_close( fds[0] );
    dc_close( fds[1] );
    return content_type;
}

void r_strip(char * str) {
    size_t len = strlen(str);
    str+= len - 1;
    while (*str == '\n' || *str == '\r') {
        *str =0;
    }
}

char * make_content_type( char * path ) {
    char * content_type = get_content_type( path );
    size_t content_type_field_len = strlen( content_type ) + strlen( CONTENT_TYPE_FIELD_NAME ) + 2; // separator
    char * content_type_field = ( char * ) dc_malloc( sizeof( char ) * content_type_field_len + 1 );
    strncat( content_type_field, CONTENT_TYPE_FIELD_NAME, strlen( CONTENT_TYPE_FIELD_NAME ) + 1 );
    strncat( content_type_field, SEPARATOR, 3 );
    strncat( content_type_field, content_type, strlen( content_type ) + 1 );
    free( content_type );
    r_strip(content_type_field);
    return content_type_field;
}

#define DATE_BLOCK 35
#define SIMPLE_REQUEST "Simple-Request"


char * delete_leading_whitespaces( char * line ) {
    while ( isspace(( unsigned char ) *line )) {
        line++;
    }
    return line;
}

/**
 * This function is only responsible for checking if the path requested in http request goes beyond the scope of root directory.
 * @param path
 * @return
 */
bool path_in_bounds( char * path ) {

    size_t len = strlen( path );
    int forward_counter = *( path + len - 1 ) == URI_PATH_DELIMITER ? 1 : 0;
    int back_counter = 0;

    char buf[len + 1];
    strncpy( buf, path, len );
    buf[len] = 0;
    char ** args = tokenize_string( buf, "/\0", DEFAULT_INITIAL_TOKEN_ALLOC_COUNT );
    int i = 0;
    while ( args[i] ) {
        if ( !strcmp( args[i], ".." )) {
            back_counter++;
        } else {
            forward_counter++;
        }
        free( args[i] );
        i++;
    }
    free( args );
    return back_counter <= forward_counter;
}

bool is_simple_req( char * request_type ) {
    char * buffer = delete_leading_whitespaces( request_type );
    return strcmp( buffer, SIMPLE_REQUEST ) == 0;
}

char * prepare_entity_body( char * path, size_t * body_len ) {
    if ( !path ) {
        return NULL;
    }
    int src_fd = open( path, O_RDONLY );
    if (src_fd == -1 ) {
        if (errno == ENOENT
        || errno == EACCES
        || errno == EAGAIN
        || errno == ENOMEM
        || errno == EOVERFLOW
        || errno == ENOTDIR) {
            return NULL;
        }
    }
    return read_fd( src_fd, body_len );

}

#define TIME_FIELD_NAME "Date: "

char * get_date() {
    char buf[DATE_BLOCK];
    time_t now = time( 0 );
    struct tm tm = *gmtime( &now );
    size_t value_len = strftime( buf, sizeof buf, HTTP_TIME_FORMAT, &tm );
    size_t name_len = strlen( TIME_FIELD_NAME );
    char * date = dc_malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( date, TIME_FIELD_NAME, name_len );
    strncat( date, buf, value_len );
    return date;
}

#define SERVER_FIELD_NAME "Server: "
#define SERVER_NAME "C-Saw"

char * get_server() {
    size_t name_len = strlen( SERVER_FIELD_NAME );
    size_t value_len = strlen( SERVER_NAME );
    char * server = dc_malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( server, SERVER_FIELD_NAME, name_len );
    strncat( server, SERVER_NAME, value_len );
    return server;
}

#define ALLOWED_METHODS "GET/HEAD"
#define ALLOWED_METHODS_FIELD_NAME "Allow: "

char * get_allow() {
    size_t len_field_name = strlen( ALLOWED_METHODS_FIELD_NAME );
    size_t len_field_value = strlen( ALLOWED_METHODS );
    char * allow = dc_malloc( sizeof( char ) * ( len_field_name + len_field_value + 3 ));
    strncpy( allow, ALLOWED_METHODS_FIELD_NAME, len_field_name );
    strncat( allow, ALLOWED_METHODS, len_field_value );
    return allow;
}

#define EXPIRE_FIELD_NAME "Expires: "

char * get_expires( size_t minutes ) {
    if ( minutes == 0 ) {
        return NULL;
    }
    char buf[30];
    time_t now = time( 0 );

    struct tm expire = *gmtime( &now );
    expire.tm_min += minutes;

    mktime( &expire );

    size_t name_len = strlen( EXPIRE_FIELD_NAME );
    size_t value_len = strftime( buf, sizeof buf, HTTP_TIME_FORMAT, &expire );
    char * expiry_date = malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( expiry_date, EXPIRE_FIELD_NAME, name_len );
    strncat( expiry_date, buf, value_len );
    return expiry_date;
}

bool file_exists( char * path ) {
    struct stat st;
    if (stat( path, &st ) == -1) {
        if (errno == ENOENT
                || errno == EACCES
                || errno == EAGAIN
                || errno == ENOMEM
                || errno == EOVERFLOW
                || errno == ENOTDIR) {
            return false;
        }
        fprintf(stderr, "Error checking existence of file: %s", strerror(errno));
        exit(ERRNO_ERROR);
    }

    return S_ISREG(st.st_mode);
}

void prepare_common_headers( http_res_t * res ) {
    res->server = get_server();
    res->date = get_date();
    res->allow = get_allow();
}

