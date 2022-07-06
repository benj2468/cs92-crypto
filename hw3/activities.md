# q1

see [cbc_mac.c](../src/cbc_mac.c), run `make cbc_mac` to build and then ./cbc_mac to run. Make sure you are doing this from ../src.

# q2

see [q2.sh](./src/q2.sh)

# q3 & 4

see [hw3.c](./src/hw3.c), run with -q34 flag, and provide file with -f flag, after making: `make`

They do in fact match! Though formatted slightly differently.

See the verification when running...

# q5

see [q5.sh](./src/q5.sh)

# q6

see [q6.sh](./src/q6.sh)

The code will tell you which one you signed...

By running a quick diff, we notice that the files only differ in the 2nd line:

```bash
$ diff jabber1.txt jabber2.txt
2c2
<       Did gyre and gimble in the wabe:
---
>         Did gyre and gimble in the wabe:
```

This would imply that standards are incredibly strict! That standards for digital signatures on text do not only take into account the actual text that is written, but also the format on that text! In this example, no change to the text, but simple a change in the formatting of the text results in a different digital signature. This is a good thing -- because the integrity of the message (the text & it's formatting) are maintained as a part of the signature. i.e. I sign the text & it's formatting, not just the text.

# Running code

Run all bash scripts as is... The C code is explained how to run.
