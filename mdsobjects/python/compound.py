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

import ctypes as _C
import numpy as _N

_ver=_mimport('version')
_dsc=_mimport('descriptor')
_dat=_mimport('mdsdata')
_tre=_mimport('tree')
_exc=_mimport('mdsExceptions')

class Compound(_dat.DataX):
    maxdesc = 255
    fields = tuple()
    def __dir__(self):
        """used for tab completion"""
        return list(self.fields)+_ver.superdir(Compound,self)

    def __init__(self,*args, **kwargs):
        """MDSplus compound data."""
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
            if isinstance(ans,(_dat.Data,_tre.TreeNode)):
                self.setDescAt(i,ans.deref)
        return self

    def __getattr__(self,name):
        if name == '_fields':
            return {}
        if name in self._fields:
            return self.getDescAt(self._fields[name])
        elif name.startswith('get') and name[3:].lower() in self._fields:
            def getter():
                return self.__passTree(self._descs[self._fields[name[3:].lower()]])[0]
            return getter
        elif name.startswith('set') and name[3:].lower() in self._fields:
            def setter(value):
                self.__setattr__(name[3:].lower(),value)
            return setter
        return super(Compound,self).__getattr__(name)
        #raise AttributeError("No such attribute '%s' in %s"%(name,self.__class__.__name__))

    def __setattr__(self,name,value):
        if name in self._fields:
            self.setDescAt(self._fields[name],value)
        else:
            super(Compound,self).__setattr__(name,value)

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

    def setArgumentAt(self,idx,value):
        """Set argument at index idx (indexes start at 0)"""
        return self.setDescAt(self._argOffset+idx, value)

    def setArguments(self,args):
        """Set arguments
        @type args: tuple
        """
        self._descs = self._descs[:self._argOffset+len(args)]
        return self.setDescAt(slice(self._argOffset,None),args)

    def removeTail(self):
       """ removes tailing None args """
       if len(self._descs) <= self._argOffset: return
       for last in range(len(self._descs)-1,self._argOffset-1,-1):
           if self._descs[last] is not None: break
       if last < len(self._descs):
           self._descs = self._descs[:last+1]

    def setDescs(self,args):
        """Set descriptors
        @type args: tuple
        """
        self._descs = [_dat.Data(arg) for arg in args]
        self._setTree(*args)
        while self.getNumDescs()<self._argOffset:
            self._descs.append(None)

    @staticmethod
    def _descriptorWithProps(value,dsc):
        dsc.original=value
        if value._units is not None:
            dunits=WithUnits(None,value._units).descriptor
            dunits.dscptrs[0]=dsc.ptr_
            dunits.array[0]  =dsc
            dunits.tree=value.tree
            dsc = dunits
        if value._error is not None:
            derror=WithError(None,value._error).descriptor
            derror.dscptrs[0]=dsc.ptr_
            derror.array[0]  =dsc
            derror.tree=value.tree
            dsc = derror
        if value._help is not None or value._validation is not None:
            dparam=Parameter(None,value._help,value._validation).descriptor
            dparam.dscptrs[0]=dsc.ptr_
            dparam.array[0]  =dsc
            dparam.tree=value.tree
            dsc = dparam
        return dsc

    @property
    def _descriptor(self):
        d=_dsc.Descriptor_r()
        if hasattr(self,'opcode'):
            d.length = 2
            d.pointer= _C.cast(_C.pointer(_C.c_uint16(self.opcode)),_C.c_void_p)
        d.dtype = self.dtype_id
        d.ndesc = self.getNumDescs()
        # to store the refs of the descriptors to prevent premature gc
        d.array = [None]*d.ndesc
        for idx in _ver.xrange(d.ndesc):
            data = self.getDescAt(idx)
            if data is None:
                d.dscptrs[idx] = None
            else:
                if isinstance(data,_dsc.Descriptor):
                    d.array[idx] = data
                else:
                    d.array[idx] = _dat.Data(data)._descriptor
                d.dscptrs[idx] = d.array[idx].ptr_
        return self._descriptorWithProps(self,d)

    @classmethod
    def fromDescriptor(cls,d):
        args = [_dsc.pointerToObject(d.dscptrs[i],d.tree) for i in _ver.xrange(d.ndesc)]
        ans=cls(*args)
        if d.length>0:
            if d.length == 1:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint8))
            elif d.length == 2:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint16))
            else:
                opcptr=_C.cast(d.pointer,_C.POINTER(_C.c_uint32))
            ans.opcode = opcptr.contents.value
        return ans._setTree(d.tree)

class Action(_dat.TreeRefX,Compound):
    """
    An Action is used for describing an operation to be performed by an
    MDSplus action server. Actions are typically dispatched using the
    mdstcl DISPATCH command
    """
    fields=('dispatch','task','errorLog','completionMessage','performance')
    dtype_id=202
_dsc.addDtypeToClass(Action)

class Call(_dat.TreeRef,Compound):
    """
    A Call is used to call routines in shared libraries.
    """
    fields=('image','routine')
    dtype_id=212
    def setRType(self,data):
        if isinstance(data,_dat.Data) or (isinstance(data,type) and issubclass(data,_dat.Data)):
            self.opcode=data.dtype_id
        else:
            self.opcode = int(data)
        return self
_dsc.addDtypeToClass(Call)

class Conglom(_dat.TreeRefX,Compound):
    """A Conglom is used at the head of an MDSplus conglomerate. A conglomerate is a set of tree nodes used
    to define a device such as a piece of data acquisition hardware. A conglomerate is associated with some
    external code providing various methods which can be performed on the device. The Conglom class contains
    information used for locating the external code.
    """
    fields=('image','model','name','qualifiers')
    dtype_id=200

    @property
    def __model_unique(self):
        model = str(self.model)
        hashval = self.name.data()
        if hashval is None:
            return model,None
        return model, '%s_%016x'%(model,hashval)

    def getDevice(self,*args,**kwargs):
        if not self.image=='__python__':
            raise _exc.DevNOT_A_PYDEVICE
        import imp,sys
        model,unique = self.__model_unique
        if unique is None:
            module = imp.new_module(model)
            qualifiers = self.qualifiers.data()
            if isinstance(qualifiers,_ver.basestring):
                try:    exec(compile(qualifiers,model,'exec'),{'Device':_tre.Device},module.__dict__)
                except: pass
        else:
            if unique in sys.modules:
                module = sys.modules[unique]
            else:
                module = imp.new_module(unique)
                qualifiers = self.qualifiers.data()
                if isinstance(qualifiers,_N.ndarray) and qualifiers.dtype == _N.uint8:
                    qualifiers = qualifiers.tostring()
                elif isinstance(qualifiers,_N.generic):
                    qualifiers = qualifiers.tolist()
                    if isinstance(qualifiers,list):
                        qualifiers = '\n'.join(qualifiers)  # treat as line-by-line
                try:    exec(compile(qualifiers,model,'exec'),module.__dict__,module.__dict__)
                except: pass
                else:   sys.modules[unique] = module
        if model in module.__dict__:
            cls = module.__dict__[model]
        else:
            cls = _tre.Device.PyDevice(model)
        if issubclass(cls,(_tre.Device,)):
            return cls if len(args)+len(kwargs)==0 else cls(*args,**kwargs)
        raise _exc.DevPYDEVICE_NOT_FOUND
_dsc.addDtypeToClass(Conglom)

class Dependency(_dat.TreeRefX,Compound):
    """A Dependency object is used to describe action dependencies. This is a legacy class and may not be recognized by
    some dispatching systems
    """
    fields=('arg1','arg2')
    dtype_id=208
_dsc.addDtypeToClass(Dependency)

class Dimension(_dat.TreeRefX,Compound):
    """A dimension object is used to describe a signal dimension, typically a time axis. It provides a compact description
    of the timing information of measurements recorded by devices such as transient recorders. It associates a Window
    object with an axis. The axis is generally a range with possibly no start or end but simply a delta. The Window
    object is then used to bracket the axis to resolve the appropriate timestamps.
    """
    fields=('window','axis')
    dtype_id=196
_dsc.addDtypeToClass(Dimension)

class Dispatch(_dat.TreeRefX,Compound):
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
_dsc.addDtypeToClass(Dispatch)

