/*
 * =====================================================================================
 *
 *       Filename:  bcrack.c
 *
 *    Description:  RSA Timing Attack for Computer Science @ Dartmout | CS 94 22X
 *
 *        Version:  1.0
 *        Created:  07/22/22 13:18:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Benjamin Cape (),
 *   Organization:  Dartmouth College Computer Science
 *
 * 		Arguments:	(1) Program Name
 * 					(2) PEM File
 * 					(Options) -d (debug); -t (include tests); -s [SAMPLES (default 100)] -w [WINDOW (default 10)];
 *
 * =====================================================================================
 */

/* Standard Libraries
 * --------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

/* Crypto Libraries
 * --------------------------------
 */
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

/* GLOBALS
 * --------------------------------
 * RSA Key-Pair Globals
 *
 * N := Modulus
 * E := Public Exponent
 * D := Private Exponent
 * keysize := bits of modulus
 */
const BIGNUM *N, *E, *D;
int keysize;
/* Time Management Globals */
int ITERATIONS = 10;
int SAMPLES = 100;
int WINDOW = 1;
struct timespec ts0, ts1;
unsigned int nsec;
float sec;

/* BIGNUM global context */
BN_CTX *CTX_G;
BIGNUM *bn1_g, *bn2_g;

/* EXECUTION FLAGS
 * --------------------------------
 * DEBUG
 */
int debug = 0;
/* TEST */
int test = 0;

/* MACROS FOR TIMING
 * --------------------------------
 * start Time
 */
#define start_time                                 \
	sync();                                        \
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts0); \
	for (int i = 0; i < ITERATIONS; i++)           \
	{

/* MACROS FOR TIMING
 * --------------------------------
 * stop Time
 */
#define stop_time(r)                               \
	}                                              \
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1); \
	sec = (float)ts1.tv_sec - (float)ts0.tv_sec;   \
	sec *= 1000000000;                             \
	sec += ts1.tv_nsec;                            \
	sec -= ts0.tv_nsec;                            \
	*r += sec

/* MACROS
 * --------------------------------
 * debug only
 */
#define DEBUG_ONLY \
	if (debug)

/* KEY SETUP
 * --------------------------------
 * Setup the keys by reading from the file provided
 */
int key_setup(char *keyfile_name)
{
	RSA *rsa = NULL;

	FILE *pemfile;

	if (!(pemfile = fopen(keyfile_name, "r+")))
		goto err;

	if (!(rsa = PEM_read_RSAPrivateKey(pemfile, NULL, NULL, NULL)))
		goto err;

	RSA_get0_key(rsa, &N, &E, &D);

	keysize = BN_num_bits(N);
	DEBUG_ONLY printf("~~~~~~ key_setup output status: ~~~~~~~~\n");
	DEBUG_ONLY printf("+---------------------------------------\n");
	DEBUG_ONLY printf("| Modulus:           %s\n", BN_bn2hex(N));
	DEBUG_ONLY printf("| Public Exponent:   %s\n", BN_bn2hex(E));
	DEBUG_ONLY printf("| Private Exponent:  %s\n", BN_bn2hex(D));
	DEBUG_ONLY printf("| Keysize:           %d\n", keysize);
	DEBUG_ONLY printf("+---------------------------------------\n");

	return 1;
err:
	printf("There was an error while attempting to extract and setup the keys\n");
	return 0;
}

/* GLOBAL SETUP
 * --------------------------------
 * Setup the global pointers
 */
int global_setup()
{
	bn1_g = BN_new();
	bn2_g = BN_new();

	CTX_G = BN_CTX_new();
}

/* PARTIAL RSA
 * --------------------------------
 * Performs the modulo exponentiation over [last_bit,first_bit] of the exponent
 * It also times the operation, storing the time in time_m (Time Measurement)
 */
