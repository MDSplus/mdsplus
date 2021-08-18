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
    except Exception:
        return __import__(name, globals())


import numpy as _N
import ctypes as _C

_ver = _mimport('version')
_dsc = _mimport('descriptor')
_exc = _mimport('mdsExceptions')

MDSplusException = _exc.MDSplusException
#### Load Shared Libraries Referenced #######
#
_MdsShr = _ver.load_library('MdsShr')
_MdsMisc = _ver.load_library('MdsMisc')
#
#############################################


class staticmethodX(object):
    def __get__(self, inst, cls):
        if inst is not None:
            return self.method.__get__(inst)
        return self.static

    def __init__(self, method):
        self.method = method

    def static(self, obj, *args, **kwargs):
        if obj is None:
            return None
        return self.method(Data(obj), *args, **kwargs)


def _unwrap(args):
    if len(args) == 1 and isinstance(args[0], tuple):
        return _unwrap(args[0])
    return args


def TdiCompile(expression, *args, **kwargs):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    return _cmp.COMPILE(expression, *_unwrap(args))._setTree(**kwargs).evaluate()


def TdiData(mdsobject, **kwargs):
    """Convert MDSplus object into primitive data type. Format: TdiData(mdsobject)"""
    return _cmp.DATA(mdsobject)._setTree(**kwargs).evaluate()


def TdiDecompile(mdsobject, **kwargs):
    """Decompile an MDSplus object. Format: TdiDecompile(mdsobject)"""
    return _ver.tostr(_cmp.DECOMPILE(mdsobject)._setTree(**kwargs).evaluate())


def TdiEvaluate(mdsobject, **kwargs):
    """Evaluate an MDSplus object. Format: TdiEvaluate(mdsobject)"""
    return _cmp.EVALUATE(mdsobject)._setTree(**kwargs).evaluate()


def TdiExecute(expression, *args, **kwargs):
    """Compile and evaluate a TDI expression. Format: TdiExecute('expression-string')"""
    return _cmp.EXECUTE(expression, *_unwrap(args))._setTree(**kwargs).evaluate()


tdi = TdiExecute


class NoTreeRef(object):
    @property
    def tree(self): return

    @tree.setter
    def tree(self, val): return

    def _setTree(self, *a, **kw): return self

    def __hasBadTreeReferences__(self, tree):
        return False

    def __fixTreeReferences__(self, tree):
        return self


class TreeRef(object):
    tree = None

    def _setTree(self, *args, **kwargs):
        if 'tree' in kwargs and isinstance(kwargs['tree'], _tre.Tree):
            self.tree = kwargs['tree']
            return self
        if len(args) == 1 and isinstance(args[0], _tre.Tree):
            self.tree = args[0]
            return self
        for arg in args:
            if isinstance(arg, TreeRef) and isinstance(arg.tree, _tre.Tree):
                self.tree = arg.tree
                return self
        return self

    def __hasBadTreeReferences__(self, tree):
        return self.tree != tree

    def __fixTreeReferences__(self, tree):
        self.tree = tree
        return self


