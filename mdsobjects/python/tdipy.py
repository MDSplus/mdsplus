def execPy(varname=None):
    """Get array of python commands from tdi public variable ___TDI___cmds
    and execute them. The ___TDI___cmds variable should be either a scalar string
    or a string array. If varname is defined
    then set the tdi public variable ___TDI___answer to the value of the variable
    with the name specified in varname. If varname is not defined
    then set public variable ___TDI___answer to 1 if there is no exception. If there
    is an exception then set public variable ___TDI___exception to be the
    exception string.
    """
    from MDSplus import Data as ___TDI___Data,makeData as ___TDI___makeData,String as ___TDI___String
    try:
        cmds=list()
        for cmd in ___TDI___Data.getTdiVar('___TDI___cmds'):
            cmds.append(str(cmd))
        cmds="\n".join(cmds)
        isglobal=False
        try:
          if int(___TDI___Data.getTdiVar('___TDI___global_ns'))==1:
            isglobal=True
        except:
            pass
        if isglobal:
          exec( cmds) in globals()
        else:
          exec( cmds) in {}
        if varname is not None:
            ans=ns[varname]
        else:
            ans=1
        ___TDI___makeData(ans).setTdiVar("___TDI___answer")
    except Exception:
        import sys
        e=sys.exc_info()[1]    
        ___TDI___String("Error: "+str(e)).setTdiVar("___TDI___exception")
