# Q1

Run `q1.sh`

# Q2 - 8.6

A nonce, as we defined it in Section 8.2.3, is in most cases a psuedorandom number.

1. Explain why this means we cannot guarantee that a particular nonce has not been used before.

We cannot guarantee that a particular nonce has not been used before because each output from a psuedorandom generator is meant to have equal likelihood of being an output. And each probability should be independent of all previous probabilities (otherwise each output would NOT be equally likely). Therefore, since the generation of the nonce is one event, and we want all our events to be independent from all other events, then it's entirely possible that a nonce was generated, by chance, before.

For example, say we are generating a two bit nonce. The output options are $\{00, 01, 10, 11\}$, and say out psuedorandom generator function is called $\mathcal{F} = R^n \rightarrow \{0,1\}^2$. We cannot guarantee by ANY means that we will not generate the same nonce twice for multiple reasons! First, if we generate more than a few we would HIGHLY expect to have duplicates (given this small example), but also the generator function has no dependency on previous runs.

ANother reason why we cannot guarantee this is that a psuedorandom number is only as random as the seed provided to the generator. If the seed is for some reason compromised, or identical to a previous seed, we would also get a previously used nonce.

2. What should we do if the require a guarantee that each nonce is used at most once?

We could keep a storage of previously used nonces. Or we could include some counter information in the nonce.

3. The terms nonce and salt are used in different areas of cryptography (and not always consistently). Conduct some research on the use of these terms and suggest, in general,. what the difference is between them.

NIST defines a **nonce** as "A time-varying value that has at most a negligible chance of repeating, for example, a random value that is generated anew for each use, a timestamp, a sequence number, or some combination of these.". Whereas they define a **salt** as "A non-secret value used in a cryptographic process, usually to ensure that the results of computations for one instance cannot be reused by an attacker."

The differences and similarities between the two then is multifold:

Similarities:

1. They are both randomly generated
2. They are both not required to necessarily be secret

Differences:

1. A nonce is by definition randomly generated, while a salt does not necessarily have to be random
2. A nonce is used once, while a salt may be used many times
3. A nonce is a primitive that can provides freshness, while a salt is a primitive that can provide obfuscation.

# Q3 8.7

For each of the following, explain what problems might arise if we base a freshness mechanism on the suggested component.

1. an inaccurate clock

It is possible that Alice sends a message to Bob using some clock data from Alice's clock, but Bob's clock is actually 5 minutes fast, therefore Bob assumes the message was sent five minutes ago and might deny it as not fresh. Unless the two clocks could synchronize, this would result in the inability for Alice to EVER send a (fresh)message to Bob.

2. A sequence number which regularly cycles around

It would be very easy for an adversary, or an untrustworthy partner to replay a previous message rather than require they send a new one.

For example, say the last message Bob receives from Alice has sequence number 10, and the cycle is of size 11 (so the next sequence number should be \$0\$ \$[0...10]\$). Alice can either resend what she went before with sequence number 0, or some adversary can resend it! Also, if bob doesn't require sequences to be exactly in order, then Bob might accept anything between $[0,w]$ for some window $w$, allowing a large window of previously went messages to be potentially replayed.

3. A nonce which is randomly generated from a small space.

See the example from Q2, where the space is 2-bits. The likelihood of repeating nonces is just much higher.

# Q4 8.13

Passwords are stored on a computer in encrypted form are a potential attack target. Explain how encrypted passwords can be attacked by using:

1. an exhaustive search

We can simply attempt to type in all possible passwords (maybe the search space is much smaller because of some required password format) and see which one gets encrypted to the correct value (i.e. gives us access)

2. A dictionary attack

A malicious actor could install some malware on a computer that listens for password input and output and compiles a list a password, ciphertext pairs in some dictionary. Then, when it sees a new ciphertext that corresponds to a previously seen one it can derive the password.

The above attack would not work if the system for encrypting the password was also using salt, which most do I believe to keep this attack at bay.

If we add the salt into this "dictionary" the adversary might be able to perform some crypanalysis on those triples to discover what the key might be.

3. Rainbow tables

# TODO the book does not mention rainbow tables...

it appears that this is the same attack as above, but it pre-computes hashes, and is also unfeasible with salting.

# Q5 8.20

Explain why a stream cipher would be a poor choice for the encryption mechanism used to compute responses to challenges in a dynamic password scheme based on challenge-response.

I'm not entirely sure why...

Could be because of key management, i.e. stream cipher's need to use different keys each time they encrypt something.

The token needs to have a key-stream generating function that also agrees with the server. If these go out of sync then (because we are using different components of the key-stream consistently) then the encryption algorithms also go out of sync. (not sure if this is actually a real problem...)

# Q6

Zero-knowledge authentication is a fun topic in textbooks. Can you find examples of it being used in the real world? (Actual zero knowledge, not "cryptographically sign a challenge").

Currently ZKP is being used heavily in blockchain technology to provide verification without the need to reveal ones identity.

"In 2019, EY released Nightfall, a public ZKP protocol that allows companies to preserve confidentiality while conducting private and secure transactions on public blockchains." - https://blockheadtechnologies.com/zero-knowledge-proofs-a-powerful-addition-to-blockchain/

I'm not exactly sure HOW it is being used, but they say it is!
