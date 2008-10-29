from MDSobjects.data import Data,makeData
from MDSobjects._mdsdtypes import *


class Compound(Data):

    def __init__(self,dtype,opcode_name,opcode,argOffset,args):
        """MDSplus compound data.
        """
        if self.__class__.__name__=='Compound':
            raise TypeError,"Cannot create instances of class Compound"
        self._dtype=dtype
        self._opcode_name=opcode_name
        self._opcode=opcode
        self._argOffset=argOffset
        self._fields=dict()
        idx=0
        for name in self.fields:
            self._fields[name]=idx
            idx=idx+1
        if isinstance(args,tuple):
            if len(args) > 0:
                if isinstance(args[0],tuple):
                    args=args[0]
        self.args=args

    def __getField__(self,*args):
        return self._fieldValue

    def getArguments(self):
        return self[self._argOffset:]

    def getArgumentAt(self,n):
        return self[self._argOffset+n]

    def setArguments(self,args):
        self[self._argOffset:]=args

    def setArgumentAt(self,n,arg):
        self[self._argOffset+n]=arg

    def __getattr__(self,name,*args):
        if name in self.__dict__:
            return self.__dict__[name]
        if name == '_opcode_name':
            return 'undefined'
        if name == '_fields':
            return dict()
        if name == '_opcode':
            return None
        if name == '_argOffset':
            return 0
        if name == 'opcode':
            return self._opcode
        if name == self._opcode_name:
            return self._opcode
        if name in self._fields:
            try:
                return self.args[self._fields[name]]
            except:
                return None
        if name[0:3]=='get':
            self._fieldValue=self.__getattr__(name[3:4].lower()+name[4:])
            return self.__getField__
        if name[0:3]=='set':
            self._fieldValue=name[3:4].lower()+name[4:]
            return self.__setField__
        raise AttributeError,'No such attribute '+str(name)

    def __setField__(self,arg):
        self.__setattr__(self._fieldValue,arg)

    def __setattr__(self,name,value):
        if name == 'opcode':
            self._opcode=value
        if name == 'args':
            if isinstance(value,tuple):
                self.__dict__[name]=value
                return
            else:
                raise TypeError,'args attribute must be a tuple'
        if name in self._fields:
            tmp=list(self.args)
            while len(tmp) < self._fields[name]:
                tmp.append(None)
            tmp[self._fields[name]]=value
            self.args=tuple(tmp)
            return
        if name == self._opcode_name:
            self._opcode=value
            return
        super(Compound,self).__setattr__(name,value)

    def getNumDescs(self):
        return len(self.args)

    def getDescAt(self,num):
        return self[num]

    def __getitem__(self,num):
        return self.args[num]

    def setDescAt(self,num,value):
        self[num]=value

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

    def getDescs(self):
        return self.args

    def setDescs(self,value):
        self.args=value

    def makeDoc(cls,fields):
        from pydoc import TextDoc
        import sys
        try:
            if sys.argv[0]=='(imported)':
                def bold(value):
                    return value
            else:
                from pydoc import TextDoc
                tdoc=TextDoc()
                def bold(value):
                    return tdoc.bold(value)
        except:
            def bold(value):
                return value
        def upcase(s):
            return s[0:1].upper()+s[1:]
        
        ans = 'Compound data type\n\nCompound Attributes:\n\n'
        for i in range(len(fields)):
            ans=ans+bold(fields[i])+' = <Data object>\n    '+upcase(fields[i])+' attribute\n\n'
        ans = ans + '\nThe associated method are also provided:\n\n'
        for i in range(len(fields)):
            ans = ans + bold('get'+upcase(fields[i]))+'(self)\n    Return the '+fields[i]+' attribute\n\n'+bold('set'+upcase(fields[i]))+'(self,value)\n  Set the '+fields[i]+' attribute\n\n'
        return ans+'\n'
    makeDoc=classmethod(makeDoc)

class Action(Compound):
    fields=('dispatch','task','errorLog','completionMessage','performance')
    def __init__(self,*args):
        super(Action,self).__init__(DTYPE_ACTION,None,None,0,args)
    __doc__=Compound.makeDoc(fields)

class Call(Compound):
    fields=('image','routine')
    def __init__(self,retType,image=None,routine=None,*args_in):
        if isinstance(retType,tuple):
            args=retType[1:]
            retType=retType[0]
        else:
            args=[image,routine]
            for i in range(len(args_in)):
                args.append(args_in[i])
            args=tuple(args)
        super(Call,self).__init__(DTYPE_CALL,'retType',retType,2,args)
    __doc__=Compound.makeDoc(fields)
    
