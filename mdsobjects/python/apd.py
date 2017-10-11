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

import numpy as _N
import ctypes as _C

_descriptor=_mimport('descriptor')
_data=_mimport('mdsdata')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_ver=_mimport('version')

class Apd(_array.Array):
    """The Apd class represents the Array of Pointers to Descriptors structure.
    This structure provides a mechanism for storing an array of non-primitive items.
    """
    mdsclass=196
    dtype_id=24

    @property
    def _descriptor(self):
        descs=self.descs
        _compound=_mimport('compound')
        d=descriptor.Descriptor_apd()
        d.scale=0
        d.digits=0
        d.aflags=0
        d.dtype=self.dtype_id
        d.dimct=1
        d.length=_C.sizeof(_C.c_void_p)
        ndesc = len(descs)
        d.array=[None]*ndesc
        if ndesc:
            d.arsize=d.length*ndesc
            descs_ptrs=(_descriptor.Descriptor.PTR*ndesc)()
            for idx in range(len(descs)):
                d.array[idx] = _data.Data(descs[idx])
                descs_ptrs[idx] = _data.Data.pointer(d.array[idx])
            d.pointer=_C.cast(_C.pointer(descs_ptrs),_C.c_void_p)
        d.a0=d.pointer
        return _compound.Compound._descriptorWithProps(self,d)
    @property
    def tree(self):
        for desc in self.descs:
            if isinstance(desc,_data.Data):
                tree=desc.tree
                if tree is not None:
                    return tree
        return None
    @tree.setter
    def tree(self,tree):
        for desc in self.descs:
            if isinstance(desc,_data.Data):
                desc._setTree(tree)

    @classmethod
    def fromDescriptor(cls,d):
        num   = int(d.arsize/d.length)
        dptrs = _C.cast(d.pointer,_C.POINTER(_C.c_void_p*num)).contents
        descs = [_descriptor.pointerToObject(dptr,d.tree) for dptr in dptrs]
        return cls(descs)._setTree(d.tree)


    def __hasBadTreeReferences__(self,tree):
        for desc in self.descs:
            if isinstance(desc,_data.Data) and desc.__hasBadTreeReferences__(tree):
                return True
        return False


    def __fixTreeReferences__(self,tree):
        for idx in range(len(self.descs)):
            d=self.descs[idx]
            if isinstance(d,_data.Data) and d.__hasBadTreeReferences__(tree):
                self.descs[idx]=d.__fixTreeReferences__(tree)
        return self

    def __init__(self,value,dtype=0):
        """Initializes a Apd instance
        """
        if value is self: return
        if isinstance(value,(tuple,list,_ver.mapclass,_ver.generator,_N.ndarray)):
            self.descs=list(value)
            self.dtype=dtype
        else:
            raise TypeError("must provide tuple of items when creating ApdData: %s"%(type(value),))

    def __len__(self):
        """Return the number of descriptors in the apd"""
        return len(self._descs)

    def __getitem__(self,idx):
        """Return descriptor(s) x.__getitem__(idx) <==> x[idx]
        @rtype: Data|tuple
        """
        try:
            return self.descs[idx]._setTree(self.tree)
        except:
            return

    def __setitem__(self,idx,value):
        """Set descriptor. x.__setitem__(idx,value) <==> x[idx]=value
        @rtype: None
        """
        diff = 1+idx-len(self._descs)
        if diff>0:
            self._descs+=[None]*diff
        self._descs[idx]=value

    def getDescs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return self._descs

    def getDescAt(self,idx=0):
        """Return the descriptor indexed by idx. (indexes start at 0).
        @rtype: Data
        """
        return self[idx]._setTree(self.tree)

    def setDescs(self,descs):
        """Set the descriptors of the Apd.
        @type descs: tuple
        @rtype: None
        """
        if isinstance(descs,(tuple,list,_ver.mapclass,_ver.generator,_N.ndarray)):
            self._descs=list(map(_data.Data,descs))
        else:
            raise TypeError("must provide tuple")
        return self

    def setDescAt(self,idx,value):
        """Set a descriptor in the Apd
        """
        self[idx]=_data.Data(value)
        return self

    def append(self,value):
        """Append a value to apd"""
        self[len(self)]=_data.Data(value)
        return self

    @property
    def value(self):
        return _N.array(self.descs,object)

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return tuple(self._descs)

    @property
    def _value(self):
        """Returns native representation of the List"""
        return _N.asarray(tuple(d.value for d in self.descs),'object')


class Dictionary(dict,Apd):
    """dictionary class"""
    class dict_np(_N.ndarray):
        def __new__(cls,items):
            return _N.asarray(tuple(d.value for d in items),'object').view(Dictionary.dict_np)
        def tolist(self):
            return dict(super(Dictionary.dict_np,self).tolist())

    _key_value_exception = Exception('A dictionary requires an even number of elements read as key-value pairs.')

    dtype_id=216

    def __init__(self,value=None):
        if value is self: return
        if value is not None:
            if isinstance(value,dict):
                for key,val in value.items():
                    self.setdefault(key,val)
            elif isinstance(value,(Apd,tuple,list,_ver.mapclass,_N.ndarray)):
                if isinstance(value,(_ver.mapclass,)) and not isinstance(value,(tuple,)):
                    value = tuple(value)
                if  len(value)&1:
                    raise Dictionary._key_value_exception
                for idx in range(0,len(value),2):
                    self.setdefault(value[idx],value[idx+1])
            elif isinstance(value,(_ver.generator)):
                for key in value:
                    self.setdefault(key,next(value))
            else:
                raise TypeError('Cannot create Dictionary from type: '+str(type(value)))

    @staticmethod
    def toKey(key):
        if isinstance(key,(_scalar.Scalar,)):
            key = key.value
        if isinstance(key,(_ver.npbytes,_ver.npunicode)):
            return _ver.tostr(key)
        if isinstance(key,(_N.int32,)):
            return int(key)
        if isinstance(key,(_N.float32,_N.float64)):
            return float(key)
        return _data.Data(key).data().tolist()

    def setdefault(self,key,val):
        """check keys and converts values to instances of Data"""
        key = Dictionary.toKey(key)
        if not isinstance(val,_data.Data):
            val=_data.Data(val)
        super(Dictionary,self).setdefault(key,val)

    def remove(self,key):
        """remove pair with key"""
        del(self[Dictionary.toKey(key)])

    def __setitem__(self,name,value):
        """sets values as instances of Data"""
        self.setdefault(name,value)

    def __getitem__(self,name):
        """gets values as instances of Data"""
        return super(Dictionary,self).__getitem__(Dictionary.toKey(name))

    @property
    def value(self):
        """Return native representation of data item"""
        return Dictionary.dict_np(self.items())

    def toApd(self):
        return Apd(self.descs,self.dtype_id)

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return sum(self.items(),())

class List(list,Apd):
    """list class"""

    dtype_id=214

    def __init__(self,value=None):
        if value is self: return
        if value is not None:
            if isinstance(value,(Apd,tuple,list,_ver.mapclass,_ver.generator,_N.ndarray)):
                for val in value:
                    List.append(self,_data.Data(val))
            else:
                raise TypeError('Cannot create List from type: '+str(type(value)))

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return tuple(self)

descriptor=_mimport('descriptor')
descriptor.dtypeToArrayClass[Apd.dtype_id]=Apd
descriptor.dtypeToArrayClass[List.dtype_id]=List
descriptor.dtypeToArrayClass[Dictionary.dtype_id]=Dictionary

