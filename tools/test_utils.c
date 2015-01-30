#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define OFFSET 65000
#define LEN 6
int main (int argc, char ** argv)
{
  char * buffer;
  int len = LEN;
  int i;
  unsigned long int numbers[6]={1234, 25729, 65432, 3, 11111, 8954432};
  
  for(i = 0; i < 6; i++)
    {
      buffer = malloc(LEN);
      memset(buffer, '.', LEN);
      len = zero_pad(buffer, numbers[i], LEN);
      printf("\nThe n. %d has been zero padded to: \n ", numbers[i]);
      write(STDOUT_FILENO, buffer, len);
      write(STDOUT_FILENO, "\n", 1);
      free(buffer);
    }
  
}
