//
//  read_config_file.h
//  
//
//  Created by Gurden Angi on 2020-11-08.
//

#ifndef read_config_file_h
#define read_config_file_h

#include <stdio.h>
#include "server_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

int read_confg_file(server_config_t * server_cfgs);

#endif /* read_config_file_h */
