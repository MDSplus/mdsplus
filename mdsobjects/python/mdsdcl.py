def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C

_ver=_mimport('version')
_exc=_mimport('mdsExceptions')
_dsc=_mimport('descriptor')
_tre=_mimport('tree')

_mdsdcl=_ver.load_library('Mdsdcl')
_mdsdcl_do_command_dsc=_mdsdcl.mdsdcl_do_command_dsc
_mdsdcl_do_command_dsc.argtypes=[_C.c_char_p, _dsc.Descriptor_xd.PTR, _dsc.Descriptor_xd.PTR]

def dcl(command,return_out=False,return_error=False,raise_exception=False,ctx=None,setcommand='mdsdcl'):
    """Execute a dcl command
    @param command: command expression to execute
    @type command: str
    @param return_out: True if output should be returned in the result of the function.
    @type return_out: bool
    @param error_out: True if error should be returned in the result of the function.
    @type error_out: bool
    @param raise_exception: True if the function should raise an exception on failure.
    @type raise_exception: bool
    @param setcommand: invokes 'set command $' to load a command set.
    @type setcommand: str
    @rtype: str / tuple / None
    """
    if return_error:
        xd_error=_dsc.Descriptor_xd()
        error_p=xd_error.ptr
    else:
        error_p=_dsc.Descriptor_xd.null
    if return_out:
        xd_output = _dsc.Descriptor_xd()
        out_p=xd_output.ptr
    else:
        out_p=_dsc.Descriptor_xd.null
    _exc.checkStatus(_mdsdcl_do_command_dsc(_ver.tobytes('set command %s'%(setcommand,)), error_p, out_p))
    _tre._TreeCtx.pushCtx(ctx)
    try:
        status = _mdsdcl_do_command_dsc(_ver.tobytes(command), error_p, out_p)
    finally:
        _tre._TreeCtx.popCtx()
    if raise_exception: _exc.checkStatus(status)
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
