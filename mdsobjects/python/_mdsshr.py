import ctypes as _C
import os

if os.name == 'nt':
    MdsShr=_C.CDLL('mdsshr')
else:
    MdsShr=_C.CDLL('libMdsShr.so')

__MdsGetMsg=MdsShr.MdsGetMsg
__MdsGetMsg.argtypes=[_C.c_int]
__MdsGetMsg.restype=_C.c_char_p

def MdsException(Exception):
    pass

def MdsGetMsg(status,default=None):
    if status==0 and not default is None:
        return default
    return __MdsGetMsg(status)

def MdsDecompress(value):
    from MDSobjects._descriptor import descriptor_xd
    from MDSobjects.array import makeArray
    xd=descriptor_xd()
    status = MdsShr.MdsDecompress(_C.pointer(value),_C.pointer(xd))
    if (status & 1) == 1:
        return makeArray(xd.value)
    else:
        raise MdsException,MdsGetMsg(status)

def MdsCopyDxXd(desc):
    from MDSobjects._descriptor import descriptor,descriptor_xd
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
