//
// Created by animesh on 2020-11-22.
//

#include "handle_req.h"
#include "string.h"
#include "return_codes.h"
#include "http_req_checks.h"
#include "stdio.h"

#define HTTP_VERSION "HTTP/1.0"


void initialize_res( http_res_t * res ) {
    res->body = NULL;
    res->status_line = NULL;
    res->date = NULL;
    res->server = NULL;
    res->content_type = NULL;
    res->allow = NULL;
    res->expires = NULL;
    res->content_length = NULL;
    res->last_modified = NULL;
}


int prepare_get_head_response( http_req_t * req, http_res_t * res, server_config_t * server_cfg, size_t * body_len ) {
    int status;
    char * path = NULL;
    bool is_head = check_method( req ) == METHOD_HEAD;
    if ( !path_in_bounds( req->request_URI )) {
        status = STATUS_FORBIDDEN;
    } else if ( file_exists( req->request_URI )) {
        if ( check_modified_since( req )) {
            status = STATUS_OK;
            path = req->request_URI;
        } else {
            status = STATUS_NOT_MODIFIED;
        }
        res->last_modified = make_last_modified( req->request_URI );
        res->content_length = make_content_length( req->request_URI );
        res->content_type = make_content_type( req->request_URI );
        res->expires = get_expires( server_cfg->page_expiration_time_mins );
    } else {
        status = STATUS_NOT_FOUND;
        path = server_cfg->page_404_path;
        res->content_length = make_content_length(server_cfg->page_404_path);
        res->content_type = make_content_type(server_cfg->page_404_path);
    }
    if ( !path || is_head ) {
        res->body = NULL;
        return status;
    }

    res->body = prepare_entity_body( path, body_len );
    if (!res->body) {
        status = STATUS_INTERNAL_SERVER_ERROR;
        free(res->content_length);
        res->content_length = NULL;
        free(res->content_type);
        res->content_type = NULL;
    }
    res->status_line = make_status_field( status );
    return status;
}


int handle_simple_request( http_req_t * req, http_res_t * res, size_t * body_len ) {
    if ( path_in_bounds( req->request_type )) {
        res->body = prepare_entity_body( req->request_URI, body_len );
        return STATUS_OK;
    }
    res->status_line = make_status_field( STATUS_FORBIDDEN );
    return STATUS_FORBIDDEN;
}

int handle_req( http_req_t * req, http_res_t * res, server_config_t * server_cfg, size_t * body_len ) {

    initialize_res( res );

    prepare_common_headers( res );
    if ( is_simple_req( req->request_type )) {
        return handle_simple_request( req, res, body_len );
    }

    if ( check_method( req ) != METHOD_OTHER ) {  //if method is GET
        return prepare_get_head_response( req, res, server_cfg, body_len );
    }

    res->status_line = make_status_field( STATUS_NOT_IMPLEMENTED );

    return STATUS_NOT_IMPLEMENTED;
}
