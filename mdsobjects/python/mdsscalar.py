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

_dat=_mimport('mdsdata')
_arr=_mimport('mdsarray')
_ver=_mimport('version')
_dsc=_mimport('descriptor')
_exc=_mimport('mdsExceptions')

class Scalar(_dat.Data):
    _value = None
    def _setTree(self,*a,**kw): return self;

    def __new__(cls,*value):
        if cls is not Scalar or len(value)==0:
            return object.__new__(cls)
        value = value[0]
        if isinstance(value,(Scalar,)):
            return value
        if isinstance(value,_dat.Data):
            value = value.data()
        if (isinstance(value,_arr.Array)) or isinstance(value,list) or isinstance(value, _ver.nparray):
            key = cls.__name__+'Array'
            if key in _arr.__dict__:
                cls = _arr.__dict__[key]
                return cls.__new__(cls,value)
        if isinstance(value,(_ver.npbytes, _ver.npunicode,_ver.basestring)):
            cls = String
        elif isinstance(value,(_N.bool_,)):
            cls = Uint8
        elif isinstance(value,(_N.generic,)):
            cls = globals()[value.__class__.__name__.capitalize()]
        elif isinstance(value,(_C.c_double)):
            cls = Float64
        elif isinstance(value,(_C.c_float,float)):
            cls = Float32
        elif isinstance(value,(_C.c_int64,_ver.long)):
            cls = Int64
        elif isinstance(value,(_C.c_uint64,)):
            cls = Uint64
        elif isinstance(value,(_C.c_int32,int)):
            cls = Int32
        elif isinstance(value,(_C.c_uint32,)):
            cls = Uint32
        elif isinstance(value,(_C.c_int16,)):
            cls = Int16
        elif isinstance(value,(_C.c_uint16,)):
            cls = Uint16
        elif isinstance(value,(_C.c_int8,)):
            cls = Int8
        elif isinstance(value,(_C.c_uint8,_C.c_bool,bool,_N.bool_)):
            cls = Uint8
        elif isinstance(value,_N.complex64):
            cls = Complex64
        elif isinstance(value,(_N.complex128,complex)):
            cls = Complex128
        else:
            raise TypeError('Cannot make Scalar out of '+str(type(value)))
        return cls.__new__(cls,value)

    def __init__(self,value=0):
        if value is self: return
        if self is Scalar:
            raise TypeError("cannot create 'Scalar' instances")
        if isinstance(value,self.__class__):
            self._value = value._value.copy()
            return
        if isinstance(value,_dat.Data):
            value = value.data()
        elif isinstance(value,_C._SimpleCData):
            value = value.value
        if _ver.ispy3 and self._ntype is _N.bytes_:
            self._value = self._ntype(_ver.tobytes(value))
        else:   
            self._value = self._ntype(value)

    def _str_bad_ref(self):
        return _ver.tostr(self._value)

    @property
    def mdsdtype(self):
        return self.dtype_id

    @property
    def _descriptor(self):
        d=_dsc.Descriptor_s()
        d.length=self._value.nbytes
        d.dtype=self.dtype_id
        array=_N.array(self._value)
        d.pointer=_C.c_void_p(array.ctypes.data)
        d.array=array
        return _cmp.Compound._descriptorWithProps(self,d)

    def __getattr__(self,name):
        if name.startswith("__array"):
            raise AttributeError
        try: return super(Scalar,self).__getattr__(name)
        except AttributeError: pass
        return self.value.__getattribute__(name)

    @property
    def value(self):
        """Return the numpy scalar representation of the scalar"""
        return self._value

    def decompile(self):
        formats={ Int8:'%dB' , Int16:'%dW' , Int32:'%d'  , Int64:'%dQ',
                 Uint8:'%uBU',Uint16:'%uWU',Uint32:'%uLU',Uint64:'%uQU'}
        if self.__class__ in formats:
            return formats[self.__class__] % (self._value,)
        return super(Scalar,self).decompile()

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return int(self.value)

    def __long__(self):
        """Long: x.__long__() <==> long(x)
        @rtype: int"""
        return _ver.long(self.value)

    def _unop(self,op):
        return _dat.Data(getattr(self.value,op)())

    def _binop(self,op,y):
        try:
            y=y.value
        except AttributeError:
            pass
        ans=getattr(self.value,op)(y)
        return _dat.Data(ans)

    def _triop(self,op,y,z):
        try:
            y=y.value
        except AttributeError:
            pass
        try:
            z=z.value
        except AttributeError:
            pass
        return _dat.Data(getattr(self.value,op)(y,z))

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
        return _dat.Data(self.value.argsort(axis,kind,order))

    def astype(self,type):
        return _dat.Data(self.value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)

    @classmethod
    def fromDescriptor(cls,d):
        if d.dtype == FloatF.dtype_id:
            return _cmp.FS_FLOAT(d).evaluate()
        if d.dtype == FloatD.dtype_id:
            return _cmp.FT_FLOAT(d).evaluate()
        if d.dtype == FloatG.dtype_id:
            return _cmp.FT_FLOAT(d).evaluate()
        if d.dtype == ComplexF.dtype_id:
            return _cmp.FS_COMPLEX(d).evaluate()
        if d.dtype == ComplexD.dtype_id:
            return _cmp.FT_COMPLEX(d).evaluate()
        if d.dtype == ComplexG.dtype_id:
            return _cmp.FT_COMPLEX(d).evaluate()
        value=_C.cast(d.pointer,_C.POINTER(cls._ctype)).contents
        if isinstance(value,_C.Array):
            return cls(complex(value[0],value[1]))
        else:
            return cls(value.value)

