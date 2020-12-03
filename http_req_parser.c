#include <string.h>
#include <stdlib.h>
#include <dc/stdlib.h>

#include "http_req_parser.h"
#include "return_codes.h"
#include "string_allocation.h"


#define BLOCK_FOR_REQUEST_LINE 17
#define AUTH "Authorization"
#define IF_MODIFIED_SINCE "If-Modified-Since"
#define FROM "From"
#define REFERER "Referer"
#define USER_AGENT "User-Agent"
#define SIMPLE_REQUEST "Simple-Request"
#define FULL_REQUEST "Full-Request"
#define STATUS_LINE_INDEX 0
#define FIELD_NAME_INDEX 0
#define FIELD_VALUE_INDEX 1
#define DELIMITER_LENGTH 2
#define METHOD_INDEX 0
#define PATH_INDEX 1
#define PROTOCOL_VERSION_INDEX 2

void free_parsed_request_line( char ** parsed_request_line ) {
    free( parsed_request_line[METHOD_INDEX] );
    if (parsed_request_line[PATH_INDEX]) {
        free( parsed_request_line[PATH_INDEX] );
    }
    if ( parsed_request_line[PROTOCOL_VERSION_INDEX] ) {
        free( parsed_request_line[PROTOCOL_VERSION_INDEX] );
    }
    free( parsed_request_line );
}

int tokenize_header( char * container[], char * req_string, const char * delimiter ) {
    if ( req_string == NULL) {
        return INVALID_HEADER_FIELD;
    }

    char * buffer;
    buffer = strtok( req_string, delimiter );
    container[FIELD_NAME_INDEX] = alloc_string( buffer );

    buffer = strtok(NULL, "\0" );

    if ( !buffer ) {
        return INVALID_HEADER_FIELD;
    }

    container[FIELD_VALUE_INDEX] = alloc_string( buffer );
    return SUCCESS;
}

#define INDEX_FILENAME "index.html"

int setup_request_line( http_req_t * req, char ** parsed_request_line, server_config_t * server_cfg ) {
    req->method = alloc_string( parsed_request_line[0] );

    if ( parsed_request_line[PATH_INDEX] ) {
        if ( *parsed_request_line[PATH_INDEX] != URI_PATH_DELIMITER ) {
            return INVALID_URI;
        }
        char * requested_path = ( parsed_request_line[PATH_INDEX] + 1 );
        if ( parsed_request_line[PATH_INDEX][strlen( parsed_request_line[PATH_INDEX]  ) - 1] == '/' ) {
            requested_path = join_strings( requested_path, INDEX_FILENAME );
            free( parsed_request_line[PATH_INDEX] );
            parsed_request_line[PATH_INDEX] = NULL;
            req->request_URI = join_strings( server_cfg->content_root_dir_path, requested_path );
            free( requested_path );
        } else {
            req->request_URI = join_strings( server_cfg->content_root_dir_path, requested_path );
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


int parse_http_req( http_req_t * req, char * req_string, server_config_t * server_cfg ) {
    initialize_req( req );
    char ** lines = dynamic_tokenize_req( req_string, DELIMITER_LENGTH );

    char ** parsed_request_line = tokenize_string(
            lines[STATUS_LINE_INDEX], " ", BLOCK_FOR_REQUEST_LINE );

    int result = setup_request_line( req, parsed_request_line, server_cfg );

    if ( result != 0 ) {
        return result;
    }

    if ( strcmp( req->request_type, SIMPLE_REQUEST ) == 0 ) {
        return SUCCESS;
    }

    int i = FIELD_VALUE_INDEX;
    char * parsed_header[2];
    while ( lines[i] ) {
        if ( tokenize_header( parsed_header, lines[i], ":" ) == INVALID_HEADER_FIELD ) {
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

