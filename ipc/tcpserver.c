#include "tcp.h"

/***************************************************************
tcpserver.c
Forks at each connection and provides the caller with 
a socket descriptor. A signal handler provides waiting for
children's exit status.
 ***************************************************************/
void sigchld_handler (int signum)
{
  int status;
  signal (signum, sigchld_handler);
  if (waitpid(WAIT_ANY, &status, WNOHANG) < 0)
    perror("sigchld_handler: error\n");
}

int makeserver (uint16_t port)
{
  int sock;                           //main listening socket
  sock = make_socket(port);
  if (listen(sock, SERVER_QLEN) < 0)
    {
      perror("Listen failed. Exiting.");
      exit(EXIT_FAILURE);
    }
  signal(SIGCHLD, sigchld_handler); //estabilish handler for managing SIGCHLD
  while(1)
    {
      struct sockaddr_in client;
      int sd;
      int size;
      size = sizeof(client);
      if ((sd = accept(sock, (struct sockaddr *)&client, &size)) < 0)
	{
	  perror("Error while accepting connection");
	}
      else
	{
	  pid_t pid;
	  if((pid = fork())==0) //we are in the child process
	    {
	      close(sock); //should not interfere with the parent copy
	      return (sd);
	    }
	}
    }

}

/* void makeserverh (uint16_t port, int (* handler)(int)) */
/* { */
  
/*   FD_ZERO (sd_set);   */
/*   int sd;                  //main socket descriptor */
/*   int size;                //sizeof(client) */
/*   //  fd_set * active_ds; */
/*   struct sockaddr_in client; */
/*   sd = make_socket(port); */
/*   size = sizeof(client); */
/*   if (listen(sd) < 0) */
/*     { */
/*       perror("makeserverfs: listen()"); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*   FD_SET (sd, sd_set); */
/*   while (1) */
/*     { */
/*       int newsd; //new socket descriptor */
/*       if (select (FD_SETSIZE, sd_set, NULL, NULL, NULL) < 0) */
/* 	{ */
/* 	  perror("makeserverfs: select()"); */
/* 	  exit(EXIT_FAILURE); */
/* 	} */
/*       int i; */
/*       for (i = 0; i<FD_SETSIZE; i++) */
/* 	{ */
/* 	  if (FD_ISSET(i, sd_set)) */
/* 	    { */
/* 	      if (i == sd) //the main socket is active and receiving connection request */
/* 		{ */
/* 		  if ((newsd = accept(sd, (struct sockaddr *)&client, &size)) < 0)  */
/* 		    { */
/* 		      perror("makeserverfs: accept()"); */
/* 		    } */
/* 		  else { */
/* 		    //FD_SET(newsd, active_ds); */
/* 		    FD_SET(newsd, sd_set); */
/* 		    if ((* handler)(newsd); */
/* 		  } */
/* 		} */
/* 	      else { */
/* 		//another socket is receiving input */
/* 	      } */

/* 	    } */
/* 	} */


/*     } */
/* } */

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
