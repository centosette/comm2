/*************************
bigint.c <--- bigint.h
 *************************/
#include "bigint.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
void get_seed(mpz_t ret, mpz_t bigger_than)
{
  mpz_t seed;
  gmp_randstate_t rnd;
  mpz_init (seed);
  //initiate rnd state
  gmp_randinit_default(rnd);

  get_rnd(seed);
  gmp_randseed(rnd, seed);
  //get random number
  mpz_urandomm(seed, rnd, bigger_than);
  mpz_add(ret, seed, bigger_than);
  mpz_clear(seed);
  //now ret contains a random integer bigger than bigger_than
}

//get a random prime number, greater than "bigger_than"
void get_prime(mpz_t ret, mpz_t bigger_than)
{
  mpz_t seed;
  mpz_init(seed);
  get_seed(seed, bigger_than);
  //find the prime next to seed
  mpz_nextprime(ret, seed);
  mpz_clear(seed);
}

void get_rnd_n (mpz_t ret, int read_size)
{
  char buffer [read_size];
  unsigned long int nread, i, b;
  mpz_t temp, temp1;
  mpz_inits(temp, temp1, ret, NULL);
  mpz_set_ui (ret, 0);
  int dev_random;
  dev_random = open("/dev/urandom", O_RDONLY);
  nread = read(dev_random, buffer, read_size);
  for (i = 0; i < nread; i++)
    {
      mpz_ui_pow_ui(temp, 256, i);
      //gmp_printf("\n i = %d, temp = 256^i = %Zd\n", i, temp);
      b = char2uli(buffer[i]);
      mpz_mul_ui(temp1, temp, b);
      //gmp_printf("\n b = %d, i = %d, temp1 = temp * b =  %Zd\n",b, i, temp1);
      //gmp_printf("\n ret = %Zd\n", ret);
      mpz_set(temp, ret);
      mpz_add(ret, temp, temp1);
      //gmp_printf("\n ret' = ret + temp1 = %Zd\n", ret);
    }

}

void get_rnd (mpz_t ret)
{
  get_rnd_n(ret, READ_SIZE);
}


void get_pow(mpz_t ret, mpz_t base, mpz_t exp, mpz_t modulo)
{
  mpz_init(ret);
  mpz_powm(ret, base, exp, modulo);
}

int get_inverse(mpz_t ret, mpz_t op, mpz_t modulo)
{
  mpz_init(ret);
  return(mpz_invert(ret, op, modulo));
}

void mpz_to_bytes(char * buffer, mpz_t number, size_t * count)
{
  mpz_export(buffer, count, WORD_ORDER, WORD_SIZE, ENDIAN, NAILS , number);
}
void bytes_to_mpz(mpz_t number, char * buffer)
{
  mpz_import(number, WORD_COUNT, WORD_ORDER, WORD_SIZE, ENDIAN, NAILS, buffer);
}

int mpz_2_bytes (char * buffer, mpz_t number, size_t * count)
{
  mpz_t temp;
  unsigned long int remainder;
  int bufsize = 0;

  mpz_init(temp);
  mpz_set(temp, number);
  while(mpz_cmp_ui(temp, 0) > 0)
    {
      bufsize++;
      remainder =  mpz_tdiv_ui(temp, 256);
      mpz_tdiv_q_ui(temp,temp,256);
      buffer = realloc(buffer, sizeof(char)*bufsize);
      if (buffer == NULL) 
	{
	  mpz_clear(temp);
	  return (EXIT_FAILURE);
	}
      buffer[bufsize-1] = (char)remainder;
    }
  *count = bufsize;
  mpz_clear(temp);
  return(EXIT_SUCCESS);
}

int bytes_2_mpz (mpz_t number, char * buffer, size_t count)
{
mpz_t pwr;
mpz_inits(number, pwr, NULL);
  mpz_set_ui(number, 0);
  unsigned long int i;
  unsigned long int temp;


  for (i = 0; i < count; i++)
    {
      temp = char2uli(buffer[i]);
mpz_ui_pow_ui(pwr, 256, i);
mpz_mul_ui(pwr, pwr, temp);
      mpz_add(number, number, pwr);
    }
  return(EXIT_SUCCESS);
}

unsigned long int char2uli (const char c)
{
  unsigned char i;
  i = (unsigned char)c;
  return((unsigned long int) i);
}


//convert a big integer into its modulo n representation
void get_mod(mpz_t ret, mpz_t number, mpz_t modulo)
{
  mpz_init(ret);
  mpz_mod(ret, number, modulo);
}

//get a random number and its inverse modulo n
void get_rnd_inv(mpz_t rnd, mpz_t inv, mpz_t modulo)
{
  mpz_t temp;
  mpz_inits(rnd, inv, temp, NULL);
  while(1)
    {
      get_rnd(temp);
      //gmp_printf("temp = %Zd\n", temp);
      get_mod(rnd, temp, modulo);
      //gmp_printf("rnd = temp mod %Zd = %Zd\n", modulo, rnd);
      if(get_inverse(inv, rnd, modulo)) break;
      //gmp_printf("inv = %Zd\n", inv);
    }
  mpz_clear(temp);
}
