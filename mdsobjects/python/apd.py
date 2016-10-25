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
    def descriptor(self):
      try:
        descs=self.descs
        _compound=_mimport('compound')
        d=descriptor.Descriptor_apd()
        d.scale=0
        d.digits=0
        d.aflags=0
        d.dtype=self.dtype_id
        d.dimct=1
        d.length=_C.sizeof(_C.c_void_p)
        d.original=[]
        if len(descs) > 0:
            d.arsize=_C.sizeof(_descriptor.c_desc_p)*len(descs)
            descs_ptrs=(_descriptor.c_desc_p*len(descs))()
            for idx in range(len(descs)):
                desc=descs[idx]
                d.original.append(desc)
                descs_ptrs[idx] = _descriptor.objectToPointer(_data.Data(desc))
            d.pointer=_C.cast(_C.pointer(descs_ptrs),_C.c_void_p)
        else:
            d.arsize=0
            d.pointer=_C.c_void_p(0)
        d.a0=d.pointer
        return _compound.Compound.descriptorWithProps(self,d)
      except:
          import traceback
          traceback.print_exc()

    @classmethod
    def fromDescriptor(cls,d):
        num   = d.arsize/d.length
        dptrs = _C.cast(d.pointer,_C.POINTER(_C.c_void_p*num)).contents
        descs = [_descriptor.pointerToObject(dptr) for dptr in dptrs]
        return cls(descs)


    def __hasBadTreeReferences__(self,tree):
        for desc in self._descs:
            if isinstance(desc,_data.Data) and desc.__hasBadTreeReferences__(tree):
                return True
        return False


    def __fixTreeReferences__(self,tree):
        for idx in range(len(self._descs)):
            if isinstance(self._descs[idx],_data.Data) and self._descs[idx].__hasBadTreeReferences__(tree):
                self._descs[idx]=self._descs[idx].__fixTreeReferences__(tree)
        return self

    def __init__(self,value,dtype=0):
        """Initializes a Apd instance
        """
        if value is self: return
        if isinstance(value,(tuple,list,_ver.generator)):
            self._descs=list(value)
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
            return self._descs[idx]
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
        return self[idx]

    def setDescs(self,descs):
        """Set the descriptors of the Apd.
        @type descs: tuple
        @rtype: None
        """
        if isinstance(descs,(tuple,list,_ver.generator)):
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
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return tuple(self._descs)


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
            elif isinstance(value,(tuple,list,Apd)):
                if  len(value)&1:
                    raise Dictionary._key_value_exception
                for idx in range(0,len(value),2):
                    self.setdefault(value[idx],value[idx+1])
            elif isinstance(value,(_ver.generator)):
                for key in value:
                    self.setdefault(key,_ver.next(value))
            else:
                raise TypeError('Cannot create Dictionary from type: '+str(type(value)))

    @staticmethod
    def toKey(key):
        if isinstance(key,_scalar.Scalar):
            key = key.value
        if isinstance(key,_N.string_):
            return str(key)
        if isinstance(key,_N.int32):
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
            if isinstance(value,(tuple,list,Apd,_ver.generator,_N.ndarray)):
                for val in value:
                    self.append(_data.Data(val))
            else:
                raise TypeError('Cannot create List from type: '+str(type(value)))

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return tuple(self)

    @property
    def value(self):
        """Returns native representation of the List"""
        return _N.asarray(tuple(d.value for d in self.descs),'object')

descriptor=_mimport('descriptor')
descriptor.dtypeToClass[Apd.dtype_id]=Apd
descriptor.dtypeToClass[List.dtype_id]=List
descriptor.dtypeToClass[Dictionary.dtype_id]=Dictionary

