//
// Created by animesh on 2020-11-07.
//
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "dc/stdlib.h"




#include "http_req_parser.h"

#define BLOCK 17
#define AUTH "Authorization"
#define FROM "From"
#define REFERER "Referer"
#define USER_AGENT "User-Agent"
#define SIMPLE_REQUEST "Simple-Request"
#define FULL_REQUEST "Full-Request"
#define DEFAULT_REQ_URI "/"
#define METHOD_ERROR 99
#define URI_ERROR 999
#define SUCCESS 0

char **tokenize_string(char * req_string, const char *delim){
    int block = BLOCK;
    int current_index = 0;

    char *buffer;
    char **args = malloc(sizeof(char*) * block);

    if(!args){
        fprintf(stderr, "malloc allocation error\n");
        exit(EXIT_FAILURE);
    }

    buffer = strtok(req_string, delim);

    size_t buffer_len = strlen(buffer);

    char *token = dc_malloc(sizeof(char) * (buffer_len + 1));
    strncpy(token,buffer,buffer_len);
    token[buffer_len] = '\0';


    while (token != NULL) {

        args[current_index] = token;
        current_index++;

        if (current_index >= block) {
            block += 10;
            args = realloc(args, block * sizeof(char*));
            if (!args) {
                fprintf(stderr, "realloc allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        buffer = strtok(NULL, delim);
        if(buffer != NULL) {
            buffer_len = strlen( buffer );
            token = dc_malloc( sizeof( char ) * ( buffer_len + 1 ));
            strncpy( token, buffer, buffer_len );
            token[ buffer_len ] = '\0';
        }else
            token = NULL;
    }
    args[current_index] = NULL;

    free(buffer);
    free(token);

    return args;
}

int setup_request_line(http_req_t * req, char ** parsed_request_line){
    int i = SUCCESS;
    if((strcmp(parsed_request_line[0], "GET") == 0) || (strcmp(parsed_request_line[0], "HEAD") == 0)){
        req->method = parsed_request_line[0];
    }else{
        return METHOD_ERROR;
    }

    req->request_URI = DEFAULT_REQ_URI;


    if(parsed_request_line[1] != (void *)0){
        if(parsed_request_line[1][0] != '/')
            return URI_ERROR;
        else
            req->request_URI = parsed_request_line[1];
    }

    if(parsed_request_line[2] != (void *)0) {
        req->request_type = FULL_REQUEST;
        req->protocol_version = parsed_request_line[2];
    }else{
        req->request_type = SIMPLE_REQUEST;
        req->protocol_version = NULL;
    }

    return i;

}

void initialize_req(http_req_t * req){
    req->request_type = NULL;
    req->method = NULL;
    req->request_URI = NULL;
    req->protocol_version = NULL;
    req->authorization = NULL;
    req->from = NULL;
    req->if_modified_since = NULL;
    req->referer = NULL;
    req->user_agent = NULL;
}
char **tokenize_header(char * req_string, const char *delim){
    int block = BLOCK;
    int current_index = 0;

    char *buffer;
    char **args = malloc(sizeof(char*) * block);

    if(!args){
        fprintf(stderr, "malloc allocation error\n");
        exit(EXIT_FAILURE);
    }

    buffer = strtok(req_string, delim);

    size_t buffer_len = strlen(buffer);

    char *token = dc_malloc(sizeof(char) * (buffer_len + 1));
    strncpy(token,buffer,buffer_len);
    token[buffer_len] = '\0';


    while (token != NULL) {

        args[current_index] = token;
        current_index++;

        if (current_index >= block) {
            block += 10;
            args = realloc(args, block * sizeof(char*));
            if (!args) {
                fprintf(stderr, "realloc allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        buffer = strtok(NULL, "\r\n");
        if(buffer != NULL) {
            buffer_len = strlen( buffer );
            token = dc_malloc( sizeof( char ) * ( buffer_len + 1 ));
            strncpy( token, buffer, buffer_len );
            token[ buffer_len ] = '\0';
        }else
            token = NULL;
    }
    args[current_index] = NULL;

    free(buffer);
    free(token);

    return args;

}

int parse_http_req(http_req_t * req, char * req_string){
    int result = 0;
    initialize_req(req);
    char **lines = tokenize_string(req_string, "\r\n");

    char *request_line = lines[0];
    char **parsed_request_line = tokenize_string(request_line, " ");


    if((result = setup_request_line(req, parsed_request_line)) != 0){
        return result;
    }else if((strcmp(req->request_type, SIMPLE_REQUEST) == 0))
        return SUCCESS;

    int i = 1;
    char **parsed_headers;
    while(lines[i]){
        parsed_headers = tokenize_header(lines[i], ":");

        if (strcmp(parsed_headers[0], AUTH) == 0){
            req->authorization = parsed_headers[1];
        }else if(strcmp(parsed_headers[0], FROM) == 0){
            req->from = parsed_headers[1];
        }else if(strcmp(parsed_headers[0], REFERER) == 0){
            req->referer = parsed_headers[1];
        }else if(strcmp(parsed_headers[0], USER_AGENT) == 0){
            req->user_agent = parsed_headers[1];
        }
        i++;
    }

    free(parsed_headers);
    free(parsed_request_line );
    free(request_line);
    free(lines);
    return result;
}

//int main(int argc, char * argv[]) {
//    struct http_req_t http_req_t;
//    char str[] ="GET / HTTP/1.1\r\nHost: localhost:8080\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\r\nAccept-Language: en-us\r\nDNT: 1\r\nAccept-Encoding: gzip, deflate\r\n";
//    int result = parse_http_req(&http_req_t, str);
//    switch(result){
//        case 0:
//            printf("method : %s\n",http_req_t.method);
//            printf("request URI : %s\n",http_req_t.request_URI);
//            printf("protocol version : %s\n",http_req_t.protocol_version);
//            printf("user agent: %s\n",http_req_t.user_agent);
//            printf("referer: %s\n",http_req_t.referer);
//            printf("authorization: %s\n",http_req_t.authorization);
//            printf("from: %s\n",http_req_t.from);
//            break;
//        case METHOD_ERROR:
//            printf("Incorrect get method.\n");
//            break;
//        case URI_ERROR:
//            printf("BAD REQUEST_URI.\n");
//            break;
//    }
//    return 0;
//}

