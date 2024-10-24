
import os
import csv
import shutil

from subprocess import *

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

OPCODES_FILENAME = 'tdishr/opcodes.csv'
OUTPUT_FILENAME = 'tdishr/TdiHash.c'
INTERMEDIARY_FILENAME = 'tdishr/TdiHash.c.in'

if 'GPERF' in os.environ:
    gperf = os.environ['GPERF']
else:
    gperf = shutil.which('gperf')

if gperf is None:
    print('Please install gperf, or set the GPERF environment variable')
    exit(1)

# Generate the input file for gperf
print(f"Generating '{INTERMEDIARY_FILENAME}' from '{OPCODES_FILENAME}'")
intermediary_file = open(INTERMEDIARY_FILENAME, 'wt')

intermediary_file.write('''
%language=C
%ignore-case
%compare-strncmp
%7bit
%pic
%includes
%readonly-tables
%struct-type
%define slot-name name
%define initializer-suffix ,-1
%{
#include "tdirefcat.h"
#include "tdireffunction.h"
#ifdef _WIN32
// Windows uses long to cast position and cause a compiler warning
#define long size_t
#endif
%}
struct fun { int name; int idx; };
%%
''')

# Generate a list of opcodes for gperf to process
# Each one must be in the format of `string,index`

opcodes_file = open(OPCODES_FILENAME, newline='')
reader = csv.DictReader(opcodes_file)

opcode_lines = []
for line in reader:
    opcode_lines.append(
        '{},{}'.format(
            line['builtin'],
            line['opcode']
        )
    )

intermediary_file.write('\n'.join(opcode_lines))

intermediary_file.write('''
%%
int tdi_hash(const int len, const char *const pstring)
{
	const struct fun *fun = in_word_set(pstring, len);
	return fun ? fun->idx : -1;
}
''')

intermediary_file.close()

# Process the input file with gperf
proc = Popen(
    f'{gperf} {INTERMEDIARY_FILENAME}'.split(),
    stdin=PIPE,
    stdout=PIPE,
)

stdout = proc.communicate()[0].decode()
lines = stdout.split('\n')

# Remove '#line' directives to simplify debugging
output_lines = []
for line in lines:
    if line.startswith('#line'):
        continue

    output_lines.append(line)

# Output the final product
print(f"Generating '{OUTPUT_FILENAME}' from '{INTERMEDIARY_FILENAME}'")
output_file = open(OUTPUT_FILENAME, 'wt')

output_file.write('\n'.join(output_lines))

os.remove(INTERMEDIARY_FILENAME)
