#!/usr/bin/env python
"""
call 'python deploy/build_python_compound.py'
edit 'python/MDSplus/compound.py' and trunk file below '### deploy/build_python_compound.py ###'
call 'cat /tmp/compound_tail.py >> python/MDSplus/compound.py'
"""

import os
import sys

forceref = ('$EXPT', '$SHOT', '$SHOTNAME', '$DEFAULT', 'GETNCI',
            'MAKE_FUNCTION', 'BUILD_FUNCTION', 'EXT_FUNCTION'
            'MAKE_PROCEDURE', 'BUILD_PROCEDURE',
            'MAKE_ROUTINE', 'BUILD_ROUTINE',
            'MAKE_CALL', 'BUILD_CALL',
            'COMPILE', 'EXECUTE',
            )

srcdir = os.path.realpath(os.path.dirname(__file__) + '/..')
filepath = srcdir + '/include/opcbuiltins.h'
target = srcdir + '/python/MDSplus/compound.py'
print('Parsing %s to generate %s.' % (filepath, target))


def opc(f=sys.stdout):
    opcnames = ('name', 'NAME', 'f1', 'f2', 'f3', 'i1',
                'i2', 'o1', 'o2', 'm1', 'm2', 'token')
    opcode = 0
    f.write('MAX_DIMS = 8\n')
    with open(filepath, 'r') as opcf:
        for line in opcf:
            if not line.startswith('OPC ('):
                continue
            args = line[5:].split(')', 2)[0].split(',', 12)
            vars = dict(zip(opcnames, tuple(arg.strip('\t ') for arg in args)))
            vars['opcode'] = opcode
            vars['dNAME'] = vars['NAME'].replace("$", "d")
            has_m2 = int(eval(vars['m2'].replace('MAX_DIMS', '8'))) > 0
            vars['min_args_def'] = (
                '    min_args = %(m1)s\n' % vars) if has_m2 else ''
            if vars['NAME'] in forceref:
                vars['Function'] = '_dat.TreeRef, Function'
            elif has_m2:
                vars['Function'] = '_dat.TreeRefX, Function'
                vars['min_args_def'] = '    min_args = %(m1)s\n' % vars
            else:
                vars['Function'] = 'Function'
            f.write((
                    'class %(dNAME)s(%(Function)s):\n'
                    '    opcode = %(opcode)d\n'
                    '%(min_args_def)s'
                    '    max_args = %(m2)s\n'
                    ) % vars)
            if vars['NAME'] in ('BUILD_RANGE', 'MAKE_RANGE'):
                f.write((
                    '    def __init__(self, *args):\n'
                    '        if len(args)==1 and args[0] is self:\n'
                    '            return\n'
                    '        if len(args)==1 and isinstance(args, (slice,)):\n'
                    '            super(%(NAME)s, self).__init__(args[0].start, args[0].stop, args[0].step)\n'
                    '        else:\n'
                    '            super(%(NAME)s, self).__init__(*args)\n\n'
                ) % vars)
            elif vars['NAME'] in ('PRIVATE', 'PUBLIC'):
                f.write(
                    '    def assign(self, value):\n'
                    '        EQUALS(self, value).evaluate()\n'
                    '        return self\n'
                )
            else:
                f.write('\n')
            opcode += 1
    f.write(
        '_c = None\n'
        'for _c in globals().values():\n'
        '     if isinstance(_c,Function.__class__) and issubclass(_c,Function) and _c is not Function:\n'
        '         Function.opcodeToClass[_c.opcode]=_c\n'
        'del(_c)\n'
    )
#       better but not for python 2.6
#        'Function.opcodeToClass.update({\n'
#        '    (c.opcode, c)\n'
#        '    for c in globals().values()\n'
#        '    if isinstance(c, Function.__class__) and issubclass(c, Function) and c is not Function\n'
#        '})\n'


with open(target, 'w+') as f:
    with open(target+'.in', 'r') as i:
        for line in i:
            f.write(line)
    f.write('\n')
    opc(f)