makeScalar=Scalar

class Float32(Scalar):
    """32-bit floating point number"""
    dtype_id=52
    _ctype=_C.c_float
    _ntype=_N.float32
_dsc.addDtypeToClass(Float32)

class Float64(Scalar):
    """64-bit floating point number"""
    dtype_id=53
    _ctype=_C.c_double
    _ntype=_N.float64
_dsc.addDtypeToClass(Float64)

class Complex64(Scalar):
    """32-bit complex number"""
    dtype_id=54
    _ctype=_C.c_float*2
    _ntype=_N.complex64
_dsc.addDtypeToClass(Complex64)

class Complex128(Scalar):
    """128-bit complex number"""
    dtype_id=55
    _ctype=_C.c_double*2
    _ntype=_N.complex128
_dsc.addDtypeToClass(Complex128)

class Uint8(Scalar):
    """8-bit unsigned number"""
    dtype_id=2
    _ctype=_C.c_uint8
    _ntype=_N.uint8
_dsc.addDtypeToClass(Uint8)

class Uint16(Scalar):
    """16-bit unsigned number"""
    dtype_id=3
    _ctype=_C.c_uint16
    _ntype=_N.uint16
_dsc.addDtypeToClass(Uint16)

class Uint32(Scalar):
    """32-bit unsigned number"""
    dtype_id=4
    _ctype=_C.c_uint32
    _ntype=_N.uint32
_dsc.addDtypeToClass(Uint32)

class Uint64(Scalar):
    """64-bit unsigned number"""
    dtype_id=5
    _ctype=_C.c_uint64
    _ntype=_N.uint64
    _utc0 = _N.uint64("35067168000000000")
    _utc1 = 1E7
    @classmethod
    def fromTime(cls,value):
        """converts from seconds since 01-JAN-1970 00:00:00.00
        For example:
        >>> import MDSplus
        >>> import time
        >>> mdstime=MDSplus.Uint64.fromTime(time.time()-time.altzone)
        >>> print(mdstime.date)
        """
        return cls(int(value * cls._utc1) + cls._utc0)

    def _getDate(self):
        return _dat.Data.execute('date_time($)',self)
    date=property(_getDate)

    def _getTime(self):
        """returns date in seconds since 01-JAN-1970 00:00:00.00"""
        return float(self.value - Uint64._utc0) / Uint64._utc1
    time=property(_getTime)
_dsc.addDtypeToClass(Uint64)

class Int8(Uint8):
    """8-bit signed number"""
    dtype_id=6
    _ctype=_C.c_int8
    _ntype=_N.int8
_dsc.addDtypeToClass(Int8)

class Int16(Uint16):
    """16-bit signed number"""
    dtype_id=7
    _ctype=_C.c_int16
    _ntype=_N.int16
_dsc.addDtypeToClass(Int16)

class Int32(Uint32):
    """32-bit signed number"""
    dtype_id=8
    _ctype=_C.c_int32
    _ntype=_N.int32
_dsc.addDtypeToClass(Int32)

class Int64(Uint64):
    """64-bit signed number"""
    dtype_id=9
    _ctype=_C.c_int64
    _ntype=_N.int64
_dsc.addDtypeToClass(Int64)

