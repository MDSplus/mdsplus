#!/usr/bin/env python
import sys
import re

maxdepth = 7
root = dict()
curr = None
record = -1
filter = {'fun:UnknownInlinedFun', re.compile(r'fun:.+\.lto_priv\..+'), "obj:*"}


for filepath in sys.argv[1:]:
    with open(filepath, 'r') as f:
        gen = (l for l in f)
        for line in gen:
            line = line.strip()
            if record >= 0:
                if line == "}":
                    record = -1
                    curr = None
                elif record < maxdepth:
                    record += 1
                    curr = curr.setdefault(line, dict())
            elif line == "{":
                record = 0
                curr = root
                next(gen)


def print_level(curr, pre):
    if curr:
        for k, v in curr.items():
            print_level(v, pre + [k])
    else:
        while True:
            for flt in filter:
                if isinstance(flt, str):
                    if flt == pre[-1]:
                        break
                else:
                    if flt.search(pre[-1]):
                        break
            else:
                break
            if len(pre) <= 2:
                break
            pre.pop()
        print("{")
        print("   %s" % pre[-1].split(":", 2)[-1])
        for l in pre:
            print("   %s" % l)
        print("}")


print_level(root, [])
