import ctypes as _C

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_ver=_mimport('version')
_descriptor=_mimport('descriptor')
_data=_mimport('mdsdata')
_tree=_mimport('tree')
_exceptions=_mimport('mdsExceptions')

class Compound(_data.Data):
    def __init__(self,*args, **kwargs):
        """MDSplus compound data.
        """
        if len(args)==1 and args[0] is self: return
        if self.__class__ is Compound:
            raise TypeError("Cannot create instances of class Compound")
        self._fields={}
        for idx in range(len(self.fields)):
            self._fields[self.fields[idx]]=idx
        self._argOffset=len(self.fields)
        self.setDescs(args)
        for k,v in kwargs:
            if k in self.fields:
                self.setDescAt(self._fields[k],v)

    @property
    def deref(self):
        for i in range(self.getNumDescs()):
            ans = self.getDescAt(i)
            if isinstance(ans,(_data.Data,_tree.TreeNode)):
                self.setDescAt(i,ans.deref)
        return self


    def __hasBadTreeReferences__(self,tree):
        for arg in self._args:
            if isinstance(arg,_data.Data) and arg.__hasBadTreeReferences__(tree):
                return True
        return False

    def __fixTreeReferences__(self,tree):
        from copy import deepcopy
        ans = deepcopy(self)
        for arg in ans._args:
            if isinstance(arg,_data.Data) and arg.__hasBadTreeReferences__(tree):
                arg.__fixTreeReferences__(tree)
        return ans

    def __getattr__(self,name):
        if name == '_fields':
            return {}
        if name in self._fields:
            return self.getDescAt(self._fields[name])
        elif name.startswith('get') and name[3:].lower() in self._fields:
            def getter():
                return self._args[self._fields[name[3:].lower()]]
            return getter
        elif name.startswith('set') and name[3:].lower() in self._fields:
            def setter(value):
                self.__setattr__(name[3:].lower(),value)
            return setter
        raise AttributeError("No such attribute '%s' in %s"%(name,self.__class__.__name__))

    def __getitem__(self,idx):
        return self.getDescAt(idx)

    def __setattr__(self,name,value):
        if name in self._fields:
            self.setDescAt(self._fields[name],value)
        else:
            super(Compound,self).__setattr__(name,value)

    def __setitem__(self,idx,value):
        return self.setDescAt(idx,value)

    def getArgumentAt(self,idx):
        """Return argument at index idx (indexes start at 0)
        @rtype: Data,None
        """
        return self.getDescAt(self._argOffset+idx)

    def getArguments(self):
        """Return arguments
        @rtype: Data,None
        """
        return self.getDescAt(slice(self._argOffset,None))

    def getDescAt(self,idx):
        """Return descriptor with index idx (first descriptor is 0)
        @rtype: Data
        """
        if isinstance(idx, (slice,)):
            return tuple(self._args[idx])
        if idx<len(self._args):
            return self._args[idx]
        return None

    def getDescs(self):
        """Return descriptors or None if no descriptors
        @rtype: tuple,None
        """
        return tuple(self._args)

    def getNumDescs(self):
       """Return number of descriptors
       @rtype: int
       """
       return len(self._args)

    def setArgumentAt(self,idx,value):
        """Set argument at index idx (indexes start at 0)"""
        return self.setDescAt(self._argOffset+idx, value)

    def setArguments(self,args):
        """Set arguments
        @type args: tuple
        """
        self._args = self._args[:self._argOffset+len(args)]
        return self.setDescAt(slice(self._argOffset,None),args)

    def setDescAt(self,idx,value):
        """Set descriptor at index idx (indexes start at 0)"""
        if isinstance(idx,slice):
            indices=idx.indices(idx.start+len(value))
            idx = 0
            for i in range(indices[0],indices[1],indices[2]):
                self.setDescAt(i,value[idx])
                idx += 1
        else:
            if value is None:
                if len(self._args) > idx:
                    self._args[idx] = None
            else:
                if len(self._args) <= idx:
                    self._args+=[None]*(idx-len(self._args)+1)
                self._args[idx]=_data.Data(value)
        return self

    def setDescs(self,args):
        """Set descriptors
        @type args: tuple
        """
        self._args = [_data.Data(arg) for arg in args]
        while self.getNumDescs()<self._argOffset:
            self._args.append(None)

    @staticmethod
    def descriptorWithProps(value,d):
        if value._units is not None:
            dunits=_descriptor.Descriptor_r()
            dunits.length=0
            dunits.dtype=WithUnits.dtype_id
            dunits.pointer=_C.c_void_p(0)
            dunits.ndesc=2
            dunits.dscptrs[0]=_descriptor.descToPointer(d)
            dunits.dscptrs[1]=_descriptor.objectToPointer(_data.Data(value._units))
            dunits.original=d
        else:
            dunits=d
        if value._error is not None:
            derror=_descriptor.Descriptor_r()
            derror.length=0
            derror.dtype=WithError.dtype_id
            derror.pointer=_C.c_void_p(0)
            derror.ndesc=2
            derror.dscptrs[0]=_descriptor.descToPointer(dunits)
            derror.dscptrs[1]=_descriptor.objectToPointer(_data.Data(value._error))
            derror.original=dunits
        else:
            derror=dunits
        if value._help is not None or value._validation is not None:
            dparam=_descriptor.Descriptor_r()
            dparam.length=0
            dparam.dtype=Parameter.dtype_id
            dparam.pointer=_C.c_void_p(0)
            dparam.ndesc=3
            dparam.dscptrs[0]=_descriptor.descToPointer(derror)
            dparam.dscptrs[1]=_descriptor.objectToPointer(_data.Data(value._help))
            dparam.dscptrs[2]=_descriptor.objectToPointer(_data.Data(value._validation))
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
        d.ndesc = self.getNumDescs()
        for idx in _ver.xrange(d.ndesc):
            d.dscptrs[idx]= _descriptor.objectToPointer(self.getDescAt(idx))
        d.original=self
        if self._units is None and self._error is None and self._help is None and self._validation is None:
            return d  # if not props
        else:
            return self.descriptorWithProps(self,d)

    @classmethod
    def fromDescriptor(cls,d):
        args = [_descriptor.pointerToObject(d.dscptrs[i]) for i in _ver.xrange(d.ndesc)]
        ans=cls(*args)
        if d.length>0:
            if d.length == 1:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint8))
            elif d.length == 2:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint16))
            else:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint32))
            ans.opcode = opcptr.contents.value
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
    def getClass(self, *args):
        if not self.image=='__python__':
            raise _exceptions.DevNOT_A_PYDEVICE
        model = str(self.model)
        safe_env = {}
        qualifiers = self.qualifiers.value.tolist()
        if isinstance(qualifiers,list): qualifiers = ';'.join(qualifiers)  # make it a list of statements
        exec(compile(qualifiers,'<string>','exec'),safe_env)
        if model in safe_env:
            cls = safe_env[model]
        else:
            cls = _tree.Device.importPyDeviceModule(model).__dict__[model]
        if issubclass(cls,(_tree.Device,)):
            return cls if len(args)==0 else cls(*args)
        raise _exceptions.DevPYDEVICE_NOT_FOUND

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
        if len(args)==1 and args[0] is self: return
        if 'type' in kwargs:
            self.opcode=kwargs['type']
        else:
            self.opcode=2
        super(Dispatch,self).__init__(*args,**kwargs)