class FloatF(Float32):
    """32-bit VMS floating point number"""
    dtype_id=10
_dsc.addDtypeToClass(FloatF)

class FloatD(Float64):
    """64-bit VMS floating point number"""
    dtype_id=11
_dsc.addDtypeToClass(FloatD)

class ComplexF(Complex64):
    """128-bit VMS complex number"""
    dtype_id=12
_dsc.addDtypeToClass(ComplexF)

class ComplexD(Complex128):
    """128-bit VMS complex number"""
    dtype_id=13
_dsc.addDtypeToClass(ComplexD)

class String(Scalar):
    """String"""
    dtype_id=14
    _ntype=_ver.npbytes
    def __init__(self,value):
        super(String,self).__init__(value)
        if not isinstance(self._value,_N.str):
            self._value = _ver.npstr(_ver.tostr(self._value))
    @property
    def _descriptor(self):
        d=_dsc.Descriptor_s()
        d.length=len(self)
        d.dtype=self.dtype_id
        d.pointer=_C.cast(_C.c_char_p(_ver.tobytes(str(self))),_C.c_void_p)
        return _cmp.Compound._descriptorWithProps(self,d)

    @classmethod
    def fromDescriptor(cls,d):
        if d.length == 0:
            return cls('')
        return cls(_N.array(_C.cast(d.pointer,_C.POINTER((_C.c_byte*d.length))).contents[:],dtype=_N.uint8).tostring())

    def __radd__(self,y):
        """radd: x.__radd__(y) <==> y+x
        @rtype: Data"""
        return self.execute('$//$',y,self)
    def __add__(self,y):
        """Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return self.execute('$//$',self,y)
    def __contains__(self,y):
        """Contains: x.__contains__(y) <==> y in x
        @rtype: Bool"""
        return self.find(str(y)) != -1
    def __len__(self):
        return len(self._value)
    def __str__(self):
        return _ver.tostr(self._value)
    def __repr__(self):
        return repr(_ver.tostr(self._value))

_dsc.addDtypeToClass(String)

class Uint128(Scalar):
    """128-bit unsigned number"""
    dtype_id=25
    def __init__(self):
        raise TypeError("Uint128 is not yet supported")
_dsc.addDtypeToClass(Uint128)

class Int128(Uint128):
    """128-bit number"""
    dtype_id=26
    def __init__(self):
        raise TypeError("Int128 is not yet supported")
_dsc.addDtypeToClass(Int128)

class FloatG(Float64):
    """64-bit VMS floating point number"""
    dtype_id=27
_dsc.addDtypeToClass(FloatG)

class ComplexG(Complex128):
    """128-bit VMS complex number"""
    dtype_id=29
_dsc.addDtypeToClass(ComplexG)

class Pointer(Scalar):
    """32/64bit pointer"""
    dtype_id=51
    def __init__(self, value=0, is64=True):
        if value is self: return
        if is64:
            self._ctype=_C.c_uint64
            self._ntype=_N.uint64
        else:
            self._ctype=_C.c_uint32
            self._ntype=_N.uint32
        super(Pointer,self).__init__(value)

    @classmethod
    def fromDescriptor(cls,d):
        is64 = d.length>4
        ctype = _C.c_uint64 if is64 else _C.c_uint32
        value=_C.cast(d.pointer,_C.POINTER(ctype)).contents
        return cls(value.value,is64)
_dsc.addDtypeToClass(Pointer)

class Ident(_dat.Data):
    """Reference to MDSplus Ken Variable"""
    dtype_id=191
    def __init__(self,name):
        if isinstance(name,Ident):
            return
        if not name.startswith('_'):
            raise _exc.TdiUNKNOWN_VAR
        self.name=_ver.tostr(name)
    def decompile(self):
        return self.name
    def assign(self,value):
        _cmp.EQUALS(self,value).evaluate()
        return self
    @property
    def _descriptor(self):
        d=_dsc.Descriptor_s()
        d.dtype=self.dtype_id
        d.length=len(self.name)
        d.pointer=_C.cast(_C.c_char_p(_ver.tobytes(self.name)),_C.c_void_p)
        return _cmp.Compound._descriptorWithProps(self,d)
    @classmethod
    def fromDescriptor(cls,d):
        return cls(_ver.tostr(_C.cast(d.pointer,_C.POINTER(_C.c_char*d.length)).contents.value))
_dsc.addDtypeToClass(Ident)

_cmp=_mimport('compound')
