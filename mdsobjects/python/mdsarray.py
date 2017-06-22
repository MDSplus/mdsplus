def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N
import ctypes as _C

_data=_mimport('mdsdata')
_dtypes=_mimport('_mdsdtypes')
_scalar=_mimport('mdsscalar')
_ver=_mimport('version')

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

    def _getValue(self):
        """Return the numpy ndarray representation of the array"""
        return self._value
    value=property(_getValue)

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

    def _getMdsDtypeNum(self):
        return {'Uint8Array':_dtypes.DTYPE_BU,'Uint16Array':_dtypes.DTYPE_WU,'Uint32Array':_dtypes.DTYPE_LU,'Uint64Array':_dtypes.DTYPE_QU,
                'Int8Array':_dtypes.DTYPE_B,'Int16Array':_dtypes.DTYPE_W,'Int32Array':_dtypes.DTYPE_L,'Int64Array':_dtypes.DTYPE_Q,
                'StringArray':_dtypes.DTYPE_T,'Float32Array':_dtypes.DTYPE_FS,'Float64Array':_dtypes.DTYPE_FT}[self.__class__.__name__]
    mdsdtype=property(_getMdsDtypeNum)

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

class Int8Array(Array):
    """8-bit signed number"""
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _data.Data.deserialize(self)

class Int16Array(Array):
    """16-bit signed number"""

class Int32Array(Array):
    """32-bit signed number"""

class Int64Array(Array):
    """64-bit signed number"""

class Uint8Array(Array):
    """8-bit unsigned number"""
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return _data.Data.deserialize(self)

class Uint16Array(Array):
    """16-bit unsigned number"""

class Uint32Array(Array):
    """32-bit unsigned number"""

class Uint64Array(Array):
    """64-bit unsigned number"""

class Float32Array(Array):
    """32-bit floating point number"""

class Complex64Array(Array):
    """32-bit complex number"""

class Float64Array(Array):
    """64-bit floating point number"""

class Complex128Array(Array):
    """64-bit complex number"""

class StringArray(Array):
    """String"""
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
    def __init__(self):
        raise TypeError("Int128Array is not yet supported")

class Uint128Array(Array):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError("Uint128Array is not yet supported")

