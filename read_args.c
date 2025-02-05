//
//  read_args.c
//
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_args.h"
    
char** get_args(int argc, char **argv){
    char** args = calloc(13, sizeof(char*));
    int c;

    static struct option long_options[] =
    {
        {"port", required_argument, 0, 'a'},
        {"sin_family", required_argument, 0, 'b'},
        {"addr", required_argument, 0, 'c'},
        {"concurrent_conn", required_argument, 0, 'd'},
        {"open_conn", required_argument, 0, 'e'},
        {"read_buffer_size", required_argument, 0, 'f'},
        {"log_connections", required_argument, 0, 'g'},
        {"concurrency_model", required_argument, 0, 'h'},
        {"root_dir", required_argument, 0, 'i'},
        {"page_404", required_argument, 0, 'j'},
        {"write_buffer_size", required_argument, 0, 'k'},
        {"expire", required_argument, 0, 'l'},
        {"pooled", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    char* arg_pattern = "a:b:c:d:e:f:g:h:i:j:k:l:m:";

    int option_index = 0;

    while ((c = getopt_long(argc, argv, arg_pattern, long_options, &option_index)) != -1)
        switch (c)
        {
            case 'a':
            {
                args[0] = optarg;
                break;
            }
           case 'b':
                args[1] = optarg;
                break;
           case 'c':
                args[2] = optarg;
                break;
           case 'd':
                args[3] = optarg;
                break;
           case 'e':
                args[4] = optarg;
                break;
           case 'f':
                args[5] = optarg;
                break;
           case 'g':
                args[6] = optarg;
                break;
           case 'h':
                args[7] = optarg;
                break;
           case 'i':
                args[8] = optarg;
                break;
           case 'j':
                args[9] = optarg;
                break;
            case 'k':
                args[10] = optarg;
                break;
            case 'l':
                args[11] = optarg;
                break;
            case 'm':
                args[12] = optarg;
                break;
           case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
//                 return NULL;
           default:
             abort ();
           }

    
    return args;
}
