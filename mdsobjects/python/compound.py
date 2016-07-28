import ctypes as _C

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_descriptor=_mimport('descriptor')
_data=_mimport('mdsdata')
_exceptions=_mimport('mdsExceptions')

class Compound(_data.Data):
    def __init__(self,*args, **params):
        """MDSplus compound data.
        """
        if self.__class__.__name__=='Compound':
            raise TypeError("Cannot create instances of class Compound")
        self._fields={}
        for idx in range(len(self.fields)):
            self._fields[self.fields[idx]]=idx
        if 'args' in params:
            args=params['args']
        if 'params' in params:
            params=params['params']
        try:
            self._argOffset=self.argOffset
        except:
            self._argOffset=len(self.fields)
        if isinstance(args,tuple):
            if len(args) > 0:
                if isinstance(args[0],tuple):
                    args=args[0]
        self.args=args
        for keyword in params:
            if keyword in self.fields:
                super(type(self),self).__setitem__(self._fields[keyword],params[keyword])

    def __hasBadTreeReferences__(self,tree):
        for arg in self.args:
            if isinstance(arg,_data.Data) and arg.__hasBadTreeReferences__(tree):
                return True
        return False

    def __fixTreeReferences__(self,tree):
        from copy import deepcopy
        ans = deepcopy(self)
        newargs=list(ans.args)
        for idx in range(len(newargs)):
            if isinstance(newargs[idx],_data.Data) and newargs[idx].__hasBadTreeReferences__(tree):
                newargs[idx]=newargs[idx].__fixTreeReferences__(tree)
        ans.args=tuple(newargs)
        return ans

    def __getattr__(self,name,*args):
        if name == '_fields':
            return {}
        if name in self._fields:
            try:
                return self.args[self._fields[name]]
            except:
                return None
        elif name.startswith('get') and name[3:].lower() in self._fields:
            def getter():
                return self.args[self._fields[name[3:].lower()]]
            return getter
        elif name.startswith('set') and name[3:].lower() in self._fields:
            def setter(value):
                self.__setattr__(name[3:].lower(),value)
            return setter
        raise AttributeError('No such attribute '+str(name))

    def __getitem__(self,num):
        try:
            return self.args[num]
        except:
            return None

    def __setattr__(self,name,value):
        if name in self._fields:
            tmp=list(self.args)
            while len(tmp) <= self._fields[name]:
                tmp.append(None)
            tmp[self._fields[name]]=value
            self.args=tuple(tmp)
            return
        super(Compound,self).__setattr__(name,value)


    def __setitem__(self,num,value):
        if isinstance(num,slice):
            indices=num.indices(num.start+len(value))
            idx=0
            for i in range(indices[0],indices[1],indices[2]):
                self.__setitem__(i,value[idx])
                idx=idx+1
        else:
            try:
                tmp=list(self.args)
            except:
                tmp=list()
            while len(tmp) <= num:
                tmp.append(None)
            tmp[num]=value
            self.args=tuple(tmp)
        return

    def getArgumentAt(self,idx):
        """Return argument at index idx (indexes start at 0)
        @rtype: Data,None
        """
        return Compound.__getitem__(self,idx+self._argOffset)

    def getArguments(self):
        """Return arguments
        @rtype: Data,None
        """
        return Compound.__getitem__(self,slice(self._argOffset,None))

    def getDescAt(self,idx):
        """Return descriptor with index idx (first descriptor is 0)
        @rtype: Data
        """
        return Compound.__getitem__(self,idx)

    def getDescs(self):
        """Return descriptors or None if no descriptors
        @rtype: tuple,None
        """
        return self.args

    def getNumDescs(self):
       """Return number of descriptors
       @rtype: int
       """
       try:
           return len(self.args)
       except:
           return 0

    def setArgumentAt(self,idx,value):
        """Set argument at index idx (indexes start at 0)"""
        return super(type(self),self).__setitem__(idx+self._argOffset,value)

    def setArguments(self,args):
        """Set arguments
        @type args: tuple
        """
        return super(type(self),self).__setitem__(slice(self._argOffset,None),args)

    def setDescAt(self,n,value):
        """Set descriptor at index idx (indexes start at 0)"""
        return super(type(self),self).__setitem__(n,value)

    def setDescs(self,args):
        """Set descriptors
        @type args: tuple
        """
        self.args=args

    @staticmethod
    def descriptorWithProps(value,d):
        dpt=_C.POINTER(_descriptor.Descriptor)
        try:
            units_d=_data.makeData(value._units).descriptor
        except AttributeError:
            units_d=None
        if units_d is not None:
            dunits=_descriptor.Descriptor_r()
            dunits.length=0
            dunits.dtype=WithUnits.dtype_id
            dunits.pointer=_C.c_void_p(0)
            dunits.ndesc=2
            dunits.dscptrs[0]=_C.cast(_C.pointer(d),dpt)
            dunits.dscptrs[1]=_C.cast(_C.pointer(units_d),dpt)
            dunits.original=d
        else:
            dunits=d
        try:
            error_d=_data.makeData(value._error).descriptor
        except AttributeError:
            error_d=None
        if error_d is not None:
            derror=_descriptor.Descriptor_r()
            derror.length=0
            derror.dtype=WithError.dtype_id
            derror.pointer=_C.c_void_p(0)
            derror.ndesc=2
            derror.dscptrs[0]=_C.cast(_C.pointer(dunits),dpt)
            derror.dscptrs[1]=_C.cast(_C.pointer(error_d),dpt)
            derror.original=dunits
        else:
            derror=dunits
        try:
            help_d=_data.makeData(value._help).descriptor
        except AttributeError:
            help_d=None
        try:
            validation_d=_data.makeData(value._validation).descriptor
        except AttributeError:
            validation_d=None
        if help_d is not None or validation_d is not None:
            dparam=_descriptor.Descriptor_r()
            dparam.length=0
            dparam.dtype=Parameter.dtype_id
            dparam.pointer=_C.c_void_p(0)
            dparam.ndesc=3
            dparam.dscptrs[0]=_C.cast(_C.pointer(derror),dpt)
            if help_d is not None:
                dparam.dscptrs[1]=_C.cast(_C.pointer(help_d),dpt)
            else:
                dparam.dscptrs[1]=_C.cast(_C.c_void_p(0),dpt)
            if validation_d is not None:
                dparam.dscptrs[2]=_C.cast(_C.pointer(validation_d),dpt)
            else:
                dparam.dscptrs[2]=_C.cast(_C.c_void_p(0),dpt)
            dparam.original=derror
        else:
            dparam=derror
        return dparam

    @property
    def descriptor(self):
        d=_descriptor.Descriptor_r()
        try:
            opcode=self.opcode
        except AttributeError:
            opcode=None
        if opcode is None:
            d.length=0
            d.pointer=_C.c_void_p(0)
        else:
            d.length=2
            d.pointer=_C.cast(_C.pointer(_C.c_uint16(opcode)),_C.c_void_p)
        d.dtype=self.dtype_id
        d.ndesc=len(self.args)
        for idx in range(len(self.args)):
            if self.args[idx] is None:
                d.dscptrs[idx]=_C.cast(_C.c_void_p(0),type(d.dscptrs[idx]))
            else:
                d.dscptrs[idx]=_C.cast(_C.pointer(_data.makeData(self.args[idx]).descriptor),_C.POINTER(_descriptor.Descriptor))
        d.original=self
        if self._units or self._error is not None or self._help is not None or self._validation is not None:
            return self.descriptorWithProps(self,d)
        else:
            return d

    @classmethod
    def fromDescriptor(cls,d):
        args=[]
        for i in range(d.ndesc):
            if _C.cast(d.dscptrs[i],_C.c_void_p).value is None:
                args.append(None)
            else:
                args.append(d.dscptrs[i].contents.value)
        args=tuple(args)
        ans=cls(args=args)
        if d.length == 2:
            ans.opcode=_C.cast(d.pointer,_C.POINTER(_C.c_uint16)).contents.value
        return ans
            
class Action(Compound):
    """
    An Action is used for describing an operation to be performed by an
    MDSplus action server. Actions are typically dispatched using the
    mdstcl DISPATCH command
    """
    fields=('dispatch','task','errorLog','completionMessage','performance')
    dtype_id=202

class Call(Compound):
    """
    A Call is used to call routines in shared libraries.
    """
    fields=('image','routine')
    dtype_id=212

class Conglom(Compound):
    """A Conglom is used at the head of an MDSplus conglomerate. A conglomerate is a set of tree nodes used
    to define a device such as a piece of data acquisition hardware. A conglomerate is associated with some
    external code providing various methods which can be performed on the device. The Conglom class contains
    information used for locating the external code.
    """
    fields=('image','model','name','qualifiers')
    dtype_id=200
    def getClass(self, ):
        if not self.image=='__python__':
            raise Exception('Conglom does not represent a python class.')
        model = str(self.model)
        safe_env = {}
        qualifiers = self.qualifiers.value.tolist()
        if isinstance(qualifiers,list): qualifiers = ';'.join(qualifiers)  # make it a list of statements
        exec(compile(qualifiers,'<string>','exec')) in safe_env
        if not model in safe_env:
            raise _exceptions.DevPYDEVICE_NOT_FOUND
        return safe_env[model]

