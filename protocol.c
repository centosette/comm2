
/*****************************************
protocol.c <--- protocol.h
implements basic protocol facilities
 *****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"

void proto_init_library()
{
  proto_library_members = PROTO_LIBRARY_SIZE;
  proto_module_t modules[PROTO_LIBRARY_SIZE] = {&proto_plain_get, &proto_plain_put, NULL, NULL};
  client_module_t c_modules[PROTO_LIBRARY_SIZE] = {&client_plain_get, &client_plain_put, NULL, NULL};
  char names[PROTO_LIBRARY_SIZE][PROTO_NAME_LENGTH] = {PLAIN_GET,PLAIN_PUT,MAOM_GET,MAOM_PUT};
  int i;
  for(i = 0; i < PROTO_LIBRARY_SIZE; i++)
    {
      strcpy(proto_library[i].proto_name, names[i]);
      proto_library[i].proto_module = modules[i];
      proto_library[i].client_module = c_modules[i];
    }
  
}

void proto_get_protocol_code (int socket_descriptor, char * proto_name)
{
  int i;  
  int nread;
  int total_read = 0;
  char buffer[BUFSIZE];
  //char readbytes[PROTO_NAME_LENGTH];
  int filesize;
  while (total_read < PROTO_NAME_LENGTH)
    {
      if((nread = read(socket_descriptor, buffer, BUFSIZE)) < 0)
	{
	  perror("protocol.c:proto_get_protocol_code():read error");
	  proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
	}
      if ((total_read + nread) <= PROTO_NAME_LENGTH) 
	{
	  memcpy(&proto_name[total_read], buffer, nread);
	}
      else {
	memcpy(&proto_name[total_read], buffer, PROTO_NAME_LENGTH-total_read);
	if (write(2, proto_name, PROTO_NAME_LENGTH) < 0)
	  {
	    perror("Unable to write proto_name");
	  }
      }

      total_read += nread;

    }
  
}

proto_module_t proto_lookup (const char * proto_name)
{
  int i;
  for (i = 0; i < proto_library_members; i++)
    {
      if (memcmp(proto_library[i].proto_name, proto_name, PROTO_NAME_LENGTH) == 0)
	return(proto_library[i].proto_module);
    }
  return NULL;
}

client_module_t client_lookup(const char * proto_name)
{
int i;
  for (i = 0; i < proto_library_members; i++)
    {
      if (memcmp(proto_library[i].proto_name, proto_name, PROTO_NAME_LENGTH) == 0)
	return(proto_library[i].client_module);
    }
  return NULL;
}

int proto_expect_command (int socket_descriptor, const char * command)
{
  int i;  
  int nread;
  int total_read = 0;
  char buffer[BUFSIZE];
  char readbytes[CTRL_SIZE];
  int filesize;
  while (total_read < CTRL_SIZE)
    {
      if((nread = read(socket_descriptor, buffer, BUFSIZE)) < 0)
	{
	  perror("protocol.c:proto_expect_command():read error");
	  proto_send_command(socket_descriptor, SERVER_ERROR);
	  close (socket_descriptor);
	  return (-1);
	}
      if ((total_read + nread) <= CTRL_SIZE) 
	{
	  memcpy(&readbytes[total_read], buffer, nread);
	}
      else {
	memcpy(&readbytes[total_read], buffer, CTRL_SIZE-total_read);
      }

      total_read += nread;

    }
  fprintf(stderr, "received: ");
  write(2, readbytes, total_read);
  fprintf(stderr, "\n");
  fprintf(stderr, "expected: ");
  write(2, command, CTRL_SIZE);
  fprintf(stderr, "\n");
  if (memcmp(readbytes, command, CTRL_SIZE) == 0)
    {
      return(0);
    }
  else return (1);
  
}

int proto_send_command (int socket_descriptor, const char * command)
{
  if (write(socket_descriptor, command, CTRL_SIZE) < 0)
    {
      perror("protocol.c:proto_send_command():write failed");
      close(socket_descriptor);
      return(-1);
    }
  fprintf(stderr, "sending: ");
  write(2, command, CTRL_SIZE);
  fprintf(stderr, "\n");
  fsync(socket_descriptor);
  return(0);
}

void proto_goodbye_failure (int socket_descriptor, const char * command)
{
  proto_send_command(socket_descriptor, command);
  shutdown (socket_descriptor, 2);
  exit(EXIT_FAILURE);
}

int read_filename(int socket_descriptor, int * read_bytes, char * filename)
{
  int total_read=0, nread=0;
  char buffer [BUFSIZE];
  const int ERROR = 1;
  const int SUCCESS = 0;
  
  while(total_read < MAX_FILENAME_LENGTH) {
    nread = read(socket_descriptor, buffer, BUFSIZE);
    if (nread < 0) {
      return ERROR;
    }
    else
      {
	if ((total_read + nread) >= MAX_FILENAME_LENGTH)
	  {
	    return ERROR;
	  }
	else
	  {
	    memcpy(&filename[total_read], buffer, nread);
	  }
	total_read += nread;
	if(nread < BUFSIZE) break;
      }
  }
  filename[total_read] = '\0';
  *read_bytes = total_read;
  clean_filename(filename);
  return SUCCESS;
}

/*****************************************************
Server plain modules
 *****************************************************/
