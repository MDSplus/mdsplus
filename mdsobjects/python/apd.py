def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N
import ctypes as _C

_data=_mimport('mdsdata')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_ver=_mimport('version')

class Apd(_data.Data):
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
            d.arsize=_C.sizeof(_C.c_void_p)*len(descs)
            descs_ptrs=(_C.c_void_p*len(descs))()
            for idx in range(len(descs)):
                desc=descs[idx]
                desc=_data.makeData(desc)
                desc_d=desc.descriptor
                d.original.append(desc_d)
                descs_ptrs[idx]=_C.cast(_C.pointer(desc_d),_C.c_void_p)
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
        num=d.arsize/d.length
        dptrs=_C.cast(d.pointer,_C.POINTER(_C.c_void_p*num)).contents
        descs=[]
        for idx in range(num):
            d_d_ptr=dptrs[idx]
            d_d=_C.cast(d_d_ptr,_C.POINTER(descriptor.Descriptor)).contents
            d_d=_C.cast(d_d_ptr,_C.POINTER(descriptor.dclassToClass[d_d.dclass])).contents
            descs.append(d_d.value)
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

    def __init__(self,descs,dtype=0):
        """Initializes a Apd instance
        """
        if isinstance(descs,(tuple,list,_ver.generator)):
            self._descs=list(descs)
            self.dtype=dtype
        else:
            raise TypeError("must provide tuple of items when creating ApdData: %s"%(type(descs),))

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
            self._descs=list(descs)
        else:
            raise TypeError("must provide tuple")
        return self

    def setDescAt(self,idx,value):
        """Set a descriptor in the Apd
        """
        self[idx]=value
        return self

    def append(self,value):
        """Append a value to apd"""
        self[len(self)]=value
        return self

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return tuple(self._descs)


class Dictionary(dict,Apd):
    """dictionary class"""
    _key_value_exception = Exception('A dictionary requires an even number of elements read as key-value pairs.')

    dtype_id=216

    def __init__(self,value=None):
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
    def tokey(key):
        if isinstance(key,_scalar.Scalar):
            return key.value
        if isinstance(key,_N.string_):
            return str(key)
        elif isinstance(key,_N.int32):
            return int(key)
        elif isinstance(key,(_N.float32,_N.float64)):
            return float(key)
        else:
            return _data.Data.make(key).data()

    def setdefault(self,key,val):
        """check keys and converts values to instances of Data"""
        key = Dictionary.tokey(key)
        if not isinstance(val,_data.Data):
            val=_data.Data.make(val)
        super(Dictionary,self).setdefault(key,val)

    def remove(self,key):
        """remove pair with key"""
        del(self[Dictionary.tokey(key)])

    def __setitem__(self,name,value):
        """sets values as instances of Data"""
        self.setdefault(name,value)

    def __getitem__(self,name):
        """gets values as instances of Data"""
        return super(Dictionary,self).__getitem__(Dictionary.tokey(name))

    @property
    def value(self):
        """Return native representation of data item"""
        return dict(zip(self.keys(),map(_data.Data.data,self.values())))

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
        if value is not None:
            if isinstance(value,(tuple,list,Apd,_ver.generator)):
                for val in value:
                    self.append(val)
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
        return list(map(_data.Data.data,self._descs))


descriptor=_mimport('descriptor')
descriptor.dtypeToClass[Apd.dtype_id]=Apd
descriptor.dtypeToClass[List.dtype_id]=List
descriptor.dtypeToClass[Dictionary.dtype_id]=Dictionary

