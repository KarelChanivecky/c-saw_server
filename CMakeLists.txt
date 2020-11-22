cmake_minimum_required(VERSION 3.17)
project(c_saw_server C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(c_saw_server main.c server_driver.c config.c read_args.c read_config_file.c read_defaults.c
        logging.c http_req_parser.c http_res_encoder.c http_res_encoder.h http_res_type.c http_res_type.h)
target_link_libraries(c_saw_server pthread dc_utils dc config)
target_compile_options(c_saw_server PRIVATE -Wpedantic -Wall -Wextra -fPIC)



