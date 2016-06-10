def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C
import numpy as _N
import struct as _struct
import os as _os
import sys as _sys

_dtypes=_mimport('_mdsdtypes')
_mdsclasses=_mimport('_mdsclasses')
_data=_mimport('mdsdata')
_ident=_mimport('ident')
_apd=_mimport('apd')
_compound=_mimport('compound')
_mdsshr=_mimport('_mdsshr')
_ver=_mimport('version')
_array=_mimport('mdsarray')
_scalar=_mimport('mdsscalar')
# _tdi=_mimport('tdibuiltins') <- import in _getValue: prevents circle dependency in builtin due to import in python 2.4


def pointerToObject(pointer):
    if pointer == 0:
        return None
    else:
        d=descriptor()
        d.dtype=_dtypes.DTYPE_DSC
        d.dclass=_mdsclasses.CLASS_S
        d.length=100000
        d.pointer=_C.cast(pointer,type(d.pointer))
        return d.value


class descriptor(_C.Structure):
    __cached_values={}
    indentation=0
    tree=None

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
            return

        self.dclass=_mdsclasses.CLASS_S
        if isinstance(value,descriptor_a):
            self.dtype=_dtypes.DTYPE_DSC
            self.length=100000
            self.pointer=_C.cast(_C.pointer(value),type(self.pointer))
            return

        if value is None or isinstance(value,_data.EmptyData):
            self.length=0
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=None
            return

        if isinstance(value,_C.Array):
            try:
                self.__init__(_N.ctypeslib.as_array(value))
                return
            except Exception:
                pass

        if isinstance(value,_N.generic):
            if isinstance(value,_N.unicode_):
                value = value.astype('S')
            a=_N.array(value)
            self.dtype=_dtypes.mdsdtypes.fromNumpy(value)
            self.length=value.nbytes
            self.pointer=_C.cast(_C.c_void_p(a.ctypes.data),type(self.pointer))
            self.addToCache(a)
            return

        if isinstance(value,dict) or isinstance(value,list) or isinstance(value,tuple):
            value=_data.makeData(value)

        if isinstance(value,_N.ndarray):
            if str(value.dtype)[1] in 'SU':
                value = value.astype('S')
                for i in range(len(value.flat)):
                    value.flat[i]=value.flat[i].ljust(value.itemsize)
            if not value.flags['CONTIGUOUS']:
                value=_N.ascontiguousarray(value)
            a=descriptor_a(value)
            self.length=10000
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(a),type(self.pointer))
            self.addToCache(value)
            return

        if (not hasattr(value,'_doing_units')) & hasattr(value,'_units'):
            value._doing_units=True
            r_d=descriptor_r(_dtypes.DTYPE_WITH_UNITS,2)
            r_d.length=0
            r_d.pointer=_C.cast(_C.c_void_p(0),type(r_d.pointer))
            dscs=list()
            dscs.append(descriptor(value))
            r_d.dscptrs[0]=_C.cast(_C.pointer(dscs[0]),type(r_d.dscptrs[0]))
            dscs.append(descriptor(value._units))
            r_d.dscptrs[1]=_C.cast(_C.pointer(dscs[1]),type(r_d.dscptrs[1]))
            self.length=1000
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),type(self.pointer))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_units')
            return

        if (not hasattr(value,'_doing_help')) & (hasattr(value,'_help') | hasattr(value,'_validation')):
            value._doing_help=True
            r_d=descriptor_r(_dtypes.DTYPE_PARAM,3)
            r_d.length=0
            r_d.pointer=_C.cast(_C.c_void_p(0),type(r_d.pointer))
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
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),type(self.pointer))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_help')
            return

        if (not hasattr(value,'_doing_error')) & hasattr(value,'_error'):
            value._doing_error=True
            r_d=descriptor_r(_dtypes.DTYPE_WITH_ERROR,2)
            r_d.length=0
            r_d.pointer=_C.cast(_C.c_void_p(0),type(r_d.pointer))
            dscs=list()
            dscs.append(descriptor(value))
            r_d.dscptrs[0]=_C.pointer(dscs[0])
            dscs.append(descriptor(value._error))
            r_d.dscptrs[1]=_C.pointer(dscs[1])
            self.length=1000
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(r_d),type(self.pointer))
            self.addToCache(r_d)
            self.addToCache(value)
            delattr(value,'_doing_error')
            return


        if isinstance(value,_scalar.Scalar):
            self.__init__(value.value)
            return

        if isinstance(value,_array.Array):
            self.__init__(value.value)
            return

        if (isinstance(value,_compound.Compound)):
            c_d=descriptor_r(value._dtype,len(value.args))
            if value.opcode is None:
                c_d.length=0
                c_d.pointer=_C.cast(_C.c_void_p(0),type(c_d.pointer))
            else:
                c_d.length=2
                try:
                    _C_value=_C.c_ushort(value.opcode)
                except:
                    print("Wrong opcode! ",type(value.opcode),value.opcode)
                c_d.pointer=_C.cast(_C.pointer(_C_value),type(c_d.pointer))
            for i in range(len(value.args)):
                if value.args[i] is None:
                    c_d.dscptrs[i]=_C.cast(_C.c_void_p(0),type(c_d.dscptrs[i]))
                else:
                    c_d.dscptrs[i]=_C.pointer(descriptor(value.args[i]))
            self.length=1000
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(c_d),type(self.pointer))
            self.addToCache(c_d)
            self.addToCache(value)
            return

        if (isinstance(value,_treenode.TreePath)):
            value.restoreContext()
            str_d=descriptor_string(str(value))
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=_dtypes.DTYPE_PATH
            self.pointer=d.pointer
            self.addToCache(value)
            return

        if (isinstance(value,_treenode.TreeNode)):
            value.restoreContext()
            self.length=4
            self.dtype=_dtypes.DTYPE_NID
            self.pointer=_C.cast(_C.pointer(_C.c_int32(value.nid)),type(self.pointer))
            self.addToCache(value)
            return

        if (isinstance(value,_treenode.TreeNodeArray)):
            value.restoreContext()
            self.__init__(value.nids)
            return

        if isinstance(value,_ident.Ident):
            str_d=descriptor_string(value.name)
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=_dtypes.DTYPE_IDENT
            self.pointer=d.pointer
            self.addToCache(value)
            return

        if isinstance(value,_apd.Dictionary):
            apd=descriptor(value.toApd())
            self.length=apd.length
            self.dtype=apd.dtype
            self.pointer=apd.pointer
            self.addToCache(value)
            self.addToCache(apd)
            return

        if isinstance(value,_apd.List):
            apd=descriptor(value.toApd())
            self.length=apd.length
            self.dtype=apd.dtype
            self.pointer=apd.pointer
            self.addToCache(value)
            self.addToCache(apd)
            return

        if isinstance(value,_apd.Apd):
            apd_a=(_C.POINTER(descriptor)*len(value.descs))()
            ds=list()
            self.addToCache(ds)
            for i in range(len(value.descs)):
                ds.append(descriptor(value.descs[i]))
                if ds[-1].dtype == _dtypes.DTYPE_DSC:
                    apd_a[i]=ds[-1].pointer
                else:
                    apd_a[i]=_C.pointer(ds[-1])
            apd=descriptor_apd(_C.cast(_C.pointer(apd_a),_C.POINTER(_C.POINTER(descriptor))),len(value.descs))
            apd.dtype=value.dtype
            self.length=10000
            self.dtype=_dtypes.DTYPE_DSC
            self.pointer=_C.cast(_C.pointer(apd),type(self.pointer))
            self.addToCache(value)
            self.addToCache(apd_a)
            self.addToCache(apd)
            return
        if isinstance(value,_ver.long):#must be before int: treat int as long in py3
            self.length=8
            self.dtype=_dtypes.DTYPE_Q
            self.pointer=_C.cast(_C.pointer(_C.c_int64(value)),type(self.pointer))
            self.addToCache(value)
            return
        if isinstance(value,int):
            self.length=4
            self.dtype=_dtypes.DTYPE_L
            self.pointer=_C.cast(_C.pointer(_C.c_int32(value)),type(self.pointer))
            self.addToCache(value)
            return
        if isinstance(value,(_ver.basestring)):
            str_d=descriptor_string(value)
            d=_C.cast(_C.pointer(str_d),_C.POINTER(descriptor)).contents
            self.length=d.length
            self.dtype=d.dtype
            self.pointer=d.pointer
            self.addToCache(value)
            return
        if isinstance(value,float):
            self.length=8
            self.dtype=_dtypes.DTYPE_NATIVE_DOUBLE
            self.pointer=_C.cast(_C.pointer(_C.c_double(value)),type(self.pointer))
            self.addToCache(value)
            return
        if isinstance(value,complex):
            self.length=8
            self.dtype=_dtypes.DTYPE_FLOAT_COMPLEX
            self.pointer=_C.cast(_C.pointer((_C.c_float*2)(value.real,value.imag)),type(self.pointer))
            self.addToCache(value)
            return

        raise TypeError('Cannot make descriptor of '+str(type(value)))
        return

    def __str__(self):
        if (self.length == 4):
            if (self.dtype == _dtypes.DTYPE_F):
                _tdishr._CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int32,_C.POINTER(_C.c_float),_C.c_int32]
                val=_C.c_float(0)
                _tdishr._CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),_dtypes.DTYPE_F,_C.pointer(val),_dtypes.DTYPE_FS)
                ptrstr=", value="+_ver.tostr(val.value)
            else:
                ptrstr=", value="+_ver.tostr(_C.cast(self.pointer,_C.POINTER(_C.c_uint)).contents.value)
        elif (self.length == 2):
            ptrstr=", value="+_ver.tostr(_C.cast(self.pointer,_C.POINTER(_C.c_ushort)).contents.value)
        else:
            if (bool(self.pointer)==True and (self.dtype == _dtypes.DTYPE_DSC or self.length > 0)):
                ptrstr=", pointer="+_ver.tostr(self.pointer)
            else:
                ptrstr=", pointer=NULL"
        return str().rjust(descriptor.indentation*4)+"length="+_ver.tostr(self.length)+", dtype="+_ver.tostr(_dtypes.mdsdtypes(self.dtype))+", dclass="+_ver.tostr(_mdsclasses.mdsclasses(self.dclass))+ptrstr

    def _getValue(self):
        _tdi=_mimport('tdibuiltins')
        def d_contents(dsc):
            try:
                return dsc.contents.value
            except:
                return None

        if ((self.dtype == _dtypes.DTYPE_DSC) & (self.dclass != _mdsclasses.CLASS_APD)):
            try:
                return self.pointer.contents.value
            except ValueError:
                return _data.makeData(None)

        if (self.dclass == _mdsclasses.CLASS_S or self.dclass == _mdsclasses.CLASS_D):
            if (self.dtype == _dtypes.DTYPE_T):
                if self.length == 0:
                    return _scalar.makeScalar('')
                else:
                    return(_scalar.makeScalar(_N.array(_C.cast(self.pointer,_C.POINTER((_C.c_byte*self.length))).contents[:],dtype=_N.uint8).tostring()))
            if (self.dtype == _dtypes.DTYPE_FSC):
                ans=_C.cast(self.pointer,_C.POINTER((_C.c_float*2))).contents
                return _scalar.makeScalar(_N.complex64(complex(ans[0],ans[1])))
            if (self.dtype == _dtypes.DTYPE_FTC):
                ans=_C.cast(self.pointer,_C.POINTER((_C.c_double*2))).contents
                return _scalar.makeScalar(_N.complex128(complex(ans[0],ans[1])))
            if (self.length == 0):
                return _data.makeData(None)
            try:
                return _scalar.makeScalar(_dtypes.mdsdtypes(self.dtype).toNumpy()(_C.cast(self.pointer,_C.POINTER(_dtypes.mdsdtypes(self.dtype).toCtype())).contents.value))
            except TypeError:
                _CvtConvertFloat=_tdishr._CvtConvertFloat
                if (self.dtype == _dtypes.DTYPE_F):
                    _CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int32,_C.POINTER(_C.c_float),_C.c_int32]
                    val=_C.c_float(0)
                    _CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),_dtypes.DTYPE_F,_C.pointer(val),_dtypes.DTYPE_NATIVE_FLOAT)
                    return _scalar.makeScalar(_N.float32(val.value))
                if (self.dtype == _dtypes.DTYPE_G):
                    _CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int32,_C.POINTER(_C.c_float),_C.c_int32]
                    val=_C.c_float(0)
                    _CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),_dtypes.DTYPE_G,_C.pointer(val),_dtypes.DTYPE_NATIVE_FLOAT)
                    return _scalar.makeScalar(_N.float32(val.value))
                if (self.dtype == _dtypes.DTYPE_D):
                    _CvtConvertFloat.argtypes=[_C.POINTER(_C.c_double),_C.c_int32,_C.POINTER(_C.c_double),_C.c_int32]
                    val=_C.c_double(0)
                    _CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_double)),_dtypes.DTYPE_D,_C.pointer(val),_dtypes.DTYPE_NATIVE_DOUBLE)
                    return _scalar.makeScalar(_N.float64(val.value))
                if (self.dtype == _dtypes.DTYPE_FC):
                    _CvtConvertFloat.argtypes=[_C.POINTER(_C.c_float),_C.c_int32,_C.POINTER(_C.c_float),_C.c_int32]
                    val=_C.c_float(0)
                    _CvtConvertFloat(_C.cast(self.pointer,_C.POINTER(_C.c_float)),_dtypes.DTYPE_F,_C.pointer(val),_dtypes.DTYPE_NATIVE_FLOAT)
                    raise Exception("_dtypes.DTYPE_FC is not yet supported")
                    return _scalar.makeScalar(_N.float32(val.value))
                if (self.dtype == _dtypes.DTYPE_DC):
                    raise Exception("_dtypes.DTYPE_DC is not yet supported")
                    return None
                if (self.dtype == _dtypes.DTYPE_NID):
                    return _treenode.TreeNode(_C.cast(self.pointer,_C.POINTER(_C.c_int32)).contents.value,descriptor.tree)
                if (self.dtype == _dtypes.DTYPE_PATH):
                    if descriptor.tree is None:
                      return _treenode.TreePath(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value,_tree.Tree())
                    else:
                       return _treenode.TreePath(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value,descriptor.tree)
                if (self.dtype == _dtypes.DTYPE_IDENT):
                    return _ident.Ident(_C.cast(self.pointer,_C.POINTER(_C.c_char*self.length)).contents.value)
                if (self.dtype == _dtypes.DTYPE_Z):
                    return _tdi.Builtin('$MISSING',tuple())
                raise TypeError('Unsupported data type: (%s,%d)' % (str(_dtypes.mdsdtypes(self.dtype)),self.dtype))
        if (self.dclass == _mdsclasses.CLASS_R):
            ans = _C.cast(_C.pointer(self),_C.POINTER(descriptor_r)).contents
            if self.dtype == _dtypes.DTYPE_WITH_UNITS:
                ans2=_data.makeData(d_contents(ans.dscptrs[0]))
                ans2._units=d_contents(ans.dscptrs[1])
                return ans2
            if self.dtype == _dtypes.DTYPE_WITH_ERROR:
                ans2=_data.makeData(d_contents(ans.dscptrs[0]))
                ans2._error=d_contents(ans.dscptrs[1])
                return ans2
            if self.dtype == _dtypes.DTYPE_PARAM:
                ans2=_data.makeData(d_contents(ans.dscptrs[0]))
                ans2._help=d_contents(ans.dscptrs[1])
                ans2._validation=d_contents(ans.dscptrs[2])
                return ans2
            if self.dtype == _dtypes.DTYPE_RANGE:
                if (ans.ndesc == 2):
                    return _compound.Range(d_contents(ans.dscptrs[0]),d_contents(ans.dscptrs[1]))
                elif (ans.ndesc == 3):
                   return _compound.Range(d_contents(ans.dscptrs[0]),d_contents(ans.dscptrs[1]),d_contents(ans.dscptrs[2]))
                else:
                    raise Exception("Range must have two or three fields, this range has %d fields" % ans.ndesc)
            if self.dtype == _dtypes.DTYPE_SLOPE:
                if ans.ndesc == 0:
                  raise Exception("Slope must have at least one field.")
                if ans.ndesc == 1:
                  return _compound.Range(None, None, d_contents(ans.dscptrs[0]))
                elif ans.ndesc == 2:
                  return _compound.Range(d_contents(ans._dscptrs[1]),None, d_contents(ans.dscptrs[0]))
                else:
                  return _compound.Range(d_contents(ans.dscptrs[1]),d_contents(ans.dscptrs[2]),d_contents(ans.dscptrs[0]))
            if self.dtype in  (_dtypes.DTYPE_FUNCTION,_dtypes.DTYPE_CALL,_dtypes.DTYPE_DEPENDENCY):
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
                if self.dtype == _dtypes.DTYPE_FUNCTION:
                    return _tdi.Builtin(opcode,tuple(arglist))
                else:
                    return _compound.__dict__[str(_dtypes.mdsdtypes(self.dtype))[6:].lower().capitalize()](args=tuple(arglist),opcode=opcode)
            if self.dtype in (_dtypes.DTYPE_ACTION,_dtypes.DTYPE_PROCEDURE,_dtypes.DTYPE_DISPATCH,_dtypes.DTYPE_DIMENSION,_dtypes.DTYPE_METHOD,_dtypes.DTYPE_CONGLOM,_dtypes.DTYPE_SIGNAL,_dtypes.DTYPE_PROGRAM,
                              _dtypes.DTYPE_ROUTINE,_dtypes.DTYPE_WINDOW,_dtypes.DTYPE_OPAQUE):
                arglist=list()
                for i in range(ans.ndesc):
                    if (bool(ans.dscptrs[i]) == True):
                        arglist.append(ans.dscptrs[i].contents.value)
                    else:
                        arglist.append(None)
                arglist=tuple(arglist)
                return _compound.__dict__[str(_dtypes.mdsdtypes(self.dtype))[6:].lower().capitalize()](arglist)
            raise Exception("%s is not yet supported" % str(_dtypes.mdsdtypes(ans.dtype)))
        if self.dclass == _mdsclasses.CLASS_A:
            if self.dtype == _dtypes.DTYPE_Z:
                self.dtype = _dtypes.DTYPE_L
            descr = _C.cast(_C.pointer(self),_C.POINTER(descriptor_a)).contents
            if descr.coeff:
                descr.arsize=descr.length
                shape=list()
                for i in range(descr.dimct):
                    dim=descr.coeff_and_bounds[descr.dimct-i-1]
                    if dim > 0:
                        descr.arsize=descr.arsize*dim
                        shape.append(dim)
            else:
                shape=[int(descr.arsize/descr.length),]
            if self.dtype == _dtypes.DTYPE_T:
                return _array.StringArray(_N.ndarray(shape=shape,dtype=_N.dtype(('S',descr.length)),buffer=_ver.buffer(_C.cast(self.pointer,_C.POINTER(_C.c_byte*descr.arsize)).contents)))
