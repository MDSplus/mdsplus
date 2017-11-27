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

_ver=_mimport('version')
_dsc=_mimport('descriptor')
_dat=_mimport('mdsdata')
_scr=_mimport('mdsscalar')
_cmd=_mimport('compound')

class Array(_dat.Data):
    ctype = None
    __MAX_DIM = 8
    def _setTree(self,*a,**kw): return self;
    @property  # used by numpy.array
    def __array_interface__(self):
        data = self.value
        return {
            'shape':data.shape,
            'typestr':data.dtype.str,
            'descr':data.dtype.descr,
            'strides':data.strides,
            'data':data,
            'version':3,
        }

    def __new__(cls,*value):
        """Convert a python object to a MDSobject Data array
        @param value: Any value
        @type data: any
        @rtype: Array
        """
        if cls is not Array or len(value)==0:
            return object.__new__(cls)
        value=value[0]
        if isinstance(value,(Array,)):
            return value
        if isinstance(value,_dat.Data):
            value = _N.array(value.data())
        elif isinstance(value,_C.Array):
            try: value = _N.ctypeslib.as_array(value)
            except: pass
        elif isinstance(value,(int,_ver.long)):
            value = _N.array(value)
        elif isinstance(value,(tuple,list)):
            if len(value)==0:
                return Int32Array.__new__(Int32Array,[])
            try:
                value = _N.array(value)
            except (ValueError,TypeError):
                newlist=list()
                for i in value:
                    newlist.append(_dat.Data(i).data())
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
        if isinstance(value,self.__class__):
            self._value = value._value.copy()
            return
        if isinstance(value,_dat.Data):
            value = value.data()
        elif isinstance(value,_C.Array):
            try:   value=_N.ctypeslib.as_array(value)
            except Exception: pass
        else:
            value = _N.array(value)
        if len(value.shape) == 0 or len(value.shape) > Array.__MAX_DIM:  # happens if value has been a scalar, e.g. int
            value = value.flatten()
        self._value = value.__array__(self.ntype).copy('C')

    def _str_bad_ref(self):
        return _ver.tostr(self._value)

    def __getattr__(self,name):
        try: return super(Array,self).__getattribute__(name)
        except AttributeError: pass
        if name=='_value': raise Exception('_value undefined')
        try: return self._value.__getattribute__(name)
        except AttributeError:  pass
        raise AttributeError

    @property
    def value(self):
        """Return the numpy ndarray representation of the array"""
        return self._value

    def _unop(self,op):
        return _dat.Data(getattr(self._value,op)())

    def _binop(self,op,y):
        if hasattr(y,'_value'): y=y._value
        return _dat.Data(getattr(self._value,op)(y))

    def _triop(self,op,y,z):
        if hasattr(y,'_value'): y=y._value
        if hasattr(z,'_value'): z=z._value
        return _dat.Data(getattr(self._value,op)(y,z))

    def __array__(self):
        return self.value

    def __copy__(self):
        return self.__class__(self._value)

    def __deepcopy__(self,memo=None):
        return self.__copy__()

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return len(self._value)

    def __setitem__(self,index,value):
        self._value[index]=value

    def __getitem__(self,index):
        return _dat.Data(self._value[index])

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
        return _dat.Data(self._value.argsort(axis,kind,order))

    def astype(self,type):
        return _dat.Data(self._value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)

    @property
    def _descriptor(self):
        if not self._value.flags.c_contiguous:
            self._value=self._value.copy('C')
        value = self._value.T
        d=_dsc.Descriptor_a()
        d.scale=0
        d.digits=0
        d.dtype=self.dtype_id
        d.length=value.itemsize
        d._value = value
        d.pointer=_C.c_void_p(value.__array_interface__["data"][0])
        d.dimct=value.ndim
        d.aflags=48
        d.arsize=value.nbytes
        d.a0=d.pointer
        if d.dimct > 1:
            d.coeff=True
            for i in range(d.dimct):
                d.coeff_and_bounds[i]=_N.shape(value)[i]
        return _cmd.Compound._descriptorWithProps(self,d)


    @classmethod
    def fromDescriptor(cls,d):
        def getNumpy(ntype,ctype,length):
            buffer = _ver.buffer(_C.cast(d.pointer,_C.POINTER(ctype*length)).contents)
            return _N.ndarray(shape,ntype,buffer)
        if d.dtype == 0:
            d.dtype = Int32Array.dtype_id
        if d.coeff:
            d.arsize=d.length
            shape=list()
            for i in range(d.dimct):
                dim=d.coeff_and_bounds[d.dimct-i-1]
                d.arsize=d.arsize*dim
                shape.append(dim)
        else:
            shape=[int(d.arsize/d.length),]
        if d.dtype == StringArray.dtype_id:
            if d.length == 0:
                strarr=''
                for dim in shape:
                    if dim > 0:
                        strarr=[strarr]*dim
                ans = StringArray(_N.array(strarr))
            else:
                ans = StringArray(getNumpy(_N.dtype(('S',d.length)),_C.c_byte,d.arsize))
            return ans
        if d.dtype == _tre.TreeNode.dtype_id:
            d.dtype=Int32Array.dtype_id
            nids=getNumpy(_N.int32,_C.c_int32,int(d.arsize/d.length))
            return _tre.TreeNodeArray(list(nids))
        if d.dtype == Complex64Array.dtype_id:
            return Array(getNumpy(_N.complex64,_C.c_float,int(d.arsize*2/d.length)))
        if d.dtype == Complex128Array.dtype_id:
            return Array(getNumpy(_N.complex128,_C.c_double,int(d.arsize*2/d.length)))
        if d.dtype == FloatFArray.dtype_id:
            return _cmp.FS_FLOAT(d).evaluate()
        if d.dtype == FloatDArray.dtype_id:
            return _cmp.FT_FLOAT(d).evaluate()
        if d.dtype == FloatGArray.dtype_id:
            return _cmp.FT_FLOAT(d).evaluate()
        if d.dtype == ComplexFArray.dtype_id:
            return _cmp.FS_COMPLEX(d).evaluate()
        if d.dtype == ComplexDArray.dtype_id:
            return _cmp.FT_COMPLEX(d).evaluate()
        if d.dtype == ComplexGArray.dtype_id:
            return _cmp.FT_COMPLEX(d).evaluate()
        if d.dtype in _dsc.dtypeToArrayClass:
            cls = _dsc.dtypeToArrayClass[d.dtype]
            if cls.ctype is not None:
                return Array(getNumpy(cls.ctype,cls.ctype,int(d.arsize/d.length)))
        raise TypeError('Arrays of dtype %d are unsupported.' % d.dtype)
