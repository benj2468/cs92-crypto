#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "openssl/md5.h"

int hash_md5(char *message, unsigned char hash[16])
{
    MD5_CTX c;

    int res;

    res = MD5_Init(&c);
    if (res == 0)
    {
        printf("There was an error initiating md5 context\n");
        return 1;
    }

    res = MD5_Update(&c, message, strlen(message));
    if (res == 0)
    {
        printf("There was an error updating the hash\n");
        return 1;
    }

    res = MD5_Final(&hash[0], &c);
    if (res == 0)
    {
        printf("There was an error finalizing the has.\n");
        return 1;
    }

    return 0;
}

char *bin_to_string(const unsigned char input[], int nbytes)
{
#define MAX_BYTES 64
    static char output[MAX_BYTES * 2 + 3];
    char *output_ptr = output;
    if (nbytes > MAX_BYTES)
    {
        fprintf(stderr, "ERROR: bin_to_string supports up to %d bytes.  %d bytes given.\n", MAX_BYTES, nbytes);
    }
    sprintf(output, "0x");
    output_ptr += 2;
    for (int i = 0; i < nbytes; i++)
    {
        sprintf(output_ptr, "%02x", input[i]);
        output_ptr += 2;
    }
    return output;
#undef MAX_BYTES
}

int main()
{
    char *message = "USCYBERCOM plans, coordinates, integrates, synchronizes and conducts activities to: direct the operations and defense of specified Department of Defense information networks and; prepare to, and when directed, conduct full spectrum military cyberspace operations in order to enable actions in all domains, ensure US/Allied freedom of action in cyberspace and deny the same to our adversaries.";

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("../assets/words", "r");
    if (fp == NULL)
        exit(0);

    unsigned char hash[16];
    memset(hash, 0, 16);

    unsigned int num_buckets = 256;

    int buckets[num_buckets];
    memset(&buckets[0], 0, sizeof(int) * num_buckets);

    unsigned char hval[128];
    unsigned int *ip = (unsigned int *)&hval[0], bucket;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        char *fixed_msg = malloc(strlen(message) + len);
        memcpy(fixed_msg, message, strlen(message));
        strcat(fixed_msg, line);

        int res = hash_md5(fixed_msg, &hval[0]);
        if (res == 1)
        {
            printf("There was an error hashing a line\n");
            exit(0);
        }

        bucket = (*ip) % num_buckets;
        buckets[bucket] += 1;
    }

    printf("Bucket Distribution:\n");
    for (int i = 0; i < num_buckets; i++)
    {
        printf("\tBucket %d: %d\n", i, buckets[i]);
    }

    return 0;
}