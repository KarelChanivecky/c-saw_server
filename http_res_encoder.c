/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_res_encoder.c
 */


#include <dc/stdlib.h>
#include <stdlib.h>
#include "http_res_encoder.h"
#include "string.h"

#define FIELD_DELIMITER "\r\n"

char * getField(http_res_t * res, size_t offset) {
    char ** field_ptr = (char**) ((char*)res + offset);
    return field_ptr == NULL? NULL : *field_ptr;
}

uint8_t * http_res_encode(http_res_t * res, size_t * body_len) {
    if (!res) {
        return NULL;
    }

    size_t offsets[HTTP_RES_T_FIELD_COUNT];
    get_http_res_offsets(offsets);

    size_t bytes_to_allocate = 0;
    for (int i = 0; i < HTTP_RES_T_FIELD_COUNT - 1; i++) {
        char * field = getField(res, offsets[i]);
        bytes_to_allocate += field == NULL? 0 : (strlen(field) + 2);
    }

    bytes_to_allocate += *body_len + 3; // \r\n
    size_t old_body_len = *body_len;
    *body_len = bytes_to_allocate - 2;
    uint8_t * res_string = dc_malloc(sizeof(uint8_t ) * bytes_to_allocate  + 1);

    for ( int i = 0; i < HTTP_RES_T_FIELD_COUNT - 1; ++i ) {
        char * field = getField(res, offsets[i]);
        if (field != NULL) {
            size_t field_len =  strlen(field);
            strncat((char *)res_string, field, field_len + 1);
            strncat((char *)res_string, FIELD_DELIMITER, 3);
            free(field);
        }
    }
    strncat((char *)res_string, FIELD_DELIMITER, 3);
    if (old_body_len == 0) {
        return res_string;
    }

    res_string[bytes_to_allocate] = 0;
    while (bytes_to_allocate-- && old_body_len + 1) {
        uint8_t foo = res->body[old_body_len--];
        res_string[bytes_to_allocate] = foo;
    }
    free(res->body);
    return res_string;
}








