//
// Created by root on 2020-11-30.
//

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
