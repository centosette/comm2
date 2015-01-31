/******************************************
packet-protocol.h
Defines application level packet structure and operations on subprotocols:
- listen for incoming connections or user instructions
- retrieve right module to be used depending on the required protocol
- connect the protocol module to the socket descriptor
 *****************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <features.h>

//General configuration

#define PROTO_LIBRARY_SIZE 4
#define PROTO_NAME_LENGTH 5
#define MAX_FILENAME_LENGTH FILENAME_MAX
#define BUFSIZE 512
#define CLOCK_ID CLOCK_MONOTONIC


//stereotype of a server side protocol module
typedef int (* proto_module_t) (int socket_descriptor);

//stereotype of a client side protocol module
typedef int (* client_module_t) (int socket_descriptor, const char * filename);


typedef struct
{
  char proto_name[PROTO_NAME_LENGTH];
  proto_module_t proto_module;
  client_module_t client_module;
} xprotocol_t;

xprotocol_t proto_library[PROTO_LIBRARY_SIZE];
int proto_library_members;
void proto_get_protocol_code (int socket_descriptor, char * proto_name);
proto_module_t proto_lookup (const char * proto_name);
client_module_t client_lookup(const char * proto_name);
void proto_init_library();

/**
****************************
PACKET DEFINITION
 ******************************/

/**
***************************
Packet layout
***************************


*/
#define SUBPROTOCOL_CODE_LEN #PROTO_NAME_LENGTH
#define CTRL_SIZE 3
#define BODY_SIZE 
#define BODY_MAX_SIZE 512

typedef struct
{
char subprotocol_code [SUBPROTOCOL_CODE_LEN]; 
} packet_header_t;

typedef struct
{
  char** sections;
  int num_sections;
  int* section_sizes;
  int body_len;
} packet_body_generic_t;

/************************
PROTOCOLS
 ************************/
//codes: PUT means "put a file on the server", GET "get a file from the server"
#define PLAIN_PUT "PP000"
#define PLAIN_GET "PG000"
#define MAOM_PUT "MAOMP"
#define MAOM_GET "MAOMG"


/*******************************
server modules
********************************/
//plain file transfer
int proto_plain_put (int socket_descriptor);  //PLAIN_PUT
int proto_plain_get (int socket_descriptor);      //PLAIN_GET

//Massey-Omura file transfer
int maom_get(int socket_descriptor);             //MAOM_GET
int maom_put(int socket_descriptor);             //MAOM_PUT

/**********************************
client modules
***********************************/
int client_plain_get (int socket_descriptor, const char * filename);
int client_plain_put (int socket_descriptor, const char * filename);

//Massey - Omura
int client_maom_get (int socket_descriptor, const char * filename);
int client_maom_put (int socket_descriptor, const char * filename);

/*******************************
SENDING AND RECEIVING COMMANDS FUNCTIONS
 ******************************/
int proto_expect_command (int socket_descriptor, const char * command);
int proto_send_command (int socket_descriptor, const char * command);
void proto_goodbye_failure (int socket_descriptor, const char * command);

/**************************
GENERAL COMMANDS AND RESPONSES
commands and responses are 3-digits integers. Range 100-400 is reserved.
each protocol may define its own response codes between 401 and 999.
 **************************/

//normal 200-100
#define OK "100"
#define FILENAME "110"
#define FILESIZE "115"
#define BEGIN_FILE_TRANSMISSION "120"
#define END_FILE_TRANSMISSION "130"

//warning 300-201
#define GENERAL_WARNING "300"
#define FILE_CUT_TO_DECLARED_SIZE "290"

//error 400-301
#define KO "400"
#define PROTOCOL_ERROR "390"
#define SERVER_ERROR "380"
#define FILENAME_LENGTH_ERROR "370"
#define FILE_NOT_FOUND "360"
#define CLIENT_ERROR "350"


/**********************************************
GENERAL UTILITY FUNCTIONS
***********************************************/
int clean_filename (char * filename);
int timespec_subtract (struct timespec * result, struct timespec * x, struct timespec * y);
