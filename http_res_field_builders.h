/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */

#ifndef C_SAW_SERVER_HTTP_RES_FIELD_BUILDERS_H
#define C_SAW_SERVER_HTTP_RES_FIELD_BUILDERS_H

#include "server_config.h"
#include "http_req_parser.h"
#include "http_res_encoder.h"
#include "string_allocation.h"

#define HTTP_TIME_FORMAT "%a, %d %b %Y %T %x"
#define REQ_END "\r\n"

char * make_status_field( int code );

char * make_last_modified( char * path );

char * make_content_length( char * path );

char * make_content_type( char * path );

char * delete_leading_whitespaces( char * line );

bool path_in_bounds( char * path );

bool is_simple_req( char * request_type );

char * prepare_entity_body( char * path, size_t * body_len );

char * get_date();

char * get_server();

char * get_allow();

char * get_expires( size_t minutes );

void prepare_common_headers( http_res_t * res );

bool file_exists( char * path );

bool file_modified_after_requested_if_modified_date( char * path, char * if_mod_date );

#endif //C_SAW_SERVER_HTTP_RES_FIELD_BUILDERS_H
