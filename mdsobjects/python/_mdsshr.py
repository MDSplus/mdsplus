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

