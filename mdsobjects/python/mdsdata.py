def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N, ctypes as _C

_ver=_mimport('version')
_Exceptions=_mimport('mdsExceptions')

MDSplusException = _Exceptions.MDSplusException
MdsException = MDSplusException
#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
_TdiShr=_ver.load_library('TdiShr')
#
#############################################

def getUnits(item):
    """Return units of item. Evaluate the units expression if necessary.
    @rtype: string"""
    try:
        return item.units
    except:
        return ""

def getError(item):
    """Return the data of the error of an object
    @rtype: Data"""
    try:
        return item.error
    except:
        return None

def getValuePart(item):
    """Return the value portion of an object
    @rtype: Data"""
    try:
        return _builtins.VALUE_OF(item).evaluate()
    except:
        return None

def getDimension(item,idx=0):
    """Return dimension of an object
    @rtype: Data"""
    try:
        return _builtins.DIM_OF(item,idx).evaluate()
    except:
        return None

def data(item):
    """Return the data for an object converted into a primitive data type
    @rtype: Data"""
    return _builtins.DATA(item).evaluate().value

def decompile(item):
    """Returns the item converted to a string
    @rtype: string"""
    return str(makeData(item).decompile())

def evaluate(item,):
    """Return evaluation of mdsplus object"""
    try:
        return makeData(item).evaluate()
    except:
        return item

def rawPart(item):
    """Return raw portion of data item"""
    try:
        return item.raw
    except:
        return None

def makeData(value):
    """Convert a python object to a MDSobject Data object"""
    _tree=_mimport("tree")
    if value is None:
        return EmptyData()
    if isinstance(value,(Data,_tree.TreeNode)):
        return value
    if isinstance(value,(_N.generic,int,float,complex,_ver.basestring,_ver.long,_C._SimpleCData)):
        return _scalar.makeScalar(value)
    if isinstance(value,(tuple,list)):
        return _apd.List(value)
    if isinstance(value,(_N.ndarray,_C.Array)):
        return _array.makeArray(value)
    if isinstance(value,dict):
        return _apd.Dictionary(value)
    if isinstance(value,slice):
        return _builtins.BUILD_RANGE(value.start,value.stop,value.step).evaluate()

    else:
        raise TypeError('Cannot make MDSplus data type from type: %s' % (str(type(value)),))

