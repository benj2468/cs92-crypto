from collections import defaultdict
from matplotlib import pyplot as pyt

chars = []
histogram = defaultdict(lambda: 0)

with open("mobydick.txt") as f:
    for line in f.readlines():
        for c in line.strip():
            if c.isalpha():
                chars.append(c)
                histogram[c] += 1


values = sorted(histogram.items(), key=lambda x: -x[1])

pyt.bar(list(map(lambda x: x[0], values)), list(map(lambda x: x[1], values)))

pyt.show()
