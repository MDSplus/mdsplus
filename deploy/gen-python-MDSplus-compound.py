
import os
import csv

# Move to the root of the repository
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

OPCODES_FILENAME = 'tdishr/opcodes.csv'
INPUT_FILENAME = 'python/MDSplus/compound.py.in'
OUTPUT_FILENAME = 'python/MDSplus/compound.py'

FORCEREF_BUILTINS = (
    'dEXPT', 'dSHOT', 'dSHOTNAME', 'dDEFAULT', 'GETNCI',
    'MAKE_FUNCTION', 'BUILD_FUNCTION', 'EXT_FUNCTION',
    'MAKE_PROCEDURE', 'BUILD_PROCEDURE',
    'MAKE_ROUTINE', 'BUILD_ROUTINE',
    'MAKE_CALL', 'BUILD_CALL',
    'COMPILE', 'EXECUTE',
)

print("Generating '{}' from '{}' and '{}'".format(OUTPUT_FILENAME, INPUT_FILENAME, OPCODES_FILENAME))
with open(OUTPUT_FILENAME, 'w+') as output_file:
    with open(INPUT_FILENAME, 'r') as input_file:
        for line in input_file:
            output_file.write(line)
    output_file.write('\n')

    output_file.write('MAX_DIMS = 8\n')

    with open(OPCODES_FILENAME) as input_file:  # , newline=''
        reader = csv.DictReader(input_file)

        for line in reader:
            line['builtin'] = line['builtin'].replace("$", "d")
            has_m2 = int(eval(line['m2'].replace('MAX_DIMS', '8'))) > 0
            line['min_args_def'] = (
                '    min_args = %(m1)s\n' % line) if has_m2 else ''

            if line['builtin'] in FORCEREF_BUILTINS:
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
