
import os
import csv
import shutil

import subprocess

# For compatability with python < 3.3
if not hasattr(shutil, 'which'):
    def which(cmd):
        try:
            which_result = subprocess.check_output(['which', cmd])
        except subprocess.CalledProcessError:
            return None
        return which_result.strip()

    shutil.which = which

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
print("Generating '{}' from '{}'".format(INTERMEDIARY_FILENAME, OPCODES_FILENAME))
with open(INTERMEDIARY_FILENAME, 'wt') as intermediary_file:

    intermediary_file.write(
        '\n'
        '%language=C\n'
        '%ignore-case\n'
        '%compare-strncmp\n'
        '%7bit\n'
        '%pic\n'
        '%includes\n'
        '%readonly-tables\n'
        '%struct-type\n'
        '%define slot-name name\n'
        '%define initializer-suffix ,-1\n'
        '%{\n'
        '#include "tdirefcat.h"\n'
        '#include "tdireffunction.h"\n'
        '#ifdef _WIN32\n'
        '// Windows uses long to cast position and cause a compiler warning\n'
        '#define long size_t\n'
        '#endif\n'
        '%}\n'
        'struct fun { int name; int idx; };\n'
        '%%\n'
    )

    # Generate a list of opcodes for gperf to process
    # Each one must be in the format of `string,index`

    opcode_lines = []
    with open(OPCODES_FILENAME) as opcodes_file:  # , newline=''
        reader = csv.DictReader(opcodes_file)

        for line in reader:
            opcode_lines.append(
                '{},{}'.format(
                    line['builtin'],
                    line['opcode']
                )
            )

    intermediary_file.write('\n'.join(opcode_lines))

    intermediary_file.write(
        '\n'
        '%%\n'
        'int tdi_hash(const int len, const char *const pstring)\n'
        '{\n'
        '    const struct fun *fun = in_word_set(pstring, len);\n'
        '    return fun ? fun->idx : -1;\n'
        '}\n'
    )

# Process the input file with gperf
proc = subprocess.Popen(
    ['gperf', INTERMEDIARY_FILENAME],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
)

stdout = proc.communicate()[0].decode()
lines = stdout.split('\n')

os.remove(INTERMEDIARY_FILENAME)

# Remove '#line' directives to simplify debugging
output_lines = []
for line in lines:
    if line.startswith('#line'):
        continue

    output_lines.append(line)

# Output the final product
print("Generating '{}' from '{}'".format(OUTPUT_FILENAME, INTERMEDIARY_FILENAME))
with open(OUTPUT_FILENAME, 'wt') as output_file:
    output_file.write('\n'.join(output_lines))
