//
// Created by animesh on 2020-11-22.
//

#include "handle_req.h"
#include "string.h"

#define HTTP_VERSION "HTTP/1.0"
#define REASON_200 "OK"
#define REASON_304 "Not modified since cached time"
#define REASON_403 "Path points outside of public directory"
#define REASON_404 "Page not found"
#define REASON_405 "Only "
#define REASON_505 "Internal server error"
#define GET "GET"
#define HEAD "HEAD"

void initialize_res(http_res_t *res){
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

int prepare_get_response(http_req_t * req, http_res_t * res, server_config_t * config){
    if(is_valid_path(req->request_URI))
        if(file_exists(req->request_URI, config->content_root_dir_path)) {
            if(file_modified_after_requested_if_moddified_date(req->request_URI, req->if_modified_since, config->content_root_dir_path)){ //TODO implement compare_dates func.
                res->status_line = make_status_field( 200 );
                prepare_entity_body(req->request_URI, res);
                res->last_modified = make_last_modified(req->request_URI);
                res->content_length = make_content_length(req->request_URI);
                res->content_type = make_content_type(req->request_URI);
                set_expires(res, config->max_concurrent_conn); //TODO change it to expiry time from config.
                return EXIT_SUCCESS;
            }else{
                res->status_line = make_status_field( 304 );
                res->last_modified = make_last_modified(req->request_URI);
                res->content_length = make_content_length(req->request_URI);
                res->content_type = make_content_type(req->request_URI);
                set_expires(res, config->max_concurrent_conn); //TODO change it to expiry time from config.
                return EXIT_SUCCESS;
            }
        }else{
            res->status_line = make_status_field( 404 );
            set_time(res);
            set_server(res);
            prepare_entity_body(config->page_404_path, res);
            return EXIT_SUCCESS;
        }
    else{
        res->status_line = make_status_field( 403 );
        set_time(res);
        set_server(res);
        return EXIT_SUCCESS;
    }
}

int prepare_head_response(http_req_t * req, http_res_t * res, server_config_t * config){
    if(is_valid_path(req->request_URI))
        if(file_exists(req->request_URI, config->content_root_dir_path)) {
            if(file_modified_after_requested_if_moddified_date(req->request_URI, req->if_modified_since, config->content_root_dir_path)){ //TODO implement compare_dates func.
                res->status_line = make_status_field( 200 );
                res->last_modified = make_last_modified(req->request_URI);
                res->content_length = make_content_length(req->request_URI);
                res->content_type = make_content_type(req->request_URI);
                set_expires(res, config->max_concurrent_conn); //TODO change it to expiry time from config.
                return EXIT_SUCCESS;
            }else{
                res->status_line = make_status_field( 304 );
                res->last_modified = make_last_modified(req->request_URI);
                res->content_length = make_content_length(req->request_URI);
                res->content_type = make_content_type(req->request_URI);
                set_expires(res, config->max_concurrent_conn); //TODO change it to expiry time from config.
                return EXIT_SUCCESS;
            }
        }else{
            res->status_line = make_status_field( 404 );
            set_time(res);
            set_server(res);
            return EXIT_SUCCESS;
        }
    else{
        res->status_line = make_status_field( 403 );
        set_time(res);
        set_server(res);
        return EXIT_SUCCESS;
    }
}

int handle_req(http_req_t * req, http_res_t * res, server_config_t * config){
    
    initialize_res(res);
    
    if(is_simple_req(req->request_type)){
        if(is_valid_path(req->request_type))
            prepare_entity_body(req->request_URI, res);
        else{
            res->status_line = make_status_field( 403 );
            set_time(res);
            set_server(res);
            return EXIT_SUCCESS;
        }
    }else{
        prepare_common_headers(res);
        if(strcmp((req->method), GET) == 0){  //if method is GET
            prepare_get_response( req, res, config);
            return EXIT_SUCCESS;
        }else if(strcmp((req->method), HEAD) == 0){ //if method is GET
            prepare_head_response( req, res, config);
            return EXIT_SUCCESS;
        }else{
            res->status_line = make_status_field( 501 );
            set_time(res);
            set_server(res);
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}
