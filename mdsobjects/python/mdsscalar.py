def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N
import ctypes as _C

_data=_mimport('mdsdata')
_array=_mimport('mdsarray')
_ver=_mimport('version')
descriptor=_mimport('descriptor')

def makeScalar(value):
    if isinstance(value,Scalar):
        return value
        from copy import deepcopy
        return deepcopy(value)
    if isinstance(value,_ver.basestring):
        return String(value)
    if isinstance(value,_N.generic):
        if isinstance(value,(_N.string_, _N.unicode_)):  # includes _N.bytes_
            return String(value)
        if isinstance(value,_N.bool_):
            return makeScalar(int(value))
        return globals()[value.__class__.__name__.capitalize()](value)
    if isinstance(value,_C._SimpleCData):
        if isinstance(value,_C.c_int64):
            return Int64(value.value)
        if isinstance(value,_C.c_uint64):
            return Uint64(value.value)
        if isinstance(value,_C.c_int32):
            return Int32(value.value)
        if isinstance(value,_C.c_uint32):
            return Uint32(value.value)
        if isinstance(value,_C.c_int16):
            return Int16(value.value)
        if isinstance(value,_C.c_uint16):
            return Uint16(value.value)
        if isinstance(value,_C.c_int8):
            return Int8(value.value)
        if isinstance(value,(_C.c_uint8,_C.c_bool)):
            return Uint8(value.value)
    if isinstance(value,_ver.long):
        return Int64(value)
    if isinstance(value,int):
        return Int32(value)
    if isinstance(value,float):
        return Float32(value)
    if isinstance(value,bool):
        return Int8(int(value))
    if isinstance(value,complex):
        return Complex128(_N.complex128(value))
    if isinstance(value,_N.complex64):
        return Complex64(value)
    if isinstance(value,_N.complex128):
        return Complex128(value)
    raise TypeError('Cannot make Scalar out of '+str(type(value)))

class Scalar(_data.Data):
    _value = None
    def __new__(cls,value=0):
        try:
            if (isinstance(value,_array.Array)) or isinstance(value,list) or isinstance(value, _N.ndarray):
               return _array.__dict__[cls.__name__+'Array'](value)
        except:
            pass
        return super(Scalar,cls).__new__(cls)

    def __init__(self,value=0):
        if isinstance(value,self.__class__):
            self._value=value._value
        elif self.__class__ == Scalar:
            raise TypeError("cannot create 'Scalar' instances")
        else:
            self._value = _N.__dict__[self.__class__.__name__.lower()](value)

    @property
    def mdsdtype(self):
        return self.dtype_id

    @property
    def descriptor(self):
        d=descriptor.Descriptor_s()
        d.length=self._value.nbytes
        d.dtype=self.dtype_id
        array=_N.array(self._value)
        d.pointer=_C.c_void_p(array.ctypes.data)
        d.original=self
        d.array=array
        if self._units or self._error is not None or self._help is not None or self._validation is not None:
            return _compound.Compound.descriptorWithProps(self,d)
        else:
            return d

    def __getattr__(self,name):
        if name.startswith("__array"):
          raise AttributeError
        return self._value.__getattribute__(name)

    @property
    def value(self):
        """Return the numpy scalar representation of the scalar"""
        return self._value

    def __str__(self):
        formats={Int8:'%dB',Int16:'%dW',Int32:'%d',Int64:'0X%0uQ',
                 Uint8:'%uBU',Uint16:'%uWU',Uint32:'%uLU',Uint64:'0X%0xQU',
                 Float32:'%g'}
        ans=formats[self.__class__] % (self._value,)
        if ans=='nan':
            ans="$ROPRAND"
        elif isinstance(self,Float32) and ans.find('.')==-1:
            ans=ans+"."
        return ans

    def decompile(self):
        return _ver.tostr(self)

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return self._value.__int__()

    def __long__(self):
        """Long: x.__long__() <==> long(x)
        @rtype: int"""
        return self.__value.__long__()

    def _unop(self,op):
        return _data.makeData(getattr(self.value,op)())

    def _binop(self,op,y):
        try:
            y=y.value
        except AttributeError:
            pass
        ans=getattr(self.value,op)(y)
        return _data.makeData(ans)

    def _triop(self,op,y,z):
        try:
            y=y.value
        except AttributeError:
            pass
        try:
            z=z.value
        except AttributeError:
            pass
        return _data.makeData(getattr(self.value,op)(y,z))

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
        return _data.makeData(self.value.argsort(axis,kind,order))

    def astype(self,type):
        return _data.makeData(self.value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)

    @classmethod
    def fromDescriptor(cls,d):
        value=_C.cast(d.pointer,_C.POINTER(cls._ctype)).contents
        if isinstance(value,_C.Array):
            ans = cls(cls._ntype(complex(value[0],value[1])))
        else:
            ans = cls(cls._ntype(value.value))
        return ans

class Int8(Scalar):
    """8-bit signed number"""
    dtype_id=6
    _ctype=_C.c_int8
    _ntype=_N.int8

class Int16(Scalar):
    """16-bit signed number"""
    dtype_id=7
    _ctype=_C.c_int16
    _ntype=_N.int16

class Int32(Scalar):
    """32-bit signed number"""
    dtype_id=8
    _ctype=_C.c_int32
    _ntype=_N.int32

