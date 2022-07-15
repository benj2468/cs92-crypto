from __future__ import annotations
from copy import deepcopy
from typing import List


class QuBit:
    def __init__(self, prob=0.5) -> None:
        self.prob = prob

    def __str__(self) -> str:
        return str(self.prob)

    def __add__(self, o) -> QuBit:
        return deepcopy(self)


class QuInt:
    def __init__(self, n: int, bits=None) -> None:
        self.n_bits = n
        self.bits = bits if bits != None else [QuBit() for _ in range(0, n)]

    def __str__(self) -> str:
        return f'<{",".join(map(str,self.bits))}>'

    def _add(a, b):
        res = []
        for i in range(len(a)):
            bit_a = a[i]
            bit_b = b[i] if i < len(b) else 0
            res.append(bit_a + bit_b)
        return res

    def __add__(self, o) -> QuInt:
        r_bits = []
        if type(o) == int:
            bits = [int(i) for i in bin(o)[2:]]
            r_bits = QuInt._add(self.bits, bits)
        elif type(o) == QuInt:
            r_bits = QuInt._add(self.bits, o.bits)
        return QuInt(self.n_bits, r_bits)

    def __mul__(self, o) -> QuInt:
        if type(o) == int:
            if o == 2:
                r_bits = list(self.bits)[1:] + [QuBit(0)]
            else:
                ##...

        return QuInt(self.n_bits, r_bits)


print(QuInt(5) + 5)
print(QuInt(5) * 5)
