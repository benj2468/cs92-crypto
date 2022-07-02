#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "includes/crypto.h"

unsigned int *
read_primes(unsigned char *file_name)
{
    FILE *f;

    f = fopen(file_name, "r");
    if (f == NULL)
    {
        printf("Could not open file: %s", file_name);
        exit(0);
    }

    unsigned int *primes = malloc(100);
    unsigned int i = 0;
    unsigned int line;
    while (fscanf(f, "%d", &line) == 1)
    {
        if (i % 100 == 0)
        {
            primes = realloc(primes, 200);
        }
        primes[i] = line;
        i++;
    };
    printf("read %d primes\n", i);

    return primes;
}

int min(int a, int b)
{
    return (a > b) ? b : a;
}

int is_coprime(unsigned int a, unsigned int b)
{
    int m = min(a, b);
    for (unsigned int i = 2; i <= m; i++)
    {
        if (a % m == 0 && b % m == 0)
            return 0;
    }
    return 1;
}

unsigned int find_coprime(unsigned int phi)
{
    for (unsigned int i = 2; i < phi; i++)
    {
        if (is_coprime(i, phi) == 1)
            return i;
    }
    return 0;
}

typedef struct PublicKey
{
    unsigned int n;
    unsigned int e;
} PublicKey;

typedef struct PrivateKey
{
    unsigned int n;
    unsigned int d;
} PrivateKey;

int generate_rsa_keys(unsigned int *primes, PublicKey *pub_key, PrivateKey *priv_key)
{

    printf("Enter indices for two primes:\n");

    unsigned int index1, index2;

    scanf("%u", &index1);
    scanf("%u", &index2);

    unsigned int p = primes[index1];
    unsigned int q = primes[index2];

    printf("p = %u, q = %u\n", p, q);

    unsigned int n = p * q;
    unsigned int phi = (p - 1) * (q - 1);

    printf("n = %u, phi = %u\n", n, phi);

    unsigned int e = find_coprime(phi);

    printf("e = %u\n", e);

    int x, y;
    unsigned int gcd = extended_gcd(e, phi, &x, &y);
    unsigned int d = mod(x, phi);

    printf("d = %d\n", d);

    unsigned char buff[31];
    int2bin(e, buff, 30);
    printf("e = %s\n", buff);

    int2bin(d, buff, 30);
    printf("d = %s\n", buff);

    int2bin(n, buff, 30);
    printf("n = %s\n", buff);

    pub_key->n = n;
    pub_key->e = e;

    priv_key->n = n;
    priv_key->d = d;

    return 1;
}

int rsa_encrypt(unsigned int message, PublicKey *key, unsigned int *buffer)
{
    *buffer = mod_exp(message, key->e, key->n);

    printf("c = m^e mod n = %u\n", *buffer);

    return 0;
}

int rsa_decrypt(unsigned int cipher, PrivateKey *key, unsigned int *buffer)
{
    *buffer = mod_exp(cipher, key->d, key->n);

    printf("c^d mod n = %u\n", *buffer);

    return 0;
}

int main()
{
    int res;
    unsigned int *primes = read_primes("./support/primes.txt");

    PublicKey *pub_key = malloc(sizeof(struct PublicKey));
    PrivateKey *priv_key = malloc(sizeof(struct PrivateKey));

    generate_rsa_keys(primes, pub_key, priv_key);

    while (1)
    {
        printf("Enter Message:\n");

        unsigned char *msg_str = malloc(10);
        scanf("%s", msg_str);

        unsigned int message = atoi(msg_str);
        free(msg_str);

        printf("Message = %lu\n", message);

        if (message > (1 << 31))
        {
            printf("Message is too large... must be less than 15 bits\n");
            continue;
        }

        unsigned int cipher;
        res = rsa_encrypt(message, pub_key, &cipher);
        ERROR_CHECK

        unsigned int plaintxt;
        res = rsa_decrypt(cipher, priv_key, &plaintxt);
        ERROR_CHECK

        if (plaintxt != message)
        {
            printf("There is an error... did not decode the correct message");
        }
    }
}
