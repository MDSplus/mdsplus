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
from IPython.core.magic import register_line_cell_magic
import MDSplus as _M
import sys


@register_line_cell_magic
def tcl(line,cell=None):
    """
      magic to execute one line of TCL code.

      usage:
         %tcl tcl-command

         or

         %tcl
         command
         command
         ...


      for example:
         %tcl directory /full
    """
    toOut=[]
    toError=[]
    def doit(line,toOut,toError):
        if len(line) == 0:
            return
        out,error=_M.tcl(line,return_out=True,return_error=True)
        if error is not None and len(str(error)) > 0:
            toError.append(str(error))
        if out is not None and len(str(out)) > 0:
            toOut.append(str(out))
        
    if cell is None:
        doit(line,toOut,toError)
    else:
        for line in cell.split('\n'):
            doit(line,toOut,toError)
    if len(toOut) > 0:
        print('\n'.join(toOut))
    if len(toError) > 0:
        print('\n'.join(toError),file=sys.stderr)

@register_line_cell_magic
def tdi(line,cell=None):
    """
      magic to execute one line of TDI code.

      usage:
         %tdi expression

         or

         %tdi
         expression
         expression
         ...


      for example:
         %tdi _a=53+75
    """
    toOut=[]
    toError=[]
    def doit(line,toOut,toError):
        if len(line) == 0:
            return
        try:
            toOut.append(str(_M.Data.execute(line)))
        except Exception as e:
            toError.append(str(e))
        
    if cell is None:
        doit(line,toOut,toError)
    else:
        doit(cell,toOut,toError)
#        for line in cell.split('\n'):
#            doit(line,toOut,toError)
    if len(toOut) > 0:
        print('\n'.join(toOut))
    if len(toError) > 0:
        print('\n'.join(toError),file=sys.stderr)
