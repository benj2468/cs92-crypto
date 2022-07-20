/*******************************************************************
 *
 *	Kocher's Timing Attack Demo
 *	by Wei Zhang (wei@cs.dartmouth.edu)
 *	May 2004
 *
 *	CREDIT:
 *		my_rsa_partial borrowed from S. W. Smith's code.
 *		Slightly modified. -->
 *			BN_mul and BN_mod combined to BN_mod_mul
 *			BN_sqr and BN_mod combined to BN_mod_sqr
 *	UPDATE:
 *		Benjamin Cape 7.20.22 UID 001
 *
 *******************************************************************/

/*******************************************************************
 *	CONFIGURATION
 *******************************************************************/

// How many samples to use
#define SAMPLECOUNT 1000

// How many time measurements are performed for RSA
#define WINDOW 6

/***********************
 *	HEADERS
 ***********************/

#include <openssl/bn.h>

#include <time.h>
#include <stdlib.h>
#include <unistd.h>

/***************************************************************************
 *	GLOBAL VARIABLES
 ***************************************************************************/

// for keypair information
BIGNUM *N, *E, *D;
int keysize;

// globals for use in time measurement
BIGNUM *thistime, *mintime;
int timepass;
struct timespec ts0, ts1;
unsigned int nsec, sec;

//  global BIGNUM context and scratches, to try to reduce variations due to
// dynamic memory allocation
BN_CTX *ctx_g;
BIGNUM *s1_g, *s2_g;

BIGNUM *y[SAMPLECOUNT];		// samples
BIGNUM *T[SAMPLECOUNT];		// total times
BIGNUM *diff0[SAMPLECOUNT]; // time differences
BIGNUM *diff1[SAMPLECOUNT]; // ditto
BIGNUM *d0;					// guessed key
BIGNUM *d1;					// ditto
BIGNUM *p0[SAMPLECOUNT];	// ciphertexts
BIGNUM *p1[SAMPLECOUNT];	// ditto
BIGNUM *av0;				// average of time differences
BIGNUM *av1;				// ditto
BIGNUM *temp;				// temp var for use
BIGNUM *var0;				// variance
BIGNUM *var1;				// ditto
BIGNUM *t0;					// tmp var for time
BIGNUM *t1;					// ditto
BIGNUM *bnzero;				// zero;

int correct_bit; // how many bits are guessed correctly
int bad_diff;	 // diff<0;
/*************************************************************************
 *	TIMING MACROS
 *	Extracted from S. W. Smith's code
 *************************************************************************/

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
	// }                                                 \
	BN_add(r, r, mintime);

/***************************************************************
 *	FUNCTION DECLARATIONS
 ***************************************************************/

void global_setup();
void global_cleanup();
int my_rsa_partial(BIGNUM *s_out, BIGNUM *s_in,
				   BIGNUM *y, BIGNUM *x,
				   int first_bit, int last_bit, BIGNUM *timep);
void time_bit(int i);
void cal_var();
void guess_bit(int i);
void get_T();

/**********************************
 *	HERE IT GOES
 **********************************/

int main()
{
	global_setup();
	get_T();
	printf("Start guessing keys bit by bit.\n");
	int i;
	for (i = keysize - 1; i >= 0; i--)
	{
		// Start of Update #001
		BN_zero(var0);
		BN_zero(var1);
		BN_zero(av0);
		BN_zero(av1);
		// End of Update #001
		time_bit(i);
		cal_var();
		guess_bit(i);
	}
	printf("Guessing done.\n");

	printf("D is:\n0x%s", BN_bn2hex(D));
	printf("Guessed D is:\n0x%s", BN_bn2hex(d0));
	printf("Correctly guessed %d out of %d bits\
		by correcting along the way.\n",
		   correct_bit, keysize);
	printf("There are %d bad_diff.\n", bad_diff);

	global_cleanup();
	return 0;

err:
	printf("Error occurs in main.\n");
	global_cleanup();
	return 1;
}

