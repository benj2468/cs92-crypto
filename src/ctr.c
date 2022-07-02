#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/des.h"

int ctr_des_encrypt(const char *plaintext, const unsigned char key[BLOCK_SIZE], unsigned char *ciphertext, unsigned char *counter)
{
    unsigned char *counter_value;
    unsigned char encrypted_counter_value[BLOCK_SIZE];
    unsigned char block_cipher_text[BLOCK_SIZE];

    int plaintext_len = strlen(plaintext);
    int i = 0;
    while (i < plaintext_len)
    {
        counter_value = &counter[i];

        printf("Counter Value [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(counter, BLOCK_SIZE));

        des_encrypt(counter_value, key, encrypted_counter_value);

        printf("Encrypted Counter [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(encrypted_counter_value, BLOCK_SIZE));

        xor(encrypted_counter_value, &plaintext[i], block_cipher_text, BLOCK_SIZE);

        printf("XORed [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(block_cipher_text, BLOCK_SIZE));

        strncat(ciphertext, block_cipher_text, BLOCK_SIZE);

        printf("CipherText [%d]: %s\n", i / BLOCK_SIZE, bin_to_string(ciphertext, (i + BLOCK_SIZE)));

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

    ctr_des_encrypt(plaintext, key, ciphertext, initialization_vector);

    printf("%s\n", bin_to_string(ciphertext, strlen(plaintext)));
}
