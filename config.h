//
//  config.h
//  
//
//  Created by Gurden Angi on 2020-11-07.
//

#ifndef config_h
#define config_h

#include <stdio.h>
#include "read_args.h"
#include "read_config_file.h"
#include "read_defaults.h"
#include "server_config.h"
#include <stdio.h>
#include <stdlib.h>

int get_config(server_config_t * server_cfgs, int argc, char ** argv);

#endif /* config_h */
