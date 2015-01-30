/*******************************************//**
proto_maom.h <------ proto_maom.c
implements massey omura cryptosystem based
communication.
 *********************************************/
#include "proto_maom.h"

/***********************************************/
/**
This function implements the server side of the protocol when a file has to
be served on client request.
The protocol provides the following steps:
1) the client contacts the server and requires to get a file using massey-omura transfer mode
2) the server determines if the protocol is supported and if so replies with OK. Otherwise the
connection gets terminated with specific error code PROTOCOL_ERROR.
3) the client te
 */
/ ***********************************************/
int maom_get (int socket_descriptor)
{
  const FUNCNAME="maom_get()";
  //first, the server needs to know the filename
  if (proto_expect_command(socket_descriptor, FILENAME)!=0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Expected FILENAME command.");
    }
  
  //read first chunk of bytes and determine the prime number
  //send the prime number to the client
  //choose encoding exponent
  //calculate inverse
  //start encoding/sending/receiving/decoding/sending cycle
}

int maom_put (int socket_descriptor)
{
  //first, the server needs to know the prime number to be used
  //choose encoding exponent
  //calculate inverse
  //start receiving/encoding/sending/receiving/decoding cycle
}

int client_maom_get (int socket_descriptor, const char * filename)
{
}

int client_maom_put (int socket_descriptor, const char * filename)
{
}



/***************************************/
/**
The purpose of this function is to send a single big integer over a socket.
 - socket descriptor identifies the socket to be used
 - number represents the integer to be sent
 - mode is the protocol code that will precede the actual number.
				       */
/***************************************/
int send_number(int socket_descriptor
              , mpz_t number
              , const char * mode)
{
  const FUNCNAME = "send_number()";
  char * buffer, numlen;
  size_t bytes;
  //convert the big number into a stream of bytes
  mpz_2_bytes (buffer, number, &bytes);

  //the number of significant bytes we are going to transmit
  //must be expressed in a fixed-length string 
  if(zero_pad(numlen, bytes, INTEGER_SIZE_LEN) != INTEGER_SIZE_LEN)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Illegal number of zero-padded bytes\n");
    }

  //now we try and tell the other terminal the protocol code
  if (proto_send_command(socket_descriptor, mode) < 0)
    THROW(FUNCNAME, "Unable to send protocol code\n");

  //the other end should give an OK
  if (proto_expect_command(socket_descriptor, OK) != 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Unable to send protocol code\n");
    }
    
  //now we tell the oher end that a fixed size string
  //containing the length of the number to be transmitted
  //is about to be sent
  if (proto_send_command(socket_descriptor, INTEGER_SIZE) < 0)
    THROW(FUNCNAME, "Unable to send protocol code\n");

  //the other end should give an OK
  if (proto_expect_command(socket_descriptor, OK) != 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Unable to send protocol code\n");
    }
  //send the length
  if (write(socket_descriptor, numlen, INTEGER_SIZE_LEN) < 0)
    {
      proto_goodbye_failure(socket_descriptor, KO);
      THROW(FUNCNAME, "Unable to send integer size\n");
    }

  //the other end should give an OK
  if (proto_expect_command(socket_descriptor, OK) != 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Expecting OK from other end\n");
    }

  //now we write the actual bytes representing the integer to be sent
  if (write(socket_descriptor, buffer, bytes) < 0)
    {
      proto_goodbye_failure(socket_descriptor, KO);
      THROW(FUNCNAME, "Unable to send number\n");
    } 
  return(EXIT_SUCCESS);
}

int expect_number(int socket_descriptor
                , mpz_t * number
                , const char * mode)
{
  int nread, numlen;
  char buffer[BUFSIZE];
  //we should first of all receive the protocol code
  if (proto_expect_command(socket_descriptor, mode) != 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Unexpected protocol code or read error\n");
    }

  //send OK
  if (proto_send_command(socket_descriptor, OK) < 0)
    THROW(FUNCNAME, "Cannot write on socket descriptor\n");

  //now we expect to receive an INTEGER_SIZE instruction
  if (proto_expect_command(socket_descriptor, INTEGER_SIZE) != 0)
    {
      proto_goodbye_failure(socket_descriptor, PROTOCOL_ERROR);
      THROW(FUNCNAME, "Unexpected protocol code or read error\n");
    }

  //send OK
  if (proto_send_command(socket_descriptor, OK) < 0)
    THROW(FUNCNAME, "Cannot write on socket descriptor\n");

  //the other end should now give us the length
  if ((nread = read(socket_descriptor, buffer, INTEGER_SIZE_LEN)) < 0)
    {
      proto_goodbye_failure(socket_descriptor, KO);
      THROW(FUNCNAME, "Unable to read integer size\n");
    }

  //send OK
  if (proto_send_command(socket_descriptor, OK) < 0)
    THROW(FUNCNAME, "Cannot write on socket descriptor\n");

  buffer[INTEGER_SIZE_LEN] = '\0';
  numlen = atoi(buffer);

  //finally we should receive the actual bytes
  if ((nread = read(socket_descriptor, buffer, numlen)) < 0)
    {
      proto_goodbye_failure(socket_descriptor, KO);
      THROW(FUNCNAME, "Unable to read number\n");
    }

  bytes_2_mpz(number, buffer);
  return(EXIT_SUCCESS);

}
