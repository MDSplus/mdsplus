import MDSplus
import sys
import types
modulename = "tdi_functions"


def addfun(name, code, _file_=__file__):
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
    name = str(name .data() if isinstance(name, MDSplus.Data) else name)
    _file_ = str(_file_.data() if isinstance(_file_, MDSplus.Data) else _file_)
    mdsname = name.upper()
    if not modulename in sys.modules:
        sys.modules[modulename] = types.ModuleType(modulename)
    code = MDSplus.Data.data(code)
    env = {}
    exec(compile(code, "TDI/%s" % (name,), 'exec'), env, env)
    module_dict = sys.modules[modulename].__dict__
    module_dict[mdsname] = env[name]
    module_dict["__file__%s" % mdsname] = _file_
    mdsname = MDSplus.String(mdsname)
    return MDSplus.EQUALS(MDSplus.PUBLIC(mdsname), mdsname).evaluate()