#                return StringArray(_N.chararray(shape,itemsize=descr.length,buffer=buffer(_C.cast(self.pointer,_C.POINTER(_C.c_char*descr.arsize)).contents.value)))
            if self.dtype == _dtypes.DTYPE_NID:
                self.dtype=_dtypes.DTYPE_L
                nids=_array.makeArray(_N.ndarray(shape=shape,dtype=_dtypes.mdsdtypes(self.dtype).toCtype(),
                                          buffer=_ver.buffer(_C.cast(descr.pointer,_C.POINTER(_dtypes.mdsdtypes(self.dtype).toCtype() * int(descr.arsize/descr.length))).contents)))
                return _treenode.TreeNodeArray(nids)
            if self.dtype == _dtypes.DTYPE_F:
                return _array.makeArray(_data.Data.execute("float($)",(descr,)))
            if self.dtype == _dtypes.DTYPE_D or self.dtype == _dtypes.DTYPE_G:
                return _array.makeArray(_data.Data.execute("FT_FLOAT($)",(descr,)))
            if self.dtype == _dtypes.DTYPE_FSC:
                return _array.makeArray(_N.ndarray(shape=shape,
                                            dtype=_N.complex64,
                                            buffer=_ver.buffer(_C.cast(descr.pointer,_C.POINTER(_C.c_float * int(descr.arsize*2/descr.length))).contents)))
            if self.dtype == _dtypes.DTYPE_FTC:
                return _array.makeArray(_N.ndarray(shape=shape,
                                            dtype=_N.complex128,
                                            buffer=_ver.buffer(_C.cast(descr.pointer,_C.POINTER(_C.c_double * int(descr.arsize*2/descr.length))).contents)))
            try:
                a=_N.ndarray(shape=shape,dtype=_dtypes.mdsdtypes(self.dtype).toCtype(),
                                  buffer=_ver.buffer(_C.cast(descr.pointer,_C.POINTER(_dtypes.mdsdtypes(self.dtype).toCtype() * int(descr.arsize/descr.length))).contents))
                return _array.makeArray(a)
            except TypeError:
                raise TypeError('Arrays of type %s are unsupported. Error message was: %s' % (str(_dtypes.mdsdtypes(self.dtype)),str(_sys.exc_info()[1])))
            raise Exception('Unsupported array type')
        if self.dclass == _mdsclasses.CLASS_APD:
            descr = _C.cast(_C.pointer(self),_C.POINTER(descriptor_apd)).contents
            apd_a=_C.cast(descr.pointer,_C.POINTER(_C.POINTER(descriptor)*int(descr.arsize/descr.length))).contents
            descs=list()
            for i in range(int(descr.arsize/descr.length)):
                descs.append(apd_a[i].contents.value)
            ans=_apd.Apd(tuple(descs),descr.dtype)
            if descr.dtype == _dtypes.DTYPE_DICTIONARY:
                return _apd.Dictionary(ans)
            elif descr.dtype == _dtypes.DTYPE_LIST:
                return _apd.List(ans)
            else:
                return ans

        if self.dclass == _mdsclasses.CLASS_CA:
            return _mdsshr.MdsDecompress(self)
        raise Exception('Unsupported class: '+str(_mdsclasses.mdsclasses(self.dclass)))
    value = property(_getValue)

    def toXd(self):
        return _mdsshr.MdsCopyDxXd(self)

    def copy(self):
        xd=self.toXd()
        if xd.dtype == _dtypes.DTYPE_DSC and xd.l_length > 0:
            ans=_C.cast(xd.pointer,_C.POINTER(descriptor)).contents
            ans.addToCache(xd)
        else:
            ans=descriptor(None)
        return ans

    def _addressof(self):
        return _C.addressof(self)
    addressof=property(_addressof)


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
        self.dclass=_mdsclasses.CLASS_XD

    def __str__(self):
        return _ver.tostr(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+" l_length="+str(self.l_length)

    def _getValue(self):
        if self.l_length==0:
          return None
        else:
          return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __del__(self):
        try:
          _mdsshr.MdsFree1Dx(self)
        except:
          pass

class descriptor_r(_C.Structure):
    if _os.name=='nt' and _struct.calcsize("P")==8:
        _fields_=descriptor._fields_+[("ndesc",_C.c_ubyte),("fill1",_C.c_ubyte*6),("dscptrs",_C.POINTER(descriptor)*256)]
    else:
        _fields_=descriptor._fields_+[("ndesc",_C.c_ubyte),("fill1",_C.c_ubyte*3),("dscptrs",_C.POINTER(descriptor)*256)]

    def __init__(self,dtype,ndesc):
        self.length=0
        self.dtype=dtype
        self.dclass=_mdsclasses.CLASS_R
        self.pointer=None
        self.ndesc=ndesc

    def _getValue(self):
        return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __str__(self):
         ans=_ver.tostr(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", ndesc="+_ver.tostr(self.ndesc)+"\n"
         for i in range(self.ndesc):
             if (bool(self.dscptrs[i])==False):
                 ans=ans+str().rjust(descriptor.indentation*4+4)+"dscptrs["+str(i)+"]=None\n"
             else:
                 descriptor.indentation=descriptor.indentation+1
                 ans=ans+str().rjust(descriptor.indentation*4)+"dscptrs["+str(i)+"]="+_ver.tostr(self.dscptrs[i].contents.value)+"\n"
                 descriptor.indentation=descriptor.indentation-1
         return ans

class descriptor_string(_C.Structure):
    _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_char_p)]
    def __init__(self,string):
        self.length=len(string)
        self.dtype=_dtypes.DTYPE_T
        self.dclass=_mdsclasses.CLASS_S
        self.pointer=_ver.tobytes(string)