class Function(Compound):
    """A Function object is used to reference builtin MDSplus functions. For example the expression 1+2
    is represented in as Function instance created by Function(opcode='ADD',args=(1,2))
    """
    fields=tuple()
    dtype_id=199
    opcodeToClass={}

    @classmethod
    def fromDescriptor(cls,d):
        opc  = _C.cast(d.pointer,_C.POINTER(_C.c_uint16)).contents.value
        args = [_descriptor.pointerToObject(d.dscptrs[i]) for i in _ver.xrange(d.ndesc)]
        return cls.opcodeToClass[opc](*args)

    def __init__(self,*args):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        if len(args)==1 and args[0] is self: return
        if len(args)>self.max_args or (self.max_args>0 and len(args)<self.min_args):
            if self.max_args==0 or self.max_args==self.min_args:
                raise TypeError("Requires %d input arguments for %s but %d given"%(self.max_args,self.__class__.__name__,len(args)))
            else:
                raise TypeError("Requires %d to %d input arguments for %s but %d given"%(self.min_args,self.max_args,self.__class__.__name__,len(args)))
        super(Function,self).__init__(*args)

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
        for arg in self._args:
            parts.append(_data.Data(arg).decompile())
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
        return self.getDimensionAt(idx)

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
        return self.getArgumentAt(idx)

    def getDimensions(self):
        """Return all the dimensions of the signal
        @rtype: tuple
        """
        return self.getArguments()

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
      return Image.open(StringIO(_data.Data(self.getData()).data().data))

    @staticmethod
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
        opq=Opaque(_data.Data(_N.fromstring(f.read(),dtype="uint8")),typestring)
      finally:
        f.close()
      return opq

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
    def __init__(self,*args):
        if len(args)==1 and args[0] is self: return
        if len(args)==1 and isinstance(args,(slice,)):
            super(BUILD_RANGE,self).__init__(args[0].start,args[0].stop,args[0].step)
        else:
            super(BUILD_RANGE,self).__init__(*args)

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
    opcode=192

