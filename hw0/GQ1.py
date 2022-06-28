from string import ascii_lowercase


def word_to_int(word):
    h = ""
    for c in word:
        h += hex(ord(c))[2:]
    return int(h, 16)


def int_to_word(num):
    hex_word = hex(num)[2:]
    w = ""
    for i in range(0, 16, 2):
        w += chr(int(hex_word[i : i + 2], 16))
    return w


words = set()
with open("./words", "r") as f:
    for w in f.readlines():
        word = w.strip().lower()
        if len(word) == 8:
            words.add(word_to_int(word))


def part1(a, b):
    assert len(a) == len(b)
    a_int = int(a, 16)
    b_int = int(b, 16)
    diff = a_int ^ b_int

    for word in words:
        if (word ^ diff) in words:
            return int_to_word(word), int_to_word(word ^ diff)


def part2(meaning):
    cipher = "010101101111011101110111111011101011010110101110110010111001010101011011011011110111111101101101"

    # Find a key that does it...
    meaning_int = word_to_int(meaning)
    cipher_int = int(cipher, 2)

    key = cipher_int ^ meaning_int
    return hex(key)


def alphaOTPSolveKey(plain, cipher):
    assert len(plain) == len(cipher)
    key = ""
    for i in range(len(plain)):
        diff = (
            ascii_lowercase.find(cipher[i].lower())
            - ascii_lowercase.find(plain[i].lower())
        ) % 26
        print(diff)
        key += ascii_lowercase[diff]
    return key


print(">>> Part 1: ", part1("e93ae9c5fc7355d5", "f43afec7e1684adf"))
print(">>> Part 2: ", part2("Da worst Ivy"))

print(alphaOTPSolveKey("WOMBAT", "DDGEXC"))
