#include "tcp.h"

/********************************************************
tcpclient.c
provides a basic tcp client. Returns a file descriptor 
for the estabilished connection.
 *******************************************************/
int makeclient(const char * address, uint16_t port)
{
  int sd;
  struct sockaddr_in server;
  init_sockaddr (&server, address, port);
  fprintf(stderr, "Hostname: %s, Port: %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
  if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("makeclient: socket()");
      exit(EXIT_FAILURE);
    }
  if (connect(sd, (struct sockaddr *) &server, sizeof(server))<0)
    {
      perror("makeclient: connect()");
      exit(EXIT_FAILURE);
    }
  return(sd);
}

void init_sockaddr (struct sockaddr_in * address, const char * hostname, uint16_t port)
{
  struct hostent * hostinfo;
  address->sin_family = AF_INET;
  address->sin_port = htons(port); 
  hostinfo = gethostbyname(hostname);
  if (hostinfo == NULL) {
    fprintf(stderr, "Error while resolving %s.", hostname);
    exit(EXIT_FAILURE);
  }
  address->sin_addr = *(struct in_addr *)hostinfo->h_addr;

}
