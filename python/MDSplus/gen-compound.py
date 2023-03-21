#!/usr/bin/env python

import os
import sys
import csv
import argparse

parser = argparse.ArgumentParser()

parser.add_argument('--opcodes', help='Path to opcodes.csv')
parser.add_argument('--input', help='Path to compound.py.in')
parser.add_argument('--output', help='Path to write output to')

args = parser.parse_args()

forceref = ('dEXPT', 'dSHOT', 'dSHOTNAME', 'dDEFAULT', 'GETNCI',
            'MAKE_FUNCTION', 'BUILD_FUNCTION', 'EXT_FUNCTION'
            'MAKE_PROCEDURE', 'BUILD_PROCEDURE',
            'MAKE_ROUTINE', 'BUILD_ROUTINE',
            'MAKE_CALL', 'BUILD_CALL',
            'COMPILE', 'EXECUTE',
            )

opcodes_filename = args.opcodes
input_filename = args.input
output_filename = args.output

with open(output_filename, 'w+') as output_file:
    with open(input_filename, 'r') as input_file:
        for line in input_file:
            output_file.write(line)
    output_file.write('\n')

    output_file.write('MAX_DIMS = 8\n')
    
    with open(opcodes_filename, newline='') as input_file:
        reader = csv.DictReader(input_file)
        
        for line in reader:
            line['builtin'] = line['builtin'].replace("$", "d")
            has_m2 = int(eval(line['m2'].replace('MAX_DIMS', '8'))) > 0
            line['min_args_def'] = ('    min_args = %(m1)s\n' % line) if has_m2 else ''
            
            if line['builtin'] in forceref:
                line['Function'] = '_dat.TreeRef, Function'
            elif has_m2:
                line['Function'] = '_dat.TreeRefX, Function'
                line['min_args_def'] = '    min_args = %(m1)s\n' % line
            else:
                line['Function'] = 'Function'
            
            output_file.write((
                    'class %(builtin)s(%(Function)s):\n'
                    '    opcode = %(opcode)s\n'
                    '%(min_args_def)s'
                    '    max_args = %(m2)s\n'
                    ) % line)
            
            if line['builtin'] in ('BUILD_RANGE', 'MAKE_RANGE'):
                output_file.write((
                    '    def __init__(self, *args):\n'
                    '        if len(args)==1 and args[0] is self:\n'
                    '            return\n'
                    '        if len(args)==1 and isinstance(args, (slice,)):\n'
                    '            super(%(builtin)s, self).__init__(args[0].start, args[0].stop, args[0].step)\n'
                    '        else:\n'
                    '            super(%(builtin)s, self).__init__(*args)\n\n'
                ) % line)
            elif line['builtin'] in ('PRIVATE', 'PUBLIC'):
                output_file.write(
                    '    def assign(self, value):\n'
                    '        EQUALS(self, value).evaluate()\n'
                    '        return self\n'
                )
            else:
                output_file.write('\n')
                
    output_file.write(
        '_c = None\n'
        'for _c in globals().values():\n'
        '     if isinstance(_c,Function.__class__) and issubclass(_c,Function) and _c is not Function:\n'
        '         Function.opcodeToClass[_c.opcode]=_c\n'
        'del(_c)\n'
    )
    
