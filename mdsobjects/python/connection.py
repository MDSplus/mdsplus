import ctypes as _C
import numpy as _N
from _descriptor import descriptor,descriptor_a
from _mdsshr import _load_library,MdsException,MdsGetMsg
from mdsdata import makeData,Data
from mdsscalar import Scalar
from mdsarray import Array
from _mdsdtypes import *

class Connection(object):
    """Implements an MDSip connection to an MDSplus server"""
    __MdsIpShr=_load_library('MdsIpShr')
    __ConnectToMds=__MdsIpShr.ConnectToMds
    __ConnectToMds.argtypes=[_C.c_char_p]
    __MdsOpen=__MdsIpShr.MdsOpen
    __MdsOpen.argtypes=[_C.c_int,_C.c_char_p,_C.c_int]
    __SendArg=__MdsIpShr.SendArg
    __SendArg.argtypes=[_C.c_int,_C.c_ubyte,_C.c_ubyte,_C.c_ubyte,_C.c_ushort,_C.c_ubyte,_C.c_void_p, _C.c_void_p]
    __GetAnswerInfoTS=__MdsIpShr.GetAnswerInfoTS
    __GetAnswerInfoTS.argtypes=[_C.c_int,_C.POINTER(_C.c_ubyte),_C.POINTER(_C.c_ushort),_C.POINTER(_C.c_ubyte),
                                _C.c_void_p,_C.POINTER(_C.c_ulong),_C.POINTER(_C.c_void_p),_C.POINTER(_C.c_void_p)]
    __mdsipFree=__MdsIpShr.mdsipFree
    __mdsipFree.argtypes=[_C.c_void_p]

    def inspect(self,value):
        d=descriptor(value)
        if d.dtype==DTYPE_DSC:
            if d.pointer.contents.dclass == 4:
                a=_C.cast(d.pointer,_C.POINTER(descriptor_a)).contents
                dims=list()
                if a.dimct == 1:
                    dims.append(a.arsize/a.length)
                else:
                    for i in range(a.dimct):
                        dims.append(a.coeff_and_bounds[i])
                dtype=a.dtype
                length=a.length
                dims=_N.array(dims,dtype=_N.uint32)
                dimct=a.dimct
                pointer=a.pointer
            else:
                raise MdsException,"Error handling argument of type %s" % (type(value),)
        else:
            length=d.length
            dtype=d.dtype
            dims=_N.array(0,dtype=_N.uint32)
            dimct=0
            pointer=d.pointer
        if dtype == DTYPE_FLOAT:
            dtype = DTYPE_F
        elif dtype == DTYPE_DOUBLE:
            dtype = DTYPE_D
        elif dtype == DTYPE_FLOAT_COMPLEX:
            dtype = DTYPE_FC
        elif dtype == DTYPE_DOUBLE_COMPLEX:
            dtype = DTYPE_DC
        return {'dtype':dtype,'length':length,'dimct':dimct,'dims':dims,'address':pointer}

    def sendArg(self,value,idx,num):
        val=makeData(value)
        if not isinstance(val,Scalar) and not isinstance(val,Array):
            val=makeData(val.data())
        valInfo=self.inspect(val)
        status=self.__SendArg(self.socket,idx,valInfo['dtype'],num,valInfo['length'],valInfo['dimct'],valInfo['dims'].ctypes.data,valInfo['address'])
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

    def getAnswer(self):
        dtype=_C.c_ubyte(0)
        length=_C.c_ushort(0)
        ndims=_C.c_ubyte(0)
        dims=_N.array([0,0,0,0,0,0,0,0],dtype=_N.uint32)
        numbytes=_C.c_ulong(0)
        ans=_C.c_void_p(0)
        mem=_C.c_void_p(0)
        status=self.__GetAnswerInfoTS(self.socket,dtype,length,ndims,dims.ctypes.data,numbytes,_C.pointer(ans),_C.pointer(mem))
        dtype=dtype.value
        if dtype == DTYPE_F:
            dtype = DTYPE_FLOAT
        elif dtype == DTYPE_D:
            dtype = DTYPE_DOUBLE
        elif dtype == DTYPE_FC:
            dtype = DTYPE_FLOAT_COMPLEX
        elif dtype == DTYPE_DC:
            dtype = DTYPE_DOUBLE_COMPLEX
        if ndims.value == 0:
            val=descriptor()
            val.dtype=dtype
            val.dclass=1
            val.length=length.value
            val.pointer=_C.cast(ans,_C.POINTER(descriptor))
            ans=val.value
        else:
            val=descriptor_a()
            val.dtype=dtype
            val.dclass=4
            val.length=length.value
            val.pointer=ans
            val.scale=0
            val.digits=0
            val.aflags=0
            val.dimct=ndims.value
            val.arsize=numbytes.value
            val.a0=val.pointer
            if val.dimct > 1:
                val.coeff=1
                for i in range(val.dimct):
                    val.coeff_and_bounds[i]=int(dims[i])
            ans=val.value
        if not ((status & 1) == 1):
            print ans
            if mem.value is not None:
                self.__mdsipFree(mem)
            raise MdsException,MdsGetMsg(status)
        if mem.value is not None:
            self.__mdsipFree(mem)
        return ans
        
    def __init__(self,hostspec):
        self.socket=self.__ConnectToMds(hostspec)
        if self.socket == -1:
            raise Exception,"Error connecting to %s" % (hostspec,)
        self.hostspec=hostspec

    def openTree(self,name,shot):
        status=self.__MdsOpen(self.socket,name,shot)
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

    def value(self,exp,*args):
        num=len(args)+1
        idx=0
        self.sendArg(exp,idx,num)
        for arg in args:
            idx=idx+1
            self.sendArg(arg,idx,num)
        return self.getAnswer()
    
    def put(self,node,exp,*args):
        raise Exception,"Not implemented yet"

    def closeTree(self,*args):
        raise Exception,"Not implemented yet"

    def setDefault(self,path):
        raise Exception,"Not implemented yet"

    def getManyObj(self):
        raise Exception,"Not implemented yet"

    def putManyObj(self):
        raise Exception,"Not implemented yet"
        
        
        
