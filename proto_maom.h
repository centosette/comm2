/***************************//**
proto_maom.h <---U protocol.h
implements massey-omura 
transfer protocol.

The INITIATOR estabilishes the connection and determines
the parameters that will be used during the communication.
Given the maximum number of bytes that can be transmitted
in a single chunk of data, it is possible to determine how
big at least must the prime P be.
The lower bound should be LB = 2 ^ (BUFSIZE * 8) - 1, where BUFSIZE is
a constant value defined in protocol.h and represents the
max size of the read buffer.
As BUFSIZE may be a relatively big number to be used as an 
exponent (e.g. BUFSIZE = 512 -> LB = 2 ^ 4096), it is necessary
to define a much smaller max number of read bytes, such as 32.
This means that a maximum of 32 bytes at a time can be read from
any file the program is going to transfer.
If this is the case, P will be greater than 2^(32 * 8) = 2 ^ 256.
			     */
/**************************************************************
----------------------------------------------------------
INITIATOR                                    RESPONDER

|choose prime P|
       |
|communicate P|--------------------------->|receive P|
       |                                        |
|choose Ei in Z*(P-1)|                     |choose Er in Z*(P-1)|
       |                                        |
|calculate Di = Ei^(-1)|                   |calculate Dr = Er^(-1)|
       |                                        |
       +----------------[cycle]-----------------+
       |                                        |
|compute C = M^(Ei) mod(P)|                     |
       |                                        |
|send C to RESPONDER|--------------------->|receive C|
       |                                        |
       |                                   |compute D = C^(Er) mod(P)|
       |                                        |
|receive D|<-------------------------------|send D back|
       |                                        |
|compute E = D^(Di) mod(P)|                     |
       |                                        |
|send E to RESPONDER|--------------------->|receive E|
       |                                        |
       |                                   |compute M = E^(Dr) mod(P)|
       +----------------[cycle]-----------------+
      //                                       //
----------------------------------------------------------
 *****************************/
#include <gmp.h>
#include "config.h"
#include "bigint.h"
#include "protocol.h"
#define MAOM_BUFSIZE 32
#define DEF_BIGGER_THAN 

//protocol control messages
#define PRIME "503"
#define ENCODED_A "510"
#define ENCODED_B "520"
#define ENCODED_C "530"
#define INTEGER_SIZE "500"

#define INTEGER_SIZE_LEN 3

//function names
#define MODULE_NAME "proto_maom.c:"
#define THROW (FUNCNAME, MESSAGE) {halt (#MODULE_NAME ## #FUNCNAME, #MESSAGE)} 

typedef struct 
{
  mpz_t prime;
  mpz_t encode_key;
  mpz_t decode_key;
} maom_kit_t;

mpz_t get_start_prime (mpz_t bigger_than);
