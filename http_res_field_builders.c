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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include "http_res_encoder.h"
#include "ctype.h"
#include "http_req_parser.h"


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
#define REASON_501 "Not Implemented"

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

char * read_type( int src) {
    char content_type_buf[MAX_CONTENT_TYPE_LENGTH];
    size_t bytes_read = dc_read( src, content_type_buf, MAX_CONTENT_TYPE_LENGTH );
    char * content_type_str = ( char * ) dc_malloc( sizeof( char ) * bytes_read + 1 );
    strncpy( content_type_str, content_type_buf, bytes_read );
    content_type_str[ bytes_read ] = 0;
    return content_type_str;

}

char * get_content_type( char * path) {
    int fds[2];
    dc_pipe( fds );
    execute_file( fds[ 1 ], path);
    char * content_type = read_type( fds[ 0 ] );
    dc_close( fds[ 0 ] );
    dc_close( fds[ 1 ] );
    return content_type;
}


char * make_content_type( char * path ) {
    char * content_type = get_content_type(path);
    size_t content_type_field_len = strlen(content_type) + strlen(CONTENT_TYPE_FIELD_NAME) + 2; // separator
    char * content_type_field = (char *) dc_malloc(sizeof(char ) * content_type_field_len + 1);
    strncat(content_type_field, CONTENT_TYPE_FIELD_NAME, strlen(CONTENT_TYPE_FIELD_NAME) + 1);
    strncat(content_type_field, SEPARATOR, 3);
    strncat(content_type_field, content_type, strlen(content_type) + 1);
    free(content_type);
    return content_type_field;
}

#define BLOCK 17
#define DATE_BLOCK 35
#define MINUTES_IN_HOUR 60
#define HOUR_IN_A_DAY 60
#define SIMPLE_REQUEST "Simple-Request"
#define SERVER_NAME "C-Saw"
#define ALLOWED_METHODS "GET/HEAD"
#define BYTES_PER_CYCLE 1u



char * delete_leading_whitespaces(char * line){
    while(isspace((unsigned char)*line))
        line++;
    return line;
}

bool is_valid_path(char * path){
    int forward_counter = 0;
    int back_counter = 0;
    char **args = tokenize_string(path, "/\0", BLOCK);
    int i = 0;
    while(args[i]){
        if(!strcmp(args[i], ".."))
            back_counter++;
        else
            forward_counter++;
        i++;
    }
    free(args);
    if(back_counter > forward_counter)
        return false;
    else
        return true;

}

bool is_simple_req(char * request_type){
    char * buffer = delete_leading_whitespaces(request_type);

    if(strcmp(buffer, SIMPLE_REQUEST))
        return true;
    else
        return false;
}

void prepare_entity_body(char * path, http_res_t * res){
    int src_fd = open(path, O_RDONLY);
    unsigned int counter = 0;
    uint8_t temp = 0;

    size_t c_read;
    c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
    while(c_read > 0) {
        counter++;
        c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
    }

    char * body = malloc(sizeof(char) * (counter + 1));
    close(src_fd);
    src_fd = open(path, O_RDONLY);

    temp = 0;
    c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
    while(c_read > 0) {
        strncat(body, &temp, 1);
        c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
    }

    body = strcat(body, "\0");
    strncat(body, "\0", 1);
    res->body = body;
    free(body);
}

void set_time(http_res_t * res){
    char buf[DATE_BLOCK];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    int len = strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char * date = dc_malloc(sizeof(char) * (len+ 2));
    strcpy(date, buf);
    strcat(date, "\r\n");
    res->date = date;

    free(date);
}

void set_server(http_res_t * res){

    int len = strlen(SERVER_NAME);
    char * server = dc_malloc(sizeof(char) * (len+ 2));
    strcpy(server, SERVER_NAME);
    strcat(server,"\r\n");

    res->server = server;
    free(server);
}

void set_allow(http_res_t * res){
    int len = strlen(ALLOWED_METHODS);
    char * allow = dc_malloc(sizeof(char) * (len+ 2));
    strcpy(allow, ALLOWED_METHODS);
    allow = strcat(ALLOWED_METHODS,"\r\n");

    res->allow = allow;

    free(allow);
}

void set_expires(http_res_t * res, double min){
    char buf[30];
    time_t now = time(0);

    struct tm expire = *gmtime(&now);
    expire.tm_min += 6594265;

    mktime( &expire);

    int len = strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &expire);
    char * expiry_date = malloc(sizeof(char) * (len+ 2));
    strcpy(expiry_date, buf);
    strcat(expiry_date, "\r\n");
    res->expires = expiry_date;
//    printf("Expiry Time is: [%s]\n", expiry_date);
    free(expiry_date);
}

bool file_exists(char * path){
    return true;
}

bool file_modified_after_requested_if_moddified_date(char * path, char * if_mod_date){

}

bool prepare_common_headers(http_res_t * res){
    set_server(&res);
    set_time(&res);
    set_allow(&res);
    set_expires(&res, 900); //TODO get the second parameter from config.
}

