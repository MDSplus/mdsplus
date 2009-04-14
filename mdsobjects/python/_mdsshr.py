import ctypes as _C
from ctypes.util import find_library as _find_library
import os,sys

def _load_library(name):
    if os.name == 'nt':
        return _C.CDLL(name)
    elif os.name == "posix" and sys.platform == "darwin":
        lib=_find_library(name)
        if lib is None:
            raise Exception,"Error finding library: "+name
        else:
            return _C.CDLL(lib)
    else:
        try:
            return _C.CDLL('lib'+name+'.so')
        
        except:
            return _C.CDLL('lib'+name+'.sl')
    raise Exception,"Error finding library: "+name

MdsShr=_load_library('MdsShr')
__MdsGetMsg=MdsShr.MdsGetMsg
__MdsGetMsg.argtypes=[_C.c_int]
__MdsGetMsg.restype=_C.c_char_p
__LibConvertDateString=MdsShr.LibConvertDateString
__LibConvertDateString.argtypes=[_C.c_char_p,_C.POINTER(_C.c_ulonglong)]

class MdsException(Exception):
    pass

def MdsGetMsg(status,default=None):
    if status==0 and not default is None:
        return default
    return __MdsGetMsg(status)

def MdsDecompress(value):
    from _descriptor import descriptor_xd
    from mdsarray import makeArray
    xd=descriptor_xd()
    status = MdsShr.MdsDecompress(_C.pointer(value),_C.pointer(xd))
    if (status & 1) == 1:
        return makeArray(xd.value)
    else:
        raise MdsException,MdsGetMsg(status)

def MdsCopyDxXd(desc):
    from _descriptor import descriptor,descriptor_xd
    xd=descriptor_xd()
    if not isinstance(desc,descriptor):
        desc=descriptor(desc)
    status=MdsShr.MdsCopyDxXd(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
        return xd
    else:
        raise MdsException,MdsGetMsg(status)

def MdsFree1Dx(value):
    MdsShr.MdsFree1Dx(_C.pointer(value),_C.c_void_p(0))

def DateToQuad(date):
    from numpy import array,uint64
    ans=_C.c_ulonglong(0)
    status = __LibConvertDateString(date,ans)
    if not (status & 1):
        raise MdsException,"Cannot parse %s as date. Use dd-mon-yyyy hh:mm:ss.hh format or \"now\",\"today\",\"yesterday\"." % (date,)
    return ans.value
