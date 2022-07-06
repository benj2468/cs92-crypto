#include <stdio.h>
#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/decoder.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>

int verify_q4(const BIGNUM *n, const BIGNUM *p, const BIGNUM *q, const BIGNUM *e, const BIGNUM *d)
{
    printf("-------------------------\n");
    printf("      Question 4\n");
    printf("--------vvvvvv-----------\n");
    int rc;

    BN_CTX *c;
    c = BN_CTX_new();

    BIGNUM *res = BN_new();

    rc = BN_mul(res, p, q, c);
    if (rc == 0)
        printf("There was some error verifying n = pq\n");

    rc = BN_cmp(res, n);
    if (rc == 0)
        printf("Verified [n = pq]\n");

    BIGNUM *p_1 = BN_new(), *q_1 = BN_new(), *modulus = BN_new(), *one = BN_new();
    BN_one(one);
    BN_sub(p_1, p, one);
    BN_sub(q_1, q, one);
    BN_mul(modulus, p_1, q_1, c);

    rc = BN_mod_mul(res, e, d, modulus, c);
    if (rc == 0)
        printf("There was some error verifying 1 = (ed) mod (p-1)(q-1)\n");

    rc = BN_cmp(res, one);
    if (rc == 0)
        printf("Verified [1 = (ed) mod (p-1)(q-1)]\n");

    return 1;
}

int examine_key(char *key_file)
{

    FILE *pemfile;

    pemfile = fopen(key_file, "r+");

    OSSL_DECODER_CTX *dctx;
    EVP_PKEY *pkey = NULL;
    const char *format = "PEM";   /* NULL for any format */
    const char *structure = NULL; /* any structure */
    const char *keytype = "RSA";  /* NULL for any key */

    dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, format, structure,
                                         keytype,
                                         OSSL_KEYMGMT_SELECT_KEYPAIR,
                                         NULL, NULL);
    if (dctx == NULL)
    {
        printf("/* error: no suitable potential decoders found */\n");
    }
    if (OSSL_DECODER_from_fp(dctx, pemfile))
    {
        BIGNUM *n = NULL;
        EVP_PKEY_get_bn_param(pkey, "n", &n);
        printf("Modulus: %s\n", BN_bn2hex(n));

        BIGNUM *e = NULL;
        EVP_PKEY_get_bn_param(pkey, "e", &e);
        printf("Public Exponent: %s\n", BN_bn2hex(e));
        BIGNUM *d = NULL;
        EVP_PKEY_get_bn_param(pkey, "d", &d);
        printf("Private Exponent: %s\n", BN_bn2hex(d));

        BIGNUM *prime_factor = NULL;
        int primes = 1;

        BIGNUM *p, *q;

        char *key_base = "rsa-factor";
        char key[20];
        sprintf(key, "%s%d", key_base, primes);
        while (EVP_PKEY_get_bn_param(pkey, key, &prime_factor) == 1)
        {
            if (primes == 1)
                p = BN_dup(prime_factor);
            else
                q = BN_dup(prime_factor);
            printf("prime%d: %s\n", primes, BN_bn2hex(prime_factor));
            primes++;
            sprintf(key, "%s%d", key_base, primes);
        }

        BIGNUM *exponent = NULL;
        int exp = 1;
        key_base = "rsa-exponent";
        sprintf(key, "%s%d", key_base, exp);
        while (EVP_PKEY_get_bn_param(pkey, key, &exponent) == 1)
        {
            printf("exponent%d: %s\n", exp, BN_bn2hex(exponent));
            exp++;
            sprintf(key, "%s%d", key_base, exp);
        }

        BIGNUM *val = NULL;

        EVP_PKEY_get_bn_param(pkey, "rsa-coefficient1", &val);
        printf("coefficient: %s\n", BN_bn2hex(val));

        int bits;
        EVP_PKEY_get_int_param(pkey, "bits", &bits);

        printf("Private-Key: (%d bit, %d primes)\n", bits, primes - 1);

        verify_q4(n, p, q, e, d);

        BN_clear_free(val);
    }
    else
    {
        /* decoding failure */
    }
    OSSL_DECODER_CTX_free(dctx);

    return 0;
}

int main(int argc, char **argv)
{

    char *file = malloc(100);
    int examining_file = 0;

    int q34 = 0;

    printf("%d Arguments\n", argc);
    for (int i = 0; i < argc; i++)
    {
        if (examining_file == 1)
        {
            memcpy(file, argv[i], strlen(argv[i]));
        }
        if (strcmp(argv[i], "-f") == 0)
            examining_file = 1;

        if (strcmp(argv[i], "-q34") == 0)
            q34 = 1;

        printf("Argument [%d]: %s\n", i, argv[i]);
    }

    if (examining_file == 0)
    {
        printf("Must provide a PEM file to examine.");
    }

    if (q34 == 1)
        examine_key(file);

    return 1;
}