//PLAIN PUT: accept file from the client
int proto_plain_put (int socket_descriptor)  //PLAIN_PUT
{
  int nread = 0;
  int total_read = 0;
  int fd;    //local file descriptor
  int response;
  size_t filesize;
  char buffer[BUFSIZE];
  char filename[MAX_FILENAME_LENGTH];
  struct timespec start, end, elapsed;
  double elapsed_seconds;
  double transfer_rate;
  //tell the client we are ready to talk

  proto_send_command(socket_descriptor, OK);
  clock_gettime(CLOCK_ID, &start);  

  //now we expect to receive a FILENAME directive
  if ((response = proto_expect_command(socket_descriptor, FILENAME)) < 0) 
    {
      perror("protocol.c:proto_plain_receive:read from socket failed");
      proto_goodbye_failure(socket_descriptor, KO);
    }
  else if (response > 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
    }
  else //we received a FILENAME directive: now we want the actual filename.
    {
      proto_send_command(socket_descriptor, OK);
      //-----------------------------------------------
      if(read_filename(socket_descriptor, &nread, filename) != 0)
	{
	  proto_goodbye_failure(socket_descriptor, FILENAME_ERROR);
	}
      fprintf(stderr, "Ready to receive: \n");     
      write(2, filename, nread);
      fprintf(stderr, "\n");
      if ((fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU)) < 0)
	{
	  perror("protocol.c:proto_plain_receive:error creating local file");
	  proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
	}
      proto_send_command(socket_descriptor, OK);
      if((response=proto_expect_command(socket_descriptor, FILESIZE)) < 0)
	{
	  proto_goodbye_failure(socket_descriptor, KO);
	}
      else if (response > 0)
	{
	  proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
	}
      else
	{
	  proto_send_command(socket_descriptor, OK);
	  //read the actual filesize
	  if ((nread = read(socket_descriptor, buffer, BUFSIZE-1)) < 0)
	    {
	      proto_goodbye_failure(socket_descriptor, KO);
	    }
	  buffer[nread] = '\0';
	  filesize = atoi(buffer);
	  total_read = 0;
	  fprintf(stderr, "Actually receiving %d bytes of  file...\n", filesize);
	  proto_send_command(socket_descriptor, OK);
	  while (total_read < filesize)
	    {
	      if ((nread = read(socket_descriptor, buffer, BUFSIZE)) < 0)
		{
		  proto_goodbye_failure(socket_descriptor, KO);
		}
	      total_read += nread;
	      if (write(fd, buffer, nread) < 0)
		{
		  perror("protocol.c:proto_plain_receive():error writing local file\n");
		  proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
		}
	    }
	  close(fd);
	  fprintf(stderr, "File written to disk\n");
	  if(total_read > filesize)
	    {
	      proto_send_command(socket_descriptor, FILE_CUT_TO_DECLARED_SIZE);
	    }
	  else
	    {
	      proto_send_command(socket_descriptor, END_FILE_TRANSMISSION);
	    }
	  shutdown(socket_descriptor, 2);
	}
      clock_gettime(CLOCK_ID, &end);
      timespec_subtract(&elapsed, &end, &start);
      elapsed_seconds = elapsed.tv_sec + elapsed.tv_nsec/1000000000;
      transfer_rate = (double) total_read/elapsed_seconds;
      printf("%d bytes in %f seconds: %f B/s.\n", total_read, elapsed_seconds, transfer_rate);
	
      exit(EXIT_SUCCESS);
    }
   
}


