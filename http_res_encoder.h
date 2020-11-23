/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * http_res_encoder.h
 */

#ifndef C_SAW_SERVER_HTTP_RES_ENCODER_H
#define C_SAW_SERVER_HTTP_RES_ENCODER_H

#include "handle_req.h"
#include "http_res_type.h"

/**
 * Encode a response struct into a string
 * @param res a pointer to struct http_res_t
 * @return the encoded response or null for error
 */
char * http_res_encode(http_res_t * res);

#endif //C_SAW_SERVER_HTTP_RES_ENCODER_H
