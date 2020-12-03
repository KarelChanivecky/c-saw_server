cmake_minimum_required(VERSION 3.17.3)
project(c_saw_server C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(c_saw_server
        main.c
        config.c
        read_args.c
        read_config_file.c
        read_defaults.c
        handle_req.c
        http_req_parser.c
        http_res_encoder.c
        http_res_type.c
        http_req_checks.c
        http_res_field_builders.c
        backbone/logging.c
        backbone/server_driver.c
	    backbone/async_serving_v2.c
        backbone/async_threading.c
        backbone/async_process.c
        backbone/serve_request.c
		string_allocation.c
		string_allocation.h)
add_executable(n_curses ncurses_main.c)
target_link_libraries(n_curses ncurses config)
target_compile_options(c_saw_server PRIVATE -Wpedantic -Wall -Wextra)

target_link_libraries(c_saw_server pthread dc_utils dc config m)
target_compile_options(c_saw_server PRIVATE -Wpedantic -Wall -Wextra -fPIC)