_TdiShr=_ver.load_library('TdiShr')

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
        args = [_dsc.pointerToObject(d.dscptrs[i],d.tree) for i in _ver.xrange(d.ndesc)]
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
    @classmethod
    def name(cls):
        if cls.__name__.startswith('d'):
            return '$%s'%cls.__name__[1:]
        return cls.__name__

    def evaluate(self):
        dargs = self._descs
        nargs = len(dargs)
        argslist = (_C.c_void_p*nargs)()
        for i in _ver.xrange(nargs):
            argslist[i] = _C.cast(_dat.Data.pointer(dargs[i]),_C.c_void_p)
        xd = _dsc.Descriptor_xd()
        if isinstance(self,_dat.TreeRef):
            tree = self.tree
            if isinstance(tree,_tre.Tree):
                _exc.checkStatus(_TdiShr._TdiIntrinsic(tree.pctx,self.opcode,nargs,argslist,xd.ref))
                return xd._setTree(tree).value
        _exc.checkStatus(_TdiShr. TdiIntrinsic(               self.opcode,nargs,argslist,xd.ref))
        return xd.value
_dsc.addDtypeToClass(Function)

class Method(_dat.TreeRefX,Compound):
    """A Method object is used to describe an operation to be performed on an MDSplus conglomerate/device
    """
    fields=('timeout','method','object')
    dtype_id=207
_dsc.addDtypeToClass(Method)

class Procedure(_dat.TreeRef,Compound):
    """A Procedure is a deprecated object
    """
    fields=('timeout','language','procedure')
    dtype_id=206
_dsc.addDtypeToClass(Procedure)

class Program(_dat.TreeRef,Compound):
    """A Program is a deprecated object"""
    fields=('timeout','program')
    dtype_id=204
_dsc.addDtypeToClass(Program)

class Range(_dat.TreeRefX,Compound):
    """A Range describes a ramp. When used as an axis in a Dimension object along with a Window object it can be
    used to describe a clock. In this context it is possible to have missing begin and ending values or even have the
    begin, ending, and delta fields specified as arrays to indicate a multi-speed clock.
    """
    fields=('begin','ending','delta')
    dtype_id=201
    @property
    def slice(self):
        return slice(self.begin.data(),self.ending.data(),self.delta.data())
_dsc.addDtypeToClass(Range)

class Routine(_dat.TreeRef,Compound):
    """A Routine is a deprecated object"""
    fields=('timeout','image','routine')
    dtype_id=205
_dsc.addDtypeToClass(Routine)

class Slope(_dat.TreeRefX,Compound):
    """A Slope is a deprecated object. You should use Range instead."""
    fields=('slope','begin','end')
    dtype_id=198
    def slice(self):
        return slice(self.begin.data(),self.end.data(),self.slope.data())
_dsc.addDtypeToClass(Slope)

class Signal(_dat.TreeRefX,Compound):
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
        return _dat.Data.execute('$[$]',self,idx)

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
_dsc.addDtypeToClass(Signal)

class Window(_dat.TreeRefX,Compound):
    """A Window object can be used to construct a Dimension object. It brackets the axis information stored in the
    Dimension to construct the independent axis of a signal.
    """
    fields=('startIdx','endIdx','timeAt0')
    dtype_id=197
_dsc.addDtypeToClass(Window)

class Opaque(_dat.TreeRefX,Compound):
    """An Opaque object containing a binary uint8 array and a string identifying the type.
    """
    fields=('value','otype')
    dtype_id=217


    @property
    def value(self):
        "Data portion of Opaque object"
        return self.getDescAt(0)
    @value.setter
    def value(self,value):
        self.setDescAt(0,value)

    @property
    def image(self):
        "Return image from contents of data portion"
        return self.getImage()

    def getImage(self):
        try: from PIL import Image
        except:       import Image
        if _ver.ispy3:
            from io import BytesIO as io
        else:
            from StringIO import StringIO as io
        return Image.open(io(self.value.data().tostring()))

    @classmethod
    def fromFile(cls,filename,typestring=None):
        """Read a file and return an Opaque object
        @param filename: Name of file to read in
        @type filename: str
        @param typestring: String to denote the type of file being stored. Defaults to file type.
        @type typestring: str
        @rtype: Opaque instance
        """
        import os
        if typestring is None:
            fn, typestring = os.path.splitext(filename)
        f = open(filename,'rb')
        try:
            opq=cls(_dat.Data(_N.fromstring(f.read(),dtype="uint8")),typestring)
        finally:
            f.close()
        return opq
_dsc.addDtypeToClass(Opaque)

class WithUnits(_dat.TreeRefX,Compound):
    """Specifies a units for any kind of data.
    """
    fields=('data','units')
    dtype_id=211
_dsc.addDtypeToClass(WithUnits)

class WithError(_dat.TreeRefX,Compound):
    """Specifies error information for any kind of data.
    """
    fields=('data','error')
    dtype_id=213
_dsc.addDtypeToClass(WithError)

class Parameter(_dat.TreeRefX,Compound):
    """Specifies a help text and validation information for any kind of data.
    """
    fields=('data','help','validation')
    dtype_id=194
_dsc.addDtypeToClass(Parameter)

MAXDIM = 8
class d(Function):
    opcode = 0
    max_args = 0

class dA0(Function):
    opcode = 1
    max_args = 0

class dALPHA(Function):
    opcode = 2
    max_args = 0

class dAMU(Function):
    opcode = 3
    max_args = 0

class dC(Function):
    opcode = 4
    max_args = 0

class dCAL(Function):
    opcode = 5
    max_args = 0

class dDEGREE(Function):
    opcode = 6
    max_args = 0

class dEV(Function):
    opcode = 7
    max_args = 0

class dFALSE(Function):
    opcode = 8
    max_args = 0

class dFARADAY(Function):
    opcode = 9
    max_args = 0

class dG(Function):
    opcode = 10
    max_args = 0

class dGAS(Function):
    opcode = 11
    max_args = 0

class dH(Function):
    opcode = 12
    max_args = 0

class dHBAR(Function):
    opcode = 13
    max_args = 0

class dI(Function):
    opcode = 14
    max_args = 0

class dK(Function):
    opcode = 15
    max_args = 0

class dME(Function):
    opcode = 16
    max_args = 0

class dMISSING(Function):
    opcode = 17
    max_args = 0

class dMP(Function):
    opcode = 18
    max_args = 0

class dN0(Function):
    opcode = 19
    max_args = 0

class dNA(Function):
    opcode = 20
    max_args = 0

class dP0(Function):
    opcode = 21
    max_args = 0

class dPI(Function):
    opcode = 22
    max_args = 0

class dQE(Function):
    opcode = 23
    max_args = 0

class dRE(Function):
    opcode = 24
    max_args = 0

class dROPRAND(Function):
    opcode = 25
    max_args = 0

class dRYDBERG(Function):
    opcode = 26
    max_args = 0

class dT0(Function):
    opcode = 27
    max_args = 0

class dTORR(Function):
    opcode = 28
    max_args = 0

class dTRUE(Function):
    opcode = 29
    max_args = 0

class dVALUE(Function):
    opcode = 30
    max_args = 0

class ABORT(_dat.TreeRefX,Function):
    opcode = 31
    min_args = 0
    max_args = 255

class ABS(_dat.TreeRefX,Function):
    opcode = 32
    min_args = 1
    max_args = 1

class ABS1(_dat.TreeRefX,Function):
    opcode = 33
    min_args = 1
    max_args = 1

class ABSSQ(_dat.TreeRefX,Function):
    opcode = 34
    min_args = 1
    max_args = 1

class ACHAR(_dat.TreeRefX,Function):
    opcode = 35
    min_args = 1
    max_args = 2

class ACOS(_dat.TreeRefX,Function):
    opcode = 36
    min_args = 1
    max_args = 1

class ACOSD(_dat.TreeRefX,Function):
    opcode = 37
    min_args = 1
    max_args = 1

class ADD(_dat.TreeRefX,Function):
    opcode = 38
    min_args = 2
    max_args = 2

class ADJUSTL(_dat.TreeRefX,Function):
    opcode = 39
    min_args = 1
    max_args = 1

