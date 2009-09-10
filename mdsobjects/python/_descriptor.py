from mdsscalar import makeScalar,Scalar
from mdsarray import makeArray,Array,StringArray
from _mdsdtypes import *
from _mdsclasses import *
from mdsdata import makeData,EmptyData,Data
from treenode import TreeNode,TreePath,TreeNodeArray
from ident import Ident
from apd import Apd,Dictionary,List
from compound import *
from _mdsshr import MdsGetMsg,MdsDecompress,MdsFree1Dx,MdsCopyDxXd
import numpy as _N

import ctypes as _C
import os

class descriptor(_C.Structure):
    __cached_values={}
    indentation=0

    def addToCache(self,value):
        try:
            cache=self.__dict__['_cache']
        except:
            cache=self.__dict__['_cache']=list()
        cache.append(value)
        return

    def nextIdx(self):
        descriptor.__next_idx=descriptor.__next_idx+1
        return descriptor.__next_idx
    
    def __init__(self,value=None):
        if isinstance(value,descriptor):
            self.dtype=value.dtype
            self.dclass=value.dclass
            self.length=value.length
            self.pointer=value.pointer
            print self

        if isinstance(value,descriptor_a):
            self.dtype=DTYPE_DSC
            self.dclass=CLASS_S
            self.length=100000
            self.pointer=_C.cast(_C.pointer(value),_C.POINTER(descriptor))
            return
        
        self.dclass=CLASS_S
        if value is None or isinstance(value,EmptyData):
            self.length=0
            self.dtype=DTYPE_DSC
            self.pointer=None
            return
        if isinstance(value,_N.generic):
            a=_N.array(value)
            self.dtype=mdsdtypes.fromNumpy(value)
            self.length=value.nbytes
            self.pointer=_C.cast(_C.c_void_p(a.ctypes.data),_C.POINTER(descriptor))
            self.addToCache(a)
            return
        if isinstance(value,int):
            self.length=4
            self.dtype=DTYPE_L
            self.pointer=_C.cast(_C.pointer(_C.c_int(value)),_C.POINTER(descriptor))
            self.addToCache(value)
            return
        if isinstance(value,long):
            self.length=8
            self.dtype=DTYPE_Q
            self.pointer=_C.cast(_C.pointer(_C.c_long(value)),_C.POINTER(descriptor))
            self.addToCache(value)
            return
        if isinstance(value,str):
            str_d=descriptor_string(value)
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=d.dtype
            self.pointer=d.pointer
            self.addToCache(value)
            return
        if isinstance(value,float):
            self.length=8
            self.dtype=DTYPE_NATIVE_DOUBLE
            self.pointer=_C.cast(_C.pointer(_C.c_double(value)),_C.POINTER(descriptor))
            self.addToCache(value)
            return
        
        if isinstance(value,dict) or isinstance(value,list) or isinstance(value,tuple):
            value=makeData(value)

        if isinstance(value,_N.ndarray):
            if str(value.dtype)[1:2]=='S':
                for i in range(len(value.flat)):
                    value.flat[i]=value.flat[i].ljust(value.itemsize)
            a=descriptor_a(value.T)
            self.length=10000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(a),_C.POINTER(descriptor))
            self.addToCache(value)
            return

        if (not hasattr(value,'_doing_units')) & hasattr(value,'_units'):
            value._doing_units=True
            r_d=descriptor_r(DTYPE_WITH_UNITS,2)
            r_d.length=0
            r_d.pointer=_C.cast(0,_C.POINTER(descriptor))
            dscs=list()
            dscs.append(descriptor(value))
            r_d.dscptrs[0]=_C.pointer(dscs[0])
            dscs.append(descriptor(value._units))
            r_d.dscptrs[1]=_C.pointer(dscs[1])
            self.length=1000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),_C.POINTER(descriptor))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_units')
            return

        if (not hasattr(value,'_doing_help')) & (hasattr(value,'_help') | hasattr(value,'_validation')):
            value._doing_help=True
            r_d=descriptor_r(DTYPE_PARAM,3)
            r_d.length=0
            r_d.pointer=_C.cast(0,_C.POINTER(descriptor))
            dscs=list()
            dscs.append(descriptor(value))
            r_d.dscptrs[0]=_C.pointer(dscs[0])
            if hasattr(value,'_help'):
                dscs.append(descriptor(value._help))
            else:
                dscs.append(descriptor(None))
            r_d.dscptrs[1]=_C.pointer(dscs[1])
            if hasattr(value,'_validation'):
                dscs.append(descriptor(value._validation))
            else:
                dscs.append(descriptor(None))
            r_d.dscptrs[2]=_C.pointer(dscs[2])
            self.length=1000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),_C.POINTER(descriptor))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_help')
            return
                
        if (not hasattr(value,'_doing_error')) & hasattr(value,'_error'):
            value._doing_error=True
            r_d=descriptor_r(DTYPE_WITH_ERROR,2)
            r_d.length=0
            r_d.pointer=_C.cast(0,_C.POINTER(descriptor))
            dscs=list()
            dscs.append(descriptor(value))
            r_d.dscptrs[0]=_C.pointer(dscs[0])
            dscs.append(descriptor(value._error))
            r_d.dscptrs[1]=_C.pointer(dscs[1])
            self.length=1000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),_C.POINTER(descriptor))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_error')
            return


        if isinstance(value,Scalar):
            self.__init__(value.value)
            return

        if isinstance(value,Array):
            self.__init__(value.value)
            return
        
        if (isinstance(value,Compound)):
            c_d=descriptor_r(value._dtype,len(value.args))
            if value.opcode is None:
                c_d.length=0
                c_d.pointer=_C.cast(0,_C.POINTER(descriptor))
            else:
                c_d.length=2
                c_d.pointer=_C.cast(_C.pointer(_C.c_ushort(value.opcode)),_C.POINTER(descriptor))
            arglist=list()
            for i in range(len(value.args)):
                if value.args[i] is None:
                    c_d.dscptrs[i]=_C.cast(0,_C.POINTER(descriptor))
                else:
                    c_d.dscptrs[i]=_C.pointer(descriptor(value.args[i]))
            self.length=1000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(c_d),_C.POINTER(descriptor))
            self.addToCache(c_d)
            self.addToCache(value)
            return

        if (isinstance(value,TreePath)):
            value.restoreContext()
            str_d=descriptor_string(str(value))
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=DTYPE_PATH
            self.pointer=d.pointer
            self.addToCache(value)
            return

        if (isinstance(value,TreeNode)):
            value.restoreContext()
            self.length=4
            self.dtype=DTYPE_NID
            self.pointer=_C.cast(_C.pointer(_C.c_int(value.nid)),_C.POINTER(descriptor))
            self.addToCache(value)
            return

        if (isinstance(value,TreeNodeArray)):
            value.restoreContext()
            self.__init__(value.nids)
            return

        if isinstance(value,Ident):
            str_d=descriptor_string(value.name)
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=DTYPE_IDENT
            self.pointer=d.pointer
            self.addToCache(value)
            return

        if isinstance(value,Dictionary):
            apd=descriptor(value.toApd())
            self.length=apd.length
            self.dtype=apd.dtype
            self.pointer=apd.pointer
            self.addToCache(value)
            self.addToCache(apd)
            return

        if isinstance(value,List):
            apd=descriptor(value.toApd())
            self.length=apd.length
            self.dtype=apd.dtype
            self.pointer=apd.pointer
            self.addToCache(value)
            self.addToCache(apd)
            return

        if isinstance(value,Apd):
            apd_a=descriptor_apd_a()
            ds=list()
            self.addToCache(ds)
            for i in range(len(value.descs)):
                ds.append(descriptor(value.descs[i]))
                if ds[-1].dtype == DTYPE_DSC:
                    apd_a.dscptrs[i]=ds[-1].pointer
                else:
                    apd_a.dscptrs[i]=_C.pointer(ds[-1])
            apd=descriptor_apd(_C.pointer(apd_a),len(value.descs))
            apd.dtype=value.dtype
            self.length=10000
            self.dtype=DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(apd),_C.POINTER(descriptor))
            self.addToCache(value)
            self.addToCache(apd_a)
            self.addToCache(apd)
            return
        raise TypeError,'Cannot make descriptor of '+str(type(value))
        return

    def __str__(self):
        if (self.length == 4):
            from _tdishr import CvtConvertFloat
            if (self.dtype == DTYPE_F):
                CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int,_C.POINTER(_C.c_float),_C.c_int]
                val=_C.c_float(0)
                CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),DTYPE_F,_C.pointer(val),DTYPE_FS)
                ptrstr=", value="+str(val.value)
            else:
                ptrstr=", value="+str(_C.cast(self.pointer,_C.POINTER(_C.c_uint)).contents.value)
        elif (self.length == 2):
            ptrstr=", value="+str(_C.cast(self.pointer,_C.POINTER(_C.c_ushort)).contents.value)
        else:
            if (bool(self.pointer)==True and (self.dtype == DTYPE_DSC or self.length > 0)):
                ptrstr=", pointer="+str(self.pointer)
            else:
                ptrstr=", pointer=NULL"
        return str().rjust(descriptor.indentation*4)+"length="+str(self.length)+", dtype="+str(mdsdtypes(self.dtype))+", dclass="+str(mdsclasses(self.dclass))+ptrstr

    def _getValue(self):
        def d_contents(dsc):
            try:
                return dsc.contents.value
            except:
                return None
        
        if ((self.dtype == DTYPE_DSC) & (self.dclass != CLASS_APD)):
            try:
                return self.pointer.contents.value
            except ValueError,e:
                return makeData(None)
            
        if (self.dclass == CLASS_S or self.dclass == CLASS_D):
            if (self.dtype == DTYPE_T):
                if self.length == 0:
                    return makeScalar('')
                else:
                    return makeScalar(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value)
            if (self.length == 0):
                return makeData(None)
            try:
                return makeScalar(mdsdtypes(self.dtype).toNumpy()(_C.cast(self.pointer,_C.POINTER(mdsdtypes(self.dtype).toCtype())).contents.value))
            except TypeError:
                from _tdishr import CvtConvertFloat
                if (self.dtype == DTYPE_F):
                    CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int,_C.POINTER(_C.c_float),_C.c_int]
                    val=_C.c_float(0)
                    CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),DTYPE_F,_C.pointer(val),DTYPE_NATIVE_FLOAT)
                    return makeScalar(_N.float32(val.value))
                if (self.dtype == DTYPE_G):
                    CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int,_C.POINTER(_C.c_float),_C.c_int]
                    val=_C.c_float(0)
                    CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),DTYPE_G,_C.pointer(val),DTYPE_NATIVE_FLOAT)
                    return makeScalar(_N.float32(val.value))
                if (self.dtype == DTYPE_D):
                    CvtConvertFloat.argtypes=[_C.POINTER(_C.c_double),_C.c_int,_C.POINTER(_C.c_double),_C.c_int]
                    val=_C.c_double(0)
                    CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_double)),DTYPE_D,_C.pointer(val),DTYPE_NATIVE_DOUBLE)
                    return makeScalar(_N.float64(val.value))
                if (self.dtype == DTYPE_FC):
                    CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int,_C.POINTER(_C.c_float),_C.c_int]
                    val=_C.c_float(0)
                    CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),DTYPE_F,_C.pointer(val),DTYPE_NATIVE_FLOAT)
                    raise "DTYPE_FC is not yet supported"
                    return makeScalar(_N.float32(val.value))
                if (self.dtype == DTYPE_DC):
                    raise "DTYPE_DC is not yet supported"
                    return None
                if (self.dtype == DTYPE_NID):
                    return TreeNode(_C.cast(self.pointer,_C.POINTER(_C.c_long)).contents.value)
                if (self.dtype == DTYPE_PATH):
                    return TreePath(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value)
                if (self.dtype == DTYPE_IDENT):
                    return Ident(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value)
                raise TypeError,'Unsupported data type: (%s,%d)' % (str(mdsdtypes(self.dtype)),self.dtype)
        if (self.dclass == CLASS_R):
            ans = _C.cast(_C.pointer(self),_C.POINTER(descriptor_r)).contents
            if self.dtype == DTYPE_WITH_UNITS:
                ans2=makeData(d_contents(ans.dscptrs[0]))
                ans2._units=d_contents(ans.dscptrs[1])
                return ans2
            if self.dtype == DTYPE_WITH_ERROR:
                ans2=makeData(d_contents(ans.dscptrs[0]))
                ans2._error=d_contents(ans.dscptrs[1])
                return ans2
            if self.dtype == DTYPE_PARAM:
                ans2=makeData(d_contents(ans.dscptrs[0]))
                ans2._help=d_contents(ans.dscptrs[1])
                ans2._validation=d_contents(ans.dscptrs[2])
                return ans2
            if self.dtype == DTYPE_RANGE:
                if (ans.ndesc == 2):
                    return Range(d_contents(ans.dscptrs[0]),d_contents(ans.dscptrs[1]))
                elif (ans.ndesc == 3):
                   return Range(d_contents(ans.dscptrs[0]),d_contents(ans.dscptrs[1]),d_contents(ans.dscptrs[2]))
                else:
                    raise Exception,"Range has must have two or three fields, this range has %d fields" % ans.ndesc
            if self.dtype == DTYPE_SLOPE:
                return Range(d_contents(ans.dscptrs[1]),d_contents(ans.dscptrs[2]),d_contents(ans.dscptrs[0]))
            if self.dtype in  (DTYPE_FUNCTION,DTYPE_CALL,DTYPE_DEPENDENCY):
                if ans.length==0:
                    opcode=None
                else:
                    if ans.length == 1:
                        opcode=_C.cast(ans.pointer,_C.POINTER(_C.c_ubyte)).contents.value
                    else:
                        opcode=_C.cast(ans.pointer,_C.POINTER(_C.c_ushort)).contents.value
                arglist=list()
                for i in range(ans.ndesc):
                    if (bool(ans.dscptrs[i]) == True):
                        arglist.append(ans.dscptrs[i].contents.value)
                    else:
                        arglist.append(None)
                return eval(str(mdsdtypes(self.dtype))[6:].lower().capitalize()+'(args=tuple(arglist),opcode=opcode)')
            if self.dtype in (DTYPE_ACTION,DTYPE_PROCEDURE,DTYPE_DISPATCH,DTYPE_DIMENSION,DTYPE_METHOD,DTYPE_CONGLOM,DTYPE_SIGNAL,DTYPE_PROGRAM,
                              DTYPE_ROUTINE,DTYPE_WINDOW):
                arglist=list()
                for i in range(ans.ndesc):
                    if (bool(ans.dscptrs[i]) == True):
                        arglist.append(ans.dscptrs[i].contents.value)
                    else:
                        arglist.append(None)
                arglist=tuple(arglist)
                return eval(str(mdsdtypes(self.dtype))[6:].lower().capitalize()+'(arglist)')
            raise Exception,"%s is not yet supported" % str(mdsdtypes(ans.dtype))
        if self.dclass == CLASS_A:
            descr = _C.cast(_C.pointer(self),_C.POINTER(descriptor_a)).contents
            if descr.coeff:
                shape=list()
                for i in range(descr.dimct):
                    shape.append(descr.coeff_and_bounds[descr.dimct-i-1])
            else:
                shape=[descr.arsize/descr.length,]
            if self.dtype == DTYPE_T:
                return StringArray(_N.chararray(shape,itemsize=descr.length,buffer=buffer(_C.cast(self.pointer,_C.POINTER(_C.c_char*descr.arsize)).contents.value)))
            if self.dtype == DTYPE_NID:
                self.dtype=DTYPE_L
                nids=makeArray(_N.ndarray(shape=shape,dtype=mdsdtypes(self.dtype).toCtype(),
                                          buffer=buffer(_C.cast(descr.pointer,_C.POINTER(mdsdtypes(self.dtype).toCtype() * (descr.arsize/descr.length))).contents)))
                return TreeNodeArray(nids)
            if self.dtype == DTYPE_F:
                return makeArray(Data.execute("float($)",(descr,)))
            if self.dtype == DTYPE_D or self.dtype == DTYPE_G:
                return makeArray(Data.execute("FT_FLOAT($)",(descr,)))
            try:
                a=_N.ndarray(shape=shape,dtype=mdsdtypes(self.dtype).toCtype(),
                                  buffer=buffer(_C.cast(descr.pointer,_C.POINTER(mdsdtypes(self.dtype).toCtype() * (descr.arsize/descr.length))).contents))
                return makeArray(a)
            except TypeError,e:
                raise TypeError,'Arrays of type %s are unsupported. Error message was: %s' % (str(mdsdtypes(self.dtype)),str(e))
            raise Exception,'Unsupported array type'
        if self.dclass == CLASS_APD:
            descr = _C.cast(_C.pointer(self),_C.POINTER(descriptor_apd)).contents
            apd_a=descr.pointer.contents
            descs=list()
            for i in range(descr.arsize/descr.length):
                descs.append(apd_a.dscptrs[i].contents.value)
            ans=Apd(tuple(descs),descr.dtype)
            if descr.dtype == DTYPE_DICTIONARY:
                return Dictionary(ans)
            elif descr.dtype == DTYPE_LIST:
                return List(ans)
            else:
                return ans

        if self.dclass == CLASS_CA:
            return MdsDecompress(self)
        raise Exception,'Unsupported class: '+str(mdsclasses(self.dclass))
    value = property(_getValue)

    def toXd(self):
        return MdsCopyDxXd(self)

    def copy(self):
        xd=self.toXd()
        if xd.dtype == DTYPE_DSC and xd.l_length > 0:
            ans=_C.cast(xd.pointer,_C.POINTER(descriptor)).contents
            ans.addToCache(xd)
        else:
            ans=descriptor(None)
        return ans

        
