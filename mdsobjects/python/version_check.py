from __future__ import print_function
import ctypes as _C
import sys as _sys
def _mimport(name, level=1):
    try:
        if not __package__:
            return __import__(name, globals())
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_ver = _mimport('version')

def eprint(*args, **kwargs):
    print(*args, file=_sys.stderr, **kwargs)

def version_check(version):
    class MDSplusVersionInfo(_C.Structure):
        _fields_= [("MAJOR",_C.c_char_p),
                   ("MINOR",_C.c_char_p),
                   ("RELEASE",_C.c_char_p),
                   ("BRANCH",_C.c_char_p),
                   ("RELEASE_TAG",_C.c_char_p),
                   ("COMMIT",_C.c_char_p),
                   ("DATE",_C.c_char_p),
                   ("MDSVERSION",_C.c_char_p)]

    try:
        mdsshr=_ver.load_library('MdsShr')
        _info=_C.cast(mdsshr.MDSplusVersion,_C.POINTER(MDSplusVersionInfo)).contents
        if _info.BRANCH in ('stable','alpha') and _info.MDSVERSION != version:
            eprint("""Warning: 
  The MDSplus python module version (%s) does not match
  the version of the installed MDSplus libraries (%s).
  Upgrade the module using the mdsplus/mdsobjects/python directory of the
  MDSplus installation.
""" % (version, _info.MDSVERSION ))
    except:
        pass
    
