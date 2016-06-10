def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_data=_mimport('mdsdata')
_dtypes=_mimport('_mdsdtypes')
_Exceptions=_mimport('mdsExceptions')

class Compound(_data.Data):
    def __init__(self,*args, **params):
        """MDSplus compound data.
        """
        if self.__class__.__name__=='Compound':
            raise TypeError("Cannot create instances of class Compound")
        if 'args' in params:
            args=params['args']
        if 'params' in params:
            params=params['params']
        if not hasattr(self,'_dtype'):
            self._dtype=_dtypes.fromName('DTYPE_'+self.__class__.__name__.upper())
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

#    def decompile(self):
#        arglist=list()
#        for arg in self.args:
#            arglist.append(makeData(arg).decompile())
#        return 'Build_'+self.__class__.__name__+'('+','.join(arglist)+')'

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


class MetaClass(type):

    def __new__(meta,classname,bases,classDict):
        if len(classDict)==0:
            classDict=dict(bases[0].__dict__)
            newClassDict=classDict
            newClassDict['_fields']=dict()
            idx=0
            if 'fields' in classDict:
                for f in classDict['fields']:
                    name=f[0:1].upper()+f[1:]
                    exec ("def get"+name+"(self): return self.__getattr__('"+f+"')")
                    exec ("newClassDict['get'+name]=get"+name)
                    newClassDict['get'+name].__doc__='Get the '+f+' field\n@rtype: Data'
                    exec ('def set'+name+'(self,value): return self.__setattr__("'+f+'",value)')
                    exec ("newClassDict['set'+name]=set"+name)
                    newClassDict['set'+name].__doc__='Set the '+f+' field\n@type value: Data\n@rtype: None'
                    exec ("newClassDict['_dtype']=_dtypes.DTYPE_"+classname.upper())
                    newClassDict['_fields'][f]=idx
                    idx=idx+1
                c=type.__new__(meta,classname,bases,newClassDict)
        else:
            c=type.__new__(meta,classname,bases,classDict)
        return c

class _Action(Compound):
    """
    An Action is used for describing an operation to be performed by an
    MDSplus action server. Actions are typically dispatched using the
    mdstcl DISPATCH command
    """
    fields=('dispatch','task','errorLog','completionMessage','performance')
Action=MetaClass('Action',(_Action,),{})

class _Call(Compound):
    """
    A Call is used to call routines in shared libraries.
    """
    fields=('image','routine')
    _opcode_name='retType'
Call=MetaClass('Call',(_Call,),{})

class _Conglom(Compound):
    """A Conglom is used at the head of an MDSplus conglomerate. A conglomerate is a set of tree nodes used
    to define a device such as a piece of data acquisition hardware. A conglomerate is associated with some
    external code providing various methods which can be performed on the device. The Conglom class contains
    information used for locating the external code.
    """
    fields=('image','model','name','qualifiers')
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
Conglom=MetaClass('Conglom',(_Conglom,),{})

class _Dependency(Compound):
    """A Dependency object is used to describe action dependencies. This is a legacy class and may not be recognized by
    some dispatching systems
    """
    fields=('arg1','arg2')
Dependency=MetaClass('Dependency',(_Dependency,),{})

class _Dimension(Compound):
    """A dimension object is used to describe a signal dimension, typically a time axis. It provides a compact description
    of the timing information of measurements recorded by devices such as transient recorders. It associates a Window
    object with an axis. The axis is generally a range with possibly no start or end but simply a delta. The Window
    object is then used to bracket the axis to resolve the appropriate timestamps.
    """
    fields=('window','axis')
Dimension=MetaClass('Dimension',(_Dimension,),{})

class _Dispatch(Compound):
    """A Dispatch object is used to describe when an where an action should be dispatched to an MDSplus action server.
    """
    fields=('ident','phase','when','completion')

    def __init__(self,*args,**kwargs):
        if 'dispatch_type' not in kwargs:
            kwargs['dispatch_type']=2
        kwargs['opcode']=kwargs['dispatch_type']
        super(_Dispatch,self).__init__(args=args,params=kwargs)
        if self.completion is None:
           self.completion = None
Dispatch=MetaClass('Dispatch',(_Dispatch,),{})

