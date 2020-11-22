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

    size_t offsets[HTTP_RES_T_FIELD_COUNT];
    get_http_res_offsets(offsets);

    for ( int i = 0; i < HTTP_RES_T_FIELD_COUNT; ++i ) {
        char * field = (char*) res + offsets[i];
        if (res != NULL) {
            size_t field_len =  strlen(field);
            strncat(res_string, field, field_len);
            free(field);
        }
    }
    return res_string;
}