//PLAIN GET: send the requested file to the client
int proto_plain_get(int socket_descriptor)      //PLAIN_GET
{
  int nread; //read bytes counter
  int total_read = 0;
  int fd;    //local file descriptor
  FILE * stream; //local file stream wrapper
  int response;
  size_t filesize;
  char buffer[BUFSIZE];
  char filename[MAX_FILENAME_LENGTH];
  struct timespec start, end, elapsed;
  double elapsed_seconds;
  double transfer_rate;
  struct stat attributes;
  //tell the client we are ready to talk

  proto_send_command(socket_descriptor, OK);
  clock_gettime(CLOCK_ID, &start);  

  //now we expect to receive a FILENAME directive
  if ((response = proto_expect_command(socket_descriptor, FILENAME)) < 0) 
    {
      perror("protocol.c:proto_plain_get:read from socket failed");
      proto_goodbye_failure(socket_descriptor, KO);
    }
  else if (response > 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
    }
  else //we received a FILENAME directive: now we want the actual filename.
    {
      if((read_filename, socket_descriptor, &nread, filename) != 0)
	{
	  proto_goodbye_failure(socket_descriptor, FILENAME_ERROR);
	}
      write(2, filename, nread);
      if ((fd = open(filename, O_RDONLY)) < 0)
	{
	  perror("protocol.c:proto_plain_get:error reading local file");
	  if(errno = ENOENT)
	    {
	      proto_goodbye_failure(socket_descriptor, FILE_NOT_FOUND);
	    }
	  else
	    {
	      proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
	    }
	}
      stream = fdopen(socket_descriptor, "w");
      stat(filename, &attributes);
      filesize = (int)attributes.st_size;
      proto_send_command(socket_descriptor, OK);
      proto_send_command(socket_descriptor, FILESIZE);
      if((response=proto_expect_command(socket_descriptor, OK)) < 0)
	{
	  proto_goodbye_failure(socket_descriptor, KO);
	}
      else if (response > 0)
	{
	  proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
	}
      else
	{
	  fprintf(stream, "%d", filesize);
	  fflush(stream);
	  
	  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
	    {
	      proto_goodbye_failure(socket_descriptor, KO);
	    }
	  if (response > 0)
	    {
	      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
	    }
	  //BEGIN FILE TRANSMISSION
	  total_read = 0;
	  while ((nread = read(fd, buffer, BUFSIZE)) > 0)
	    {
	      if ((response = write(socket_descriptor, buffer, nread)) < 0)
		{
		  proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
		}
	      total_read += nread;
	      
	    }
	  if(total_read > filesize)
	    {
	      proto_goodbye_failure(socket_descriptor, SERVER_ERROR);
	    }
	  else
	    {
	      if ((response = proto_expect_command(socket_descriptor, END_FILE_TRANSMISSION)) != 0)
		{
		  proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
		}
	    }
	  shutdown(socket_descriptor, 2);
	    
	  close(fd);
	}
      clock_gettime(CLOCK_ID, &end);
      timespec_subtract(&elapsed, &end, &start);
      elapsed_seconds = elapsed.tv_sec + elapsed.tv_nsec/1000000000;
      transfer_rate = (double) total_read/elapsed_seconds;
      printf("%d bytes in %f seconds: %f B/s.\n", total_read, elapsed_seconds, transfer_rate);
	
      exit(EXIT_SUCCESS);
    }
   
}