class ADJUSTR(_dat.TreeRefX,Function):
    opcode = 40
    min_args = 1
    max_args = 1

class AIMAG(_dat.TreeRefX,Function):
    opcode = 41
    min_args = 1
    max_args = 1

class AINT(_dat.TreeRefX,Function):
    opcode = 42
    min_args = 1
    max_args = 2

class ALL(_dat.TreeRefX,Function):
    opcode = 43
    min_args = 1
    max_args = 2

class ALLOCATED(_dat.TreeRefX,Function):
    opcode = 44
    min_args = 1
    max_args = 1

class AND(_dat.TreeRefX,Function):
    opcode = 45
    min_args = 2
    max_args = 2

class AND_NOT(_dat.TreeRefX,Function):
    opcode = 46
    min_args = 2
    max_args = 2

class ANINT(_dat.TreeRefX,Function):
    opcode = 47
    min_args = 1
    max_args = 2

class ANY(_dat.TreeRefX,Function):
    opcode = 48
    min_args = 1
    max_args = 2

class ARG(_dat.TreeRefX,Function):
    opcode = 49
    min_args = 1
    max_args = 1

class ARGD(_dat.TreeRefX,Function):
    opcode = 50
    min_args = 1
    max_args = 1

class ARG_OF(_dat.TreeRefX,Function):
    opcode = 51
    min_args = 1
    max_args = 2

class ARRAY(_dat.TreeRefX,Function):
    opcode = 52
    min_args = 0
    max_args = 2

class ASIN(_dat.TreeRefX,Function):
    opcode = 53
    min_args = 1
    max_args = 1

class ASIND(_dat.TreeRefX,Function):
    opcode = 54
    min_args = 1
    max_args = 1

class AS_IS(_dat.TreeRefX,Function):
    opcode = 55
    min_args = 1
    max_args = 1

class ATAN(_dat.TreeRefX,Function):
    opcode = 56
    min_args = 1
    max_args = 1

class ATAN2(_dat.TreeRefX,Function):
    opcode = 57
    min_args = 2
    max_args = 2

class ATAN2D(_dat.TreeRefX,Function):
    opcode = 58
    min_args = 2
    max_args = 2

class ATAND(_dat.TreeRefX,Function):
    opcode = 59
    min_args = 1
    max_args = 1

class ATANH(_dat.TreeRefX,Function):
    opcode = 60
    min_args = 1
    max_args = 1

class AXIS_OF(_dat.TreeRefX,Function):
    opcode = 61
    min_args = 1
    max_args = 1

class BACKSPACE(_dat.TreeRefX,Function):
    opcode = 62
    min_args = 1
    max_args = 1

class IBCLR(_dat.TreeRefX,Function):
    opcode = 63
    min_args = 2
    max_args = 2

class BEGIN_OF(_dat.TreeRefX,Function):
    opcode = 64
    min_args = 1
    max_args = 2

class IBITS(_dat.TreeRefX,Function):
    opcode = 65
    min_args = 3
    max_args = 3

class BREAK(Function):
    opcode = 66
    max_args = 0

class BSEARCH(_dat.TreeRefX,Function):
    opcode = 67
    min_args = 2
    max_args = 4

class IBSET(_dat.TreeRefX,Function):
    opcode = 68
    min_args = 2
    max_args = 2

class BTEST(_dat.TreeRefX,Function):
    opcode = 69
    min_args = 2
    max_args = 2

class BUILD_ACTION(_dat.TreeRefX,Function):
    opcode = 70
    min_args = 2
    max_args = 5

class BUILD_CONDITION(_dat.TreeRefX,Function):
    opcode = 71
    min_args = 2
    max_args = 2

class BUILD_CONGLOM(_dat.TreeRefX,Function):
    opcode = 72
    min_args = 4
    max_args = 4

class BUILD_DEPENDENCY(_dat.TreeRefX,Function):
    opcode = 73
    min_args = 3
    max_args = 3

class BUILD_DIM(_dat.TreeRefX,Function):
    opcode = 74
    min_args = 2
    max_args = 2

class BUILD_DISPATCH(_dat.TreeRefX,Function):
    opcode = 75
    min_args = 5
    max_args = 5

class BUILD_EVENT(_dat.TreeRefX,Function):
    opcode = 76
    min_args = 1
    max_args = 1

class BUILD_FUNCTION(_dat.TreeRef,Function):
    opcode = 77
    min_args = 1
    max_args = 254

class BUILD_METHOD(_dat.TreeRefX,Function):
    opcode = 78
    min_args = 3
    max_args = 254

class BUILD_PARAM(_dat.TreeRefX,Function):
    opcode = 79
    min_args = 3
    max_args = 3

class BUILD_PATH(_dat.TreeRefX,Function):
    opcode = 80
    min_args = 1
    max_args = 1

class BUILD_PROCEDURE(_dat.TreeRef,Function):
    opcode = 81
    min_args = 3
    max_args = 254

class BUILD_PROGRAM(_dat.TreeRefX,Function):
    opcode = 82
    min_args = 2
    max_args = 2

class BUILD_RANGE(_dat.TreeRefX,Function):
    opcode = 83
    min_args = 2
    max_args = 3
    def __init__(self,*args):
        if len(args)==1 and args[0] is self: return
        if len(args)==1 and isinstance(args,(slice,)):
            super(BUILD_RANGE,self).__init__(args[0].start,args[0].stop,args[0].step)
        else:
            super(BUILD_RANGE,self).__init__(*args)

class BUILD_ROUTINE(_dat.TreeRef,Function):
    opcode = 84
    min_args = 3
    max_args = 254

class BUILD_SIGNAL(_dat.TreeRefX,Function):
    opcode = 85
    min_args = 2
    max_args = 2+MAXDIM

class BUILD_SLOPE(_dat.TreeRefX,Function):
    opcode = 86
    min_args = 1
    max_args = 254

class BUILD_WINDOW(_dat.TreeRefX,Function):
    opcode = 87
    min_args = 3
    max_args = 3

class BUILD_WITH_UNITS(_dat.TreeRefX,Function):
    opcode = 88
    min_args = 2
    max_args = 2

class BUILTIN_OPCODE(_dat.TreeRefX,Function):
    opcode = 89
    min_args = 1
    max_args = 1

class BYTE(_dat.TreeRefX,Function):
    opcode = 90
    min_args = 1
    max_args = 1

class BYTE_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 91
    min_args = 1
    max_args = 1

class CASE(_dat.TreeRefX,Function):
    opcode = 92
    min_args = 2
    max_args = 254

class CEILING(_dat.TreeRefX,Function):
    opcode = 93
    min_args = 1
    max_args = 1

class CHAR(_dat.TreeRefX,Function):
    opcode = 94
    min_args = 1
    max_args = 2

class CLASS(_dat.TreeRefX,Function):
    opcode = 95
    min_args = 1
    max_args = 1

class FCLOSE(_dat.TreeRefX,Function):
    opcode = 96
    min_args = 1
    max_args = 1

class CMPLX(_dat.TreeRefX,Function):
    opcode = 97
    min_args = 1
    max_args = 3

class COMMA(_dat.TreeRefX,Function):
    opcode = 98
    min_args = 2
    max_args = 254

class COMPILE(_dat.TreeRef,Function):
    opcode = 99
    min_args = 1
    max_args = 254

class COMPLETION_OF(_dat.TreeRefX,Function):
    opcode = 100
    min_args = 1
    max_args = 1

class CONCAT(_dat.TreeRefX,Function):
    opcode = 101
    min_args = 2
    max_args = 254

class CONDITIONAL(_dat.TreeRefX,Function):
    opcode = 102
    min_args = 3
    max_args = 3

class CONJG(_dat.TreeRefX,Function):
    opcode = 103
    min_args = 1
    max_args = 1

class CONTINUE(Function):
    opcode = 104
    max_args = 0

class CONVOLVE(_dat.TreeRefX,Function):
    opcode = 105
    min_args = 2
    max_args = 2

class COS(_dat.TreeRefX,Function):
    opcode = 106
    min_args = 1
    max_args = 1

