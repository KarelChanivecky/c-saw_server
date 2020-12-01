#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "dc/stdlib.h"

#include "http_req_parser.h"
#include "return_codes.h"
#include "string_allocation.h"

#define INITIAL_TOKEN_ALLOC_COUNT 10
#define BLOCK_FOR_REQUEST_LINE 17
#define AUTH "Authorization"
#define IF_MODIFIED_SINCE "If-Modified-Since"
#define FROM "From"
#define REFERER "Referer"
#define USER_AGENT "User-Agent"
#define SIMPLE_REQUEST "Simple-Request"
#define FULL_REQUEST "Full-Request"
#define URI_PATH_DELIMITER '/'
#define SUCCESS 0
#define STATUS_LINE_INDEX 0
#define FIELD_NAME_INDEX 0
#define FIELD_VALUE_INDEX 1
#define DELIMITER_LENGTH 2
#define METHOD_INDEX 0
#define PATH_INDEX 1
#define PROTOCOL_VERSION_INDEX 2

void free_parsed_request_line(char ** parsed_request_line) {
    free(parsed_request_line[METHOD_INDEX]);
    free(parsed_request_line[PATH_INDEX]);
    if (parsed_request_line[PROTOCOL_VERSION_INDEX]) {
        free(parsed_request_line[PROTOCOL_VERSION_INDEX]);
    }
    free(parsed_request_line);
}


char ** tokenize_string( char * line, const char * delim, int i ) {
    int block = i;
    int index = 0;


    char * buffer;
    char ** args = dc_malloc( sizeof( char * ) * block );

    buffer = strtok( line, delim );

    char * token = alloc_string( buffer );

    while ( token != NULL) {

        args[index] = token;
        index++;

        if ( index >= block ) {
            block += INITIAL_TOKEN_ALLOC_COUNT;
            args = realloc( args, block * sizeof( char * ));
            if ( !args ) {
                fprintf( stderr, "realloc allocation error\n" );
                exit( EXIT_FAILURE );
            }
        }

        buffer = strtok(NULL, delim );
        if ( buffer != NULL) {
            token = alloc_string( buffer );
        } else {
            token = NULL;
        }
    }
    args[index] = NULL;

    return args;
}

char ** dynamic_tokenize_req( char * req, int delimeter_len ) {
    int index = 0;
    int block = INITIAL_TOKEN_ALLOC_COUNT;

    size_t line_len = strcspn( req, "\r\n" );
    if ( line_len == 0 ) {
        return NULL;
    }

    char ** args = dc_malloc( sizeof( char * ) * block );
    char * token = dc_malloc( sizeof( char ) * ( line_len + 1 ));
    strncpy( token, req, line_len );
    token[line_len] = '\0';

    while ( token != NULL) {
        args[index] = token;
        index++;

        if ( index >= block ) {
            block += INITIAL_TOKEN_ALLOC_COUNT;
            args = realloc( args, block * sizeof( char * ));
            if ( !args ) {
                fprintf( stderr, "realloc allocation error\n" );
                exit( EXIT_FAILURE );
            }
        }

        req += line_len + delimeter_len;
        line_len = strcspn( req, "\r\n" );
        if ( 0 < line_len ) {
            token = NULL;
        } else {
            token = dc_malloc( sizeof( char ) * ( line_len + 1 ));
            memcpy( token, req, line_len );
            token[line_len] = '\0';
        }
    }

    return args;
}

int setup_request_line( http_req_t * req, char ** parsed_request_line ) {
    req->method = alloc_string( parsed_request_line[0] );

    if ( parsed_request_line[PATH_INDEX] ) {
        if ( *parsed_request_line[PATH_INDEX] != URI_PATH_DELIMITER ) {

            return INVALID_URI;
        } else {
            req->request_URI = alloc_string( parsed_request_line[PATH_INDEX] );
        }
    }

    if ( parsed_request_line[PROTOCOL_VERSION_INDEX] ) {
        req->request_type = alloc_string( FULL_REQUEST );
        req->protocol_version = alloc_string( parsed_request_line[PROTOCOL_VERSION_INDEX] );
        return SUCCESS;
    }

    req->request_type = alloc_string( SIMPLE_REQUEST );
    req->protocol_version = NULL;
    return SUCCESS;
}


void initialize_req( http_req_t * req ) {
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

int tokenize_header(char * container[], char * req_string, const char * delimiter ) {
    if (req_string == NULL) {
        return INVALID_HEADER_FIELD;
    }

    char * buffer;
    buffer = strtok( req_string, delimiter );
    container[FIELD_NAME_INDEX] = alloc_string(buffer);

    buffer = strtok(NULL, "\0");

    if (!buffer) {
        return INVALID_HEADER_FIELD;
    }

    container[FIELD_VALUE_INDEX] = alloc_string(buffer);
    return SUCCESS;
}


int parse_http_req( http_req_t * req, char * req_string ) {
    initialize_req( req );
    char ** lines = dynamic_tokenize_req( req_string, DELIMITER_LENGTH );

    char ** parsed_request_line = tokenize_string(
            lines[STATUS_LINE_INDEX], " ", BLOCK_FOR_REQUEST_LINE );

    int result = setup_request_line( req, parsed_request_line );

    if ( result != 0 ) {
        return result;
    }

    if ( strcmp( req->request_type, SIMPLE_REQUEST ) == 0 ) {
        return SUCCESS;
    }

    int i = FIELD_VALUE_INDEX;
    char * parsed_header[2];
    while ( lines[i] ) {
        if (tokenize_header(parsed_header, lines[i], ":" ) == INVALID_HEADER_FIELD) {
            return INVALID_HEADER_FIELD;
        }

        if ( strcmp( parsed_header[FIELD_NAME_INDEX], AUTH ) == 0 ) {
            req->authorization = alloc_string( parsed_header[FIELD_VALUE_INDEX] );
        } else if ( strcmp( parsed_header[FIELD_NAME_INDEX], FROM ) == 0 ) {
            req->from = alloc_string( parsed_header[FIELD_VALUE_INDEX] );
        } else if ( strcmp( parsed_header[FIELD_NAME_INDEX], REFERER ) == 0 ) {
            req->referer = alloc_string( parsed_header[FIELD_VALUE_INDEX] );
        } else if ( strcmp( parsed_header[FIELD_NAME_INDEX], USER_AGENT ) == 0 ) {
            req->user_agent = alloc_string( parsed_header[FIELD_VALUE_INDEX] );
        } else if ( strcmp( parsed_header[FIELD_NAME_INDEX], IF_MODIFIED_SINCE ) == 0 ) {
            req->if_modified_since = alloc_string( parsed_header[FIELD_VALUE_INDEX] );
        }
        i++;
    }

    free_parsed_request_line( parsed_request_line );
    free( lines );
    return SUCCESS;
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
//        case INVALID_URI:
//            printf("BAD REQUEST_URI.\n");
//            break;
//    }
//}

