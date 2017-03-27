def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C, threading as _thread

_ver=_mimport('version')
_desc=_mimport('_descriptor')
_tree=_mimport('tree')
_treeshr=_mimport('_treeshr')
_Exceptions=_mimport('mdsExceptions')

_mdsdcl=_ver.load_library('Mdsdcl')
_mdsdcl_do_command_dsc=_mdsdcl.mdsdcl_do_command_dsc
_mdsdcl_do_command_dsc.argtypes=[_C.c_char_p, _C.POINTER(_desc.descriptor_xd), _C.POINTER(_desc.descriptor_xd)]
_mdsdcl.local = _thread.local()
def _gettctx():
    return getattr(_mdsdcl.local,'tctx',None)

def _setUpCtx(ctx):
    """The dcl interface has its own TreeCtx in case no tree is open
    This method will set up the context of the current tree if any of
    default to its own context. One may also provide a context ctx,
    as done by _tree.Tree.tcl()
    """
    if ctx is None:
      try: ctx=_tree.Tree().ctx
      except _Exceptions.TreeNOT_OPEN:
        tctx = getattr(_mdsdcl.local,'tctx',None)
        if tctx is None:
            return _treeshr.switchDbid()
        else:
            return _treeshr.switchDbid(tctx.ctx)
    else:
        return _treeshr.switchDbid(ctx.value)

def _restoreCtx(ctx,ctx0):
    ctx_new = _treeshr.switchDbid(ctx0)
    if ctx is None:
        if ctx_new:
            tctx = _gettctx()
            if tctx is None or ctx_new != tctx.ctx:
                _mdsdcl.local.tctx = _tree._TreeCtx(ctx_new)
        else:
            _mdsdcl.local.tctx = None
    else:
        ctx.value = ctx_new


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
    ctx0 = _setUpCtx(ctx)
    try:
        status = _mdsdcl_do_command_dsc(_ver.tobytes(command), error_p, out_p)
    finally:
        _restoreCtx(ctx,ctx0)
    if (status & 1) == 0 and raise_exception:
      raise _Exceptions.statusToException(status)
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

