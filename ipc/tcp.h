#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define SERVER_QLEN 1

/******************************************************
Utilities to create a simple, low-level inter-process communication
system between machines over a TCP/IP network
 *****************************************************/

//create a server listening on port. Caller specifies an handler for
//the estabilished connection.
int makeserverh(uint16_t port, int (* handler)(int socket_des));

//simple forking server: no handler
int makeserver(uint16_t port);

//create a client connecting to address:port. Returns a socket descriptor
int makeclient(const char * address, uint16_t port);

//read data from socket descriptor
//put data on socket descriptor


//create a server socket and bind to port, exits on failure.
int make_socket (uint16_t port);

//initialize an address data structure (sockaddr_in) to be used in
//a client connection
void init_sockaddr (struct sockaddr_in * address,
		    const char * hostname,
		    uint16_t port);

//SIGCHLD handler
void sigchld_handler (int signum);
