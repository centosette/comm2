#include <stdio.h>
#include "utils.h"

int main (int argc, char ** argv)
{
  char buffer[512];
  tokenized_t * tokens;
  int i = 0;
  printf("Enter a string:\n");
  read(0, buffer, 512);
  tokens = tokenize(buffer);
  for (i = 0; i < tokens->num_tokens; i++)
    {
      printf("%s\n", tokens->tokens[i]);
    }
}
