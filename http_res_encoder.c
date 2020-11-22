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

char * getField(http_res_t * res, size_t offset) {
    char ** field_ptr = (char**) ((char*)res + offset);
    return field_ptr == NULL? NULL : *field_ptr;
}

char * http_res_encode(http_res_t * res) {
    if (!res) {
        return NULL;
    }

    size_t offsets[HTTP_RES_T_FIELD_COUNT];
    get_http_res_offsets(offsets);

    size_t bytes_to_allocate = 0;
    for (int i = 0; i < HTTP_RES_T_FIELD_COUNT; i++) {
        char * field = getField(res, offsets[i]);
        bytes_to_allocate += field == NULL? 0 : strlen(field);
    }

    char * res_string = dc_malloc(sizeof(char ) * bytes_to_allocate + 1);

    for ( int i = 0; i < HTTP_RES_T_FIELD_COUNT; ++i ) {
        char * field = getField(res, offsets[i]);
        if (field != NULL) {
            size_t field_len =  strlen(field);
            strncat(res_string, field, field_len);
            free(field);
        }
    }

    res_string[bytes_to_allocate] = 0;
    return res_string;
}