descriptor._fields_ = [("length",_C.c_ushort),
                       ("dtype",_C.c_ubyte),
                       ("dclass",_C.c_ubyte),
                       ("pointer",_C.POINTER(descriptor))]


class descriptor_xd(_C.Structure):
    _fields_=descriptor._fields_+[("l_length",_C.c_uint)]
    
    def __init__(self):
        self.l_length=0
        self.length=0
        self.pointer=None
        self.dclass=CLASS_XD

    def __str__(self):
        return str(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+" l_length="+str(self.l_length)

    def _getValue(self):
        return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __del__(self):
	try:
          MdsFree1Dx(self)
        except:
          pass
        
class descriptor_r(_C.Structure):
    if os.name=='nt':
        _fields_=descriptor._fields_+[("ndesc",_C.c_ubyte),("fill1",_C.c_ubyte*4),("dscptrs",_C.POINTER(descriptor)*256)]
    else:
        _fields_=descriptor._fields_+[("ndesc",_C.c_ubyte),("fill1",_C.c_ubyte*3),("dscptrs",_C.POINTER(descriptor)*256)]

    def __init__(self,dtype,ndesc):
        self.length=0
        self.dtype=dtype
        self.dclass=CLASS_R
        self.pointer=None
        self.ndesc=ndesc

    def _getValue(self):
        return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)
    
    def __str__(self):
         ans=str(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", ndesc="+str(self.ndesc)+"\n"
         for i in range(self.ndesc):
             if (bool(self.dscptrs[i])==False):
                 ans=ans+str().rjust(descriptor.indentation*4+4)+"dscptrs["+str(i)+"]=None\n"
             else:
                 descriptor.indentation=descriptor.indentation+1
                 ans=ans+str().rjust(descriptor.indentation*4)+"dscptrs["+str(i)+"]="+str(self.dscptrs[i].contents.value)+"\n"
                 descriptor.indentation=descriptor.indentation-1
         return ans
             
class descriptor_string(_C.Structure):
    _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_char_p)]
    def __init__(self,string):
        self.length=len(string)
        self.dtype=DTYPE_T
        self.dclass=CLASS_S
        self.pointer=string

