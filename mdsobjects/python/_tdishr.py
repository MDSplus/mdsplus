if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_mdsshr=_mimport('_mdsshr',1)
_ver=_mimport('version',1)

TdiShr=_mdsshr._load_library('TdiShr')
class TdiException(Exception):
    pass

def _TdiShrFun(function,errormessage,expression,args=None):
    import ctypes as C
    _descriptor = _mimport('_descriptor',1)
    descriptor = _descriptor.descriptor
    def parseArguments(args):
        if args is None:
            return []
        if isinstance(args,tuple):
            if args:
                if isinstance(args[0],tuple):
                    return(parseArguments(args[0]))
            return([C.pointer(descriptor(arg)) for arg in args])
        raise TypeError('Arguments must be passed as a tuple')
    xd = _descriptor.descriptor_xd()
    arguments = [C.pointer(descriptor(expression))]+parseArguments(args)+[C.pointer(xd),C.c_void_p(-1)]
    Tree = _mimport('tree',1).Tree
    Tree.lock()
    try:
        tree = Tree.getActiveTree()
        if tree is not None:
            tree.restoreContext()
        status = function(*arguments)
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,errormessage))
       
def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    return _TdiShrFun(TdiShr.TdiCompile,"Error compiling",expression,args)

def TdiExecute(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    return _TdiShrFun(TdiShr.TdiExecute,"Error executing",expression,args)

def TdiDecompile(expression):
    """Decompile a TDI expression. Format: TdiDecompile(tdi_expression)"""
    return _ver.tostr(_TdiShrFun(TdiShr.TdiDecompile,"Error decompiling",expression))

def TdiEvaluate(expression):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    return _TdiShrFun(TdiShr.TdiEvaluate,"Error evaluating",expression)

def TdiData(expression):
    """Return primiitive data type. Format: TdiData(value)"""
    return _TdiShrFun(TdiShr.TdiData,"Error converting to data",expression)

CvtConvertFloat=TdiShr.CvtConvertFloat
