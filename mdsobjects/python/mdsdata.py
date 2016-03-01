def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N

_dtypes=_mimport('_mdsdtypes')
_ver=_mimport('version')
_Exceptions=_mimport('mdsExceptions')

MDSplusException = _Exceptions.MDSplusException
MdsException = MDSplusException

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
        return Data.execute('value_of($)',item)
    except:
        return None

def getDimension(item,idx=0):
    """Return dimension of an object
    @rtype: Data"""
    try:
        return Data.execute('dim_of($,$)',item,idx)
    except:
        return None

def data(item):
    """Return the data for an object converted into a primitive data type
    @rtype: Data"""
    return _tdishr.TdiCompile('data($)',(item,)).evaluate().value

def decompile(item):
    """Returns the item converted to a string
    @rtype: string"""

    return _tdishr.TdiDecompile(item)

def evaluate(item,):
    """Return evaluation of mdsplus object"""
    try:
        return item.evaluate()
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
    if value is None:
        return EmptyData()
    if isinstance(value,Data):
        return value
    if isinstance(value,(_N.generic,int,float,complex,_ver.basestring,_ver.long)):
        return _scalar.makeScalar(value)
    if isinstance(value,(tuple,list)):
        apd = _apd.Apd(tuple(value),_dtypes.DTYPE_LIST)
        return _apd.List(apd)
    if isinstance(value,_N.ndarray):
        return _array.makeArray(value)
    if isinstance(value,dict):
        return _apd.Dictionary(value)
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
        return Data.execute('value_of($)',self)

    def raw_of(self):
        """Return raw part of object
        @rtype: Data"""
        return Data.execute('raw_of($)',self)
    def units_of(self):
        """Return units of object
        @rtype: Data"""
        return Data.execute('units_of($)',self)

    def getDimensionAt(self,idx=0):
        """Return dimension of object
        @param idx: Index of dimension
        @type idx: int
        @rtype: Data"""
        return Data.execute('dim_of($,$)',(self,idx))

    dim_of=getDimensionAt

    def getUnits(self):
        """Return the TDI evaluation of UNITS_OF(this).
        EmptyData is returned if no units defined.
        @rtype: Data"""
        return Data.execute('units($)',self)
    def setUnits(self,units):
        """Set the units of the Data instance.
        @type: String
        @rtype: original type"""
        if units is None:
            if hasattr(self,'_units'):
                delattr(self,'_units')
        else:
            self._units=units
        return self
    units=property(getUnits,setUnits)

    def getError(self):
        """Get the error field.
        Returns EmptyData if no error defined.
        @rtype: Data"""
        return Data.execute('error_of($)',self)

    def setError(self,error):
        """Set the Error field for this Data instance.
        @type: Data
        @rtype: original type"""
        if error is None:
            if hasattr(self,'_error'):
                delattr(self,'_error')
        else:
            self._error=error
        return self

    error=property(getError,setError)

    def getHelp(self):
        """Returns the result of TDI GET_HELP(this).
        Returns EmptyData if no help field defined.
        @rtype: Data"""
        return Data.execute('help_of($)',self)

    def setHelp(self,help):
        """Set the Help  field for this Data instance.
        @rtype: original type
        """
        if help is None:
            if hasattr(self,'_help'):
                delattr(self,'_help')
        else:
            self._help=help
        return self

    help=property(getHelp,setHelp)

    def getValidation(self):
        return Data.execute('validation_of($)',self)

    def setValidation(self,validation):
        """A validation procedure for the data.
        Currently no built-in utilities make use of this validation property.
        One could envision storing an expression which tests the data and returns
        a result.
        @type: Data
        @rtype: original type"""
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation
        return self

    validation=property(getValidation,setValidation)

    def push_dollar_value(self):
        """Set $value for expression evaluation
        @rtype: None"""
        pass

    def pop_dollar_value(self):
        """Pop $value for expression evaluation
        @rtype: Data"""
        pass

    def __abs__(self):
        """
        Absolute value: x.__abs__() <==> abs(x)
        @rtype: Data
        """
        return Data.execute('abs($)',self)

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
        if isinstance(y,Data):
            return Data.execute('$+$',self,y)
        else:
            return self+makeData(y)

    def __and__(self,y):
        """And: x.__and__(y) <==> x&y
        @rtype: Data"""
        return Data.execute('$ & $',self,y)

    def __div__(self,y):
        """Divide: x.__div__(y) <==> x/y
        @rtype: Data"""
        return Data.execute('$/$',self,y)

    __truediv__=__div__

    def __eq__(self,y):
        """Equals: x.__eq__(y) <==> x==y
        @rtype: Bool"""
        try:
            return Data.execute('$ == $',self,y).bool()
        except:
            return False

    def __hasBadTreeReferences__(self,tree):
        return False

    def __fixTreeReferences__(self,tree):
        return self

    def __float__(self):
        """Float: x.__float__() <==> float(x)
        @rtype: Data"""
        return float(Data.execute('float($)[0]',self).value)

    def __floordiv__(self,y):
        """Floordiv: x.__floordiv__(y) <==> x//y
        @rtype: Data"""
        return Data.execute('floor($/$)',self,y)

    def __ge__(self,y):
        """Greater or equal: x.__ge__(y) <==> x>=y
        @rtype: Bool"""
        return Data.execute('$ >= $',self,y).bool()

    def __getitem__(self,y):
        """Subscript: x.__getitem__(y) <==> x[y]
        @rtype: Data"""
        if isinstance(y,slice):
            y=_compound.Range(y.start,y.stop,y.step)
        ans = Data.execute('$[$]',self,y)
        if isinstance(ans,_array.Array):
            if ans.shape[0]==0:
                raise IndexError
        return ans

    def __gt__(self,y):
        """Greater than: x.__gt__(y) <==> x>y
        @rtype: Bool"""
        return Data.execute('$ > $',self,y).bool()

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
        return int(_tdishr.TdiCompile('size($)',(self,)).data())

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

    def _getDescriptor(self):
        """Return descriptor for passing data to MDSplus library routines.
        @rtype: descriptor
        """
        return _descriptor.descriptor(self)

    descriptor=property(_getDescriptor)
    """Descriptor of data.
    @type: descriptor
    """

    def compare(self,value):
        """Compare this data with argument
        @param value: data to compare to
        @type value: Data
        @return: Return True if the value and this Data object contain the same data
        @rtype: Bool
        """
        status = _mdsshr.MdsCompareXd(self,value)
        if status == 1:
            return True
        else:
            return False

    def compile(expr, *args):
        """Static method (routine in C++) which compiles the expression (via TdiCompile())
        and returns the object instance correspondind to the compiled expression.
        @rtype: Data
        """
        return _tdishr.TdiCompile(expr,args)
    compile=staticmethod(compile)

    def execute(expr,*args):
        """Execute and expression inserting optional arguments into the expression before evaluating
        @rtype: Data"""
        return _tdishr.TdiExecute(expr,args)
    execute=staticmethod(execute)

    def setTdiVar(self,tdivarname):
        """Set tdi public variable with this data
        @param tdivarname: The name of the public tdi variable to create
        @type tdivarname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return self.execute("`public "+str(tdivarname)+"=$",self)

    def getTdiVar(tdivarname):
        """Get value of tdi public variable
        @param tdivarname: The name of the publi tdi variable
        @type tdivarname: string
        @rtype: Data"""
        try:
#            return _compound.Function(opcode='public',args=(str(tdivarname),)).evaluate()
            return _tdishr.TdiExecute('public '+str(tdivarname))
        except:
            return None
    getTdiVar=staticmethod(getTdiVar)

    def decompile(self):
        """Return string representation
        @rtype: string
        """
        return _tdishr.TdiDecompile(self)

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
            return self.execute("data($)",(self,)).value
        except _Exceptions.TreeNODATA:
            if len(altvalue)==1:
                return altvalue[0]
            raise

    def _getDescrPtr(self):
        """Return pointer to descriptor of inself as an int
        @rtype: int
        """
        from ctypes import addressof
        return addressof(self.descriptor)

    descrPtr=property(_getDescrPtr)

    def evaluate(self):
        """Return the result of TDI evaluate(this).
        @rtype: Data
        """
        return _tdishr.TdiEvaluate(self)

    def _isScalar(x):
        """Is item a Scalar
        @rtype: Bool"""
        _scalar=_mimport('mdsscalar')
        return isinstance(x,_scalar.Scalar)
    _isScalar=staticmethod(_isScalar)

    def getByte(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(BYTE(this)). If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Int8
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=Data.execute('byte($)',self)
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getShort(self):
        """Convert this data into a short. Implemented at this class level by returning TDI
        data(WORD(this)).If data() fails or the returned class is not scalar, generate
        an exception.
        @rtype: Int16
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=Data.execute('word($)',self)
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
        ans=Data.execute('long($)',self)
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getLong(self):
        """Convert this data into a long. Implemented at this class level by returning TDI
        data(QUADWORD(this)).If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Int64
        @raise TypeError: if data is not a scalar value
        """
        ans=Data.execute('quadword($)',self)
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloat(self):
        """Convert this data into a float32. Implemented at this class level by returning TDI
        data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Float32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=Data.execute('float($)',self)
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getDouble(self):
        """Convert this data into a float64. Implemented at this class level by returning TDI
        data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Float64
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=Data.execute('ft_float($)',self)
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloatArray(self):
        """Convert this data into a float32. Implemented at this class level by returning TDI
        data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Float32
        """
        ans=Data.execute('float($)',self)
        return ans

    def getDoubleArray(self):
        """Convert this data into a float64. Implemented at this class level by returning TDI
        data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
        generate an exception.
        @rtype: Float64
        """
        ans=Data.execute('ft_float($)',self)
        return ans

    def getShape(self):
        """Get the array dimensions as an integer array. It is implemented at this class
        level by computing TDI expression SHAPE(this). If shape fails an exception is
        generated.
        @rtype: Int32Array
        """
        return Data.execute('shape($)',self)

    def getByteArray(self):
        """Convert this data into a byte array. Implemented at this class level by
        returning TDI data(BYTE(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        @rtype: Int8Array
        """
        return Data.execute('byte($)',self)

    def getShortArray(self):
        """Convert this data into a short array. Implemented at this class level by
        returning TDI data(WORD(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        @rtype: Int16Array
        """
        return Data.execute('word($)',self)

    def getIntArray(self):
        """Convert this data into a int array. Implemented at this class level by
        returning TDI data (LONG(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        @rtype: Int32Array
        """
        return Data.execute('long($)',self)

    def getLongArray(self):
        """Convert this data into a long array. Implemented at this class level by
        returning TDI data(QUADWORD(this)). If data() fails or the returned class is
        not array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        @rtype: Int64Array
        """
        return Data.execute('quadword($)',self)

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        @rtype: String
        """
        return str(Data.execute('text($)',self))

    def hasNodeReference(self):
        """Return True if data item contains a tree reference
        @rtype: Bool
        """
        if isinstance(self,_treenode.TreeNode) or isinstance(self,_treenode.TreePath):
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
        return Data.execute('sind($)',self)

    def serialize(self):
        """Return Uint8Array binary representation.
        @rtype: Uint8Array
        """
        return _array.Uint8Array(_mdsshr.MdsSerializeDscOut(self))
        return Data.execute('SerializeOut($)',self)

    def deserialize(data):
        """Return Data from serialized buffer.
        @param data: Buffer returned from serialize.
        @type data: Uint8Array
        @rtype: Data
        """
        return _mdsshr.MdsSerializeDscIn(data)
    deserialize=staticmethod(deserialize)

    def makeData(value):
        """Return MDSplus data class from value.
        @param value: Any value
        @type data: Any
        @rtype: Data
        """
        return makeData(value)
    makeData=staticmethod(makeData)

class EmptyData(Data):
    """No Value"""
    def __init__(self):
        pass

    def __str__(self):
        return "<no-data>"

    def _getValue(self):
       return None

    value=property(_getValue)


_descriptor=_mimport('_descriptor')
_tdishr=_mimport('_tdishr')
_mdsshr=_mimport('_mdsshr')
_apd=_mimport('apd')
_compound=_mimport('compound')
_array=_mimport('mdsarray')
_scalar=_mimport('mdsscalar')
_scope=_mimport('scope')
_treenode=_mimport('treenode')
