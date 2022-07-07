#define INTEGER_BYTES 32

#ifndef CRYPTO
//
// unsigned char *ERROR_CODES[2] = {"None", "Some Error"};

#define ERROR_CHECK \
    if (res != 0)   \
        printf("There was some error");

unsigned long mod_exp(unsigned long b, unsigned long e, unsigned long modulus);
unsigned long extended_gcd(unsigned long a, unsigned long b, unsigned long *x, unsigned long *y);
unsigned int mod(unsigned long a, unsigned int b);
char *int2bin(int a, char *buffer, int buf_size);

#endif