int partial_rsa(BIGNUM *s_out, BIGNUM *s_in, const BIGNUM *base, const BIGNUM *exp,
				int first_bit, int last_bit, float *time_m)
{
	// Ensure bits are structured properly
	if (last_bit > first_bit)
		goto err;

	BN_CTX_start(CTX_G);

	if (!s_in)
	{
		s_in = BN_new();
		BN_one(s_in);
	}
	if (!BN_copy(bn1_g, s_in))
		goto err;

	// k := bit number
	int k;
	*time_m = 0;

	BIGNUM *tmp = BN_new();

	for (k = first_bit; k >= last_bit; k--)
	{
		if (BN_is_bit_set(exp, k))
		{
			// Perform the operation when it is set
			// R = R * X mod N
			if (!BN_copy(tmp, bn1_g))
				goto err;
			start_time if (!BN_mod_mul(bn1_g, tmp, base, N, CTX_G))
			{
				goto err;
			}
			stop_time(time_m);
		}

		if (!BN_copy(bn2_g, bn1_g))
			goto err;

		// Perform the operation when it is not set
		// R = R^2 mod N
		if (!BN_copy(tmp, bn1_g))
			goto err;
		start_time if (!BN_mod_sqr(bn1_g, tmp, N, CTX_G))
		{
			goto err;
		}
		stop_time(time_m);
	}

	BN_CTX_end(CTX_G);

	if (!s_out)
		return 1;

	if (last_bit)
	{
		if (!BN_copy(s_out, bn1_g))
			goto err;
	}
	else
	{
		if (!BN_copy(s_out, bn2_g))
			goto err;
	}

	return 1;
err:
	printf("There was an error in partial_rsa\n");
	return -1;
}

/* MEASURE VARIANCE
 * --------------------------------
 * This function measures the variance in ts_samples
 * And updates the prefix BIGNUM with the correct bit (bit k)
 */
int measure_variance(float *data, unsigned int len, float expected, float *res_var)
{
	if (len <= 0)
		goto err;
	float tmp, var = 0;
	int i;

	for (i = 0; i < len; i++)
	{
		tmp = data[i] - expected;
		tmp = tmp * tmp;
		tmp = tmp / len;
		var = var + tmp;
	}

	if (res_var)
		*res_var = var;

	return 1;

err:
	printf("There was an error when trying to measure the variance\n");
	return -1;
}

int BN_set_window(BIGNUM *a, unsigned int k, unsigned int v, int window_size)
{
	unsigned int rem = v;
	for (int b = k - window_size + 1; b <= k; b++)
	{
		if (rem % 2 == 1)
			BN_set_bit(a, b);
		else
			BN_clear_bit(a, b);
		rem = rem >> 1;
	}
}

/* EXECUTE GUESSES
 * --------------------------------
 * Starts with the most significant bit, and continues to try to guess the remaining bits, one at a time.
 */
