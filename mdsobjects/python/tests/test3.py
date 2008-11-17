from MDSobjects.data import makeData,Data

def test():
    """Exercise data operations"""
    print abs(makeData(-10))
    x=makeData(30)
    y=makeData(2)
    print x+y
    print x-y
    print x&y
    print x==y
    print x>y
    print x>=y
    print x<y
    print x<=y
    print x%y
    print x!=y
    print -x
    print x!=0
    print x|y
    print +x
    print x*y
    print x/y
    print x//y
    print x<<y
    print x>>y
    print abs(makeData(-10))
    print float(x)
    print int(x)
    print x.getDouble()
    print x.decompile()
    print x
    print x.evaluate()
    print x.getByte()
    print x.getInt()
    print x.getShape()
    print x.getByteArray()
    print x.getShortArray()
    print x.getIntArray()
    print x.getLongArray()
    print x.getString()
    print x.getUnits()
    x.setUnits('amps')
    print x.getUnits()
    x.setError(42)
    print x.getError()
    x.setHelp('This is help')
    print x.getHelp()
    x=Data.execute('data(1:100.)')
    print x+y
    print x-y
    print x&y
    print x==y
    print x>y
    print x>=y
    print x<y
    print x<=y
    print x%y
    print x!=y
    print -x
    print x!=0
    print x|y
    print +x
    print x*y
    print x/y
    print x//y
    print x<<y
    print x>>y
    print abs(makeData(-10))
    print float(x)
    print x.decompile()
    print x
    print x.evaluate()
    print x.getShape()
    print x.getByteArray()
    print x.getShortArray()
    print x.getIntArray()
    print x.getLongArray()
    print x.getString()
    print x.getUnits()
    x.setUnits('amps')
    print x.getUnits()
    x.setError(42)
    print x.getError()
    x.setHelp('This is help')
    print x.getHelp()

