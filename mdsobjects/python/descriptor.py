import ctypes as _C

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_ver=_mimport('version')
_exceptions=_mimport('mdsExceptions')

#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
#
#############################################

def objectToPointer(obj):
    if obj is None: return c_desc_p0
    return descToPointer(obj.descriptor)

def descToPointer(desc):
    if desc is None: return c_desc_p0
    return _C.cast(_C.pointer(desc),c_desc_p)

def pointerToDesc(pointer):
    if pointer is None or pointer == 0: return None
    LP = _C.cast(pointer,c_desc_p)
    if _ver.ispy2:
        return LP.contents if LP.__nonzero__() else None
    else:
        return LP.contents if LP else None


def pointerToDesc_x(pointer):
    desc = pointerToDesc(pointer)
    if desc is None: return None
    return _C.cast(pointer,_C.POINTER(dclassToClass[desc.dclass])).contents

def pointerToObject(pointer):
    desc = pointerToDesc(pointer)
    if desc is None : return None
    return desc.value

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
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_s(_C.Structure):
    dclass_id = 1
    __init__=__desc_init__
    _fields_=Descriptor._fields_
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_d(_C.Structure):
    dclass_id = 2
    __init__=__desc_init__
    _fields_=Descriptor._fields_
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    @property
    def addressof(self):
        return _C.addressof(self)
    def __del__(self):
          _MdsShr.MdsFree1Dx(_C.pointer(self),_C.c_void_p(0))
c_desc_p=_C.POINTER(Descriptor)
c_desc_p0=_C.cast(_C.c_void_p(0),c_desc_p)

class Descriptor_a(_C.Structure):
    dclass_id = 4
    __init__=__desc_init__
    _fields_ = Descriptor._fields_ + [
               ("scale",_C.c_byte),
               ("digits",_C.c_ubyte),
               ("_fill1",_C.c_ubyte * (0 if _ver.isNt else 2)),
               ("aflags",_C.c_ubyte),
               ("_fill2",_C.c_ubyte * (0 if _ver.isNt else 3)),
               ("dimct",_C.c_ubyte),
               ("arsize",_C.c_uint),
               ("a0",_C.c_void_p),
               ("coeff_and_bounds",_C.c_int32 * 24)]

    @property
    def value(self):
        return dtypeToArrayClass[self.dtype].fromDescriptor(self)
    @property
    def addressof(self):
        return _C.addressof(self)

    @property
    def binscale(self):
        return bool(self.aflags & 8)
    @binscale.setter
    def binscale(self,value):
        if value:
            self.aflags|= 8
        else:
            self.aflags&= ~8

    @property
    def redim(self):
        return bool(self.aflags & 16)
    @redim.setter
    def redim(self,value):
        if value:
            self.aflags|= 16
        else:
            self.aflags&= ~16
    @property
    def column(self):
        return bool(self.aflags & 32)
    @column.setter
    def column(self,value):
        if value:
            self.aflags|= 32
        else:
            self.aflags&= ~32

    @property
    def coeff(self):
        return bool(self.aflags & 64)
    @coeff.setter
    def coeff(self,value):
        if value:
            self.aflags|= 64
        else:
            self.aflags&= ~64

    @property
    def bounds(self):
        return bool(self.aflags & 128)
    @bounds.setter
    def bounds(self,value):
        if value:
            self.aflags|= 128
        else:
            self.aflags&= ~128


def __desc_xd_init__(self):
    self.length = 0
    self.dtype = 0
    self.dclass = self.dclass_id
    self.pointer = _C.c_void_p(0)
    self.l_length = 0

class Descriptor_xd(_C.Structure):
    dclass_id = 192
    dtype_dsc = 24
    __init__=__desc_xd_init__
    _fields_=Descriptor._fields_ + [("l_length",_C.c_uint32)]
    @property
    def value(self):
        if self.pointer is None:
            return None
        d=_C.cast(self.pointer,_C.POINTER(Descriptor)).contents
        if (d.dclass==0):
            return None
        if hasattr(self,'tree'):
            d.tree=self.tree
        return _C.cast(_C.pointer(d),_C.POINTER(dclassToClass[d.dclass])).contents.value
    def __del__(self):
          _MdsShr.MdsFree1Dx(_C.pointer(self),_C.c_void_p(0))
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_xs(_C.Structure):
    dclass_id = 193
    __init__=__desc_xd_init__
    _fields_=Descriptor_xd._fields_
    @property
    def value(self):
        if self.pointer is None:
            return None
        else:
            return _C.cast(self.pointer,_C.POINTER(dclassToClass[self.dclass])).contents.value
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_r(_C.Structure):
    dclass_id = 194
    __init__=__desc_init__
    _pack_ = _C.sizeof(_C.c_void_p)
    _fields_ = Descriptor._fields_ + [
               ("ndesc",_C.c_ubyte),
               ("dscptrs",_C.POINTER(Descriptor)*256)]
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_ca(_C.Structure):
    dclass_id = 195
    __init__=__desc_init__
    _fields_=Descriptor_a._fields_
    @property
    def value(self):
        xd=Descriptor_xd()
        status = _MdsShr.MdsDecompress(_C.pointer(self),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _exceptions.statusToException(status)
    @property
    def addressof(self):
        return _C.addressof(self)

class Descriptor_apd(_C.Structure):
    dclass_id = 196
    __init__=__desc_init__
    _fields_=Descriptor_a._fields_
    @property
    def value(self):
        return dtypeToClass[self.dtype].fromDescriptor(self)
    @property
    def addressof(self):
        return _C.addressof(self)

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