class INAND(Function):
    min_args=2
    max_args=2
    opcode=193

class INAND_NOT(Function):
    min_args=2
    max_args=2
    opcode=194

class INDEX(Function):
    min_args=2
    max_args=3
    opcode=195

class INOR(Function):
    min_args=2
    max_args=2
    opcode=196

class INOR_NOT(Function):
    min_args=2
    max_args=2
    opcode=197

class INOT(Function):
    min_args=1
    max_args=1
    opcode=198

class INOUT(Function):
    min_args=1
    max_args=1
    opcode=199

class INQUIRE(Function):
    min_args=2
    max_args=2
    opcode=200

class INT(Function):
    min_args=1
    max_args=2
    opcode=201

class INTEGRAL(Function):
    min_args=2
    max_args=3
    opcode=202

class INTERPOL(Function):
    min_args=2
    max_args=3
    opcode=203

class INTERSECT(Function):
    min_args=0
    max_args=254
    opcode=204

class INT_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=205

class INVERSE(Function):
    min_args=1
    max_args=1
    opcode=206

class IOR(Function):
    min_args=2
    max_args=2
    opcode=207

class IOR_NOT(Function):
    min_args=2
    max_args=2
    opcode=208

class IS_IN(Function):
    min_args=2
    max_args=3
    opcode=209

class IEOR(Function):
    min_args=2
    max_args=2
    opcode=210

class IEOR_NOT(Function):
    min_args=2
    max_args=2
    opcode=211

class LABEL(Function):
    min_args=2
    max_args=254
    opcode=212

class LAMINATE(Function):
    min_args=2
    max_args=254
    opcode=213

class LANGUAGE_OF(Function):
    min_args=1
    max_args=1
    opcode=214

class LASTLOC(Function):
    min_args=1
    max_args=2
    opcode=215

class LE(Function):
    min_args=2
    max_args=2
    opcode=216

class LEN(Function):
    min_args=1
    max_args=1
    opcode=217

class LEN_TRIM(Function):
    min_args=1
    max_args=1
    opcode=218

class LGE(Function):
    min_args=2
    max_args=2
    opcode=219

class LGT(Function):
    min_args=2
    max_args=2
    opcode=220

class LLE(Function):
    min_args=2
    max_args=2
    opcode=221

class LLT(Function):
    min_args=2
    max_args=2
    opcode=222

