#!/usr/bin/python

import sys
import re

if len(sys.argv) < 3:
    exit(-1)

in_file = sys.argv[1]
out_file = sys.argv[2]

res = []

with open(in_file) as fs:
    txt = fs.read()
    i = 0

    splits = txt.split("-----END CERTIFICATE-----\n-----BEGIN CERTIFICATE-----")

    for s in splits[1:]:
        s = s.replace("-----BEGIN CERTIFICATE-----\n", "")
        s = s.replace("\n-----END CERTIFICATE-----", "")
        res.append(f"-----BEGIN CERTIFICATE-----{s}-----END CERTIFICATE-----")

with open(out_file, "w") as fs:
    fs.write("\n".join(res))

exit(0)
