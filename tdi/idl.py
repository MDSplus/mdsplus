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

import ctypes

ctypes.CDLL('libXt.so', ctypes.RTLD_GLOBAL)
ctypes.CDLL('libXm.so', ctypes.RTLD_GLOBAL)
ctypes.CDLL('libXpm.so', ctypes.RTLD_GLOBAL)
ctypes.CDLL('libXinerama.so', ctypes.RTLD_GLOBAL)
ctypes.CDLL('librt.so', ctypes.RTLD_GLOBAL)
try:
    ctypes.CDLL('libtermcap.so', ctypes.RTLD_GLOBAL)
except:
    ctypes.CDLL('libtinfo.so', ctypes.RTLD_GLOBAL)
ctypes.CDLL('libgcc_s.so.1', ctypes.RTLD_GLOBAL)
libidl = ctypes.CDLL('libidl.so')
libidl.IDL_Init(ctypes.c_int32(32768 | 64),
                ctypes.pointer(ctypes.c_int32(0)), ctypes.c_int32(0))


def idl(command, libidl=libidl):
    libidl.IDL_ExecuteStr(ctypes.c_char_p(str(command)))