class LOG(Function):
    min_args=1
    max_args=1
    opcode=223

class LOG10(Function):
    min_args=1
    max_args=1
    opcode=224

class LOG2(Function):
    min_args=1
    max_args=1
    opcode=225

class LOGICAL(Function):
    min_args=1
    max_args=2
    opcode=226

class LONG(Function):
    min_args=1
    max_args=1
    opcode=227

class LONG_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=228

class LT(Function):
    min_args=2
    max_args=2
    opcode=229

class MATMUL(Function):
    min_args=2
    max_args=2
    opcode=230

class MAT_ROT(Function):
    min_args=2
    max_args=5
    opcode=231

class MAT_ROT_INT(Function):
    min_args=2
    max_args=5
    opcode=232

class MAX(Function):
    min_args=2
    max_args=254
    opcode=233

class MAXEXPONENT(Function):
    min_args=1
    max_args=1
    opcode=234

class MAXLOC(Function):
    min_args=1
    max_args=3
    opcode=235

class MAXVAL(Function):
    min_args=1
    max_args=3
    opcode=236

class MEAN(Function):
    min_args=1
    max_args=3
    opcode=237

class MEDIAN(Function):
    min_args=2
    max_args=2
    opcode=238

class MERGE(Function):
    min_args=3
    max_args=3
    opcode=239

class METHOD_OF(Function):
    min_args=1
    max_args=1
    opcode=240

class MIN(Function):
    min_args=2
    max_args=254
    opcode=241

class MINEXPONENT(Function):
    min_args=1
    max_args=1
    opcode=242

class MINLOC(Function):
    min_args=1
    max_args=3
    opcode=243

class MINVAL(Function):
    min_args=1
    max_args=3
    opcode=244

class MOD(Function):
    min_args=2
    max_args=2
    opcode=245

class MODEL_OF(Function):
    min_args=1
    max_args=1
    opcode=246

class MULTIPLY(Function):
    min_args=2
    max_args=2
    opcode=247

class NAME_OF(Function):
    min_args=1
    max_args=1
    opcode=248

class NAND(Function):
    min_args=2
    max_args=2
    opcode=249

class NAND_NOT(Function):
    min_args=2
    max_args=2
    opcode=250

class NDESC(Function):
    min_args=1
    max_args=1
    opcode=251

class NE(Function):
    min_args=2
    max_args=2
    opcode=252

class NEAREST(Function):
    min_args=2
    max_args=2
    opcode=253

class NEQV(Function):
    min_args=2
    max_args=2
    opcode=254

class NINT(Function):
    min_args=1
    max_args=2
    opcode=255

class NOR(Function):
    min_args=2
    max_args=2
    opcode=256

class NOR_NOT(Function):
    min_args=2
    max_args=2
    opcode=257

class NOT(Function):
    min_args=1
    max_args=1
    opcode=258

class OBJECT_OF(Function):
    min_args=1
    max_args=1
    opcode=259

class OCTAWORD(Function):
    min_args=1
    max_args=1
    opcode=260

class OCTAWORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=261

class ON_ERROR(Function):
    min_args=1
    max_args=1
    opcode=262

class OPCODE_BUILTIN(Function):
    min_args=1
    max_args=1
    opcode=263

class OPCODE_STRING(Function):
    min_args=1
    max_args=1
    opcode=264

class FOPEN(Function):
    min_args=2
    max_args=254
    opcode=265

class OPTIONAL(Function):
    min_args=1
    max_args=1
    opcode=266

class OR(Function):
    min_args=2
    max_args=2
    opcode=267

class OR_NOT(Function):
    min_args=2
    max_args=2
    opcode=268

class OUT(Function):
    min_args=1
    max_args=1
    opcode=269

class PACK(Function):
    min_args=2
    max_args=3
    opcode=270

class PHASE_OF(Function):
    min_args=1
    max_args=1
    opcode=271

class POST_DEC(Function):
    min_args=1
    max_args=1
    opcode=272

