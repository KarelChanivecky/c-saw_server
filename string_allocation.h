//
// Created by root on 2020-11-30.
//

#ifndef C_SAW_SERVER_STRING_ALLOCATION_H
#define C_SAW_SERVER_STRING_ALLOCATION_H

#define DEFAULT_INITIAL_TOKEN_ALLOC_COUNT 10

char * alloc_string( char * string );

char ** tokenize_string( char * line, const char * delim, int i );

char ** dynamic_tokenize_req( char * req, int delimeter_len );

char * read_fd( int fd );

char * join_strings( char * first, char * scnd );

#endif //C_SAW_SERVER_STRING_ALLOCATION_H
