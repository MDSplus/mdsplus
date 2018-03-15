#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N, ctypes as _C

_ver=_mimport('version')
_dsc=_mimport('descriptor')
_exc=_mimport('mdsExceptions')

MDSplusException = _exc.MDSplusException
MdsException = MDSplusException
#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
_TdiShr=_ver.load_library('TdiShr')
#
#############################################
class staticmethodX(object):
    def __get__(self, inst, cls):
        if inst is not None:
            return self.method.__get__(inst)
        return self.static
    def __init__(self, method):
        self.method = method
    def static(mself,self,*args,**kwargs):
        if self is None: return None
        return mself.method(Data(self),*args,**kwargs)

def _TdiShrFun(function,errormessage,expression,*args,**kwargs):
    def parseArguments(args):
        if len(args)==1 and isinstance(args[0],tuple):
            return parseArguments(args[0])
        return args
    args  = parseArguments(args) #  unwrap tuple style arg list
    dargs = [Data(expression)]+list(map(Data,args))  # cast to Data type
    if "tree" in kwargs:
        tree = kwargs["tree"]
    elif isinstance(expression,Data) and not expression.tree is None:
        tree = expression.tree
    else:
        tree = None
        for arg in args:
            if not isinstance(arg,Data) or arg.tree is None: continue
            tree = arg.tree
            if isinstance(arg,_tre.TreeNode): break
    xd = _dsc.Descriptor_xd()
    rargs = list(map(Data.byref,dargs))+[xd.ref,_C.c_void_p(-1)]
    _tre._TreeCtx.pushTree(tree)
    try:
        _exc.checkStatus(function(*rargs))
    finally:
        _tre._TreeCtx.popTree()
    return xd._setTree(tree).value

def TdiCompile(expression,*args,**kwargs):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    return _TdiShrFun(_TdiShr.TdiCompile,"Error compiling",expression,*args,**kwargs)

def TdiExecute(expression,*args,**kwargs):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    return _TdiShrFun(_TdiShr.TdiExecute,"Error executing",expression,*args,**kwargs)
tdi=TdiExecute
def TdiDecompile(expression,**kwargs):
    """Decompile a TDI expression. Format: TdiDecompile(tdi_expression)"""
    return _ver.tostr(_TdiShrFun(_TdiShr.TdiDecompile,"Error decompiling",expression,**kwargs))

def TdiEvaluate(expression,**kwargs):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    return _TdiShrFun(_TdiShr.TdiEvaluate,"Error evaluating",expression,**kwargs)

def TdiData(expression,**kwargs):
    """Return primiitive data type. Format: TdiData(value)"""
    return _TdiShrFun(_TdiShr.TdiData,"Error converting to data",expression,**kwargs)

