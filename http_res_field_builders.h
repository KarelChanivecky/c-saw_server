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

#define HTTP_TIME_FORMAT "%a, %d %b %Y %T %x"

char * make_status_field( int code );

char * make_last_modified( char * path );

char * make_content_length( char * path );

char * make_content_type( server_config_t server_cfg, char * path );

#endif //C_SAW_SERVER_HTTP_RES_FIELD_BUILDERS_H
