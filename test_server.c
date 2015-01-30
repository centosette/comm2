/*******************
test_server.c
simple server
 ******************/
#include "config.h"

void inthandler (int signal);
void inthandler (int signal)
{
  perror("Exiting program");
  exit(EXIT_SUCCESS);
}

int main (int argc, char ** argv)
{
  signal(SIGINT, inthandler);
  listen_for_connections (PORT);
  return(0);
}
