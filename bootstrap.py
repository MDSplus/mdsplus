
import os
import sys

from subprocess import *

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# conf/update_submodules

def run(args):
    command = ' '.join(args)
    print(f"Running '{command}'")
    status = Popen(args).wait()
    if status != 0:
        print(f"Failed to run '{command}'")
        exit(1)

run([sys.executable, 'deploy/gen-messages-exceptions.py'])

run([sys.executable, 'deploy/gen-include-opcbuiltins.py'])

run([sys.executable, 'deploy/gen-python-compound.py'])

run([sys.executable, 'deploy/gen-tdishr-TdiHash.py'])

run([sys.executable, 'deploy/gen-yacc-lex.py'])