class _Function(Compound):
    """A Function object is used to reference builtin MDSplus functions. For example the expression 1+2
    is represented in as Function instance created by Function(opcode='ADD',args=(1,2))
    """
    fields=tuple()

    def __init__(self,opcode,args):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
#        from _opcodes.opcodes import find_opcode
        super(Function,self).__init__(args=args,opcode=opcode)
#        opc=find_opcode(self._opcode)
#        if opc:
#            opc.check_args(args)
#            self._opcode=opc.number
#        else:
#            raise Exception("Invalid opcode - "+str(self._opcode))
#        self.__dict__['opc']=opc

#    def setOpcode(self,opcode):
#        """Set opcode
#        @param opcode: either a string or a index number of the builtin operation
#        @type opcode: str,int
#        """
#        from _opcodes.opcodes import find_opcode
#        opc=find_opcode(opcode)
#        if not opc:
#            raise Exception("Invalid opcode - "+str(opcode))
#        self.opcode=opcode
#        self.__dict__['opc']=opc

#
# The following code can be used if we want to implement TDI opcodes in python code using the opcodes.py module.
# If it is commened out, it will default to using TdiEvaluate to perform the evaluation.
#
#    def evaluate(self):
#        """Returns the answer when the function is evaluated."""
#        return self.opc.evaluate(self.args)
#
#    def decompile(self):
#            return self.opc.str(self.args)

#    def __str__(self):
#        """Returns the string representation of the function."""
#        return self.opc.str(self.args)
#
#def compile_function(name,*args):
#
#    opcode=find_opcode(name)
#    if opcode:
#        if opcode.class_of:
#            return opcode.class_of(*args)
#        else:
#            return opc(opcode.number,*args)
#    else:
#        return opc('BUILD_CALL',None,*args)
Function=MetaClass('Function',(_Function,),{})

class _Method(Compound):
    """A Method object is used to describe an operation to be performed on an MDSplus conglomerate/device
    """
    fields=('timeout','method','object')
Method=MetaClass('Method',(_Method,),{})

class _Procedure(Compound):
    """A Procedure is a deprecated object
    """
    fields=('timeout','language','procedure')
Procedure=MetaClass('Procedure',(_Procedure,),{})

class _Program(Compound):
    """A Program is a deprecated object"""
    fields=('timeout','program')
Program=MetaClass('Program',(_Program,),{})

class _Range(Compound):
    """A Range describes a ramp. When used as an axis in a Dimension object along with a Window object it can be
    used to describe a clock. In this context it is possible to have missing begin and ending values or even have the
    begin, ending, and delta fields specified as arrays to indicate a multi-speed clock.
    """
    fields=('begin','ending','delta')

    def decompile(self):
        parts=list()
        for arg in self.args:
            parts.append(_data.makeData(arg).decompile())
        return ' : '.join(parts)
Range=MetaClass('Range',(_Range,),{})

class _Routine(Compound):
    """A Routine is a deprecated object"""
    fields=('timeout','image','routine')
Routine=MetaClass('Routine',(_Routine,),{})

class _Signal(Compound):
    """A Signal is used to describe a measurement, usually time dependent, and associated the data with its independent
    axis (Dimensions). When Signals are indexed using s[idx], the index is resolved using the dimension of the signal
    """
    fields=('value','raw')

    def _getDims(self):
        return self.getArguments()

    dims=property(_getDims)
    """The dimensions of the signal"""

#    def decompile(self):
#        arglist=list()
#        for arg in self.args:
#            arglist.append(makeData(arg).decompile())
#        return 'Build_Signal('+','.join(arglist)+')'

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
Signal=MetaClass('Signal',(_Signal,),{})

class _Window(Compound):
    """A Window object can be used to construct a Dimension object. It brackets the axis information stored in the
    Dimension to construct the independent axis of a signal.
    """
    fields=('startIdx','endIdx','timeAt0')

#    def decompile(self):
#        return 'Build_Window('+makeData(self.startIdx).decompile()+','+makeData(self.endIdx).decompile()+','+makeData(self.timeAt0)+')'
Window=MetaClass('Window',(_Window,),{})

class _Opaque(Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('data','otype')

#    def decompile(self):
#        return 'Build_Opaque('+makeData(self.data).decompile()+','+makeData(self.otype).decompile()+')'

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
Opaque=MetaClass('Opaque',(_Opaque,),{})
