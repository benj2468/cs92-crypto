#define INTEGER_BYTES 32

#ifndef CRYPTO
//
// unsigned char *ERROR_CODES[2] = {"None", "Some Error"};

#define ERROR_CHECK \
    if (res != 0)   \
        printf("There was some error");

unsigned int mod_exp(unsigned int b, unsigned int e, unsigned int modulus);
int extended_gcd(int a, int b, int *x, int *y);
unsigned int mod(unsigned long a, unsigned int b);
char *int2bin(int a, char *buffer, int buf_size);

#endif