/*allocate memory for global variables and initialize them*/
void global_setup()
{
	printf("Setting up global variables...\n");
#include "key.h"
	if (!(N = BN_bin2bn(n, sizeof(n) - 1, NULL)))
		goto err;
	if (!(E = BN_bin2bn(e, sizeof(e) - 1, NULL)))
		goto err;
	if (!(D = BN_bin2bn(d, sizeof(d) - 1, NULL)))
		goto err;
	if (!(ctx_g = BN_CTX_new()))
		goto err;
	if (!(s1_g = BN_new()))
		goto err;
	if (!(s2_g = BN_new()))
		goto err;

	if (!(mintime = BN_new()))
		goto err;
	if (!(thistime = BN_new()))
		goto err;
	keysize = BN_num_bits(N);
	correct_bit = 0;
	int i;
	for (i = 0; i < SAMPLECOUNT; i++)
	{
		if (!(y[i] = BN_new()))
			goto err;
		if (!BN_rand_range(y[i], N))
			goto err;
		if (!(T[i] = BN_new()))
			goto err;
		if (!(diff0[i] = BN_new()))
			goto err;
		if (!(diff1[i] = BN_new()))
			goto err;
		if (!(p0[i] = BN_new()))
			goto err;
		if (!BN_one(p0[i]))
			goto err;
		if (!(p1[i] = BN_new()))
			goto err;
		if (!BN_one(p1[i]))
			goto err;
	}
	if (!(d0 = BN_new()))
		goto err;

	// Update #001
	BN_zero(d0);
	if (!(d1 = BN_new()))
		goto err;
	// Update #001
	BN_zero(d1);
	if (!(av0 = BN_new()))
		goto err;
	if (!(av1 = BN_new()))
		goto err;
	if (!(temp = BN_new()))
		goto err;
	if (!(var0 = BN_new()))
		goto err;
	if (!(var1 = BN_new()))
		goto err;
	if (!(t0 = BN_new()))
		goto err;
	if (!(t1 = BN_new()))
		goto err;
	if (!(bnzero = BN_new()))
		goto err;
	// Update #001
	BN_zero(bnzero);

	bad_diff = 0;
	printf("global_setup done!\n");
	return;
err:
	printf("global_setup err!\n");
	exit(1);
}

