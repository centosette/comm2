#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "bigint.h"
//#define INIT_SEED "3141592653589793238462643383279502884"
#define INIT_SEED "31415"
#define MENU "1. Standard test\n2. random list\n3. power and inverse\n4. to and from bytes\n5. 2 and fro bytes\n EXIT BY CTRL-C.\n"
#define MAKE_Y_CHOICE "Please select a menu item: "
#define YOU_KIDDING_ME "Are you kidding me??\n"
#define MAX_CHOICE 5
#define RANDOM_LEN 4
#define BUFLEN 256

typedef void (* menu_item_t) (void);

void sighandler (int signum)
{
  printf("\nCaught signal %d\n", signum);
  exit(EXIT_FAILURE);
}


void std_test()
{
  mpz_t this, that;
  mpz_init_set_str (that, INIT_SEED, 10);
  mpz_init(this);
  get_seed(this, that);
  gmp_printf("This is a seed:\n>> %Zd\nbigger than:\n>> %Zd\n", this, that);
  mpz_clear(this);
  mpz_init(this);
  get_prime(this, that);
  gmp_printf("This is a prime:\n>> %Zd\nbigger than:\n>> %Zd\n", this, that);
  get_rnd(this);
  gmp_printf("This is a random:\n>> %Zd\n", this);

  mpz_clear(this);
  mpz_clear(that);
}

void rnd_list()
{
  mpz_t ran;
  int i;
  mpz_init(ran);
  for (i = 0; i < RANDOM_LEN; i++)
    {
      get_rnd(ran);
      gmp_printf("%Zd\n", ran);
    }
}

void pow_inv()
{
mpz_t base, off, exp, iexp, p, p_1, inv, res;
mpz_inits(base, off, exp, iexp, p, p_1, inv, res, NULL);
  get_rnd(base);
get_rnd(off);
mpz_add(off, off, base);
get_prime(p, off);
mpz_sub_ui(p_1, p, 1);

get_rnd_inv(exp, iexp, p_1);

  get_pow(res, base, exp, p);
  gmp_printf("\nBASE: %Zd\nEXPONENT: %Zd\nMODULO: %Zd\nRESULT: %Zd\n",
	     base,
	     exp,
	     p, 
	     res);
  get_pow(inv, res, iexp, p);
  gmp_printf("\nBASE: %Zd\nEXPONENT: %Zd\nMODULO: %Zd\nRESULT: %Zd\n",
	     res,
	     iexp,
	     p, 
	     inv);
}

void to_fro_bytes()
{
  char buffer[BUFLEN];
  int nread;
  size_t words;
  mpz_t number;
  printf("Enter 32 bytes:\n");
  nread = read(STDIN_FILENO, buffer, BUFLEN);
mpz_init(number);
  bytes_to_mpz(number, buffer);
  gmp_printf("the bytes you entered form the subsequent number:\n %Zd\n", number);
  mpz_to_bytes(buffer, number, &words);
  buffer[(int)words * WORD_SIZE] = '\0';
  gmp_printf("And \n%Zd\n converts into \n%s\n, as we read only %d words\n",
	     number,
	     buffer,
	     words);

}

void two_fro_bytes()
{
  char * buffer;
  int nread;
  size_t words;
  mpz_t number;
  buffer = malloc(BUFLEN);
  printf("Enter how many bytes you like, max %d:\n", BUFLEN);
  nread = read(STDIN_FILENO, buffer, BUFLEN);
  mpz_init(number);
  bytes_2_mpz(number, buffer, nread);
  gmp_printf("the bytes you entered form the subsequent number:\n %Zd\n", number);
  mpz_2_bytes(buffer, number, &words);
  buffer = realloc(buffer, BUFLEN);
  buffer[(int)words] = '\0';
  gmp_printf("And \n%Zd\n converts into \n%s\n, as we read only %d bytes\n",
	     number,
	     buffer,
	     words);

}

int main (int argc, char ** argv)
{
  menu_item_t func[MAX_CHOICE] = {&std_test, &rnd_list, &pow_inv, &to_fro_bytes, &two_fro_bytes};
  signal(SIGINT, sighandler);
  int choice;
  while (1)
    {
      printf("%s", MENU);
      printf("%s", MAKE_Y_CHOICE);
      scanf("%d", &choice);
      //      printf("%d", choice);
      if (choice > MAX_CHOICE || choice <= 0)
	{
	  printf("%s", YOU_KIDDING_ME);
	}
      else
	{
	  printf("\n Executing test %d\n", choice);
	  func[choice-1]();
	}
    }
  
}

