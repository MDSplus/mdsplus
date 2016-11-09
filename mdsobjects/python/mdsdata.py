def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N, ctypes as _C

_version=_mimport('version')
_descriptor=_mimport('descriptor')
_exceptions=_mimport('mdsExceptions')

MDSplusException = _exceptions.MDSplusException
MdsException = MDSplusException
#### Load Shared Libraries Referenced #######
#
_MdsShr=_version.load_library('MdsShr')
_TdiShr=_version.load_library('TdiShr')
#
#############################################
class Data(object):
    """Superclass used by most MDSplus objects. This provides default methods if not provided by the subclasses.
    """
    __array_priority__ = 100. ##### Needed to force things like numpy-array * mdsplus-data to use our __rmul__

    _units=None
    _error=None
    _help=None
    _validation=None

    @property  # used by numpy.array
    def __array_interface__(self):
        data = self.data()
        return {
            'shape':data.shape,
            'typestr':data.dtype.str,
            'descr':data.dtype.descr,
            'strides':data.strides,
            'data':data,
            'version':3,
        }

    def __new__(cls,*value):
        """Convert a python object to a MDSobject Data object
        @param value: Any value
        @type data: Any
        @rtype: Data
        """
        if cls is not Data or len(value)==0:
            return object.__new__(cls)
        value = value[0]
        if value is None:
            return EmptyData
        if isinstance(value,(cls,_tree.TreeNode)):
            return value
        if isinstance(value,(_N.ScalarType,_C._SimpleCData)):
            cls = _scalar.Scalar
        elif isinstance(value,(_N.ndarray,_C.Array,tuple,list)):
            cls = _array.Array
        elif isinstance(value,dict):
            cls = _apd.Dictionary
        elif isinstance(value,slice):
            return _compound.BUILD_RANGE.__new__(_compound.BUILD_RANGE,value).evaluate()
        else:
            raise TypeError('Cannot make MDSplus data type from type: %s' % (str(type(value)),))
        return cls.__new__(cls,value)

    def __function(self,name,default):
        found = False
        ans=self.evaluate()
        while(self is not ans and hasattr(ans,name) and callable(ans.__getattribute__(name))):
            found = True
            ans=ans.__getattribute__(name)()
        if not found:
            return default
        return ans

    @property
    def deref(self):
        return self

    def value_of(self):
        """Return value part of object
        @rtype: Data"""
        return _compound.VALUE_OF(self).evaluate()

    def raw_of(self):
        """Return raw part of object
        @rtype: Data"""
        return _compound.RAW_OF(self).evaluate()

    def units_of(self):
        """Return units of object
        @rtype: Data"""
        return _compound.UNITS_OF(self).evaluate()

    def getDimensionAt(self,idx=0):
        """Return dimension of object
        @param idx: Index of dimension
        @type idx: int
        @rtype: Data"""
        return _compound.DIM_OF(self,idx).evaluate()
    dim_of=getDimensionAt

    @property
    def units(self):
        """units associated with this data."""
        return _compound.UNITS(self).evaluate()
    @units.setter
    def units(self,units):
        if units is None:
            if hasattr(self,'_units'):
                delattr(self,'_units')
        else:
            self._units=units
    def setUnits(self,units):
        self.units=units
        return self

    @property
    def error(self):
        """error property of this data."""
        return _compound.ERROR_OF(self).evaluate()
    @error.setter
    def error(self,error):
        if error is None:
            if hasattr(self,'_error'):
                delattr(self,'_error')
        else:
            self._error=error
    def setError(self,error):
        self.error=error
        return self

    @property
    def help(self):
        """help property of this node."""
        return _compound.HELP_OF(self).evaluate()
    @help.setter
    def help(self,help):
        if help is None:
            if hasattr(self,'_help'):
                delattr(self,'_help')
        else:
            self._help=help
    def setHelp(self,help):
        self.help=help
        return self

    @property
    def validation(self):
        """Validation property of this node"""
        return _compound.VALIDATION_OF(self).evaluate()
    @validation.setter
    def validation(self,validation):
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation
    def setValidation(self,validation):
        self.validation=validation
        return self

    """ binary operator methods (order: https://docs.python.org/2/library/operator.html) """
    @staticmethod
    def __bool(data):
        if isinstance(data,_array.Array):
            return data.all().bool()
        if isinstance(data,Data):
            return data.bool()
        return bool(data)

    def __lt__(self,y):
        return _compound.LT(self,y).evaluate().bool()
    def __rlt__(self,y):
        return  Data(y)<self

    def __le__(self,y):
        return _compound.LE(self,y).evaluate().bool()
    def __rle__(self,y):
        return  Data(y)<=self

    def __eq__(self,y):
        return _compound.EQ(self,y).evaluate().bool()
    def __req__(self,y):
        return  Data(y)==self

    def __ne__(self,y):
        return _compound.NE(self,y).evaluate().bool()
    def __rne__(self,y):
        return Data(y)!=self

    def __gt__(self,y):
        return _compound.GT(self,y).evaluate().bool()
    def __rgt__(self,y):
        return  Data(y)>self

    def __ge__(self,y):
        return _compound.GE(self,y).evaluate().bool()
    def __rge__(self,y):
        return  Data(y)>=self

    def __add__(self,y):
        return _compound.ADD(self,y).evaluate()
    def __radd__(self,y):
        return Data(y)+self
    def __iadd__(self,y):
        self._value = (self+y)._value

    def __and__(self,y):
        return _compound.IAND(self,y).evaluate()
    def __rand__(self,y):
        return Data(y)&self
    def __iand__(self,y):
        self._value = (self&y)._value

    def __div__(self,y):
        return _compound.DIVIDE(self,y).evaluate()
    def __rdiv__(self,y):
        return Data(y)/self
    def __idiv__(self,y):
        self._value = (self/y)._value
    __truediv__=__div__
    __rtruediv__=__rdiv__
    __itruediv__=__idiv__

    def __floordiv__(self,y):
        return _compound.FLOOR(_compound.DIVIDE(self,y)).evaluate()
    def __rfloordiv__(self,y):
        return Data(y)//self
    def __ifloordiv__(self,y):
        self._value = (self//y)._value

    def __lshift__(self,y):
        return _compound.SHIFT_LEFT(self,y).evaluate()
    def __rlshift__(self,y):
        return Data(y)<<self
    def __ilshift__(self,y):
        self._value = (self<<y)._value

    def __mod__(self,y):
        return _compound.MOD(self,y).evaluate()
    def __rmod__(self,y):
        return Data(y)%self
    def __imod__(self,y):
        self._value = (self%y)._value

    def __sub__(self,y):
        return _compound.SUBTRACT(self,y).evaluate()
    def __rsub__(self,y):
        return Data(y)-self
    def __isub__(self,y):
        self._value = (self-y)._value

    def __rshift__(self,y):
        return _compound.SHIFT_RIGHT(self,y).evaluate()
    def __rrshift__(self,y):
        return Data(y)>>self
    def __irshift__(self,y):
        self._value = (self>>y)._value

    def __mul__(self,y):
        return _compound.MULTIPLY(self,y).evaluate()
    def __rmul__(self,y):
        return Data(y)*self
    def __imul__(self,y):
        self._value = (self*y)._value

    def __or__(self,y):
        return _compound.IOR(self,y).evaluate()
    def __ror__(self,y):
        return Data(y)|self
    def __ior__(self,y):
        self._value = (self|y)._value

    def __pow__(self,y):
        return _compound.POWER(self,y).evaluate()
    def __rpow__(self,y):
        return Data(y)**self
    def __ipow__(self,y):
        self._value = (self**y)._value

    def __xor__(self,y):
        return _compound.MULTIPLY(self,y).evaluate()
    def __rxor__(self,y):
        return Data(y)^self
    def __ixor__(self,y):
        self._value = (self^y)._value

    def __abs__(self):
        return _compound.ABS(self).evaluate()
    def __invert__(self):
        return _compound.INOT(self).evaluate()
    def __neg__(self):
        return _compound.UNARY_MINUS(self).evaluate()
    def __pos__(self):
        return _compound.UNARY_PLUS(self).evaluate()
    def __nonzero__(self):
        return Data.__bool(self != 0)

    def __hasBadTreeReferences__(self,tree):
        return False

    def __fixTreeReferences__(self,tree):
        return self

    def decompile(self):
        """Return string representation
        @rtype: string"""
        return _compound.DECOMPILE(self).evaluate()

    def __getitem__(self,y):
        """Subscript: x.__getitem__(y) <==> x[y]
        @rtype: Data"""
        ans = _compound.SUBSCRIPT(self,y).evaluate()
        if isinstance(ans,_array.Array) and ans.shape[0]==0:
            raise IndexError
        return ans

    def __bool__(self):
        """Return boolean
        @rtype: Bool"""
        if isinstance(self,_array.Array):
            return self._value!=0
        elif isinstance(self,_compound.Compound) and hasattr(self,'value_of'):
            return self.value_of().bool()
        else:
            ans=int(self)
            return (ans & 1) == 1
    bool=__bool__

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return int(self.getInt().value)

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return int(_compound.SIZE(self).data())

    def __long__(self):
        """Convert this object to python long
        @rtype: long"""
        return _version.long(self.getLong()._value)

    def __float__(self):
        """Float: x.__float__() <==> float(x)
        @rtype: float"""
        return float(self.getInt().value)

    def compare(self,value):
        """Compare this data with argument
        @param value: data to compare to
        @type value: Data
        @return: Return True if the value and this Data object contain the same data
        @rtype: Bool
        """
        status = _MdsShr.MdsCompareXd(_C.pointer(self.descriptor),_descriptor.objectToPointer(Data(value)))
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
        if len(args)==2 and isinstance(args[1],(tuple,)):
            args = tuple([args[0]]+list(args[1]))  # compatibility
        return _compound.COMPILE(*args).evaluate()

    @staticmethod
    def execute(*args):
        """Execute and expression inserting optional arguments into the expression before evaluating
        @rtype: Data"""
        return Data.compile(*args).evaluate()

    def assignTo(self,varname):
        """Set tdi variable with this data
        @param varname: The name of the public tdi variable to create
        @type varname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return self.execute("%s=$"%(varname,),self)

    def setTdiVar(self,tdivarname):
        """Set tdi public variable with this data
        @param tdivarname: The name of the public tdi variable to create
        @type tdivarname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return self.execute("public %s=$"%(tdivarname,),self)

    @staticmethod
    def getTdiVar(tdivarname):
        """Get value of tdi public variable
        @param tdivarname: The name of the publi tdi variable
        @type tdivarname: string
        @rtype: Data"""
        try:
            return _compound.PUBLIC(str(tdivarname)).evaluate()
        except:
            return None

    def __repr__(self):
        """Representation
        @type: String"""
        return str(self.decompile())
    __str__=__repr__

    def data(self,*altvalue):
        """Return primitimive value of the data.
        @rtype: numpy or native type
        """
        try:
            return _compound.DATA(self).evaluate().value
        except _exceptions.TreeNODATA:
            if len(altvalue):
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
        xd = _descriptor.Descriptor_xd()
        status = _TdiShr.TdiEvaluate(_descriptor.objectToPointer(self),
                                  _C.pointer(xd),
                                  _C.c_void_p(-1))
        if (status & 1 != 0):
            return xd.value
        else:
            raise _exceptions.statusToException(status)

    @staticmethod
    def _isScalar(x):
        """Is item a Scalar
        @rtype: Bool"""
        return isinstance(x,_scalar.Scalar)

    def getData(self,*altvalue):
        """Return primitimive value of the data.
        @rtype: Scalar,Array
        """
        try:
            return _compound.DATA(self).evaluate()
        except _exceptions.TreeNODATA:
            if len(altvalue):
                return altvalue[0]
            raise

    def getByte(self):
        """Convert this data into a byte.
        @rtype: Int8
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_compound.BYTE(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getShort(self):
        """Convert this data into a short.
        @rtype: Int16
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_compound.WORD(self).evaluate()
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
        ans=_compound.LONG(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getLong(self):
        """Convert this data into a long.
        @rtype: Int64
        @raise TypeError: if data is not a scalar value
        """
        ans=_compound.QUADWORD(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloat(self):
        """Convert this data into a float32.
        @rtype: Float32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_compound.FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getDouble(self):
        """Convert this data into a float64
        @rtype: Float64
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_compound.FT_FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloatArray(self):
        """Convert this data into a float32.
        @rtype: Float32
        """
        return _compound.FLOAT(self).evaluate()

    def getDoubleArray(self):
        """Convert this data into a float64.
        @rtype: Float64
        """
        return _compound.FT_FLOAT(self).evaluate()

    def getShape(self):
        """Get the array dimensions as an integer array.
        @rtype: Int32Array
        """
        return _compound.SHAPE(self).evaluate()

    def getByteArray(self):
        """Convert this data into a byte array.
        @rtype: Int8Array
        """
        return _compound.BYTE(self).evaluate()

    def getShortArray(self):
        """Convert this data into a short array.
        @rtype: Int16Array
        """
        return _compound.WORD(self).evaluate()

    def getIntArray(self):
        """Convert this data into a int array.
        @rtype: Int32Array
        """
        return _compound.LONG(self).evaluate()

    def getLongArray(self):
        """Convert this data into a long array.
        @rtype: Int64Array
        """
        return _compound.QUADWORD(self).evaluate()

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        @rtype: String
        """
        return str(_compound.TEXT(self).evaluate())

    def hasNodeReference(self):
        """Return True if data item contains a tree reference
        @rtype: Bool
        """
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
            scope=_mimport('scope').Scope(title)
        scope.plot(self,self.dim_of(0),row,col)
        scope.show()

    def sind(self):
        """Return sin() of data assuming data is in degrees
        @rtype: Float32Array
        """
        return _compound.SIND(self).evaluate()

    def serialize(self):
        """Return Uint8Array binary representation.
        @rtype: Uint8Array
        """
        xd=_descriptor.Descriptor_xd()
        status=_MdsShr.MdsSerializeDscOut(_C.pointer(self.descriptor),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _exceptions.statusToException(status)

    @staticmethod
    def deserialize(bytes):
        """Return Data from serialized buffer.
        @param data: Buffer returned from serialize.
        @type data: Uint8Array
        @rtype: Data
        """
        if len(bytes) == 0:  # short cut if setevent did not send array
            return _apd.List([])
        xd=_descriptor.Descriptor_xd()
        status=_MdsShr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),_C.pointer(xd))
        if (status & 1) == 1:
            return xd.value
        else:
            raise _exceptions.statusToException(status)

makeData=Data

class EmptyData(Data):
    """No Value aka *"""
    def __init__(self,*value):
        pass

    def decompile(self):
        return "*"

    @property
    def value(self):
        return None

    def data(self):
        return None

    @property
    def descriptor(self):
        return None

    @classmethod
    def fromDescriptor(cls,d):
        return EmptyData
EmptyData = EmptyData()

class Missing(EmptyData):
    """No Value aka $Missing"""
    descriptor=_descriptor.Descriptor()
    _descriptor.dtype=0
    def decompile(self):
        return "$Missing"
    @classmethod
    def fromDescriptor(cls,d):
        return Missing


_descriptor.dtypeToClass[0]=Missing
_descriptor.dtypeToArrayClass[0]=Missing

_compound=_mimport('compound')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_tree=_mimport('tree')
_apd=_mimport('apd')
