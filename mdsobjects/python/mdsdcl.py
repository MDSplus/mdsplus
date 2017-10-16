#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

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

def dcl(command,return_out=False,return_error=False,raise_exception=False,tree=None,setcommand='mdsdcl'):
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
    _tre._TreeCtx.pushTree(tree)
    try:
        status = _mdsdcl_do_command_dsc(_ver.tobytes(command), error_p, out_p)
    finally:
        _tre._TreeCtx.popTree()
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
