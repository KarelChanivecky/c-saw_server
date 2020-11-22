cmake_minimum_required(VERSION 3.17.3)
project(c_saw_server C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)`

add_executable(c_saw_server main.c server_driver.c config.c read_args.c read_config_file.c read_defaults.c
        logging.c http_req_parser.c http_res_encoder.c http_res_encoder.h http_res_type.c http_res_type.h http_req_checks.c http_req_checks.h http_res_field_builders.c http_res_field_builders.h)
target_link_libraries(c_saw_server pthread dc_utils dc config m)
target_compile_options(c_saw_server PRIVATE -Wpedantic -Wall -Wextra -fPIC)

add_executable(test_http_res_encoder http_res_encoder.c http_res_encoder.h http_res_type.c http_res_type.h
        tests/test_http_res_encode.c)
target_link_libraries(test_http_res_encoder pthread dc)
target_compile_options(test_http_res_encoder PRIVATE -Wpedantic -Wall -Wextra -fPIC)



