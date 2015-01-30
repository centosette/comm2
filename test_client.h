/*****************************************
test_client.h
******************************************/
#include <signal.h>

#include "client.h"

#define COMMAND_LINE_LEN 512
#define PARAMETER_LEN 256
#define USAGE_TRANSFER "#> <put|get> <filename> <host> [port]\n"
#define USAGE_PROTOCOL "#> proto <protocol_code>\n"
#define USAGE_CMDLINE "<proto> <put|get> <filename> <host> [port]\n"
#define PROMPT "#> " 
#define PLAIN 0
#define MAOM 1


struct info
{
  int protocol_code;
  char protocol_name[PROTO_NAME_LENGTH];
  char * hostname;
  char * filename;
  int port;
};
