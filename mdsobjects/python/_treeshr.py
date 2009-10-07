import ctypes as _C
from _mdsshr import _load_library
from _descriptor import descriptor_xd,MdsGetMsg,descriptor,descriptor_a
__TreeShr=_load_library('TreeShr')
__TreeOpen=__TreeShr._TreeOpen
__TreeOpen.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int,_C.c_int]
__TreeOpenNew=__TreeShr._TreeOpenNew
__TreeOpenNew.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeOpenEdit=__TreeShr._TreeOpenEdit
__TreeOpenEdit.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeQuitTree=__TreeShr._TreeQuitTree
__TreeQuitTree.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeWriteTree=__TreeShr._TreeWriteTree
__TreeWriteTree.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeClose=__TreeShr._TreeClose
__TreeClose.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
__TreeFindNode=__TreeShr._TreeFindNode
__TreeFindNode.argtypes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int)]
__TreeGetPath=__TreeShr._TreeGetPath
__TreeGetPath.argtypes=[_C.c_void_p,_C.c_int]
__TreeGetPath.restype=_C.c_void_p
__TreeFindTagWild=__TreeShr._TreeFindTagWild
__TreeFindTagWild.argtpes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int),_C.POINTER(_C.c_void_p)]
__TreeFindTagWild.restype=_C.c_void_p
__TreeGetDbi=__TreeShr._TreeGetDbi
__TreeGetDbi.argtypes=[_C.c_void_p,_C.c_void_p]
__TreeSetDbi=__TreeShr._TreeSetDbi
__TreeSetDbi.argtypes=[_C.c_void_p,_C.c_void_p]
TreeFindTagEnd=__TreeShr.TreeFindTagEnd
TreeFindTagEnd.argtypes=[_C.POINTER(_C.c_void_p)]
TreeFree=__TreeShr.TreeFree
TreeFree.argtypes=[_C.c_void_p]
__TreeSwitchDbid=__TreeShr.TreeSwitchDbid
__TreeSwitchDbid.argtypes=[_C.c_void_p]
__TreeSwitchDbid.restype=_C.c_void_p
TreeCreatePulseFile=__TreeShr._TreeCreatePulseFile
TreeCreatePulseFile.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.c_void_p]
__TreeDeletePulseFile=__TreeShr._TreeDeletePulseFile
__TreeDeletePulseFile.argtypes=[_C.c_void_p,_C.c_int,_C.c_int]
__TreeDoMethod=__TreeShr._TreeDoMethod
__TreeDoMethod.argtypes=[_C.c_void_p,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int]
__TreeGetRecord=__TreeShr._TreeGetRecord
__TreeGetRecord.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_xd)]
__TreePutRecord=__TreeShr._TreePutRecord
__TreePutRecord.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor),_C.c_int]
__TreeSetTimeContext=__TreeShr.TreeSetTimeContext
__TreeSetTimeContext.argtypes=[_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor)]
__TreeTurnOn=__TreeShr._TreeTurnOn
__TreeTurnOn.argtypes=[_C.c_void_p,_C.c_int]
__TreeTurnOff=__TreeShr._TreeTurnOff
__TreeTurnOff.argtypes=[_C.c_void_p,_C.c_int]
__TreeFindNodeTags=__TreeShr._TreeFindNodeTags
__TreeFindNodeTags.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(_C.c_void_p)]
__TreeFindNodeTags.restype=_C.c_void_p
__TreeAddNode=__TreeShr._TreeAddNode
__TreeAddNode.argtypes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int),_C.c_int]
__TreeSetSubtree=__TreeShr._TreeSetSubtree
__TreeSetSubtree.argtypes=[_C.c_void_p,_C.c_int]
__TreeSetNoSubtree=__TreeShr._TreeSetNoSubtree
__TreeSetNoSubtree.argtypes=[_C.c_void_p,_C.c_int]
__TreeAddTag=__TreeShr._TreeAddTag
__TreeAddTag.argtypes=[_C.c_void_p,_C.c_int,_C.c_char_p]
__TreeRemoveTag=__TreeShr._TreeRemoveTag
__TreeRemoveTag.argtypes=[_C.c_void_p,_C.c_char_p]
__TreeRenameNode=__TreeShr._TreeRenameNode
__TreeRenameNode.argtypes=[_C.c_void_p,_C.c_int,_C.c_char_p]
__TreeAddConglom=__TreeShr._TreeAddConglom
__TreeAddConglom.argtypes=[_C.c_void_p,_C.c_char_p,_C.c_char_p,_C.POINTER(_C.c_int)]
__TreeDeleteNodeInitialize=__TreeShr._TreeDeleteNodeInitialize
__TreeDeleteNodeInitialize.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(_C.c_int),_C.c_int]
__TreeDeleteNodeExecute=__TreeShr._TreeDeleteNodeExecute
__TreeDeleteNodeExecute.argtypes=[_C.c_void_p]
TreeGetCurrentShotId=__TreeShr.TreeGetCurrentShotId
TreeGetCurrentShotId.argtypes=[_C.c_char_p]
TreeSetCurrentShotId=__TreeShr.TreeSetCurrentShotId
TreeSetCurrentShotId.argtypes=[_C.c_char_p,_C.c_int]
__TreeSetViewDate=__TreeShr.TreeSetViewDate
__TreeSetViewDate.argtypes=[_C.POINTER(_C.c_ulonglong)]
__TreeGetViewDate=__TreeShr.TreeGetViewDate
__TreeGetViewDate.argtypes=[_C.POINTER(_C.c_ulonglong)]
__TreeBeginSegment=__TreeShr._TreeBeginSegment
__TreeBeginSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor_a),_C.c_int]
__TreeBeginTimestampedSegment=__TreeShr._TreeBeginTimestampedSegment
__TreeBeginTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_a),_C.c_int]
__TreeUpdateSegment=__TreeShr._TreeUpdateSegment
__TreeUpdateSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int]
__TreePutSegment=__TreeShr._TreePutSegment
__TreePutSegment.argtpes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_a)]
__TreePutTimestampedSegment=__TreeShr._TreePutTimestampedSegment
__TreePutTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_a),_C.POINTER(_C.c_int64)]
__TreePutRow=__TreeShr._TreePutRow
__TreePutRow.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(_C.c_int64),_C.POINTER(descriptor_a)]
__TreeGetNumSegments=__TreeShr._TreeGetNumSegments
__TreeGetNumSegments.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(_C.c_int)]
__TreeGetSegment=__TreeShr._TreeGetSegment
__TreeGetSegment.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__TreeGetSegmentLimits=__TreeShr._TreeGetSegmentLimits
__TreeGetSegmentLimits.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__TreeStartConglomerate=__TreeShr._TreeStartConglomerate
__TreeStartConglomerate.argtypes=[_C.c_void_p,_C.c_int]


