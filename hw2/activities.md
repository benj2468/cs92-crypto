# Homework 2

## Q1 (5.7)

Certain RSA encryption exponents $e$ are regularly chosen because they have a special format. An example is $e = 2^{16}+ 1$.

1. What are the advantages of RSA exponents which have this type of format?

Exponentiation of this format is incredibly efficient. Because of the efficiency of repeated squaring.

2. Explain why it is not a problem if more than one user had the same encryption exponent $e$?

$e$ is part of the public key. Since the hard part is calculating $d$, which is dependent on the choice of $p$ and $q$ (which are both secret) and hard to find due to the difficulty of RSA, then knowing $e$, or having the same $e$ doesn't necessarily mean that you will have the same $d$.

For example, consider the example from the textbook: $p=3$, $q=7$, and another example: $p=2$, $q=7$. Now consider a valid public key for the first example: $(21, 5)$. And another valid public key for the second example $(14, 5)$ (it's easy to show that these are both valid keys). Even though they use the same value for $e$, even knowing both keys gives no further knowledge of the factors of $n$, other than they have different factors (which we already knew because we know that each number has a unique prime factorization). Thus using the same $e$ value twice serves no benefit to an adversary.

3. Why is it generally not advisable to choose RSA decryption keys $d$ which have a similar format?

because then they might be guessable... and if the format is known, then the search space can be dramatically decreased to make an exhaustive key search much easier. (or can open more back doors other than exhaustive key search depending on the actual format)

4. Under what special circumstances might there be a case for allowing decryption keys $d$ to have special formats (similar, but not necessarily identical to the above example)

I know we don't have to do this problem... but I'm curious what the answer is. I was thinking maybe if the information being sent is incredibly time-sensitive, or we are doing digital signatures, and swapping d and e?

## Q2 (5.11)

RSA and ElGamal both require the use of large primes. Find out how many primes can be generated which are sufficiently large for use in practical public-key cryptosystems.

- If we assume that practical public-key cryptosystems use 3072 bit modulus, that means each prime selected needs to be within 1536 bits. So we need to count how many primes exist below 2^1536, the prime number theorem states that the number of primes less than $x$ is roughly $x/ln(x)$, that implies that the number of primes less than $2^{1536}$ is roughly

$$\frac{2^{1536}}{ln(2^{1536})} = \frac{2^{1536}}{1536*ln(2)} \approx \frac{2^{1536}}{1064} \approx 2.2*10^{459}$$

(Math with help of Wolfram Alpha)

## Q3

See [elgamal.c](../src/elgamal.c)

## Q4 (6.7)

Pins are a commonly used security mecahnism

1. In a class of 120 students who have been assigned 4-digit PINS by their banks, is it more, or less likely that two students have the same PIN to protect their payment card?

More Likely

2.  Approximately how many students would you need to have in the class before a similar likelihood was expected if banks used 5-digit PINS instead?
    5-digit pins implies $10^5$ possible pins, which means that we would expect a collision after roughly $\sqrt(10^5)$ pin distributions, which would mean $10^{2.5} \approx 316$ students.

    The same math as above for part 1, implies 100 students as the threshold.

## Q5 (6.13)

(Consider a "false" message as one made up by the attacker.)

Explain the extent to which a MAC prevents the following attacks:

1.  unauthorized changing of part of message
    1.  This is prevented by MACs because if any part of the message is changed, the MAC calculated by the receiver will (ideally) not match the MAC sent in the message.
    2.  This is because the adversary cannot re-calculate a new MAC, because they don't know the MAC Key.
2.  unauthorized deletion of part of a message
    1.  Same reason as above...
3.  unauthorized sending of a false message
    1.  Same reason as above... The attacker can change part, or all of the message, but without the ability to calculate a new MAC, it is very unlikely that the original MAC will still match.
4.  unauthorized replaying of a previously sent message
    1.  If the message isn't changed, then the MAC wouldn't change either, so this is allowed by the protocol.
    2.  MACs on a message-by-message basis, so there is nothing in the MAC protocol that would prevent this. The receiver might want to guard against this, and could keep a running list of previously sent MACs/messages.

## Q6

1. Does a cryptographic hash function lead to a good bucket hash function?
   1. The desireable properties of a bucket hash function are as follows. We will use the properties of a cryptographic hash function to determine if all of the following properties are established:
      1. It can be computed quickly.
         1. yes, this follows from the second physical property of a cryptographic hash function: Easy to compute.
      2. It spreads the universe of keys fairly evenly over the table.
         1. While this is not strictly stated in the security properties, this can be derived from the third property. Consider a case where a cryptographic hash function did not spread it's values out evenly across it's universe (2^n, where n is the bit length of the functions output). In this case, finding a collision would be slightly easier. If the function were to preference suffixes, say 90% of hash outputs ended in a 0, that dramatically lessens the search space for example by heavily preferring odd numbers over even numbers.
         2. Then we must ask the mathematical question, if we are evenly distributed across the range $[0,2^n]$, does that mean we are also evenly distributed across the range $[0,2^l]$, for some $l << n$ (l much less than n)? The answer should be yes. If we were not evenly distributed across some subset, then clearly we are not evenly distributed across the entire set, therefore forming a proof by contradiction.
      3. Small changes in the key (e.g. changing a character in a string or changing the order of the letters) should usually result in a different hash value.
         1. This follows from a conversation of second-preimage resistance. If only small changes to $x$ were necessary to find a new $x'$ such that the hashes $h(x) = h(x')$, then the second pre-image resistance principle wouldn't hold.
         2. But again, if we are reducing to an even SMALLER space than the hash function intended to a certain number of buckets using the buckets as a modulus, are we actually getting the same distribution? Will a small change necessarily result in a different hash value? This is harder to guarantee, but we can try with some empirical data to see if this is the case.
2. Does a good bucket hash function (such as cs10_bhash above) lead to a good cryptographic hash function

   1. Lets check the traits of a cryptographic hash function, and see if they are provided by a bucket hash function.

      1. compress arbitrary length to fixed input
         1. yup, compresses to log(buckets) length output
      2. easy to compute
         1. yup
      3. pre-image resistance
         1. this is not necessarily provided by a bucket hash function.
         2. Take the bucket hash provided in the CS10 material, that simply computes the polynomial sum of an array of numbers.
         3. It's not terribly difficult to start filling in suspected numbers and a find a polynomial that creates a particular sum.

      Consider the example of us trying to has this array: $[1,2,3]$, using $a = 37$. (as suggested in the CS10 notes), with maybe 256 buckets.

      $$sum = 1 + 37*2 + 37^2 * 3 = 4182 mod 256 = 86$$

      pre-image resistance does not require that I find this SPECIFIC input, but just any input.

      But I know $37^2 mod 256 = 89$, which means I can EASILY find two more values to create a sum of 86, for example $[28, 13, 1]$.

      1. second pre-image resistance
         1. If we don't have pre-image resistance, then we can't possibly have second pre-image resistance. Finding a second can't be harder than finding one.
      2. Collision resistance
         1. Yes this is provided, since a good bucket hash function spreads it's outputs evenly across it's table.

If we look at the empirical evaluation performed in [q6](src/q6.c), we see that in fact we do get a relatively even distribution across the board, this implies that a good cryptographic hash function likely serves as a strong base for a good bucket hash function.