int execute_guesses()
{
	BIGNUM *samples[SAMPLES], *guesses[1 << WINDOW], *last_sums[SAMPLES], **guesses_last_sums[1 << WINDOW];

	float true_ts, tmp_ts, *guesses_ts_data[1 << WINDOW];

	int incorrect_bits = 0, last_bit;
	for (int g = 0; g < (1 << WINDOW); g++)
	{
		guesses[g] = BN_new();
		BN_rand_range(guesses[g], N);
		guesses_ts_data[g] = malloc(sizeof(float) * SAMPLES);
		guesses_last_sums[g] = malloc(sizeof(BIGNUM *) * SAMPLES);
		for (int i = 0; i < SAMPLES; i++)
		{
			guesses_last_sums[g][i] = BN_new();
		}
		BN_one(guesses_last_sums[g][0]);
	}

	// Loop over the samples and get our random samples
	for (int i = 0; i < SAMPLES; i++)
	{
		samples[i] = BN_new();
		BN_rand_range(samples[i], N);

		last_sums[i] = BN_new();
		BN_one(last_sums[0]);
	}

	// Loop over each bit in reverse order
	//
	// We will guess bit k
	DEBUG_ONLY printf("~~~ execute_guesses running status: ~~~~~\n");
	DEBUG_ONLY printf("+---------------------------------------\n");
	for (int k = keysize - 1; k >= 0; k -= WINDOW)
	{
		for (int g = 0; g < (1 << WINDOW); g++)
			BN_set_window(guesses[g], k, g, WINDOW);

		last_bit = k - WINDOW + 1;

		// For each sample, we make a guess for bit k
		for (int i = 0; i < SAMPLES; i++)
		{
			// Get time of correct
			if (!partial_rsa(last_sums[i], last_sums[i], samples[i], D, k, last_bit, &true_ts))
				goto err;

			for (int g = 0; g < (1 << WINDOW); g++)
			{
				if (!partial_rsa(guesses_last_sums[g][i], guesses_last_sums[g][i], samples[i], guesses[g], k, last_bit, &tmp_ts))
					goto err;
				guesses_ts_data[g][i] = true_ts - tmp_ts;
			}
		}

		float min_var = INFINITY, min_var_2 = INFINITY,
			  tmp_var;
		int min_guess, min_guess_2;

		for (int g = 0; g < (1 << WINDOW); g++)
		{
			DEBUG_ONLY printf("| [%d] Guess %d = %s\n", k, g, BN_bn2hex(guesses[g]));
			if (!measure_variance(guesses_ts_data[g], SAMPLES, 0, &tmp_var))
				goto err;
			DEBUG_ONLY printf("| [%d] [g = %d] [Var = %f]\n", k, g, tmp_var);
			if (tmp_var < min_var)
			{
				min_var_2 = min_var;
				min_guess_2 = min_guess;
				min_var = tmp_var;
				min_guess = g;
			}
		}

		/*
		TODO:
		- Check if the epsilon factor that differentiates #1 and #2 is sufficient enough.
		- If it is below some threshold, we should play around with this, then spin up some new samples and try again for this specific case.
		- Now sure what the fastest way to spin up samples is... ask Professor Smith
		- Maybe he has another idea for this.


		Maybe what we do....
		Increase the window size, and make guesses for each possible number in the window...
		but the one with the lowest variance only actually contributes it's most significant bit.
		Therefore, we actually need to do the first round separately than we do the remaining rounds in the window,
		because we need to save the sum of the first round (since we still will always only guess 1 round at a time)
		*/

		DEBUG_ONLY printf("| [%d] Guessed (%d)\n", k, min_guess);

		for (int g = 0; g < (1 << WINDOW); g++)
			BN_set_window(guesses[g], k, min_guess, WINDOW);

		int correct = 1;
		for (int b = k - WINDOW + 1; b <= k; b++)
		{
			if (!(BN_is_bit_set(D, b) == BN_is_bit_set(guesses[0], b)))
			{
				correct = 0;
			}
			for (int g = 0; g < (1 << WINDOW); g++)
			{
				if (BN_is_bit_set(D, b))
					BN_set_bit(guesses[g], b);
				else
					BN_clear_bit(guesses[g], b);
			}
		}

		if (!correct)
			incorrect_bits += WINDOW;
		float hit_rate = 1.0 - ((float)(incorrect_bits) / (float)(keysize - k + WINDOW));

		if (correct)
		{
			DEBUG_ONLY printf("| [%d] Correct   (Success rate: %f)\n", k, hit_rate);
		}
		else
			DEBUG_ONLY printf("| [%d] Incorrect (Success rate: %f)\n", k, hit_rate);
	}

	DEBUG_ONLY printf("~~~ execute_guesses output status: ~~~~~\n");
	DEBUG_ONLY printf("+---------------------------------------\n");
	DEBUG_ONLY printf("| Incorrect Bits: 	%d\n", incorrect_bits);
	DEBUG_ONLY printf("+---------------------------------------\n");

	return 1;
err:
	printf("There was an error while trying to execute the guesses\n");
	return -1;
}

int test_set_window()
{
	BIGNUM *a = BN_new();
	unsigned int window = 16;
	BN_zero(a);

	BN_set_window(a, 4, window, 5);

	DEBUG_ONLY printf("NEW Value: 0x%s (should be 0x10)\n", BN_bn2hex(a));

	return 1;

err:
	printf("Set Window unit test did not pass");
	return -1;
}

/* TEST Identical Process */
int test_id_proc()
{
	float samples_ts[SAMPLES], ts, var, mean;

	BIGNUM *txt = BN_new();
	BN_rand_range(txt, N);

	for (int s = 0; s < SAMPLES; s++)
	{
		partial_rsa(NULL, NULL, txt, D, keysize - 1, 0, &ts);
		samples_ts[s] = ts;
		DEBUG_ONLY printf("[%d] - %f\n", s, ts);
	}

	measure_variance(samples_ts, SAMPLES, 0, &var);

	DEBUG_ONLY printf("[test_id_proc] Variance\t: %f\n", var);
	DEBUG_ONLY printf("[test_id_proc] Mean\t\t: %f\n", mean);
}

