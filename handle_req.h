/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * handle_req.h
 */

#ifndef C_SAW_SERVER_HANDLE_REQ_H
#define C_SAW_SERVER_HANDLE_REQ_H

#include "http_req_parser.h"
#include "http_res_type.h"
#include "http_res_field_builders.h"
#include "http_res_encoder.h"
#include "server_config.h"


/**
 * Prepares a response for a given http request.
 * @param req the request information
 * @param res the un-encoded response including header and body
 * @return SUCCESS or an error code
 */
int handle_req(http_req_t * req, http_res_t * res, server_config_t * server_cfg);

#endif //C_SAW_SERVER_HANDLE_REQ_H
