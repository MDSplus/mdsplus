import ctypes as _C

if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_mdsshr=_mimport('_mdsshr',1)

TdiShr=_mdsshr._load_library('TdiShr')
class TdiException(Exception):
    pass

def restoreContext():
    t=_mimport('tree',1).Tree.getActiveTree()
    if t is not None:
      t.restoreContext()

def _parseArguments(args):
    _descriptor=_mimport('_descriptor',1)
    descriptor=_descriptor.descriptor
    if args is None:
        return list()
    if isinstance(args,tuple):
        if args:
            if isinstance(args[0],tuple):
                return(_parseArguments(args[0]))
        argin = [_C.pointer(descriptor(arg)) for arg in args]
        return argin
    raise TypeError('Arguments must be passed as a tuple')
       
def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    _descriptor=_mimport('_descriptor',1)
    descriptor_xd=_descriptor.descriptor_xd
    descriptor=_descriptor.descriptor
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    Tree.lock()
    try:
        restoreContext()
        argin = [_C.pointer(descriptor(expression))]+_parseArguments(args)+[_C.pointer(xd),_C.c_void_p(-1)]
        status = TdiShr.TdiCompile(*argin)
    finally:
        Tree.unlock()
    if (status & 1 != 0):
            return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error compiling expression"))

def TdiExecute(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    _descriptor=_mimport('_descriptor',1)
    descriptor=_descriptor.descriptor
    descriptor_xd=_descriptor.descriptor_xd
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    Tree.lock()
    try:
        restoreContext()
        argin = [_C.pointer(descriptor(expression))]+_parseArguments(args)+[_C.pointer(xd),_C.c_void_p(-1)]
        status = TdiShr.TdiExecute(*argin)
    finally:
        Tree.unlock()
    if (status & 1 != 0):
            return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error compiling expression"))

def TdiDecompile(value):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    _descriptor=_mimport('_descriptor',1)
    descriptor_xd=_descriptor.descriptor_xd
    descriptor=_descriptor.descriptor
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    Tree.lock()
    try:
        restoreContext()
        status=TdiShr.TdiDecompile(_C.pointer(descriptor(value)),_C.pointer(xd),_C.c_void_p(-1))
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        try:
            return str(xd.value)
        except Exception:
            return str(xd.value.value)
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error decompiling value"))

def TdiEvaluate(value):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    _descriptor=_mimport('_descriptor',1)
    descriptor=_descriptor.descriptor
    descriptor_xd=_descriptor.descriptor_xd
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    Tree.lock()
    try:
        restoreContext()
        status=TdiShr.TdiEvaluate(_C.pointer(descriptor(value)),_C.pointer(xd),_C.c_void_p(-1))
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error evaluating value"))

def TdiData(value):
    """Return primiitive data type. Format: TdiData(value)"""
    _descriptor=_mimport('_descriptor',1)
    descriptor=_descriptor.descriptor
    descriptor_xd=_descriptor.descriptor_xd
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    Tree.lock()
    try:
        restoreContext()
        status=TdiShr.TdiData(_C.pointer(descriptor(value)),_C.pointer(xd),_C.c_void_p(-1))
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error converting value to data"))

CvtConvertFloat=TdiShr.CvtConvertFloat