class descriptor_apd_a(_C.Structure):
    _fields_=[("dscptrs",_C.POINTER(descriptor)*10000000),]

class descriptor_apd(_C.Structure):
    if os.name=='nt':
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.POINTER(descriptor_apd_a)),("scale",_C.c_byte),("digits",_C.c_ubyte),
                  ("aflags",_C.c_ubyte),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.POINTER(descriptor_apd_a)),("coeff_and_bounds",_C.c_int * 24)]
    else:
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.POINTER(descriptor_apd_a)),("scale",_C.c_byte),("digits",_C.c_ubyte),("fill1",_C.c_ushort),
                  ("aflags",_C.c_ubyte),("fill2",_C.c_ubyte * 3),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.POINTER(descriptor_apd_a)),("coeff_and_bounds",_C.c_int * 24)]

    def __init__(self,ptr,num):
        self.dclass=CLASS_APD
        self.scale=0
        self.digits=0
        self.aflags=0
        self.dtype=DTYPE_DSC
        self.length=_C.sizeof(_C.c_void_p)
        self.pointer=ptr
        self.dimct=1
        self.arsize=_C.sizeof(_C.c_void_p)*num
        self.a0=self.pointer
        return

    def __getattr__(self,name):
        if name == 'binscale':
            return not ((self.aflags & 8) == 0)
        elif name == 'redim':
            return not ((self.aflags & 16) == 0)
        elif name == 'column':
            return not ((self.aflags & 32) == 0)
        elif name == 'coeff':
            return not ((self.aflags & 64) == 0)
        elif name == 'bounds':
            return not ((self.aflags & 128) == 0)
        else:
            raise AttributeError
        
