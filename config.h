/****************************************
config.h
general configuration parameters
 ****************************************/
#include <string.h>
#include <stdlib.h>

#include "ipc/tcp.h" //provides facilities to easily create tcp client and server
#include "protocol.h" //protocol management
#include "utils.h"

#define CONFIG_FILE "./config.txt"
#define PORT 50000


/********************************************************************
config.h--------->|
                  |-> protocol.h
                  |-> 
                  |-> utils.h
                  |-> ipc/tcp.h
                  |-> 
*********************************************************************/
