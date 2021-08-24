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
from __future__ import print_function
import ctypes as _C
import sys as _sys


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except Exception:
        return __import__(name, globals())


_ver = _mimport('version')
_exc = _mimport('mdsExceptions')
_dsc = _mimport('descriptor')
_tre = _mimport('tree')
try:
    _mdsdcl = _ver.load_library('Mdsdcl')
    _mdsdcl. mdsdcl_do_command_dsc.argtypes = [
        _C.c_char_p, _dsc.DescriptorXD.PTR, _dsc.DescriptorXD.PTR]
    _mdsdcl._mdsdcl_do_command_dsc.argtypes = [
        _C.c_void_p, _C.c_char_p, _dsc.DescriptorXD.PTR, _dsc.DescriptorXD.PTR]
except Exception:
    def dcl(*a, **kw):
        raise _exc.LibNOTFOU("Mdsdcl")
else:
    def dcl(command, return_out=False, return_error=False, raise_exception=False, tree=None, setcommand='mdsdcl'):
        """Execute a dcl command
        @param command: command expression to execute
        @type command: str
        @param return_out: True if output should be returned in the result of the function.
        @type return_out: bool
        @param return_error: True if error should be returned in the result of the function.
        @type return_error: bool
        @param raise_exception: True if the function should raise an exception on failure.
        @type raise_exception: bool
        @param setcommand: invokes 'set command $' to load a command set.
        @type setcommand: str
        @rtype: str / tuple / None
        """
        xd_error = _dsc.DescriptorXD()
        error_p = xd_error.ptr
        xd_output = _dsc.DescriptorXD()
        out_p = xd_output.ptr
        _exc.checkStatus(_mdsdcl.mdsdcl_do_command_dsc(
            _ver.tobytes('set command %s' % (setcommand,)), error_p, out_p))
        if isinstance(tree, _tre.Tree) and not tree.public:
            status = _mdsdcl._mdsdcl_do_command_dsc(
                tree.pctx, _ver.tobytes(command), error_p, out_p)
        else:
            status = _mdsdcl.mdsdcl_do_command_dsc(
                _ver.tobytes(command), error_p, out_p)
        if (return_out or return_error) and raise_exception:
            _exc.checkStatus(status, message=xd_error.value)
        if return_out and return_error:
            return (xd_output.value, xd_error.value)
        elif return_out:
            return xd_output.value
        elif return_error:
            return xd_error.value
        else:
            if xd_output.value is not None:
                print(xd_output.value)
            if xd_error.value is not None:
                print(xd_error.value, file=_sys.stderr)


def ccl(command, *args, **kwargs):
    """Executes a ccl command (c.f. dcl)"""
    return dcl(command, *args, setcommand='ccl', **kwargs)


def tcl(command, *args, **kwargs):
    """Executes a tcl command (c.f. dcl)"""
    return dcl(command, *args, setcommand='tcl', **kwargs)


def cts(command, *args, **kwargs):
    """Executes a cts command (c.f. dcl)"""
    return dcl(command, *args, setcommand='cts', **kwargs)
