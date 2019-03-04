#!/usr/bin/python
"""
call 'python parse_headers.py'
edit 'mdsobjects/python/compound.py' and trunk file below '### parse_headers.py ###'
call 'cat /tmp/compound_rest.py >> mdsobjects/python/compound.py'
"""
forceref = ('$EXPT','$SHOT','$SHOTNAME','$DEFAULT','GETNCI',
  'MAKE_FUNCTION','BUILD_FUNCTION','EXT_FUNCTION'
  'MAKE_PROCEDURE','BUILD_PROCEDURE',
  'MAKE_ROUTINE','BUILD_ROUTINE',
  'MAKE_CALL','BUILD_CALL',
  'COMPILE','EXECUTE',
)
filepath = './include/opcbuiltins.h'
import sys
def opc(f=sys.stdout):
    with file(filepath) as opcf:
        lines = opcf.readlines()
    opcode = 0
    f.write('MAX_DIMS = 8\n')
    for line in lines:
        if not line.startswith('OPC ('):
            continue
        args = line[5:].split(')',2)[0].split(',',12)
        name,NAME,f1,f2,f3,i1,i2,o1,o2,m1,m2,token = tuple(arg.strip('\t ') for arg in args)
        m1i,m2i = int(m1),int(eval(m2.replace('MAX_DIMS','8')))
        pyname = NAME.replace("$","d")
        if   NAME in forceref:
            parent = '_dat.TreeRef,Function'
        elif m2i > 0:
            parent = '_dat.TreeRefX,Function'
        else:
            parent = 'Function'
        f.write('class %s(%s):\n'%(pyname,parent))
        f.write('    opcode = %d\n'%(opcode,))
        if m2i>0: f.write('    min_args = %d\n'%(m1i,))
        f.write('    max_args = %s'%(m2,))
        if NAME in ('BUILD_RANGE','MAKE_RANGE'):
            f.write("""
    def __init__(self,*args):
        if len(args)==1 and args[0] is self: return
        if len(args)==1 and isinstance(args,(slice,)):
            super(BUILD_RANGE,self).__init__(args[0].start,args[0].stop,args[0].step)
        else:
            super(BUILD_RANGE,self).__init__(*args)\n
""")
        elif NAME in ('PRIVATE','PUBLIC'):
            f.write("""
    def assign(self,value):
        EQUALS(self,value).evaluate()
        return self
""")
        else: f.write('\n\n')
        opcode+=1
    f.write("""
_c = None
for _c in globals().values():
    if isinstance(_c,Function.__class__) and issubclass(_c,Function) and _c is not Function:
        Function.opcodeToClass[_c.opcode]=_c
del(_c)
""")
with open('/tmp/compound_rest.py','w+') as f:
    opc(f)
