#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "includes/crypto.h"

unsigned int p = 0x3b23cf2b;

unsigned int g = 2;

typedef struct PrivateKey
{
    unsigned int x;
} PrivateKey;

typedef struct PublicKey
{
    unsigned int y;
} PublicKey;

typedef struct ElGamalCipher
{
    unsigned int c1, c2;
} ElGamalCipher;

int find_generator(unsigned int p)
{
}

int generate_keys(PrivateKey *priv_key, PublicKey *pub_key)
{

    unsigned int x = 1;
    while (x <= 1)
    {
        x = rand() % p;
    }

    unsigned int y = mod_exp(g, x, p);

    pub_key->y = y;
    priv_key->x = x;

    return 0;
}

int elgamal_encrypt(unsigned int message, PublicKey *key, ElGamalCipher *cipher)
{

    unsigned int k = rand() % p;

    printf("Encrypting... k = 0x%08x\n", k);

    unsigned int c1 = mod_exp(g, k, p);
    unsigned int c2 = mod((unsigned long)message * (unsigned long)mod_exp(key->y, k, p), p);

    cipher->c1 = c1;
    cipher->c2 = c2;

    printf("Encrypted... c2 = 0x%08x\n", c2);
    printf("Encrypted... c1 = 0x%08x\n", c1);

    return 0;
}

int elgamal_decrypt(ElGamalCipher *cipher, PrivateKey *key, unsigned int *plaintext)
{

    unsigned int step_1 = mod_exp(cipher->c1, key->x, p);

    printf("Step 1 Complete... = 0x%08x\n", step_1);

    unsigned int x, y;
    int gcd = extended_gcd(step_1, p, &x, &y);

    unsigned int inverse = mod(x, p);

    unsigned int step_2 = mod((unsigned long)cipher->c2 * (unsigned long)inverse, p);

    printf("Step 2 Complete... = 0x%08x\n", step_2);

    *plaintext = step_2;

    return 0;
}

int main()
{

    PrivateKey *priv_key = malloc(sizeof(PrivateKey));
    PublicKey *pub_key = malloc(sizeof(PublicKey));

    int res = generate_keys(priv_key, pub_key);
    ERROR_CHECK

    while (1)
    {
        printf("Enter Message:\n");

        unsigned char *msg_str = malloc(10);
        scanf("%s", msg_str);

        unsigned int message = atoi(msg_str);
        free(msg_str);

        printf("Message = 0x%08x\n", message);

        if (message > (1 << 31))
        {
            printf("Message is too large... must be less than 15 bits\n");
            continue;
        }

        ElGamalCipher *cipher = malloc(sizeof(ElGamalCipher));

        res = elgamal_encrypt(message, pub_key, cipher);
        ERROR_CHECK

        unsigned int plaintxt;
        res = elgamal_decrypt(cipher, priv_key, &plaintxt);
        ERROR_CHECK

        if (plaintxt != message)
        {
            printf("There is an error... did not decode the correct message");
        }
        break;
    }

    return 1;
}