#include <stdio.h>
#include <openssl/dsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define BLEN 100

int debug = 0;

int extract_data(char *file_name, const BIGNUM **r, const BIGNUM **s, BIGNUM *h)
{

    FILE *hashfile, *sigfile;
    DSA_SIG *dsig;

    unsigned char hashbuf[BLEN];
    unsigned char sigbuf[BLEN];
    int hashlen, siglen;
    int rc;
    const unsigned char *cp, **cpp;

    char *hash_file_name = malloc(sizeof(file_name) + 15);
    if (hash_file_name == NULL)
    {
        printf("Could not allocate memory for hash_file_name\n");
        return -1;
    }
    sprintf(hash_file_name, "../assets/%s.hash", file_name);
    hashfile = fopen(hash_file_name, "r");

    char *sig_file_name = malloc(sizeof(file_name) + 15);
    if (sig_file_name == NULL)
    {
        printf("Could not allocate memory for sig_file_name\n");
        return -1;
    }
    sprintf(sig_file_name, "../assets/%s.sign", file_name);
    sigfile = fopen(sig_file_name, "r");

    hashlen = fread(hashbuf, 1, BLEN, hashfile);
    siglen = fread(sigbuf, 1, BLEN, sigfile);

    if (hashlen != 20)
        printf("warning: hash is not a sha-1 hash?\n");

    BN_bin2bn(hashbuf, hashlen, h);
    if (debug == 1)
        printf("hash as a bignum  %s\n", BN_bn2hex(h));

    cp = &sigbuf[0];
    cpp = &cp;

    dsig = d2i_DSA_SIG(NULL, cpp, siglen);

    if (NULL == dsig)
    {

        printf("warning: signature file was not properly formatted\n");
        return -1;
    }

    DSA_SIG_get0(dsig, r, s);
    if (debug == 1)
    {
        printf("r %s\n", BN_bn2hex(*r));
        printf("s %s\n", BN_bn2hex(*s));
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int res;
    if (argc < 4)
    {
        printf("Must provide two file names to compare where there ${name}.hash and ${name}.sign both exists in ../assets/\n");
        return -1;
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
            debug = 1;
    }

    FILE *pubfile;
    const BIGNUM *p, *q, *g, *y;
    DSA *d;

    char *pub_file_name = argv[1];

    char *pub_file_path = malloc(sizeof(pub_file_name) + 15);
    if (pub_file_path == NULL)
    {
        printf("Could not allocate memory for pub_file_name\n");
        return -1;
    }
    sprintf(pub_file_path, "../assets/%s.pem", pub_file_name);

    pubfile = fopen(pub_file_path, "r");
    if (pubfile == NULL)
    {
        printf("Could not open public key file: %s\n", pub_file_path);
        return -1;
    }

    d = PEM_read_DSA_PUBKEY(pubfile, NULL, NULL, NULL);
    DSA_get0_pqg(d, &p, &q, &g);
    DSA_get0_key(d, &y, NULL);

    if (debug == 1)
    {
        printf("p %s\n", BN_bn2hex(p));
        printf("q %s\n", BN_bn2hex(q));
        printf("g %s\n", BN_bn2hex(g));
        printf("y %s\n", BN_bn2hex(y));
    }

    char *file_name1 = argv[2];
    char *file_name2 = argv[3];

    BIGNUM *r1, *s1, *r2, *s2;
    BIGNUM *h1 = BN_new();
    BIGNUM *h2 = BN_new();

    res = extract_data(file_name1, &r1, &s1, h1);
    if (res == -1)
    {
        printf("Could not extract data for file: %s", file_name1);
        return -1;
    }
    res = extract_data(file_name2, &r2, &s2, h2);
    if (res == -1)
    {
        printf("Could not extract data for file: %s", file_name2);
        return -1;
    }

    if (BN_cmp(r1, r2) != 0)
    {
        printf("r1 and r2 are the not the same, that means we didn't reuse k, this app's cryptanalysis won't work...\n");
        return -1;
    }

    BIGNUM *numerator = BN_new();
    res = BN_sub(numerator, h1, h2);
    if (res == 0)
    {
        printf("There was an error subtracting h1 and h2\n");
        return -1;
    }

    BIGNUM *denominator = BN_new();
    res = BN_sub(denominator, s1, s2);
    if (res == 0)
    {
        printf("There was an error subtracting s1 and s2\n");
        return -1;
    }

    BN_CTX *ctx = BN_CTX_new();

    BIGNUM *den_inv = BN_new();
    BN_mod_inverse(den_inv, denominator, q, ctx);

    BIGNUM *k = BN_new();
    res = BN_mod_mul(k, numerator, den_inv, q, ctx);
    if (res == 0)
    {
        printf("There was an error subtracting dividing the two values...\n");
        return -1;
    }

    if (debug == 1)
    {
        printf("ha - hb = %s\n", BN_bn2hex(numerator));
        printf("sa - sb = %s\n", BN_bn2hex(denominator));
        printf("denominator(^) inverse = %s\n", BN_bn2hex(den_inv));
        printf("k = %s\n", BN_bn2hex(k));
    }
    // Verify k

    BIGNUM *verify_k_first = BN_new(), *verify_k_2 = BN_new();
    if (!BN_mod_exp(verify_k_first, g, k, p, ctx))
        printf("There was an error: %s\n", ERR_error_string(ERR_get_error(), NULL));

    if (!BN_mod(verify_k_2, verify_k_first, q, ctx))
        printf("There was an error: %s\n", ERR_error_string(ERR_get_error(), NULL));

    if (BN_cmp(verify_k_2, r1))
    {
        printf("There was an error, k is not correct...\n");
        return -1;
    }

    // Now solving for x which is the private key: S = k^{-1}(H(m) + x*r) mod q

    BIGNUM *r_inv = BN_new();
    BN_mod_inverse(r_inv, r1, q, ctx);

    BIGNUM *x = BN_new(), *mult = BN_new(), *subt = BN_new();
    BN_mul(mult, s1, k, ctx);

    BN_sub(subt, mult, h1);

    BN_mul(x, subt, r_inv, ctx);

    BIGNUM *private_key = BN_new();

    if (!BN_mod(private_key, x, q, ctx))
    {
        char buffer[100];
        printf("Error: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return -1;
    }

    if (debug == 1)
    {
        printf("s * k = \t%s\n", BN_bn2hex(mult));
        printf("(s * k) - H(m) = \t%s\n", BN_bn2hex(subt));
        printf("((s * k) - H(m)) * r^{-1} = %s\n", BN_bn2hex(x));
        printf("The private key: %s\n", BN_bn2hex(private_key));
    }
    // Now we will verify the private key...

    DSA *d2 = DSA_new();
    BIGNUM *p2, *q2, *g2, *y2;
    FILE *privfile;

    p2 = BN_dup(p);
    q2 = BN_dup(q);
    g2 = BN_dup(g);
    y2 = BN_dup(y);

    DSA_set0_pqg(d2, p2, q2, g2);
    DSA_set0_key(d2, y2, private_key);

    privfile = fopen("../out/fake_private.pem", "w+");

    PEM_write_DSAPrivateKey(privfile, d2, NULL, NULL, 0, NULL, NULL);
}