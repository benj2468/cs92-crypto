# Precursor

1. When were the last five times you think you may have used cryptography?
   1. Whatsapp
   2. Login to Duo
   3. OTP for my job's login
   4. CryptoPass :)
   5. In today's HW :)
2. What three things do you hope to learn from this course?
   1. Application of modern cryptography
   2. What questions in cryptography we have not answered yet
   3. How to think about designing cryptographic systems
3. What color do you associate with cryptography?

# Chapter 1

14. We often lose perspective of very large numbers.

    1.  Place the following in order of increasing size:

        1.  number of possible 40-bit keys $2^{40} = 1.1 * 10^{12}$
        2.  number of possible 90-bit keys $2^{90} = 1.2 * 10^{27}$
        3.  number of possible 128-bit keys $2^{128} = 3.4 * 10^{38}$
        4.  number of web pages indexed by google $3.5 * 10^{13}$
        5.  number of stars in our galaxy - $1 * 10^{11}$
        6.  number of stars in the universe - $2 * 10^{23}$
        7.  number of species of bird on earth - $1.8 * 10^4$
        8.  number of seconds since the beginning of the universe - $4.36 * 10^{17}$

    2.  Answer:
        1.  number of species of bird on earth
        2.  number of stars in our galaxy
        3.  number of possible 40-bit keys
        4.  number of web pages indexed by google
        5.  number of seconds since the beginning of the universe
        6.  number of stars in the universe
        7.  number of possible 90-bit keys
        8.  number of possible 128-bit keys
    3.  For each of the above, identify how many bits would be required to represent symmetric keys in a keyspace of that size
        1.  number of species of bird on earth
            1.  $log_2(1.8 * 10^4) \approx 15$
        2.  number of stars in our galaxy
            1.  $log_2(1 * 10^{11}) \approx 37$
        3.  number of possible 40-bit keys
            1.  40
        4.  this is all trivial - just simply take the base two log of each value... we'll see that even though the numbers get really big, the number of bits levels off. We love exponents!

15. Encryption algorithm ALEX has a 40-bit key and encryption algorithm CARLOS has a 48-bit key. Assume you have sufficient computing power to use an exhaustive key search to find the key of ALEX in one day.

    1.  Assuming they have similar computational complexity, how long would you expect it to take to find the key of CARLOS by means of exhaustive search?
        1.  Assume that on average we find the key in $2^{n-1}$ tried.
        2.  Well, if we can find ALEX in one day, then we can search $2^{39}$ keys in 24 hours, using simply ratios, to determine how long it will take to check $2^{47}$ keys, we get $x = 2^{47} * 24 / 2^{39} = 2^{8} * 24 = 256 * 24$, so it would take 256 days.
    2.  Assume now that the (bad) design of CARLOS allows it to be run in two separate stages such that it is possible to conduct an exhaustive key search for the first 40-bits of CARLOS key and then perform a separate exhaustive key search for the last 8 bits. How long do you now expect to take to recover a CARLOS key by means of exhaustive search?
        1.  Well we know the first 40 bits will take 1 day, then we can expect the remaining 8 bits to take $2^7 * 24 / 2^{39} = 24 hours / 2^{31} = 0.004 milliseconds$

16. Explain whether the following scenarios are possible for a symmetric cryptosystem
    1.  two different plaintexts encrypt the same ciphertext under different keys.
        1. Yes, if there are two different keys, then the ciphertext output could very well be identical for different plaintexts.
    2.  two different plaintexts encrypt the same ciphertext under the same keys
        1. No. different plaintexts would need to encrypt different ciphertexts - otherwise the encryption function wouldn't be bijective with the key.
    3.  a plaintext encrypts the same ciphertext under different keys
        1. This is possible, but very unlikely.
17. 0011001010101010001110000100110100100101

# Chapter 2

3. The Simple Substitution Cipher is a major improvement on the Caesar Cipher
   1. In what different ways is it an improvement
      1. The key size is dramatically larger
   2. If a SSC is used and we intercept the ciphertext OXAO, then which of the following four-letter words could be the plaintext?
      1. JOHN - NO
      2. SKID - NO
      3. SPAS - YES
      4. LOOT - NO
      5. PLOP - YES
      6. OSLO - YES
   3. If a SSC us used and we intercept the ciphertext BRKKLK, then what do we know about the plaintext?
      1. we know that one letter is repeated 3 times
   4. suppose in the case of intercepting BRKKLK we also know that the plaintext is the name of a country. What is the plaintext?
      1. greece
   5. What important lesson does the BRKKLK example illustrate?
      1. that large key size does not mean that the cipher is secure.
4. The playfair cipher represents one improvement over the monoalphabetic ciphers
   1. decrypt the following ciphertext using the playfair key depicted in figure 2.4
      1. NR SH NA SR HF CG FL TN RW NS DN NF SK RW TN DS XN DS BR NA BI ND SN CR NT WO TQ FR BR HT BM FW MC
      2. THE NATTERJACK IS A TOAD NATIVE TO SANDY AND HEATHLAND AREAS OF NORTHERN EUROPE

# Chapter 3

1. The version of the one-time pad which operates on alphabetic letters (associating A=0, B=1, etc.) is being used, and the ciphertext DDGEXC is intercepted. (Code for this problem can be found in GQ1 - alphaOTPSolveKey)
   1. If the plaintext was BADGER, then was was the key?
      1. CDDYTL
   2. If the plaintext was WOMBAT, then what was the key?
      1. HPUDXJ
   3. How many different plaintexts could have resulted in this ciphertext if the plaintext is not necessarily an english word?
      1. For any given key, we can find a plaintext that would result in this ciphertext, therefore, there is a 1:1 relationship between the key-space and the possible plaintexts. Since the length of the ciphertext is 6 characters, the keyspace (and potential plaintext space) is: $26^6 = 308915776$
   4. How many different plaintexts could have resulted in this ciphertext if the plaintext is an english word.
      1. Since we can always find a key to translate from any english word into this ciphertext, the answer is simply the number of english 6 letter words.
      2. Estimate for 6-letter words in english is between 22,000 and 24,000
         1. https://wordfinder.yourdictionary.com/letter-words/6/
