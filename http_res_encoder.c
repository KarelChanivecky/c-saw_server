/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_res_encoder.c
 */


#include <dc/stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <fcntl.h>
#include "http_res_encoder.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "stdio.h"
#include "http_req_parser.h"

#define BLOCK 17
#define SIMPLE_REQUEST "Simple-Request"
#define SERVER_NAME "C-Saw"
#define ALLOWED_METHODS "GET/HEAD"
#define BYTES_PER_CYCLE 1u




char * get_nth_element(http_res_t * res, int n) {
    return (char*)(res + n * sizeof(char *));
}

char * http_res_encode(http_res_t * res) {
    if (!res) {
        return NULL;
    }
    size_t bytes_to_allocate = 0;
    for (int i = 0; i < HTTP_RES_T_FIELD_COUNT; i++) {
        bytes_to_allocate += strlen(get_nth_element(res, i));
    }
    char * res_string = dc_malloc(sizeof(char ) * bytes_to_allocate + 1);

    strncat(res_string, res->status_line, strlen(res->status_line));
    strncat(res_string, res->date, strlen(res->date));
    strncat(res_string, res->server, strlen(res->server));
    strncat(res_string, res->, strlen(res->));
    strncat(res_string, res->, strlen(res->));
    strncat(res_string, res->, strlen(res->));
    strncat(res_string, res->, strlen(res->));
    strncat(res_string, res->, strlen(res->));
    strncat(res_string, res->, strlen(res->));
    return NULL;
}

char * delete_leading_whitespaces(char * line){
    while(isspace((unsigned char)*line))
        line++;
    return line;
}

bool is_valid_path(char * path){
    int forward_counter = 0;
    int back_counter = 0;
    char **args = tokenize_string(path, "/\0");
    int i = 0;
    while(args[i]){
        if(!strcmp(args[i], ".."))
            back_counter++;
        else
            forward_counter++;
        i++;
    }

    if(back_counter > forward_counter)
        return false;
    else
        return true;

    free(args);
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
    char *buf;
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    int len = strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char * date = dc_malloc(sizeof(char) * (len+ 2));
    date = strcat(buf, "\r\n");
    res->date = time;

    free(buf);
    free(date);
}

void set_server(http_res_t * res){

    int len = strlen(SERVER_NAME);
    char * server = dc_malloc(sizeof(char) * (len+ 2));
    server = strcat(SERVER_NAME,"\r\n");

    res->server = server;
}

void set_allow(http_res_t * res){
    int len = strlen(ALLOWED_METHODS);
    char * allow = dc_malloc(sizeof(char) * (len+ 2));
    allow = strcat(ALLOWED_METHODS,"\r\n");

    res->allow = allow;
}

void set_expires(http_res_t * res){
    char *buf;
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    tm.tm_hour += 10; //TODO get the time from config file.
    int len = strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);

    char * expiry_date = dc_malloc(sizeof(char) * (len+ 2));
    expiry_date = strcat(buf, "\r\n");

    res->expires = expiry_date;

    free(buf);
    free(expiry_date);
}

bool prepare_common_headers(char * path, http_res_t * res){
    set_server(&res);
    set_time(&res);
    set_allow(&res);
    set_expires(&res);
}
