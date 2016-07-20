def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import copy as _copy
import numpy as _N
import ctypes as _C

_data=_mimport('mdsdata')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')

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
        return cls(value=tuple(descs))


    def __hasBadTreeReferences__(self,tree):
        for desc in self.descs:
            if isinstance(desc,_data.Data) and desc.__hasBadTreeReferences__(tree):
                return True
        return False

    def __fixTreeReferences__(self,tree):
        descs=list(ans.descs)
        for idx in range(len(descs)):
            if isinstance(descs[idx],_data.Data) and descs[idx].__hasBadTreeReferences__(tree):
                descs[idx]=descs[idx].__fixTreeReferences__(tree)
        descs=tuple(descs)
        return type(self)(descs)

    def __init__(self,descs,dtype=0):
        """Initializes a Apd instance
        """
        if isinstance(descs,tuple):
            self.descs=descs
            self.dtype=dtype
        else:
            raise TypeError("must provide tuple of items when creating ApdData")
        return

    def __len__(self):
        """Return the number of descriptors in the apd"""
        return len(self.descs)

    def __getitem__(self,idx):
        """Return descriptor(s) x.__getitem__(idx) <==> x[idx]
        @rtype: Data|tuple
        """
        try:
            return self.descs[idx]
        except:
            return None
        return

    def __setitem__(self,idx,value):
        """Set descriptor. x.__setitem__(idx,value) <==> x[idx]=value
        @rtype: None
        """
        l=list(self.descs)
        while len(l) <= idx:
            l.append(None)
        l[idx]=value
        self.descs=tuple(l)
        return None

    def getDescs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return self.descs

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
        if isinstance(descs,tuple):
            self.descs=descs
        else:
            raise TypeError("must provide tuple")
        return

    def setDescAt(self,idx,value):
        """Set a descriptor in the Apd
        """
        self[idx]=value
        return None

    def append(self,value):
        """Append a value to apd"""
        self[len(self)]=value

    def data(self):
        """Returns native representation of the apd"""
        l=list()
        for d in self.descs:
            l.append(d.data())
        return tuple(l)

class Dictionary(dict,Apd):
    """dictionary class"""

    dtype_id=216

    def __init__(self,value=None):
        if value is not None:
            if isinstance(value,dict):
                for key,val in value.items():
                    self.setdefault(key,val)
            elif isinstance(value,tuple):
                for idx in range(0,len(value),2):
                    key=value[idx]
                    if isinstance(key,_scalar.Scalar):
                        key=key.value
                    if isinstance(key,_N.string_):
                        key=str(key)
                    elif isinstance(key,_N.int32):
                        key=int(key)
                    elif isinstance(key,_N.float32) or isinstance(key,_N.float64):
                        key=float(key)
                    val=value[idx+1]
                    if isinstance(val,Apd):
                        val=Dictionary(val)
                    self.setdefault(key,val)
            else:
                raise TypeError('Cannot create Dictionary from type: '+str(type(value)))

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        descs=[]
        items=self.items()
        for item in items:
            descs=descs+list(item)
        return tuple(descs)

    def __getattr__(self,name):
        if name in self.keys():
            return self[name]
        else:
            raise AttributeError('No such attribute: '+name)

    def __setattr__(self,name,value):
        if name in self.keys():
            self[name]=value
        elif hasattr(self,name):
            self.__dict__[name]=value
        else:
            self.setdefault(name,value)

    def data(self):
        """Return native representation of data item"""
        d=dict()
        for key,val in self.items():
            d.setdefault(key,val.data())
        return d

    def toApd(self):
        apd=Apd(tuple(),self.dtype_id)
        for key,val in self.items():
            apd.append(key)
            apd.append(val)
        return apd

    def __str__(self):
        return dict.__str__(self)

class List(list,Apd):
    """list class"""

    dtype_id=214

    def __init__(self,value=None):
        if value is not None:
            if isinstance(value,Apd) or isinstance(value,list) or isinstance(value,tuple):
                for idx in range(len(value)):
                    super(List,self).append(value[idx])
            else:
                raise TypeError('Cannot create List from type: '+str(type(value)))

    def __str__(self):
        return list.__str__(self)

    @property
    def descs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        descs=[]
        for item in self:
            descs.append(item)
        return tuple(descs)

descriptor=_mimport('descriptor')
descriptor.dtypeToClass[Apd.dtype_id]=Apd
descriptor.dtypeToClass[List.dtype_id]=List
descriptor.dtypeToClass[Dictionary.dtype_id]=Dictionary
                        