class POST_INC(Function):
    min_args=1
    max_args=1
    opcode=273

class POWER(Function):
    min_args=2
    max_args=2
    opcode=274

class PRESENT(Function):
    min_args=1
    max_args=1
    opcode=275

class PRE_DEC(Function):
    min_args=1
    max_args=1
    opcode=276

class PRE_INC(Function):
    min_args=1
    max_args=1
    opcode=277

class PRIVATE(Function):
    min_args=1
    max_args=1
    opcode=278

class PROCEDURE_OF(Function):
    min_args=1
    max_args=1
    opcode=279

class PRODUCT(Function):
    min_args=1
    max_args=3
    opcode=280

class PROGRAM_OF(Function):
    min_args=1
    max_args=1
    opcode=281

class PROJECT(Function):
    min_args=3
    max_args=4
    opcode=282

class PROMOTE(Function):
    min_args=2
    max_args=2
    opcode=283

class PUBLIC(Function):
    min_args=1
    max_args=1
    opcode=284

class QUADWORD(Function):
    min_args=1
    max_args=1
    opcode=285

class QUADWORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=286

class QUALIFIERS_OF(Function):
    min_args=1
    max_args=1
    opcode=287

class RADIX(Function):
    min_args=1
    max_args=1
    opcode=288

class RAMP(Function):
    min_args=0
    max_args=2
    opcode=289

class RANDOM(Function):
    min_args=0
    max_args=2
    opcode=290

class RANDOM_SEED(Function):
    min_args=0
    max_args=1
    opcode=291

class DTYPE_RANGE(Function):
    min_args=2
    max_args=3
    opcode=292

class RANK(Function):
    min_args=1
    max_args=1
    opcode=293

class RAW_OF(Function):
    min_args=1
    max_args=1
    opcode=294

class READ(Function):
    min_args=1
    max_args=1
    opcode=295

class REAL(Function):
    min_args=1
    max_args=2
    opcode=296

class REBIN(Function):
    min_args=2
    max_args=4
    opcode=297

class REF(Function):
    min_args=1
    max_args=1
    opcode=298

class REPEAT(Function):
    min_args=2
    max_args=2
    opcode=299

class REPLICATE(Function):
    min_args=3
    max_args=3
    opcode=300

class RESHAPE(Function):
    min_args=2
    max_args=4
    opcode=301

class RETURN(Function):
    min_args=0
    max_args=1
    opcode=302

class REWIND(Function):
    min_args=1
    max_args=1
    opcode=303

class RMS(Function):
    min_args=1
    max_args=3
    opcode=304

class ROUTINE_OF(Function):
    min_args=1
    max_args=1
    opcode=305

class RRSPACING(Function):
    min_args=1
    max_args=1
    opcode=306

class SCALE(Function):
    min_args=2
    max_args=2
    opcode=307

class SCAN(Function):
    min_args=2
    max_args=3
    opcode=308

class FSEEK(Function):
    min_args=1
    max_args=3
    opcode=309

class SET_EXPONENT(Function):
    min_args=2
    max_args=2
    opcode=310

class SET_RANGE(Function):
    min_args=2
    max_args=9
    opcode=311

class ISHFT(Function):
    min_args=2
    max_args=2
    opcode=312

class ISHFTC(Function):
    min_args=3
    max_args=3
    opcode=313

class SHIFT_LEFT(Function):
    min_args=2
    max_args=2
    opcode=314

class SHIFT_RIGHT(Function):
    min_args=2
    max_args=2
    opcode=315

class SIGN(Function):
    min_args=2
    max_args=2
    name='SIGN'
    opcode=316
SIGN.__name__='SIGN'

class SIGNED(Function):
    min_args=1
    max_args=1
    opcode=317

class SIN(Function):
    min_args=1
    max_args=1
    opcode=318

class SIND(Function):
    min_args=1
    max_args=1
    opcode=319

