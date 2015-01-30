#include <gmp.h> //GNU Multiple Precision library

///Default number of bytes that will be read from /dev/urandom when generating random numbers
#define READ_SIZE 8
//import-export constant values
#define WORD_COUNT 8
#define WORD_ORDER 1 //1 = most significant byte first
#define WORD_SIZE 4
#define ENDIAN 1 // 1 = most significant byte first
#define NAILS 0

///ret is a random number bigger than bigger_than
void get_seed(mpz_t ret, mpz_t bigger_than);

///ret is a prime number bigger than bigger_than
void get_prime(mpz_t ret, mpz_t bigger_than);

///ret is a random number generated from /dev/urandom
void get_rnd(mpz_t ret);

///same as get_rnd, but specify how many bytes of urandom have to be read
void get_rnd_n (mpz_t ret, int read_size);

///ret is the result of the xponentiation of base to exp modulo modulo
void get_pow(mpz_t ret, mpz_t base, mpz_t exp, mpz_t modulo);

///ret is the inverse of operand op modulo modulo
int get_inverse(mpz_t ret, mpz_t op, mpz_t modulo);

///convert integer number to count plain bytes stored in buffer
void mpz_to_bytes(char * buffer, mpz_t number, size_t * count);

///convert bytes of buffer into integer, stored in number
void bytes_to_mpz(mpz_t number, char * buffer);

///custom edition of mpz_to_bytes
int mpz_2_bytes (char * buffer, mpz_t number, size_t * count);

///custom edition of bytes_to_mpz
int bytes_2_mpz (mpz_t number, char * buffer, size_t count);



///convert char into unsigned long int
unsigned long int char2uli (const char c);

///convert a big integer into its modulo n representation
void get_mod(mpz_t ret, mpz_t number, mpz_t modulo);

///get a random number rnd and its inverse inv modulo modulo
void get_rnd_inv(mpz_t rnd, mpz_t inv, mpz_t modulo);
