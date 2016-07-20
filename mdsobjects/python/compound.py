import ctypes as _C

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_data=_mimport('mdsdata')
_Exceptions=_mimport('mdsExceptions')
descriptor=_mimport('descriptor')

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
        if 'opcode' in params:
            self._opcode=params['opcode']
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
        if name in self.__dict__:
            return self.__dict__[name]
        if name == '_dtype':
            return self.dtype_id
        if name == '_opcode_name':
            return 'undefined'
        if name == '_fields':
            return dict()
        if name == 'args':
            try:
                return self.__dict__[name]
            except:
                return None
        if name == '_opcode':
            return None
        if name == '_argOffset':
            return 0
        if name == 'opcode':
            return self._opcode
        if name == 'dtype':
            return self._dtype
        if name == self._opcode_name:
            return self._opcode
        if name in self._fields:
            try:
                return self.args[self._fields[name]]
            except:
                return None
        if name.startswith('get') and name[3:].lower() in self._fields:
            def getter():
                return self.args[self._fields[name[3:].lower()]]
            return getter
        if name.startswith('set') and name[3:].lower() in self._fields:
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
        if name == 'opcode':
            self._opcode=value
        if name == 'args':
            if isinstance(value,tuple):
                self.__dict__[name]=value
                return
            else:
                raise TypeError('args attribute must be a tuple')
        if name in self._fields:
            tmp=list(self.args)
            while len(tmp) <= self._fields[name]:
                tmp.append(None)
            tmp[self._fields[name]]=value
            self.args=tuple(tmp)
            return
        if name == self._opcode_name:
            self._opcode=value
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
        dpt=_C.POINTER(descriptor.Descriptor)
        if hasattr(value,'_units') and value._units is not None:
            dunits=descriptor.Descriptor_r()
            dunits.length=0
            dunits.dtype=WithUnits.dtype_id
            dunits.pointer=_C.c_void_p(0)
            dunits.ndesc=2
            dunits.dscptrs[0]=_C.cast(_C.pointer(d),dpt)
            dunits.dscptrs[1]=_C.cast(_C.pointer(_data.makeData(value.units).descriptor),dpt)
            dunits.original=d
        else:
            dunits=d
        if hasattr(value,'_error') and value._error is not None:
            derror=descriptor.Descriptor_r()
            derror.length=0
            derror.dtype=WithError.dtype_id
            derror.pointer=_C.c_void_p(0)
            derror.ndesc=2
            derror.dscptrs[0]=_C.cast(_C.pointer(dunits),dpt)
            derror.dscptrs[1]=_C.cast(_C.pointer(_data.makeData(value._error).descriptor),dpt)
            derror.original=dunits
        else:
            derror=dunits
        if (hasattr(value,'_help') and value._help is not None) or (hasattr(value,'_validation') and value._validation is not None):
            dparam=descriptor.Descriptor_r()
            dparam.length=0
            dparam.dtype=Parameter.dtype_id
            dparam.pointer=_C.c_void_p(0)
            dparam.ndesc=3
            dparam.dscptrs[0]=_C.cast(_C.pointer(derror),dpt)
            if hasattr(value,'_help') and value._help is not None:
                dparam.dscptrs[1]=_C.cast(_C.pointer(_data.makeData(value._help).descriptor),dpt)
            else:
                dparam.dscptrs[1]=_C.cast(_C.c_void_p(0),dpt)
            if hasattr(value,'_validation') and value._validation is not None:
                dparam.dscptrs[2]=_C.cast(_C.pointer(_data.makeData(value._validation).descriptor),dpt)
            else:
                dparam.dscptrs[2]=_C.cast(_C.c_void_p(0),dpt)
            dparam.original=derror
        else:
            dparam=derror
        return dparam

    @property
    def descriptor(self):
        d=descriptor.Descriptor_r()
        if self.opcode is None:
            d.length=0
            d.pointer=_C.c_void_p(0)
        else:
            d.length=2
            d.pointer=_C.cast(_C.pointer(_C.c_uint16(self.opcode)),_C.c_void_p)
        d.dtype=self.dtype_id
        d.ndesc=len(self.args)
        for idx in range(len(self.args)):
            if self.args[idx] is None:
                d.dscptrs[idx]=_C.cast(_C.c_void_p(0),type(d.dscptrs[idx]))
            else:
                d.dscptrs[idx]=_C.cast(_C.pointer(_data.makeData(self.args[idx]).descriptor),_C.POINTER(descriptor.Descriptor))
        d.original=self
        return self.descriptorWithProps(self,d)


    @classmethod
    def fromDescriptor(cls,d):
        if d.pointer is None:
            opcode=None
        else:
            opcode=_C.cast(d.pointer,_C.POINTER(_C.c_uint16)).contents.value
        args=[]
        for i in range(d.ndesc):
            if _C.cast(d.dscptrs[i],_C.c_void_p).value is None:
                args.append(None)
            else:
                args.append(d.dscptrs[i].contents.value)
        args=tuple(args)
        return cls(opcode=opcode,args=args)
            
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
    _opcode_name='retType'
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
            raise _Exceptions.DevPYDEVICE_NOT_FOUND
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
        if 'dispatch_type' not in kwargs:
            kwargs['dispatch_type']=2
        kwargs['opcode']=kwargs['dispatch_type']
        super(_Dispatch,self).__init__(args=args,params=kwargs)
        if self.completion is None:
           self.completion = None

class Function(Compound):
    """A Function object is used to reference builtin MDSplus functions. For example the expression 1+2
    is represented in as Function instance created by Function(opcode='ADD',args=(1,2))
    """
    fields=tuple()
    dtype_id=199

    def __init__(self,opcode,args):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        super(Function,self).__init__(args=args,opcode=opcode)

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

descriptor.dtypeToClass[Action.dtype_id]=Action
descriptor.dtypeToClass[Call.dtype_id]=Call
descriptor.dtypeToClass[Conglom.dtype_id]=Conglom
descriptor.dtypeToClass[Dependency.dtype_id]=Dependency
descriptor.dtypeToClass[Dimension.dtype_id]=Dimension
descriptor.dtypeToClass[Dispatch.dtype_id]=Dispatch
descriptor.dtypeToClass[Function.dtype_id]=Function
descriptor.dtypeToClass[Method.dtype_id]=Method
descriptor.dtypeToClass[Procedure.dtype_id]=Procedure
descriptor.dtypeToClass[Program.dtype_id]=Program
descriptor.dtypeToClass[Range.dtype_id]=Range
descriptor.dtypeToClass[Routine.dtype_id]=Routine
descriptor.dtypeToClass[Signal.dtype_id]=Signal
descriptor.dtypeToClass[Window.dtype_id]=Window
descriptor.dtypeToClass[Opaque.dtype_id]=Opaque
descriptor.dtypeToClass[WithError.dtype_id]=WithError
descriptor.dtypeToClass[WithUnits.dtype_id]=WithUnits
descriptor.dtypeToClass[Parameter.dtype_id]=Parameter

