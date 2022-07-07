#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "includes/crypto.h"

unsigned int p = 0x3b23cf2b;

unsigned int g = 2;

int poker = 0;
int debug = 0;

typedef struct PrivateKey
{
    unsigned int x;
} PrivateKey;

typedef struct PublicKey
{
    unsigned int y;
} PublicKey;

typedef struct ElGamalCipher
{
    unsigned int c1, c2;
} ElGamalCipher;

int find_generator(unsigned int p)
{
}

int generate_keys(unsigned int *priv_key, unsigned int *pub_key)
{

    unsigned int x = 1;
    while (x <= 1)
    {
        x = mod(rand(), p);
    }

    unsigned int y = mod_exp(g, x, p);

    *pub_key = y;
    *priv_key = x;

    if (debug == 1 && poker == 0)
        printf("KeyGen... y = 0x%08x\n", y);
    if (debug == 1 && poker == 0)
        printf("KeyGen... x = 0x%08x\n", x);

    return 0;
}

int elgamal_encrypt(unsigned int message, unsigned int key, unsigned int *k, ElGamalCipher *cipher)
{

    if (k == NULL)
        k = mod(rand(), p);

    if (debug == 1 && poker == 0)
        printf("Encrypting... k = 0x%08x\n", k);

    unsigned int c1 = mod_exp(g, k, p);
    unsigned int c2 = mod((unsigned long)message * (unsigned long)mod_exp((unsigned long)key, (unsigned long)k, p), p);

    cipher->c1 = c1;
    cipher->c2 = c2;

    if (debug == 1 && poker == 0)
        printf("Encrypted... c1 = 0x%08x\n", c1);
    if (debug == 1 && poker == 0)
        printf("Encrypted... c2 = 0x%08x\n", c2);

    return 0;
}

int elgamal_decrypt(ElGamalCipher *cipher, unsigned int key, unsigned int *plaintext)
{

    unsigned long step_1 = mod_exp((unsigned long)cipher->c1, (unsigned long)key, p);

    if (debug == 1 && poker == 0)
        printf("Step 1 Complete... = 0x%08x\n", step_1);

    unsigned long x, y;
    int gcd = extended_gcd(step_1, p, &x, &y);

    unsigned int inverse = mod(x, p);

    if (debug == 1 && poker == 0)
        printf("Inverse: 0x%08x\n", inverse);

    unsigned int step_2 = mod((unsigned long)cipher->c2 * (unsigned long)inverse, p);

    if (debug == 1 && poker == 0)
        printf("Step 2 Complete... = 0x%08x\n", step_2);

    *plaintext = step_2;

    return 0;
}

int play_poker()
{
    unsigned int bob_c1, alice_c1;

    int res;
    unsigned int bob_priv_key, bob_pub_key;
    res = generate_keys(&bob_priv_key, &bob_pub_key);

    unsigned int alice_priv_key, alice_pub_key;
    res = generate_keys(&alice_priv_key, &alice_pub_key);

    int num_cards = 16;
    unsigned int cards[num_cards];
    for (unsigned int i = 0; i < num_cards; i++)
    {
        cards[i] = i;
    }

    unsigned int bob_encryption_k, alice_encryption_k;
    bob_encryption_k = mod(rand(), p);

    ElGamalCipher encrypted_cards[num_cards];
    ElGamalCipher *encrypted_card = malloc(sizeof(ElGamalCipher));
    for (int i = 0; i < num_cards; i++)
    {
        unsigned int card = cards[i];

        elgamal_encrypt(card, bob_pub_key, bob_encryption_k, encrypted_card);
        memcpy(&encrypted_cards[i], encrypted_card, sizeof(ElGamalCipher));
    }
    bob_c1 = encrypted_card->c1;

    // shuffling the array

    int rand_loc;
    for (int i = num_cards - 1; i > 0; i--)
    {
        rand_loc = (rand() % i);
        if (debug == 1)
            printf("\t* Shuffling Card %d --> %d\n", rand_loc, i);
        memcpy(encrypted_card, &encrypted_cards[i], sizeof(ElGamalCipher));
        memcpy(&encrypted_cards[i], &encrypted_cards[rand_loc], sizeof(ElGamalCipher));
        memcpy(&encrypted_cards[rand_loc], encrypted_card, sizeof(ElGamalCipher));
    }

    // BOB has encrypted the cards with his PRIVATE KEY
    //
    // Now ALICE will select a card for BOB, and select a card for herself (encrypting the one for herself with her own public key)

    int alice_card, bob_card;
    bob_card = (rand() % num_cards);
    do
    {
        alice_card = (rand() % num_cards);
    } while (alice_card == bob_card);

    printf("ALICE selected %d for BOB\n", bob_card);
    printf("ALICE selected %d for ALICE\n", alice_card);

    elgamal_encrypt(encrypted_cards[alice_card].c2, alice_pub_key, NULL, encrypted_card);
    alice_c1 = encrypted_card->c1;

    // Now ALICE sends bob_card and encrypted_card back to BOB
    //
    // Bob can now calculate his own card... and then send the card

    unsigned int bobs_card;
    elgamal_decrypt(&encrypted_cards[bob_card], bob_priv_key, &bobs_card);
    printf("Bob's card: %d\n", bobs_card);

    // Bob also decrypts his component of the encryption on alice's card...
    unsigned int alices_card;

    encrypted_card->c1 = bob_c1;
    elgamal_decrypt(encrypted_card, bob_priv_key, &alices_card);

    // Then BOB sends that card back to ALICE for her to figure out which one she selected...

    encrypted_card->c2 = alices_card;
    encrypted_card->c1 = alice_c1;
    elgamal_decrypt(encrypted_card, alice_priv_key, &alices_card);

    printf("Alice's card: %d\n", alices_card);

    return 0;
}

int main(int argc, char **argv)
{

    for (int i = 0; i < argc; i++)
    {
        if (strcmp("-poker", argv[i]) == 0)
            poker = 1;
        if (strcmp("-d", argv[i]) == 0)
            debug = 1;
    }

    // Setup random...
    srand(time(0));

    if (poker == 1)
        return play_poker();

    unsigned int priv_key, pub_key;

    int res = generate_keys(&priv_key, &pub_key);
    ERROR_CHECK

    while (1)
    {
        printf("Enter Message:\n");

        unsigned char *msg_str = malloc(10);
        scanf("%s", msg_str);

        unsigned int message = atoi(msg_str);
        free(msg_str);

        printf("Message = 0x%08x\n", message);

        if (message > (1 << 31))
        {
            printf("Message is too large... must be less than 15 bits\n");
            continue;
        }

        ElGamalCipher *cipher = malloc(sizeof(ElGamalCipher));

        res = elgamal_encrypt(message, pub_key, NULL, cipher);
        ERROR_CHECK

        unsigned int plaintxt;
        res = elgamal_decrypt(cipher, priv_key, &plaintxt);
        ERROR_CHECK

        if (plaintxt != message)
        {
            printf("There is an error... did not decode the correct message");
        }
        break;
    }

    return 1;
}