class COSD(_dat.TreeRefX,Function):
    opcode = 107
    min_args = 1
    max_args = 1

class COSH(_dat.TreeRefX,Function):
    opcode = 108
    min_args = 1
    max_args = 1

class COUNT(_dat.TreeRefX,Function):
    opcode = 109
    min_args = 1
    max_args = 2

class CSHIFT(_dat.TreeRefX,Function):
    opcode = 110
    min_args = 2
    max_args = 3

class CVT(_dat.TreeRefX,Function):
    opcode = 111
    min_args = 2
    max_args = 2

class DATA(_dat.TreeRefX,Function):
    opcode = 112
    min_args = 1
    max_args = 1

class DATE_AND_TIME(_dat.TreeRefX,Function):
    opcode = 113
    min_args = 0
    max_args = 1

class DATE_TIME(_dat.TreeRefX,Function):
    opcode = 114
    min_args = 0
    max_args = 1

class DBLE(_dat.TreeRefX,Function):
    opcode = 115
    min_args = 1
    max_args = 1

class DEALLOCATE(_dat.TreeRefX,Function):
    opcode = 116
    min_args = 0
    max_args = 254

class DEBUG(_dat.TreeRefX,Function):
    opcode = 117
    min_args = 0
    max_args = 1

class DECODE(_dat.TreeRefX,Function):
    opcode = 118
    min_args = 1
    max_args = 2

class DECOMPILE(_dat.TreeRefX,Function):
    opcode = 119
    min_args = 1
    max_args = 2

class DECOMPRESS(_dat.TreeRefX,Function):
    opcode = 120
    min_args = 4
    max_args = 4

class DEFAULT(_dat.TreeRefX,Function):
    opcode = 121
    min_args = 1
    max_args = 254

class DERIVATIVE(_dat.TreeRefX,Function):
    opcode = 122
    min_args = 2
    max_args = 3

class DESCR(_dat.TreeRefX,Function):
    opcode = 123
    min_args = 1
    max_args = 1

class DIAGONAL(_dat.TreeRefX,Function):
    opcode = 124
    min_args = 1
    max_args = 2

class DIGITS(_dat.TreeRefX,Function):
    opcode = 125
    min_args = 1
    max_args = 1

class DIM(_dat.TreeRefX,Function):
    opcode = 126
    min_args = 2
    max_args = 2

class DIM_OF(_dat.TreeRefX,Function):
    opcode = 127
    min_args = 1
    max_args = 2

class DISPATCH_OF(_dat.TreeRefX,Function):
    opcode = 128
    min_args = 1
    max_args = 1

class DIVIDE(_dat.TreeRefX,Function):
    opcode = 129
    min_args = 2
    max_args = 2

class LBOUND(_dat.TreeRefX,Function):
    opcode = 130
    min_args = 1
    max_args = 2

class DO(_dat.TreeRefX,Function):
    opcode = 131
    min_args = 2
    max_args = 254

class DOT_PRODUCT(_dat.TreeRefX,Function):
    opcode = 132
    min_args = 2
    max_args = 2

class DPROD(_dat.TreeRefX,Function):
    opcode = 133
    min_args = 2
    max_args = 2

class DSCPTR(_dat.TreeRefX,Function):
    opcode = 134
    min_args = 1
    max_args = 2

class SHAPE(_dat.TreeRefX,Function):
    opcode = 135
    min_args = 1
    max_args = 2

class SIZE(_dat.TreeRefX,Function):
    opcode = 136
    min_args = 1
    max_args = 2

class KIND(_dat.TreeRefX,Function):
    opcode = 137
    min_args = 1
    max_args = 1

class UBOUND(_dat.TreeRefX,Function):
    opcode = 138
    min_args = 1
    max_args = 2

class D_COMPLEX(_dat.TreeRefX,Function):
    opcode = 139
    min_args = 1
    max_args = 2

class D_FLOAT(_dat.TreeRefX,Function):
    opcode = 140
    min_args = 1
    max_args = 1

class RANGE(_dat.TreeRefX,Function):
    opcode = 141
    min_args = 1
    max_args = 1

class PRECISION(_dat.TreeRefX,Function):
    opcode = 142
    min_args = 1
    max_args = 1

class ELBOUND(_dat.TreeRefX,Function):
    opcode = 143
    min_args = 1
    max_args = 2

class ELSE(Function):
    opcode = 144
    max_args = 0

class ELSEWHERE(Function):
    opcode = 145
    max_args = 0

class ENCODE(_dat.TreeRefX,Function):
    opcode = 146
    min_args = 1
    max_args = 254

class ENDFILE(_dat.TreeRefX,Function):
    opcode = 147
    min_args = 1
    max_args = 1

class END_OF(_dat.TreeRefX,Function):
    opcode = 148
    min_args = 1
    max_args = 2

class EOSHIFT(_dat.TreeRefX,Function):
    opcode = 149
    min_args = 3
    max_args = 4

class EPSILON(_dat.TreeRefX,Function):
    opcode = 150
    min_args = 1
    max_args = 1

class EQ(_dat.TreeRefX,Function):
    opcode = 151
    min_args = 2
    max_args = 2

class EQUALS(_dat.TreeRefX,Function):
    opcode = 152
    min_args = 2
    max_args = 2

class EQUALS_FIRST(_dat.TreeRefX,Function):
    opcode = 153
    min_args = 1
    max_args = 1

class EQV(_dat.TreeRefX,Function):
    opcode = 154
    min_args = 2
    max_args = 2

class ESHAPE(_dat.TreeRefX,Function):
    opcode = 155
    min_args = 1
    max_args = 2

class ESIZE(_dat.TreeRefX,Function):
    opcode = 156
    min_args = 1
    max_args = 2

class EUBOUND(_dat.TreeRefX,Function):
    opcode = 157
    min_args = 1
    max_args = 2

class EVALUATE(_dat.TreeRefX,Function):
    opcode = 158
    min_args = 1
    max_args = 1

class EXECUTE(_dat.TreeRef,Function):
    opcode = 159
    min_args = 1
    max_args = 254

class EXP(_dat.TreeRefX,Function):
    opcode = 160
    min_args = 1
    max_args = 1

class EXPONENT(_dat.TreeRefX,Function):
    opcode = 161
    min_args = 1
    max_args = 1

class EXT_FUNCTION(_dat.TreeRefX,Function):
    opcode = 162
    min_args = 2
    max_args = 254

class FFT(_dat.TreeRefX,Function):
    opcode = 163
    min_args = 2
    max_args = 2

class FIRSTLOC(_dat.TreeRefX,Function):
    opcode = 164
    min_args = 1
    max_args = 2

class FIT(_dat.TreeRefX,Function):
    opcode = 165
    min_args = 2
    max_args = 2

class FIX_ROPRAND(_dat.TreeRefX,Function):
    opcode = 166
    min_args = 2
    max_args = 2

class FLOAT(_dat.TreeRefX,Function):
    opcode = 167
    min_args = 1
    max_args = 2

class FLOOR(_dat.TreeRefX,Function):
    opcode = 168
    min_args = 1
    max_args = 1

class FOR(_dat.TreeRefX,Function):
    opcode = 169
    min_args = 4
    max_args = 254

class FRACTION(_dat.TreeRefX,Function):
    opcode = 170
    min_args = 1
    max_args = 1

class FUN(_dat.TreeRefX,Function):
    opcode = 171
    min_args = 2
    max_args = 254

class F_COMPLEX(_dat.TreeRefX,Function):
    opcode = 172
    min_args = 1
    max_args = 2

class F_FLOAT(_dat.TreeRefX,Function):
    opcode = 173
    min_args = 1
    max_args = 1

class GE(_dat.TreeRefX,Function):
    opcode = 174
    min_args = 2
    max_args = 2

class GETNCI(_dat.TreeRef,Function):
    opcode = 175
    min_args = 2
    max_args = 3

class GOTO(_dat.TreeRefX,Function):
    opcode = 176
    min_args = 1
    max_args = 1

class GT(_dat.TreeRefX,Function):
    opcode = 177
    min_args = 2
    max_args = 2

class G_COMPLEX(_dat.TreeRefX,Function):
    opcode = 178
    min_args = 1
    max_args = 2