class SINH(Function):
    min_args=1
    max_args=1
    opcode=320

class SIZEOF(Function):
    min_args=1
    max_args=1
    opcode=321

class SLOPE_OF(Function):
    min_args=1
    max_args=2
    opcode=322

class SMOOTH(Function):
    min_args=2
    max_args=2
    opcode=323

class SOLVE(Function):
    min_args=2
    max_args=2
    opcode=324

class SORTVAL(Function):
    min_args=1
    max_args=2
    opcode=325

class SPACING(Function):
    min_args=1
    max_args=1
    opcode=326

class SPAWN(Function):
    min_args=0
    max_args=3
    opcode=327

class SPREAD(Function):
    min_args=3
    max_args=3
    opcode=328

class SQRT(Function):
    min_args=1
    max_args=1
    opcode=329

class SQUARE(Function):
    min_args=1
    max_args=1
    opcode=330

class STATEMENT(Function):
    min_args=0
    max_args=254
    opcode=331

class STD_DEV(Function):
    min_args=1
    max_args=3
    opcode=332

class STRING(Function):
    min_args=1
    max_args=254
    opcode=333

class STRING_OPCODE(Function):
    min_args=1
    max_args=1
    opcode=334

class SUBSCRIPT(Function):
    min_args=1
    max_args=9
    opcode=335

class SUBTRACT(Function):
    min_args=2
    max_args=2
    opcode=336

class SUM(Function):
    min_args=1
    max_args=3
    opcode=337

class SWITCH(Function):
    min_args=2
    max_args=254
    opcode=338

class SYSTEM_CLOCK(Function):
    min_args=1
    max_args=1
    opcode=339

class TAN(Function):
    min_args=1
    max_args=1
    opcode=340

class TAND(Function):
    min_args=1
    max_args=1
    opcode=341

class TANH(Function):
    min_args=1
    max_args=1
    opcode=342

class TASK_OF(Function):
    min_args=1
    max_args=1
    opcode=343

class TEXT(Function):
    min_args=1
    max_args=2
    opcode=344

class TIME_OUT_OF(Function):
    min_args=1
    max_args=1
    opcode=345

class TINY(Function):
    min_args=1
    max_args=1
    opcode=346

class TRANSFER(Function):
    min_args=2
    max_args=3
    opcode=347

class TRANSPOSE_(Function):
    min_args=1
    max_args=1
    opcode=348

class TRIM(Function):
    min_args=1
    max_args=1
    opcode=349

class UNARY_MINUS(Function):
    min_args=1
    max_args=1
    opcode=350

class UNARY_PLUS(Function):
    min_args=1
    max_args=1
    opcode=351

class UNION(Function):
    min_args=0
    max_args=254
    opcode=352

class UNITS(Function):
    min_args=1
    max_args=1
    opcode=353

class UNITS_OF(Function):
    min_args=1
    max_args=1
    opcode=354

class UNPACK(Function):
    min_args=3
    max_args=3
    opcode=355

class UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=356

class VAL(Function):
    min_args=1
    max_args=1
    opcode=357

class VALIDATION_OF(Function):
    min_args=1
    max_args=1
    opcode=358

class VALUE_OF(Function):
    min_args=1
    max_args=1
    opcode=359

class VAR(Function):
    min_args=1
    max_args=2
    opcode=360

class VECTOR(Function):
    min_args=0
    max_args=254
    opcode=361

class VERIFY(Function):
    min_args=2
    max_args=3
    opcode=362

class WAIT(Function):
    min_args=1
    max_args=1
    opcode=363

class WHEN_OF(Function):
    min_args=1
    max_args=1
    opcode=364

class WHERE(Function):
    min_args=2
    max_args=3
    opcode=365

class WHILE(Function):
    min_args=2
    max_args=254
    opcode=366

class WINDOW_OF(Function):
    min_args=1
    max_args=1
    opcode=367

class WORD(Function):
    min_args=1
    max_args=1
    opcode=368

