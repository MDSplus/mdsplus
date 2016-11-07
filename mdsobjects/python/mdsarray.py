def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N
import ctypes as _C

_scalar=_mimport('mdsscalar')
_data=_mimport('mdsdata')
_ver=_mimport('version')
_descriptor=_mimport('descriptor')
_compound=_mimport('compound')

class Array(_data.Data):
    ctype = None
    __MAX_DIM = 8
    def __new__(cls,*value):
        """Convert a python object to a MDSobject Data array
        @param value: Any value
        @type data: any
        @rtype: Array
        """
        if cls is not Array or len(value)==0:
            return object.__new__(cls)
        value=value[0]
        if isinstance(value,(cls,)):
            return value
        if isinstance(value,_scalar.Scalar):
            value = value._value
        if isinstance(value,_C.Array):
            try: value = _N.ctypeslib.as_array(value)
            except: pass
        if isinstance(value,(tuple,list)):
            if len(value)==0:
                return Int32Array.__new__(Int32Array,[])
            try:
                value = _N.array(value)
            except (ValueError,TypeError):
                newlist=list()
                for i in value:
                    newlist.append(_data.Data(i).data())
                value = _N.array(newlist)
        if isinstance(value,(_N.ndarray,_N.generic)):
            if   str(value.dtype)[1] in 'SU':
                cls = StringArray
            elif str(value.dtype) == 'bool':
                cls = Uint8Array
            elif str(value.dtype) == 'object':
                cls = _apd.List
            else:
                cls = globals()[str(value.dtype).capitalize()+'Array']
        else:
            raise TypeError('Cannot make Array out of '+str(type(value)))
        return cls.__new__(cls,value)

    def __init__(self,value=0):
        if value is self: return
        if self.__class__ is Array:
            raise TypeError("cannot instantiate 'Array'")
        if isinstance(value,_C.Array):
            try:
                value=_N.ctypeslib.as_array(value)
            except Exception:
                pass
        value = _N.array(value)
        if len(value.shape) == 0 or len(value.shape) > Array.__MAX_DIM:  # happens if value has been a scalar, e.g. int
            value = value.reshape(1)
        self._value = value.__array__(_N.__dict__[self.__class__.__name__[0:-5].lower()])
        return

    def __getattr__(self,name):
        if name=='_value': raise Exception('_value undefined')
        return self._value.__getattribute__(name)

    @property
    def value(self):
        """Return the numpy ndarray representation of the array"""
        return self._value

    def _unop(self,op):
        return _data.Data(getattr(self._value,op)())

    def _binop(self,op,y):
        if hasattr(y,'_value'): y=y._value
        return _data.Data(getattr(self._value,op)(y))

    def _triop(self,op,y,z):
        if hasattr(y,'_value'): y=y._value
        if hasattr(z,'_value'): z=z._value
        return _data.Data(getattr(self._value,op)(y,z))

    def __array__(self):
        return self.value

    def __copy__(self):
        return type(self)(self._value)

    def __deepcopy__(self,memo=None):
        return self.__copy__()

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return len(self.data())

    def __setitem__(self,index,value):
        self._value[index]=value

    def __getitem__(self,index):
        return _data.Data(self._value[index])

    def getElementAt(self,index):
        return self[index]

    def setElementAt(self,index,value):
        self[index] = value

    def all(self):
        return self._unop('all')

    def any(self):
        return self._unop('any')

    def argmax(self,*axis):
        if axis:
            return self._binop('argmax',axis[0])
        return self._unop('argmax')

    def argmin(self,*axis):
        if axis:
            return self._binop('argmin',axis[0])
        return self._unop('argmin')

    def argsort(self,axis=-1,kind='quicksort',order=None):
        return _data.Data(self._value.argsort(axis,kind,order))

    def astype(self,type):
        return _data.Data(self._value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)

    def decompile(self):
        def arrayDecompile(a,cl):
            ans='['
            if len(a.shape)==1:
                for idx in range(len(a)):
                    if idx > 0: ans+=', '
                    ans+=cl(a[idx]).decompile()
            else:
                for idx in range(a.shape[0]):
                    if idx > 0: ans+=', '
                    ans+=arrayDecompile(a[idx],cl)
            return ans+']'

        cl=_scalar.__dict__[str(type(self))[:-5]]
        return arrayDecompile(self._value,cl)

    @property
    def descriptor(self):
        value=self._value
        dti = str(value.dtype)[1]
        if dti in 'SU' and not dti=='S':
            value = value.astype('S')
        if not value.flags['CONTIGUOUS']:
            value=_N.ascontiguousarray(value)
        value = value.T
        if not value.flags.f_contiguous:
            value=value.copy('F')
        d=_descriptor.Descriptor_a()
        d.scale=0
        d.digits=0
        d.dtype=self.dtype_id
        d.length=value.itemsize
        d.pointer=_C.c_void_p(value.ctypes.data)
        d.dimct=value.ndim
        d.aflags=48
        d.arsize=value.nbytes
        d.a0=d.pointer
        if d.dimct > 1:
            d.coeff=True
            for i in range(d.dimct):
                d.coeff_and_bounds[i]=_N.shape(value)[i]
        d.original=self
        if self._units is not None or self._error is not None or self._help is not None or self._validation is not None:
            return _compound.Compound.descriptorWithProps(self,d)
        return d

    @classmethod
    def fromDescriptor(cls,d):
        def getNumpy(ntype,ctype,length):
            buffer = _ver.buffer(_C.cast(d.pointer,_C.POINTER(ctype*length)).contents)
            return _N.ndarray(shape,ntype,buffer)
        _tree=_mimport('tree')
        if d.dtype == 0:
            d.dtype = Int32Array.dtype_id
        if d.coeff:
            d.arsize=d.length
            shape=list()
            for i in range(d.dimct):
                dim=d.coeff_and_bounds[d.dimct-i-1]
                if dim > 0:
                    d.arsize=d.arsize*dim
                    shape.append(dim)
        else:
            shape=[int(d.arsize/d.length),]
        if d.dtype == StringArray.dtype_id:
            return StringArray(getNumpy(_N.dtype(('S',d.length)),_C.c_byte,d.arsize))
        if d.dtype == _tree.TreeNode.dtype_id:
            d.dtype=Int32Array.dtype_id
            nids=getNumpy(_N.int32,_C.c_int32,int(d.arsize/d.length))
            return _tree.TreeNodeArray(list(nids))
        if d.dtype == Complex64Array.dtype_id:
            return Array(getNumpy(_N.complex64,_C.c_float,int(d.arsize*2/d.length)))
        if d.dtype == Complex128Array.dtype_id:
            return Array(getNumpy(_N.complex128,_C.c_double,int(d.arsize*2/d.length)))
        if d.dtype in _descriptor.dtypeToArrayClass:
            cls = _descriptor.dtypeToArrayClass[d.dtype]
            if cls.ctype is not None:
                return Array(getNumpy(cls.ctype,cls.ctype,int(d.arsize/d.length)))
        raise TypeError('Arrays of dtype %d are unsupported.' % d.dtype)

