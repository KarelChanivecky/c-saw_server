//
// Created by root on 2020-11-30.
//

#include <stdlib.h>
#include <dc/unistd.h>
#include "string_allocation.h"
#include "string.h"
#include "dc/stdlib.h"



char * alloc_string( char * string ) {
    if ( !string ) {
        return NULL;
    }
    size_t len = strlen( string );
    char * allocated_string = dc_malloc( sizeof( char ) * len + 1 );
    strncpy( allocated_string, string, len );
    allocated_string[len] = 0;
    return allocated_string;
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
            block += DEFAULT_INITIAL_TOKEN_ALLOC_COUNT;
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
    int block = DEFAULT_INITIAL_TOKEN_ALLOC_COUNT;

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
            block += DEFAULT_INITIAL_TOKEN_ALLOC_COUNT;
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

#define DEFAULT_BUFFER_SIZE 1024
#define READ_ALLOC_COEFF 4

char * read_fd( int fd ) {
    size_t allocated_space = DEFAULT_BUFFER_SIZE * READ_ALLOC_COEFF;
    char * req_string = dc_malloc( sizeof( char ) * DEFAULT_BUFFER_SIZE * READ_ALLOC_COEFF );
    char buffer[DEFAULT_BUFFER_SIZE];
    size_t bytes_read = dc_read( fd, buffer, DEFAULT_BUFFER_SIZE );
    size_t total_bytes_read = bytes_read;

    while ( bytes_read ) {


        if (( long long ) allocated_space - ( long long ) total_bytes_read < 0 ) {
            req_string = dc_realloc( req_string,
                                     sizeof( char ) * allocated_space + ( DEFAULT_BUFFER_SIZE * READ_ALLOC_COEFF ));
        }

        strncat( req_string, buffer, bytes_read );

        bytes_read = dc_read( fd, buffer, DEFAULT_BUFFER_SIZE );
        total_bytes_read += bytes_read;
    }
    req_string = dc_realloc( req_string, sizeof( char ) * ( total_bytes_read + 1 ));
    req_string[ total_bytes_read ] = 0;
    return req_string;
}

char * join_strings( char * first, char * scnd ) {
    size_t first_len = strlen( first );
    size_t scnd_len = strlen( scnd );
    char * buf = dc_malloc( sizeof( char * ) * ( first_len + scnd_len + 1 ));
    strncpy( buf, first, first_len );
    strncat( buf, scnd, scnd_len);
    return buf;
}
