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

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C

_ver=_mimport('version')
_exc=_mimport('mdsExceptions')

#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
#
#############################################

def pointerToObject(pointer,tree=None):
    if not pointer: return None
    return Descriptor(pointer)._setTree(tree).value

class Descriptor(object):
    tree = None
    dclass_id = 0
    _value = None
    _structure = None
    class _structure_class(_C.Structure):
        _fields_=[("length",_C.c_ushort),
                  ("dtype",_C.c_ubyte),
                  ("dclass",_C.c_ubyte),
                  ("pointer",_C.c_void_p),]
    PTR = _C.POINTER(_structure_class)
    null= _C.cast(0,PTR)
    @property
    def value(self):
        if self.dclass:
            return self.desc_class(self._structure,self.__dict__)._setTree(self.tree).value
    def _setTree(self,tree):
        _tre = _mimport('tree')
        if isinstance(tree,_tre.Tree): self.tree=tree
        return self

    @property
    def desc_class(self):
        return dclassToClass[self.dclass]

    def _new_structure(self,length=0,dtype=0,dclass=0,pointer=None,**kwargs):
        self._structure = self._structure_class()
        self._structure.length = length
        self._structure.dtype  = dtype
        self._structure.dclass = dclass
        self._structure.pointer= None
        for k,v in kwargs.items():
            exec(compile("struct.%s = v"%k,'<string>','exec'))

    def __new__(cls,obj_in=None,_dict_={}):
        if cls is not Descriptor or not obj_in:
            return object.__new__(cls)
        if not obj_in and not hasattr(cls,'__del__'):
            return DescriptorNULL
        if not isinstance(obj_in,cls._structure_class):
            if isinstance(obj_in,_C.Structure):
                obj_in = _C.pointer(obj_in)
            obj_in = _C.cast(obj_in,cls.PTR).contents
        obj = dclassToClass[obj_in.dclass](obj_in,_dict_)
        obj.__init__ = lambda *a: None  # done call __init__ again
        return obj
    def __init__(self,obj_in=None,_dict_={}):
        if self.__class__ is Descriptor:
            return Exception("cannot instanciate Descriptor")
        for k,v in _dict_.items():
            if k not in ['ptr','ptr_']:
                self.__dict__[k] = v
        if obj_in is None:
            self._new_structure(dclass=self.dclass_id)
        elif isinstance(obj_in,self._structure_class):
            self._structure = obj_in
        else:
            if isinstance(obj_in,_C.Structure):
                obj_in = _C.pointer(obj_in)
            elif isinstance(obj_in,(int,_ver.long)):
                obj_in = _C.c_void_p(obj_in)
            self._structure=_C.cast(obj_in,self.PTR).contents
        self.ptr = _C.pointer(self._structure)
        self.ptr_= _C.cast(self.ptr,Descriptor.PTR)

    def __getattr__(self,name):
        if name is not '_structure' and name in dict(self._structure._fields_):
            return self._structure.__getattribute__(name)
        super(Descriptor,self).__getattribute__(name)
    def __setattr__(self,name,value):
        if name is not '_structure' and name in dict(self._structure._fields_):
            return self._structure.__setattr__(name,value)
        super(Descriptor,self).__setattr__(name,value)

    @property
    def addressof(self):
        return _C.addressof(self._structure)
    @property
    def ref(self):
        return _C.byref(self._structure)

class DescriptorNULL(Descriptor):
    dclass = length = dtype = addressof = pointer = 0
    ref=ptr_=ptr=Descriptor.null
    def __init__(self):pass
DescriptorNULL=DescriptorNULL()

class Descriptor_s(Descriptor):
    dclass_id = 1
    @property
    def value(self):
        if self.dtype:
            return dtypeToClass[self.dtype].fromDescriptor(self)._setTree(self.tree)

class Descriptor_d(Descriptor_s):
    dclass_id = 2
    def __del__(self):
          _MdsShr.MdsFree1Dx(self.ptr,0)

class Descriptor_xs(Descriptor_s):
    dclass_id = 193
    class _structure_class(_C.Structure):
        _fields_=Descriptor_s._structure_class._fields_ + [
               ("l_length",_C.c_uint32)]
    def _new_structure(self,l_length=0,**kwarg):
        super(Descriptor_xs,self)._new_structure(**kwarg)
        self._structure.l_length = l_length
    PTR = _C.POINTER(_structure_class)
    null= _C.cast(0,PTR)
    @property
    def value(self):
        if self.l_length and self.pointer:
            return Descriptor(self.pointer,self.__dict__)._setTree(self.tree).value

class Descriptor_xd(Descriptor_xs):
    dclass_id = 192
    dtype_dsc = 24
    def __del__(self):
          _MdsShr.MdsFree1Dx(self.ptr,0)

class Descriptor_r(Descriptor_s):
    dclass_id = 194
    class _structure_class(_C.Structure):
        _pack_ = _C.sizeof(_C.c_void_p)
        _fields_=Descriptor_s._structure_class._fields_ + [
               ("ndesc",_C.c_ubyte),
               ("dscptrs",Descriptor.PTR*256)]
    PTR = _C.POINTER(_structure_class)
    null= _C.cast(0,PTR)

## HINT: arrays

class Descriptor_a(Descriptor):
    dclass_id = 4
    class _structure_class(_C.Structure):
        _fields_ = Descriptor._structure_class._fields_ + [
               ("scale",_C.c_byte),
               ("digits",_C.c_ubyte),
               ("",_C.c_ubyte * (0 if _ver.isNt else 2)),
               ("aflags",_C.c_ubyte),
               ("",_C.c_ubyte * (0 if _ver.isNt else 3)),
               ("dimct",_C.c_ubyte),
               ("arsize",_C.c_uint),
               ("a0",_C.c_void_p),
               ("coeff_and_bounds",_C.c_int32 * 24)]
    def _new_structure(self,arsize=0,**kwarg):
        super(Descriptor_a,self)._new_structure(**kwarg)
        self._structure.arsize = arsize
    PTR = _C.POINTER(_structure_class)
    null= _C.cast(0,PTR)
    @property
    def value(self):
        if self.dtype:
            return dtypeToArrayClass[self.dtype].fromDescriptor(self)._setTree(self.tree)
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

class Descriptor_ca(Descriptor_a):
    dclass_id = 195
    @property
    def value(self):
        xd = Descriptor_xd()
        _exc.checkStatus(_MdsShr.MdsDecompress(self.ptr,xd.ptr))
        return xd._setTree(self.tree).value

class Descriptor_apd(Descriptor_a):
    dclass_id = 196


dclassToClass={Descriptor_s.dclass_id : Descriptor_s,
               Descriptor_d.dclass_id : Descriptor_d,
               Descriptor_xs.dclass_id : Descriptor_xs,
               Descriptor_xd.dclass_id : Descriptor_xd,
               Descriptor_r.dclass_id : Descriptor_r,
               Descriptor_a.dclass_id : Descriptor_a,
               Descriptor_ca.dclass_id : Descriptor_ca,
               Descriptor_apd.dclass_id : Descriptor_apd}

dtypeToClass={}
def addDtypeToClass(Class):       dtypeToClass[Class.dtype_id]=Class
dtypeToArrayClass={}
def addDtypeToArrayClass(Class):  dtypeToArrayClass[Class.dtype_id]=Class