class Int64(Scalar):
    """64-bit signed number"""
    dtype_id=9
    _ctype=_C.c_int64
    _ntype=_N.int64

class Uint8(Scalar):
    """8-bit unsigned number"""
    dtype_id=2
    _ctype=_C.c_uint8
    _ntype=_N.uint8

class Uint16(Scalar):
    """16-bit unsigned number"""
    dtype_id=3
    _ctype=_C.c_uint16
    _ntype=_N.uint16

class Uint32(Scalar):
    """32-bit unsigned number"""
    dtype_id=4
    _ctype=_C.c_uint32
    _ntype=_N.uint32

class Uint64(Scalar):
    """64-bit unsigned number"""
    dtype_id=5
    _ctype=_C.c_uint64
    _ntype=_N.uint64
    _utc0 = _N.uint64("35067168000000000")
    _utc1 = 1E7
    @staticmethod
    def fromTime(value):
        """converts from seconds since 01-JAN-1970 00:00:00.00
        For example:
           import MDSplus
           import time
           mdstime=MDSplus.Uint64.fromTime(time.time()-time.altzone)
           print(mdstime.date)
        """
        return Uint64(int(value * Uint64._utc1) + Uint64._utc0)

    def _getDate(self):
        return _data.Data.execute('date_time($)',self)
    date=property(_getDate)

    def _getTime(self):
        """returns date in seconds since 01-JAN-1970 00:00:00.00"""
        return float(self.value - Uint64._utc0) / Uint64._utc1
    time=property(_getTime)

class Float32(Scalar):
    """32-bit floating point number"""
    dtype_id=52
    _ctype=_C.c_float
    _ntype=_N.float32

class Complex64(Scalar):
    """32-bit complex number"""
    dtype_id=54
    _ctype=_C.c_float*2
    _ntype=_N.complex64
    def __str__(self):
        return "Cmplx(%g,%g)" % (self._value.real,self._value.imag)

class Float64(Scalar):
    """64-bit floating point number"""
    dtype_id=53
    _ctype=_C.c_double
    _ntype=_N.float64
    def __str__(self):
        return ("%E" % self._value).replace("E","D")

class Complex128(Scalar):
    """64-bit complex number"""
    dtype_id=55
    _ctype=_C.c_double*2
    _ntype=_N.complex128
    def __str__(self):
        return "Cmplx(%s,%s)" % (str(Float64(self._value.real)),str(Float64(self._value.imag)))

class String(Scalar):
    """String"""
    dtype_id=14

    @property
    def descriptor(self):
        d=descriptor.Descriptor_s()
        d.length=len(self)
        d.dtype=self.dtype_id
        d.pointer=_C.cast(_C.c_char_p(_ver.tobytes(str(self))),_C.c_void_p)
        d.original=self
        if self._units or self._error is not None or self._help is not None or self._validation is not None:
            return _compound.Compound.descriptorWithProps(self,d)
        else:
            return d

    @classmethod
    def fromDescriptor(cls,d):
        if d.length == 0:
            return cls('')
        else:
            return cls(_N.array(_C.cast(d.pointer,_C.POINTER((_C.c_byte*d.length))).contents[:],dtype=_N.uint8).tostring())

    def __radd__(self,y):
        """Reverse add: x.__radd__(y) <==> y+x
        @rtype: Data"""
        return self.execute('$//$',y,self)
    def __add__(self,y):
        """Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return self.execute('$//$',self,y)
    def __contains__(self,y):
        """Contains: x.__contains__(y) <==> y in x
        @rtype: Bool"""
        return str(self._value).find(str(y)) != -1

    def __init__(self,value):
        self._value = _N.str_(_ver.tostr(value))

    def __str__(self):
        """String: x.__str__() <==> str(x)
        @rtype: String"""
        if len(self._value) > 0:
            return str(self._value)
        else:
            return ''
    def __len__(self):
        return len(str(self))
    def decompile(self):
        return repr(str(self))

class Int128(Scalar):
    """128-bit number"""
    dtype_id=26
    def __init__(self):
        raise TypeError("Int128 is not yet supported")

class Uint128(Scalar):
    """128-bit unsigned number"""
    dtype_id=25
    def __init__(self):
        raise TypeError("Uint128 is not yet supported")

descriptor.dtypeToClass[Uint8.dtype_id]=Uint8
descriptor.dtypeToClass[Uint16.dtype_id]=Uint16
descriptor.dtypeToClass[Uint32.dtype_id]=Uint32
descriptor.dtypeToClass[Uint64.dtype_id]=Uint64
descriptor.dtypeToClass[Uint128.dtype_id]=Uint128
descriptor.dtypeToClass[Int8.dtype_id]=Int8
descriptor.dtypeToClass[Int16.dtype_id]=Int16
descriptor.dtypeToClass[Int32.dtype_id]=Int32
descriptor.dtypeToClass[Int64.dtype_id]=Int64
descriptor.dtypeToClass[Int128.dtype_id]=Int128
descriptor.dtypeToClass[Float32.dtype_id]=Float32
descriptor.dtypeToClass[Float64.dtype_id]=Float64
descriptor.dtypeToClass[Complex64.dtype_id]=Complex64
descriptor.dtypeToClass[Complex128.dtype_id]=Complex128
descriptor.dtypeToClass[String.dtype_id]=String

_compound=_mimport('compound')
