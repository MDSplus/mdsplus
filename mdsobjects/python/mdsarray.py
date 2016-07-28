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
descriptor=_mimport('descriptor')
_compound=_mimport('compound')

def makeArray(value):
    if isinstance(value,Array):
        return value
    if isinstance(value,_scalar.Scalar):
        return makeArray((value._value,))
    if isinstance(value,_C.Array):
        try:
            return makeArray(_N.ctypeslib.as_array(value))
        except Exception:
            pass
    if isinstance(value,tuple) | isinstance(value,list):
        try:
            ans=_N.array(value)
            if str(ans.dtype)[1:2] in 'SU':
                ans = ans.astype(_ver.npstr)
            return makeArray(ans)
        except (ValueError,TypeError):
            newlist=list()
            for i in value:
                newlist.append(_data.makeData(i).data())
            return makeArray(_N.array(newlist))
    if isinstance(value,_N.ndarray):
        if str(value.dtype)[0:2] in ['|S', '<U']:
            return StringArray(value)
        if str(value.dtype) == 'bool':
            return makeArray(value.__array__(_N.uint8))
        if str(value.dtype) == 'object':
            raise TypeError('cannot make Array out of an numpy.ndarray of dtype object')
        return globals()[str(value.dtype).capitalize()+'Array'](value)
    if isinstance(value,(_N.generic, int, _ver.long, float, str, bool)):
        return makeArray(_N.array(value).reshape(1))
    raise TypeError('Cannot make Array out of '+str(type(value)))

def arrayDecompile(a,cl):
    if len(a.shape)==1:
        ans='['
        for idx in range(len(a)):
            sval=cl(a[idx])
            if idx < len(a)-1:
                ending=','
            else:
                ending=']'
            ans=ans+sval.decompile()+ending
        return ans
    else:
        ans='['
        for idx in range(a.shape[0]):
            if idx < a.shape[0]-1:
                ending=', '
            else:
                ending=']'
            ans=ans+arrayDecompile(a[idx],cl)+ending
        return ans

class Array(_data.Data):
    ctype=None
    
    def __init__(self,value=0):
        if self.__class__.__name__ == 'Array':
            raise TypeError("cannot create 'Array' instances")
        if self.__class__.__name__ == 'StringArray':
            self._value=_N.array(value).__array__(_N.str_)
            return
        if isinstance(value,_C.Array):
            try:
                value=_N.ctypeslib.as_array(value)
            except Exception:
                pass
        value = _N.array(value)
        if len(value.shape) == 0:  # happens if value has been a scalar, e.g. int
            value = value.reshape(1)
        self._value = value.__array__(_N.__dict__[self.__class__.__name__[0:-5].lower()])
        return

    def __getattr__(self,name):
        return self._value.__getattribute__(name)

    @property
    def value(self):
        """Return the numpy ndarray representation of the array"""
        return self._value

    def _unop(self,op):
        return _data.makeData(getattr(self._value,op)())

    def _binop(self,op,y):
        try:
            y=y._value
        except AttributeError:
            pass
        return _data.makeData(getattr(self._value,op)(y))

    def _triop(self,op,y,z):
        try:
            y=y._value
        except AttributeError:
            pass
        try:
            z=z._value
        except AttributeError:
            pass
        return _data.makeData(getattr(self._value,op)(y,z))

    def __array__(self):
        raise TypeError('__array__ not yet supported')

    def __copy__(self):
        return type(self)(self._value)

    def __deepcopy__(self,memo=None):
        return self.__copy__()

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return len(self.data())
    
    def getElementAt(self,itm):
        return _data.makeData(self._value[itm])

    def setElementAt(self,i,y):
        self._value[i]=y

    def all(self):
        return self._unop('all')

    def any(self):
        return self._unop('any')

    def argmax(self,*axis):
        if axis:
            return self._binop('argmax',axis[0])
        else:
            return self._unop('argmax')

    def argmin(self,*axis):
        if axis:
            return self._binop('argmin',axis[0])
        else:
            return self._unop('argmin')

    def argsort(self,axis=-1,kind='quicksort',order=None):
        return _data.makeData(self._value.argsort(axis,kind,order))

    def astype(self,type):
        return _data.makeData(self._value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)


    def decompile(self):
        if str(self._value.dtype)[0:2] in ['|S', '<U']:
            cl=_scalar.String
        else:
            cl=_scalar.__dict__[str(self._value.dtype).capitalize()]
        return arrayDecompile(self._value,cl)

    @property
    def descriptor(self):
        value=self._value
        if str(value.dtype)[1] in 'SU':
            value = value.astype('S')
            for i in range(len(value.flat)):
                value.flat[i]=value.flat[i].ljust(value.itemsize)
        if not value.flags['CONTIGUOUS']:
            value=_N.ascontiguousarray(value)
        value = value.T
        if not value.flags.f_contiguous:
            value=value.copy('F')
        d=descriptor.Descriptor_a()
        d.scale=0
        d.digits=0
        d.aflags=0
        d.dtype=self.dtype_id
        d.length=value.itemsize
        d.pointer=_C.c_void_p(value.ctypes.data)
        d.dimct=_N.shape(_N.shape(value))[0]
        d.arsize=value.nbytes
        d.a0=d.pointer
        if d.dimct > 1:
            d.coeff=1
            for i in range(d.dimct):
                d.coeff_and_bounds[i]=_N.shape(value)[i]
        d.original=self
        if self._units or self._error is not None or self._help is not None or self._validation is not None:
            return _compound.Compound.descriptorWithProps(self,d)
        else:
            return d

    @classmethod
    def fromDescriptor(cls,d):
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
            return StringArray(
                _N.ndarray(shape=shape,
                           dtype=_N.dtype(('S',d.length)),
                           buffer=_ver.buffer(
                               _C.cast(
                                   d.pointer,
                                   _C.POINTER(_C.c_byte*d.arsize)).contents)))
        if d.dtype == _tree.TreeNode.dtype_id:
            d.dtype=Int32Array.dtype_id
            nids=_N.ndarray(
                shape=shape,
                dtype=_N.int32,
                buffer=_ver.buffer(
                    _C.cast(
                        d.pointer,
                        _C.POINTER(_C.c_int32 * int(d.arsize/d.length))).contents))
            return _tree.TreeNodeArray(list(nids))
        if d.dtype == 10: ### VMS FLOAT
            return _array.makeArray(_data.Data.execute("float($)",(d,)))
        if d.dtype == 11 or d.dtype == 27: ### VMS DOUBLES
            return _array.makeArray(_data.Data.execute("FT_FLOAT($)",(d,)))
        if d.dtype == Complex64Array.dtype_id:
            return _array.makeArray(
                _N.ndarray(
                    shape=shape,
                    dtype=_N.complex64,
                    buffer=_ver.buffer(
                        _C.cast(
                            d.pointer,
                            _C.POINTER(
                                _C.c_float * int(d.arsize*2/d.length))).contents)))
        if d.dtype == Complex128Array.dtype_id:
            return _array.makeArray(
                _N.ndarray(
                    shape=shape,
                    dtype=_N.complex128,
                    buffer=_ver.buffer(
                        _C.cast(
                            d.pointer,
                            _C.POINTER(_C.c_double * int(d.arsize*2/d.length))).contents)))
        if d.dtype in descriptor.dtypeToArrayClass:
            cls = descriptor.dtypeToArrayClass[d.dtype]
            if cls.ctype is not None:
                a=_N.ndarray(
                    shape=shape,
                    dtype=cls.ctype,
                    buffer=_ver.buffer(
                        _C.cast(
                            d.pointer,
                            _C.POINTER(
                                cls.ctype * int(d.arsize/d.length))).contents))
                return makeArray(a)
        raise TypeError('Arrays of dtype %d are unsupported.' % d.dtype)

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

