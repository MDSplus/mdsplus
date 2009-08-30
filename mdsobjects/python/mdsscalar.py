import numpy,copy
from types import NotImplementedType
from mdsdata import *
from _mdsdtypes import *

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
    raise TypeError,'Cannot make Scalar out of '+str(type(value))

class Scalar(Data):
   
    def __new__(cls,value=0):
        try:
            import numpy
            from mdsarray import Array
            if (isinstance(value,Array)) or isinstance(value,list) or isinstance(value,numpy.ndarray):
                exec "from mdsarray import "+cls.__name__+"Array"
                return eval(cls.__name__+"Array(value)")
        except:
            pass

        return super(Scalar,cls).__new__(cls)
        
    def __init__(self,value=0):
        if self.__class__.__name__ == 'Scalar':
            raise TypeError,"cannot create 'Scalar' instances"
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

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return self._value.__int__()

    def __long__(self):
        """Long: x.__long__() <==> long(x)
        @rtype: int"""
        return self.__value.__long__()

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
        return self.value

class Int128(Scalar):
    """128-bit number"""
    def __init__(self):
        raise TypeError,"Int128 is not yet supported"

class Uint128(Scalar):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError,"Uint128 is not yet supported"
