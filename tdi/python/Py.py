import traceback as _tb
import MDSplus

def Py(cmds, *arg):
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
    varname = arg[0] if len(arg)>0 else None
    #isglobal= arg[1] if len(arg)>1 else False
    arg     = arg[3:]if len(arg)>3 else []
    MDSplus.DEALLOCATE('public _py_exception')
    cmdlist=list()
    ans=1
    for cmd in cmds:
        cmdlist.append(MDSplus.version.tostr(cmd))
    cmds="\n".join(cmdlist)
    env = {"arg":arg,'MDSplus':MDSplus}
    try:
        exec(compile(cmds,'<string>','exec'),globals(),env)
    except Exception as exc:
        _tb.print_exc()
        MDSplus.String(exc).setTdiVar("_py_exception")
        ans = 0
    if varname is not None:
        varname=MDSplus.version.tostr(varname)
        if varname in env:
            ans=env[varname]
        else:
            ans=None
    return ans
