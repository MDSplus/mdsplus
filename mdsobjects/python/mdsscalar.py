def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N

_dtypes=_mimport('_mdsdtypes')
_data=_mimport('mdsdata')
_array=_mimport('mdsarray')
_ver=_mimport('version')

def makeScalar(value):
    if isinstance(value,_ver.basestring):
        return String(value)
    if isinstance(value,Scalar):
        from copy import deepcopy
        return deepcopy(value)
    if isinstance(value,_N.generic):
        if isinstance(value,(_N.string_, _N.unicode_)):  # includes _N.bytes_
            return String(value)
        if isinstance(value,_N.bool_):
            return makeScalar(int(value))
        return globals()[value.__class__.__name__.capitalize()](value)
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
        if self.__class__.__name__ == 'Scalar':
            raise TypeError("cannot create 'Scalar' instances")
        if self.__class__.__name__ == 'String':
            self._value = _N.str_(_ver.tostr(value))
            return
        self._value = _N.__dict__[self.__class__.__name__.lower()](value)

    def __getattr__(self,name):
        if name.startswith("__array"):
          raise AttributeError
        return self._value.__getattribute__(name)

    def _getValue(self):
        """Return the numpy scalar representation of the scalar"""
        return self._value
    value=property(_getValue)

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

    def _getMdsDtypeNum(self):
        return {'Uint8':_dtypes.DTYPE_BU,'Uint16':_dtypes.DTYPE_WU,'Uint32':_dtypes.DTYPE_LU,'Uint64':_dtypes.DTYPE_QU,
                'Int8':_dtypes.DTYPE_B,'Int16':_dtypes.DTYPE_W,'Int32':_dtypes.DTYPE_L,'Int64':_dtypes.DTYPE_Q,
                'String':_dtypes.DTYPE_T,
                'Float32':_dtypes.DTYPE_FS,
                'Float64':_dtypes.DTYPE_FT,'Complex64':_dtypes.DTYPE_FSC,'Complex128':_dtypes.DTYPE_FTC}[self.__class__.__name__]
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
        return _data.makeData(self.value.argsort(axis,kind,order))

    def astype(self,type):
        return _data.makeData(self.value.astype(type))

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

class Complex64(Scalar):
    """32-bit complex number"""
    def __str__(self):
        return "Cmplx(%g,%g)" % (self._value.real,self._value.imag)

class Float64(Scalar):
    """64-bit floating point number"""
    def __str__(self):
        return ("%E" % self._value).replace("E","D")

class Complex128(Scalar):
    """64-bit complex number"""
    def __str__(self):
        return "Cmplx(%s,%s)" % (str(Float64(self._value.real)),str(Float64(self._value.imag)))

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
    def __contains__(self,y):
        """Contains: x.__contains__(y) <==> y in x
        @rtype: Bool"""
        return str(self._value).find(str(y)) != -1
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
    def __init__(self):
        raise TypeError("Int128 is not yet supported")

class Uint128(Scalar):
    """128-bit unsigned number"""
    def __init__(self):
        raise TypeError("Uint128 is not yet supported")