class Data(NoTreeRef):
    """Superclass used by most MDSplus objects. This provides default methods if not provided by the subclasses.
    """
    __array_priority__ = 100.  # Needed to force things like numpy-array * mdsplus-data to use our __rmul__

    _units = None
    _error = None
    _help = None
    _validation = None
    __descriptor = None

    @property  # numpy array
    def __array_interface__(self):
        data = _N.array(self.data(), copy=False)
        aif = data.__array_interface__
        aif['data'] = data
        return aif

    def __new__(cls, *value):
        """Convert a python object to a MDSobject Data object
        @param value: Any value
        @rtype: Data
        """
        if cls is not Data or len(value) == 0:
            return object.__new__(cls)
        value = value[0]
        if value is None:
            return EmptyData
        if isinstance(value, (Data, _dsc.Descriptor)):
            return value
        if isinstance(value, (_N.ScalarType, _C._SimpleCData)):
            cls = _scr.Scalar
        elif isinstance(value, (_N.ndarray, _C.Array)):
            cls = _arr.Array
        elif isinstance(value, (tuple, list)):
            cls = _apd.List
        elif isinstance(value, dict):
            cls = _apd.Dictionary
        elif isinstance(value, slice):
            return _cmp.BUILD_RANGE.__new__(_cmp.BUILD_RANGE, value).evaluate()
        else:
            raise TypeError(
                'Cannot make MDSplus data type from type: %s' % (value.__class__,))
        return cls.__new__(cls, value)

    def getXYSignal(self, x=None, xmin=None, xmax=None, num=2048):
        num = _C.c_int32(num)
        xmin = Data(xmin)
        xmax = Data(xmax)
        xd = _dsc.DescriptorXD()
        if self.tree is None:
            status = _MdsMisc. GetXYSignalXd(self.ref, Data.byref(
                x), Data.byref(xmin), Data.byref(xmax), num, xd.ref)
        else:
            status = _MdsMisc._GetXYSignalXd(self.tree.pctx, self.ref, Data.byref(
                x), Data.byref(xmin), Data.byref(xmax), num, xd.ref)
        _exc.checkStatus(status)
        return xd.value

    def __getattr__(self, name):
        def getXxx():
            return self.__getattribute__(name[3:].lower())
        if name.startswith('get'):
            return getXxx

        def setXxx(value):
            self.__setattr__(name[3:].lower(), value)
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
        return _cmp.VALUE_OF(self).evaluate()

    def raw_of(self):
        """Return raw part of object
        @rtype: Data"""
        return _cmp.RAW_OF(self).evaluate()

    def units_of(self):
        """Return units of object
        @rtype: Data"""
        return _cmp.UNITS_OF(self).evaluate()

    def getDimensionAt(self, idx=0):
        """Return dimension of object
        @param idx: Index of dimension
        @type idx: int
        @rtype: Data"""
        return _cmp.DIM_OF(self, idx).evaluate()
    dim_of = getDimensionAt

    @property
    def units(self):
        """units associated with this data."""
        return _cmp.UNITS(self).evaluate()

    @units.setter
    def units(self, units):
        if units is None:
            if hasattr(self, '_units'):
                delattr(self, '_units')
        else:
            self._units = units

    @property
    def error(self):
        """error property of this data."""
        return _cmp.ERROR_OF(self).evaluate()

    @error.setter
    def error(self, error):
        if error is None:
            if hasattr(self, '_error'):
                delattr(self, '_error')
        else:
            self._error = error

    @property
    def help(self):
        """help property of this node."""
        return _cmp.HELP_OF(self).evaluate()

    @help.setter
    def help(self, help):
        if help is None:
            if hasattr(self, '_help'):
                delattr(self, '_help')
        else:
            self._help = help

    @property
    def validation(self):
        """Validation property of this node"""
        return _cmp.VALIDATION_OF(self).evaluate()

    @validation.setter
    def validation(self, validation):
        if validation is None:
            if hasattr(self, '_validation'):
                delattr(self, '_validation')
        else:
            self._validation = validation

    """ binary operator methods (order: https://docs.python.org/2/library/operator.html) """
    @staticmethod
    def __bool(data):
        if _N.isscalar(data):
            return bool(data)
        return data

    if _ver.npver < (1, 8):  # e.g. on fc18 slower but more compatible
        def __lt__(self, y): return _cmp.LT(self, y).evaluate().bool()

        def __rlt__(self, y): return Data(y) < self

        def __le__(self, y): return _cmp.LE(self, y).evaluate().bool()

        def __rle__(self, y): return Data(y) <= self

        def __eq__(self, y): return _cmp.EQ(self, y).evaluate().bool()

        def __req__(self, y): return Data(y) == self

        def __ne__(self, y): return _cmp.NE(self, y).evaluate().bool()

        def __rne__(self, y): return Data(y) != self

        def __gt__(self, y): return _cmp.GT(self, y).evaluate().bool()

        def __rgt__(self, y): return Data(y) > self

        def __ge__(self, y): return _cmp.GE(self, y).evaluate().bool()

        def __rge__(self, y): return Data(y) >= self
    else:
        def __lt__(self, y): return self.__bool(self.data() < y)

        def __rlt__(self, y): return self.__bool(y < self.data())

        def __le__(self, y): return self.__bool(self.data() <= y)

        def __rle__(self, y): return self.__bool(y <= self.data())

        def __eq__(self, y): return self.__bool(self.data() == y)

        def __req__(self, y): return self.__bool(y == self.data())

        def __ne__(self, y): return self.__bool(self.data() != y)

        def __rne__(self, y): return self.__bool(y != self.data())

        def __gt__(self, y): return self.__bool(self.data() > y)

        def __rgt__(self, y): return self.__bool(y > self.data())

        def __ge__(self, y): return self.__bool(self.data() >= y)

        def __rge__(self, y): return self.__bool(y >= self.data())

    def __add__(self, y):
        return _cmp.ADD(self, y).evaluate()

    def __radd__(self, y):
        return Data(y)+self

    def __iadd__(self, y):
        self._value = (self+y)._value

    def __and__(self, y):
        return _cmp.IAND(self, y).evaluate()

    def __rand__(self, y):
        return Data(y) & self

    def __iand__(self, y):
        self._value = (self & y)._value

    def __div__(self, y):
        return _cmp.DIVIDE(self, y).evaluate()

    def __rdiv__(self, y):
        return Data(y)/self

    def __idiv__(self, y):
        self._value = (self/y)._value
    __truediv__ = __div__
    __rtruediv__ = __rdiv__
    __itruediv__ = __idiv__

    def __floordiv__(self, y):
        return _cmp.FLOOR(_cmp.DIVIDE(self, y)).evaluate()

    def __rfloordiv__(self, y):
        return Data(y)//self

    def __ifloordiv__(self, y):
        self._value = (self//y)._value

    def __lshift__(self, y):
        return _cmp.SHIFT_LEFT(self, y).evaluate()

    def __rlshift__(self, y):
        return Data(y) << self

    def __ilshift__(self, y):
        self._value = (self << y)._value

    def __mod__(self, y):
        return _cmp.MOD(self, y).evaluate()

    def __rmod__(self, y):
        return Data(y) % self

    def __imod__(self, y):
        self._value = (self % y)._value

    def __sub__(self, y):
        return _cmp.SUBTRACT(self, y).evaluate()

    def __rsub__(self, y):
        return Data(y)-self

    def __isub__(self, y):
        self._value = (self-y)._value

    def __rshift__(self, y):
        return _cmp.SHIFT_RIGHT(self, y).evaluate()

    def __rrshift__(self, y):
        return Data(y) >> self

    def __irshift__(self, y):
        self._value = (self >> y)._value

    def __mul__(self, y):
        return _cmp.MULTIPLY(self, y).evaluate()

    def __rmul__(self, y):
        return Data(y)*self

    def __imul__(self, y):
        self._value = (self*y)._value

    def __or__(self, y):
        return _cmp.IOR(self, y).evaluate()

    def __ror__(self, y):
        return Data(y) | self

    def __ior__(self, y):
        self._value = (self | y)._value

    def __pow__(self, y):
        return _cmp.POWER(self, y).evaluate()

    def __rpow__(self, y):
        return Data(y)**self

    def __ipow__(self, y):
        self._value = (self**y)._value

    def __xor__(self, y):
        return bool(self) ^ y

    def __rxor__(self, y):
        return y ^ bool(self)

    def __ixor__(self, y):
        self._value = (self ^ y)._value

    def __abs__(self):
        return _cmp.ABS(self).evaluate()

    def __invert__(self):
        return _cmp.INOT(self).evaluate()

    def __neg__(self):
        return _cmp.UNARY_MINUS(self).evaluate()

    def __pos__(self):
        return _cmp.UNARY_PLUS(self).evaluate()

    def __nonzero__(self):
        ans = self != 0
        if isinstance(ans, _N.ndarray):
            return ans.all()
        return ans

    def decompile(self):
        """Return string representation
        @rtype: string"""
        return str(_cmp.DECOMPILE(self).evaluate())

    def __getitem__(self, y):
        """Subscript: x.__getitem__(y) <==> x[y]
        @rtype: Data"""
        ans = _cmp.SUBSCRIPT(self, y).evaluate()
        if isinstance(ans, _arr.Array) and ans.shape[0] == 0:
            raise IndexError
        return ans

    def __bool__(self):
        """Return boolean
        @rtype: Bool"""
        if isinstance(self, _arr.Array):
            return self._value != 0
        elif isinstance(self, _cmp.Compound) and hasattr(self, 'value_of'):
            return self.value_of().bool()
        else:
            ans = int(self)
            return (ans & 1) == 1
    bool = __bool__

    def __int__(self):
        """Integer: x.__int__() <==> int(x)
        @rtype: int"""
        return int(_cmp.DATA(_cmp.QUADWORD(self)).evaluate()._value)
    __index__ = __int__

    def __len__(self):
        """Length: x.__len__() <==> len(x)
        @rtype: Data
        """
        return int(_cmp.SIZE(self).data())

    def __long__(self):
        """Convert this object to python long
        @rtype: long"""
        return _ver.long(_cmp.DATA(_cmp.QUADWORD(self)).evaluate()._value)

    def __float__(self):
        """Float: x.__float__() <==> float(x)
        @rtype: float"""
        return float(_cmp.DATA(_cmp.FT_FLOAT(self)).evaluate()._value)

    def __round__(self, *arg):
        """Round value to next integer: x.__round__() <==> round(x)
        @rtype: Data"""
        return round(self._value, *arg)

    def compare(self, value):
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
    def compile(*args, **kwargs):
        """Static method (routine in C++) which compiles the expression (via TdiCompile())
        and returns the object instance correspondind to the compiled expression.
        @rtype: Data
        """
        return TdiCompile(*args, **kwargs)

    @staticmethod
    def execute(*args, **kwargs):
        """Execute and expression inserting optional arguments into the expression before evaluating
        @rtype: Data"""
        return TdiExecute(*args, **kwargs)

    @staticmethodX
    def evaluate(self, **kwargs):
        """Return the result of TDI evaluate(this).
        @rtype: Data
        """
        return TdiEvaluate(self, **kwargs)

    def assignTo(self, varname):
        """Set tdi variable with this data
        @param varname: The name of the public tdi variable to create
        @type varname: string
        @rtype: Data
        @return: Returns the tdi variable
        """
        return _scr.Ident(varname).assign(self)

    @staticmethodX
    def setTdiVar(self, varname):
        """Set tdi public variable with this data
        @param varname: The name of the public tdi variable to create
        @type varname: string
        @rtype: Data
        @return: Returns new value of the tdi variable
        """
        return _cmp.PUBLIC(_scr.Ident(varname)).assign(self)

    @staticmethod
    def getTdiVar(varname):
        """Get value of tdi public variable
        @param varname: The name of the public tdi variable
        @type varname: string
        @rtype: Data"""
        try:
            return _cmp.PUBLIC(_scr.Ident(varname)).evaluate()
        except _exc.MDSplusException:
            return None

    def _str_bad_ref(self):
        return super(Data, self).__str__()

    def __str__(self):
        try:
            return _ver.tostr(self.decompile())
        except _exc.MDSplusException:
            return self._str_bad_ref()

    def __bytes__(self):
        try:
            return _ver.tobytes(self.decompile())
        except _exc.MDSplusException:
            return self._str_bad_ref()

    def __repr__(self):
        """Representation
        @rtype: String"""
        try:
            return _ver.tostr(self.decompile())
        except _exc.MDSplusException:
            return self._str_bad_ref()

    def data(self, *altvalue):
        """Return primitimive value of the data.
        @rtype: numpy or native type
        """
        try:
            data = _cmp.DATA(self).evaluate()
            return data.value if isinstance(data, Data) else data
        except _exc.TreeNODATA:
            if len(altvalue):
                return altvalue[0]
            raise

    @classmethod
    def byref(cls, data):
        if isinstance(data, (Data, _dsc.Descriptor)):
            return data.ref
        if data is None:
            return _dsc.Descriptor.null
        dsc = cls(data)._descriptor
        ptr = dsc.ptr_
        ptr.value = dsc
        return ptr

    @classmethod
    def pointer(cls, data):
        if isinstance(data, _dsc.Descriptor):
            return data.ptr_
        if isinstance(data, Data):
            return data.descriptor.ptr_
        if data is None:
            return _dsc.Descriptor.null
        dsc = cls(data)._descriptor
        ptr = dsc.ptr_
        ptr.value = dsc
        return ptr

    @property
    def ref(self):
        return self.descriptor.ref

    @staticmethod
    def _isScalar(x):
        """Is item a Scalar
        @rtype: Bool"""
        return isinstance(x, _scr.Scalar)

    def getData(self, *altvalue):
        """Return primitimive value of the data.
        @rtype: Scalar,Array
        """
        try:
            return _cmp.DATA(self).evaluate()
        except _exc.TreeNODATA:
            if len(altvalue):
                return altvalue[0]
            raise

    def getByte(self):
        """Convert this data into a byte.
        @rtype: Int8
        @raise TypeError: Raised if data is not a scalar value
        """
        ans = _cmp.BYTE(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getShort(self):
        """Convert this data into a short.
        @rtype: Int16
        @raise TypeError: Raised if data is not a scalar value
        """
        ans = _cmp.WORD(self).evaluate()
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
        ans = _cmp.LONG(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getLong(self):
        """Convert this data into a long.
        @rtype: Int64
        @raise TypeError: if data is not a scalar value
        """
        ans = _cmp.QUADWORD(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloat(self):
        """Convert this data into a float32.
        @rtype: Float32
        @raise TypeError: Raised if data is not a scalar value
        """
        ans = _cmp.FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getDouble(self):
        """Convert this data into a float64
        @rtype: Float64
        @raise TypeError: Raised if data is not a scalar value
        """
        ans = _cmp.FT_FLOAT(self).evaluate()
        if not Data._isScalar(ans):
            raise TypeError('Value not a scalar, %s' % str(type(self)))
        return ans

    def getFloatArray(self):
        """Convert this data into a float32.
        @rtype: Float32
        """
        return _cmp.FLOAT(self).evaluate()

    def getDoubleArray(self):
        """Convert this data into a float64.
        @rtype: Float64
        """
        return _cmp.FT_FLOAT(self).evaluate()

    def getShape(self):
        """Get the array dimensions as an integer array.
        @rtype: Int32Array
        """
        return _cmp.SHAPE(self).evaluate()

    def getByteArray(self):
        """Convert this data into a byte array.
        @rtype: Int8Array
        """
        return _cmp.BYTE(self).evaluate()

    def getShortArray(self):
        """Convert this data into a short array.
        @rtype: Int16Array
        """
        return _cmp.WORD(self).evaluate()

    def getIntArray(self):
        """Convert this data into a int array.
        @rtype: Int32Array
        """
        return _cmp.LONG(self).evaluate()

    def getLongArray(self):
        """Convert this data into a long array.
        @rtype: Int64Array
        """
        return _cmp.QUADWORD(self).evaluate()

    def getString(self):
        """Convert this data into a STRING. Implemented at this class level by returning
        TDI data((this)). If data() fails or the returned class is not string,
        generates an exception.
        @rtype: String
        """
        return str(_cmp.TEXT(self).evaluate())

    def hasNodeReference(self):
        """Return True if data item contains a tree reference
        @rtype: Bool
        """
        if isinstance(self, _tre.TreeNode):
            return True
        elif isinstance(self, _cmp.Compound):
            for arg in self.args:
                if isinstance(arg, Data) and arg.hasNodeReference():
                    return True
        elif isinstance(self, _apd.Apd):
            for arg in self.getDescs():
                if isinstance(arg, Data) and arg.hasNodeReference():
                    return True
        elif isinstance(self, _scr.Ident):
            return None  # treated as False but is uncertain
        return False

    def mayHaveChanged(self): return True  # deprecated

    def plot(self, title='', scope=None, row=1, col=1):
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
            scope = _mimport('scope').Scope(title)
        scope.plot(self, self.dim_of(), row, col)
        scope.show()

    def sind(self):
        """Return sin() of data assuming data is in degrees
        @rtype: Float32Array
        """
        return _cmp.SIND(self).evaluate()

    def serialize(self):
        """Return Uint8Array binary representation.
        @rtype: Uint8Array
        """
        xd = _dsc.DescriptorXD()
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
        xd = _dsc.DescriptorXD()
        _exc.checkStatus(
            _MdsShr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),
                                      xd.ref))
        return xd.value


makeData = Data


class EmptyData(Data):
    _descriptor = _dsc.DescriptorNULL
    dtype_id = 24
    """No Value aka *"""

    def __init__(self, *value): """empty"""

    def decompile(self): return "*"

    @property
    def value(self): return None

    def data(self): return None

    @staticmethod
    def fromDescriptor(d): return EmptyData


# the old API had EmptyData as instance
EmptyData = EmptyData()

# Missing should extend the class of EmptyData


class Missing(EmptyData.__class__):
    """No Value aka $Missing"""

    def decompile(self): return "$Missing"

    @staticmethod
    def fromDescriptor(d): return Missing


class DataX(Data):
    """Data with a list of childs in descs"""

    def setDescAt(self, idx, value):
        """Set descriptor at index idx (indexes start at 0)"""
        if isinstance(idx, slice):
            indices = idx.indices(self.maxdesc)  # max ndesc
            last = indices[0]+len(value)*indices[2]
            diff = 1+last-len(self._descs)
            if diff > 0:
                self._descs += [None]*diff
            self._descs[idx] = tuple(Data(val) for val in value)
            self._setTree(*value)
        else:
            if value is None:
                if len(self._descs) > idx:
                    self._descs[idx] = None
            else:
                diff = 1+idx-len(self._descs)
                if diff > 0:
                    self._descs += [None]*diff
                self._descs[idx] = Data(value)
                self._setTree(value)
        return self

    def __getitem__(self, idx):
        return self.getDescAt(idx)

    def __setitem__(self, idx, value):
        return self.setDescAt(idx, value)

    def getDescAt(self, idx):
        """Return descriptor with index idx (first descriptor is 0)
        @rtype: Data
        """
        if isinstance(idx, (slice,)):
            return self._descs[idx]
        if isinstance(idx, int) and idx < len(self.descs):
            return self._descs[idx]
        return None

    def getDescs(self):
        """Return descriptors or None if no descriptors
        @rtype: tuple,None
        """
        return self.descs

    @property
    def descs(self):
        return self._descs

    def getNumDescs(self):
        """Return number of descriptors
        @rtype: int
        """
        return len(self._descs)


class TreeRefX(TreeRef, DataX):
    """TreeRef depending on DataX child descs"""
    @property
    def tree(self):
        for desc in self._descs:
            if isinstance(desc, TreeRef):
                tree = desc.tree
                if tree is not None:
                    return tree
        return None

    @tree.setter
    def tree(self, tree):
        if not isinstance(tree, _tre.Tree):
            return
        for desc in self._descs:
            if isinstance(desc, Data):
                desc._setTree(tree)

    def __hasBadTreeReferences__(self, tree):
        for desc in self._descs:
            if isinstance(desc, TreeRef) and desc.__hasBadTreeReferences__(tree):
                return True
        return False

    def __fixTreeReferences__(self, tree):
        for idx, d in enumerate(self._descs):
            if isinstance(d, Data) and d.__hasBadTreeReferences__(tree):
                self._descs[idx] = d.__fixTreeReferences__(tree)
        return self


_dsc.dtypeToClass[0] = Missing
_dsc.dtypeToArrayClass[0] = Missing
# also dtypeToClass expects its values to be classes
_dsc.dtypeToClass[EmptyData.dtype_id] = EmptyData.__class__

_cmp = _mimport('compound')
_arr = _mimport('mdsarray')
_scr = _mimport('mdsscalar')
_tre = _mimport('tree')
_apd = _mimport('apd')
