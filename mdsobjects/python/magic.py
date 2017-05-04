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
