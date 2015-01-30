/*********************************
utils.c <--- utils.h
 ********************************/
#include "utils.h"
void halt (const char * funcname, const char * msg)
{
  char * message;
  message = (char *) malloc (strlen(funcname)+strlen(msg)+1);
  memcpy(message, funcname, strlen(funcname));
  memcpy(&message[strlen(funcname)], msg, strlen(msg));
  perror(message);
  exit(EXIT_FAILURE);
}

tokenized_t * tokenize (const char * string)
{
  int total_length;
  int token_size;
  int token_number = 0;
  int memory = 0;
  int i, j;
  char * buffer;

  tokenized_t * tokenized;


  total_length = strlen(string);

  memory = sizeof(tokenized_t);
  i = 0;

  tokenized = (tokenized_t*) malloc (memory);
  tokenized->num_tokens = 0;

  while(i < total_length)
    {
      token_size = 0;
      for (j = i; j < total_length; j++)
	{
	  if (iscntrl(string[j]) || isspace(string[j]))
	    {
	      if(token_size > 0)
		{
		  break;
		}
	      else {
		i = j;
	      }
	    }
	  else
	    {
	      token_size++;
	    }
	}
      buffer = (char *) malloc (sizeof(char) * token_size+1);
      for (j = 0; j < token_size; j++)
	{
	  buffer[j] = string[i];
	  i++;
	}
      buffer[token_size] = '\0';

      token_number++;
      memory = sizeof(char *) * (tokenized->num_tokens+1);
      tokenized->tokens = (char **) realloc (tokenized->tokens, memory);

      memory = sizeof(char) * (token_size + 1);
      tokenized->tokens[tokenized->num_tokens] = (char *) malloc (memory);

      memcpy(tokenized->tokens[tokenized->num_tokens], buffer, token_size + 1);
      tokenized->num_tokens = token_number;
      i++;
    }
  return(tokenized);

}

int zero_pad(char * buffer, unsigned long int number, int len)
{
  //allocate a buffer of length len
  buffer = (char *) realloc(buffer, sizeof(char)*len);

  int i = 0, j = 0;
  int remainder;
  unsigned long int quotient = number;
  int real_len = len;
  char digit;
  while (quotient > 0)
    {
      //if (quotient == 0) break;
      remainder = quotient%10;
      quotient = quotient/10;
      digit = (char) (remainder + 48);
      i++; //i is the number of converted digits

      if (i > real_len)  //if we converted more digits than expected...
	{
	  //...realloc buffer, shift chars right
	  real_len++;
	  buffer = (char *) realloc (buffer, sizeof(char)*real_len);
	  for (j = real_len-1; j > 0; j--)
	    {
	      buffer[j] = buffer[j-1];
	    }
	}
      buffer[real_len - i] = digit;
    } 
  //finally, do the padding
  for (j = 0; j < (real_len - i); j++)
    {
      buffer[j] = '0';
    }
  return (real_len);
}
