import numpy
import copy
from MDSobjects._tdishr import TdiEvaluate,TdiCompile,TdiDecompile,TdiData,TdiExecute

def __mdsplus_function(args,exp,default):
    try:
        return TdiExecute(exp,args)
    except (AttributeError,TypeError),e:
#        print e
        return default

def getUnits(item):
    """Return units of item. Evaluate the units expression if necessary."""
    return __mdsplus_function((item,),"units($)","")
    
def getError(item):
    """Return the data of the error of an object"""
    return __mdsplus_function((item,),"error_of($)",None)
    
def getValuePart(item):
    """Return the value portion of an object"""
    return __mdsplus_function((item,),"value_of($)",None)

def getDimension(item,idx=0):
    """Return dimension of an object"""
    return __mdsplus_function((item,idx),"dim_of($,$)",None)

def data(item):
    """Return the data for an object converted into a primitive (non-mdsplus) data type"""
    return __mdsplus_function((item,),"data($)",item)

def decompile(item):
    """Returns the item converted to text"""
    return __mdsplus_function((item,),"decompile($)",str(item))

def evaluate(item,):
    """Return evaluation of mdsplus object"""
    return __mdsplus_function((item,),"evaluate($)",item)

def rawPart(item):
    """Return raw portion of data item"""
    return __mdsplus_function((item,),"raw_of($)",None)
   
def makeData(value):
    if value is None:
        return EmptyData()
    if isinstance(value,Data):
#        return copy.deepcopy(value)
        return value
    if isinstance(value,numpy.generic) | isinstance(value,int) | isinstance(value,long) | isinstance(value,float) | isinstance(value,str):
        from MDSobjects.scalar import makeScalar
        return makeScalar(value)
    if isinstance(value,numpy.ndarray) | isinstance(value,tuple) | isinstance(value,list):
        from MDSobjects.array import makeArray
        return makeArray(value)

