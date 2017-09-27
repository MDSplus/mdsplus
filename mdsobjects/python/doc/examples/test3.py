# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

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

