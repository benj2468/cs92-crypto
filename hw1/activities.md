# Chapter 4

### Problem 7

Triple DES is just one idea for creatinga stronger block cipher using DES. an alternative idea is DES-X, which is an encryption algorithm built from DES in the following way:

1.  Let $K_1$ and $K_2$ be two 64-bit keys, and let $K$ be a DES key.
2.  Let $DES_K(P)$ denote encryption of plaintext $P$ using normal single DES with key $K$
3.  For a plaintext $P$ we define the encryption of $P$ using DES-X to be

$$C = K_2 \oplus DES_K(P\oplus K_1)$$

1.  What is the effective length of a DES-X key?
    1. $64 + 64 + 56 = 128 + 56 = 184$
1.  Explain how to decrypt a ciphertext using DES-X
    1. There are 3 operations, first, in order to get back the sub-cipher text that is $DES_K(P\oplus K_1)$ we perform XOR $C$ with $K_2$ (which is obviously a shared key). Next, perform a normal $DES$ decryption with key $K$ to receive $P \oplus K_1$. Then perform the final XOR with $K_1$ to get $P$.
1.  Is DES-X backwards compatible with single DES?

    1. No because keys are of different length?

### Problem 17

For each of the modes of operation discussed in Section 4.6, what is the effect of a 2-bit transmission error in the ciphertext if the two erroneous bits occur:

1. on the same ciphertext block
   1. For ECB this will cause that block to have erroneous plaintext after decryption, but only that block.
   2. For CBC, this is the same as one bit flip, so it will cause 2-blocks to have erroneous output.
   3. The same goes for CFB
2. on two consecutive ciphertext blocks?
   1. For ECB this will cause both of those blocks to have erroneous plaintext after decryption
   2. For CBC, and for CFB, this will cause 3 blocks to have erroneous plaintext. **PROVE THIS**
3. On two ciphertext blocks separated by at least one error-free ciphertext block
   1. ECB - just those blocks
   2. CBC and CFB - 4 block - this is effectively the same analysis as just one block being wrong, but in different locations (since each is only dependent on the validity of the prior)

### Q3

See (des.c)[des.c]