class Dependency(Compound):
    """A Dependency object is used to describe action dependencies. This is a legacy class and may not be recognized by
    some dispatching systems
    """
    fields=('arg1','arg2')
    dtype_id=208

class Dimension(Compound):
    """A dimension object is used to describe a signal dimension, typically a time axis. It provides a compact description
    of the timing information of measurements recorded by devices such as transient recorders. It associates a Window
    object with an axis. The axis is generally a range with possibly no start or end but simply a delta. The Window
    object is then used to bracket the axis to resolve the appropriate timestamps.
    """
    fields=('window','axis')
    dtype_id=196

class Dispatch(Compound):
    """A Dispatch object is used to describe when an where an action should be dispatched to an MDSplus action server.
    """
    fields=('ident','phase','when','completion')
    dtype_id=203

    def __init__(self,*args,**kwargs):
        if 'dispatch_type' in kwargs:
            self.opcode=kwargs['dispatch_type']
        else:
            self.opcode=2
        super(_Dispatch,self).__init__(args=args)
        if self.completion is None:
           self.completion = None

class Function(Compound):
    """A Function object is used to reference builtin MDSplus functions. For example the expression 1+2
    is represented in as Function instance created by Function(opcode='ADD',args=(1,2))
    """
    _fields={}
    dtype_id=199

    @classmethod
    def fromDescriptor(cls,d):
        opc=_C.cast(d.pointer,_C.POINTER(_C.c_uint16)).contents.value
        args=[]
        for i in range(d.ndesc):
            if _C.cast(d.dscptrs[i],_C.c_void_p).value is None:
                args.append(None)
            else:
                args.append(d.dscptrs[i].contents.value)
        args=tuple(args)
        return _opcodeToClass[opc](args)

    def __init__(self,*args):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        fargs=[]
        if len(args) == 1 and isinstance(args[0],tuple):
            args_in=args[0]
        else:
            args_in=args
        for arg in args_in:
            fargs.append(_data.makeData(arg))
        self.args=tuple(fargs)

class Method(Compound):
    """A Method object is used to describe an operation to be performed on an MDSplus conglomerate/device
    """
    fields=('timeout','method','object')
    dtype_id=207

class Procedure(Compound):
    """A Procedure is a deprecated object
    """
    fields=('timeout','language','procedure')
    dtype_id=206

class Program(Compound):
    """A Program is a deprecated object"""
    fields=('timeout','program')
    dtype_id=204

class Range(Compound):
    """A Range describes a ramp. When used as an axis in a Dimension object along with a Window object it can be
    used to describe a clock. In this context it is possible to have missing begin and ending values or even have the
    begin, ending, and delta fields specified as arrays to indicate a multi-speed clock.
    """
    fields=('begin','ending','delta')
    dtype_id=201

    def decompile(self):
        parts=list()
        for arg in self.args:
            parts.append(_data.makeData(arg).decompile())
        return ' : '.join(parts)

class Routine(Compound):
    """A Routine is a deprecated object"""
    fields=('timeout','image','routine')
    dtype_id=205

class Signal(Compound):
    """A Signal is used to describe a measurement, usually time dependent, and associated the data with its independent
    axis (Dimensions). When Signals are indexed using s[idx], the index is resolved using the dimension of the signal
    """
    fields=('value','raw')
    dtype_id=195

    @property
    def dims(self):
        """The dimensions of the signal"""
        return self.getArguments()

    def dim_of(self,idx=0):
        """Return the signals dimension
        @rtype: Data
        """
        if idx < len(self.dims):
            return self.dims[idx]
        else:
            return _data.makeData(None)

    def __getitem__(self,idx):
        """Subscripting <==> signal[subscript]. Uses the dimension information for subscripting
        @param idx: index or Range used for subscripting the signal based on the signals dimensions
        @type idx: Data
        @rtype: Signal
        """
        if isinstance(idx,slice):
          idx = Range(idx.start,idx.stop,idx.step)
        return _data.Data.execute('$[$]',self,idx)

    def getDimensionAt(self,idx=0):
        """Return the dimension of the signal
        @param idx: The index of the desired dimension. Indexes start at 0. 0=default
        @type idx: int
        @rtype: Data
        """
        try:
            return self.dims[idx]
        except:
            return None

    def getDimensions(self):
        """Return all the dimensions of the signal
        @rtype: tuple
        """
        return self.dims

    def setDimensionAt(self,idx,value):
        """Set the dimension
        @param idx: The index into the dimensions of the signal.
        @rtype: None
        """
        return self.setArgumentAt(idx,value)

    def setDimensions(self,value):
        """Set all the dimensions of a signal
        @param value: The dimensions
        @type value: tuple
        @rtype: None
        """
        return self.setArguments(value)

class Window(Compound):
    """A Window object can be used to construct a Dimension object. It brackets the axis information stored in the
    Dimension to construct the independent axis of a signal.
    """
    fields=('startIdx','endIdx','timeAt0')
    dtype_id=197

