import numpy,copy
from types import NotImplementedType
from MDSobjects.data import Data,makeData
from MDSobjects._mdsdtypes import *
from MDSobjects.scalar import Scalar

def makeArray(value):
    if isinstance(value,Array):
        return copy.deepcopy(value)
    if isinstance(value,Scalar):
        return makeArray((value._value,))
    if isinstance(value,tuple) | isinstance(value,list):
        return makeArray(numpy.array(value))
    if isinstance(value,numpy.ndarray):
        if isinstance(value[0],numpy.string_):
            return StringArray(value)
        if isinstance(value[0],numpy.bool_):
            return makeArray(value.__array__(numpy.int8))
        exec 'ans='+value.reshape(value.size)[0].__class__.__name__.capitalize()+'Array(value)'
        return ans
    if isinstance(value,numpy.generic) | isinstance(value,int) | isinstance(value,long) | isinstance(value,float) | isinstance(value,str) | isinstance(value,bool):
        return makeArray(numpy.array(value).reshape(1))
    raise TypeError,'Cannot make MDSobjects.Array out of '+str(type(value))
                        

class Array(Data):
    def __init__(self,value=0):
        if self.__class__.__name__ == 'Array':
            raise TypeError,"cannot create 'MDSobjects.Array' instances"
        if self.__class__.__name__ == 'StringArray':
            self._value=numpy.array(value).__array__(numpy.string_)
            return
        exec 'self._value=numpy.array(value).__array__(numpy.'+self.__class__.__name__[0:len(self.__class__.__name__)-5].lower()+')'
        return

    def __getattr__(self,name):
        exec 'ans=self._value.'+name
        return ans

    def _getValue(self):
        """Return the numpy ndarray representation of the array"""
        return self._value

    value=property(_getValue)

    
    def _unop(self,op):
        return makeData(getattr(self._value,op)())

    def _binop(self,op,y):
        try:
            y=y._value
        except (AttributeError),e:
            pass
        return makeData(getattr(self._value,op)(y))

    def _triop(self,op,y,z):
        try:
            y=y._value
        except (AttributeError),e:
            pass
        try:
            z=z._value
        except (AttributeError),e:
            pass
        return makeData(getattr(self._value,op)(y,z))

    def _getMdsDtypeNum(self):
        return {'Uint8Array':DTYPE_BU,'Uint16Array':DTYPE_WU,'Uint32Array':DTYPE_LU,'Uint64Array':DTYPE_QU,
                'Int8Array':DTYPE_B,'Int16Array':DTYPE_W,'Int32Array':DTYPE_L,'Int64Array':DTYPE_Q,
                'StringArray':DTYPE_T,
                'Float32Array':DTYPE_FS,
                'Float64Array':DTYPE_FT}[self.__class__.__name__]

    mdsdtype=property(_getMdsDtypeNum)
    
    def __array__(self):
        raise TypeError,'__array__ not yet supported'

    def __copy__(self):
        return type(self)(self._value)

    def __deepcopy__(self,memo=None):
        return self.__copy__()

    def __getitem__(self,itm):
        return self._binop('__getitem__',itm)
    getElementAt=__getitem__

    def __setitem__(self,i,y):
        return self._triop('__setitem__',i,y)
    setElementAt=__setitem__
    
    def __hex__(self):
        return self._unop('__hex__')

    def __invert__(self):
        return self._unop('__invert__')

    def __lshift__(self,y):
        return self._binop('__lshift__',y)

    def __mod__(self,y):
        return self._binop('__mod__',y)

    def __mul__(self,y):
        return self._binop('__mul__',y)

    def __neg__(self):
        return self._unop('__neg__')

    def __nonzero__(self,y):
        return self._unop('__nonzero__')

    def __oct__(self):
        return self._unop('__oct__')

    def __or__(self,y):
        return self._binop('__or__')

    def __pos__(self):
        return self._unop('__pos__')

    def __pow__(self,y,*z):
        if z:
            return self._triop('__pow__',y,z[0])
        else:
            return self._binop('__pow__',y)

    def __rand__(self,y):
        return self._binop('__rand__',y)

    def __rdiv__(self,y):
        return self._binop('__rdiv__',y)

    def __rdivmod(self,y):
        return self._binop('__rdivmod__',y)

    def __reduce__(self):
        return self._unop('__reduce__')

    def __repr__(self):
        return self._value.__repr__()

    def __rfloordiv__(self,y):
        return self._binop('__rfloordiv__',y)

    def __rlshift__(self,y):
        return self._binop('__rlshift__',y)

    def __rmod__(self,y):
        return self._binop('__rmod__',y)

    def __rmul__(self,y):
        return self._binop('__rmul__',y)

    def __ror__(self,y):
        return self._binop('__ror__',y)

    def __rpow__(self,y,*z):
        if z:
            return self._triop('__rppow__',y,z[0])
        else:
            return self._binop('__rpow__',y)

    def __rrshift__(self,y):
        return self._binop('__rrshift__',y)

    def __rshift__(self,y):
        return self._binop('__rshift__',y)

    def __rsub__(self,y):
        return self._binop('__rsub__',y)

    def __rtruediv(self,y):
        return self._binop('__rtruediv__',y)

    def __rxor__(self,y):
        return self._binop('__rxor__',y)

    def __setstate__(self):
        return self._unop('__setstate__')

    def __sub__(self,y):
        return self._binop('__sub__',y)

    def __truediv__(self,y):
        return self._binop('__truediv__',y)

    def __xor__(self,y):
        return self._binop('__xor__',y)
    
    def __eq__(self,y):
        return self._binop('__eq__',y)

    def __ge__(self,y):
        return self._binop('__ge__',y)

    def __gt__(self,y):
        return self._binop('__gt__',y)

    def __le__(self,y):
        return self._binop('__le__',y)

    def __lt__(self,y):
        return self._binop('__lt__',y)

    def __ne__(self,y):
        return self._binop('__ne__',y)

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
        return makeData(self._value.argsort(axis,kind,order))

    def astype(self,type):
        return makeData(self._value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)


class Int8Array(Array):
    """8-bit signed number"""

class Int16Array(Array):
    """16-bit signed number"""

class Int32Array(Array):
    """32-bit signed number"""

class Int64Array(Array):
    """64-bit signed number"""

class Uint8Array(Array):
    """8-bit unsigned number"""

class Uint16Array(Array):
    """16-bit unsigned number"""

class Uint32Array(Array):
    """32-bit unsigned number"""

class Uint64Array(Array):
    """64-bit unsigned number"""

class Float32Array(Array):
    """32-bit floating point number"""

class Float64Array(Array):
    """64-bit floating point number"""

class StringArray(Array):
    """String"""

class Int128Array(Array):
    """128-bit signed number"""
    def __init__(self):
        raise TypeError,"Int128Array is not yet supported"

class Uint128Array(Array):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError,"Uint128Array is not yet supported"