class Data(object):
    """Superclass used by most MDSplus objects. This provides default methods if not provided by the subclasses.
    """
    __array_priority__ = 100. ##### Needed to force things like numpy-array * mdsplus-data to use our __rmul__

    _units=None
    _error=None
    _help=None
    _validation=None
    __descriptor=None
    tree=None
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
        @rtype: Data
        """
        if cls is not Data or len(value)==0:
            return object.__new__(cls)
        value = value[0]
        if value is None:
            return EmptyData
        if isinstance(value,(Data,_dsc.Descriptor)):
            return value
        if isinstance(value,(_N.ScalarType,_C._SimpleCData)):
            cls = _sca.Scalar
        elif isinstance(value,(_N.ndarray,_C.Array)):
            cls = _arr.Array
        elif isinstance(value,(tuple,list)):
            cls = _apd.List
        elif isinstance(value,dict):
            cls = _apd.Dictionary
        elif isinstance(value,slice):
            return _cmp.BUILD_RANGE.__new__(_cmp.BUILD_RANGE,value).evaluate()
        else:
            raise TypeError('Cannot make MDSplus data type from type: %s' % (value.__class__,))
        return cls.__new__(cls,value)

    def _setTree(self,tree):
        if isinstance(tree,_tre.Tree): self.tree=tree
        return self

    def __getattr__(self,name):
        def getXxx():
            return self.__getattribute__(name[3:].lower())
        if name.startswith('get'):
            return getXxx
        def setXxx(value):
            self.__setattr__(name[3:].lower(),value)
            return self
        if name.startswith('set'):
            return setXxx
        return self.__getattribute__(name)

    @property
    def deref(self):
        return self

    def value_of(self):
        """Return value part of object
        @rtype: Data"""
        return _cmp.VALUE_OF(self)._setTree(self.tree).evaluate()

    def raw_of(self):
        """Return raw part of object
        @rtype: Data"""
        return _cmp.RAW_OF(self)._setTree(self.tree).evaluate()

    def units_of(self):
        """Return units of object
        @rtype: Data"""
        return _cmp.UNITS_OF(self)._setTree(self.tree).evaluate()

    def getDimensionAt(self,idx=0):
        """Return dimension of object
        @param idx: Index of dimension
        @type idx: int
        @rtype: Data"""
        return _cmp.DIM_OF(self,idx)._setTree(self.tree).evaluate()
    dim_of=getDimensionAt

    @property
    def units(self):
        """units associated with this data."""
        return _cmp.UNITS(self)._setTree(self.tree).evaluate()
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
        return _cmp.ERROR_OF(self)._setTree(self.tree).evaluate()
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
        return _cmp.HELP_OF(self)._setTree(self.tree).evaluate()
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
        return _cmp.VALIDATION_OF(self)._setTree(self.tree).evaluate()
    @validation.setter
    def validation(self,validation):
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation

    """ binary operator methods (order: https://docs.python.org/2/library/operator.html) """
    @staticmethod
    def __bool(data):
        if isinstance(data,_arr.Array):
            return data.all().bool()
        if isinstance(data,Data):
            return data.bool()
        return bool(data)

    def __lt__(self,y):
        return _cmp.LT(self,y)._setTree(self.tree).evaluate().bool()
    def __rlt__(self,y):
        return  Data(y)<self

    def __le__(self,y):
        return _cmp.LE(self,y)._setTree(self.tree).evaluate().bool()
    def __rle__(self,y):
        return  Data(y)<=self

    def __eq__(self,y):
        return _cmp.EQ(self,y)._setTree(self.tree).evaluate().bool()
    def __req__(self,y):
        return  Data(y)==self

    def __ne__(self,y):
        return _cmp.NE(self,y)._setTree(self.tree).evaluate().bool()
    def __rne__(self,y):
        return Data(y)!=self

    def __gt__(self,y):
        return _cmp.GT(self,y)._setTree(self.tree).evaluate().bool()
    def __rgt__(self,y):
        return  Data(y)>self

    def __ge__(self,y):
        return _cmp.GE(self,y)._setTree(self.tree).evaluate().bool()
    def __rge__(self,y):
        return  Data(y)>=self

    def __add__(self,y):
        return _cmp.ADD(self,y)._setTree(self.tree).evaluate()
    def __radd__(self,y):
        return Data(y)+self
    def __iadd__(self,y):
        self._value = (self+y)._value

    def __and__(self,y):
        return _cmp.IAND(self,y)._setTree(self.tree).evaluate()
    def __rand__(self,y):
        return Data(y)&self
    def __iand__(self,y):
        self._value = (self&y)._value

    def __div__(self,y):
        return _cmp.DIVIDE(self,y)._setTree(self.tree).evaluate()
    def __rdiv__(self,y):
        return Data(y)/self
    def __idiv__(self,y):
        self._value = (self/y)._value
    __truediv__=__div__
    __rtruediv__=__rdiv__
    __itruediv__=__idiv__

    def __floordiv__(self,y):
        return _cmp.FLOOR(_cmp.DIVIDE(self,y))._setTree(self.tree).evaluate()
    def __rfloordiv__(self,y):
        return Data(y)//self
    def __ifloordiv__(self,y):
        self._value = (self//y)._value

    def __lshift__(self,y):
        return _cmp.SHIFT_LEFT(self,y)._setTree(self.tree).evaluate()
    def __rlshift__(self,y):
        return Data(y)<<self
    def __ilshift__(self,y):
        self._value = (self<<y)._value

    def __mod__(self,y):
        return _cmp.MOD(self,y)._setTree(self.tree).evaluate()
    def __rmod__(self,y):
        return Data(y)%self
    def __imod__(self,y):
        self._value = (self%y)._value

    def __sub__(self,y):
        return _cmp.SUBTRACT(self,y)._setTree(self.tree).evaluate()
    def __rsub__(self,y):
        return Data(y)-self
    def __isub__(self,y):
        self._value = (self-y)._value

    def __rshift__(self,y):
        return _cmp.SHIFT_RIGHT(self,y)._setTree(self.tree).evaluate()
    def __rrshift__(self,y):
        return Data(y)>>self
    def __irshift__(self,y):
        self._value = (self>>y)._value

    def __mul__(self,y):
        return _cmp.MULTIPLY(self,y)._setTree(self.tree).evaluate()
    def __rmul__(self,y):
        return Data(y)*self
    def __imul__(self,y):
        self._value = (self*y)._value

    def __or__(self,y):
        return _cmp.IOR(self,y)._setTree(self.tree).evaluate()
    def __ror__(self,y):
        return Data(y)|self
    def __ior__(self,y):
        self._value = (self|y)._value

    def __pow__(self,y):
        return _cmp.POWER(self,y)._setTree(self.tree).evaluate()
    def __rpow__(self,y):
        return Data(y)**self
    def __ipow__(self,y):
        self._value = (self**y)._value

    def __xor__(self,y):
        return _cmp.MULTIPLY(self,y)._setTree(self.tree).evaluate()
    def __rxor__(self,y):
        return Data(y)^self
    def __ixor__(self,y):
        self._value = (self^y)._value

    def __abs__(self):
        return _cmp.ABS(self)._setTree(self.tree).evaluate()
    def __invert__(self):
        return _cmp.INOT(self)._setTree(self.tree).evaluate()
    def __neg__(self):
        return _cmp.UNARY_MINUS(self)._setTree(self.tree).evaluate()
    def __pos__(self):
        return _cmp.UNARY_PLUS(self)._setTree(self.tree).evaluate()
    def __nonzero__(self):
        return Data.__bool(self != 0)

    def __hasBadTreeReferences__(self,tree):
        return False

    def __fixTreeReferences__(self,tree):
        return self

    def decompile(self):
        """Return string representation
        @rtype: string"""
        return str(_cmp.DECOMPILE(self)._setTree(self.tree).evaluate())

    def __getitem__(self,y):
        """Subscript: x.__getitem__(y) <==> x[y]
        @rtype: Data"""
        ans = _cmp.SUBSCRIPT(self,y)._setTree(self.tree).evaluate()
        if isinstance(ans,_arr.Array) and ans.shape[0]==0:
            raise IndexError
        return ans

    def __bool__(self):
        """Return boolean
        @rtype: Bool"""
        if isinstance(self,_arr.Array):
            return self._value!=0
        elif isinstance(self,_cmp.Compound) and hasattr(self,'value_of'):
            return self.value_of().bool()
        else:
            ans=int(self)
            return (ans & 1) == 1
    bool=__bool__

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return int(self.getLong()._value)
    __index__ = __int__

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return int(_cmp.SIZE(self)._setTree(self.tree).data())

    def __long__(self):
        """Convert this object to python long
        @rtype: long"""
        return _ver.long(self.getLong()._value)

    def __float__(self):
        """Float: x.__float__() <==> float(x)
        @rtype: float"""
        return float(self.getDouble()._value)

    def __round__(self,*arg):
        """Round value to next integer: x.__round__() <==> round(x)
        @rtype: Data"""
        return round(self._value,*arg)

    def compare(self,value):
        """Compare this data with argument
        @param value: data to compare to
        @type value: Data
        @return: Return True if the value and this Data object contain the same data
        @rtype: Bool
        """
        return bool(
            _MdsShr.MdsCompareXd(self.ref,
                                 Data.byref(value)))
    @property
    def descriptor(self):  # keep ref of descriptor with instance
        self.__descriptor = self._descriptor
        return self.__descriptor

    @staticmethod
    def compile(*args,**kwargs):
        """Static method (routine in C++) which compiles the expression (via TdiCompile())
        and returns the object instance correspondind to the compiled expression.
        @rtype: Data
        """
        return TdiCompile(*args,**kwargs)

    @staticmethod
    def execute(*args,**kwargs):
        """Execute and expression inserting optional arguments into the expression before evaluating
        @rtype: Data"""
        return TdiExecute(*args,**kwargs)

    @staticmethodX
    def evaluate(self,**kwargs):
        """Return the result of TDI evaluate(this).
        @rtype: Data
        """
        return TdiEvaluate(self,**kwargs)


    def assignTo(self,varname):
        """Set tdi variable with this data
        @param varname: The name of the public tdi variable to create
        @type varname: string
        @rtype: Data
        @return: Returns the tdi variable
        """
        return _sca.Ident(varname).assign(self)

    @staticmethodX
    def setTdiVar(self,varname):
        """Set tdi public variable with this data
        @param varname: The name of the public tdi variable to create
        @type varname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return _cmp.PUBLIC(_sca.Ident(varname)).assign(self)

    @staticmethod
    def getTdiVar(varname):
        """Get value of tdi public variable
        @param varname: The name of the public tdi variable
        @type varname: string
        @rtype: Data"""
        try:
            return _cmp.PUBLIC(_sca.Ident(varname)).evaluate()
        except _exc.MDSplusException:
            return None

    def __str__(self):
        try:
            return _ver.tostr(self.decompile())
        except _exc.MDSplusException:
            return self._str_bad_ref()

    def _str_bad_ref(self):
        return super(Data,self).__str__()

    def __repr__(self):
        """Representation
        @rtype: String"""
        return str(self)

    def data(self,*altvalue):
        """Return primitimive value of the data.
        @rtype: numpy or native type
        """
        try:
            data = _cmp.DATA(self)._setTree(self.tree).evaluate()
            return data.value if isinstance(data,Data) else data
        except _exc.TreeNODATA:
            if len(altvalue):
                return altvalue[0]
            raise

    @classmethod
    def byref(cls,data):
        if isinstance(data,(Data,_dsc.Descriptor)):
            return data.ref
        if data is None:
            return _dsc.Descriptor.null
        return cls(data).ref

    @classmethod
    def pointer(cls,data):
        if isinstance(data,_dsc.Descriptor):
            return data.ptr_
        data = cls(data)
        if data is None:
            return _dsc.Descriptor.null
        return data.descriptor.ptr_

    @property
    def ref(self):
        return self.descriptor.ref

    @staticmethod
    def _isScalar(x):
        """Is item a Scalar
        @rtype: Bool"""
        return isinstance(x,_sca.Scalar)

    def getData(self,*altvalue):
        """Return primitimive value of the data.
        @rtype: Scalar,Array
        """
        try:
            return _cmp.DATA(self)._setTree(self.tree).evaluate()
        except _exc.TreeNODATA:
            if len(altvalue):
                return altvalue[0]
            raise

    def getByte(self):
        """Convert this data into a byte.
        @rtype: Int8
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_cmp.BYTE(self)._setTree(self.tree).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getShort(self):
        """Convert this data into a short.
        @rtype: Int16
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_cmp.WORD(self)._setTree(self.tree).evaluate()
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
        ans=_cmp.LONG(self)._setTree(self.tree).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getLong(self):
        """Convert this data into a long.
        @rtype: Int64
        @raise TypeError: if data is not a scalar value
        """
        ans=_cmp.QUADWORD(self)._setTree(self.tree).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloat(self):
        """Convert this data into a float32.
        @rtype: Float32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_cmp.FLOAT(self)._setTree(self.tree).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getDouble(self):
        """Convert this data into a float64
        @rtype: Float64
        @raise TypeError: Raised if data is not a scalar value
        """
        ans=_cmp.FT_FLOAT(self)._setTree(self.tree).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloatArray(self):
        """Convert this data into a float32.
        @rtype: Float32
        """
        return _cmp.FLOAT(self)._setTree(self.tree).evaluate()

    def getDoubleArray(self):
        """Convert this data into a float64.
        @rtype: Float64
        """
        return _cmp.FT_FLOAT(self)._setTree(self.tree).evaluate()

    def getShape(self):
        """Get the array dimensions as an integer array.
        @rtype: Int32Array
        """
        return _cmp.SHAPE(self)._setTree(self.tree).evaluate()

    def getByteArray(self):
        """Convert this data into a byte array.
        @rtype: Int8Array
        """
        return _cmp.BYTE(self)._setTree(self.tree).evaluate()

    def getShortArray(self):
        """Convert this data into a short array.
        @rtype: Int16Array
        """
        return _cmp.WORD(self)._setTree(self.tree).evaluate()

    def getIntArray(self):
        """Convert this data into a int array.
        @rtype: Int32Array
        """
        return _cmp.LONG(self)._setTree(self.tree).evaluate()

    def getLongArray(self):
        """Convert this data into a long array.
        @rtype: Int64Array
        """
        return _cmp.QUADWORD(self)._setTree(self.tree).evaluate()

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        @rtype: String
        """
        return str(_cmp.TEXT(self)._setTree(self.tree).evaluate())

    def hasNodeReference(self):
        """Return True if data item contains a tree reference
        @rtype: Bool
        """
        if isinstance(self,_tre.TreeNode):
            return True
        elif isinstance(self,_cmp.Compound):
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
        return _cmp.SIND(self)._setTree(self.tree).evaluate()

    def serialize(self):
        """Return Uint8Array binary representation.
        @rtype: Uint8Array
        """
        xd=_dsc.Descriptor_xd()
        _exc.checkStatus(
            _MdsShr.MdsSerializeDscOut(self.ref,
                                       xd.ref))
        return xd.value

    @staticmethod
    def deserialize(bytes):
        """Return Data from serialized buffer.
        @param bytes: Buffer returned from serialize.
        @type bytes: Uint8Array
        @rtype: Data
        """
        if len(bytes) == 0:  # short cut if setevent did not send array
            return _apd.List([])
        xd=_dsc.Descriptor_xd()
        _exc.checkStatus(
            _MdsShr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),
                                      xd.ref))
        return xd.value

makeData=Data

class EmptyData(Data):
    _descriptor=_dsc.DescriptorNULL
    dtype_id=24
    """No Value aka *"""
    def __init__(self,*value): pass
    def _setTree(self,*a,**kw): return self
    def decompile(self): return "*"
    @property
    def tree(self): return None
    @tree.setter
    def tree(self,value): return
    @property
    def value(self): return None
    def data(self): return None
    @staticmethod
    def fromDescriptor(d): return EmptyData
EmptyData = EmptyData()

class Missing(EmptyData):
    """No Value aka $Missing"""
    def decompile(self): return "$Missing"
    @staticmethod
    def fromDescriptor(d): return Missing

_dsc.dtypeToClass[0]=Missing
_dsc.dtypeToArrayClass[0]=Missing
_dsc.dtypeToClass[EmptyData.dtype_id]=EmptyData

_cmp=_mimport('compound')
_sca=_mimport('mdsscalar')
_arr=_mimport('mdsarray')
_tre=_mimport('tree')
_apd=_mimport('apd')
