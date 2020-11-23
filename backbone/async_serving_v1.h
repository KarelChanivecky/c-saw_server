/**
 * @since 2020-11-22
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * async_serving_v1.h
 */

#ifndef C_SAW_SERVER_ASYNC_SERVING_H
#define C_SAW_SERVER_ASYNC_SERVING_H

#include "../server_config.h"

_Noreturn void serve(server_config_t * server_config, int listen_socket_fd);

#endif //C_SAW_SERVER_ASYNC_SERVING_H
