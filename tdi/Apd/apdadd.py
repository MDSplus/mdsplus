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

"""
This function can be used to insert elements/entries to a list or dictionary.
apdadd(list,*arguments):       append arguments
apdadd(idx,list,*arguments)    inserts arguments at position idx
apdadd(dict,key0,value0, ... ) adds key-value pairs
"""
def apdadd(*args):
    from MDSplus import List, Dictionary, TdiEvaluate, Ident
    def add_apd(apd,args,idx):
        args = list(map(TdiEvaluate,args))
        if isinstance(apd, (List,)):
            if idx<0:
                idx += len(apd)+1
            for arg in args[::-1]:
                apd.insert(idx,arg)
        elif isinstance(apd, (Dictionary)):
            if  len(args)&1:
                raise Dictionary._key_value_exception
            for i in range(0,len(args),2):
                apd.setdefault(args[i],args[i+1])
        return apd
    arg0 = args[0].getData()
    if isinstance(arg0, (List, Dictionary)):
        apd = add_apd(arg0, list(args[1:]), len(arg0))
        var = args[0]
    else:
        var = args[1]
        arg1= var.getData()
        if isinstance(arg1, (List,)):
            apd = add_apd(arg1, list(args[2:]), int(arg0))
        else:
            raise TypeError('Invalid agument class. Argument must be a List or Dictionary')
    if isinstance(var, (Ident,)):
        apd.assignTo(var.name)
    return apd
