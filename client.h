/***************************
client.h

 ***************************/
#include <stdint.h>
#include "config.h"
int client_command (const char * host, uint16_t port, const char * filename, const char * proto_name);
