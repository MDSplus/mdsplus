
import os
from subprocess import *

# C compiler to use
cc = os.environ['CC']

# C compiler flags, likely -I/path/to/include
cflags = os.environ['CFLAGS']

# gperf exectuable to use
gperf = os.environ['GPERF']

# Generate a list of opcodes
proc = Popen(
    f'{cc} {cflags} -E -x c -'.split(' '),
    stdin=PIPE,
    stdout=PIPE,
)

preproc = b'''
#define COM
#define OPC(name, cmd,...) cmd,__LINE__-25
#include <opcbuiltins.h>
'''

stdout = proc.communicate(preproc)[0].decode()
lines = stdout.split('\n')

# Empty lines and comments will cause errors with gperf
output_lines = []
for line in lines:
    if line.startswith('#') or len(line.strip()) == 0:
        continue

    output_lines.append(line)

# Generate the input file for gperf
input_filename = 'TdiHash.c.in'
input_file = open(input_filename, 'wt')

input_file.write('''
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

# Insert the opcodes as part of the input to gperf
input_file.write('\n'.join(output_lines))

input_file.write('''
%%
int tdi_hash(const int len, const char *const pstring)
{
	const struct fun *fun = in_word_set(pstring, len);
	return fun ? fun->idx : -1;
}
''')

input_file.close()

# Process the input file with gperf
proc = Popen(
    f'{gperf} {input_filename}'.split(' '),
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
output_filename = 'TdiHash.c'
output_file = open(output_filename, 'wt')

output_file.write('\n'.join(output_lines))