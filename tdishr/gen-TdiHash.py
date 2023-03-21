
import os
from subprocess import *
import csv
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--opcodes', help='Path to opcodes.csv')
parser.add_argument('--gperf', help='Path to gperf executable')
parser.add_argument('--output', help='Path to write output to')

args = parser.parse_args()

opcodes_filename = args.opcodes
gperf_executable = args.gperf
output_filename = args.output

# Generate the input file for gperf
intermediary_filename = 'TdiHash.c.in'
intermediary_file = open(intermediary_filename, 'wt')

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

opcodes_file = open(opcodes_filename, newline='')
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
    f'{gperf_executable} {intermediary_filename}'.split(' '),
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
output_file = open(output_filename, 'wt')

output_file.write('\n'.join(output_lines))

os.remove(intermediary_filename)
