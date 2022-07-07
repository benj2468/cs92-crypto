#include "includes/crypto.h"

unsigned long mod_exp(unsigned long b, unsigned long e, unsigned long modulus)
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

    return (unsigned long)((y + modulus) % modulus);
}

unsigned long extended_gcd(unsigned long a, unsigned long b, unsigned long *x, unsigned long *y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return b;
    }

    unsigned long _x, _y;
    unsigned long gcd = extended_gcd(mod(b, a), a, &_x, &_y);

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