import ctypes as _C
from MDSobjects._descriptor import descriptor_xd,MdsGetMsg,descriptor
import os
if os.name=='nt':
    __TreeShr=_C.CDLL('treeshr')
else:
    __TreeShr=_C.CDLL('libTreeShr.so')
__TreeOpen=__TreeShr.TreeOpen
__TreeOpen.argtypes=[_C.c_char_p,_C.c_int]
__TreeClose=__TreeShr.TreeClose
__TreeClose.argtypes=[_C.c_char_p,_C.c_int]
__TreeFindNode=__TreeShr.TreeFindNode
__TreeFindNode.argtypes=[_C.c_char_p,_C.POINTER(_C.c_int)]
__TreeGetPath=__TreeShr.TreeGetPath
__TreeGetPath.argtypes=[_C.c_int]
__TreeGetPath.restype=_C.c_void_p
TreeFree=__TreeShr.TreeFree
TreeFree.argtypes=[_C.c_void_p]
__TreeSwitchDbid=__TreeShr.TreeSwitchDbid
__TreeSwitchDbid.argtypes=[_C.c_void_p]
__TreeSwitchDbid.restype=_C.c_void_p

__TreeGetRecord=__TreeShr.TreeGetRecord
__TreeGetRecord.argtypes=[_C.c_int,_C.POINTER(descriptor_xd),_C.c_int]
__TreeDoMethod=__TreeShr.TreeDoMethod
__TreeDoMethod.argtypes=[_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int]
__TreePutRecord=__TreeShr.TreePutRecord
__TreePutRecord.argtypes=[_C.c_int,_C.POINTER(descriptor),_C.c_int]
__TreeTurnOn=__TreeShr.TreeTurnOn
__TreeTurnOn.argtypes=[_C.c_int]
__TreeTurnOff=__TreeShr.TreeTurnOff
__TreeTurnOff.argtypes=[_C.c_int]

class TreeException(Exception):
    pass

def TreeFindNode(path):
    n=_C.c_int()
    status=__TreeFindNode(path,n)
    if (status & 1):
        return n.value
    else:
        raise TreeException,'Error finding node '+str(path)+': '+MdsGetMsg(status)

def TreeGetRecord(n):
    """Get record from MDSplus tree. Accepts Path, TreeNode or integer"""
    try:
        n=TreeFindNode(n)
    except _C.ArgumentError:
        pass
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be an Nid or integer'
    xd=descriptor_xd()
    status=__TreeGetRecord(c_nid,xd,0)
    if (status & 1):
        return xd.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeGetDefault():
    """Get default node"""
    ans=_C.c_int(0)
    status = __TreeShr.TreeGetDefaultNid(_C.pointer(ans))
    if (status & 1):
        return ans.value
    else:
        raise TreeException.MdsGetMsg(status)
    
def TreeSetDefault(n):
    """Set default node"""
    status = __TreeShr.TreeSetDefaultNid(_C.c_int(n))
    if (status & 1):
        return
    else:
        raise TreeException.MdsGetMsg(status)
    
def TreeGetPath(n):
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    p=__TreeGetPath(c_nid)
    ans=_C.cast(p,_C.c_char_p).value
    if not p is None:
        TreeFree(p)
    return ans

def TreeFindNodeTags(n):
    from MDSobjects.array import makeArray
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    ctx=_C.c_int(0)
    tags=list()
    tag='                                                                             '
    tag_d=descriptor(tag)
    while __TreeShr.TreeFindNodeTagsDsc(c_nid,_C.pointer(ctx),_C.pointer(tag_d)):
        tags.append(tag_d.value.rstrip())
    if len(tags) > 0:
        tags=makeArray(tags)
    else:
        tags=None
    return tags

def TreePutRecord(n,value):
    """Put record into MDSplus tree. Accepts path, TreeNode or integer and the value"""
    try:
        n=TreeFindNode(n)
    except _C.ArgumentError:
        pass
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    status=__TreePutRecord(c_nid,_C.pointer(descriptor(value)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeDoMethod(n,method):
    """Do a method of an MDSplus device. Accepts path, TreeNode or integer and the value"""
    try:
        n=TreeFindNode(n)
    except _C.ArgumentError:
        pass
    try:
        nid=int(n)
    except TypeError:
        try:
            nid=n.nid
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    status=__TreeDoMethod(_C.pointer(descriptor(nid)),_C.pointer(descriptor(method)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

    
def TreeTurnOn(n):
    """Turn on a tree node. Accepts path, TreeNode or integer"""
    try:
        n=TreeFindNode(n)
    except _C.ArgumentError:
        pass
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    status=__TreeTurnOn(c_nid)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeTurnOff(n):
    """Turn off a tree node. Accepts path, TreeNode or integer"""
    try:
        n=TreeFindNode(n)
    except _C.ArgumentError:
        pass
    try:
        c_nid=_C.c_int(n)
    except TypeError:
        try:
            c_nid=_C.c_int(n.nid)
        except AttributeError:
            raise TypeError,'Argument must be a path, TreeNode or integer'
    status=__TreeTurnOff(c_nid)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeOpen(tree,shot):
    oldctx=__TreeSwitchDbid(_C.c_void_p(0))
    status = __TreeOpen(tree,shot)
    if (status & 1):
        ctx=__TreeSwitchDbid(_C.c_void_p(0))
        __TreeSwitchDbid(ctx)
        return ctx
    else:
        __TreeSwitchDbid(oldctx)
        raise TreeException,MdsGetMsg(status)

def TreeRestoreContext(ctx):
    return __TreeSwitchDbid(ctx)

def TreeClose(tree,shot):
    status = __TreeClose(tree,shot)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
