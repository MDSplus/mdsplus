
import os
import csv
import shutil

from subprocess import *

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

OPCODES_FILENAME = 'tdishr/opcodes.csv'
OUTPUT_FILENAME = 'include/tdishr.h'

print("Generating '{}' from '{}'".format(OUTPUT_FILENAME, OPCODES_FILENAME))

opcodes_file = open(OPCODES_FILENAME) # , newline=''
reader = csv.DictReader(opcodes_file)

opcode_names = []
for line in reader:
    opcode_names.append(line['name'])

output_file = open(OUTPUT_FILENAME, 'wt')
output_file.write('''#ifndef _TDISHR_H
#define _TDISHR_H
                  
#include <mdsdescrip.h>
#include <mdstypes.h>
#include <status.h>
#include <tdishr_messages.h>

extern int TdiGetLong(struct descriptor *indsc, int *out_int);
extern int TdiGetFloat(struct descriptor *index, float *out_float);
extern int TdiConvert();
extern int TdiIntrinsic(
    opcode_t opcode,
    int narg,
    struct descriptor *list[],
    struct descriptor_xd *out_ptr
);
extern int _TdiIntrinsic(
    void **ctx,
    opcode_t opcode,
    int narg,
    struct descriptor *list[],
    struct descriptor_xd *out_ptr
);
extern int CvtConvertFloat(
    void *invalue,
    uint32_t indtype,
    void *outvalue,
    uint32_t outdtype,
    uint32_t options
);

''')

for name in opcode_names:
    output_file.write('extern const opcode_t Opc{};\n'.format(name))

output_file.write('\n')

for name in opcode_names:
    output_file.write('extern int Tdi{}(struct descriptor *first, ...);\n'.format(name))

output_file.write('\n')

for name in opcode_names:
    output_file.write('extern int _Tdi{}(void **ctx, struct descriptor *first, ...);\n'.format(name))

output_file.write('\n')
output_file.write('#endif\n')