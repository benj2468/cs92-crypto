#include "includes/crypto.h"

unsigned int mod_exp(unsigned int b, unsigned int e, unsigned int modulus)
{
    if (e == 0)
        return 1;
    if (b == 0)
        return 0;

    unsigned long y;
    if ((e % 2) == 0)
    {
        y = mod_exp(b, e / 2, modulus);
        y = (y * y) % modulus;
    }
    else
    {
        y = mod_exp(b, e - 1, modulus);
        y = (b * y) % modulus;
    }

    return (unsigned int)((y + modulus) % modulus);
}

int extended_gcd(int a, int b, int *x, int *y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return b;
    }

    int _x, _y;
    int gcd = extended_gcd(b % a, a, &_x, &_y);

    *x = _y - (b / a) * _x;
    *y = _x;

    return gcd;
}

unsigned int mod(unsigned long a, unsigned int b)
{
    return (a + b) % b;
}

char *int2bin(int a, char *buffer, int buf_size)
{
    buffer += (buf_size - 1);

    for (int i = INTEGER_BYTES; i > 0; i--)
    {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}