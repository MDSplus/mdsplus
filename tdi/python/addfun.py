import MDSplus,sys,types
modulename = "tdi_functions"
def addfun(name,code,_file_=__file__):
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
    def tostr(var):
        if isinstance(var,MDSplus.Data):
            return str(var.data())
        return str(var)
    name   = tostr(name)
    _file_ = tostr(_file_)
    mdsname = name.upper()
    if not modulename in sys.modules:
       sys.modules[modulename] = types.ModuleType(modulename)
    code = MDSplus.Data.data(code)
    env = {}
    exec(compile(code,"TDI/%s"%(name,),'exec'),env,env)
    module_dict = sys.modules[modulename].__dict__
    module_dict[mdsname] = env[name]
    module_dict["__file__%s"%mdsname] = _file_
    mdsname = MDSplus.String(mdsname)
    return MDSplus.EQUALS(MDSplus.PUBLIC(mdsname),mdsname).evaluate()