class G_FLOAT(_dat.TreeRefX,Function):
    opcode = 179
    min_args = 1
    max_args = 1

class HELP_OF(_dat.TreeRefX,Function):
    opcode = 180
    min_args = 1
    max_args = 1

class HUGE(_dat.TreeRefX,Function):
    opcode = 181
    min_args = 1
    max_args = 1

class H_COMPLEX(_dat.TreeRefX,Function):
    opcode = 182
    min_args = 1
    max_args = 2

class H_FLOAT(_dat.TreeRefX,Function):
    opcode = 183
    min_args = 1
    max_args = 1

class IACHAR(_dat.TreeRefX,Function):
    opcode = 184
    min_args = 1
    max_args = 1

class IAND(_dat.TreeRefX,Function):
    opcode = 185
    min_args = 2
    max_args = 2

class IAND_NOT(_dat.TreeRefX,Function):
    opcode = 186
    min_args = 2
    max_args = 2

class ICHAR(_dat.TreeRefX,Function):
    opcode = 187
    min_args = 1
    max_args = 1

class IDENT_OF(_dat.TreeRefX,Function):
    opcode = 188
    min_args = 1
    max_args = 1

class IF(_dat.TreeRefX,Function):
    opcode = 189
    min_args = 2
    max_args = 3

class IF_ERROR(_dat.TreeRefX,Function):
    opcode = 190
    min_args = 1
    max_args = 254

class IMAGE_OF(_dat.TreeRefX,Function):
    opcode = 191
    min_args = 1
    max_args = 1

class IN(_dat.TreeRefX,Function):
    opcode = 192
    min_args = 1
    max_args = 1

class INAND(_dat.TreeRefX,Function):
    opcode = 193
    min_args = 2
    max_args = 2

class INAND_NOT(_dat.TreeRefX,Function):
    opcode = 194
    min_args = 2
    max_args = 2

class INDEX(_dat.TreeRefX,Function):
    opcode = 195
    min_args = 2
    max_args = 3

class INOR(_dat.TreeRefX,Function):
    opcode = 196
    min_args = 2
    max_args = 2

class INOR_NOT(_dat.TreeRefX,Function):
    opcode = 197
    min_args = 2
    max_args = 2

class INOT(_dat.TreeRefX,Function):
    opcode = 198
    min_args = 1
    max_args = 1

class INOUT(_dat.TreeRefX,Function):
    opcode = 199
    min_args = 1
    max_args = 1

class INQUIRE(_dat.TreeRefX,Function):
    opcode = 200
    min_args = 2
    max_args = 2

class INT(_dat.TreeRefX,Function):
    opcode = 201
    min_args = 1
    max_args = 2

class INTEGRAL(_dat.TreeRefX,Function):
    opcode = 202
    min_args = 2
    max_args = 3

class INTERPOL(_dat.TreeRefX,Function):
    opcode = 203
    min_args = 2
    max_args = 3

class INTERSECT(_dat.TreeRefX,Function):
    opcode = 204
    min_args = 0
    max_args = 254

class INT_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 205
    min_args = 1
    max_args = 1

class INVERSE(_dat.TreeRefX,Function):
    opcode = 206
    min_args = 1
    max_args = 1

class IOR(_dat.TreeRefX,Function):
    opcode = 207
    min_args = 2
    max_args = 2

class IOR_NOT(_dat.TreeRefX,Function):
    opcode = 208
    min_args = 2
    max_args = 2

class IS_IN(_dat.TreeRefX,Function):
    opcode = 209
    min_args = 2
    max_args = 3

class IEOR(_dat.TreeRefX,Function):
    opcode = 210
    min_args = 2
    max_args = 2

class IEOR_NOT(_dat.TreeRefX,Function):
    opcode = 211
    min_args = 2
    max_args = 2

class LABEL(_dat.TreeRefX,Function):
    opcode = 212
    min_args = 2
    max_args = 254

class LAMINATE(_dat.TreeRefX,Function):
    opcode = 213
    min_args = 2
    max_args = 254

class LANGUAGE_OF(_dat.TreeRefX,Function):
    opcode = 214
    min_args = 1
    max_args = 1

class LASTLOC(_dat.TreeRefX,Function):
    opcode = 215
    min_args = 1
    max_args = 2

class LE(_dat.TreeRefX,Function):
    opcode = 216
    min_args = 2
    max_args = 2

class LEN(_dat.TreeRefX,Function):
    opcode = 217
    min_args = 1
    max_args = 1

class LEN_TRIM(_dat.TreeRefX,Function):
    opcode = 218
    min_args = 1
    max_args = 1

class LGE(_dat.TreeRefX,Function):
    opcode = 219
    min_args = 2
    max_args = 2

class LGT(_dat.TreeRefX,Function):
    opcode = 220
    min_args = 2
    max_args = 2

class LLE(_dat.TreeRefX,Function):
    opcode = 221
    min_args = 2
    max_args = 2

class LLT(_dat.TreeRefX,Function):
    opcode = 222
    min_args = 2
    max_args = 2

class LOG(_dat.TreeRefX,Function):
    opcode = 223
    min_args = 1
    max_args = 1

class LOG10(_dat.TreeRefX,Function):
    opcode = 224
    min_args = 1
    max_args = 1

class LOG2(_dat.TreeRefX,Function):
    opcode = 225
    min_args = 1
    max_args = 1

class LOGICAL(_dat.TreeRefX,Function):
    opcode = 226
    min_args = 1
    max_args = 2

class LONG(_dat.TreeRefX,Function):
    opcode = 227
    min_args = 1
    max_args = 1

class LONG_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 228
    min_args = 1
    max_args = 1

class LT(_dat.TreeRefX,Function):
    opcode = 229
    min_args = 2
    max_args = 2

class MATMUL(_dat.TreeRefX,Function):
    opcode = 230
    min_args = 2
    max_args = 2

class MAT_ROT(_dat.TreeRefX,Function):
    opcode = 231
    min_args = 2
    max_args = 5

class MAT_ROT_INT(_dat.TreeRefX,Function):
    opcode = 232
    min_args = 2
    max_args = 5

class MAX(_dat.TreeRefX,Function):
    opcode = 233
    min_args = 2
    max_args = 254

class MAXEXPONENT(_dat.TreeRefX,Function):
    opcode = 234
    min_args = 1
    max_args = 1

class MAXLOC(_dat.TreeRefX,Function):
    opcode = 235
    min_args = 1
    max_args = 3

class MAXVAL(_dat.TreeRefX,Function):
    opcode = 236
    min_args = 1
    max_args = 3

class MEAN(_dat.TreeRefX,Function):
    opcode = 237
    min_args = 1
    max_args = 3

class MEDIAN(_dat.TreeRefX,Function):
    opcode = 238
    min_args = 2
    max_args = 2

class MERGE(_dat.TreeRefX,Function):
    opcode = 239
    min_args = 3
    max_args = 3

class METHOD_OF(_dat.TreeRefX,Function):
    opcode = 240
    min_args = 1
    max_args = 1

class MIN(_dat.TreeRefX,Function):
    opcode = 241
    min_args = 2
    max_args = 254

class MINEXPONENT(_dat.TreeRefX,Function):
    opcode = 242
    min_args = 1
    max_args = 1

class MINLOC(_dat.TreeRefX,Function):
    opcode = 243
    min_args = 1
    max_args = 3

class MINVAL(_dat.TreeRefX,Function):
    opcode = 244
    min_args = 1
    max_args = 3

class MOD(_dat.TreeRefX,Function):
    opcode = 245
    min_args = 2
    max_args = 2

class MODEL_OF(_dat.TreeRefX,Function):
    opcode = 246
    min_args = 1
    max_args = 1

class MULTIPLY(_dat.TreeRefX,Function):
    opcode = 247
    min_args = 2
    max_args = 2

class NAME_OF(_dat.TreeRefX,Function):
    opcode = 248
    min_args = 1
    max_args = 1

class NAND(_dat.TreeRefX,Function):
    opcode = 249
    min_args = 2
    max_args = 2

class NAND_NOT(_dat.TreeRefX,Function):
    opcode = 250
    min_args = 2
    max_args = 2

