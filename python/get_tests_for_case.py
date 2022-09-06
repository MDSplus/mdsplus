#!/usr/bin/env python3

import ast
import sys
import re
import os

pattern = re.compile(r"TESTS\s*=\s*({[\w\s',]*})")

case_filename = sys.argv[1]
with open(case_filename) as file:
    content = file.read()
    
    match = pattern.search(content)
    tests = ast.literal_eval(match[1])
    
    print(';'.join(tests))
    
    # for test in tests:
    #     prefix = os.path.basename(case_filename)[:-8]
    #     test_filename = "%s_%s_test.py" % (prefix, test)
    #     print(case_filename, '->', test_filename)
    #     # os.symlink(filename, '')