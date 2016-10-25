import ctypes as _C
def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_data=_mimport('mdsdata')
_ver=_mimport('version')
descriptor=_mimport('descriptor')

class Ident(_data.Data):
    """Reference to MDSplus Ken Variable"""
    dtype_id=191
    def __init__(self,name):
        self.name=_ver.tostr(name)
    def decompile(self):
        return self.name
    @property
    def descriptor(self):
        d=descriptor.Descriptor_s()
        d.dtype=self.dtype_id
        d.length=len(self.name)
        d.pointer=_C.cast(_C.c_char_p(_ver.tobytes(self.name)),_C.c_void_p)
        d.original=self
        return d

    @classmethod
    def fromDescriptor(cls,d):
        return cls(
            _ver.tostr(
                _C.cast(d.pointer,_C.POINTER(_C.c_char*d.length)).contents.value))

descriptor.dtypeToClass[Ident.dtype_id]=Ident
