import sys
from opcodes import *
find_opcode('$')
for o in opcodes_by_name:
    try:
        opcodes_by_name[o].name.lower().index('build')
    except:
        try:
            exec 'def '+opcodes_by_name[o].name.lower()+'(*args):\n\treturn find_opcode("'+opcodes_by_name[o].name+'").evaluate(args)'
        except:
            pass
