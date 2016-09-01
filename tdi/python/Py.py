import traceback as _tb
import sys as _sys
import MDSplus as _mdsplus

def Py(cmds, varname=None, isglobal=False,lock=None):
    """
    Execute python commands passed as a string or string array in cmds argument.
    If you want to return the value of a variable defined in the commands pass
    the variable name in the varname argument as a string. For example:

    py(["from MDSplus import Tree","ans=Tree('main',-1).getNode('numeric').record"],"ans")

    If you want to use the global namespace for the variables pass a 1 in the isglobal argument.
    This is unusual but does let you save the variables in a process global name space which
    could be retrieved later. For example in tdi:

    py("x=42",*,1)
    .....
    py("None","x",1)

    The lock argument is no longer used but retained for compatibility.
    """
    _mdsplus.Data.execute("deallocate(public _py_exception)")
    cmdlist=list()
    ans=1
    for cmd in cmds:
        cmdlist.append(str(cmd))
    cmds="\n".join(cmdlist)
    if isglobal:
        ns = globals()
    else:
        ns = {}
    try:
        exec( cmds ) in ns
    except:
        _tb.print_exc()
        _mdsplus.String(_sys.exc_info()[1]).setTdiVar("_py_exception")
        ans = 0
    if varname is not None:
        varname=str(varname)
        if varname in ns:
            ans=ns[varname]
        else:
            ans=None
    return ans
