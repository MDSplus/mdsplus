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

import numpy as _N
import ctypes as _C

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_ver=_mimport('version')
_data=_mimport('mdsdata')
_exc=_mimport('mdsExceptions')
_mds=_ver.load_library('MdsShr')

class MdsshrException(_exc.MDSplusException): pass

def getenv(name,*default):
    """get environment variable value
    @param name: name of environment variable
    @type name: str
    @return: value of environment variable or None if not defined
    @rtype: str or None
    """
    tl = _mds.TranslateLogical
    tl.restype=_C.c_void_p
    ptr = tl(_ver.tobytes(name))
    if ptr is None:
        if len(default)>0:
            return default[0]
        return None
    ptr = _C.c_void_p(ptr)
    try:
        return _ver.tostr(_C.cast(ptr,_C.c_char_p).value)
    finally:
        _mds.TranslateLogicalFree(ptr)

def setenv(name,value):
    """set environment variable
    @param name: name of the environment variable
    @type name: str
    @param value: value of the environment variable
    @type value: str
    """
    pe=_mds.MdsPutEnv
    pe(_ver.tobytes("=".join([str(name),str(value)])))


def DateToQuad(date):
    ans=_C.c_ulonglong(0)
    status = _mds.LibConvertDateString(_C.c_char_p(_ver.tobytes(date)),_C.pointer(ans))
    if not (status & 1):
        raise MdsshrException("Cannot parse %s as date. Use dd-mon-yyyy hh:mm:ss.hh format or \"now\",\"today\",\"yesterday\"." % (date,))
    return _data.Data(_N.uint64(ans.value))

