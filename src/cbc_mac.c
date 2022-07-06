#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/des.h"

int cbc_mac(const unsigned char *message, unsigned char *key, void (*encrypt)(unsigned char *block, unsigned char *key, unsigned char *output), unsigned char *mac)
{

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        mac[i] = 0x00;
    }

    unsigned char *xored = malloc(BLOCK_SIZE);

    for (int i = 0; i < strlen(message); i += BLOCK_SIZE)
    {
        unsigned char *block = &message[i];

        xor(block, mac, xored, BLOCK_SIZE);

        encrypt(xored, key, mac);
    }

    return 1;
}

typedef struct UnEncryptedMACedMsg
{
    unsigned char *msg, mac;
} UnEMACMsg;

UnEMACMsg *newMsg(unsigned char *msg, unsigned char *key)
{
    UnEMACMsg *e = malloc(sizeof(UnEMACMsg));
    e->msg = malloc(strlen(msg));
    e->mac = malloc(BLOCK_SIZE);

    memcpy(e->msg, msg, strlen(msg));

    cbc_mac(msg, key, des_encrypt, &e->mac);

    return e;
}

unsigned char *eve(const UnEMACMsg *M1, const UnEMACMsg *M2, unsigned char *m3)
{
    unsigned char *m2_first_block = M2->msg;

    memcpy(m3, M1->msg, strlen(M1->msg));
    strcat(m3, M2->msg);

    xor(&M1->mac, m2_first_block, &m3[strlen(M1->msg)], BLOCK_SIZE);
}

int eve_driver(unsigned char *key)
{

    unsigned char *m1 = "Benjamin J. Cape";

    UnEMACMsg *M1 = newMsg(m1, key);
    printf("[1] Message : %s\n", bin_to_string(m1, strlen(m1)));
    printf("[1] MAC     : %s\n", bin_to_string(&M1->mac, BLOCK_SIZE));
    printf("----------------------------\n");

    unsigned char *m2 = "Noah Victor Cape";

    UnEMACMsg *M2 = newMsg(m2, key);
    printf("[2] Message : %s\n", bin_to_string(m2, strlen(m2)));
    printf("[2] MAC     : %s\n", bin_to_string(&M2->mac, BLOCK_SIZE));
    printf("----------------------------\n");

    unsigned char m3[1000];
    memset(m3, 0, 1000);

    eve(M1, M2, &m3);

    UnEMACMsg *M3 = newMsg(m3, key);

    printf("[3] Message : %s\n", bin_to_string(m3, strlen(m3)));
    printf("[3] MAC     : %s\n", bin_to_string(&M3->mac, BLOCK_SIZE));

    if (memcmp(&M2->mac, &M3->mac, BLOCK_SIZE) != 0)
        printf("There was an error in the calculation...\n");
}

int main()
{

    unsigned char key[BLOCK_SIZE] = {0x0f, 0x15, 0x71, 0xc9, 0x47, 0xd9, 0xe8, 0x59};

    unsigned char *plaintext = "Benjamin J. Cape";

    unsigned char mac[8];

    cbc_mac(plaintext, key, des_encrypt, mac);

    eve_driver(key);

    return 1;
}