class WORD_UNSIGNED(Function):
    min_args=1
    max_args=1
    opcode=369

class WRITE(Function):
    min_args=1
    max_args=254
    opcode=370

class ZERO(Function):
    min_args=0
    max_args=2
    opcode=371

class d2PI(Function):
    min_args=0
    max_args=0
    opcode=372

class dNARG(Function):
    min_args=0
    max_args=0
    opcode=373

class ELEMENT(Function):
    min_args=3
    max_args=3
    opcode=374

class RC_DROOP(Function):
    min_args=3
    max_args=3
    opcode=375

class RESET_PRIVATE(Function):
    min_args=0
    max_args=0
    opcode=376

class RESET_PUBLIC(Function):
    min_args=0
    max_args=0
    opcode=377

class SHOW_PRIVATE(Function):
    min_args=0
    max_args=254
    opcode=378

class SHOW_PUBLIC(Function):
    min_args=0
    max_args=254
    opcode=379

class SHOW_VM(Function):
    min_args=0
    max_args=2
    opcode=380

class TRANSLATE(Function):
    min_args=3
    max_args=3
    opcode=381

class TRANSPOSE_MUL(Function):
    min_args=2
    max_args=2
    opcode=382

class UPCASE(Function):
    min_args=1
    max_args=1
    name='UPCASE'
    opcode=383
UPCASE.__name__='UPCASE'

class USING(Function):
    min_args=2
    max_args=4
    opcode=384

class VALIDATION(Function):
    min_args=1
    max_args=1
    opcode=385

class dDEFAULT(Function):
    min_args=0
    max_args=0
    opcode=386

class dEXPT(Function):
    min_args=0
    max_args=0
    opcode=387

class dSHOT(Function):
    min_args=0
    max_args=0
    opcode=388

class GETDBI(Function):
    min_args=1
    max_args=2
    opcode=389

class CULL(Function):
    min_args=1
    max_args=4
    opcode=390

class EXTEND(Function):
    min_args=1
    max_args=4
    name='EXTEND'
    opcode=391
EXTEND.__name__='EXTEND'

class I_TO_X(Function):
    min_args=1
    max_args=2
    opcode=392

class X_TO_I(Function):
    min_args=1
    max_args=2
    opcode=393

class MAP(Function):
    min_args=2
    max_args=2
    opcode=394

class COMPILE_DEPENDENCY(Function):
    min_args=1
    max_args=1
    opcode=395

class DECOMPILE_DEPENDENCY(Function):
    min_args=1
    max_args=1
    opcode=396

class BUILD_CALL(Function):
    min_args=3
    max_args=254
    opcode=397

class ERRORLOGS_OF(Function):
    min_args=1
    max_args=1
    opcode=398

class PERFORMANCE_OF(Function):
    min_args=1
    max_args=1
    opcode=399

class XD(Function):
    min_args=1
    max_args=1
    opcode=400

class CONDITION_OF(Function):
    min_args=1
    max_args=1
    opcode=401

class SORT(Function):
    min_args=1
    max_args=2
    opcode=402

class dTHIS(Function):
    min_args=0
    max_args=0
    opcode=403

class DATA_WITH_UNITS(Function):
    min_args=1
    max_args=1
    opcode=404

class dATM(Function):
    min_args=0
    max_args=0
    opcode=405

class dEPSILON0(Function):
    min_args=0
    max_args=0
    opcode=406

class dGN(Function):
    min_args=0
    max_args=0
    opcode=407

class dMU0(Function):
    min_args=0
    max_args=0
    opcode=408

class EXTRACT(Function):
    min_args=3
    max_args=3
    opcode=409

class FINITE(Function):
    min_args=1
    max_args=1
    opcode=410

class BIT_SIZE(Function):
    min_args=1
    max_args=1
    opcode=411

class MODULO(Function):
    min_args=2
    max_args=2
    opcode=412

class SELECTED_INT_KIND(Function):
    min_args=1
    max_args=1
    opcode=413

