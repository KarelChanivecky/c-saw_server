# c-saw_server
A static http server

## known issues
if-modified-since does not take into account time zone differences

## Configurations

1) port:
    1) default value: 80
    2) command line flag: ```--port```
    
2) max_concurrent_conn:
    1) default value: 10
    2) command line flag: ```--concurrent_conn```
    
3) max_open_conn:
    1) default value: 40
    2) command line flag: ```--open_conn```
    
4) read_buffer_size:
    1) default value: 1024
    2) command line flag: ```--read_buffer_size```
    
5) write_buffer_size:
    1) default value: 1024
    2) command line flag: ```--write_buffer_size```
    
6) page_expiration_time_mins:
    1) default value: 5
    2) command line flag: ```--expire```
    
7) log_connections:
    1) default value: 1
    2) command line flag: ```--log_connections```
    
8) concurrency_model: 1 for processes, 0 for threads
    1) default value: 1
    2) command line flag: ```--concurrency_model```
    
9) pooled:
    1) default value: 1
    2) command line flag: ```--pooled```
    
10) content_root_dir_path:
    1) default value: "../static_html/"
    2) command line flag: ```--root_dir```
    
11) page_404_path:
    1) default value: "../static_html/404.html"
    2) command line flag: ```--page_404```
    
12) sin_family:
    1) default value: 2
    2) command line flag: ```--sin_family```
    
13) addr:
    1) default value: 0
    2) command line flag: ```--addr```
