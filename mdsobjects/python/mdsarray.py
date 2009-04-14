import numpy,copy
from types import NotImplementedType
from mdsdata import Data,makeData
from _mdsdtypes import *
from mdsscalar import Scalar

def makeArray(value):
    if isinstance(value,Array):
        return value
    if isinstance(value,Scalar):
        return makeArray((value._value,))
    if isinstance(value,tuple) | isinstance(value,list):
        return makeArray(numpy.array(value))
    if isinstance(value,numpy.ndarray):
        if str(value.dtype)[0:2] == '|S':
            return StringArray(value)
        if str(value.dtype) == 'bool':
            return makeArray(value.__array__(numpy.uint8))
        if str(value.dtype) == 'object':
            raise TypeError,'cannot make Array out of an numpy.ndarray of dtype object'
        exec 'ans='+str(value.dtype).capitalize()+'Array(value)'
        return ans
    if isinstance(value,numpy.generic) | isinstance(value,int) | isinstance(value,long) | isinstance(value,float) | isinstance(value,str) | isinstance(value,bool):
        return makeArray(numpy.array(value).reshape(1))
    raise TypeError,'Cannot make Array out of '+str(type(value))
                        

class Array(Data):
    def __init__(self,value=0):
        if self.__class__.__name__ == 'Array':
            raise TypeError,"cannot create 'Array' instances"
        if self.__class__.__name__ == 'StringArray':
            self._value=numpy.array(value).__array__(numpy.string_)
            return
        exec 'self._value=numpy.array(value).__array__(numpy.'+self.__class__.__name__[0:len(self.__class__.__name__)-5].lower()+')'
        return


#    def __getitem__(self,y):
#        """Subscript: x.__getitem__(y) <==> x[y]
#        @rtype: Data"""
#        return makeData(self.value.__getitem__(y))
    
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

    def getElementAt(self,itm):
        return makeData(self._value[itm])

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
    def deserialize(self):
        """Return data item if this array was returned from serialize.
        @rtype: Data
        """
        return Data.deserialize(self)

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
        raise TypeError,"Int128Array is not yet supported"

class Uint128Array(Array):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError,"Uint128Array is not yet supported"