class SELECTED_REAL_KIND(Function):
    min_args=1
    max_args=2
    opcode=414

class DSQL(Function):
    min_args=1
    max_args=254
    opcode=415

class ISQL(Function):
    min_args=1
    max_args=1
    opcode=416

class FTELL(Function):
    min_args=1
    max_args=1
    opcode=417

class MAKE_ACTION(Function):
    min_args=2
    max_args=5
    opcode=418

class MAKE_CONDITION(Function):
    min_args=2
    max_args=2
    opcode=419

class MAKE_CONGLOM(Function):
    min_args=4
    max_args=4
    opcode=420

class MAKE_DEPENDENCY(Function):
    min_args=3
    max_args=3
    opcode=421

class MAKE_DIM(Function):
    min_args=2
    max_args=2
    opcode=422

class MAKE_DISPATCH(Function):
    min_args=5
    max_args=5
    opcode=423

class MAKE_FUNCTION(Function):
    min_args=1
    max_args=254
    opcode=424

class MAKE_METHOD(Function):
    min_args=3
    max_args=254
    opcode=425

class MAKE_PARAM(Function):
    min_args=3
    max_args=3
    opcode=426

class MAKE_PROCEDURE(Function):
    min_args=3
    max_args=254
    opcode=427

class MAKE_PROGRAM(Function):
    min_args=2
    max_args=2
    opcode=428

class MAKE_RANGE(Function):
    min_args=2
    max_args=3
    opcode=429

class MAKE_ROUTINE(Function):
    min_args=3
    max_args=254
    opcode=430

class MAKE_SIGNAL(Function):
    min_args=2
    max_args=10
    opcode=431

class MAKE_WINDOW(Function):
    min_args=3
    max_args=3
    opcode=432

class MAKE_WITH_UNITS(Function):
    min_args=2
    max_args=2
    opcode=433

class MAKE_CALL(Function):
    min_args=3
    max_args=254
    opcode=434

class CLASS_OF(Function):
    min_args=1
    max_args=1
    opcode=435

class DSCPTR_OF(Function):
    min_args=1
    max_args=2
    opcode=436

class KIND_OF(Function):
    min_args=1
    max_args=1
    opcode=437

class NDESC_OF(Function):
    min_args=1
    max_args=1
    opcode=438

class ACCUMULATE(Function):
    min_args=1
    max_args=3
    opcode=439

class MAKE_SLOPE(Function):
    min_args=1
    max_args=254
    opcode=440

class REM(Function):
    min_args=1
    max_args=254
    opcode=441

class COMPLETION_MESSAGE_OF(Function):
    min_args=1
    max_args=1
    opcode=442

class INTERRUPT_OF(Function):
    min_args=1
    max_args=1
    opcode=443

class dSHOTNAME(Function):
    min_args=0
    max_args=0
    opcode=444

class BUILD_WITH_ERROR(Function):
    min_args=2
    max_args=2
    opcode=445

class ERROR_OF(Function):
    min_args=1
    max_args=1
    opcode=446

class MAKE_WITH_ERROR(Function):
    min_args=2
    max_args=2
    opcode=447

class DO_TASK(Function):
    min_args=1
    max_args=1
    opcode=448

class ISQL_SET(Function):
    min_args=1
    max_args=3
    opcode=449

class FS_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=450

class FS_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=451

class FT_FLOAT(Function):
    min_args=1
    max_args=1
    opcode=452

class FT_COMPLEX(Function):
    min_args=1
    max_args=2
    opcode=453

class BUILD_OPAQUE(Function):
    min_args=2
    max_args=2
    opcode=454

class MAKE_OPAQUE(Function):
    min_args=2
    max_args=2
    opcode=455

from inspect import isclass as _isclass
_c=None
for _c in globals().values():
    if _isclass(_c) and issubclass(_c,Function) and _c.__name__ != 'Function':
        Function.opcodeToClass[_c.opcode]=_c
