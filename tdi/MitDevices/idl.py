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
