cmake_minimum_required(VERSION 3.17)
project(c_saw_server C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(c_saw_server server_driver.c)
target_link_libraries(c_saw_server dc_utils dc)
target_compile_options(c_saw_server PRIVATE -Wpedantic -Wall -Wextra -fPIC)

add_executable(http_req_parser http_req_parser.c)
target_link_libraries(http_req_parser dc)
target_compile_options(http_req_parser PRIVATE -Wpedantic -Wall -Wextra -fPIC)


