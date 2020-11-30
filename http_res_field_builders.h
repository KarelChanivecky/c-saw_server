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

#define HTTP_TIME_FORMAT "%a, %d %b %Y %T %x"
#define REQ_END "\r\n"

char * make_status_field( int code );

char * make_last_modified( char * path );

char * make_content_length( char * path );

char * make_content_type( char * path );

char * delete_leading_whitespaces(char * line);

bool is_valid_path(char * path);

bool is_simple_req(char * request_type);

void prepare_entity_body(char * path, http_res_t * res);

void set_time(http_res_t * res);

void set_server(http_res_t * res);

void set_allow(http_res_t * res);

void set_expires(http_res_t * res, double min);

bool prepare_common_headers(http_res_t * res);

bool file_exists(char * path, char * root);

bool file_modified_after_requested_if_moddified_date(char * path, char *root, char * if_mod_date);

#endif //C_SAW_SERVER_HTTP_RES_FIELD_BUILDERS_H
