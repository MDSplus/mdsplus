from ctypes import CDLL,pointer,c_void_p
from _mdsshr import _load_library

TdiShr=_load_library('TdiShr')
class TdiException(Exception):
    pass

def restoreContext():
    from tree import Tree
    if hasattr(Tree,'_activeTree') and Tree._activeTree is not None:
        Tree._activeTree.restoreContext()
        
def TdiCompile(expression,args=None):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
    from _descriptor import descriptor_xd,descriptor,MdsGetMsg
    from tree import Tree
    __execute=TdiShr.TdiCompile
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
                raise TypeError,'Arguments must be passed as a tuple'
    finally:
        Tree.unlock()
    if done:
        return ans
    if (status & 1 != 0):
            return xd.value
    else:
        raise TdiException,MdsGetMsg(status,"Error compiling expression")

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
        return str(xd.value.value)
    else:
        raise TdiException,MdsGetMsg(status,"Error decompiling value")

def TdiEvaluate(value):
    """Compile and execute a TDI expression. Format: TdiExecute('expression-string')"""
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
        raise TdiException,MdsGetMsg(status,"Error evaluating value")

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
        raise TdiException,MdsGetMsg(status,"Error converting value to data")

def tditest():
    """Execute Tdi expressions and display results. Format: tditest()"""
    try:
        exp=raw_input('Enter an expression: ')
        while exp:
            try:
                ans = TdiExecute(exp)
                print type(ans)
                print ans
            except Exception,e:
                print "Error executing expression: ",e
            exp=raw_input('Enter an expression: ')
    except EOFError:
        exp=False
        print ""

CvtConvertFloat=TdiShr.CvtConvertFloat
