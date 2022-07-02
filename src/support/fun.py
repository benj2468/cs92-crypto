from typing import Generator, Tuple


def is_coprime(a: int, b: int) -> bool:
    m = min(a, b)
    for i in range(2, m + 1):
        if (a % i == 0) and (b % i == 0):
            return False

    return True


def find_coprimes(num: int) -> Generator[int, None, None]:
    for i in range(2, num):
        if is_coprime(i, num):
            yield i


def gcd(a: int, b: int) -> int:
    if a == 0:
        return b

    return gcd(b % a, a)


def solve_coeff(a: int, b: int, s: int, t: int) -> Tuple[int, int]:
    return (t - (b // a) * s, s)


def egcd(a: int, b: int) -> Tuple[int, int, int]:
    if a == 0:
        return b, 0, 1

    gcd, s1, s2 = egcd(b % a, a)

    s, t = solve_coeff(a, b, s1, s2)

    return gcd, s, t


print(egcd(7, 178238443))