makeArray = Array

class Float32Array(Array):
    """32-bit floating point number"""
    dtype_id=52
    ctype=_C.c_float
    ntype=_N.float32
_dsc.addDtypeToArrayClass(Float32Array)

class Float64Array(Array):
    """64-bit floating point number"""
    dtype_id=53
    ctype=_C.c_double
    ntype=_N.float64
_dsc.addDtypeToArrayClass(Float64Array)

class Complex64Array(Array):
    """32-bit complex number"""
    dtype_id=54
    ntype=_N.complex64
_dsc.addDtypeToArrayClass(Complex64Array)

class Complex128Array(Array):
    """64-bit complex number"""
    dtype_id=55
    ntype=_N.complex128
_dsc.addDtypeToArrayClass(Complex128Array)

class Uint8Array(Array):
    """8-bit unsigned number"""
    dtype_id=2
    ctype=_C.c_uint8
    ntype=_N.uint8
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _dat.Data.deserialize(self)
_dsc.addDtypeToArrayClass(Uint8Array)

class Uint16Array(Array):
    """16-bit unsigned number"""
    dtype_id=3
    ctype=_C.c_uint16
    ntype=_N.uint16
_dsc.addDtypeToArrayClass(Uint16Array)

class Uint32Array(Array):
    """32-bit unsigned number"""
    dtype_id=4
    ctype=_C.c_uint32
    ntype=_N.uint32
