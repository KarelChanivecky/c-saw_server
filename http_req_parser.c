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

char **tokenize_string(char * req_string, const char *delim){
    int block = BLOCK;
    int current_index = 0;
    char *token;
    char **args = malloc(sizeof(char*) * block);

    if(!args){
        fprintf(stderr, "malloc allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(req_string, delim);

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

        token = strtok(NULL, delim);
    }
    args[current_index] = NULL;

    return args;
}

int setup_request_line(http_req_t * req, char ** parsed_request_line){
    int i = 0;
    if((strcmp(parsed_request_line[0], "GET") == 0) || (strcmp(parsed_request_line[0], "HEAD") == 0)){
        req->method = parsed_request_line[0];
    }else{
        i = METHOD_ERROR;
    }

    req->request_URI = DEFAULT_REQ_URI;


    if(parsed_request_line[1] != (void *)0){
        char * uri = parsed_request_line[1];
        if(uri[0] != '/')
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


int parse_http_req(http_req_t * req, char * req_string){
    int result = 0;
    initialize_req(req);
    char **lines = tokenize_string(req_string, "\n");

    char *request_line = lines[0];
    char **parsed_request_line = tokenize_string(request_line, " ");


    if((result = setup_request_line(req, parsed_request_line)) != 0){
        return result;
    }

    int i = 1;
    while(lines[i]){
        char *headers = lines[i];
        char **parsed_headers = tokenize_string(headers, ":");

        char *header = parsed_headers[0];
        char *value = parsed_headers[1];

        if (strcmp(header, AUTH) == 0){
            req->authorization = parsed_headers[1];
        }else if(strcmp(header, FROM) == 0){
            req->from = parsed_headers[1];
        }else if(strcmp(header, REFERER) == 0){
            req->referer = parsed_headers[1];
        }else if(strcmp(header, USER_AGENT) == 0){
            req->user_agent = parsed_headers[1];
        }
        i++;
    }
    return result;
}

int main(int argc, char * argv[]) {
    struct http_req_t http_req_t;
    char str[] ="GET /home.html HTTP/1.1\nHost: developer.mozilla.org\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5";
    int result = parse_http_req(&http_req_t, str);
    switch(result){
        case 0:
            printf("method : %s\n",http_req_t.method);
            printf("user_agent : %s\n",http_req_t.user_agent);
            break;
        case METHOD_ERROR:
            printf("Incorrect get method.\n");
            break;
        case URI_ERROR:
            printf("BAD REQUEST_URI.\n");
            break;
    }

}