/*free memory alloc'ed for global variables*/
void global_cleanup()
{
	BN_free(N);
	BN_free(E);
	BN_free(D);
	BN_CTX_free(ctx_g);
	BN_free(s1_g);
	BN_free(s2_g);
	BN_free(mintime);
	BN_free(thistime);
	int i;
	for (i = 0; i < SAMPLECOUNT; i++)
	{
		BN_free(y[i]);
		BN_free(T[i]);
		BN_free(diff0[i]);
		BN_free(diff1[i]);
		BN_free(p0[i]);
		BN_free(p1[i]);
	}

	BN_free(d0);
	BN_free(d1);
	BN_free(av0);
	BN_free(av1);
	BN_free(var0);
	BN_free(var1);
	BN_free(temp);
	BN_free(t0);
	BN_free(t1);
	BN_free(bnzero);
	printf("global_cleanup done!\n");
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
				   int first_bit, int last_bit, BIGNUM *timep)
{
	printf("Starting partial RSA with parameters:\n\
		\t s_out: %s\n\
		\t s_in : %s\n\
		\t y	: %s\n\
		\t x	: %s\n\
		\t first: %d\n\
		\t last : %d\n",
		   BN_bn2hex(s_out), BN_bn2hex(s_in), BN_bn2hex(y), BN_bn2hex(x), first_bit, last_bit);

	int k;
	int not_used;
	BN_CTX_start(ctx_g);
	BN_zero(timep);
	BN_copy(s1_g, s_in);
	for (k = first_bit; k >= last_bit; k--)
	{
		printf("Working on it... %d\n", k);
		BN_copy(s2_g, s1_g);
		not_used = BN_num_bits(y); /* heuristic: try to get y in the cache! */
		if (BN_is_bit_set(x, k))
		{
			// start_time0;
			start_time1;
			BN_mod_mul(s1_g, s2_g, y, N, ctx_g);
			stop_time(timep);
		}
		BN_copy(s2_g, s1_g);
		// in theory, we should skip this next step for k  == 0.
		// that would reduce noise.  but i haven't tried that yet.
		// start_time0;
		start_time1;
		BN_mod_sqr(s1_g, s2_g, N, ctx_g);
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
}

/*time_bit times the next bit operation and calculate the average time
 * differences of guessing 0 and 1
 */
void time_bit(int i)
{
	if (!BN_set_bit(d1, i))
		goto err;
	int j;
	for (j = 0; j < SAMPLECOUNT; j++)
	{
		my_rsa_partial(p0[j], p0[j], y[j], d0, i, i, t0);
		my_rsa_partial(p1[j], p1[j], y[j], d1, i, i, t1);
		if (!BN_sub(diff0[j], diff0[j], t0))
			goto err;
		if (!BN_sub(diff1[j], diff1[j], t1))
			goto err;
		if (BN_cmp(diff0[j], bnzero) < 0)
		{
			bad_diff++;
			BN_sub(diff0[i], bnzero, diff0[i]);
		}
		if (BN_cmp(diff1[j], bnzero) < 0)
		{
			bad_diff++;
			BN_sub(diff1[j], bnzero, diff1[j]);
		}
		if (!BN_add(av0, diff0[j], av0))
			goto err;
		if (!BN_add(av1, diff1[j], av1))
			goto err;
	}
	BN_div_word(av0, SAMPLECOUNT);
	BN_div_word(av1, SAMPLECOUNT);
	return;

err:
	printf("Error in time_bit.\n");
	global_cleanup();
	exit(1);
}

/*calculate variances*/
void cal_var()
{
	BN_CTX_start(ctx_g);
	int j;
	for (j = 0; j < SAMPLECOUNT; j++)
	{
		if (!BN_sub(temp, diff0[j], av0))
			goto err;
		if (!BN_sqr(temp, temp, ctx_g))
			goto err;
		if (!BN_add(var0, var0, temp))
			goto err;
		if (!BN_sub(temp, diff1[j], av1))
			goto err;
		if (!BN_sqr(temp, temp, ctx_g))
			goto err;
		if (!BN_add(var1, var1, temp))
			goto err;
	}
	BN_CTX_end(ctx_g);
	return;

err:
	printf("Error in cal_var.\n");
	global_cleanup();
	exit(1);
}

/*guess bit i and correct it*/
void guess_bit(int i)
{
	int guessed_bit = 1;
	if (BN_cmp(var0, var1) < 0)
		guessed_bit = 0;
	int actual_bit = BN_is_bit_set(D, i);

	if (actual_bit == guessed_bit)
		correct_bit++;
	printf("bit %d: %d|%d; correct guesses: %d out of %d\n",
		   i, guessed_bit, actual_bit, correct_bit, keysize - i);
	printf("var0 %s, var1 %s\n", BN_bn2dec(var0), BN_bn2dec(var1));
	int j;
	if (actual_bit)
	{
		BN_set_bit(d0, i);
		for (j = 0; j < SAMPLECOUNT; j++)
		{
			if (!BN_copy(diff0[j], diff1[j]))
				goto err;
			if (!BN_copy(p0[j], p1[j]))
				goto err;
		}
	}
	else
	{
		BN_clear_bit(d1, i);
		for (j = 0; j < SAMPLECOUNT; j++)
		{
			if (!BN_copy(diff1[j], diff0[j]))
				goto err;
			if (!BN_copy(p1[j], p0[j]))
				goto err;
		}
	}
	return;
err:
	printf("Error in guess_bit while guessing bit %d", i);
	global_cleanup();
	exit(1);
}

/*Getting T for all the samples*/
void get_T()
{
	printf("getting T...\n");
	BIGNUM *s_in;
	if (!(s_in = BN_new()))
		goto err;
	if (!(BN_one(s_in)))
	{
		BN_free(s_in);
		goto err;
	}
	int i;
	for (i = 0; i < SAMPLECOUNT; i++)
	{
		my_rsa_partial(temp, s_in, y[i], D, keysize - 1, 0, T[i]);
		if (i % 500 == 0)
		{
			printf("T[%d]=%s\n", i, BN_bn2dec(T[i]));
		}
		if (!BN_copy(diff0[i], T[i]))
			goto err;
		if (!BN_copy(diff1[i], T[i]))
			goto err;
	}
	BN_free(s_in);
	printf("T gotten.\n");
	return;
err:
	global_cleanup();
	printf("Error in get_T.\n");
	exit(1);
}
