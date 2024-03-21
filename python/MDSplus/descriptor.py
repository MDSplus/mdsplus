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


import ctypes as _C


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except Exception:
        return __import__(name, globals())


_ver = _mimport('version')
_exc = _mimport('mdsExceptions')

#### Load Shared Libraries Referenced #######
#
_MdsShr = _ver.load_library('MdsShr')
#
#############################################


def pointerToObject(pointer, tree=None):
    if not pointer:
        return None
    return Descriptor(pointer)._setTree(tree).value


class Descriptor(object):
    tree = None
    dclass_id = 0
    _value = None
    _structure = None

    class _structure_class(_C.Structure):
        _fields_ = [("length", _C.c_ushort),
                    ("dtype", _C.c_ubyte),
                    ("dclass", _C.c_ubyte),
                    ("pointer", _C.c_void_p), ]
    PTR = _C.POINTER(_structure_class)
    null = _C.cast(0, PTR)

    @property
    def value(self):
        if self.dclass:
            return self.desc_class(self._structure, self.__dict__)._setTree(self.tree).value

    def _setTree(self, tree):
        _tre = _mimport('tree')
        if isinstance(tree, _tre.Tree):
            self.tree = tree
        return self

    @property
    def dtype_name(self):
        if self.dtype in dclassToClass:
            return dtypeToClass[self.dtype].__name__
        if self.dtype in dtypeToArrayClass:
            return dtypeToArrayClass[self.dtype].__name__
        return "Unknown-%d" % int(self.dtype)

    def __str__(self):
        return "%s(%d,%s,%d,0x%x)" % (self.__class__.__name__, self.length, self.dtype_name, self.dclass, 0 if self.pointer is None else self.pointer)

    def __repr__(self): return str(self)

    @property
    def desc_class(self):
        return dclassToClass[self.dclass]

    def _new_structure(self, length=0, dtype=0, dclass=0, pointer=None, **kwargs):
        self._structure = self._structure_class()
        self._structure.length = length
        self._structure.dtype = dtype
        self._structure.dclass = dclass
        self._structure.pointer = None
        for k, v in kwargs.items():
            exec(compile("struct.%s = v" % k, '<string>', 'exec'))

    def __new__(cls, obj_in=None, _dict_={}):
        if cls is not Descriptor or not obj_in:
            return object.__new__(cls)
        if not obj_in and not hasattr(cls, '__del__'):
            return DescriptorNULL
        if not isinstance(obj_in, cls._structure_class):
            if isinstance(obj_in, _C.Structure):
                obj_in = _C.pointer(obj_in)
            obj_in = _C.cast(obj_in, cls.PTR).contents
        obj = dclassToClass[obj_in.dclass](obj_in, _dict_)
        obj.__init__ = lambda *a: None  # done call __init__ again
        return obj

    def __init__(self, obj_in=None, _dict_={}):
        if self.__class__ is Descriptor:
            raise _exc.MdsException("cannot instanciate Descriptor")
        for k, v in _dict_.items():
            if k not in ['ptr', 'ptr_']:
                self.__dict__[k] = v
        if obj_in is None:
            self._new_structure(dclass=self.dclass_id)
        elif isinstance(obj_in, self._structure_class):
            self._structure = obj_in
        else:
            if isinstance(obj_in, _C.Structure):
                obj_in = _C.pointer(obj_in)
            elif isinstance(obj_in, (int, _ver.long)):
                obj_in = _C.c_void_p(obj_in)
            self._structure = _C.cast(obj_in, self.PTR).contents
        self.ptr = _C.pointer(self._structure)
        self.ptr_ = _C.cast(self.ptr, Descriptor.PTR)

    @property
    def length(self): return self._structure.length
    @length.setter
    def length(self, value): self._structure.length = value

    @property
    def dclass(self): return self._structure.dclass
    @dclass.setter
    def dclass(self, value): self._structure.dclass = value

    @property
    def dtype(self): return self._structure.dtype
    @dtype.setter
    def dtype(self, value): self._structure.dtype = value

    @property
    def pointer(self): return self._structure.pointer
    @pointer.setter
    def pointer(self, value): self._structure.pointer = value

    @property
    def addressof(self):
        return _C.addressof(self._structure)

    @property
    def ref(self):
        return _C.byref(self._structure)


class DescriptorNULL(Descriptor):
    dclass = length = dtype = addressof = pointer = 0
    ref = ptr_ = ptr = Descriptor.null

    def __init__(self): """NULL"""


DescriptorNULL = DescriptorNULL()


class DescriptorS(Descriptor):
    dclass_id = 1

    @property
    def value(self):
        if self.dtype:
            return dtypeToClass[self.dtype].fromDescriptor(self)._setTree(self.tree)


class DescriptorD(DescriptorS):
    dclass_id = 2

    def __del__(self):
        _MdsShr.MdsFree1Dx(self.ptr, 0)


class DescriptorXS(DescriptorS):
    dclass_id = 193

    class _structure_class(_C.Structure):
        _fields_ = DescriptorS._structure_class._fields_ + [
            ("l_length", _C.c_uint32)]

    def _new_structure(self, l_length=0, **kwarg):
        super(DescriptorXS, self)._new_structure(**kwarg)
        self._structure.l_length = l_length
    PTR = _C.POINTER(_structure_class)
    null = _C.cast(0, PTR)

    @property
    def descriptor(self):
        if self.l_length and self.pointer:
            return Descriptor(self.pointer, self.__dict__)._setTree(self.tree)
        return DescriptorNULL

    @property
    def value(self):
        return self.descriptor.value

    @property
    def l_length(self): return self._structure.l_length
    @l_length.setter
    def l_length(self, value): self._structure.l_length = value


