/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_res_encoder.c
 */


#include <dc/stdlib.h>
#include "http_res_encoder.h"
#include "string.h"
#include "stdlib.h"

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
