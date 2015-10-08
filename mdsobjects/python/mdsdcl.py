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
_mdsExceptions=_mimport('mdsExceptions',1)
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

_mdsdcl=_load_library('Mdsdcl')
_mdsdcl_do_command_dsc=_mdsdcl.mdsdcl_do_command_dsc
_mdsdcl_do_command_dsc.argtypes=[_C.c_char_p, _C.POINTER(_desc.descriptor_xd), _C.POINTER(_desc.descriptor_xd)]


def dcl(command,return_out=False,return_error=False,raise_exception=False):
    """Execute a dcl command
    @param command: command expression to execute
    @type command: str
    @param return_out: True if output should be returned in the result of the function.
    @type return_out: Boolean
    @param error_out: True if error should be returned in the result of the function.
    @type error_out: Boolean
    @param raise_exception: True if the function should raise an exception on failure.
    @type raise_exception: False
    @rtype: str / tuple / None
    """
    if return_error:
      xd_error=_desc.descriptor_xd()
      error_p=_C.pointer(xd_error)
    else:
      error_p=_C.cast(_C.c_void_p(0),_C.POINTER(_desc.descriptor_xd))
    if return_out:
      xd_output = _desc.descriptor_xd()
      out_p=_C.pointer(xd_output)
    else:
      out_p=_C.cast(_C.c_void_p(0),_C.POINTER(_desc.descriptor_xd))
    status = _mdsdcl_do_command_dsc(_ver.tobytes(command), error_p, out_p)
    if (status & 1) == 0 and raise_exception:
      raise _mdsExceptions.statusToException(status)
    if return_out and return_error:
      return (xd_output.value,xd_error.value)
    elif return_out:
      return xd_output.value
    elif return_error:
      return xd_error.value

def ccl(command,return_out=False,return_error=False,raise_exception=False):
    """Execute a ccl command
    @param command: command expression to execute
    @type command: str
    @param return_out: True if output should be returned in the result of the function.
    @type return_out: Boolean
    @param error_out: True if error should be returned in the result of the function.
    @type error_out: Boolean
    @param raise_exception: True if the function should raise an exception on failure.
    @type raise_exception: False
    @rtype: str / tuple / None
    """
    dcl('set command ccl',raise_exception=True)
    return dcl(command,return_out,return_error,raise_exception)

def tcl(command,return_out=False,return_error=False,raise_exception=False):
    """Execute a tcl command
    @param command: command expression to execute
    @type command: str
    @param return_out: True if output should be returned in the result of the function.
    @type return_out: Boolean
    @param error_out: True if error should be returned in the result of the function.
    @type error_out: Boolean
    @param raise_exception: True if the function should raise an exception on failure.
    @type raise_exception: False
    @rtype: str / tuple / None
    """
    dcl('set command tcl',raise_exception=True)
    return dcl(command,return_out,return_error,raise_exception)

def cts(command,return_out=False,return_error=False,raise_exception=False):
    """Execute a cts command
    @param command: command expression to execute
    @type command: str
    @param return_out: True if output should be returned in the result of the function.
    @type return_out: Boolean
    @param error_out: True if error should be returned in the result of the function.
    @type error_out: Boolean
    @param raise_exception: True if the function should raise an exception on failure.
    @type raise_exception: False
    @rtype: str / tuple / None
    """
    dcl('set command cts',raise_exception=True)
    return dcl(command,return_out,return_error,raise_exception)

