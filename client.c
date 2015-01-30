/**************************************
client.c <--- client.h
         |<--U protocol.h
provides user interface with an uniform 
way to use client modules.
 *************************************/
#include "client.h"

/*****************************************************************
connect to host:port, manage filename through protocol proto_name
******************************************************************/
int client_command (const char * host, uint16_t port, const char * filename, const char * proto_name)
{
  const char * _funcname = "client.c:client_command:";
  int socket_descriptor;
  client_module_t client_module;
  proto_init_library();
  client_module = client_lookup(proto_name);
  if (client_module == NULL)
    {
      halt(_funcname, "client_lookup()");
    }
  if ((socket_descriptor = makeclient(host, port)) < 0)
    {
      halt (_funcname, "makeclient()");
    }
  if( client_module(socket_descriptor, filename) < 0)
    {
      halt(_funcname, "client_module()");
    }
  return(0);

}