/* TEST RSA Implementation */
int test_rsa()
{
	BIGNUM *y0, *y1, *y2, *s_in;

	y0 = BN_new();
	y1 = BN_new();
	y2 = BN_new();
	s_in = BN_new();

	BN_rand_range(y0, N);

	BN_one(s_in);

	float t0, t1;

	// Complete

	// Perform the private op
	partial_rsa(y1, s_in, y0, D, keysize - 1, 0, &t0);

	// Perform the public op
	partial_rsa(y2, s_in, y1, E, keysize - 1, 0, &t1);

	DEBUG_ONLY printf("y0 = 0x%s\ny1 = 0x%s\ny2 = 0x%s\n",
					  BN_bn2hex(y0), BN_bn2hex(y1), BN_bn2hex(y2));

	DEBUG_ONLY printf("TIME CALCS:\n\tPublic Op: %f\n\tPrivate Op:%f\n", t0, t1);

	if (BN_cmp(y0, y2))
		goto err;

	// Bit by bit

	BN_one(y2);

	for (int k = keysize - 1; k >= 0; k++)
	{
		partial_rsa(y2, y2, y1, E, k, k, &t1);
	}
	if (BN_cmp(y0, y2))
		goto err;

	DEBUG_ONLY printf("It worked\n");

	return 1;
err:
	printf("There was an error while testing the RSA implementation\n");
	return -1;
}

/* TEST Variance Calculation */
int test_variance()
{
	float data[10];
	data[0] = 5;
	data[1] = 2;
	data[2] = 15;
	data[3] = 35;
	data[4] = 20;
	data[5] = 6;
	data[6] = 17;
	data[7] = 24;
	data[8] = 19;
	data[9] = 28;

	float var;
	float mean = 0;
	for (int i = 0; i < 10; i++)
		mean += (data[i] / 10);
	measure_variance(data, 10, mean, &var);
	if ((var - 100) > 1)
		goto err;

	DEBUG_ONLY printf("Variance: %f\n", var);

	return 1;
err:
	printf("Variance unit test did not pass\n");
	return -1;
}

/* MAIN
 * --------------------------------
 */
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Not enough arguments supplied\n");
		goto err;
	}

	char *prog_name = argv[0];
	char *pemfile_name = argv[1];

	for (int argi = 2; argi < argc; argi++)
	{
		char *arg = argv[argi];

		if (strcmp(arg, "-d") == 0)
			debug = 1;
		if (strcmp(arg, "-t") == 0)
			test = 1;
		if (strcmp(arg, "-s") == 0)
			SAMPLES = atoi(argv[argi + 1]);
		if (strcmp(arg, "-w") == 0)
			WINDOW = atoi(argv[argi + 1]);
		if (strcmp(arg, "-i") == 0)
			ITERATIONS = atoi(argv[argi + 1]);
	}

	DEBUG_ONLY printf("~~~~~~~~~~ main input status: ~~~~~~~~~~\n");
	DEBUG_ONLY printf("+---------------------------------------\n");
	DEBUG_ONLY printf("|    PEM File:         %s\n", pemfile_name);
	DEBUG_ONLY printf("|  DEBUG MODE:         ON\n");
	DEBUG_ONLY
	{
		if (test)
			printf("|   TEST MODE:         ON\n");
	}
	DEBUG_ONLY printf("|      WINDOW:         %d\n", WINDOW);
	DEBUG_ONLY printf("|  ITERATIONS:         %d\n", ITERATIONS);
	DEBUG_ONLY printf("|     SAMPLES:         %d\n", SAMPLES);
	DEBUG_ONLY printf("+---------------------------------------\n");

	// SETUP
	key_setup(pemfile_name);
	global_setup();

	// RUN TESTS
	if (test)
	{
		if (!test_rsa())
			goto err;
		if (!test_variance())
			goto err;
		if (!test_set_window())
			goto err;

		DEBUG_ONLY printf("Tests All Passed\n");
	}

	execute_guesses();

	return 1;
err:
	return -1;
}