__RTreeShr=_load_library('CacheShr')
__RTreeOpen=__RTreeShr._RTreeOpen
__RTreeOpen.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p]
__RTreeClose=__RTreeShr._RTreeClose
__RTreeClose.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int]
RTreeConfigure=__RTreeShr.RTreeConfigure
RTreeConfigure.argtypes=[_C.c_int,_C.c_int]
RTreeSynch=__RTreeShr.RTreeSynch
RTreeFlush=__RTreeShr._RTreeFlush
RTreeFlush.argtypes=[_C.c_void_p,_C.c_int]

__RTreePutRecord=__RTreeShr._RTreePutRecord
__RTreePutRecord.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_xd),_C.c_int]
__RTreeBeginSegment=__RTreeShr._RTreeBeginSegment
__RTreeBeginSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor_a),_C.c_int,_C.c_int]
__RTreeBeginTimestampedSegment=__RTreeShr._RTreeBeginTimestampedSegment
__RTreeBeginTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_a),_C.c_int,_C.c_int]
__RTreeUpdateSegment=__RTreeShr._RTreeUpdateSegment
__RTreeUpdateSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int,_C.c_int]
__RTreePutSegment=__RTreeShr._RTreePutSegment
__RTreePutSegment.argtpes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_a),_C.c_int]
__RTreePutTimestampedSegment=__RTreeShr._RTreePutTimestampedSegment
__RTreePutTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_a),_C.POINTER(_C.c_int64),_C.c_int]
__RTreePutRow=__RTreeShr._RTreePutRow
__RTreePutRow.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(_C.c_int64),_C.c_void_p,_C.c_int]
__RTreeGetNumSegments=__RTreeShr._RTreeGetNumSegments
__RTreeGetNumSegments.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(_C.c_int)]
__RTreeGetSegment=__RTreeShr._RTreeGetSegment
__RTreeGetSegment.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__RTreeGetSegmentLimits=__RTreeShr._RTreeGetSegmentLimits
__RTreeGetSegmentLimits.argtypes=[_C.c_void_p,_C.c_int,_C.c_int,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__RTreeDiscardOldSegments=__RTreeShr._RTreeDiscardOldSegments
__RTreeDiscardOldSegments.argtypes=[_C.c_void_p,_C.c_int,_C.c_int64]
__RTreeDiscardData=__RTreeShr._RTreeDiscardData
__RTreeDiscardData.argtypes=[_C.c_void_p,_C.c_int]
__RTreeFlushNode=__RTreeShr._RTreeFlushNode
__RTreeFlushNode.argtypes=[_C.c_void_p,_C.c_int]
__RTreeSetWarm=__RTreeShr._RTreeSetWarm
__RTreeSetWarm.argtypes=[_C.c_void_p,_C.c_int,_C.c_int]
__RTreeGetRecord=__RTreeShr._RTreeGetRecord
__RTreeGetRecord.argtypes=[_C.c_void_p,_C.c_int,_C.POINTER(descriptor_xd)]



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
    
def TreeSetUsage(ctx,n,usage):
    """Set node usage"""
    try:
        status = __TreeShr._TreeSetUsage(ctx,_C.c_int(n),_C.c_byte(usage))
    except:
        raise TreeException,"Feature not present in current MDSplus installation. Upgrade to newer version of MDSplus."
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
    from mdsarray import makeArray
    ctx=_C.c_void_p(0)
    tags=list()
    done=False
    while not done:
        tag_ptr=__TreeFindNodeTags(n.tree.ctx,n.nid,_C.pointer(ctx))
        try:
            tags.append(_C.cast(tag_ptr,_C.c_char_p).value.rstrip())
            TreeFree(tag_ptr)
        except:
            done=True
    if len(tags) > 0:
        tags=makeArray(tags).value
    else:
        tags=None
    return tags

def TreeFindTagWild(tree,wild):
    from mdsarray import makeArray
    nid=_C.c_int(0)
    ctx=_C.c_void_p(0)
    tags=list()
    done=False
    while not done:
        tag_ptr=__TreeFindTagWild(tree.ctx,wild,_C.pointer(nid),_C.pointer(ctx))
        try:
            tags.append(_C.cast(tag_ptr,_C.c_char_p).value.rstrip())
        except:
            TreeFindTagEnd(_C.pointer(ctx))
            done=True
    if len(tags) > 0:
        tags=makeArray(tags).value
    else:
        tags=None
    return tags

def TreeGetRecord(n):
    """Get record from MDSplus tree. Accepts path, TreeNode or integer and the value"""
    value=descriptor_xd()
    status=__TreeGetRecord(n.tree.ctx,n.nid,_C.pointer(value))
    if (status & 1):
        return value.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreePutRecord(n,value):
    """Put record into MDSplus tree. Accepts path, TreeNode or integer and the value"""
    status=__TreePutRecord(n.tree.ctx,n.nid,_C.pointer(descriptor(value)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeSetTimeContext(begin,end,delta):
    """Set begin, end and delta for segmented record retrieval"""
    status = __TreeSetTimeContext(_C.pointer(descriptor(begin)),
                                  _C.pointer(descriptor(end)),
                                  _C.pointer(descriptor(delta)))
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeDoMethod(n,method,arg=None):
    """Do a method of an MDSplus device. Accepts path, TreeNode or integer and the value"""
    status=__TreeDoMethod(n.tree.ctx,_C.pointer(descriptor(n)),_C.pointer(descriptor(method)),_C.pointer(descriptor(arg)),0)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeStartConglomerate(tree,num):
    """Start a conglomerate in a tree."""
    try:
        tree.lock()
        status=__TreeStartConglomerate(tree.ctx,num)
    finally:
        tree.unlock()
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    return None

def TreeTurnOn(n):
    """Turn on a tree node."""
    try:
        n.tree.lock()
        status=__TreeTurnOn(n.tree.ctx,n.nid)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeTurnOff(n):
    """Turn off a tree node."""
    try:
        n.tree.lock()
        status=__TreeTurnOff(n.tree.ctx,n.nid)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def TreeOpen(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpen(_C.pointer(ctx),tree,shot,0)
    if (status & 1):
        return ctx
    else:
        raise TreeException,MdsGetMsg(status)

def TreeOpenReadOnly(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpen(_C.pointer(ctx),tree,shot,1)
    if (status & 1):
        return ctx
    else:
        raise TreeException,MdsGetMsg(status)

def TreeOpenNew(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpenNew(_C.pointer(ctx),tree,shot)
    if (status & 1):
        return ctx
    else:
        raise TreeException,MdsGetMsg(status)

def TreeOpenEdit(tree):
    status = __TreeOpenEdit(_C.pointer(tree.ctx),tree.tree,tree.shot)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeQuitTree(tree):
    status = __TreeQuitTree(_C.pointer(tree.ctx),tree.tree,tree.shot)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeWriteTree(tree,name,shot):
    status = __TreeWriteTree(_C.pointer(tree.ctx),name,shot)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeAddNode(tree,name,usage):
    from treenode import usage_table
    nid=_C.c_int(0)
    try:
        usagenum=usage_table[usage.upper()]
        if usagenum==-1:
            status = __TreeAddNode(tree.ctx,name,nid,1)
            if status & 1:
                status = __TreeSetSubtree(tree.ctx,nid)
        else:
            status = __TreeAddNode(tree.ctx,name,nid,usagenum)
        if not (status & 1):
            raise TreeException,MdsGetMsg(status)
    except KeyError:
        raise 'Invalid usage must be one of: %s' % (str(usage_table.keys()),)
    return nid.value

def TreeSetSubtree(node,flag):
    try:
        node.tree.lock()
        if flag:
            status=__TreeSetSubtree(node.tree.ctx,node.nid)
        else:
            status=__TreeSetNoSubtree(node.tree.ctx,node.nid)
        if not (status & 1):
            raise TreeException,MdsGetMsg(status)
    finally:
        node.tree.unlock()

def TreeRenameNode(node,name):
    try:
        node.tree.lock()
        status = __TreeRenameNode(node.tree.ctx,node.nid,name)
        if not (status & 1):
            raise TreeException,MdsGetMsg(status)
    finally:
        node.tree.unlock()

def TreeAddTag(tree,nid,tag):
    try:
        tree.lock()
        status = __TreeAddTag(tree.ctx,nid,tag)
        if not (status & 1):
            raise TreeException,MdsGetMsg(status)
    finally:
        tree.unlock()

def TreeRemoveTag(tree,tag):
    try:
        tree.lock()
        status = __TreeRemoveTag(tree.ctx,tag)
        if not (status & 1):
            raise TreeException,MdsGetMsg(status)
    finally:
        tree.unlock()

def TreeAddConglom(tree,name,devname):
    nid=_C.c_int(0)
    status = __TreeAddConglom(tree.ctx,name,devname,nid)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    return nid.value
    
def TreeDeleteNodeInitialize(tree):
    count=_C.c_int(0)
    status = __TreeDeleteNodeInitialize(tree.ctx,0,count,1)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeDeleteNode(tree,nid):
    count=_C.c_int(0)
    status = __TreeDeleteNodeInitialize(tree.ctx,nid,count,0)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeDeleteNodeExecute(tree):
    status = __TreeDeleteNodeExecute(tree.ctx)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    
def TreeDeletePulse(tree,shot):
    status = __TreeDeletePulseFile(tree.ctx,shot,1)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    
def RTreeOpen(tree,shot):
    ctx=_C.c_void_p(0)
    status = __RTreeOpen(_C.pointer(ctx),tree,shot)
    if (status & 1):
        return ctx
    else:
        raise TreeException,MdsGetMsg(status)

def TreeRestoreContext(ctx):
    try:
        return __TreeSwitchDbid(ctx)
    except:
        return __TreeSwitchDbid(_C.c_void_p(0))

def TreeGetContext():
        ctx=__TreeSwitchDbid(_C.c_void_p(0))
        if ctx is None:
            raise TreeException,'Tree not open'
        else:
            __TreeSwitchDbid(ctx)
        return ctx

def TreeClose(ctx,tree,shot):
    status = __TreeClose(_C.pointer(ctx),tree,shot)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)

def RTreeClose(ctx,tree,shot):
    status = __RTreeClose(_C.pointer(ctx),tree,shot)
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)


def TreeCloseAll(ctx):
    if ctx is not None:
        status = __TreeClose(_C.pointer(ctx),None,0)
        while (status & 1) == 1:
            try:
                status = __TreeClose(_C.pointer(ctx),None,0)
            except:
                status = 0

def TreeSetVersionDate(date):
    from _mdsshr import DateToQuad
    status = __TreeSetViewDate(_C.c_ulonglong(DateToQuad(date)))
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)

def TreeGetVersionDate():
    from mdsscalar import Uint64
    dt=_C.c_ulonglong(0)
    status = __TreeGetViewDate(dt)
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    return Uint64(dt.value).date

def RTreeCloseAll(ctx):
    if ctx is not None:
        status = __RTreeClose(_C.pointer(ctx),None,0)
        while (status & 1) == 1:
            try:
                status = __RTreeClose(_C.pointer(ctx),None,0)
            except:
                status = 0

def TreeGetNumSegments(n):
    """Get number of segments in a node."""
    try:
        n.tree.lock()
        num=_C.c_int(0)
        status=__TreeGetNumSegments(n.tree.ctx,n.nid,_C.pointer(num))
    finally:
        n.tree.unlock()
    if (status & 1):
        return num.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreePutRecord(n,value):
    """Put record into MDSplus tree. Accepts path, TreeNode or integer and the value"""
    try:
        n.tree.lock()
        status=__RTreePutRecord(n.tree.ctx,n.nid,_C.pointer(descriptor(value)),n.cachePolicy)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeBeginSegment(n,start,end,dimension,initialValue,idx):
    """Begin a segment."""
    try:
        n.tree.lock()
        status=__RTreeBeginSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                   _C.pointer(descriptor(dimension)),_C.pointer(descriptor(initialValue)),
                                   idx,n.cachePolicy)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeUpdateSegment(n,start,end,dimension,idx):
    """Update a segment."""
    try:
        n.tree.lock()
        status=__RTreeUpdateSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                    _C.pointer(descriptor(dimension)),idx,n.cachePolicy)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None
    
def RTreePutSegment(n,value,idx):
    """Put a segment"""
    try:
        n.tree.lock()
        status=__RTreePutSegment(n.tree.ctx,n.nid,idx,_C.pointer(descriptor(value)),n.cachePolicy)
    finally:
        n.treee.unlock()
    if (status & 1):
        return status;
    else:
        raise TreeException,MdsGetMsg(status)

def RTreePutTimestampedSegment(n,value,timestampArray):
    """Put a timestampedsegment"""
    try:
        n.tree.lock()
        status=__RTreePutTimestampedSegment(n.tree.ctx,n.nid,idx,_C.pointer(descriptor(value)),descriptor_a(timestampArray).pointer,n.cachePolicy)
    finally:
        n.treee.unlock()
    if (status & 1):
        return status;
    else:
        raise TreeException,MdsGetMsg(status)

def RTreePutRow(n,bufsize,array,timestamp):
    """Begin a segment."""
    try:
        n.tree.lock()
        status=__RTreePutRow(n.tree.ctx,n.nid,bufsize,_C.pointer(_C.c_int64(timestamp)),
                             _C.pointer(descriptor(array)),n.cachePolicy)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeGetNumSegments(n):
    """Get number of segments in a node."""
    try:
        n.tree.lock()
        num=_C.c_int(0)
        status=__RTreeGetNumSegments(n.tree.ctx,n.nid,_C.pointer(num))
    finally:
        n.tree.unlock()
    if (status & 1):
        return num.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeGetRecord(n):
    """Get cached record"""
    value=descriptor_xd()
    try:
        n.tree.lock()
        status=__RTreeGetRecord(n.tree.ctx,n.nid,_C.pointer(value))
    finally:
        n.tree.unlock()
    if (status & 1):
        return value.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeGetSegmentStart(n,idx):
    """Get cached record"""
    start=descriptor_xd()
    end=descriptor_xd()
    try:
        n.tree.lock()
        status=__RTreeGetSegmentLimits(n.tree.ctx,n.nid,idx,_C.pointer(start),_C.pointer(end))
    finally:
        n.tree.unlock()
    if (status & 1):
        return start.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeGetSegmentEnd(n,idx):
    """Get cached record"""
    start=descriptor_xd()
    end=descriptor_xd()
    try:
        n.tree.lock()
        status=__RTreeGetSegmentLimits(n.tree.ctx,n.nid,idx,_C.pointer(start),_C.pointer(end))
    finally:
        n.tree.unlock()
    if (status & 1):
        return end.value
    else:
        raise TreeException,MdsGetMsg(status)
    return None

def RTreeFlushNode(n):
    """Flush cache of node"""
    try:
        n.tree.lock()
        status=__RTreeFlushNode(n.tree.ctx,n.nid)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise TreeException,mdsGetMsg(status)
    return None

class DBI_ITM_INT(_C.Structure):
    _fields_=[("buffer_length",_C.c_ushort),("code",_C.c_ushort),("pointer",_C.POINTER(_C.c_int)),
              ("retlen",_C.c_void_p),
              ("buffer_length2",_C.c_ushort),("code2",_C.c_ushort),("pointer2",_C.c_void_p),
              ("retlen2",_C.c_void_p),]
    
    def __init__(self,code,value):
        self.buffer_length=_C.c_ushort(4)
        self.code=_C.c_ushort(code)
        self.pointer=_C.pointer(value)
        self.retlen=_C.c_void_p(0)
        self.buffer_length2=0
        self.code2=0
        self.pointer2=_C.c_void_p(0)
        self.retlen2=_C.c_void_p(0)

class DBI_ITM_CHAR(_C.Structure):
    _fields_=[("buffer_length",_C.c_ushort),("code",_C.c_ushort),("pointer",_C.c_char_p),
              ("retlen",_C.POINTER(_C.c_int)),
              ("buffer_length2",_C.c_ushort),("code2",_C.c_ushort),("pointer2",_C.c_void_p),
              ("retlen2",_C.c_void_p),]
    
    def __init__(self,code,buflen,string_p,retlen_p):
        self.buffer_length=_C.c_ushort(buflen)
        self.code=_C.c_ushort(code)
        self.pointer=string_p
        self.retlen=_C.pointer(retlen_p)
        self.buffer_length2=0
        self.code2=0
        self.pointer2=_C.c_void_p(0)
        self.retlen2=_C.c_void_p(0)

def TreeGetDbi(tree,itemname):
    itemlist={'NAME':(1,str,12),'SHOTID':(2,int),'MODIFIED':(3,bool),
              'OPEN_FOR_EDIT':(4,bool),'INDEX':(5,int),'NUMBER_OPENED':(6,int),
              'MAX_OPEN':(7,int),'DEFAULT':(10,int),'OPEN_READONLY':(9,bool),
              'VERSIONS_IN_MODEL':(10,bool),'VERSIONS_IN_PULSE':(11,bool)}
    try:
        item=itemlist[itemname.upper()]
    except KeyError:
        raise KeyError,'Item name must be one of %s' % (str(itemlist.keys()),)
    if item[1]==str:
        ans=_C.c_char_p('x'.rjust(item[2]))
        retlen=_C.c_int(0)
        itmlst=DBI_ITM_CHAR(item[0],item[2],ans,retlen)
    else:
        ans=_C.c_int(0)
        itmlst=DBI_ITM_INT(item[0],ans)
    try:
        tree.lock()
        status=__TreeGetDbi(tree.ctx,_C.cast(
            _C.pointer(itmlst),_C.c_void_p))
    finally:
        tree.unlock()
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
    return item[1](ans.value)

def TreeSetDbi(tree,itemname,value):
    itemlist={'VERSIONS_IN_MODEL':(10,bool),'VERSIONS_IN_PULSE':(11,bool)}
    try:
        item=itemlist[itemname.upper()]
    except KeyError:
        raise KeyError,'Item name must be one of %s' % (str(itemlist.keys()),)
    if item[1]==str:
        retlen=_C.c_int(0)
        itmlst=DBI_ITM_CHAR(item[0],len(str(value)),_C.c_char_p(str(value)),retlen)
    else:
        ans=_C.c_int(0)
        itmlst=DBI_ITM_INT(item[0],_C.c_int(int(value)))
    try:
        tree.lock()
        status=__TreeSetDbi(tree.ctx,_C.cast(
            _C.pointer(itmlst),_C.c_void_p))
    finally:
        tree.unlock()
    if not (status & 1):
        raise TreeException,MdsGetMsg(status)
