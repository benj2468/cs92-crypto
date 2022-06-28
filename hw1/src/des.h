char *bin_to_string(const unsigned char input[], int nbytes);

void xor (const unsigned char *block_a, const unsigned char *block_b, unsigned char *output, unsigned char nbytes);

void des_encrypt(unsigned char block[8], unsigned char key[8], unsigned char output[8]);