class Data(object):
    """Superclass used by most MDSplus objects. This provides default methods if not provided by the subclasses."""
    
    def __init__(self,*value):
        """Cannot create instancces of class Data objects. Use MDSobjects.Data.makeData(initial-value) instead"""
        raise Exception,'Cannot create instances of class Data. Use MDSobjects.Data.makeData(initial-value) instead'

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
        """Return value part of object"""
        return Data.execute('value_of($)',self)

    def raw_of(self):
        """Return raw part of object"""
        return Data.execute('raw_of($)',self)

    def _getUnits(self):
        return Data.execute('units($)',self)

    def _setUnits(self,units):
        if units is None:
            if hasattr(self,'_units'):
                delattr(self,'_units')
        else:
            self._units=units
        return

    units=property(_getUnits,_setUnits)
            
    def _getError(self):
        return Data.execute('error_of($)',self)

    def _setError(self,error):
        if error is None:
            if hasattr(self,'_error'):
                delattr(self,'_error')
        else:
            self._error=error
        return

    error=property(_getError,_setError)
            
    def _getHelp(self):
        return Data.execute('help_of($)',self)

    def _setHelp(self,help):
        if help is None:
            if hasattr(self,'_help'):
                delattr(self,'_help')
        else:
            self._help=help
        return

    help=property(_getHelp,_setHelp)
            
    def _getValidation(self):
        return Data.execute('validation_of($)',self)

    def _setValidation(self,validation):
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation
        return

    validation=property(_getValidation,_setValidation)
            
    def units_of(self):
        """Return units part of object or a string with a single blank space it no units part"""
        return Data.execute('units_of($)',self)

    def push_dollar_value(self):
        """Set $value for expression evaluation"""
        pass

    def pop_dollar_value(self):
        """Pop $value for expression evaluation"""
        pass

    def __abs__(self):
        return Data.execute('abs($)',self)

    def __add__(self,y):
        from MDSobjects.scalar import String
        from MDSobjects.array import StringArray
        y=makeData(y)
        if isinstance(self,String) or isinstance(y,String) or isinstance(self,StringArray) or isinstance(y,StringArray):
            return Data.execute('$//$',self,y)
        else:
            return Data.execute('$+$',self,y)

    def __and__(self,y):
        return Data.execute('$ && $',self,y)

    def __mul__(self,y):
        return Data.execute('$ * $',self,y)

    __rmul__=__mul__

    def __sub__(self,y):
        return Data.execute('$ - $',self,y)

    def __rsub__(self,y):
        return Data.execute('$ - $',y,self)

    def __div__(self,y):
        return Data.execute('$/$',self,y)

    def __rdiv__(self,y):
        return Data.execute('$/$',y,self)

    def __floordiv__(self,y):
        return Data.execute('floor($/$)',self,y)

    def __radd__(self,y):
        from MDSobjects.scalar import String
        from MDSobjects.array import StringArray
        y=makeData(y)
        if isinstance(self,String) or isinstance(y,String) or isinstance(self,StringArray) or isinstance(y,StringArray):
            return Data.execute('$//$',y,self)
        else:
            return Data.execute('$+$',y,self)

    def _getDescriptor(self):
        from MDSobjects._descriptor import descriptor
        return descriptor(self)

    descriptor=property(_getDescriptor)

    def compile(expr, *args):
        """Static method (routine in C++) which compiles the expression (via TdiCompile())
        and returns the object instance correspondind to the compiled expression.
        """
        return TdiCompile(expr,args)
    compile=staticmethod(compile)

    def execute(expr,*args):
        return TdiExecute(expr,args)
    execute=staticmethod(execute)
    
    def decompile(self):
        """Return the result of TDI decompile(this)
        """
        return TdiDecompile(self)

    __str__=decompile

    def __len__(self):
        return int(TdiCompile('size($)',(self,)).data())

    def data(self):
        """Method data exports TDI data() functionality, i.e. returns a native type
        (scalar or array). If isImmutable() returns true and a cached instance is
        avavilable, this is returned instead.
        """
        return TdiData(self)

    def evaluate(self):
        """Return the result of TDI evaluate(this).
        """
        return TdiEvaluate(self)

    def _isScalar(x):
        from MDSobjects.scalar import Scalar
        return isinstance(x,Scalar)
    _isScalar=staticmethod(_isScalar)
    
    def getByte(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(BYTE(this)). If data() fails or the returned class is not scalar,
        generated an exception.
        """
        ans=Data.execute('byte($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans

    def getShort(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(WORD(this)).If data() fails or the returned class is not scalar, generated
        an exception.
        """
        ans=Data.execute('word($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans
    
    def getInt(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(LONG(this)).If data() fails or the returned class is not scalar, generated
        an exception.
        """
        ans=Data.execute('long($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans

    def __int__(self):
        return int(self.getInt()._value)

    def getLong(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(QUADWORD(this)).If data() fails or the returned class is not scalar,
        generated an exception.
        """
        ans=Data.execute('quadword($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans

    def __long__(self):
        return long(self.getLong()._value)

    def getFloat(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
        generated an exception.
        """
        ans=Data.execute('float($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans

    def __float__(self):
        return float(self.getFloat()._value)
    
    def getDouble(self):
        """Convert this data into a byte. Implemented at this class level by returning TDI
        data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
        generated an exception.
        """
        ans=Data.execute('ft_float($)',self)
        if not Data._isScalar(ans):
            raise TypeError,'Value not a scalar'
        return ans

    def getShape(self):
        """Get the array dimensions as an integer array. It is implemented at this class
        level by computing TDI expression SHAPE(this). If shape fails an exception is
        generated.
        """
        return Data.execute('shape($)',self)

    def getByteArray(self):
        """Convert this data into a byte array. Implemented at this class level by
        returning TDI data(BYTE(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        """
        return Data.execute('byte($)',self)

    def getShortArray(self):
        """Convert this data into a byte array. Implemented at this class level by
        returning TDI data(WORD(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        """
        return Data.execute('short($)',self)

    def getIntArray(self):
        """Convert this data into a byte array. Implemented at this class level by
        returning TDI data (LONG(this)). If data() fails or the returned class is not
        array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        """
        return Data.execute('long($)',self)

    def getLongArray(self):
        """Convert this data into a byte array. Implemented at this class level by
        returning TDI data(QUADWORD(this)). If data() fails or the returned class is
        not array, generates an exception. In Java and C++ will return a 1 dimensional
        array using row-first ordering if a multidimensional array.
        """
        return Data.execute('quadword($)',self)

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        """
        return str(Data.execute('text($)',self))

    def getUnits(self):
        """Return the TDI evaluation of UNITS_OF(this). EmptyData is returned if no units
        defined.
        """
        return self.units

    def getHelp(self):
        """Returns the result of TDI GET_HELP(this). Returns EmptyData if no help field
        defined.
        """
        return self.help

    def getError(self):
        """Get the error field. Returns EmptyData if no error defined.
        """
        return self.error

    def setUnits(self,units):
        self.units=units

    def setHelp(self,help):
        """Set the Help  field for this Data instance.
        """
        self.help=help

    def setError(self,error):
        """Set the Error field for this Data instance.
        """
        self.error=error

    def mayHaveChanged(self):
        """return true if the represented data could have been changed since the last time
        this method has been called.
        """
        return True

    def sind(self):
        return Data.execute('sind($)',self)

class EmptyData(Data):
    """No Value"""
    def __init__(self):
        pass
    
    def __str__(self):
        return "<no-data>"
    pass