class Data(object):
    """Superclass used by most MDSplus objects. This provides default methods if not provided by the subclasses.
    """
    __array_priority__ = 100. ##### Needed to force things like numpy-array * mdsplus-data to use our __rmul__

    def __init__(self,*value):
        """Cannot create instances of class Data objects. Use Data.makeData(initial-value) instead
        @raise TypeError: Raised if attempting to create an instance of Data
        @rtype: Data
        """
        raise TypeError('Cannot create \'Data\' instances')

    def __function(self,name,default):
        found = False
        ans=self.evaluate()
        while(self is not ans and hasattr(ans,name) and callable(ans.__getattribute__(name))):
            found = True
            ans=ans.__getattribute__(name)()
        if not found:
            return default
        return ans

    def value_of(self):
        """Return value part of object
        @rtype: Data"""
        return _builtins.VALUE_OF(self).evaluate()

    def raw_of(self):
        """Return raw part of object
        @rtype: Data"""
        return _builtins.RAW_OF(self).evaluate()
    def units_of(self):
        """Return units of object
        @rtype: Data"""
        return _builtins.UNITS_OF(self).evaluate()

    def getDimensionAt(self,idx=0):
        """Return dimension of object
        @param idx: Index of dimension
        @type idx: int
        @rtype: Data"""
        return _builtins.DIM_OF(self,idx).evaluate()

    dim_of=getDimensionAt

    @property
    def units(self):
        """units associated with this data."""
        return _builtins.UNITS(self).evaluate()
    @units.setter
    def units(self,units):
        if units is None:
            if hasattr(self,'_units'):
                delattr(self,'_units')
        else:
            self._units=units

    @property
    def error(self):
        """error property of this data."""
        return _builtins.ERROR_OF(self).evaluate()
    @error.setter
    def error(self,error):
        if error is None:
            if hasattr(self,'_error'):
                delattr(self,'_error')
        else:
            self._error=error

    @property
    def help(self):
        """help property of this node."""
        return _builtins.HELP_OF(self).evaluate()
    @help.setter
    def help(self,help):
        if help is None:
            if hasattr(self,'_help'):
                delattr(self,'_help')
        else:
            self._help=help

    @property
    def validation(self):
        """Validation property of this node"""
        return _builtins.VALIDATION_OF(self).evaluate()
    @validation.setter
    def validation(self,validation):
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation

    def __abs__(self):
        """
        Absolute value: x.__abs__() <==> abs(x)
        @rtype: Data
        """
        return _builtins.ABS(self).evaluate()

    def bool(self):
        """
        Return boolean
        @rtype: Bool
        """
        if isinstance(self,_array.Array):
            return self._value!=0
        elif isinstance(self,_compound.Compound) and hasattr(self,'value_of'):
            return self.value_of().bool()
        else:
            ans=int(self)
            return (ans & 1) == 1
    __bool__=bool

    def __add__(self,y):
        """
        Add: x.__add__(y) <==> x+y
        @rtype: Data"""
        return _builtins.ADD(self,y).evaluate()

    def __and__(self,y):
        """And: x.__and__(y) <==> x&y
        @rtype: Data"""
        return _builtins.IAND(self,y).evaluate()

    def __div__(self,y):
        """Divide: x.__div__(y) <==> x/y
        @rtype: Data"""
        return _builtins.DIVIDE(self,y).evaluate()

    __truediv__=__div__

    def __eq__(self,y):
        """Equals: x.__eq__(y) <==> x==y
        @rtype: Bool"""
        try:
            return _builtins.EQ(self,y).evaluate().bool()
        except:
            return False

    def __hasBadTreeReferences__(self,tree):
        return False

    def __fixTreeReferences__(self,tree):
        return self

    def __float__(self):
        """Float: x.__float__() <==> float(x)
        @rtype: Data"""
        ans=_builtins.FLOAT(self).evaluate().value
        try:
            return float(ans)
        except:
            return float(ans[0])

    def __floordiv__(self,y):
        """Floordiv: x.__floordiv__(y) <==> x//y
        @rtype: Data"""
        return _builtins.FLOOR(self,y).evaluate()

    def __ge__(self,y):
        """Greater or equal: x.__ge__(y) <==> x>=y
        @rtype: Bool"""
        return _builtins.GE(self,y).evaluate().bool()

    def __getitem__(self,y):
        """Subscript: x.__getitem__(y) <==> x[y]
        @rtype: Data"""
        ans = _builtins.SUBSCRIPT(self,y).evaluate()
        if isinstance(ans,_array.Array):
            if ans.shape[0]==0:
                raise IndexError
        return ans

    def __gt__(self,y):
        """Greater than: x.__gt__(y) <==> x>y
        @rtype: Bool"""
        return _builtins.GT(self,y).evaluate().bool()

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return int(self.getInt().value)

    def __invert__(self):
        """Binary not: x.__invert__() <==> ~x
        @rtype: Data"""
        return Data.execute('~$',self)

    def __le__(self,y):
        """Less than or equal: x.__le__(y) <==> x<=y
        @rtype: Bool"""
        return Data.execute('$<=$',self,y).bool()

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return int(_builtins.SIZE(self).data())

    def __long__(self):
        """Convert this object to python long
        @rtype: long"""
        return _ver.long(self.getLong()._value)

    def __lshift__(self,y):
        """Lrft binary shift: x.__lshift__(y) <==> x<<y
        @rtype: Data"""
        return Data.execute('$<<$',self,y)

    def __lt__(self,y):
        """Less than: x.__lt__(y) <==> x<y
        @rtype: Bool"""
        return Data.execute('$<$',self,y).bool()

    def __mod__(self,y):
        """Modulus: x.__mod__(y) <==> x%y
        @rtype: Data"""
        return Data.execute('$ mod $',self,y)

    def __mul__(self,y):
        """Multiply: x.__mul__(y) <==> x*y
        @rtype: Data"""
        ans = Data.execute('$ * $',self,y)
        return ans

    def __ne__(self,y):
        """Not equal: x.__ne__(y) <==> x!=y
        @rtype: Data"""
        return Data.execute('$ != $',self,y).bool()

    def __neg__(self):
        """Negation: x.__neg__() <==> -x
        @rtype: Data"""
        return Data.execute('-$',self)

    def __nonzero__(self):
        """Not equal 0: x.__nonzero__() <==> x != 0
        @rtype: Bool"""
        return Data.execute('$ != 0',self).bool()

    def __or__(self,y):
        """Or: x.__or__(y) <==> x|y
        @rtype: Data"""
        return Data.execute('$ | $',self,y)

    def __pos__(self):
        """Unary plus: x.__pos__() <==> +x
        @rtype: Data"""
        return self

    def __radd__(self,y):
        """Reverse add: x.__radd__(y) <==> y+x
        @rtype: Data"""
        if isinstance(y,Data):
            return Data.execute('$+$',y,self)
        else:
            return makeData(y)+self

    def __rdiv__(self,y):
        """Reverse divide: x.__rdiv__(y) <==> y/x
        @rtype: Data"""
        return Data.execute('$/$',y,self)

    def __rfloordiv__(self,y):
        """x.__rfloordiv__(y) <==> y//x
        @rtype: Data"""
        return Data.execute('floor($/$)',y,self)

    def __rlshift__(self,y):
        """Reverse left binary shift: x.__rlshift__(y) <==> y<<x
        @rtype: Data"""
        return Data.execute('$ << $',self,y)

    def __rmod__(self,y):
        """Reverse modulus: x.__rmod__(y) <==> y%x
        @rtype: Data"""
        return Data.execute('$ mod $',y,self)

    def __rmul__(self,y):
        """Multiply: x.__rmul__(y) <==> y*x
        @rtype: Data"""
        return self.__mul__(y)

    __ror__=__or__
    def __ror__(self,y):
        """Reverse or: x.__ror__(y) <==> y|x
        @type: Data"""
        return self.__or__(y)

    def __rrshift__(self,y):
        """Reverse right binary shift: x.__rrshift__(y) <==> y>>x
        @rtype: Data"""
        return Data.execute('$ >> $',y,self)

    def __rshift__(self,y):
        """Right binary shift: x.__rshift__(y) <==> x>>y
        @rtype: Data
        """
        return Data.execute('$ >> $',self,y)

    def __rsub__(self,y):
        """Reverse subtract: x.__rsub__(y) <==> y-x
        @rtype: Data"""
        return Data.execute('$ - $',y,self)

    def __rxor__(self,y):
        """Reverse xor: x.__rxor__(y) <==> y^x
        @rtype: Data"""
        return Data.execute('$^$',y,self)

    def __sub__(self,y):
        """Subtract: x.__sub__(y) <==> x-y
        @rtype: Data"""
        return Data.execute('$ - $',self,y)

    def __xor__(self,y):
        """Xor: x.__xor__(y) <==> x^y
        @rtype: Data"""
        return Data.execute('$^$',self,y)
    
    def compare(self,value):
        """Compare this data with argument
        @param value: data to compare to
        @type value: Data
        @return: Return True if the value and this Data object contain the same data
        @rtype: Bool
        """
        status = _MdsShr.MdsCompareXd(_C.pointer(self.descriptor),_C.pointer(makeData(value).descriptor))
        if status == 1:
            return True
        else:
            return False

    @staticmethod
    def compile(*args):
        """Static method (routine in C++) which compiles the expression (via TdiCompile())
        and returns the object instance correspondind to the compiled expression.
        @rtype: Data
        """
        return _builtins.COMPILE(*args).evaluate()

    @staticmethod
    def execute(*args):
        """Execute and expression inserting optional arguments into the expression before evaluating
        @rtype: Data"""
        return Data.compile(*args).evaluate()

    def setTdiVar(self,tdivarname):
        """Set tdi public variable with this data
        @param tdivarname: The name of the public tdi variable to create
        @type tdivarname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return self.execute("`public "+str(tdivarname)+"=$",self)

    @staticmethod
    def getTdiVar(tdivarname):
        """Get value of tdi public variable
        @param tdivarname: The name of the publi tdi variable
        @type tdivarname: string
        @rtype: Data"""
        try:
            return _builtins.PUBLIC(str(tdivarname)).evaluate()
        except:
            return None

    def decompile(self):
        """Return string representation
        @rtype: string
        """
        return str(_builtins.DECOMPILE(self).evaluate())

    __str__=decompile
    """String: x.__str__() <==> str(x)
    @type: String"""

    __repr__=decompile
    """Representation"""


    def data(self,*altvalue):
        """Return primitimive value of the data.
        @rtype: Scalar,Array
        """
        try:
            return _builtins.DATA(self).evaluate().value
        except _Exceptions.TreeNODATA:
            if len(altvalue)==1:
                return altvalue[0]
            raise

    @property
    def descrPtr(self):
        """Return pointer to descriptor of inself as an int
        @rtype: int
        """
        return _C.addressof(self.descriptor)

    def evaluate(self):
        """Return the result of TDI evaluate(this).
        @rtype: Data
        """
        xd = descriptor.Descriptor_xd()
        status = _TdiShr.TdiEvaluate(_C.pointer(self.descriptor),
                                  _C.pointer(xd),
                                  _C.c_void_p(-1))
        if (status & 1 != 0):
            return xd.value
        else:
            raise _Exceptions.statusToException(status)

    @staticmethod
    def _isScalar(x):
        """Is item a Scalar
        @rtype: Bool"""
        _scalar=_mimport('mdsscalar')
        return isinstance(x,_scalar.Scalar)

    def getByte(self):
        """Convert this data into a byte.
        @rtype: Int8
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_builtins.BYTE(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getShort(self):
        """Convert this data into a short.
        @rtype: Int16
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_builtins.WORD(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getInt(self):
        """Convert this data into a int. Implemented at this class level by returning TDI
        data(LONG(this)).If data() fails or the returned class is not scalar, generate
        an exception.
        @rtype: Int32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_builtins.LONG(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getLong(self):
        """Convert this data into a long.
        @rtype: Int64
        @raise TypeError: if data is not a scalar value
        """
        ans=_builtins.QUADWORD(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloat(self):
        """Convert this data into a float32. 
        @rtype: Float32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_builtins.FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getDouble(self):
        """Convert this data into a float64
        @rtype: Float64
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_builtins.FT_FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloatArray(self):
        """Convert this data into a float32.
        @rtype: Float32
        """
        return _builtins.FLOAT(self).evaluate()

    def getDoubleArray(self):
        """Convert this data into a float64.
        @rtype: Float64
        """
        return _builtins.FT_FLOAT(self).evaluate()

    def getShape(self):
        """Get the array dimensions as an integer array.
        @rtype: Int32Array
        """
        return _builtins.SHAPE(self).evaluate()

    def getByteArray(self):
        """Convert this data into a byte array. 
        @rtype: Int8Array
        """
        return _builtins.BYTE(self).evaluate()

    def getShortArray(self):
        """Convert this data into a short array.
        @rtype: Int16Array
        """
        return _builtins.WORD(self).evaluate()

    def getIntArray(self):
        """Convert this data into a int array.
        @rtype: Int32Array
        """
        return _builtins.LONG(self).evaluate()

    def getLongArray(self):
        """Convert this data into a long array.
        @rtype: Int64Array
        """
        return _builtins.QUADWORD(self).evaluate()

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        @rtype: String
        """
        return str(_builtins.TEXT(self).evaluate())

    def hasNodeReference(self):
        """Return True if data item contains a tree reference
        @rtype: Bool
        """
        _compound=_mimport('compound')
        _tree=_mimport("tree")
        if isinstance(self,_tree.TreeNode) or isinstance(self,_tree.TreePath):
            return True
        elif isinstance(self,_compound.Compound):
            for arg in self.args:
                if isinstance(arg,Data) and arg.hasNodeReference():
                    return True
        elif isinstance(self,_apd.Apd):
            for arg in self.getDescs():
                if isinstance(arg,Data) and arg.hasNodeReference():
                    return True
        return False

    def mayHaveChanged(self):
        """return true if the represented data could have been changed since the last time
        this method has been called.
        @rtype: Bool
        """
        return True

    def plot(self,title='',scope=None,row=1,col=1):
        """Plot this data item
        @param title: Title of Scope. Used if scope argument is not provided
        @type title: str
        @param scope: Optional Scope object if adding this to an existing Scope
        @type scope: Scope
        @param row: Row in existing Scope to plot this data
        @type row: int
        @param col: Column in existing Scope
        @type col: int
        @rtype: None
        """
        if scope is None:
            scope=_scope.Scope(title)
        scope.plot(self,self.dim_of(0),row,col)
        scope.show()

    def sind(self):
        """Return sin() of data assuming data is in degrees
        @rtype: Float32Array
        """
        return _builtins.SIND(self).evaluate()

    def serialize(self):
        """Return Uint8Array binary representation.
        @rtype: Uint8Array
        """
        xd=descriptor.Descriptor_xd()
        status=_MdsShr.MdsSerializeDscOut(_C.pointer(self.descriptor),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _Exceptions.statusToException(status)

    @staticmethod
    def deserialize(bytes):
        """Return Data from serialized buffer.
        @param data: Buffer returned from serialize.
        @type data: Uint8Array
        @rtype: Data
        """
        if len(bytes) == 0:  # short cut if setevent did not send array
            return _apd.List([])
        xd=descriptor.Descriptor_xd()
        status=_MdsShr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _Exceptions.statusToException(status)

    @staticmethod
    def makeData(value):
        """Return MDSplus data class from value.
        @param value: Any value
        @type data: Any
        @rtype: Data
        """
        return makeData(value)

class EmptyData(Data):
    """No Value"""
    def __init__(self):
        pass
    
    def __str__(self):
        return "<no-data>"
    
    @property
    def value(self):
        return None

    @property
    def descriptor(self):
        d = descriptor.Descriptor_xd()
        d.dtype = descriptor.Descriptor_xd.dtype_dsc
        return d
   

_scalar=_mimport('mdsscalar')
_apd=_mimport('apd')
_array=_mimport('mdsarray')
_scope=_mimport('scope')
descriptor=_mimport('descriptor')
_compound=_mimport('compound')
_builtins=_mimport('tdibuiltins')
