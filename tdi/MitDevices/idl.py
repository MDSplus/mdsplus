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

from ctypes import CDLL as _CDLL, RTLD_GLOBAL as _RTLD_GLOBAL, c_int32 as _c_int32, pointer as _pointer, c_char_p as _c_char_p
_x=_CDLL('libXt.so',_RTLD_GLOBAL)
_x=_CDLL('libXm.so',_RTLD_GLOBAL)
_x=_CDLL('libXpm.so',_RTLD_GLOBAL)
_x=_CDLL('libXinerama.so',_RTLD_GLOBAL)
_x=_CDLL('librt.so',_RTLD_GLOBAL)
try:
  _x=_CDLL('libtermcap.so',_RTLD_GLOBAL)
except:
  _x=_CDLL('libtinfo.so',_RTLD_GLOBAL)
_x=_CDLL('libgcc_s.so.1',_RTLD_GLOBAL)
_idl=_CDLL('libidl.so')
_zero=_c_int32(0)
_idl.IDL_Init(_c_int32(32768 | 64), _pointer(_zero), _c_int32(0))

def idl(command):
  _idl.IDL_ExecuteStr(_c_char_p(str(command)))
