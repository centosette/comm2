#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8000
#define QLEN 1
#define BUFSIZE 1500
int make_socket (uint16_t port);

int main (int argc, char ** argv)
{
  int sd;                            //socket descriptor
  fd_set active_fd_set, read_fd_set; //file/socket-descriptor sets
  struct sockaddr_in client;         //address of the connected client
  socklen_t size;                    //size of the sockaddr struct of the connected client, needed for accept.
  char * buffer [BUFSIZE];

  sd = make_socket(PORT);
  if (listen(sd, QLEN)<0)
    {
      perror("Failure while listening on port.");
      exit(EXIT_FAILURE);
    }
  //Initialize the set of active sockets
  FD_ZERO (&active_fd_set);     //set active_fd_set to the empty set
  FD_SET (sd, &active_fd_set);  //add sd to active_fd_set
  //Service cycle
  while(1)
    {
      read_fd_set = active_fd_set;
      if(select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
	{
	  perror("select");
	  exit(EXIT_FAILURE);
	} 
      int i;
      for (i = 0; i < FD_SETSIZE; i++)
	{
	  if (FD_ISSET(i, &read_fd_set))
	    {
	      if (i == sd) //connection request on the original socket
		{
		  int new_socket;
		  size = (socklen_t)sizeof(client);
		  if ((new_socket = accept(sd, (struct sockaddr *)&client, &size)) < 0)
		    {
		      perror("Error while accepting new connection\n");
		    }
		  else{ //socket successfully opened
		    fprintf(stderr, "Received connection from %s, port %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		    FD_SET(new_socket, &active_fd_set);
		  }
		}
	      else
		{ //incoming data from an already opened socket
		  int nread;
		  if ((nread = read(i, buffer, BUFSIZE-1)) < 0){
		    fprintf(stderr, "Error while reading from socket descriptor %d\n", i);
		    close(i);
		    FD_CLR(i, &active_fd_set);
		  }
		  else {
		    buffer[nread] = '\0';
		    fprintf(stdout, "Buffer from socket %d: %s\n", i, buffer);
		  }
		}
	    }
	}
    }

}

int make_socket (uint16_t port)
{
  int descriptor;
  struct sockaddr_in name;
  if ((descriptor = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("Error while creating socket");
      exit(EXIT_FAILURE);
    }
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(descriptor, (struct sockaddr *) &name, sizeof(name))<0)
    {
      perror("bind");
      exit(EXIT_FAILURE);
    }

 return (descriptor);

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
