import numpy,copy
from types import NotImplementedType
from MDSobjects.data import *
from MDSobjects._mdsdtypes import *

def makeScalar(value):
    if isinstance(value,Scalar):
        return copy.deepcopy(value)
    if isinstance(value,numpy.generic):
        if isinstance(value,numpy.string_):
            return String(value)
        if isinstance(value,numpy.bool_):
            return makeScalar(int(value))
        else:
            exec 'ans='+value.__class__.__name__.capitalize()+'(value)'
            return ans
    if isinstance(value,int):
        return Int32(value)
    if isinstance(value,long):
        return Int64(value)
    if isinstance(value,float):
        return Float32(value)
    if isinstance(value,str):
        return String(value)
    if isinstance(value,bool):
        return Int8(int(value))
    raise TypeError,'Cannot make MDSobjects.Scalar out of '+str(type(value))

class Scalar(Data):
    def __init__(self,value=0):
        if self.__class__.__name__ == 'Scalar':
            raise TypeError,"cannot create 'MDSobjects.Scalar' instances"
        if self.__class__.__name__ == 'String':
            self._value=numpy.string_(value)
            return
        exec 'self._value=numpy.'+self.__class__.__name__.lower()+'(value)'

    def __getattr__(self,name):
        exec 'ans=self.value.'+name
        return ans

    def _getValue(self):
        """Return the numpy scalar representation of the scalar"""
        return self._value

    value=property(_getValue)

    def _unop(self,op):
        return makeData(getattr(self.value,op)())

    def _binop(self,op,y):
        try:
            y=y.value
        except (AttributeError),e:
            pass
        ans=getattr(self.value,op)(y)
        if isinstance(ans,NotImplementedType):
            raise AttributeError,op+' is not supported for types '+str(type(self.value))+' and '+str(type(y))
        return makeData(ans)

    def _triop(self,op,y,z):
        try:
            y=y.value
        except (AttributeError),e:
            pass
        try:
            z=z.value
        except (AttributeError),e:
            pass
        return makeData(getattr(self.value,op)(y,z))

    def _getMdsDtypeNum(self):
        return {'Uint8':DTYPE_BU,'Uint16':DTYPE_WU,'Uint32':DTYPE_LU,'Uint64':DTYPE_QU,
                'Int8':DTYPE_B,'Int16':DTYPE_W,'Int32':DTYPE_L,'Int64':DTYPE_Q,
                'String':DTYPE_T,
                'Float32':DTYPE_FS,
                'Float64':DTYPE_FT}[self.__class__.__name__]
    mdsdtype=property(_getMdsDtypeNum)
    
    def __array__(self):
        raise TypeError,'__array__ not yet supported'

    def __copy__(self):
        return type(self)(self._value)

    def __deepcopy__(self,memo=None):
        return self.__copy__()

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

    def __nonzero__(self):
        return self.value.__nonzero__()

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

    def __str__(self):
        return str(self._value)
    
    def __sub__(self,y):
        return self._binop('__sub__',y)

    def __truediv__(self,y):
        return self._binop('__truediv__',y)

    def __xor__(self,y):
        return self._binop('__xor__',y)
    
    def __eq__(self,y):
        if y is None:
            return makeData(self.value is None)
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
        return makeData(self.value.argsort(axis,kind,order))

    def astype(self,type):
        return makeData(self.value.astype(type))

    def byteswap(self):
        return self._unop('byteswap')

    def clip(self,y,z):
        return self._triop('clip',y,z)

    def __nonzero__(self):
        return self.value.__nonzero__()


class Int8(Scalar):
    """8-bit signed number"""

class Int16(Scalar):
    """16-bit signed number"""

class Int32(Scalar):
    """32-bit signed number"""

class Int64(Scalar):
    """64-bit signed number"""

class Uint8(Scalar):
    """8-bit unsigned number"""

class Uint16(Scalar):
    """16-bit unsigned number"""

class Uint32(Scalar):
    """32-bit unsigned number"""

class Uint64(Scalar):
    """64-bit unsigned number"""

    def _getDate(self):
        return Data.execute('date_time($)',self)
    date=property(_getDate)

class Float32(Scalar):
    """32-bit floating point number"""

class Float64(Scalar):
    """64-bit floating point number"""
    
class String(Scalar):
    """String"""
    def __radd__(self,y):
        return String(str(y)+str(self))

class Int128(Scalar):
    """128-bit number"""
    def __init__(self):
        raise TypeError,"Int128 is not yet supported"

class Uint128(Scalar):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError,"Uint128 is not yet supported"
