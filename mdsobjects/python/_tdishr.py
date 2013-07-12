from ctypes import CDLL,pointer,c_void_p
from _mdsshr import _load_library

TdiShr=_load_library('TdiShr')
class TdiException(Exception):
    pass

def restoreContext():
    from tree import Tree
    t=Tree.getActiveTree()
    if t is not None:
      t.restoreContext()
        
def TdiCompile(expression,args=None):
    """Compile a TDI expression. Format: TdiCompile('expression-string')"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
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
        raise TdiException(MdsGetMsg(status,"Error compiling expression"))

def TdiExecute(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
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
        raise TdiException(MdsGetMsg(status,"Error compiling expression"))

def TdiDecompile(value):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
    xd=descriptor_xd()
    try:
        Tree.lock()
        restoreContext()
        status=TdiShr.TdiDecompile(pointer(descriptor(value)),pointer(xd),c_void_p(-1))
    finally:
        Tree.unlock()
    if (status & 1 != 0):
        try:
            return str(xd.value.value.decode())
        except Exception,e:
            return str(xd.value.value)
    else:
        raise TdiException(MdsGetMsg(status,"Error decompiling value"))

def TdiEvaluate(value):
    """Evaluate and functions. Format: TdiEvaluate(data)"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
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
        raise TdiException(MdsGetMsg(status,"Error evaluating value"))

def TdiData(value):
    """Return primiitive data type. Format: TdiData(value)"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
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
        raise TdiException(MdsGetMsg(status,"Error converting value to data"))

CvtConvertFloat=TdiShr.CvtConvertFloat