class descriptor_apd(_C.Structure):
    if _os.name=='nt':
        if _struct.calcsize("P")==4:
            _pack_=1
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.POINTER(_C.POINTER(descriptor))),("scale",_C.c_byte),("digits",_C.c_ubyte),
                  ("aflags",_C.c_ubyte),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.POINTER(_C.POINTER(descriptor))),("coeff_and_bounds",_C.c_int32 * 24)]
    else:
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.POINTER(_C.POINTER(descriptor))),("scale",_C.c_byte),("digits",_C.c_ubyte),("fill1",_C.c_ushort),
                  ("aflags",_C.c_ubyte),("fill2",_C.c_ubyte * 3),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.POINTER(_C.POINTER(descriptor))),("coeff_and_bounds",_C.c_int32 * 24)]

    def __init__(self,ptr,num):
        self.dclass=_mdsclasses.CLASS_APD
        self.scale=0
        self.digits=0
        self.aflags=0
        self.dtype=_dtypes.DTYPE_DSC
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


    def _getValue(self):
            return _C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents.value

    value=property(_getValue)

    def __str__(self):
        ans=_ver.tostr(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", scale="+str(self.scale)+", digits="+str(self.digits)
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
    if _os.name=='nt':
        if _struct.calcsize("P")==4:
            _pack_=1
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_void_p),("scale",_C.c_byte),("digits",_C.c_ubyte),
                  ("aflags",_C.c_ubyte),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.c_void_p),("coeff_and_bounds",_C.c_int32 * 24)]
    else:
        _fields_=[("length",_C.c_ushort),("dtype",_C.c_ubyte),("dclass",_C.c_ubyte),("pointer",_C.c_void_p),("scale",_C.c_byte),("digits",_C.c_ubyte),("fill1",_C.c_ushort),
                  ("aflags",_C.c_ubyte),("fill2",_C.c_ubyte * 3),("dimct",_C.c_ubyte),("arsize",_C.c_uint),("a0",_C.c_void_p),("coeff_and_bounds",_C.c_int32 * 24)]

    def __init__(self,*value):
        if len(value) == 1:
            value = value[0].T
            if not value.flags.f_contiguous:
                value=value.copy('F')
            self.dclass=_mdsclasses.CLASS_A
            self.scale=0
            self.digits=0
            self.aflags=0
            self.dtype=_dtypes.mdsdtypes.fromNumpy(value)
            self.length=value.itemsize
            self.pointer=_C.c_void_p(value.ctypes.data)
            self.dimct=_N.shape(_N.shape(value))[0]
            self.arsize=value.nbytes
            self.a0=self.pointer
            if self.dimct > 1:
                self.coeff=1
                for i in range(self.dimct):
                    self.coeff_and_bounds[i]=_N.shape(value)[i]
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
        ans=_ver.tostr(_C.cast(_C.pointer(self),_C.POINTER(descriptor)).contents)+", scale="+str(self.scale)+", digits="+str(self.digits)
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

_tdishr=_mimport('_tdishr')
_treenode=_mimport('treenode')
_tree=_mimport('tree')