class NDESC(_dat.TreeRefX,Function):
    opcode = 251
    min_args = 1
    max_args = 1

class NE(_dat.TreeRefX,Function):
    opcode = 252
    min_args = 2
    max_args = 2

class NEAREST(_dat.TreeRefX,Function):
    opcode = 253
    min_args = 2
    max_args = 2

class NEQV(_dat.TreeRefX,Function):
    opcode = 254
    min_args = 2
    max_args = 2

class NINT(_dat.TreeRefX,Function):
    opcode = 255
    min_args = 1
    max_args = 2

class NOR(_dat.TreeRefX,Function):
    opcode = 256
    min_args = 2
    max_args = 2

class NOR_NOT(_dat.TreeRefX,Function):
    opcode = 257
    min_args = 2
    max_args = 2

class NOT(_dat.TreeRefX,Function):
    opcode = 258
    min_args = 1
    max_args = 1

class OBJECT_OF(_dat.TreeRefX,Function):
    opcode = 259
    min_args = 1
    max_args = 1

class OCTAWORD(_dat.TreeRefX,Function):
    opcode = 260
    min_args = 1
    max_args = 1

class OCTAWORD_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 261
    min_args = 1
    max_args = 1

class ON_ERROR(_dat.TreeRefX,Function):
    opcode = 262
    min_args = 1
    max_args = 1

class OPCODE_BUILTIN(_dat.TreeRefX,Function):
    opcode = 263
    min_args = 1
    max_args = 1

class OPCODE_STRING(_dat.TreeRefX,Function):
    opcode = 264
    min_args = 1
    max_args = 1

class FOPEN(_dat.TreeRefX,Function):
    opcode = 265
    min_args = 2
    max_args = 254

class OPTIONAL(_dat.TreeRefX,Function):
    opcode = 266
    min_args = 1
    max_args = 1

class OR(_dat.TreeRefX,Function):
    opcode = 267
    min_args = 2
    max_args = 2

class OR_NOT(_dat.TreeRefX,Function):
    opcode = 268
    min_args = 2
    max_args = 2

class OUT(_dat.TreeRefX,Function):
    opcode = 269
    min_args = 1
    max_args = 1

class PACK(_dat.TreeRefX,Function):
    opcode = 270
    min_args = 2
    max_args = 3

class PHASE_OF(_dat.TreeRefX,Function):
    opcode = 271
    min_args = 1
    max_args = 1

class POST_DEC(_dat.TreeRefX,Function):
    opcode = 272
    min_args = 1
    max_args = 1

class POST_INC(_dat.TreeRefX,Function):
    opcode = 273
    min_args = 1
    max_args = 1

class POWER(_dat.TreeRefX,Function):
    opcode = 274
    min_args = 2
    max_args = 2

class PRESENT(_dat.TreeRefX,Function):
    opcode = 275
    min_args = 1
    max_args = 1

class PRE_DEC(_dat.TreeRefX,Function):
    opcode = 276
    min_args = 1
    max_args = 1

class PRE_INC(_dat.TreeRefX,Function):
    opcode = 277
    min_args = 1
    max_args = 1

class PRIVATE(_dat.TreeRefX,Function):
    opcode = 278
    min_args = 1
    max_args = 1

class PROCEDURE_OF(_dat.TreeRefX,Function):
    opcode = 279
    min_args = 1
    max_args = 1

class PRODUCT(_dat.TreeRefX,Function):
    opcode = 280
    min_args = 1
    max_args = 3

class PROGRAM_OF(_dat.TreeRefX,Function):
    opcode = 281
    min_args = 1
    max_args = 1

class PROJECT(_dat.TreeRefX,Function):
    opcode = 282
    min_args = 3
    max_args = 4

class PROMOTE(_dat.TreeRefX,Function):
    opcode = 283
    min_args = 2
    max_args = 2

class PUBLIC(_dat.TreeRefX,Function):
    opcode = 284
    min_args = 1
    max_args = 1

class QUADWORD(_dat.TreeRefX,Function):
    opcode = 285
    min_args = 1
    max_args = 1

class QUADWORD_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 286
    min_args = 1
    max_args = 1

class QUALIFIERS_OF(_dat.TreeRefX,Function):
    opcode = 287
    min_args = 1
    max_args = 1

class RADIX(_dat.TreeRefX,Function):
    opcode = 288
    min_args = 1
    max_args = 1

class RAMP(_dat.TreeRefX,Function):
    opcode = 289
    min_args = 0
    max_args = 2

class RANDOM(_dat.TreeRefX,Function):
    opcode = 290
    min_args = 0
    max_args = 2

class RANDOM_SEED(_dat.TreeRefX,Function):
    opcode = 291
    min_args = 0
    max_args = 1

class DTYPE_RANGE(_dat.TreeRefX,Function):
    opcode = 292
    min_args = 2
    max_args = 3

class RANK(_dat.TreeRefX,Function):
    opcode = 293
    min_args = 1
    max_args = 1

class RAW_OF(_dat.TreeRefX,Function):
    opcode = 294
    min_args = 1
    max_args = 1

class READ(_dat.TreeRefX,Function):
    opcode = 295
    min_args = 1
    max_args = 1

class REAL(_dat.TreeRefX,Function):
    opcode = 296
    min_args = 1
    max_args = 2

class REBIN(_dat.TreeRefX,Function):
    opcode = 297
    min_args = 2
    max_args = 4

class REF(_dat.TreeRefX,Function):
    opcode = 298
    min_args = 1
    max_args = 1

class REPEAT(_dat.TreeRefX,Function):
    opcode = 299
    min_args = 2
    max_args = 2

class REPLICATE(_dat.TreeRefX,Function):
    opcode = 300
    min_args = 3
    max_args = 3

class RESHAPE(_dat.TreeRefX,Function):
    opcode = 301
    min_args = 2
    max_args = 4

class RETURN(_dat.TreeRefX,Function):
    opcode = 302
    min_args = 0
    max_args = 1

class REWIND(_dat.TreeRefX,Function):
    opcode = 303
    min_args = 1
    max_args = 1

class RMS(_dat.TreeRefX,Function):
    opcode = 304
    min_args = 1
    max_args = 3

class ROUTINE_OF(_dat.TreeRefX,Function):
    opcode = 305
    min_args = 1
    max_args = 1

class RRSPACING(_dat.TreeRefX,Function):
    opcode = 306
    min_args = 1
    max_args = 1

class SCALE(_dat.TreeRefX,Function):
    opcode = 307
    min_args = 2
    max_args = 2

class SCAN(_dat.TreeRefX,Function):
    opcode = 308
    min_args = 2
    max_args = 3

class FSEEK(_dat.TreeRefX,Function):
    opcode = 309
    min_args = 1
    max_args = 3

class SET_EXPONENT(_dat.TreeRefX,Function):
    opcode = 310
    min_args = 2
    max_args = 2

class SET_RANGE(_dat.TreeRefX,Function):
    opcode = 311
    min_args = 2
    max_args = 1+MAXDIM

class ISHFT(_dat.TreeRefX,Function):
    opcode = 312
    min_args = 2
    max_args = 2

class ISHFTC(_dat.TreeRefX,Function):
    opcode = 313
    min_args = 3
    max_args = 3

class SHIFT_LEFT(_dat.TreeRefX,Function):
    opcode = 314
    min_args = 2
    max_args = 2

class SHIFT_RIGHT(_dat.TreeRefX,Function):
    opcode = 315
    min_args = 2
    max_args = 2

class SIGN(_dat.TreeRefX,Function):
    opcode = 316
    min_args = 2
    max_args = 2

class SIGNED(_dat.TreeRefX,Function):
    opcode = 317
    min_args = 1
    max_args = 1

class SIN(_dat.TreeRefX,Function):
    opcode = 318
    min_args = 1
    max_args = 1

class SIND(_dat.TreeRefX,Function):
    opcode = 319
    min_args = 1
    max_args = 1

class SINH(_dat.TreeRefX,Function):
    opcode = 320
    min_args = 1
    max_args = 1

class SIZEOF(_dat.TreeRefX,Function):
    opcode = 321
    min_args = 1
    max_args = 1

