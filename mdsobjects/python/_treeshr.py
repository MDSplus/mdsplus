import ctypes as _C
from MDSobjects._descriptor import descriptor_xd,MdsGetMsg,descriptor
import os
if os.name=='nt':
    __TreeShr=_C.CDLL('treeshr')
else:
    __TreeShr=_C.CDLL('libTreeShr.so')
__TreeOpen=__TreeShr._TreeOpen
__TreeOpen.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeClose=__TreeShr._TreeClose
__TreeClose.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeFindNode=__TreeShr._TreeFindNode
__TreeFindNode.argtypes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int)]
__TreeGetPath=__TreeShr._TreeGetPath
__TreeGetPath.argtypes=[_C.c_void_p,_C.c_int]
__TreeGetPath.restype=_C.c_void_p
TreeFree=__TreeShr.TreeFree
TreeFree.argtypes=[_C.c_void_p]
__TreeSwitchDbid=__TreeShr.TreeSwitchDbid
__TreeSwitchDbid.argtypes=[_C.c_void_p]
__TreeSwitchDbid.restype=_C.c_void_p

__TreeDoMethod=__TreeShr._TreeDoMethod
__TreeDoMethod.argtypes=[_C.c_void_p,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int]
__TreePutRecord=__TreeShr._TreePutRecord
__TreePutRecord.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_xd),_C.c_int]
__TreeTurnOn=__TreeShr._TreeTurnOn
__TreeTurnOn.argtypes=[_C.c_void_p,_C.c_int]
__TreeTurnOff=__TreeShr._TreeTurnOff
__TreeTurnOff.argtypes=[_C.c_void_p,_C.c_int]
__TreeFindNodeTags=__TreeShr._TreeFindNodeTags
__TreeFindNodeTags.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(_C.c_void_p)]
__TreeFindNodeTags.restype=_C.c_char_p

class TreeException(Exception):
    pass

def TreeFindNode(ctx,path):
    n=_C.c_int()
    status=__TreeFindNode(ctx,path,n)
    if (status & 1):
        return n.value
    else:
        raise TreeException,'Error finding node '+str(path)+': '+MdsGetMsg(status)

def TreeGetDefault(ctx):
    """Get default node"""
    ans=_C.c_int(0)
    status = __TreeShr._TreeGetDefaultNid(ctx,_C.pointer(ans))
    if (status & 1):
        return ans.value
    else:
        raise TreeException,MdsGetMsg(status)
    
def TreeSetDefault(ctx,n):
    """Set default node"""
    status = __TreeShr._TreeSetDefaultNid(ctx,_C.c_int(n))
    if (status & 1):
        return
    else:
        raise TreeException,MdsGetMsg(status)
    
def TreeGetPath(n):
    p=__TreeGetPath(n.tree.ctx,n.nid)
    ans=_C.cast(p,_C.c_char_p).value
    if not p is None:
        TreeFree(p)
    return ans

def TreeFindNodeTags(n):
    from MDSobjects.array import makeArray
    ctx=_C.c_void_p(0)
    tags=list()
    done=False
    while not done:
        tag=__TreeFindNodeTags(n.tree.ctx,n.nid,_C.pointer(ctx))
        try:
            tags.append(tag.rstrip())
        except:
            done=True
    if len(tags) > 0:
        tags=makeArray(tags)
    else:
        tags=None
    return tags

def TreePutRecord(n,value):
    """Put record into MDSplus tree. Accepts path, TreeNode or integer and the value"""
    status=__TreePutRecord(n.tree.ctx,n.nid,_C.pointer(descriptor(value)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeDoMethod(n,method,arg=None):
    """Do a method of an MDSplus device. Accepts path, TreeNode or integer and the value"""
    status=__TreeDoMethod(n.tree.ctx,_C.pointer(descriptor(n)),_C.pointer(descriptor(method)),_C.pointer(descriptor(arg)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

    
def TreeTurnOn(n):
    """Turn on a tree node."""
    from MDSobjects.tree import Tree
    try:
        Tree.lock()
        status=__TreeTurnOn(n.tree.ctx,n.nid)
    finally:
        Tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeTurnOff(n):
    """Turn off a tree node."""
    from MDSobjects.tree import Tree
    try:
        Tree.lock()
        status=__TreeTurnOff(n.tree.ctx,n.nid)
    finally:
        Tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeOpen(tree,shot):
    ctx=_C.c_void_p(0)
#    oldctx=__TreeSwitchDbid(_C.c_void_p(0))
    status = __TreeOpen(_C.pointer(ctx),tree,shot)
    if (status & 1):
#        ctx=__TreeSwitchDbid(_C.c_void_p(0))
#        __TreeSwitchDbid(ctx)
        return ctx
    else:
#        __TreeSwitchDbid(oldctx)
        raise TreeException,MdsGetMsg(status)

def TreeRestoreContext(ctx):
    return __TreeSwitchDbid(ctx)

def TreeClose(ctx,tree,shot):
    status = __TreeClose(_C.pointer(ctx),tree,shot)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)

def TreeCloseAll(ctx):
    status = __TreeClose(_C.pointer(ctx),None,0)
    while (status & 1) == 1:
        try:
            status = __TreeClose(_C.pointer(ctx),None,0)
        except:
            status = 0
