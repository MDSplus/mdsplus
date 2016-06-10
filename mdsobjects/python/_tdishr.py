def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C

_descriptor = _mimport('_descriptor')
_mdsshr=_mimport('_mdsshr')
_Exceptions=_mimport('mdsExceptions')
_tree=_mimport('tree')
_ver=_mimport('version')
_treeshr=_mimport('_treeshr')

_TdiShr=_ver.load_library('TdiShr')

def _TdiShrFun(function,errormessage,expression,args=None):
    descriptor = _descriptor.descriptor
    def parseArguments(args):
        if args is None:
            return []
        if isinstance(args,tuple):
            if args:
                if isinstance(args[0],tuple):
                    return(parseArguments(args[0]))
            return([_C.pointer(descriptor(arg)) for arg in args])
        raise TypeError('Arguments must be passed as a tuple')
    xd = _descriptor.descriptor_xd()
    arguments = [_C.pointer(descriptor(expression))]+parseArguments(args)+[_C.pointer(xd),_C.c_void_p(-1)]
    status = function(*arguments)
    if (status & 1 != 0):
        return xd.value
    else:
        raise _Exceptions.statusToException(status)

def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    return _TdiShrFun(_TdiShr.TdiCompile,"Error compiling",expression,args)

def TdiExecute(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    return _TdiShrFun(_TdiShr.TdiExecute,"Error executing",expression,args)

def TdiDecompile(expression):
    """Decompile a TDI expression. Format: TdiDecompile(tdi_expression)"""
    return _ver.tostr(_TdiShrFun(_TdiShr.TdiDecompile,"Error decompiling",expression))

def TdiEvaluate(expression):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    return _TdiShrFun(_TdiShr.TdiEvaluate,"Error evaluating",expression)

def TdiData(expression):
    """Return primiitive data type. Format: TdiData(value)"""
    return _TdiShrFun(_TdiShr.TdiData,"Error converting to data",expression)

_CvtConvertFloat=_TdiShr.CvtConvertFloat