class SLOPE_OF(_dat.TreeRefX,Function):
    opcode = 322
    min_args = 1
    max_args = 2

class SMOOTH(_dat.TreeRefX,Function):
    opcode = 323
    min_args = 2
    max_args = 2

class SOLVE(_dat.TreeRefX,Function):
    opcode = 324
    min_args = 2
    max_args = 2

class SORTVAL(_dat.TreeRefX,Function):
    opcode = 325
    min_args = 1
    max_args = 2

class SPACING(_dat.TreeRefX,Function):
    opcode = 326
    min_args = 1
    max_args = 1

class SPAWN(_dat.TreeRefX,Function):
    opcode = 327
    min_args = 0
    max_args = 3

class SPREAD(_dat.TreeRefX,Function):
    opcode = 328
    min_args = 3
    max_args = 3

class SQRT(_dat.TreeRefX,Function):
    opcode = 329
    min_args = 1
    max_args = 1

class SQUARE(_dat.TreeRefX,Function):
    opcode = 330
    min_args = 1
    max_args = 1

class STATEMENT(_dat.TreeRefX,Function):
    opcode = 331
    min_args = 0
    max_args = 254

class STD_DEV(_dat.TreeRefX,Function):
    opcode = 332
    min_args = 1
    max_args = 3

class STRING(_dat.TreeRefX,Function):
    opcode = 333
    min_args = 1
    max_args = 254

class STRING_OPCODE(_dat.TreeRefX,Function):
    opcode = 334
    min_args = 1
    max_args = 1

class SUBSCRIPT(_dat.TreeRefX,Function):
    opcode = 335
    min_args = 1
    max_args = 1+MAXDIM

class SUBTRACT(_dat.TreeRefX,Function):
    opcode = 336
    min_args = 2
    max_args = 2

class SUM(_dat.TreeRefX,Function):
    opcode = 337
    min_args = 1
    max_args = 3

class SWITCH(_dat.TreeRefX,Function):
    opcode = 338
    min_args = 2
    max_args = 254

class SYSTEM_CLOCK(_dat.TreeRefX,Function):
    opcode = 339
    min_args = 1
    max_args = 1

class TAN(_dat.TreeRefX,Function):
    opcode = 340
    min_args = 1
    max_args = 1

class TAND(_dat.TreeRefX,Function):
    opcode = 341
    min_args = 1
    max_args = 1

class TANH(_dat.TreeRefX,Function):
    opcode = 342
    min_args = 1
    max_args = 1

class TASK_OF(_dat.TreeRefX,Function):
    opcode = 343
    min_args = 1
    max_args = 1

class TEXT(_dat.TreeRefX,Function):
    opcode = 344
    min_args = 1
    max_args = 2

class TIME_OUT_OF(_dat.TreeRefX,Function):
    opcode = 345
    min_args = 1
    max_args = 1

class TINY(_dat.TreeRefX,Function):
    opcode = 346
    min_args = 1
    max_args = 1

class TRANSFER(_dat.TreeRefX,Function):
    opcode = 347
    min_args = 2
    max_args = 3

class TRANSPOSE_(_dat.TreeRefX,Function):
    opcode = 348
    min_args = 1
    max_args = 1

class TRIM(_dat.TreeRefX,Function):
    opcode = 349
    min_args = 1
    max_args = 1

class UNARY_MINUS(_dat.TreeRefX,Function):
    opcode = 350
    min_args = 1
    max_args = 1

class UNARY_PLUS(_dat.TreeRefX,Function):
    opcode = 351
    min_args = 1
    max_args = 1

class UNION(_dat.TreeRefX,Function):
    opcode = 352
    min_args = 0
    max_args = 254

class UNITS(_dat.TreeRefX,Function):
    opcode = 353
    min_args = 1
    max_args = 1

class UNITS_OF(_dat.TreeRefX,Function):
    opcode = 354
    min_args = 1
    max_args = 1

class UNPACK(_dat.TreeRefX,Function):
    opcode = 355
    min_args = 3
    max_args = 3

class UNSIGNED(_dat.TreeRefX,Function):
    opcode = 356
    min_args = 1
    max_args = 1

class VAL(_dat.TreeRefX,Function):
    opcode = 357
    min_args = 1
    max_args = 1

class VALIDATION_OF(_dat.TreeRefX,Function):
    opcode = 358
    min_args = 1
    max_args = 1

class VALUE_OF(_dat.TreeRefX,Function):
    opcode = 359
    min_args = 1
    max_args = 1

class VAR(_dat.TreeRefX,Function):
    opcode = 360
    min_args = 1
    max_args = 2

class VECTOR(_dat.TreeRefX,Function):
    opcode = 361
    min_args = 0
    max_args = 254

class VERIFY(_dat.TreeRefX,Function):
    opcode = 362
    min_args = 2
    max_args = 3

class WAIT(_dat.TreeRefX,Function):
    opcode = 363
    min_args = 1
    max_args = 1

class WHEN_OF(_dat.TreeRefX,Function):
    opcode = 364
    min_args = 1
    max_args = 1

class WHERE(_dat.TreeRefX,Function):
    opcode = 365
    min_args = 2
    max_args = 3

class WHILE(_dat.TreeRefX,Function):
    opcode = 366
    min_args = 2
    max_args = 254

class WINDOW_OF(_dat.TreeRefX,Function):
    opcode = 367
    min_args = 1
    max_args = 1

class WORD(_dat.TreeRefX,Function):
    opcode = 368
    min_args = 1
    max_args = 1

class WORD_UNSIGNED(_dat.TreeRefX,Function):
    opcode = 369
    min_args = 1
    max_args = 1

class WRITE(_dat.TreeRefX,Function):
    opcode = 370
    min_args = 1
    max_args = 254

class ZERO(_dat.TreeRefX,Function):
    opcode = 371
    min_args = 0
    max_args = 2

class d2PI(Function):
    opcode = 372
    max_args = 0

class dNARG(Function):
    opcode = 373
    max_args = 0

class ELEMENT(_dat.TreeRefX,Function):
    opcode = 374
    min_args = 3
    max_args = 3

class RC_DROOP(_dat.TreeRefX,Function):
    opcode = 375
    min_args = 3
    max_args = 3

class RESET_PRIVATE(Function):
    opcode = 376
    max_args = 0

class RESET_PUBLIC(Function):
    opcode = 377
    max_args = 0

class SHOW_PRIVATE(_dat.TreeRefX,Function):
    opcode = 378
    min_args = 0
    max_args = 254

class SHOW_PUBLIC(_dat.TreeRefX,Function):
    opcode = 379
    min_args = 0
    max_args = 254

class SHOW_VM(_dat.TreeRefX,Function):
    opcode = 380
    min_args = 0
    max_args = 2

class TRANSLATE(_dat.TreeRefX,Function):
    opcode = 381
    min_args = 3
    max_args = 3

class TRANSPOSE_MUL(_dat.TreeRefX,Function):
    opcode = 382
    min_args = 2
    max_args = 2

class UPCASE(_dat.TreeRefX,Function):
    opcode = 383
    min_args = 1
    max_args = 1

class USING(_dat.TreeRefX,Function):
    opcode = 384
    min_args = 2
    max_args = 4

class VALIDATION(_dat.TreeRefX,Function):
    opcode = 385
    min_args = 1
    max_args = 1

class dDEFAULT(_dat.TreeRef,Function):
    opcode = 386
    max_args = 0

class dEXPT(_dat.TreeRef,Function):
    opcode = 387
    max_args = 0

class dSHOT(_dat.TreeRef,Function):
    opcode = 388
    max_args = 0

class GETDBI(_dat.TreeRefX,Function):
    opcode = 389
    min_args = 1
    max_args = 2

class CULL(_dat.TreeRefX,Function):
    opcode = 390
    min_args = 1
    max_args = 4

class EXTEND(_dat.TreeRefX,Function):
    opcode = 391
    min_args = 1
    max_args = 4

class I_TO_X(_dat.TreeRefX,Function):
    opcode = 392
    min_args = 1
    max_args = 2

class X_TO_I(_dat.TreeRefX,Function):
    opcode = 393
    min_args = 1
    max_args = 2

class MAP(_dat.TreeRefX,Function):
    opcode = 394
    min_args = 2
    max_args = 2