makeArray = Array

class Int8Array(Array):
    """8-bit signed number"""
    dtype_id=6
    ctype=_C.c_int8

    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _data.Data.deserialize(self)

class Int16Array(Array):
    """16-bit signed number"""
    dtype_id=7
    ctype=_C.c_int16

class Int32Array(Array):
    """32-bit signed number"""
    dtype_id=8
    ctype=_C.c_int32

class Int64Array(Array):
    """64-bit signed number"""
    dtype_id=9
    ctype=_C.c_int64

class Uint8Array(Array):
    """8-bit unsigned number"""
    dtype_id=2
    ctype=_C.c_uint8
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _data.Data.deserialize(self)

class Uint16Array(Array):
    """16-bit unsigned number"""
    dtype_id=3
    ctype=_C.c_uint16

class Uint32Array(Array):
    """32-bit unsigned number"""
    dtype_id=4
    ctype=_C.c_uint32

class Uint64Array(Array):
    """64-bit unsigned number"""
    dtype_id=5
    ctype=_C.c_uint64

class Float32Array(Array):
    """32-bit floating point number"""
    dtype_id=52
    ctype=_C.c_float

class FloatFArray(Float32Array):
    """64-bit floating point number"""
    dtype_id=10

class Complex64Array(Array):
    """32-bit complex number"""
    dtype_id=54

