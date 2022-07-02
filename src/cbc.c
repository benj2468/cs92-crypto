#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/des.h"

int cbc_des_encrypt(const char *plaintext, const unsigned char *key, unsigned char *ciphertext, const unsigned char *initialization_vector)
{
    unsigned char *top_register;

    top_register = initialization_vector;

    int plaintext_len = strlen(plaintext);

    int i = 0;
    unsigned char *block_txt = malloc(BLOCK_SIZE);
    unsigned char *pre_encrypt_txt = malloc(BLOCK_SIZE);
    unsigned char *block_cipher = malloc(BLOCK_SIZE);

    while (i < plaintext_len)
    {
        strncpy(block_txt, &plaintext[i], BLOCK_SIZE);

        printf("Block Text [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(block_txt, BLOCK_SIZE));

        xor(block_txt, top_register, pre_encrypt_txt, BLOCK_SIZE);

        printf("XORed Text [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(pre_encrypt_txt, BLOCK_SIZE));

        des_encrypt(pre_encrypt_txt, key, block_cipher);

        printf("Encryped Text [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(block_cipher, BLOCK_SIZE));

        strncat(ciphertext, block_cipher, BLOCK_SIZE);

        printf("CipherText [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(ciphertext, (i + BLOCK_SIZE)));

        top_register = block_cipher;

        i += BLOCK_SIZE;
    }

    return 1;
}

int main()
{
    // unsigned char plaintext[8] = {0x6d, 0x73, 0x62, 0x72, 0x6f, 0x77, 0x6e, 0x33};
    unsigned char initialization_vector[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char plaintext[8] = {0x02, 0x46, 0x8a, 0xce, 0xec, 0xa8, 0x64, 0x20};
    unsigned char key[8] = {0x0f, 0x15, 0x71, 0xc9, 0x47, 0xd9, 0xe8, 0x59};
    unsigned char *ciphertext = malloc(BLOCK_SIZE * 2);

    cbc_des_encrypt(plaintext, key, ciphertext, initialization_vector);

    printf("%s\n", bin_to_string(ciphertext, strlen(plaintext)));
}
