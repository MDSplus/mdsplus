
import os
import csv
import shutil

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

OPCODES_FILENAME = 'tdishr/opcodes.csv'
OUTPUT_FILENAME = 'include/tdishr.h'

print("Generating '{}' from '{}'".format(OUTPUT_FILENAME, OPCODES_FILENAME))

opcode_names = []
with open(OPCODES_FILENAME) as opcodes_file: # , newline=''
    reader = csv.DictReader(opcodes_file)

    for line in reader:
        opcode_names.append(line['name'])

with open(OUTPUT_FILENAME, 'wt') as output_file:
    output_file.write(
        '#ifndef _TDISHR_H\n'
        '#define _TDISHR_H\n'
        '\n'
        '#include <mdsdescrip.h>\n'
        '#include <mdstypes.h>\n'
        '#include <status.h>\n'
        '#include <tdishr_messages.h>\n'
        '\n'
        'extern int TdiGetLong(struct descriptor *indsc, int *out_int);\n'
        'extern int TdiGetFloat(struct descriptor *index, float *out_float);\n'
        'extern int TdiConvert();\n'
        'extern int TdiIntrinsic(\n'
        '    opcode_t opcode,\n'
        '    int narg,\n'
        '    struct descriptor *list[],\n'
        '    struct descriptor_xd *out_ptr\n'
        ');\n'
        'extern int _TdiIntrinsic(\n'
        '    void **ctx,\n'
        '    opcode_t opcode,\n'
        '    int narg,\n'
        '    struct descriptor *list[],\n'
        '    struct descriptor_xd *out_ptr\n'
        ');\n'
        'extern int CvtConvertFloat(\n'
        '    void *invalue,\n'
        '    uint32_t indtype,\n'
        '    void *outvalue,\n'
        '    uint32_t outdtype,\n'
        '    uint32_t options\n'
        ');\n'
        '\n'
    )

    for name in opcode_names:
        output_file.write('extern const opcode_t Opc{};\n'.format(name))

    output_file.write('\n')

    for name in opcode_names:
        output_file.write(
            'extern int Tdi{}(struct descriptor *first, ...);\n'.format(name))

    output_file.write('\n')

    for name in opcode_names:
        output_file.write(
            'extern int _Tdi{}(void **ctx, struct descriptor *first, ...);\n'.format(name))

    output_file.write('\n')
    output_file.write('#endif\n')