class DescriptorXD(DescriptorXS):
    dclass_id = 192
    dtype_dsc = 24

    def __del__(self):
        _MdsShr.MdsFree1Dx(self.ptr, 0)


class DescriptorR(DescriptorS):
    dclass_id = 194

    class _structure_class(_C.Structure):
        _pack_ = _C.sizeof(_C.c_void_p)
        _fields_ = DescriptorS._structure_class._fields_ + [
            ("ndesc", _C.c_ubyte),
            ("dscptrs", Descriptor.PTR*256)]
    PTR = _C.POINTER(_structure_class)
    null = _C.cast(0, PTR)

    @property
    def ndesc(self): return self._structure.ndesc
    @ndesc.setter
    def ndesc(self, value): self._structure.ndesc = value

    @property
    def dscptrs(self): return self._structure.dscptrs
    @dscptrs.setter
    def dscptrs(self, value): self._structure.dscptrs = value

# HINT: arrays


class DescriptorA(Descriptor):
    dclass_id = 4

    class _structure_class(_C.Structure):
        _fields_ = Descriptor._structure_class._fields_ + [
            ("scale", _C.c_byte),
            ("digits", _C.c_ubyte),
            ("fill0", _C.c_ubyte * (0 if _ver.iswin else 2)),
            ("aflags", _C.c_ubyte),
            ("fill1", _C.c_ubyte * (0 if _ver.iswin else 3)),
            ("dimct", _C.c_ubyte),
            ("arsize", _C.c_uint),
            ("a0", _C.c_void_p),
            ("coeff_and_bounds", _C.c_int32 * 24)]

    def _new_structure(self, arsize=0, **kwarg):
        super(DescriptorA, self)._new_structure(**kwarg)
        self._structure.arsize = arsize
        self._structure.aflags = 48
    PTR = _C.POINTER(_structure_class)
    null = _C.cast(0, PTR)

    @property
    def value(self):
        if self.dtype:
            return dtypeToArrayClass[self.dtype].fromDescriptor(self)._setTree(self.tree)

    @property
    def binscale(self):
        return bool(self.aflags & 8)

    @binscale.setter
    def binscale(self, value):
        if value:
            self.aflags |= 8
        else:
            self.aflags &= ~8

    @property
    def redim(self):
        return bool(self.aflags & 16)

    @redim.setter
    def redim(self, value):
        if value:
            self.aflags |= 16
        else:
            self.aflags &= ~16

    @property
    def column(self):
        return bool(self.aflags & 32)

    @column.setter
    def column(self, value):
        if value:
            self.aflags |= 32
        else:
            self.aflags &= ~32

    @property
    def coeff(self):
        return bool(self.aflags & 64)

    @coeff.setter
    def coeff(self, value):
        if value:
            self.aflags |= 64
        else:
            self.aflags &= ~64

    @property
    def bounds(self):
        return bool(self.aflags & 128)

    @bounds.setter
    def bounds(self, value):
        if value:
            self.aflags |= 128
        else:
            self.aflags &= ~128

    @property
    def scale(self): return self._structure.scale
    @scale.setter
    def scale(self, value): self._structure.scale = value

    @property
    def digits(self): return self._structure.digits
    @digits.setter
    def digits(self, value): self._structure.digits = value

    @property
    def aflags(self): return self._structure.aflags
    @aflags.setter
    def aflags(self, value): self._structure.aflags = value

    @property
    def dimct(self): return self._structure.dimct
    @dimct.setter
    def dimct(self, value): self._structure.dimct = value

    @property
    def arsize(self): return self._structure.arsize
    @arsize.setter
    def arsize(self, value): self._structure.arsize = value

    @property
    def a0(self): return self._structure.a0
    @a0.setter
    def a0(self, value): self._structure.a0 = value

    @property
    def coeff_and_bounds(self): return self._structure.coeff_and_bounds
    @coeff_and_bounds.setter
    def coeff_and_bounds(self, value): self._structure.coeff_and_bounds = value

    @property
    def size(self):
        return self._structure.arsize // self._structure.length


class DescriptorCA(DescriptorA):
    dclass_id = 195

    @property
    def value(self):
        xd = DescriptorXD()
        _exc.checkStatus(_MdsShr.MdsDecompress(self.ptr, xd.ptr))
        return xd._setTree(self.tree).value


class DescriptorAPD(DescriptorA):
    dclass_id = 196

    @property
    def descriptors(self):
        tree = self.tree
        dptrs = _C.cast(self.pointer, _C.POINTER(_C.c_void_p*self.size)).contents
        return [Descriptor(dptr)._setTree(tree) for dptr in dptrs]


dclassToClass = {DescriptorS.dclass_id: DescriptorS,
                 DescriptorD.dclass_id: DescriptorD,
                 DescriptorXS.dclass_id: DescriptorXS,
                 DescriptorXD.dclass_id: DescriptorXD,
                 DescriptorR.dclass_id: DescriptorR,
                 DescriptorA.dclass_id: DescriptorA,
                 DescriptorCA.dclass_id: DescriptorCA,
                 DescriptorAPD.dclass_id: DescriptorAPD}

dtypeToClass = {}


def _add_dtype_to_class(cls):      dtypeToClass[cls.dtype_id] = cls


dtypeToArrayClass = {}


def _add_dtype_to_array_class(cls): dtypeToArrayClass[cls.dtype_id] = cls
