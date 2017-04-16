def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C

_ver=_mimport('version')
_desc=_mimport('_descriptor')
_tree=_mimport('tree')
_treeshr=_mimport('_treeshr')
_Exceptions=_mimport('mdsExceptions')

_mdsdcl=_ver.load_library('Mdsdcl')
_mdsdcl_do_command_dsc=_mdsdcl.mdsdcl_do_command_dsc
_mdsdcl_do_command_dsc.argtypes=[_C.c_char_p, _C.POINTER(_desc.descriptor_xd), _C.POINTER(_desc.descriptor_xd)]

def dcl(command,return_out=False,return_error=False,raise_exception=False,ctx=None,setcommand='mdsdcl'):
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
      error_p=_C.byref(xd_error)
    else:
      error_p=_C.cast(_C.c_void_p(0),_C.POINTER(_desc.descriptor_xd))
    if return_out:
      xd_output = _desc.descriptor_xd()
      out_p=_C.byref(xd_output)
    else:
      out_p=_C.cast(_C.c_void_p(0),_C.POINTER(_desc.descriptor_xd))
    _Exceptions.checkStatus(_mdsdcl_do_command_dsc(_ver.tobytes('set command %s'%(setcommand,)), error_p, out_p))
    opened = _tree._TreeCtx.setUpCtx(ctx)
    try:
        status = _mdsdcl_do_command_dsc(_ver.tobytes(command), error_p, out_p)
    finally:
        _tree._TreeCtx.restoreCtx(ctx,opened)
    if raise_exception:
        _Exceptions.checkStatus(status)
    if return_out and return_error:
      return (xd_output.value,xd_error.value)
    elif return_out:
      return xd_output.value
    elif return_error:
      return xd_error.value

def ccl(command,*args,**kwargs):
    """Executes a ccl command (c.f. dcl)"""
    return dcl(command,*args,setcommand='ccl',**kwargs)

def tcl(command,*args,**kwargs):
    """Executes a tcl command (c.f. dcl)"""
    return dcl(command,*args,setcommand='tcl',**kwargs)

def cts(command,*args,**kwargs):
    """Executes a cts command (c.f. dcl)"""
    return dcl(command,*args,setcommand='cts',**kwargs)

