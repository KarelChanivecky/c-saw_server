#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "dc/stdlib.h"

#include "http_req_parser.h"

#define BLOCK 10
#define BLOCK_FOR_HEADERS 2
#define BLOCK_FOR_REQUEST_LINE 17
#define AUTH "Authorization"
#define IF_MODIFIED_SINCE "If-Modified-Since"
#define FROM "From"
#define REFERER "Referer"
#define USER_AGENT "User-Agent"
#define SIMPLE_REQUEST "Simple-Request"
#define FULL_REQUEST "Full-Request"
#define DEFAULT_REQ_URI "/"
#define URI_ERROR 999
#define SUCCESS 0
#define NUMBER_ZERO 0
#define NUMBER_ONE 1

char **tokenize_string(char * req_string, const char *delim, int i){
    int block = i;
    int index = NUMBER_ZERO;

    char *buffer;
    char **args = dc_malloc(sizeof(char*) * block);

    buffer = strtok(req_string, delim);

    size_t buffer_len = strlen(buffer);

    char *token = dc_malloc(sizeof(char) * (buffer_len + NUMBER_ONE));
    strncpy(token,buffer,buffer_len);
    token[buffer_len] = '\0';

    while (token != NULL) {

        args[index] = token;
        index++;

        if (index >= block) {
            block += BLOCK;
            args = realloc(args, block * sizeof(char*));
            if (!args) {
                fprintf(stderr, "realloc allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        buffer = strtok(NULL, delim);
        if(buffer != NULL) {
            buffer_len = strlen( buffer );
            token = dc_malloc( sizeof( char ) * ( buffer_len + NUMBER_ONE ));
            strncpy( token, buffer, buffer_len );
            token[ buffer_len ] = '\0';
        }else
            token = NULL;
    }
    args[index] = NULL;

    free(buffer);
    free(token);

    return args;
}

char **dynamic_tokenize_req(char * req){
    int index = 0;
    int block = BLOCK;
    char **args = dc_malloc(sizeof(char*) * block);
    size_t line_len = strcspn(req, "\r\n");
    char *token = dc_malloc(sizeof(char) * (line_len + NUMBER_ONE));
    memcpy(token, req, line_len);
    token[line_len] = '\0';

    while(token != NULL){
        args[index++] = token;
        if (index >= block) {
            block += BLOCK;
            args = realloc(args, block * sizeof(char*));
            if (!args) {
                fprintf(stderr, "realloc allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        req += line_len + 2;
        line_len = strcspn(req, "\r\n");
        if(line_len == 0)
            break;
        else {
            token = dc_malloc( sizeof( char ) * ( line_len + NUMBER_ONE ));
            memcpy( token, req, line_len );
            token[ line_len ] = '\0';
        }
    }

    free(token);

    return args;
}

int setup_request_line(http_req_t * req, char ** parsed_request_line){
    int i = SUCCESS;
//    if((strcmp(parsed_request_line[0], "GET") == 0) || (strcmp(parsed_request_line[0], "HEAD") == 0)){
//        req->method = parsed_request_line[0];
//    }else{
//        return METHOD_ERROR;
//    }
    req->method = parsed_request_line[0];
    req->request_URI = DEFAULT_REQ_URI;


    if(parsed_request_line[1] != (void *)0){
        if(parsed_request_line[1][NUMBER_ZERO] != '/')
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
    int block = BLOCK_FOR_HEADERS;
    int current_index = NUMBER_ZERO;

    char *buffer;
    char **args = dc_malloc(sizeof(char*) * block);


    buffer = strtok(req_string, delim);

    size_t buffer_len = strlen(buffer);

    char *token = dc_malloc(sizeof(char) * (buffer_len + 1));
    strncpy(token,buffer,buffer_len);
    token[buffer_len] = '\0';


    while (token != NULL) {

        args[current_index] = token;
        current_index++;

        if (current_index >= block) {
            block += BLOCK;
            args = realloc(args, block * sizeof(char*));
            if (!args) {
                fprintf(stderr, "realloc allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        buffer = strtok(NULL, "\0");
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
    int result = SUCCESS;
    initialize_req(req);
    char **lines  =  dynamic_tokenize_req(req_string);
    char *request_line = lines[NUMBER_ZERO];
    char **parsed_request_line = tokenize_string(request_line, " ", BLOCK_FOR_REQUEST_LINE);


    if((result = setup_request_line(req, parsed_request_line)) != 0){
        return result;
    }else if((strcmp(req->request_type, SIMPLE_REQUEST) == NUMBER_ZERO))
        return SUCCESS;

    int i = NUMBER_ONE;
    char **parsed_headers;
    while(lines[i]){
        parsed_headers = tokenize_header(lines[i], ":");

        if (strcmp(parsed_headers[NUMBER_ZERO], AUTH) == NUMBER_ZERO){
            req->authorization = parsed_headers[NUMBER_ONE];
        }else if(strcmp(parsed_headers[NUMBER_ZERO], FROM) == NUMBER_ZERO){
            req->from = parsed_headers[NUMBER_ONE];
        }else if(strcmp(parsed_headers[NUMBER_ZERO], REFERER) == NUMBER_ZERO){
            req->referer = parsed_headers[NUMBER_ONE];
        }else if(strcmp(parsed_headers[NUMBER_ZERO], USER_AGENT) == NUMBER_ZERO){
            req->user_agent = parsed_headers[NUMBER_ONE];
        }else if(strcmp(parsed_headers[NUMBER_ZERO], IF_MODIFIED_SINCE) == NUMBER_ZERO){
            req->if_modified_since = parsed_headers[NUMBER_ONE];
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
//    char str[] ="GET /home.html HTTP/1.1\nHost: developer.mozilla.org\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.9; rv:50.0) Gecko/20100101 Firefox/50.0\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\nAccept-Language: en-US,en;q=0.5";
//    int result = parse_http_req(&http_req_t, str);
//    switch(result){
//        case 0:
//            printf("method : %s\n",http_req_t.method);
//            printf("user_agent : %s\n",http_req_t.user_agent);
//            break;
//        case METHOD_ERROR:
//            printf("Incorrect get method.\n");
//            break;
//        case URI_ERROR:
//            printf("BAD REQUEST_URI.\n");
//            break;
//    }
//}

