/********************************************
listener.c <--U protocol.h
listens for incoming connections and manages
protocol selection and
protocol-socket descriptor coupling
 ********************************************/
#include "config.h"

int listen_for_connections (uint16_t port)
{
  int sd;                          //socket descriptor
  int nread;                       //read bytes
  char buffer[PROTO_NAME_LENGTH];  //reading buffer

  proto_init_library();            //initialize protocol library
  sd = makeserver(port);           //obtain socket descriptor - forks process!
  proto_module_t module;           //module implementing protocol
  printf("Request is being served by PID %d\n", getpid());
  proto_get_protocol_code(sd, buffer);

  //the first PROTO_NAME_LENGTH bytes contain the desired protocol
  module = proto_lookup(buffer);
  if (module == NULL)
    {
      proto_goodbye_failure(sd, PROTOCOL_ERROR);
    }
  if (module(sd) < 0) {
    perror("listener.c:listen_for_connections:module");
    proto_goodbye_failure(sd, KO);
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