class COMPILE_DEPENDENCY(_dat.TreeRefX,Function):
    opcode = 395
    min_args = 1
    max_args = 1

class DECOMPILE_DEPENDENCY(_dat.TreeRefX,Function):
    opcode = 396
    min_args = 1
    max_args = 1

class BUILD_CALL(_dat.TreeRef,Function):
    opcode = 397
    min_args = 3
    max_args = 254

class ERRORLOGS_OF(_dat.TreeRefX,Function):
    opcode = 398
    min_args = 1
    max_args = 1

class PERFORMANCE_OF(_dat.TreeRefX,Function):
    opcode = 399
    min_args = 1
    max_args = 1

class XD(_dat.TreeRefX,Function):
    opcode = 400
    min_args = 1
    max_args = 1

class CONDITION_OF(_dat.TreeRefX,Function):
    opcode = 401
    min_args = 1
    max_args = 1

class SORT(_dat.TreeRefX,Function):
    opcode = 402
    min_args = 1
    max_args = 2

class dTHIS(Function):
    opcode = 403
    max_args = 0

class DATA_WITH_UNITS(_dat.TreeRefX,Function):
    opcode = 404
    min_args = 1
    max_args = 1

class dATM(Function):
    opcode = 405
    max_args = 0

class dEPSILON0(Function):
    opcode = 406
    max_args = 0

class dGN(Function):
    opcode = 407
    max_args = 0

class dMU0(Function):
    opcode = 408
    max_args = 0

class EXTRACT(_dat.TreeRefX,Function):
    opcode = 409
    min_args = 3
    max_args = 3

class FINITE(_dat.TreeRefX,Function):
    opcode = 410
    min_args = 1
    max_args = 1

class BIT_SIZE(_dat.TreeRefX,Function):
    opcode = 411
    min_args = 1
    max_args = 1

class MODULO(_dat.TreeRefX,Function):
    opcode = 412
    min_args = 2
    max_args = 2

class SELECTED_INT_KIND(_dat.TreeRefX,Function):
    opcode = 413
    min_args = 1
    max_args = 1

class SELECTED_REAL_KIND(_dat.TreeRefX,Function):
    opcode = 414
    min_args = 1
    max_args = 2

class DSQL(_dat.TreeRefX,Function):
    opcode = 415
    min_args = 1
    max_args = 254

class ISQL(_dat.TreeRefX,Function):
    opcode = 416
    min_args = 1
    max_args = 1

class FTELL(_dat.TreeRefX,Function):
    opcode = 417
    min_args = 1
    max_args = 1

class MAKE_ACTION(_dat.TreeRefX,Function):
    opcode = 418
    min_args = 2
    max_args = 5

class MAKE_CONDITION(_dat.TreeRefX,Function):
    opcode = 419
    min_args = 2
    max_args = 2

class MAKE_CONGLOM(_dat.TreeRefX,Function):
    opcode = 420
    min_args = 4
    max_args = 4

class MAKE_DEPENDENCY(_dat.TreeRefX,Function):
    opcode = 421
    min_args = 3
    max_args = 3

class MAKE_DIM(_dat.TreeRefX,Function):
    opcode = 422
    min_args = 2
    max_args = 2

class MAKE_DISPATCH(_dat.TreeRefX,Function):
    opcode = 423
    min_args = 5
    max_args = 5

class MAKE_FUNCTION(_dat.TreeRef,Function):
    opcode = 424
    min_args = 1
    max_args = 254

class MAKE_METHOD(_dat.TreeRefX,Function):
    opcode = 425
    min_args = 3
    max_args = 254

class MAKE_PARAM(_dat.TreeRefX,Function):
    opcode = 426
    min_args = 3
    max_args = 3

class MAKE_PROCEDURE(_dat.TreeRefX,Function):
    opcode = 427
    min_args = 3
    max_args = 254

class MAKE_PROGRAM(_dat.TreeRefX,Function):
    opcode = 428
    min_args = 2
    max_args = 2

class MAKE_RANGE(_dat.TreeRefX,Function):
    opcode = 429
    min_args = 2
    max_args = 3
    def __init__(self,*args):
        if len(args)==1 and args[0] is self: return
        if len(args)==1 and isinstance(args,(slice,)):
            super(BUILD_RANGE,self).__init__(args[0].start,args[0].stop,args[0].step)
        else:
            super(BUILD_RANGE,self).__init__(*args)

class MAKE_ROUTINE(_dat.TreeRef,Function):
    opcode = 430
    min_args = 3
    max_args = 254

class MAKE_SIGNAL(_dat.TreeRefX,Function):
    opcode = 431
    min_args = 2
    max_args = 2+MAXDIM

class MAKE_WINDOW(_dat.TreeRefX,Function):
    opcode = 432
    min_args = 3
    max_args = 3

class MAKE_WITH_UNITS(_dat.TreeRefX,Function):
    opcode = 433
    min_args = 2
    max_args = 2

class MAKE_CALL(_dat.TreeRef,Function):
    opcode = 434
    min_args = 3
    max_args = 254

class CLASS_OF(_dat.TreeRefX,Function):
    opcode = 435
    min_args = 1
    max_args = 1

class DSCPTR_OF(_dat.TreeRefX,Function):
    opcode = 436
    min_args = 1
    max_args = 2

class KIND_OF(_dat.TreeRefX,Function):
    opcode = 437
    min_args = 1
    max_args = 1

class NDESC_OF(_dat.TreeRefX,Function):
    opcode = 438
    min_args = 1
    max_args = 1

class ACCUMULATE(_dat.TreeRefX,Function):
    opcode = 439
    min_args = 1
    max_args = 3

class MAKE_SLOPE(_dat.TreeRefX,Function):
    opcode = 440
    min_args = 1
    max_args = 254

class REM(_dat.TreeRefX,Function):
    opcode = 441
    min_args = 1
    max_args = 254

class COMPLETION_MESSAGE_OF(_dat.TreeRefX,Function):
    opcode = 442
    min_args = 1
    max_args = 1

class INTERRUPT_OF(_dat.TreeRefX,Function):
    opcode = 443
    min_args = 1
    max_args = 1

class dSHOTNAME(_dat.TreeRef,Function):
    opcode = 444
    max_args = 0

class BUILD_WITH_ERROR(_dat.TreeRefX,Function):
    opcode = 445
    min_args = 2
    max_args = 2

class ERROR_OF(_dat.TreeRefX,Function):
    opcode = 446
    min_args = 1
    max_args = 1

class MAKE_WITH_ERROR(_dat.TreeRefX,Function):
    opcode = 447
    min_args = 2
    max_args = 2

class DO_TASK(_dat.TreeRefX,Function):
    opcode = 448
    min_args = 1
    max_args = 1

class ISQL_SET(_dat.TreeRefX,Function):
    opcode = 449
    min_args = 1
    max_args = 3

class FS_FLOAT(_dat.TreeRefX,Function):
    opcode = 450
    min_args = 1
    max_args = 1

class FS_COMPLEX(_dat.TreeRefX,Function):
    opcode = 451
    min_args = 1
    max_args = 2

class FT_FLOAT(_dat.TreeRefX,Function):
    opcode = 452
    min_args = 1
    max_args = 1

class FT_COMPLEX(_dat.TreeRefX,Function):
    opcode = 453
    min_args = 1
    max_args = 2

class BUILD_OPAQUE(_dat.TreeRefX,Function):
    opcode = 454
    min_args = 2
    max_args = 2

class MAKE_OPAQUE(_dat.TreeRefX,Function):
    opcode = 455
    min_args = 2
    max_args = 2

class DICT(_dat.TreeRefX,Function):
    opcode = 456
    min_args = 0
    max_args = 254

class TUPLE(_dat.TreeRefX,Function):
    opcode = 457
    min_args = 0
    max_args = 254

class LIST(_dat.TreeRefX,Function):
    opcode = 458
    min_args = 0
    max_args = 254

from inspect import isclass as _isclass
_c=None
for _c in globals().values():
    if _isclass(_c) and issubclass(_c,Function) and _c.__name__ != 'Function':
        Function.opcodeToClass[_c.opcode]=_c
