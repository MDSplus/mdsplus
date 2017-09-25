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
This function can be used to remove elements/entries from a list or dictionary.
apdrm(list,*arguments):       remove values from list
apdrm(dict,*keys)             remove key-value pairs from dictionary
"""
def apdrm(var,*args):
    from MDSplus import List, Dictionary, Ident, PUBLIC, PRIVATE
    apd = var.getData()
    if isinstance(apd, (List,)):
        args = list(args)
        args.sort(reverse=True)
        for idx in args:
            del(apd[int(idx.data())])
    elif isinstance(apd, (Dictionary)):
        for key in args:
            del(apd[Dictionary.toKey(key.data())])
    else:
        raise TypeError('Invalid agument class. Argument must be a List or Dictionary')
    if isinstance(var, (Ident,PUBLIC,PRIVATE)):
        try:   var.assign(apd)
        except Exception as e: print(e)
    return apd
