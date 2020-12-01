//
//  read_args.c
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#include "read_args.h"
    
char** get_args(int argc, char **argv){
    char** args = calloc(10, sizeof(char*));
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
        {"conncurrency_model", required_argument, 0, 'h'},
        {"root_dir", required_argument, 0, 'i'},
        {"page_404", required_argument, 0, 'j'},
        {"write_buffer_size", required_argument, 0, 'k'},
        {0, 0, 0, 0}
    };

    char* arg_pattern = "a:b:c:d:e:f:g:h:i:j:k:";

    int option_index = 0;

    while ((c = getopt_long(argc, argv, arg_pattern, long_options, &option_index)) != -1)
        switch (c)
        {
            case 'a':
            {
//                    args[0] = malloc((int)(strlen(optarg)+1)*sizeof(char));
//                    strcpy(args[0], optarg);
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
           case '?':
//                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
//                 return NULL;
           default:
             abort ();
           }
    
//        for(int i = 0; i < 10; i++)
//            if(args[i] != NULL)
//                printf("arg[%d]: %s\n", i, args[i]);
    
    return args;
}
    
//    int main(int argc, char** argv){
//        char** args = get_args(argc, argv);
//        for(int i = 0; i < 10; i++)
//            if(args[i] != NULL)
//                printf("arg[%d]: %s\n", i, args[i]);
//        
////        for(int j = 0; j < 10; j++)
////            free(args[j]);
//        free(args);
//        
//        
//        return 0;
//    }
