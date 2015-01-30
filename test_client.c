/**************************************
test_client.c <--- test_client.h
 **************************************/
#include "test_client.h"

void sigint_hdlr (int signum)
{
  signal (signum, sigint_hdlr);
  char res;
  printf("\nAre you sure you want to quit? ");
  scanf("%c", &res);
  if (res == 'Y' || res == 'y')
    exit(EXIT_SUCCESS);
}


void sigchld_handler (int signum)
{
  int status;
  signal (signum, sigchld_handler);
  if (waitpid(WAIT_ANY, &status, WNOHANG) < 0)
    perror("sigchld_handler: error\n");
}

int process (tokenized_t * cmdline)
{
  return (0);

}

int main (int argc, char ** argv)
{
  int nread;
  int i;
  int param_size;
  struct info command;

  
  signal (SIGINT, sigint_hdlr);
  signal(SIGCHLD, sigchld_handler);
  if (argc < 5 || argc > 6) {
    printf ("%s", USAGE_CMDLINE);
    exit (EXIT_FAILURE);
  }
  command.protocol_code = atoi(argv[1]);
  if (strcmp(argv[2], "put")== 0)
    {
      if (command.protocol_code == 0)
	{
	  strcpy(command.protocol_name, PLAIN_PUT);
	}
      else
	{
	  strcpy(command.protocol_name, MAOM_PUT);
	}
    }
  if (strcmp(argv[2], "get")== 0)
    {
      if (command.protocol_code == 0)
	{
	  strcpy(command.protocol_name, PLAIN_GET);
	}
      else
	{
	  strcpy(command.protocol_name, MAOM_GET);
	}
    }
  param_size = strlen(argv[3])+1;
  command.filename = (char *) malloc(sizeof(char) * param_size);
  strcpy(command.filename, argv[3]);

  param_size = strlen(argv[4])+1;
  command.hostname = (char *) malloc(sizeof(char)* param_size);
  strcpy(command.hostname, argv[4]);

  if (argc > 5) 
    {
      command.port = atoi(argv[5]);
    }
  else command.port = PORT;
  client_command(command.hostname, command.port, command.filename, command.protocol_name);


}

