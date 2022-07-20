// s.w. smith, dartmouth college, april 2004

// sample code to do a simple RSA implementation (based on multiply-and-square)
// and record a good measurement of the time it takes.

// this code uses the BIGNUM package from openssl-devel, and was written
// using the BIGNUM source as a model.

// to compile, use -lrt (for the timing) and -lcrypto (for the bignums).
// -O0 also seemed to help

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/decoder.h>
#include <openssl/evp.h>

// permanent globals, for keypair information
BIGNUM *N, *E, *D;
int keysize;

// globals for use in time measurement
BIGNUM *thistime, *mintime;
int timepass;
struct timespec ts0, ts1;
unsigned int nsec, sec;

//  global BIGNUM context and scracthes, to try to reduce variations due to
// dynamic memory allocation
BN_CTX *ctx_g;
BIGNUM *s1_g, *s2_g;

// how many times to repeat each measurement
#define WINDOW 6

// get ready to start timing...
#define start_time0                                   \
    for (timepass = 0; timepass < WINDOW; timepass++) \
    {

// start the timing!
#define start_time1 \
    sync();         \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts0);

// stop the timing, and add the result to r
#define stop_time(r)                                  \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);    \
    sec = ts1.tv_sec - ts0.tv_sec;                    \
    BN_set_word(thistime, sec);                       \
    BN_mul_word(thistime, (unsigned long)1000000000); \
    BN_add_word(thistime, ts1.tv_nsec);               \
    BN_sub_word(thistime, ts0.tv_nsec);               \
    if (0 == timepass)                                \
        BN_copy(mintime, thistime);                   \
    else                                              \
    {                                                 \
        if (-1 == BN_cmp(thistime, mintime))          \
            BN_copy(mintime, thistime);               \
    }                                                 \
    }                                                 \
    BN_add(r, r, mintime);

// set up the globals
void key_setup()
{
    FILE *pemfile;

    pemfile = fopen("key.pem", "r+");

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

        EVP_PKEY_get_bn_param(pkey, "n", &N);
        printf("Modulus: %s\n", BN_bn2hex(N));

        EVP_PKEY_get_bn_param(pkey, "e", &E);
        printf("Public Exponent: %s\n", BN_bn2hex(E));

        EVP_PKEY_get_bn_param(pkey, "d", &D);
        printf("Private Exponent: %s\n", BN_bn2hex(D));
    }

    ctx_g = BN_CTX_new();
    s1_g = BN_new();
    s2_g = BN_new();

    mintime = BN_new();
    thistime = BN_new();

    keysize = BN_num_bits(N);
    fprintf(stderr, "keysize = %d bits\n", keysize);
}

// do RSA on input y and exponent x....
// however, only go from first_bit to last_bit in the exponent.
// (these are numbered in BIGNUM style.  to do the entire
// exponent, first_bit should be keysize -1, and last_bit should be 0.)

// record the execution time in timep

// use s_in as the initial running value, and s_out as the final one.

// (i coded it this way to make it easier to do operations only
// part of an exponent guess)

int my_rsa_partial(BIGNUM *s_out, BIGNUM *s_in,
                   BIGNUM *y, BIGNUM *x,
                   int first_bit, int last_bit,
                   BIGNUM *timep)
{

    int k, rc;
    int not_used;

    BN_CTX_start(ctx_g);
    BN_zero(timep);

    BN_copy(s1_g, s_in);

    for (k = first_bit; k >= last_bit; k--)
    {

        BN_copy(s2_g, s1_g);

        not_used = BN_num_bits(y); // heuristic: try to get y in the cache!

        if (BN_is_bit_set(x, k))
        {

            start_time0;
            start_time1;
            BN_mul(s1_g, s2_g, y, ctx_g);
            BN_mod(s1_g, s1_g, N, ctx_g);
            stop_time(timep);
        }

        BN_copy(s2_g, s1_g);

        // in theory, we should skip this next step for last_bit == 0.
        // that would reduce noise.  but i haven't tried that yet.
        start_time0;
        start_time1;
        BN_sqr(s1_g, s2_g, ctx_g);
        BN_mod(s1_g, s1_g, N, ctx_g);
        stop_time(timep);
    }

    // if it's a nonzero last_bit, then return the sum for the next time
    // else, return the RSA result
    if (last_bit)
        BN_copy(s_out, s1_g);
    else
        BN_copy(s_out, s2_g);

    BN_CTX_end(ctx_g);
    return (0);

err:
    fprintf(stderr, "error in my_rsa, k = %d\n", k);
    BN_CTX_end(ctx_g);
    return (-1);
}

int main(int argc, char *argv[])
{

    BIGNUM *y0, *y1, *y2, *s_in, *time0, *time1;

    key_setup();
    y0 = BN_new();
    y1 = BN_new();
    y2 = BN_new();
    s_in = BN_new();
    time0 = BN_new();
    time1 = BN_new();

    // get a random y0 in ZN

    BN_rand_range(y0, N);

    // for full RSA, the initial sum should be one
    BN_one(s_in);

    // do the private op
    my_rsa_partial(y1, s_in, y0, D, keysize - 1, 0, time0);

    // do the public op
    my_rsa_partial(y2, s_in, y1, E, keysize - 1, 0, time1);

    // print results
    printf("y0 = 0x%s\ny1 = 0x%s\ny2 = 0x%s\n",
           BN_bn2hex(y0), BN_bn2hex(y1), BN_bn2hex(y2));

    printf("execution times = 0x%s (%s), 0x%s (%s)\n",
           BN_bn2hex(time0), BN_bn2dec(time0),
           BN_bn2hex(time1), BN_bn2dec(time1));

    if (0 == BN_cmp(y0, y2))
        printf("it worked!\n");
    else
        printf("trouble!\n");
}
