#include <stdlib.h>
#include <dc/stdlib.h>
#include "stdio.h"
#include "../http_res_type.h"
#include "string.h"
#include "../http_res_encoder.h"

int main() {
    http_res_t res;
    res.status_line = dc_malloc(sizeof(char) * 10);
    res.date = dc_malloc(sizeof(char) * 10);
    res.server = dc_malloc(sizeof(char) * 10);
    res.allow = dc_malloc(sizeof(char) * 10);
    res.content_length = dc_malloc(sizeof(char) * 10);
    res.expires = dc_malloc(sizeof(char) * 10);
    res.last_modified = dc_malloc(sizeof(char) * 10);
    res.body = dc_malloc(sizeof(char) * 10);
    res.content_type = dc_malloc(sizeof(char) * 10);

    strncpy(res.status_line , "this status", 9);
    strncpy(res.content_type , "this type ", 9);
    strncpy(res.date , "this date", 9);
    strncpy(res.server , "the server", 9);
    strncpy(res.allow , "the allow", 9);
    strncpy(res.content_length , "this length", 9);
    strncpy(res.expires , "this expires", 9);
    strncpy(res.last_modified , "the last_modified", 9);
    strncpy(res.body , "this body", 9);

    res.status_line [9] = 0;
    res.server [9] = 0;
    res.date [9] = 0;
    res.allow [9] = 0;
    res.content_type [9] = 0;
    res.content_length [9] = 0;
    res.last_modified [9] = 0;
    res.expires [9] = 0;
    res.body [9] = 0;

    char * res_string = http_res_encode(res);

    printf("%s\n", res_string);

    free(res_string);

    return EXIT_SUCCESS;
}