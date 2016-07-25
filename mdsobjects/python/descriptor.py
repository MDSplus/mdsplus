import ctypes as _C
import os as _os

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_ver=_mimport('version')
descriptor=_mimport('descriptor')

#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
#
#############################################

def __desc_init__(self):
    self.length = 0
    self.dtype = 0
    self.dclass = self.dclass_id
    self.pointer = _C.c_void_p(0)

    
class Descriptor(_C.Structure):
    _fields_=[("length",_C.c_ushort),
              ("dtype",_C.c_ubyte),
              ("dclass",_C.c_ubyte),
              ("pointer",_C.c_void_p),]
    @property
    def value(self):
        d=_C.cast(_C.pointer(self),_C.POINTER(dclassToClass[self.dclass])).contents
        return d.value

class Descriptor_s(_C.Structure):
    _fields_=Descriptor._fields_
    dclass_id = 1
    __init__=__desc_init__
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
  
class Descriptor_d(_C.Structure):
    dclass_id = 2
    _fields_=Descriptor._fields_
    __init__=__desc_init__
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    def __del__(self):
          _MdsShr.MdsFree1Dx(_C.pointer(self),_C.c_void_p(0))

class Descriptor_a(_C.Structure):
    dclass_id = 4
    if _os.name=='nt':
        if _struct.calcsize("P")==4:
            _pack_=1
        _fields_=Descriptor._fields_ + [("scale",_C.c_byte),
                                        ("digits",_C.c_ubyte),
                                        ("aflags",_C.c_ubyte),
                                        ("dimct",_C.c_ubyte),
                                        ("arsize",_C.c_uint),
                                        ("a0",_C.c_void_p),
                                        ("coeff_and_bounds",_C.c_int32 * 24)]
    else:
        _fields_=Descriptor._fields_ + [("scale",_C.c_byte),
                                        ("digits",_C.c_ubyte),
                                        ("fill1",_C.c_ushort),
                                        ("aflags",_C.c_ubyte),
                                        ("fill2",_C.c_ubyte * 3),
                                        ("dimct",_C.c_ubyte),
                                        ("arsize",_C.c_uint),
                                        ("a0",_C.c_void_p),
                                        ("coeff_and_bounds",_C.c_int32 * 24)]
    __init__=__desc_init__

    @property
    def value(self):
        return dtypeToArrayClass[self.dtype].fromDescriptor(self)

    @property
    def binscale(self):
        return not ((self.aflags & 8) == 0)
    @binscale.setter
    def binscale(self,value):
        if value:
            self.aflags = self.aflags & ~8
        else:
            self.aflags = self.aflags | 8

    @property
    def redim(self):
        return not ((self.aflags & 16) == 0)
    @redim.setter
    def redim(self,value):
        if value:
            self.aflags = self.aflags & ~16
        else:
            self.aflags = self.aflags | 16

    @property
    def column(self):
        return not ((self.aflags & 32) == 0)
    @column.setter
    def column(self,value):
        if value:
            self.aflags = self.aflags & ~32
        else:
            self.aflags = self.aflags | 32

    @property
    def coeff(self):
        return not ((self.aflags & 64) == 0)
    @coeff.setter
    def coeff(self,value):
        if value:
            self.aflags = self.aflags & ~64
        else:
            self.aflags = self.aflags | 64

    @property
    def bounds(self):
        return not ((self.aflags & 128) == 0)
    @bounds.setter
    def bounds(self,value):
        if value:
            self.aflags = self.aflags & ~128
        else:
            self.aflags = self.aflags | 128


def __desc_xd_init__(self):
    self.length = 0
    self.dtype = 0
    self.dclass = self.dclass_id
    self.pointer = _C.c_void_p(0)
    self.l_length = 0
    
class Descriptor_xd(_C.Structure):
    dclass_id = 192
    dtype_dsc = 24
    _fields_=Descriptor._fields_ + [("l_length",_C.c_uint32)]
    __init__=__desc_xd_init__
    @property
    def value(self):
        if self.pointer is None:
            return None
        d=_C.cast(self.pointer,_C.POINTER(Descriptor)).contents
        if hasattr(self,'tree'):
            d.tree=self.tree
        return _C.cast(_C.pointer(d),_C.POINTER(dclassToClass[d.dclass])).contents.value
    def __del__(self):
          _MdsShr.MdsFree1Dx(_C.pointer(self),_C.c_void_p(0))

class Descriptor_xs(_C.Structure):
    dclass_id = 193
    _fields_=Descriptor_xd._fields_
    __init__=__desc_xd_init__
    @property
    def value(self):
        if self.pointer is None:
            return None
        else:
            return _C.cast(self.pointer,_C.POINTER(dclassToClass[self.dclass])).contents.value

    
class Descriptor_r(_C.Structure):
    dclass_id = 194
    if _os.name=='nt' and _struct.calcsize("P")==8:
        _fields_=Descriptor._fields_ + [("ndesc",_C.c_ubyte),
                                        ("fill1",_C.c_ubyte*6),
                                        ("dscptrs",_C.POINTER(Descriptor)*256)]
    else:
        _fields_=Descriptor._fields_ + [("ndesc",_C.c_ubyte),
                                        ("fill1",_C.c_ubyte*3),
                                        ("dscptrs",_C.POINTER(Descriptor)*256)]
    __init__=__desc_init__
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    
class Descriptor_ca(_C.Structure):
    dclass_id = 195
    _fields_=Descriptor_a._fields_
    __init__=__desc_init__
    @property
    def value(self):
        xd=Descriptor_xd()
        status = _MdsShr.MdsDecompress(_C.pointer(self),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _Exceptions.statusToException(status)
    
class Descriptor_apd(_C.Structure):
    dclass_id = 196
    _fields_=Descriptor_a._fields_
    __init__=__desc_init__
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)

dclassToClass={Descriptor_s.dclass_id : Descriptor_s,
               Descriptor_d.dclass_id : Descriptor_d,
               Descriptor_a.dclass_id : Descriptor_a,
               Descriptor_xd.dclass_id : Descriptor_xd,
               Descriptor_xs.dclass_id : Descriptor_xs,
               Descriptor_r.dclass_id : Descriptor_r,
               Descriptor_ca.dclass_id : Descriptor_ca,
               Descriptor_apd.dclass_id : Descriptor_apd}

dtypeToClass={}
dtypeToArrayClass={}

