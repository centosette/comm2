/*********************************
utils.h
useful generic utility functions
 ********************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
 typedef struct 
{
char ** tokens;
int num_tokens;
} tokenized_t;

typedef struct
{
  char * param_name;
  char * param_value;
} parameter_t;

void get_param_by_name (const char * param_name, char * param_value);


void halt(const char * func_name, const char * msg);
tokenized_t * tokenize (const char * string);

int zero_pad (char * buffer, unsigned long int number, int len);