#    def __setattr__(self,name,value):
#        if name == 'binscale':
#            if value == 0:
#                self.aflags = self.aflags & ~8
#            else:
#                self.aflags = self.aflags | 8
#            return
#        if name == 'redim':
#            if value == 0:
#                self.aflags = self.aflags & ~16
#            else:
#                self.aflags = self.aflags | 16
#            return
#        if name == 'column':
#            if value == 0:
#                self.aflags = self.aflags & ~32
#            else:
#                self.aflags = self.aflags | 32
#            return
#        if name == 'coeff':
#            if value == 0:
#                self.aflags = self.aflags & ~64
#            else:
#                self.aflags = self.aflags | 64
#            return
#        if name == 'bounds':
#            if value == 0:
#                self.aflags = self.aflags & ~128
#            else:
#                self.aflags = self.aflags | 128
#            return
#        self.__dict__[name]=value
#        return
        
    def _getValue(self):
            return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __str__(self):
        ans=str(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", scale="+str(self.scale)+", digits="+str(self.digits)
        ans=ans+", binscale="+str(self.binscale)+", redim="+str(self.redim)+", column="+str(self.column)
        ans=ans+", coeff="+str(self.coeff)+", bounds="+str(self.bounds)+", dimct="+str(self.dimct)
        ans=ans+", arsize="+str(self.arsize)
        if self.coeff:
            ans=ans+", a0=pointer"
            for i in range(self.dimct):
                ans=ans+", m["+str(i)+"]="+str(self.coeff_and_bounds[i])
            if self.bounds:
                for i in range(self.dimct):
                    ans=ans+", l["+str(i)+"]="+str(self.coeff_and_bounds[i*2+self.dimct])
                    ans=ans+", u["+str(i)+"]="+str(self.coeff_and_bounds[i*2+self.dimct+1])
        return ans
    
class descriptor_a(_C.Structure):
    if os.name=='nt':
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_void_p),("scale",_C.c_byte),("digits",_C.c_ubyte),
                  ("aflags",_C.c_ubyte),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.c_void_p),("coeff_and_bounds",_C.c_int * 24)]
    else:
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_void_p),("scale",_C.c_byte),("digits",_C.c_ubyte),("fill1",_C.c_ushort),
                  ("aflags",_C.c_ubyte),("fill2",_C.c_ubyte * 3),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.c_void_p),("coeff_and_bounds",_C.c_int * 24)]

    def __init__(self,*value):
        if len(value) == 1:
            self.dclass=CLASS_A
            self.scale=0
            self.digits=0
            self.aflags=0
            self.dtype=mdsdtypes.fromNumpy(value[0])
            self.length=value[0].itemsize
            self.pointer=_C.c_void_p(value[0].ctypes.data)
            self.dimct=_N.shape(_N.shape(value[0]))[0]
            self.arsize=value[0].nbytes
            self.a0=self.pointer
            if self.dimct > 1:
                self.coeff=1
                for i in range(self.dimct):
                    self.coeff_and_bounds[i]=_N.shape(value[0])[i]
        return

    def __getattr__(self,name):
        if name == 'binscale':
            return not ((self.aflags & 8) == 0)
        elif name == 'redim':
            return not ((self.aflags & 16) == 0)
        elif name == 'column':
            return not ((self.aflags & 32) == 0)
        elif name == 'coeff':
            return not ((self.aflags & 64) == 0)
        elif name == 'bounds':
            return not ((self.aflags & 128) == 0)
        else:
            return super(descriptor_a,self).__getattr__(name)
        
    def __setattr__(self,name,value):
        if name == 'binscale':
            if value == 0:
                self.aflags = self.aflags & ~8
            else:
                self.aflags = self.aflags | 8
            return
        if name == 'redim':
            if value == 0:
                self.aflags = self.aflags & ~16
            else:
                self.aflags = self.aflags | 16
            return
        if name == 'column':
            if value == 0:
                self.aflags = self.aflags & ~32
            else:
                self.aflags = self.aflags | 32
            return
        if name == 'coeff':
            if value == 0:
                self.aflags = self.aflags & ~64
            else:
                self.aflags = self.aflags | 64
            return
        if name == 'bounds':
            if value == 0:
                self.aflags = self.aflags & ~128
            else:
                self.aflags = self.aflags | 128
            return
        super(descriptor_a,self).__setattr__(name,value)
        return
        
    def _getValue(self):
            return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __str__(self):
        ans=str(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", scale="+str(self.scale)+", digits="+str(self.digits)
        ans=ans+", binscale="+str(self.binscale)+", redim="+str(self.redim)+", column="+str(self.column)
        ans=ans+", coeff="+str(self.coeff)+", bounds="+str(self.bounds)+", dimct="+str(self.dimct)
        ans=ans+", arsize="+str(self.arsize)
        if self.coeff:
            ans=ans+", a0=pointer"
            for i in range(self.dimct):
                ans=ans+", m["+str(i)+"]="+str(self.coeff_and_bounds[i])
            if self.bounds:
                for i in range(self.dimct):
                    ans=ans+", l["+str(i)+"]="+str(self.coeff_and_bounds[i*2+self.dimct])
                    ans=ans+", u["+str(i)+"]="+str(self.coeff_and_bounds[i*2+self.dimct+1])
        return ans