class Conglom(Compound):
    fields=('image','model','name','qualifiers')
    def __init__(self,*args):
        super(Conglom,self).__init__(DTYPE_CONGLOM,None,None,4,args)
    __doc__=Compound.makeDoc(fields)

class Dependency(Compound):
    fields=('arg1','arg2')
    def __init__(self,opcode,*args):
        super(Dependency,self).__init__(DTYPE_DEPENDENCY,'opcode',opcode,0,args)
    __doc__=Compound.makeDoc(fields)

class Dimension(Compound):
    fields=('window','axis')
    def __init__(self,*args):
        super(Dimension,self).__init__(DTYPE_DIMENSION,None,None,2,args)
    __doc__=Compound.makeDoc(fields)

class Dispatch(Compound):
    fields=('ident','phase','when','completion')
    def __init__(self,*args):
        super(Dispatch,self).__init__(DTYPE_DISPATCH,None,None,4,args)
    __doc__=Compound.makeDoc(fields)

class Function(Compound):
    fields=tuple()
    def __init__(self,opcode,args=None):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        from MDSobjects._opcodes.opcodes import find_opcode
        if (isinstance(opcode,tuple)):
            args=opcode[1:]
            opcode=opcode[0]
        opc=find_opcode(opcode)
        if opc:
            opc.check_args(args)
        else:
            raise Exception("Invalid opcode - "+str(opcode))
        self.__dict__['opc']=opc
        super(Function,self).__init__(DTYPE_FUNCTION,'opcode',opcode,0,args)


    def setOpcode(self,opcode):
        from MDSobjects._opcodes.opcodes import find_opcode
        opc=find_opcode(opcode)
        if not opc:
            raise Exception("Invalid opcode - "+str(opcode))
        self.opcode=opcode
        self.__dict__['opc']=opc
        
#
# The following code can be used if we want to implement TDI opcodes in python code using the opcodes.py module.
# If it is commened out, it will default to using TdiEvaluate to perform the evaluation.
#
#    def evaluate(self):
#        """Returns the answer when the function is evaluated."""
#        return self.opc.evaluate(self.args)
#
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

class Method(Compound):
    fields=('timeout','method','object')
    def __init__(self,*args):
        super(Method,self).__init__(DTYPE_METHOD,None,None,3,args)
    __doc__=Compound.makeDoc(fields)

class Procedure(Compound):
    fields=('timeout','language','procedure')
    def __init__(self,*args):
        super(Procedure,self).__init__(DTYPE_PROCEDURE,None,None,3,args)
    __doc__=Compound.makeDoc(fields)

class Program(Compound):
    fields=('timeout','program')
    def __init__(self,*args):
        super(Program,self).__init__(DTYPE_PROGRAM,None,None,2,args)
    __doc__=Compound.makeDoc(fields)

class Range(Compound):
    fields=('begin','ending','delta')
    def __init__(self,*args):
        super(Range,self).__init__(DTYPE_RANGE,None,None,3,args)
    __doc__=Compound.makeDoc(fields)

class Routine(Compound):
    fields=('timeout','image','routine')
    def __init__(self,*args):
        super(Routine,self).__init__(DTYPE_ROUTINE,None,None,3,args)
    __doc__=Compound.makeDoc(fields)

class Signal(Compound):
    fields=('value','raw')
    def __init__(self,*args):
        super(Signal,self).__init__(DTYPE_SIGNAL,None,None,2,args)

    def _getDims(self):
        return self.args[2:]

    dims=property(_getDims)
        
    def dim_of(self,idx):
        if idx < len(self.dims):
            return self.dims[idx]
        else:
            return makeData(None)

    def getDimension(self,idx):
        return self[2+idx]

    def getDimensions(self,idx):
        return self[2:]

    def setDimension(self,idx,value):
        self[2+idx]=value

    def setDimensions(self,value):
        self[2:]=value
    __doc__=Compound.makeDoc(fields)

class Window(Compound):
    fields=('startIdx','endIdx','timeAt0')
    def __init__(self,*args):
        super(Window,self).__init__(DTYPE_WINDOW,None,None,3,args)
    __doc__=Compound.makeDoc(fields)
