import MDSplus,sys,types
modulename = "tdi_functions"
def addfun(name,code):
    """
    can be used to add customized python functions to the tdi environment
    e.g.:
        >>> ADDFUN("myfun","def myfun(a): print(a)")
        "MYFUN"
        >>> MYFUN(3)
        3
        *
    in order to prevent the function to load more than once use:
        >>> IF_ERROR(PUBLIC("MYFUN2"),ADDFUN("myfun2","def myfun2(a): print(a)"))
        "MYFUN2"
    """
    name = str(MDSplus.Data.data(name))
    mdsname = name.upper()
    if not modulename in sys.modules:
       sys.modules[modulename] = types.ModuleType(modulename)
    code = MDSplus.Data.data(code)
    env = {}
    exec(compile(code,"TDI/%s"%(name,),'exec'),env,env)
    sys.modules[modulename].__dict__[mdsname] = env[name]
    mdsname = MDSplus.String(mdsname)
    return MDSplus.EQUALS(MDSplus.PUBLIC(mdsname),mdsname).evaluate()

