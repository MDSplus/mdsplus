
import os
import csv

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

OPCODES_FILENAME = 'tdishr/opcodes.csv'
OUTPUT_FILENAME = 'include/opcbuiltins.h'

print("Generating '{}' from '{}'".format(OUTPUT_FILENAME, OPCODES_FILENAME))

with open(OUTPUT_FILENAME, 'wt') as output_file:
    with open(OPCODES_FILENAME) as opcodes_file:  # , newline=''

        output_file.write(
            '\n'
            '/* <opcbuiltins.h> */\n'
            '/*   name,                builtin,               f1,                  f2,       f3,               i1,          i2,         o1,         o2,         m1, m2,         token */\n'
        )

        reader = csv.DictReader(opcodes_file)

        for line in reader:
            output_file.write(
                'OPC( {:<20} {:<22} {:<20} {:<9} {:<17} {:<12} {:<11} {:<11} {:<11} {:<3} {:<11} {:<12} ) /* {} */\n'.format(
                    line['name'] + ',',
                    line['builtin'] + ',',
                    line['f1'] + ',',
                    line['f2'] + ',',
                    line['f3'] + ',',
                    line['i1'] + ',',
                    line['i2'] + ',',
                    line['o1'] + ',',
                    line['o2'] + ',',
                    line['m1'] + ',',
                    line['m2'] + ',',
                    line['token'],
                    line['comment']
                )
            )