_dsc.addDtypeToArrayClass(Uint32Array)

class Uint64Array(Array):
    """64-bit unsigned number"""
    dtype_id=5
    ctype=_C.c_uint64
    ntype=_N.uint64
_dsc.addDtypeToArrayClass(Uint64Array)

class Int8Array(Array):
    """8-bit signed number"""
    dtype_id=6
    ctype=_C.c_int8
    ntype=_N.int8
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _dat.Data.deserialize(self)
_dsc.addDtypeToArrayClass(Int8Array)

class Int16Array(Array):
    """16-bit signed number"""
    dtype_id=7
    ctype=_C.c_int16
    ntype=_N.int16
_dsc.addDtypeToArrayClass(Int16Array)

class Int32Array(Array):
    """32-bit signed number"""
    dtype_id=8
    ctype=_C.c_int32
    ntype=_N.int32
_dsc.addDtypeToArrayClass(Int32Array)

class Int64Array(Array):
    """64-bit signed number"""
    dtype_id=9
    ctype=_C.c_int64
    ntype=_N.int64
_dsc.addDtypeToArrayClass(Int64Array)

class FloatFArray(Float32Array):
    """32-bit VMS floating point number"""
    dtype_id=10
_dsc.addDtypeToArrayClass(FloatFArray)

class FloatDArray(Float64Array):
    """64-bit VMS floating point number"""
    dtype_id=11
_dsc.addDtypeToArrayClass(FloatDArray)

class ComplexFArray(Complex64Array):
    """64-bit VMS complex number"""
    dtype_id=12
_dsc.addDtypeToArrayClass(ComplexFArray)

class ComplexDArray(Complex128Array):
    """128-bit VMS complex number"""
    dtype_id=13
_dsc.addDtypeToArrayClass(ComplexDArray)

class StringArray(Array):
    """String"""
    dtype_id=14
    def __init__(self,value):
        if value is self: return
        if isinstance(value, (StringArray,)):
            self._value = value._value.copy('C')
            return
        if not isinstance(value,(_N.ndarray,)):
            value = _N.array(value)
        if not value.dtype.type is _ver.npbytes:
            value = _N.array(_ver.tobytes(value.tolist()))
        elif _ver.ispy3 or not value.flags.writeable or not value.flags.c_contiguous:
            value = value.copy('C')
        length = value.itemsize
        if length>0:
            for i in _ver.xrange(len(value.flat)):
                val = value.flat[i]
                if len(val)<length or val[-1] == 0:
                    value.flat[i]=val.ljust(length)
        self._value = value
    @property
    def value(self):
        return self._value
    def __radd__(self,y):
        """Reverse add: x.__radd__(y) <==> y+x
        @rtype: Data"""
        return self.execute('$//$',y,self)
    def __add__(self,y):
        """Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return self.execute('$//$',self,y)
_dsc.addDtypeToArrayClass(StringArray)

class Uint128Array(Array):
    """128-bit unsigned number"""
    dtype_id=25
    def __init__(self):
        raise TypeError("Uint128Array is not yet supported")
_dsc.addDtypeToArrayClass(Uint128Array)

class Int128Array(Array):
    """128-bit signed number"""
    dtype_id=26
    def __init__(self):
        raise TypeError("Int128Array is not yet supported")
_dsc.addDtypeToArrayClass(Int128Array)

class FloatGArray(Float64Array):
    """64-bit VMS floating point number"""
    dtype_id=27
_dsc.addDtypeToArrayClass(FloatGArray)

class ComplexGArray(Complex128Array):
    """128-bit VMS complex number"""
    dtype_id=29
_dsc.addDtypeToArrayClass(ComplexGArray)

_apd=_mimport('apd')
_cmp=_mimport('compound')
_tre=_mimport('tree')