/********************************************************
Client plain functions
 ********************************************************/

int client_plain_put(int socket_descriptor, const char * filename)
{
  const char * funcname = "protocol.c:client_plain_put:";
  int response;
  int fd; //local file descriptor
  FILE * socket_stream; //wrapper for sending formatted output on the socket
  int filesize;
  struct stat attributes;
  char buffer[BUFSIZE];
  int nread;

  write(socket_descriptor, PLAIN_PUT, PROTO_NAME_LENGTH);
  fsync(socket_descriptor);
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
    {
      halt(funcname, "while expecting OK from server");
    }
  proto_send_command(socket_descriptor, FILENAME);
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
    {
      halt(funcname, "while expecting OK from server after FILENAME");
    }
  write(socket_descriptor, filename, strlen(filename));
  fsync(socket_descriptor);
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
    {
      halt(funcname, "while expecting OK from server after sending actual filename");
    }
  proto_send_command(socket_descriptor, FILESIZE);
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
    {
      halt(funcname, "while expecting OK from server after FILESIZE");
    }
  if ((fd = open(filename, O_RDONLY)) < 0)
    {
      perror("Error while opening local file");
      proto_goodbye_failure(socket_descriptor, CLIENT_ERROR);
    }
  if ((socket_stream = fdopen(socket_descriptor, "w")) == NULL)
    {
      perror("Error while opening stream over socket descriptor");
      proto_goodbye_failure(socket_descriptor, CLIENT_ERROR);
    }
  if (stat(filename, &attributes) < 0)
    {
      perror("Error while stat'ing the local file");
      proto_goodbye_failure(socket_descriptor, CLIENT_ERROR);
    }
  filesize = (int) attributes.st_size;
  fprintf(socket_stream, "%d", filesize);
  fprintf(stderr, "File size: %d bytes\n", filesize);
  fflush(socket_stream);
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0)
    {
      halt(funcname, "while expecting OK from server after sending actual size");
    }
  fprintf(stderr, "Actually sending file...\n");
  while((nread = read(fd, buffer, BUFSIZE)) > 0)
    {
      write(socket_descriptor, buffer, nread);
      fsync(socket_descriptor);
    }
  fprintf(stderr, "File sent.\n");
  if ((response =  proto_expect_command(socket_descriptor, END_FILE_TRANSMISSION)) < 0)
    {
      halt(funcname, "Waiting for server END_FILE_TRANSMISSION message");
    }
  if(response == 0)
    {
      exit(EXIT_SUCCESS);
    }
  exit(EXIT_FAILURE);

}

