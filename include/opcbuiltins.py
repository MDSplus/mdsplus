
import csv
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--opcodes', help='Path to opcodes.csv')

args = parser.parse_args()

input_file = open(args.opcodes, newline='')
output_file = open('opcbuiltins.h', 'wt')

output_file.write('''
/* <opcbuiltins.h> */
/*   name,                builtin,               f1,                  f2,       f3,               i1,          i2,         o1,         o2,         m1, m2,         token */
''')

reader = csv.DictReader(input_file)

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