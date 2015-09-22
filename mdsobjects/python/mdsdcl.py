import ctypes as _C
from ctypes.util import find_library as _find_library
import os as _os

if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_ver=_mimport('version',1)
_desc=_mimport('_descriptor',1)
_mdsshr=_mimport('_mdsshr',1)

def _load_library(name):
    libnam = None
    if _ver.pyver>(2,5,):
        libnam = _find_library(name)
    if libnam is None:
        try:
            return _C.CDLL('lib'+name+'.so')
        except:
            try:
                return _C.CDLL(name+'.dll')
            except:
                try:
                    return _C.CDLL('lib'+name+'.dylib')
                except:
                    raise Exception("Error finding library: "+name)
    else:
        try:
            return _C.CDLL(libnam)
        except:
            try:
                return _C.CDLL(name)
            except:
                return _C.CDLL(_os.path.basename(libnam))

_mdsdcl=_load_library('mdsdcl')
_mdsdcl_do_command_dsc=_mdsdcl.mdsdcl_do_command_dsc
_mdsdcl_do_command_dsc.argtypes=[_C.c_char_p, _C.POINTER(_desc.descriptor_xd), _C.POINTER(_desc.descriptor_xd)]

class DclException(Exception):
    pass


def dcl(command):
    """Execute a dcl command
    @param command: command expression to execute
    @type command: str
    @rtype: str / int / None
    """
    xd_error = _desc.descriptor_xd()
    xd_output = _desc.descriptor_xd()
    status = _mdsdcl_do_command_dsc(command, _C.pointer(xd_error), _C.pointer(xd_output))
    if (status & 1) == 1:
        return xd_output.value
    raise DclException(_mdsshr.MdsGetMsg(status))

def ccl(command):
    """Execute a ccl command
    @param command: command expression to execute
    @type command: str
    @rtype: str / int / None
    """
    dcl('set command ccl')
    return dcl(command)

def tcl(command):
    """Execute a tcl command
    @param command: command expression to execute
    @type command: str
    @rtype: str / int / None
    """
    dcl('set command tcl')
    return dcl(command)

def cts(command):
    """Execute a cts command
    @param command: command expression to execute
    @type command: str
    @rtype: str / int / None
    """
    dcl('set command cts')
    return dcl(command)