int client_plain_get (int socket_descriptor, const char * filename)
{
  //FIXIT: server side module copied: must be corrected to fit client side

  int nread; //read bytes counter
  int total_read = 0;
  int fd;    //local file descriptor
  int response;
  size_t filesize;
  char buffer[BUFSIZE];

  struct timespec start, end, elapsed;
  double elapsed_seconds;
  double transfer_rate;
  const char * funcname = "protocol.c:client_plain_get:";
  //tell the server we want to get a file

  clock_gettime(CLOCK_ID, &start);
  write(socket_descriptor, PLAIN_GET, PROTO_NAME_LENGTH);
  fsync(socket_descriptor);

  //now we expect to receive an OK
  if ((response = proto_expect_command(socket_descriptor, OK)) < 0) 
    {
       halt(funcname, "while expecting OK from server");
    }
  else if (response > 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
    }
  else //we send a FILENAME directive.
    {
      proto_send_command(socket_descriptor, FILENAME);
      /* if ((response = proto_expect_command(socket_descriptor, OK)) < 0)  */
      /* 	{ */
      /* 	  halt(funcname, "while expecting OK from server"); */
      /* 	} */
      /* else if (response > 0) */
      /* 	{ */
      /* 	  proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR); */
      /* 	} */
      write(socket_descriptor, filename, strlen(filename));
      fsync(socket_descriptor);
      if ((response = proto_expect_command(socket_descriptor, OK)) != 0)
	{
	  halt(funcname, "while expecting OK from server after sending actual filename");
	}
      fprintf(stderr, "Ready to receive %s: \n", filename);     
      if ((fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU)) < 0)
	{
	  perror("protocol.c:client_plain_get:error creating local file");
	  proto_goodbye_failure(socket_descriptor, CLIENT_ERROR);
	}
      //      proto_send_command(socket_descriptor, OK);
      if((response=proto_expect_command(socket_descriptor, FILESIZE)) < 0)
	{
	  proto_goodbye_failure(socket_descriptor, KO);
	}
      else if (response > 0)
	{
	  proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
	}
      else
	{
	  proto_send_command(socket_descriptor, OK);
	  //read the actual filesize
	  if ((nread = read(socket_descriptor, buffer, BUFSIZE-1)) < 0)
	    {
	      proto_goodbye_failure(socket_descriptor, KO);
	    }
	  buffer[nread] = '\0';
	  filesize = atoi(buffer);
	  total_read = 0;
	  fprintf(stderr, "Actually receiving %d bytes of  file...\n", filesize);
	  proto_send_command(socket_descriptor, OK);
	  while (total_read < filesize)
	    {
	      if ((nread = read(socket_descriptor, buffer, BUFSIZE)) < 0)
		{
		  proto_goodbye_failure(socket_descriptor, KO);
		}
	      total_read += nread;
	      if (write(fd, buffer, nread) < 0)
		{
		  perror("protocol.c:client_plain_get():error writing local file\n");
		  proto_goodbye_failure(socket_descriptor, CLIENT_ERROR);
		}
	    }
	  close(fd);
	  fprintf(stderr, "File written to disk\n");
	  if(total_read > filesize)
	    {
	      proto_send_command(socket_descriptor, FILE_CUT_TO_DECLARED_SIZE);
	    }
	  else
	    {
	      proto_send_command(socket_descriptor, END_FILE_TRANSMISSION);
	    }
	  shutdown(socket_descriptor, 2);
	}
      clock_gettime(CLOCK_ID, &end);
      timespec_subtract(&elapsed, &end, &start);
      elapsed_seconds = elapsed.tv_sec + elapsed.tv_nsec/1000000000;
      transfer_rate = (double) total_read/elapsed_seconds;
      printf("%d bytes in %f seconds: %f B/s.\n", total_read, elapsed_seconds, transfer_rate);
	
      exit(EXIT_SUCCESS);
    }

}


int clean_filename (char * filename)
{
  int i;
  for (i = 0; i < MAX_FILENAME_LENGTH; i++)
    {
      if(iscntrl(filename[i]))
	{
	  filename[i] = '\0';
	  return(i+1);
	}
    }
  return (i+1);
}

/* Subtract the `struct timespec' values X and Y,
        storing the result in RESULT.
        Return 1 if the difference is negative, otherwise 0. */
     
int timespec_subtract (result, x, y) struct timespec *result, *x, *y;
     {
       /* Perform the carry for the later subtraction by updating y. */
       if (x->tv_nsec < y->tv_nsec) {
         int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
         y->tv_nsec -= 1000000000 * nsec;
         y->tv_sec += nsec;
       }
       if (x->tv_nsec - y->tv_nsec > 1000000000) {
         int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
         y->tv_nsec += 1000000000 * nsec;
         y->tv_sec -= nsec;
       }
     
       /* Compute the time remaining to wait.
          tv_nsec is certainly positive. */
       result->tv_sec = x->tv_sec - y->tv_sec;
       result->tv_nsec = x->tv_nsec - y->tv_nsec;
     
       /* Return 1 if result is negative. */
       return x->tv_sec < y->tv_sec;
     }