class Opaque(Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('data','otype')
    dtype_id=217


    def getImage(self):
      import Image
      from StringIO import StringIO
      return Image.open(StringIO(_data.makeData(self.getData()).data().data))

    def fromFile(filename,typestring):
      """Read a file and return an Opaque object
         @param filename: Name of file to read in
         @type filename: str
         @param typestring: String to denote the type of file being stored
         @type typestring: str
         @rtype: Opaque instance
      """
      import numpy as _N
      f = open(filename,'rb')
      try:
        opq=Opaque(_data.makeData(_N.fromstring(f.read(),dtype="uint8")),typestring)
      finally:
        f.close()
      return opq
    fromFile=staticmethod(fromFile)

class WithUnits(Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('data','units')
    dtype_id=211

class WithError(Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('data','error')
    dtype_id=211

class Parameter(Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('data','help','validation')
    dtype_id=194


_descriptor.dtypeToClass[Action.dtype_id]=Action
_descriptor.dtypeToClass[Call.dtype_id]=Call
_descriptor.dtypeToClass[Conglom.dtype_id]=Conglom
_descriptor.dtypeToClass[Dependency.dtype_id]=Dependency
_descriptor.dtypeToClass[Dimension.dtype_id]=Dimension
_descriptor.dtypeToClass[Dispatch.dtype_id]=Dispatch
_descriptor.dtypeToClass[Function.dtype_id]=Function
_descriptor.dtypeToClass[Method.dtype_id]=Method
_descriptor.dtypeToClass[Procedure.dtype_id]=Procedure
_descriptor.dtypeToClass[Program.dtype_id]=Program
_descriptor.dtypeToClass[Range.dtype_id]=Range
_descriptor.dtypeToClass[Routine.dtype_id]=Routine
_descriptor.dtypeToClass[Signal.dtype_id]=Signal
_descriptor.dtypeToClass[Window.dtype_id]=Window
_descriptor.dtypeToClass[Opaque.dtype_id]=Opaque
_descriptor.dtypeToClass[WithError.dtype_id]=WithError
_descriptor.dtypeToClass[WithUnits.dtype_id]=WithUnits
_descriptor.dtypeToClass[Parameter.dtype_id]=Parameter

class dPLACEHOLDER(Function):
    min_args=0
    max_args=0
    opcode=0

class dA0(Function):
    max_args=0
    opcode=1

class dALPHA(Function):
    max_args=0
    opcode=2

class dAMU(Function):
    max_args=0
    opcode=3

class dC(Function):
    max_args=0
    opcode=4

class dCAL(Function):
    max_args=0
    opcode=5

class dDEGREE(Function):
    max_args=0
    opcode=6

class dEV(Function):
    max_args=0
    opcode=7

class dFALSE(Function):
    max_args=0
    opcode=8

class dFARADAY(Function):
    max_args=0
    opcode=9

class dG(Function):
    max_args=0
    opcode=10

class dGAS(Function):
    max_args=0
    opcode=11

class dH(Function):
    max_args=0
    opcode=12

class dHBAR(Function):
    min_args=0
    max_args=0
    opcode=13

class dI(Function):
    max_args=0
    opcode=14

class dK(Function):
    max_args=0
    opcode=15

class dME(Function):
    max_args=0
    opcode=16

class dMISSING(Function):
    max_args=0
    opcode=17

class dMP(Function):
    max_args=0
    opcode=18

class dN0(Function):
    max_args=0
    opcode=19

class dNA(Function):
    max_args=0
    opcode=20

class dP0(Function):
    max_args=0
    opcode=21

class dPI(Function):
    max_args=0
    opcode=22

class dQE(Function):
    max_args=0
    opcode=23

class dRE(Function):
    max_args=0
    name='$RE'
    opcode=24

class dROPRAND(Function):
    max_args=0
    opcode=25

class dRYDBERG(Function):
    max_args=0
    opcode=26

class dT0(Function):
    max_args=0
    name='$T0'
    opcode=27

class dTORR(Function):
    max_args=0
    opcode=28

class dTRUE(Function):
    max_args=0
    opcode=29

class dVALUE(Function):
    min_args=0
    max_args=0
    opcode=30

class ABORT(Function):
    min_args=0
    max_args=255
    opcode=31

class ABS(Function):
    min_args=1
    max_args=1
    opcode=32

class ABS1(Function):
    min_args=1
    max_args=1
    opcode=33

class ABSSQ(Function):
    min_args=1
    max_args=1
    opcode=34

class ACHAR(Function):
    min_args=1
    max_args=2
    opcode=35

class ACOS(Function):
    min_args=1
    max_args=1
    opcode=36

class ACOSD(Function):
    min_args=1
    max_args=1
    opcode=37

class ADD(Function):
    min_args=2
    max_args=2
    opcode=38

class ADJUSTL(Function):
    min_args=1
    max_args=1
    opcode=39

class ADJUSTR(Function):
    min_args=1
    max_args=1
    opcode=40

class AIMAG(Function):
    min_args=1
    max_args=1
    opcode=41

class AINT(Function):
    min_args=1
    max_args=2
    opcode=42

class ALL(Function):
    min_args=1
    max_args=2
    opcode=43

class ALLOCATED(Function):
    min_args=1
    max_args=1
    opcode=44

class AND(Function):
    min_args=2
    max_args=2
    opcode=45

class AND_NOT(Function):
    min_args=2
    max_args=2
    opcode=46

class ANINT(Function):
    min_args=1
    max_args=2
    opcode=47

class ANY(Function):
    min_args=1
    max_args=2
    opcode=48

class ARG(Function):
    min_args=1
    max_args=1
    opcode=49

class ARGD(Function):
    min_args=1
    max_args=1
    opcode=50

class ARG_OF(Function):
    min_args=1
    max_args=2
    opcode=51

class ARRAY(Function):
    min_args=0
    max_args=2
    opcode=52

class ASIN(Function):
    min_args=1
    max_args=1
    opcode=53

class ASIND(Function):
    min_args=1
    max_args=1
    opcode=54

class AS_IS(Function):
    min_args=1
    max_args=1
    opcode=55

class ATAN(Function):
    min_args=1
    max_args=1
    name='ATAN'
    opcode=56
ATAN.__name__='ATAN'

class ATAN2(Function):
    min_args=2
    max_args=2
    opcode=57

class ATAN2D(Function):
    min_args=2
    max_args=2
    opcode=58

class ATAND(Function):
    min_args=1
    max_args=1
    opcode=59

class ATANH(Function):
    min_args=1
    max_args=1
    opcode=60

class AXIS_OF(Function):
    min_args=1
    max_args=1
    opcode=61

class BACKSPACE(Function):
    min_args=1
    max_args=1
    opcode=62

class IBCLR(Function):
    min_args=2
    max_args=2
    opcode=63

class BEGIN_OF(Function):
    min_args=1
    max_args=2
    opcode=64

class IBITS(Function):
    min_args=3
    max_args=3
    opcode=65

class BREAK(Function):
    min_args=0
    max_args=0
    opcode=66

class BSEARCH(Function):
    min_args=2
    max_args=4
    opcode=67

class IBSET(Function):
    min_args=2
    max_args=2
    opcode=68

class BTEST(Function):
    min_args=2
    max_args=2
    opcode=69

class BUILD_ACTION(Function):
    min_args=2
    max_args=5
    opcode=70

class BUILD_CONDITION(Function):
    min_args=2
    max_args=2
    opcode=71

class BUILD_CONGLOM(Function):
    min_args=4
    max_args=4
    opcode=72

class BUILD_DEPENDENCY(Function):
    min_args=3
    max_args=3
    opcode=73

class BUILD_DIM(Function):
    min_args=2
    max_args=2
    opcode=74

class BUILD_DISPATCH(Function):
    min_args=5
    max_args=5
    opcode=75

class BUILD_EVENT(Function):
    min_args=1
    max_args=1
    opcode=76

class BUILD_FUNCTION(Function):
    min_args=1
    max_args=254
    opcode=77

class BUILD_METHOD(Function):
    min_args=3
    max_args=254
    opcode=78

class BUILD_PARAM(Function):
    min_args=3
    max_args=3
    opcode=79

class BUILD_PATH(Function):
    min_args=1
    max_args=1
    opcode=80

class BUILD_PROCEDURE(Function):
    min_args=3
    max_args=254
    opcode=81

class BUILD_PROGRAM(Function):
    min_args=2
    max_args=2
    opcode=82

class BUILD_RANGE(Function):
    min_args=2
    max_args=3
    opcode=83

class BUILD_ROUTINE(Function):
    min_args=3
    max_args=254
    opcode=84

class BUILD_SIGNAL(Function):
    min_args=2
    max_args=10
    opcode=85

class BUILD_SLOPE(Function):
    min_args=1
    max_args=254
    opcode=86

class BUILD_WINDOW(Function):
    min_args=3
    max_args=3
    opcode=87

class BUILD_WITH_UNITS(Function):
    min_args=2
    max_args=2
    opcode=88

class BUILTIN_OPCODE(Function):
    min_args=1
    max_args=1
    opcode=89

class BYTE(Function):
    min_args=1
    max_args=1
    opcode=90

class BYTE_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=91

class CASE(Function):
    min_args=2
    max_args=254
    opcode=92

class CEILING(Function):
    min_args=1
    max_args=1
    opcode=93

class CHAR(Function):
    min_args=1
    max_args=2
    opcode=94

class CLASS(Function):
    min_args=1
    max_args=1
    opcode=95

class FCLOSE(Function):
    min_args=1
    max_args=1
    opcode=96

class CMPLX(Function):
    min_args=1
    max_args=3
    opcode=97

class COMMA(Function):
    min_args=2
    max_args=254
    opcode=98

class COMPILE(Function):
    min_args=1
    max_args=254
    opcode=99

class COMPLETION_OF(Function):
    min_args=1
    max_args=1
    opcode=100

class CONCAT(Function):
    min_args=2
    max_args=254
    opcode=101

class CONDITIONAL(Function):
    min_args=3
    max_args=3
    opcode=102

class CONJG(Function):
    min_args=1
    max_args=1
    opcode=103

class CONTINUE(Function):
    min_args=0
    max_args=0
    opcode=104

class CONVOLVE(Function):
    min_args=2
    max_args=2
    opcode=105

class COS(Function):
    min_args=1
    max_args=1
    opcode=106

class COSD(Function):
    min_args=1
    max_args=1
    opcode=107

class COSH(Function):
    min_args=1
    max_args=1
    opcode=108

class COUNT(Function):
    min_args=1
    max_args=2
    opcode=109

class CSHIFT(Function):
    min_args=2
    max_args=3
    opcode=110

class CVT(Function):
    min_args=2
    max_args=2
    opcode=111

class DATA(Function):
    min_args=1
    max_args=1
    opcode=112

class DATE_AND_TIME(Function):
    min_args=0
    max_args=1
    opcode=113

class DATE_TIME(Function):
    min_args=0
    max_args=1
    opcode=114

class DBLE(Function):
    min_args=1
    max_args=1
    opcode=115

class DEALLOCATE(Function):
    min_args=0
    max_args=254
    opcode=116

class DEBUG(Function):
    min_args=0
    max_args=1
    opcode=117

class DECODE(Function):
    min_args=1
    max_args=2
    opcode=118

class DECOMPILE(Function):
    min_args=1
    max_args=2
    opcode=119

class DECOMPRESS(Function):
    min_args=4
    max_args=4
    opcode=120

class DEFAULT(Function):
    min_args=1
    max_args=254
    opcode=121

class DERIVATIVE(Function):
    min_args=2
    max_args=3
    opcode=122

class DESCR(Function):
    min_args=1
    max_args=1
    opcode=123

class DIAGONAL(Function):
    min_args=1
    max_args=2
    opcode=124

class DIGITS(Function):
    min_args=1
    max_args=1
    name='DIGITS'
    opcode=125

class DIM(Function):
    min_args=2
    max_args=2
    opcode=126

class DIM_OF(Function):
    min_args=1
    max_args=2
    opcode=127

class DISPATCH_OF(Function):
    min_args=1
    max_args=1
    opcode=128

class DIVIDE(Function):
    min_args=2
    max_args=2
    opcode=129

class LBOUND(Function):
    min_args=1
    max_args=2
    opcode=130

class DO(Function):
    min_args=2
    max_args=254
    opcode=131

class DOT_PRODUCT(Function):
    min_args=2
    max_args=2
    opcode=132

class DPROD(Function):
    min_args=2
    max_args=2
    opcode=133

class DSCPTR(Function):
    min_args=1
    max_args=2
    opcode=134

class SHAPE(Function):
    min_args=1
    max_args=2
    opcode=135

class SIZE(Function):
    min_args=1
    max_args=2
    opcode=136

class KIND(Function):
    min_args=1
    max_args=1
    opcode=137

class UBOUND(Function):
    min_args=1
    max_args=2
    opcode=138

class D_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=139

class D_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=140

class RANGE(Function):
    min_args=1
    max_args=1
    opcode=141

class PRECISION(Function):
    min_args=1
    max_args=1
    opcode=142

class ELBOUND(Function):
    min_args=1
    max_args=2
    opcode=143

class ELSE(Function):
    min_args=0
    max_args=0
    opcode=144

class ELSEWHERE(Function):
    min_args=0
    max_args=0
    opcode=145

class ENCODE(Function):
    min_args=1
    max_args=254
    opcode=146

class ENDFILE(Function):
    min_args=1
    max_args=1
    opcode=147

class END_OF(Function):
    min_args=1
    max_args=2
    opcode=148

class EOSHIFT(Function):
    min_args=3
    max_args=4
    opcode=149

class EPSILON(Function):
    min_args=1
    max_args=1
    opcode=150

class EQ(Function):
    min_args=2
    max_args=2
    opcode=151

class EQUALS(Function):
    min_args=2
    max_args=2
    opcode=152

class EQUALS_FIRST(Function):
    min_args=1
    max_args=1
    opcode=153

class EQV(Function):
    min_args=2
    max_args=2
    opcode=154

class ESHAPE(Function):
    min_args=1
    max_args=2
    opcode=155

class ESIZE(Function):
    min_args=1
    max_args=2
    opcode=156

class EUBOUND(Function):
    min_args=1
    max_args=2
    opcode=157

class EVALUATE(Function):
    min_args=1
    max_args=1
    opcode=158

class EXECUTE(Function):
    min_args=1
    max_args=254
    opcode=159

class EXP(Function):
    min_args=1
    max_args=1
    opcode=160

class EXPONENT(Function):
    min_args=1
    max_args=1
    opcode=161

class EXT_FUNCTION(Function):
    min_args=2
    max_args=254
    opcode=162

class FFT(Function):
    min_args=2
    max_args=2
    opcode=163

class FIRSTLOC(Function):
    min_args=1
    max_args=2
    opcode=164

class FIT(Function):
    min_args=2
    max_args=2
    opcode=165

class FIX_ROPRAND(Function):
    min_args=2
    max_args=2
    opcode=166

class FLOAT(Function):
    min_args=1
    max_args=2
    opcode=167

class FLOOR(Function):
    min_args=1
    max_args=1
    opcode=168

class FOR(Function):
    min_args=4
    max_args=254
    opcode=169

class FRACTION(Function):
    min_args=1
    max_args=1
    opcode=170

class FUN(Function):
    min_args=2
    max_args=254
    opcode=171

class F_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=172

class F_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=173

class GE(Function):
    min_args=2
    max_args=2
    opcode=174

class GETNCI(Function):
    min_args=2
    max_args=3
    opcode=175

class GOTO(Function):
    min_args=1
    max_args=1
    opcode=176

class GT(Function):
    min_args=2
    max_args=2
    opcode=177

class G_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=178

class G_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=179

class HELP_OF(Function):
    min_args=1
    max_args=1
    opcode=180

class HUGE(Function):
    min_args=1
    max_args=1
    opcode=181

class H_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=182

class H_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=183

class IACHAR(Function):
    min_args=1
    max_args=1
    opcode=184

class IAND(Function):
    min_args=2
    max_args=2
    opcode=185

class IAND_NOT(Function):
    min_args=2
    max_args=2
    opcode=186

class ICHAR(Function):
    min_args=1
    max_args=1
    opcode=187

class IDENT_OF(Function):
    min_args=1
    max_args=1
    opcode=188

class IF(Function):
    min_args=2
    max_args=3
    opcode=189

class IF_ERROR(Function):
    min_args=1
    max_args=254
    opcode=190

class IMAGE_OF(Function):
    min_args=1
    max_args=1
    opcode=191

class IN(Function):
    min_args=1
    max_args=1
    name='IN'
    opcode=192
IN.__name__='IN'

class INAND(Function):
    min_args=2
    max_args=2
    name='INAND'
    opcode=193
INAND.__name__='INAND'

class INAND_NOT(Function):
    min_args=2
    max_args=2
    name='INAND_NOT'
    opcode=194
INAND_NOT.__name__='INAND_NOT'

class INDEX(Function):
    min_args=2
    max_args=3
    name='INDEX'
    opcode=195
INDEX.__name__='INDEX'

class INOR(Function):
    min_args=2
    max_args=2
    name='INOR'
    opcode=196
INOR.__name__='INOR'

class INOR_NOT(Function):
    min_args=2
    max_args=2
    name='INOR_NOT'
    opcode=197
INOR_NOT.__name__='INOR_NOT'

class INOT(Function):
    min_args=1
    max_args=1
    name='INOT'
    opcode=198
INOT.__name__='INOT'

class INOUT(Function):
    min_args=1
    max_args=1
    name='INOUT'
    opcode=199
INOUT.__name__='INOUT'

class INQUIRE(Function):
    min_args=2
    max_args=2
    name='INQUIRE'
    opcode=200
INQUIRE.__name__='INQUIRE'

class INT(Function):
    min_args=1
    max_args=2
    name='INT'
    opcode=201
INT.__name__='INT'

class INTEGRAL(Function):
    min_args=2
    max_args=3
    name='INTEGRAL'
    opcode=202
INTEGRAL.__name__='INTEGRAL'

class INTERPOL(Function):
    min_args=2
    max_args=3
    name='INTERPOL'
    opcode=203
INTERPOL.__name__='INTERPOL'

class INTERSECT(Function):
    min_args=0
    max_args=254
    name='INTERSECT'
    opcode=204
INTERSECT.__name__='INTERSECT'

class INT_UNSIGNED(Function):
    min_args=1
    max_args=1
    name='INT_UNSIGNED'
    opcode=205
INT_UNSIGNED.__name__='INT_UNSIGNED'

class INVERSE(Function):
    min_args=1
    max_args=1
    name='INVERSE'
    opcode=206
INVERSE.__name__='INVERSE'

class IOR(Function):
    min_args=2
    max_args=2
    name='IOR'
    opcode=207
IOR.__name__='IOR'

class IOR_NOT(Function):
    min_args=2
    max_args=2
    name='IOR_NOT'
    opcode=208
IOR_NOT.__name__='IOR_NOT'

class IS_IN(Function):
    min_args=2
    max_args=3
    name='IS_IN'
    opcode=209
IS_IN.__name__='IS_IN'

class IEOR(Function):
    min_args=2
    max_args=2
    name='IEOR'
    opcode=210
IEOR.__name__='IEOR'

class IEOR_NOT(Function):
    min_args=2
    max_args=2
    name='IEOR_NOT'
    opcode=211
IEOR_NOT.__name__='IEOR_NOT'

class LABEL(Function):
    min_args=2
    max_args=254
    name='LABEL'
    opcode=212
LABEL.__name__='LABEL'

class LAMINATE(Function):
    min_args=2
    max_args=254
    name='LAMINATE'
    opcode=213
LAMINATE.__name__='LAMINATE'

class LANGUAGE_OF(Function):
    min_args=1
    max_args=1
    name='LANGUAGE_OF'
    opcode=214
LANGUAGE_OF.__name__='LANGUAGE_OF'

class LASTLOC(Function):
    min_args=1
    max_args=2
    name='LASTLOC'
    opcode=215
LASTLOC.__name__='LASTLOC'

class LE(Function):
    min_args=2
    max_args=2
    name='LE'
    opcode=216
LE.__name__='LE'

class LEN(Function):
    min_args=1
    max_args=1
    name='LEN'
    opcode=217
LEN.__name__='LEN'

class LEN_TRIM(Function):
    min_args=1
    max_args=1
    name='LEN_TRIM'
    opcode=218
LEN_TRIM.__name__='LEN_TRIM'

class LGE(Function):
    min_args=2
    max_args=2
    name='LGE'
    opcode=219
LGE.__name__='LGE'

class LGT(Function):
    min_args=2
    max_args=2
    name='LGT'
    opcode=220
LGT.__name__='LGT'

class LLE(Function):
    min_args=2
    max_args=2
    name='LLE'
    opcode=221
LLE.__name__='LLE'

class LLT(Function):
    min_args=2
    max_args=2
    name='LLT'
    opcode=222
LLT.__name__='LLT'

class LOG(Function):
    min_args=1
    max_args=1
    name='LOG'
    opcode=223
LOG.__name__='LOG'

class LOG10(Function):
    min_args=1
    max_args=1
    name='LOG10'
    opcode=224
LOG10.__name__='LOG10'

class LOG2(Function):
    min_args=1
    max_args=1
    name='LOG2'
    opcode=225
LOG2.__name__='LOG2'

class LOGICAL(Function):
    min_args=1
    max_args=2
    name='LOGICAL'
    opcode=226
LOGICAL.__name__='LOGICAL'

class LONG(Function):
    min_args=1
    max_args=1
    name='LONG'
    opcode=227
LONG.__name__='LONG'

class LONG_UNSIGNED(Function):
    min_args=1
    max_args=1
    name='LONG_UNSIGNED'
    opcode=228
LONG_UNSIGNED.__name__='LONG_UNSIGNED'

class LT(Function):
    min_args=2
    max_args=2
    name='LT'
    opcode=229
LT.__name__='LT'

class MATMUL(Function):
    min_args=2
    max_args=2
    name='MATMUL'
    opcode=230
MATMUL.__name__='MATMUL'

class MAT_ROT(Function):
    min_args=2
    max_args=5
    name='MAT_ROT'
    opcode=231
MAT_ROT.__name__='MAT_ROT'

class MAT_ROT_INT(Function):
    min_args=2
    max_args=5
    name='MAT_ROT_INT'
    opcode=232
MAT_ROT_INT.__name__='MAT_ROT_INT'

class MAX(Function):
    min_args=2
    max_args=254
    name='MAX'
    opcode=233
MAX.__name__='MAX'

class MAXEXPONENT(Function):
    min_args=1
    max_args=1
    name='MAXEXPONENT'
    opcode=234
MAXEXPONENT.__name__='MAXEXPONENT'

class MAXLOC(Function):
    min_args=1
    max_args=3
    name='MAXLOC'
    opcode=235
MAXLOC.__name__='MAXLOC'

class MAXVAL(Function):
    min_args=1
    max_args=3
    name='MAXVAL'
    opcode=236
MAXVAL.__name__='MAXVAL'

class MEAN(Function):
    min_args=1
    max_args=3
    name='MEAN'
    opcode=237
MEAN.__name__='MEAN'

class MEDIAN(Function):
    min_args=2
    max_args=2
    name='MEDIAN'
    opcode=238
MEDIAN.__name__='MEDIAN'

class MERGE(Function):
    min_args=3
    max_args=3
    name='MERGE'
    opcode=239
MERGE.__name__='MERGE'

class METHOD_OF(Function):
    min_args=1
    max_args=1
    name='METHOD_OF'
    opcode=240
METHOD_OF.__name__='METHOD_OF'

class MIN(Function):
    min_args=2
    max_args=254
    name='MIN'
    opcode=241
MIN.__name__='MIN'

class MINEXPONENT(Function):
    min_args=1
    max_args=1
    name='MINEXPONENT'
    opcode=242
MINEXPONENT.__name__='MINEXPONENT'

class MINLOC(Function):
    min_args=1
    max_args=3
    name='MINLOC'
    opcode=243
MINLOC.__name__='MINLOC'

class MINVAL(Function):
    min_args=1
    max_args=3
    name='MINVAL'
    opcode=244
MINVAL.__name__='MINVAL'

class MOD(Function):
    min_args=2
    max_args=2
    name='MOD'
    opcode=245
MOD.__name__='MOD'

class MODEL_OF(Function):
    min_args=1
    max_args=1
    name='MODEL_OF'
    opcode=246
MODEL_OF.__name__='MODEL_OF'

class MULTIPLY(Function):
    min_args=2
    max_args=2
    name='MULTIPLY'
    opcode=247
MULTIPLY.__name__='MULTIPLY'

class NAME_OF(Function):
    min_args=1
    max_args=1
    name='NAME_OF'
    opcode=248
NAME_OF.__name__='NAME_OF'

class NAND(Function):
    min_args=2
    max_args=2
    name='NAND'
    opcode=249
NAND.__name__='NAND'

class NAND_NOT(Function):
    min_args=2
    max_args=2
    name='NAND_NOT'
    opcode=250
NAND_NOT.__name__='NAND_NOT'

class NDESC(Function):
    min_args=1
    max_args=1
    name='NDESC'
    opcode=251
NDESC.__name__='NDESC'

class NE(Function):
    min_args=2
    max_args=2
    name='NE'
    opcode=252
NE.__name__='NE'

class NEAREST(Function):
    min_args=2
    max_args=2
    name='NEAREST'
    opcode=253
NEAREST.__name__='NEAREST'

class NEQV(Function):
    min_args=2
    max_args=2
    name='NEQV'
    opcode=254
NEQV.__name__='NEQV'

class NINT(Function):
    min_args=1
    max_args=2
    name='NINT'
    opcode=255
NINT.__name__='NINT'

class NOR(Function):
    min_args=2
    max_args=2
    name='NOR'
    opcode=256
NOR.__name__='NOR'

class NOR_NOT(Function):
    min_args=2
    max_args=2
    name='NOR_NOT'
    opcode=257
NOR_NOT.__name__='NOR_NOT'

class NOT(Function):
    min_args=1
    max_args=1
    name='NOT'
    opcode=258
NOT.__name__='NOT'

class OBJECT_OF(Function):
    min_args=1
    max_args=1
    name='OBJECT_OF'
    opcode=259
OBJECT_OF.__name__='OBJECT_OF'

class OCTAWORD(Function):
    min_args=1
    max_args=1
    name='OCTAWORD'
    opcode=260
OCTAWORD.__name__='OCTAWORD'

class OCTAWORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    name='OCTAWORD_UNSIGNED'
    opcode=261
OCTAWORD_UNSIGNED.__name__='OCTAWORD_UNSIGNED'

class ON_ERROR(Function):
    min_args=1
    max_args=1
    name='ON_ERROR'
    opcode=262
ON_ERROR.__name__='ON_ERROR'

class OPCODE_BUILTIN(Function):
    min_args=1
    max_args=1
    name='OPCODE_BUILTIN'
    opcode=263
OPCODE_BUILTIN.__name__='OPCODE_BUILTIN'

class OPCODE_STRING(Function):
    min_args=1
    max_args=1
    name='OPCODE_STRING'
    opcode=264
OPCODE_STRING.__name__='OPCODE_STRING'

class FOPEN(Function):
    min_args=2
    max_args=254
    name='FOPEN'
    opcode=265
FOPEN.__name__='FOPEN'

class OPTIONAL(Function):
    min_args=1
    max_args=1
    name='OPTIONAL'
    opcode=266
OPTIONAL.__name__='OPTIONAL'

class OR(Function):
    min_args=2
    max_args=2
    name='OR'
    opcode=267
OR.__name__='OR'

class OR_NOT(Function):
    min_args=2
    max_args=2
    name='OR_NOT'
    opcode=268
OR_NOT.__name__='OR_NOT'

class OUT(Function):
    min_args=1
    max_args=1
    name='OUT'
    opcode=269
OUT.__name__='OUT'

class PACK(Function):
    min_args=2
    max_args=3
    name='PACK'
    opcode=270
PACK.__name__='PACK'

class PHASE_OF(Function):
    min_args=1
    max_args=1
    name='PHASE_OF'
    opcode=271
PHASE_OF.__name__='PHASE_OF'

class POST_DEC(Function):
    min_args=1
    max_args=1
    name='POST_DEC'
    opcode=272
POST_DEC.__name__='POST_DEC'

class POST_INC(Function):
    min_args=1
    max_args=1
    name='POST_INC'
    opcode=273
POST_INC.__name__='POST_INC'

class POWER(Function):
    min_args=2
    max_args=2
    name='POWER'
    opcode=274
POWER.__name__='POWER'

class PRESENT(Function):
    min_args=1
    max_args=1
    name='PRESENT'
    opcode=275
PRESENT.__name__='PRESENT'

class PRE_DEC(Function):
    min_args=1
    max_args=1
    name='PRE_DEC'
    opcode=276
PRE_DEC.__name__='PRE_DEC'

class PRE_INC(Function):
    min_args=1
    max_args=1
    name='PRE_INC'
    opcode=277
PRE_INC.__name__='PRE_INC'

class PRIVATE(Function):
    min_args=1
    max_args=1
    name='PRIVATE'
    opcode=278
PRIVATE.__name__='PRIVATE'

class PROCEDURE_OF(Function):
    min_args=1
    max_args=1
    name='PROCEDURE_OF'
    opcode=279
PROCEDURE_OF.__name__='PROCEDURE_OF'

class PRODUCT(Function):
    min_args=1
    max_args=3
    name='PRODUCT'
    opcode=280
PRODUCT.__name__='PRODUCT'

class PROGRAM_OF(Function):
    min_args=1
    max_args=1
    name='PROGRAM_OF'
    opcode=281
PROGRAM_OF.__name__='PROGRAM_OF'

class PROJECT(Function):
    min_args=3
    max_args=4
    name='PROJECT'
    opcode=282
PROJECT.__name__='PROJECT'

class PROMOTE(Function):
    min_args=2
    max_args=2
    name='PROMOTE'
    opcode=283
PROMOTE.__name__='PROMOTE'

class PUBLIC(Function):
    min_args=1
    max_args=1
    name='PUBLIC'
    opcode=284
PUBLIC.__name__='PUBLIC'

class QUADWORD(Function):
    min_args=1
    max_args=1
    name='QUADWORD'
    opcode=285
QUADWORD.__name__='QUADWORD'

class QUADWORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    name='QUADWORD_UNSIGNED'
    opcode=286
QUADWORD_UNSIGNED.__name__='QUADWORD_UNSIGNED'

class QUALIFIERS_OF(Function):
    min_args=1
    max_args=1
    name='QUALIFIERS_OF'
    opcode=287
QUALIFIERS_OF.__name__='QUALIFIERS_OF'

class RADIX(Function):
    min_args=1
    max_args=1
    name='RADIX'
    opcode=288
RADIX.__name__='RADIX'

class RAMP(Function):
    min_args=0
    max_args=2
    name='RAMP'
    opcode=289
RAMP.__name__='RAMP'

class RANDOM(Function):
    min_args=0
    max_args=2
    name='RANDOM'
    opcode=290
RANDOM.__name__='RANDOM'

class RANDOM_SEED(Function):
    min_args=0
    max_args=1
    name='RANDOM_SEED'
    opcode=291
RANDOM_SEED.__name__='RANDOM_SEED'

class DTYPE_RANGE(Function):
    min_args=2
    max_args=3
    name='DTYPE_RANGE'
    opcode=292
DTYPE_RANGE.__name__='DTYPE_RANGE'

class RANK(Function):
    min_args=1
    max_args=1
    name='RANK'
    opcode=293
RANK.__name__='RANK'

class RAW_OF(Function):
    min_args=1
    max_args=1
    name='RAW_OF'
    opcode=294
RAW_OF.__name__='RAW_OF'

class READ(Function):
    min_args=1
    max_args=1
    name='READ'
    opcode=295
READ.__name__='READ'

class REAL(Function):
    min_args=1
    max_args=2
    name='REAL'
    opcode=296
REAL.__name__='REAL'

class REBIN(Function):
    min_args=2
    max_args=4
    name='REBIN'
    opcode=297
REBIN.__name__='REBIN'

class REF(Function):
    min_args=1
    max_args=1
    name='REF'
    opcode=298
REF.__name__='REF'

class REPEAT(Function):
    min_args=2
    max_args=2
    name='REPEAT'
    opcode=299
REPEAT.__name__='REPEAT'

class REPLICATE(Function):
    min_args=3
    max_args=3
    name='REPLICATE'
    opcode=300
REPLICATE.__name__='REPLICATE'

class RESHAPE(Function):
    min_args=2
    max_args=4
    name='RESHAPE'
    opcode=301
RESHAPE.__name__='RESHAPE'

class RETURN(Function):
    min_args=0
    max_args=1
    name='RETURN'
    opcode=302
RETURN.__name__='RETURN'

class REWIND(Function):
    min_args=1
    max_args=1
    name='REWIND'
    opcode=303
REWIND.__name__='REWIND'

class RMS(Function):
    min_args=1
    max_args=3
    name='RMS'
    opcode=304
RMS.__name__='RMS'

class ROUTINE_OF(Function):
    min_args=1
    max_args=1
    name='ROUTINE_OF'
    opcode=305
ROUTINE_OF.__name__='ROUTINE_OF'

class RRSPACING(Function):
    min_args=1
    max_args=1
    name='RRSPACING'
    opcode=306
RRSPACING.__name__='RRSPACING'

class SCALE(Function):
    min_args=2
    max_args=2
    name='SCALE'
    opcode=307
SCALE.__name__='SCALE'

class SCAN(Function):
    min_args=2
    max_args=3
    name='SCAN'
    opcode=308
SCAN.__name__='SCAN'

class FSEEK(Function):
    min_args=1
    max_args=3
    name='FSEEK'
    opcode=309
FSEEK.__name__='FSEEK'

class SET_EXPONENT(Function):
    min_args=2
    max_args=2
    name='SET_EXPONENT'
    opcode=310
SET_EXPONENT.__name__='SET_EXPONENT'

class SET_RANGE(Function):
    min_args=2
    max_args=9
    name='SET_RANGE'
    opcode=311
SET_RANGE.__name__='SET_RANGE'

class ISHFT(Function):
    min_args=2
    max_args=2
    name='ISHFT'
    opcode=312
ISHFT.__name__='ISHFT'

class ISHFTC(Function):
    min_args=3
    max_args=3
    name='ISHFTC'
    opcode=313
ISHFTC.__name__='ISHFTC'

class SHIFT_LEFT(Function):
    min_args=2
    max_args=2
    name='SHIFT_LEFT'
    opcode=314
SHIFT_LEFT.__name__='SHIFT_LEFT'

class SHIFT_RIGHT(Function):
    min_args=2
    max_args=2
    name='SHIFT_RIGHT'
    opcode=315
SHIFT_RIGHT.__name__='SHIFT_RIGHT'

class SIGN(Function):
    min_args=2
    max_args=2
    name='SIGN'
    opcode=316
SIGN.__name__='SIGN'

class SIGNED(Function):
    min_args=1
    max_args=1
    name='SIGNED'
    opcode=317
SIGNED.__name__='SIGNED'

class SIN(Function):
    min_args=1
    max_args=1
    name='SIN'
    opcode=318
SIN.__name__='SIN'

class SIND(Function):
    min_args=1
    max_args=1
    name='SIND'
    opcode=319
SIND.__name__='SIND'

class SINH(Function):
    min_args=1
    max_args=1
    name='SINH'
    opcode=320
SINH.__name__='SINH'

class SIZEOF(Function):
    min_args=1
    max_args=1
    name='SIZEOF'
    opcode=321
SIZEOF.__name__='SIZEOF'

class SLOPE_OF(Function):
    min_args=1
    max_args=2
    name='SLOPE_OF'
    opcode=322
SLOPE_OF.__name__='SLOPE_OF'

class SMOOTH(Function):
    min_args=2
    max_args=2
    name='SMOOTH'
    opcode=323
SMOOTH.__name__='SMOOTH'

class SOLVE(Function):
    min_args=2
    max_args=2
    name='SOLVE'
    opcode=324
SOLVE.__name__='SOLVE'

class SORTVAL(Function):
    min_args=1
    max_args=2
    name='SORTVAL'
    opcode=325
SORTVAL.__name__='SORTVAL'

class SPACING(Function):
    min_args=1
    max_args=1
    name='SPACING'
    opcode=326
SPACING.__name__='SPACING'

class SPAWN(Function):
    min_args=0
    max_args=3
    name='SPAWN'
    opcode=327
SPAWN.__name__='SPAWN'

class SPREAD(Function):
    min_args=3
    max_args=3
    name='SPREAD'
    opcode=328
SPREAD.__name__='SPREAD'

class SQRT(Function):
    min_args=1
    max_args=1
    name='SQRT'
    opcode=329
SQRT.__name__='SQRT'

class SQUARE(Function):
    min_args=1
    max_args=1
    name='SQUARE'
    opcode=330
SQUARE.__name__='SQUARE'

class STATEMENT(Function):
    min_args=0
    max_args=254
    name='STATEMENT'
    opcode=331
STATEMENT.__name__='STATEMENT'

class STD_DEV(Function):
    min_args=1
    max_args=3
    name='STD_DEV'
    opcode=332
STD_DEV.__name__='STD_DEV'

class STRING(Function):
    min_args=1
    max_args=254
    name='STRING'
    opcode=333
STRING.__name__='STRING'

class STRING_OPCODE(Function):
    min_args=1
    max_args=1
    name='STRING_OPCODE'
    opcode=334
STRING_OPCODE.__name__='STRING_OPCODE'

class SUBSCRIPT(Function):
    min_args=1
    max_args=9
    name='SUBSCRIPT'
    opcode=335
SUBSCRIPT.__name__='SUBSCRIPT'

class SUBTRACT(Function):
    min_args=2
    max_args=2
    name='SUBTRACT'
    opcode=336
SUBTRACT.__name__='SUBTRACT'

class SUM(Function):
    min_args=1
    max_args=3
    name='SUM'
    opcode=337
SUM.__name__='SUM'

class SWITCH(Function):
    min_args=2
    max_args=254
    name='SWITCH'
    opcode=338
SWITCH.__name__='SWITCH'

class SYSTEM_CLOCK(Function):
    min_args=1
    max_args=1
    name='SYSTEM_CLOCK'
    opcode=339
SYSTEM_CLOCK.__name__='SYSTEM_CLOCK'

class TAN(Function):
    min_args=1
    max_args=1
    name='TAN'
    opcode=340
TAN.__name__='TAN'

class TAND(Function):
    min_args=1
    max_args=1
    name='TAND'
    opcode=341
TAND.__name__='TAND'

class TANH(Function):
    min_args=1
    max_args=1
    name='TANH'
    opcode=342
TANH.__name__='TANH'

class TASK_OF(Function):
    min_args=1
    max_args=1
    name='TASK_OF'
    opcode=343
TASK_OF.__name__='TASK_OF'

class TEXT(Function):
    min_args=1
    max_args=2
    name='TEXT'
    opcode=344
TEXT.__name__='TEXT'

class TIME_OUT_OF(Function):
    min_args=1
    max_args=1
    name='TIME_OUT_OF'
    opcode=345
TIME_OUT_OF.__name__='TIME_OUT_OF'

class TINY(Function):
    min_args=1
    max_args=1
    name='TINY'
    opcode=346
TINY.__name__='TINY'

class TRANSFER(Function):
    min_args=2
    max_args=3
    name='TRANSFER'
    opcode=347
TRANSFER.__name__='TRANSFER'

class TRANSPOSE_(Function):
    min_args=1
    max_args=1
    name='TRANSPOSE_'
    opcode=348
TRANSPOSE_.__name__='TRANSPOSE_'

class TRIM(Function):
    min_args=1
    max_args=1
    name='TRIM'
    opcode=349
TRIM.__name__='TRIM'

class UNARY_MINUS(Function):
    min_args=1
    max_args=1
    name='UNARY_MINUS'
    opcode=350
UNARY_MINUS.__name__='UNARY_MINUS'

class UNARY_PLUS(Function):
    min_args=1
    max_args=1
    name='UNARY_PLUS'
    opcode=351
UNARY_PLUS.__name__='UNARY_PLUS'

class UNION(Function):
    min_args=0
    max_args=254
    name='UNION'
    opcode=352
UNION.__name__='UNION'

class UNITS(Function):
    min_args=1
    max_args=1
    name='UNITS'
    opcode=353
UNITS.__name__='UNITS'

class UNITS_OF(Function):
    min_args=1
    max_args=1
    name='UNITS_OF'
    opcode=354
UNITS_OF.__name__='UNITS_OF'

class UNPACK(Function):
    min_args=3
    max_args=3
    name='UNPACK'
    opcode=355
UNPACK.__name__='UNPACK'

class UNSIGNED(Function):
    min_args=1
    max_args=1
    name='UNSIGNED'
    opcode=356
UNSIGNED.__name__='UNSIGNED'

class VAL(Function):
    min_args=1
    max_args=1
    name='VAL'
    opcode=357
VAL.__name__='VAL'

class VALIDATION_OF(Function):
    min_args=1
    max_args=1
    name='VALIDATION_OF'
    opcode=358
VALIDATION_OF.__name__='VALIDATION_OF'

class VALUE_OF(Function):
    min_args=1
    max_args=1
    name='VALUE_OF'
    opcode=359
VALUE_OF.__name__='VALUE_OF'

class VAR(Function):
    min_args=1
    max_args=2
    name='VAR'
    opcode=360
VAR.__name__='VAR'

class VECTOR(Function):
    min_args=0
    max_args=254
    name='VECTOR'
    opcode=361
VECTOR.__name__='VECTOR'

class VERIFY(Function):
    min_args=2
    max_args=3
    name='VERIFY'
    opcode=362
VERIFY.__name__='VERIFY'

class WAIT(Function):
    min_args=1
    max_args=1
    name='WAIT'
    opcode=363
WAIT.__name__='WAIT'

class WHEN_OF(Function):
    min_args=1
    max_args=1
    name='WHEN_OF'
    opcode=364
WHEN_OF.__name__='WHEN_OF'

class WHERE(Function):
    min_args=2
    max_args=3
    name='WHERE'
    opcode=365
WHERE.__name__='WHERE'

class WHILE(Function):
    min_args=2
    max_args=254
    name='WHILE'
    opcode=366
WHILE.__name__='WHILE'

class WINDOW_OF(Function):
    min_args=1
    max_args=1
    name='WINDOW_OF'
    opcode=367
WINDOW_OF.__name__='WINDOW_OF'

class WORD(Function):
    min_args=1
    max_args=1
    name='WORD'
    opcode=368
WORD.__name__='WORD'

class WORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    name='WORD_UNSIGNED'
    opcode=369
WORD_UNSIGNED.__name__='WORD_UNSIGNED'

class WRITE(Function):
    min_args=1
    max_args=254
    name='WRITE'
    opcode=370
WRITE.__name__='WRITE'

class ZERO(Function):
    min_args=0
    max_args=2
    name='ZERO'
    opcode=371
ZERO.__name__='ZERO'

class d2PI(Function):
    min_args=0
    max_args=0
    name='$2PI'
    opcode=372
d2PI.__name__='$2PI'

class dNARG(Function):
    min_args=0
    max_args=0
    name='$NARG'
    opcode=373
dNARG.__name__='$NARG'

class ELEMENT(Function):
    min_args=3
    max_args=3
    name='ELEMENT'
    opcode=374
ELEMENT.__name__='ELEMENT'

class RC_DROOP(Function):
    min_args=3
    max_args=3
    name='RC_DROOP'
    opcode=375
RC_DROOP.__name__='RC_DROOP'

class RESET_PRIVATE(Function):
    min_args=0
    max_args=0
    name='RESET_PRIVATE'
    opcode=376
RESET_PRIVATE.__name__='RESET_PRIVATE'

class RESET_PUBLIC(Function):
    min_args=0
    max_args=0
    name='RESET_PUBLIC'
    opcode=377
RESET_PUBLIC.__name__='RESET_PUBLIC'

class SHOW_PRIVATE(Function):
    min_args=0
    max_args=254
    name='SHOW_PRIVATE'
    opcode=378
SHOW_PRIVATE.__name__='SHOW_PRIVATE'

class SHOW_PUBLIC(Function):
    min_args=0
    max_args=254
    name='SHOW_PUBLIC'
    opcode=379
SHOW_PUBLIC.__name__='SHOW_PUBLIC'

class SHOW_VM(Function):
    min_args=0
    max_args=2
    name='SHOW_VM'
    opcode=380
SHOW_VM.__name__='SHOW_VM'

class TRANSLATE(Function):
    min_args=3
    max_args=3
    name='TRANSLATE'
    opcode=381
TRANSLATE.__name__='TRANSLATE'

class TRANSPOSE_MUL(Function):
    min_args=2
    max_args=2
    name='TRANSPOSE_MUL'
    opcode=382
TRANSPOSE_MUL.__name__='TRANSPOSE_MUL'

class UPCASE(Function):
    min_args=1
    max_args=1
    name='UPCASE'
    opcode=383
UPCASE.__name__='UPCASE'

class USING(Function):
    min_args=2
    max_args=4
    name='USING'
    opcode=384
USING.__name__='USING'

class VALIDATION(Function):
    min_args=1
    max_args=1
    name='VALIDATION'
    opcode=385
VALIDATION.__name__='VALIDATION'

class dDEFAULT(Function):
    min_args=0
    max_args=0
    name='$DEFAULT'
    opcode=386
dDEFAULT.__name__='$DEFAULT'

class dEXPT(Function):
    min_args=0
    max_args=0
    name='$EXPT'
    opcode=387
dEXPT.__name__='$EXPT'

class dSHOT(Function):
    min_args=0
    max_args=0
    name='$SHOT'
    opcode=388
dSHOT.__name__='$SHOT'

class GETDBI(Function):
    min_args=1
    max_args=2
    name='GETDBI'
    opcode=389
GETDBI.__name__='GETDBI'

class CULL(Function):
    min_args=1
    max_args=4
    name='CULL'
    opcode=390
CULL.__name__='CULL'

class EXTEND(Function):
    min_args=1
    max_args=4
    name='EXTEND'
    opcode=391
EXTEND.__name__='EXTEND'

class I_TO_X(Function):
    min_args=1
    max_args=2
    name='I_TO_X'
    opcode=392
I_TO_X.__name__='I_TO_X'

class X_TO_I(Function):
    min_args=1
    max_args=2
    name='X_TO_I'
    opcode=393
X_TO_I.__name__='X_TO_I'

class MAP(Function):
    min_args=2
    max_args=2
    name='MAP'
    opcode=394
MAP.__name__='MAP'

class COMPILE_DEPENDENCY(Function):
    min_args=1
    max_args=1
    name='COMPILE_DEPENDENCY'
    opcode=395
COMPILE_DEPENDENCY.__name__='COMPILE_DEPENDENCY'

class DECOMPILE_DEPENDENCY(Function):
    min_args=1
    max_args=1
    name='DECOMPILE_DEPENDENCY'
    opcode=396
DECOMPILE_DEPENDENCY.__name__='DECOMPILE_DEPENDENCY'

class BUILD_CALL(Function):
    min_args=3
    max_args=254
    name='BUILD_CALL'
    opcode=397
BUILD_CALL.__name__='BUILD_CALL'

class ERRORLOGS_OF(Function):
    min_args=1
    max_args=1
    name='ERRORLOGS_OF'
    opcode=398
ERRORLOGS_OF.__name__='ERRORLOGS_OF'

class PERFORMANCE_OF(Function):
    min_args=1
    max_args=1
    name='PERFORMANCE_OF'
    opcode=399
PERFORMANCE_OF.__name__='PERFORMANCE_OF'

class XD(Function):
    min_args=1
    max_args=1
    name='XD'
    opcode=400
XD.__name__='XD'

class CONDITION_OF(Function):
    min_args=1
    max_args=1
    name='CONDITION_OF'
    opcode=401
CONDITION_OF.__name__='CONDITION_OF'

class SORT(Function):
    min_args=1
    max_args=2
    name='SORT'
    opcode=402
SORT.__name__='SORT'

class dTHIS(Function):
    min_args=0
    max_args=0
    name='$THIS'
    opcode=403
dTHIS.__name__='$THIS'

class DATA_WITH_UNITS(Function):
    min_args=1
    max_args=1
    name='DATA_WITH_UNITS'
    opcode=404
DATA_WITH_UNITS.__name__='DATA_WITH_UNITS'

class dATM(Function):
    min_args=0
    max_args=0
    name='$ATM'
    opcode=405
dATM.__name__='$ATM'

class dEPSILON0(Function):
    min_args=0
    max_args=0
    name='$EPSILON0'
    opcode=406
dEPSILON0.__name__='$EPSILON0'

class dGN(Function):
    min_args=0
    max_args=0
    name='$GN'
    opcode=407
dGN.__name__='$GN'

class dMU0(Function):
    min_args=0
    max_args=0
    name='$MU0'
    opcode=408
dMU0.__name__='$MU0'

class EXTRACT(Function):
    min_args=3
    max_args=3
    name='EXTRACT'
    opcode=409
EXTRACT.__name__='EXTRACT'

class FINITE(Function):
    min_args=1
    max_args=1
    name='FINITE'
    opcode=410
FINITE.__name__='FINITE'

class BIT_SIZE(Function):
    min_args=1
    max_args=1
    name='BIT_SIZE'
    opcode=411
BIT_SIZE.__name__='BIT_SIZE'

class MODULO(Function):
    min_args=2
    max_args=2
    name='MODULO'
    opcode=412
MODULO.__name__='MODULO'

class SELECTED_INT_KIND(Function):
    min_args=1
    max_args=1
    name='SELECTED_INT_KIND'
    opcode=413
SELECTED_INT_KIND.__name__='SELECTED_INT_KIND'

class SELECTED_REAL_KIND(Function):
    min_args=1
    max_args=2
    name='SELECTED_REAL_KIND'
    opcode=414
SELECTED_REAL_KIND.__name__='SELECTED_REAL_KIND'

class DSQL(Function):
    min_args=1
    max_args=254
    name='DSQL'
    opcode=415
DSQL.__name__='DSQL'

class ISQL(Function):
    min_args=1
    max_args=1
    name='ISQL'
    opcode=416
ISQL.__name__='ISQL'

class FTELL(Function):
    min_args=1
    max_args=1
    name='FTELL'
    opcode=417
FTELL.__name__='FTELL'

class MAKE_ACTION(Function):
    min_args=2
    max_args=5
    name='MAKE_ACTION'
    opcode=418
MAKE_ACTION.__name__='MAKE_ACTION'

class MAKE_CONDITION(Function):
    min_args=2
    max_args=2
    name='MAKE_CONDITION'
    opcode=419
MAKE_CONDITION.__name__='MAKE_CONDITION'

class MAKE_CONGLOM(Function):
    min_args=4
    max_args=4
    name='MAKE_CONGLOM'
    opcode=420
MAKE_CONGLOM.__name__='MAKE_CONGLOM'

class MAKE_DEPENDENCY(Function):
    min_args=3
    max_args=3
    name='MAKE_DEPENDENCY'
    opcode=421
MAKE_DEPENDENCY.__name__='MAKE_DEPENDENCY'

class MAKE_DIM(Function):
    min_args=2
    max_args=2
    name='MAKE_DIM'
    opcode=422
MAKE_DIM.__name__='MAKE_DIM'

class MAKE_DISPATCH(Function):
    min_args=5
    max_args=5
    name='MAKE_DISPATCH'
    opcode=423
MAKE_DISPATCH.__name__='MAKE_DISPATCH'

class MAKE_FUNCTION(Function):
    min_args=1
    max_args=254
    name='MAKE_FUNCTION'
    opcode=424
MAKE_FUNCTION.__name__='MAKE_FUNCTION'

class MAKE_METHOD(Function):
    min_args=3
    max_args=254
    name='MAKE_METHOD'
    opcode=425
MAKE_METHOD.__name__='MAKE_METHOD'

class MAKE_PARAM(Function):
    min_args=3
    max_args=3
    name='MAKE_PARAM'
    opcode=426
MAKE_PARAM.__name__='MAKE_PARAM'

class MAKE_PROCEDURE(Function):
    min_args=3
    max_args=254
    name='MAKE_PROCEDURE'
    opcode=427
MAKE_PROCEDURE.__name__='MAKE_PROCEDURE'

class MAKE_PROGRAM(Function):
    min_args=2
    max_args=2
    name='MAKE_PROGRAM'
    opcode=428
MAKE_PROGRAM.__name__='MAKE_PROGRAM'

class MAKE_RANGE(Function):
    min_args=2
    max_args=3
    name='MAKE_RANGE'
    opcode=429
MAKE_RANGE.__name__='MAKE_RANGE'

class MAKE_ROUTINE(Function):
    min_args=3
    max_args=254
    name='MAKE_ROUTINE'
    opcode=430
MAKE_ROUTINE.__name__='MAKE_ROUTINE'

class MAKE_SIGNAL(Function):
    min_args=2
    max_args=10
    name='MAKE_SIGNAL'
    opcode=431
MAKE_SIGNAL.__name__='MAKE_SIGNAL'

class MAKE_WINDOW(Function):
    min_args=3
    max_args=3
    name='MAKE_WINDOW'
    opcode=432
MAKE_WINDOW.__name__='MAKE_WINDOW'

class MAKE_WITH_UNITS(Function):
    min_args=2
    max_args=2
    name='MAKE_WITH_UNITS'
    opcode=433
MAKE_WITH_UNITS.__name__='MAKE_WITH_UNITS'

class MAKE_CALL(Function):
    min_args=3
    max_args=254
    name='MAKE_CALL'
    opcode=434
MAKE_CALL.__name__='MAKE_CALL'

class CLASS_OF(Function):
    min_args=1
    max_args=1
    name='CLASS_OF'
    opcode=435
CLASS_OF.__name__='CLASS_OF'

class DSCPTR_OF(Function):
    min_args=1
    max_args=2
    name='DSCPTR_OF'
    opcode=436
DSCPTR_OF.__name__='DSCPTR_OF'

class KIND_OF(Function):
    min_args=1
    max_args=1
    name='KIND_OF'
    opcode=437
KIND_OF.__name__='KIND_OF'

class NDESC_OF(Function):
    min_args=1
    max_args=1
    name='NDESC_OF'
    opcode=438
NDESC_OF.__name__='NDESC_OF'

class ACCUMULATE(Function):
    min_args=1
    max_args=3
    name='ACCUMULATE'
    opcode=439
ACCUMULATE.__name__='ACCUMULATE'

class MAKE_SLOPE(Function):
    min_args=1
    max_args=254
    name='MAKE_SLOPE'
    opcode=440
MAKE_SLOPE.__name__='MAKE_SLOPE'

class REM(Function):
    min_args=1
    max_args=254
    name='REM'
    opcode=441
REM.__name__='REM'

class COMPLETION_MESSAGE_OF(Function):
    min_args=1
    max_args=1
    name='COMPLETION_MESSAGE_OF'
    opcode=442
COMPLETION_MESSAGE_OF.__name__='COMPLETION_MESSAGE_OF'

class INTERRUPT_OF(Function):
    min_args=1
    max_args=1
    name='INTERRUPT_OF'
    opcode=443
INTERRUPT_OF.__name__='INTERRUPT_OF'

class dSHOTNAME(Function):
    min_args=0
    max_args=0
    name='$SHOTNAME'
    opcode=444
dSHOTNAME.__name__='$SHOTNAME'

class BUILD_WITH_ERROR(Function):
    min_args=2
    max_args=2
    name='BUILD_WITH_ERROR'
    opcode=445
BUILD_WITH_ERROR.__name__='BUILD_WITH_ERROR'

class ERROR_OF(Function):
    min_args=1
    max_args=1
    name='ERROR_OF'
    opcode=446
ERROR_OF.__name__='ERROR_OF'

class MAKE_WITH_ERROR(Function):
    min_args=2
    max_args=2
    name='MAKE_WITH_ERROR'
    opcode=447
MAKE_WITH_ERROR.__name__='MAKE_WITH_ERROR'

class DO_TASK(Function):
    min_args=1
    max_args=1
    name='DO_TASK'
    opcode=448
DO_TASK.__name__='DO_TASK'

class ISQL_SET(Function):
    min_args=1
    max_args=3
    name='ISQL_SET'
    opcode=449
ISQL_SET.__name__='ISQL_SET'

class FS_FLOAT(Function):
    min_args=1
    max_args=1
    name='FS_FLOAT'
    opcode=450
FS_FLOAT.__name__='FS_FLOAT'

class FS_COMPLEX(Function):
    min_args=1
    max_args=2
    name='FS_COMPLEX'
    opcode=451
FS_COMPLEX.__name__='FS_COMPLEX'

class FT_FLOAT(Function):
    min_args=1
    max_args=1
    name='FT_FLOAT'
    opcode=452
FT_FLOAT.__name__='FT_FLOAT'

class FT_COMPLEX(Function):
    min_args=1
    max_args=2
    name='FT_COMPLEX'
    opcode=453
FT_COMPLEX.__name__='FT_COMPLEX'

class BUILD_OPAQUE(Function):
    min_args=2
    max_args=2
    name='BUILD_OPAQUE'
    opcode=454
BUILD_OPAQUE.__name__='BUILD_OPAQUE'

class MAKE_OPAQUE(Function):
    min_args=2
    max_args=2
    name='MAKE_OPAQUE'
    opcode=455
MAKE_OPAQUE.__name__='MAKE_OPAQUE'


_opcodeToClass={}

globs=list(globals().values())
for c in globs:
    if hasattr(c,'opcode'):
        _opcodeToClass[c.opcode]=c
del globs