class Complex64Array(Array):
    """32-bit complex number"""
    dtype_id=54

class Float64Array(Array):
    """64-bit floating point number"""
    dtype_id=53
    ctype=_C.c_double

class Complex128Array(Array):
    """64-bit complex number"""
    dtype_id=55

class StringArray(Array):
    """String"""
    dtype_id=14
    def __radd__(self,y):
        """Reverse add: x.__radd__(y) <==> y+x
        @rtype: Data"""
        return self.execute('$//$',y,self)
    def __add__(self,y):
        """Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return self.execute('$//$',self,y)
    def __str__(self):
        """String: x.__str__() <==> str(x)
        @rtype: String"""
        return self.decompile()

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

descriptor.dtypeToArrayClass[Uint8Array.dtype_id]=Uint8Array
descriptor.dtypeToArrayClass[Uint16Array.dtype_id]=Uint16Array
descriptor.dtypeToArrayClass[Uint32Array.dtype_id]=Uint32Array
descriptor.dtypeToArrayClass[Uint64Array.dtype_id]=Uint64Array
descriptor.dtypeToArrayClass[Uint128Array.dtype_id]=Uint128Array
descriptor.dtypeToArrayClass[Int8Array.dtype_id]=Int8Array
descriptor.dtypeToArrayClass[Int16Array.dtype_id]=Int16Array
descriptor.dtypeToArrayClass[Int32Array.dtype_id]=Int32Array
descriptor.dtypeToArrayClass[Int64Array.dtype_id]=Int64Array
descriptor.dtypeToArrayClass[Int128Array.dtype_id]=Int128Array
descriptor.dtypeToArrayClass[Float32Array.dtype_id]=Float32Array
descriptor.dtypeToArrayClass[Float64Array.dtype_id]=Float64Array
descriptor.dtypeToArrayClass[Complex64Array.dtype_id]=Complex64Array
descriptor.dtypeToArrayClass[Complex128Array.dtype_id]=Complex128Array
descriptor.dtypeToArrayClass[StringArray.dtype_id]=StringArray

