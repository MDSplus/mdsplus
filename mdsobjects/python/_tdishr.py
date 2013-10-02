from ctypes import CDLL,pointer,c_void_p
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
        
def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    _descriptor=_mimport('_descriptor',1)
    descriptor_xd=_descriptor.descriptor_xd
    descriptor=_descriptor.descriptor
    Tree=_mimport('tree',1).Tree
    xd=descriptor_xd()
    done=False
    try:
        Tree.lock()
        restoreContext()
        if args is None:
            status=TdiShr.TdiCompile(pointer(descriptor(expression)),pointer(xd),c_void_p(-1))
        else:
            if isinstance(args,tuple):
                if len(args) > 0:
                    if isinstance(args[0],tuple):
                        ans = TdiCompile(expression,args[0])
                        done=True
                if not done:
                    exp='TdiShr.TdiCompile(pointer(descriptor(expression))'
                    for i in range(len(args)):
                        exp=exp+',pointer(descriptor(args[%d]))' % i
                    exp=exp+',pointer(xd),c_void_p(-1))'
                    status=eval(exp)
            else:
                raise TypeError('Arguments must be passed as a tuple')
    finally:
        Tree.unlock()
    if done:
        return ans
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
    done=False
    try:
        Tree.lock()
        restoreContext()
        if args is None:
            status=TdiShr.TdiExecute(pointer(descriptor(expression)),pointer(xd),c_void_p(-1))
        else:
            if isinstance(args,tuple):
                if len(args) > 0:
                    if isinstance(args[0],tuple):
                        ans = TdiExecute(expression,args[0])
                        done=True
                if not done:
                    exp='TdiShr.TdiExecute(pointer(descriptor(expression))'
                    for i in range(len(args)):
                        exp=exp+',pointer(descriptor(args[%d]))' % i
                    exp=exp+',pointer(xd),c_void_p(-1))'
                    status=eval(exp)
            else:
                raise TypeError('Arguments must be passed as a tuple')
    finally:
        Tree.unlock()
    if done:
        return ans
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
    try:
        Tree.lock()
        restoreContext()
        status=TdiShr.TdiDecompile(pointer(descriptor(value)),pointer(xd),c_void_p(-1))
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
    try:
        Tree.lock()
        restoreContext()
        status=TdiShr.TdiEvaluate(pointer(descriptor(value)),pointer(xd),c_void_p(-1))
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
    try:
        Tree.lock()
        restoreContext()
        status=TdiShr.TdiData(pointer(descriptor(value)),pointer(xd),c_void_p(-1))
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        return xd.value
    else:
        raise TdiException(_mdsshr.MdsGetMsg(status,"Error converting value to data"))

CvtConvertFloat=TdiShr.CvtConvertFloat