class Float64Array(Array):
    """64-bit floating point number"""
    dtype_id=53
    ctype=_C.c_double

class FloatDArray(Float64Array):
    """64-bit floating point number"""
    dtype_id=11

class FloatGArray(Float64Array):
    """64-bit floating point number"""
    dtype_id=27

class Complex128Array(Array):
    """64-bit complex number"""
    dtype_id=55

class StringArray(Array):
    """String"""
    dtype_id=14

    def __init__(self,value):
        if value is self: return
        if isinstance(value, (StringArray,)):
            self._value = value._value
            return
        if not isinstance(value,(_N.ndarray,)):
            value = _N.array(value)
        if not value.dtype.type is _ver.npbytes:
            try: value = value.astype(_ver.npbytes)
            except:
                value = _N.array(_ver.tobytes(value.tolist()))
        for i in _ver.xrange(len(value.flat)):
            value.flat[i]=value.flat[i].ljust(value.itemsize)
        self._value = value

    def __radd__(self,y):
        """Reverse add: x.__radd__(y) <==> y+x
        @rtype: Data"""
        return self.execute('$//$',y,self)
    def __add__(self,y):
        """Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return self.execute('$//$',self,y)

class Int128Array(Array):
    """128-bit signed number"""
    dtype_id=26
    def __init__(self):
        raise TypeError("Int128Array is not yet supported")

class Uint128Array(Array):
    """128-bit unsigned number"""
    dtype_id=25
    def __init__(self):
        raise TypeError("Uint128Array is not yet supported")

_apd=_mimport('apd')
_descriptor.dtypeToArrayClass[Uint8Array.dtype_id]=Uint8Array
_descriptor.dtypeToArrayClass[Uint16Array.dtype_id]=Uint16Array
_descriptor.dtypeToArrayClass[Uint32Array.dtype_id]=Uint32Array
_descriptor.dtypeToArrayClass[Uint64Array.dtype_id]=Uint64Array
_descriptor.dtypeToArrayClass[Uint128Array.dtype_id]=Uint128Array
_descriptor.dtypeToArrayClass[Int8Array.dtype_id]=Int8Array
_descriptor.dtypeToArrayClass[Int16Array.dtype_id]=Int16Array
_descriptor.dtypeToArrayClass[Int32Array.dtype_id]=Int32Array
_descriptor.dtypeToArrayClass[Int64Array.dtype_id]=Int64Array
_descriptor.dtypeToArrayClass[Int128Array.dtype_id]=Int128Array
_descriptor.dtypeToArrayClass[Float32Array.dtype_id]=Float32Array
_descriptor.dtypeToArrayClass[FloatFArray.dtype_id]=Float32Array
_descriptor.dtypeToArrayClass[Float64Array.dtype_id]=Float64Array
_descriptor.dtypeToArrayClass[FloatDArray.dtype_id]=Float64Array
_descriptor.dtypeToArrayClass[FloatGArray.dtype_id]=Float64Array
_descriptor.dtypeToArrayClass[Complex64Array.dtype_id]=Complex64Array
_descriptor.dtypeToArrayClass[Complex128Array.dtype_id]=Complex128Array
_descriptor.dtypeToArrayClass[StringArray.dtype_id]=StringArray

