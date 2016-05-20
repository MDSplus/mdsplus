def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C
_mdsshr=_mimport('_mdsshr')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_compound=_mimport('compound')
_treenode=_mimport('treenode')
_Exceptions=_mimport('mdsExceptions')
_ver=_mimport('version')


def TreeSetCurrentShotId(tree,shot):
  return __TreeSetCurrentShotId(_ver.tobytes(tree),shot)

def TreeFindNode(ctx,path):
    n=_C.c_int32()
    status=__TreeFindNode(ctx,_ver.tobytes(path),n)
    if (status & 1):
        return n.value
    else:
        raise _Exceptions.statusToException(status)

def TreeGetDefault(ctx):
    """Get default node"""
    ans=_C.c_int32(0)
    status = __TreeShr._TreeGetDefaultNid(ctx,_C.pointer(ans))
    if (status & 1):
        return ans.value
    else:
        raise _Exceptions.statusToException(status)

def TreeSetDefault(ctx,n):
    """Set default node"""
    status = __TreeShr._TreeSetDefaultNid(ctx,_C.c_int32(n))
    if (status & 1):
        return
    else:
        raise _Exceptions.statusToException(status)

def TreeSetUsage(ctx,n,usage):
    """Set node usage"""
    try:
        status = __TreeShr._TreeSetUsage(ctx,_C.c_int32(n),_C.c_byte(usage))
    except:
        raise Exception("Feature not present in current MDSplus installation. Upgrade to newer version of MDSplus.")
    if (status & 1):
        return
    else:
        raise _Exceptions.statusToException(status)

def TreeGetPath(n):
    p=__TreeGetPath(n.tree.ctx,n.nid)
    ans=_C.cast(p,_C.c_char_p).value
    ans = _ver.tostr(ans)
    if not p is None:
        _TreeFree(p)
    return ans

def TreeFindNodeTags(n):
    ctx=_C.c_void_p(0)
    tags=list()
    done=False
    while not done:
        tag_ptr=__TreeFindNodeTags(n.tree.ctx,n.nid,_C.pointer(ctx))
        try:
            tags.append(_ver.tostr(_C.cast(tag_ptr,_C.c_char_p).value.rstrip()))
            _TreeFree(tag_ptr)
        except:
            done=True
    tags = _array.makeArray(tags).astype(_ver.npstr)
    return tags


def TreeFindNodeWild(tree, wild, *usage):
    usage_table=_treenode._usage_table
    if len(usage) == 0:
        usage_mask=0xFFFF
    else :
        try:
            usage_mask=0
            for u in usage:
                usage_mask |= 1 << usage_table[u.upper()]
        except KeyError:
            raise KeyError('Invalid usage must be one of: %s' % list(usage_table.keys()))

    nid=_C.c_int32()
    ctx=_C.c_void_p(0)
    try:
        while __TreeFindNodeWild(tree, _ver.tobytes(wild), _C.pointer(nid), _C.pointer(ctx), _C.c_int32(usage_mask)) & 1 != 0:
            yield nid.value
    except GeneratorExit:
        pass
    __TreeFindNodeEnd(tree, _C.pointer(ctx))

def TreeFindTagWild(tree,wild):
    nid=_C.c_int32(0)
    ctx=_C.c_void_p(0)
    try:
        while True:
            tag_ptr = __TreeFindTagWild(tree,_ver.tobytes(wild),_C.pointer(nid),_C.pointer(ctx))
            if  not tag_ptr:
                break
            ans = _C.cast(tag_ptr,_C.c_char_p).value.rstrip()
            yield ans
    except GeneratorExit:
        pass
    _TreeFindTagEnd(_C.pointer(ctx))


def TreeGetRecord(n,*altvalue):
    """Get record from MDSplus tree. Accepts path, TreeNode or integer and the value"""
    value=descriptor_xd()
    status=__TreeGetRecord(n.tree.ctx,n.nid,_C.pointer(value))
    if (status & 1):
        ans = value.value
        return ans
    elif len(altvalue)==1 and status == _Exceptions.treeshrExceptions.TreeNODATA.status:
        return altvalue[0]
    else:
        raise _Exceptions.statusToException(status)

def TreeGetSegment(n,idx):
    """Get segment from MDSplus node."""
    value=descriptor_xd()
    dim=descriptor_xd()
    status=__TreeGetSegment(n.tree.ctx,n.nid,idx,_C.pointer(value),_C.pointer(dim))
    if (status & 1):
        return _compound.Signal(value.value,None,dim.value)
    else:
        raise _Exceptions.statusToException(status)

def TreeGetSegmentLimits(n,idx):
    """Get segment from MDSplus node."""
    start=descriptor_xd()
    end=descriptor_xd()
    status=__TreeGetSegmentLimits(n.tree.ctx,n.nid,idx,_C.pointer(start),_C.pointer(end))
    if (status & 1):
        try:
            start=start.value.evaluate()
        except:
            start=None
        try:
            end=end.value.evaluate()
        except:
            end=None
        if start is not None or end is not None:
            return (start,end,)
        else:
            return None
    else:
        raise _Exceptions.statusToException(status)

def TreePutRecord(n,value):
    """Put record into MDSplus tree. Accepts path, TreeNode or integer and the value"""
    status=__TreePutRecord(n.tree.ctx,n.nid,_C.pointer(descriptor(value)),0)
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeSetTimeContext(begin,end,delta):
    """Set begin, end and delta for segmented record retrieval"""
    status = __TreeSetTimeContext(_C.pointer(descriptor(begin)),
                                  _C.pointer(descriptor(end)),
                                  _C.pointer(descriptor(delta)))
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeDoMethod(n,method,*args):
    """Do a method of an MDSplus device. Accepts path, TreeNode or integer and the value"""
    arglist=[n.tree.ctx,_C.pointer(descriptor(n)),_C.pointer(descriptor(method))]
    ans_d=descriptor_xd()
    for arg in args:
        arglist.append(_C.pointer(descriptor(arg)))
    arglist.append(_C.pointer(ans_d))
    arglist.append(_C.c_void_p(0xffffffff))
    status=__TreeDoMethod(*arglist)
    if (status & 1):
        return ans_d.value
    else:
        raise _Exceptions.statusToException(status)

def TreeStartConglomerate(tree,num):
    """Start a conglomerate in a tree."""
    tree.lock()
    try:
        status=__TreeStartConglomerate(tree.ctx,num)
    finally:
        tree.unlock()
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeEndConglomerate(tree):
    """End a conglomerate in a tree."""
    tree.lock()
    try:
        status=__TreeEndConglomerate(tree.ctx)
    finally:
        tree.unlock()
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeTurnOn(n):
    """Turn on a tree node."""
    n.tree.lock()
    try:
        status=__TreeTurnOn(n.tree.ctx,n.nid)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeTurnOff(n):
    """Turn off a tree node."""
    n.tree.lock()
    try:
        status=__TreeTurnOff(n.tree.ctx,n.nid)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeOpen(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpen(_C.pointer(ctx),_ver.tobytes(tree),shot,0)
    if (status & 1):
        return ctx
    else:
        raise _Exceptions.statusToException(status)

def TreeOpenReadOnly(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpen(_C.pointer(ctx),_ver.tobytes(tree),shot,1)
    if (status & 1):
        return ctx
    else:
        raise _Exceptions.statusToException(status)

def TreeOpenNew(tree,shot):
    ctx=_C.c_void_p(0)
    status = __TreeOpenNew(_C.pointer(ctx),_ver.tobytes(tree),shot)
    if (status & 1):
        return ctx
    else:
        raise _Exceptions.statusToException(status)

def TreeOpenEdit(tree):
    status = __TreeOpenEdit(_C.pointer(tree.ctx),_ver.tobytes(tree.tree),tree.shot)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeQuitTree(tree):
    status = __TreeQuitTree(_C.pointer(tree.ctx),_ver.tobytes(tree.tree),tree.shot)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeWriteTree(tree,name,shot):
    status = __TreeWriteTree(_C.pointer(tree.ctx),_ver.tobytes(name),shot)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeAddNode(tree,name,usage):
    usage_table=_treenode._usage_table
    nid=_C.c_int32(0)
    try:
        usagenum=usage_table[usage.upper()]
        if usagenum==-1:
            status = __TreeAddNode(tree.ctx,_ver.tobytes(name),nid,1)
            if status & 1:
                status = __TreeSetSubtree(tree.ctx,nid)
        else:
            status = __TreeAddNode(tree.ctx,_ver.tobytes(name),nid,usagenum)
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    except KeyError:
        raise KeyError('Invalid usage must be one of: %s' % list(usage_table.keys()))
    return nid.value

def TreeSetSubtree(node,flag):
    node.tree.lock()
    try:
        if flag:
            status=__TreeSetSubtree(node.tree.ctx,node.nid)
        else:
            status=__TreeSetNoSubtree(node.tree.ctx,node.nid)
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    finally:
        node.tree.unlock()

def TreeRenameNode(node,name):
    node.tree.lock()
    try:
        status = __TreeRenameNode(node.tree.ctx,node.nid,_ver.tobytes(name))
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    finally:
        node.tree.unlock()

def TreeAddTag(tree,nid,tag):
    tree.lock()
    try:
        status = __TreeAddTag(tree.ctx,nid,_ver.tobytes(tag))
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    finally:
        tree.unlock()

def TreeRemoveTag(tree,tag):
    tree.lock()
    try:
        status = __TreeRemoveTag(tree.ctx,_ver.tobytes(tag))
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    finally:
        tree.unlock()

def TreeAddConglom(tree,name,devname):
    nid=_C.c_int32(0)
    status = __TreeAddConglom(tree.ctx,_ver.tobytes(name),_ver.tobytes(devname),nid)
    if not (status & 1):
        raise _Exceptions.statusToException(status)
    return nid.value

def TreeDeleteNode(tree,nid,reset):
    count=_C.c_int32(0)
    reset_flag=0
    if reset:
      reset_flag=1
    status = __TreeDeleteNodeInitialize(tree.ctx,nid,count,reset_flag)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeDeleteNodeExecute(tree):
    __TreeDeleteNodeExecute(tree.ctx)

def TreeDeletePulse(tree,shot):
    status = __TreeDeletePulseFile(tree.ctx,shot,1)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeRestoreContext(ctx):
    try:
        return _TreeSwitchDbid(ctx)
    except:
        return _TreeSwitchDbid(_C.c_void_p(0))

def TreeGetContext():
        ctx=_TreeSwitchDbid(_C.c_void_p(0))
        if ctx is None:
            raise _Exceptions.TreeNOT_OPEN()
        else:
            _TreeSwitchDbid(ctx)
        return ctx

def TreeCreatePulseFile(ctx,shot,included=None):
    if included is None:
        status = __TreeCreatePulseFile(ctx,shot,0,_C.c_void_p(0))
    else:
        status = __TreeCreatePulseFile(ctx,shot,len(included),_C.c_void_p(included.ctypes.data))
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeCleanDatafile(ctx,tree,shot):
    status = __TreeCleanDatafile(ctx, _ver.tobytes(tree), shot)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeCompressDatafile(ctx,tree,shot):
    status = __TreeCompressDatafile(ctx, _ver.tobytes(tree), shot)
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeClose(ctx,tree,shot):
    status = __TreeClose(_C.pointer(ctx),_ver.tobytes(tree),shot)
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeCloseAll(ctx):
    if ctx is not None:
        status1 = status = __TreeClose(_C.pointer(ctx),None,0)
        while (status & 1) == 1:
            try:
                status = __TreeClose(_C.pointer(ctx),None,0)
            except:
                status = 0
        return status1

def TreeSetVersionDate(date):
    status = __TreeSetViewDate(_mdsshr.DateToQuad(_ver.tobytes(date)).data())
    if not (status & 1):
        raise _Exceptions.statusToException(status)

def TreeGetVersionDate():
    dt=_C.c_ulonglong(0)
    status = __TreeGetViewDate(dt)
    if not (status & 1):
        raise _Exceptions.statusToException(status)
    return _scalar.Uint64(dt.value).date

def TreeGetNumSegments(n):
    """Get number of segments in a node."""
    n.tree.lock()
    try:
        num=_C.c_int32(0)
        status=__TreeGetNumSegments(n.tree.ctx,n.nid,_C.pointer(num))
    finally:
        n.tree.unlock()
    if (status & 1):
        return num.value
    else:
        raise _Exceptions.statusToException(status)

def TreePutTimestampedSegment(n,timestampArray,value):
    """Put a timestampedsegment"""
    timestampArray=_array.Int64Array(timestampArray)
    value=_array.makeArray(value)
    n.tree.lock()
    try:
        status=__TreePutTimestampedSegment(n.tree.ctx,n.nid,descriptor_a(timestampArray).pointer,_C.pointer(descriptor_a(value)))
    finally:
        n.tree.unlock()
    if (status & 1):
        return status;
    else:
        raise _Exceptions.statusToException(status)

def TreeMakeTimestampedSegment(n,timestamps,value,idx,rows_filled):
    """Put a segment"""
    timestamps=_array.Int64Array(timestamps)
    n.tree.lock()
    try:
        status=__TreeMakeTimestampedSegment(n.tree.ctx,n.nid,descriptor_a(timestamps).pointer,_C.pointer(descriptor_a(value)),idx,rows_filled)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status;
    else:
        raise _Exceptions.statusToException(status)

def TreePutSegment(n,value,idx):
    """Put a segment"""
    n.tree.lock()
    try:
        status=__TreePutSegment(n.tree.ctx,n.nid,idx,_C.pointer(descriptor(value)))
    finally:
        n.tree.unlock()
    if (status & 1):
        return status;
    else:
        raise _Exceptions.statusToException(status)

def TreePutRow(n,bufsize,array,timestamp):
    """Begin a segment."""
    n.tree.lock()
    try:
        array=_array.makeArray(array)
        status=__TreePutRow(n.tree.ctx,n.nid,bufsize,_C.pointer(_C.c_int64(int(timestamp))),
                             _C.pointer(descriptor_a(array)))
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeBeginTimestampedSegment(n,value,idx):
    """Begin a segment"""
    n.tree.lock()
    try:
        status=__TreeBeginTimestampedSegment(n.tree.ctx,n.nid,_C.pointer(descriptor_a(value)),idx)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status;
    else:
        raise _Exceptions.statusToException(status)

def TreeMakeSegment(n,start,end,dimension,initialValue,idx):
    n.tree.lock()
    try:
        if isinstance(initialValue,_compound.Compound):
            __TreeMakeSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),
                            _C.POINTER(descriptor),_C.c_int32,_C.c_int32]
            status=__TreeMakeSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                     _C.pointer(descriptor(dimension)),_C.pointer(descriptor(initialValue)),idx,1)
        else:
            __TreeMakeSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),
                            _C.POINTER(descriptor_a),_C.c_int32,_C.c_int32]
            status=__TreeMakeSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                     _C.pointer(descriptor(dimension)),_C.pointer(descriptor_a(initialValue)),idx,initialValue.shape[0])
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeBeginSegment(n,start,end,dimension,initialValue,idx):
    """Begin a segment."""
    n.tree.lock()
    try:
        status=__TreeBeginSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                   _C.pointer(descriptor(dimension)),_C.pointer(descriptor_a(initialValue)),idx)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

def TreeUpdateSegment(n,start,end,dimension,idx):
    """Update a segment."""
    n.tree.lock()
    try:
        status=__TreeUpdateSegment(n.tree.ctx,n.nid,_C.pointer(descriptor(start)),_C.pointer(descriptor(end)),
                                    _C.pointer(descriptor(dimension)),idx)
    finally:
        n.tree.unlock()
    if (status & 1):
        return status
    else:
        raise _Exceptions.statusToException(status)

class DBI_ITM_INT(_C.Structure):
    _fields_=[("buffer_length",_C.c_ushort),("code",_C.c_ushort),("pointer",_C.POINTER(_C.c_int32)),
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
              ("retlen",_C.POINTER(_C.c_int32)),
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
        raise KeyError('Item name must be one of %s' % list(itemlist.keys()))
    if item[1] is str:
        ans=_C.c_char_p(_ver.tobytes('x'.rjust(item[2])))
        retlen=_C.c_int32(0)
        itmlst=DBI_ITM_CHAR(item[0],item[2],ans,retlen)
    else:
        ans=_C.c_int32(0)
        itmlst=DBI_ITM_INT(item[0],ans)
    tree.lock()
    try:
        status=__TreeGetDbi(tree.ctx,_C.cast(
            _C.pointer(itmlst),_C.c_void_p))
    finally:
        tree.unlock()
    if not (status & 1):
        raise _Exceptions.statusToException(status)
    if item[1] is str:
        return _ver.tostr(ans.value)
    else:
        return item[1](ans.value)

def TreeSetDbi(tree,itemname,value):
    itemlist={'VERSIONS_IN_MODEL':(10,bool),'VERSIONS_IN_PULSE':(11,bool)}
    try:
        item=itemlist[itemname.upper()]
    except KeyError:
        raise KeyError('Item name must be one of %s' % list(itemlist.keys()))
    """  itemlist does not contain str type
    if item[1] is str: tuple(
        retlen=_C.c_int32(0)
        value = _ver.tobytes(value)
        itmlst = DBI_ITM_CHAR(item[0], len(value), _C.c_char_p(value), retlen)
    else:
    """
    itmlst=DBI_ITM_INT(item[0],_C.c_int32(int(value)))
    tree.lock()
    try:
        status=__TreeSetDbi(tree.ctx,_C.cast(
            _C.pointer(itmlst),_C.c_void_p))
    finally:
        tree.unlock()
    if not (status & 1):
        raise _Exceptions.statusToException(status)

_descriptor=_mimport('_descriptor')
descriptor_xd=_descriptor.descriptor_xd
descriptor_a=_descriptor.descriptor_a
descriptor=_descriptor.descriptor

__TreeShr=_ver.load_library('TreeShr')
__TreeOpen=__TreeShr._TreeOpen
__TreeOpen.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32,_C.c_int32]
__TreeOpenNew=__TreeShr._TreeOpenNew
__TreeOpenNew.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32]
__TreeOpenEdit=__TreeShr._TreeOpenEdit
__TreeOpenEdit.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32]
__TreeQuitTree=__TreeShr._TreeQuitTree
__TreeQuitTree.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32]
__TreeWriteTree=__TreeShr._TreeWriteTree
__TreeWriteTree.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32]
__TreeClose=__TreeShr._TreeClose
__TreeClose.argtypes=[_C.POINTER(_C.c_void_p),_C.c_char_p,_C.c_int32]
__TreeFindNode=__TreeShr._TreeFindNode
__TreeFindNode.argtypes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int32)]
_TreeFreeDbid=__TreeShr.TreeFreeDbid
_TreeFreeDbid.argtypes=[_C.c_void_p]
__TreeGetPath=__TreeShr._TreeGetPath
__TreeGetPath.argtypes=[_C.c_void_p,_C.c_int32]
__TreeGetPath.restype=_C.c_void_p

__TreeFindNodeWild=__TreeShr._TreeFindNodeWild
__TreeFindNodeWild.argtpes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int32),_C.POINTER(_C.c_void_p),_C.c_int32]
__TreeFindNodeWild.restype=_C.c_void_p
__TreeFindNodeEnd=__TreeShr._TreeFindNodeEnd
__TreeFindNodeEnd.argtypes=[_C.c_void_p,_C.POINTER(_C.c_void_p)]

__TreeFindTagWild=__TreeShr._TreeFindTagWild
__TreeFindTagWild.argtpes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int32),_C.POINTER(_C.c_void_p)]
__TreeFindTagWild.restype=_C.c_void_p

__TreeGetDbi=__TreeShr._TreeGetDbi
__TreeGetDbi.argtypes=[_C.c_void_p,_C.c_void_p]
__TreeSetDbi=__TreeShr._TreeSetDbi
__TreeSetDbi.argtypes=[_C.c_void_p,_C.c_void_p]
_TreeFindTagEnd=__TreeShr.TreeFindTagEnd
_TreeFindTagEnd.argtypes=[_C.POINTER(_C.c_void_p)]
_TreeFree=__TreeShr.TreeFree
_TreeFree.argtypes=[_C.c_void_p]
_TreeSwitchDbid=__TreeShr.TreeSwitchDbid
_TreeSwitchDbid.argtypes=[_C.c_void_p]
_TreeSwitchDbid.restype=_C.c_void_p
__TreeCreatePulseFile=__TreeShr._TreeCreatePulseFile
__TreeCreatePulseFile.argtypes=[_C.c_void_p,_C.c_int32,_C.c_int32,_C.c_void_p]

__TreeCleanDatafile=__TreeShr._TreeCleanDatafile
__TreeCleanDatafile.argtypes=[_C.c_void_p,_C.c_char_p,_C.c_int32]
__TreeCompressDatafile=__TreeShr._TreeCompressDatafile
__TreeCompressDatafile.argtypes=[_C.c_void_p,_C.c_char_p,_C.c_int32]

__TreeDeletePulseFile=__TreeShr._TreeDeletePulseFile
__TreeDeletePulseFile.argtypes=[_C.c_void_p,_C.c_int32,_C.c_int32]
__TreeDoMethod=__TreeShr._TreeDoMethod
__TreeGetRecord=__TreeShr._TreeGetRecord
__TreeGetRecord.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor_xd)]
__TreePutRecord=__TreeShr._TreePutRecord
__TreePutRecord.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor),_C.c_int32]
__TreeSetTimeContext=__TreeShr.TreeSetTimeContext
__TreeSetTimeContext.argtypes=[_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor)]
__TreeTurnOn=__TreeShr._TreeTurnOn
__TreeTurnOn.argtypes=[_C.c_void_p,_C.c_int32]
__TreeTurnOff=__TreeShr._TreeTurnOff
__TreeTurnOff.argtypes=[_C.c_void_p,_C.c_int32]
__TreeFindNodeTags=__TreeShr._TreeFindNodeTags
__TreeFindNodeTags.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(_C.c_void_p)]
__TreeFindNodeTags.restype=_C.c_void_p
__TreeAddNode=__TreeShr._TreeAddNode
__TreeAddNode.argtypes=[_C.c_void_p,_C.c_char_p,_C.POINTER(_C.c_int32),_C.c_int32]
__TreeSetSubtree=__TreeShr._TreeSetSubtree
__TreeSetSubtree.argtypes=[_C.c_void_p,_C.c_int32]
__TreeSetNoSubtree=__TreeShr._TreeSetNoSubtree
__TreeSetNoSubtree.argtypes=[_C.c_void_p,_C.c_int32]
__TreeAddTag=__TreeShr._TreeAddTag
__TreeAddTag.argtypes=[_C.c_void_p,_C.c_int32,_C.c_char_p]
__TreeRemoveTag=__TreeShr._TreeRemoveTag
__TreeRemoveTag.argtypes=[_C.c_void_p,_C.c_char_p]
__TreeRenameNode=__TreeShr._TreeRenameNode
__TreeRenameNode.argtypes=[_C.c_void_p,_C.c_int32,_C.c_char_p]
__TreeAddConglom=__TreeShr._TreeAddConglom
__TreeAddConglom.argtypes=[_C.c_void_p,_C.c_char_p,_C.c_char_p,_C.POINTER(_C.c_int32)]
__TreeDeleteNodeInitialize=__TreeShr._TreeDeleteNodeInitialize
__TreeDeleteNodeInitialize.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(_C.c_int32),_C.c_int32]
__TreeDeleteNodeExecute=__TreeShr._TreeDeleteNodeExecute
__TreeDeleteNodeExecute.argtypes=[_C.c_void_p]
_TreeGetCurrentShotId=__TreeShr.TreeGetCurrentShotId
_TreeGetCurrentShotId.argtypes=[_C.c_char_p]
__TreeSetCurrentShotId=__TreeShr.TreeSetCurrentShotId
__TreeSetCurrentShotId.argtypes=[_C.c_char_p,_C.c_int32]
__TreeSetViewDate=__TreeShr.TreeSetViewDate
__TreeSetViewDate.argtypes=[_C.POINTER(_C.c_ulonglong)]
__TreeGetViewDate=__TreeShr.TreeGetViewDate
__TreeGetViewDate.argtypes=[_C.POINTER(_C.c_ulonglong)]
__TreeBeginSegment=__TreeShr._TreeBeginSegment
__TreeBeginSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor_a),_C.c_int32]
__TreeMakeSegment=__TreeShr._TreeMakeSegment
__TreeBeginTimestampedSegment=__TreeShr._TreeBeginTimestampedSegment
__TreeBeginTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor_a),_C.c_int32]
__TreeMakeTimestampedSegment=__TreeShr._TreeMakeTimestampedSegment
__TreeMakeTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.c_void_p,_C.POINTER(descriptor_a),_C.c_int32,_C.c_int32]
__TreeUpdateSegment=__TreeShr._TreeUpdateSegment
__TreeUpdateSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(descriptor),_C.POINTER(descriptor),_C.POINTER(descriptor),_C.c_int32]
__TreePutSegment=__TreeShr._TreePutSegment
__TreePutSegment.argtpes=[_C.c_void_p,_C.c_int32,_C.c_int32,_C.POINTER(descriptor_a)]
__TreePutTimestampedSegment=__TreeShr._TreePutTimestampedSegment
__TreePutTimestampedSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.c_void_p,_C.POINTER(descriptor_a)]
__TreePutRow=__TreeShr._TreePutRow
__TreePutRow.argtypes=[_C.c_void_p,_C.c_int32,_C.c_int32,_C.POINTER(_C.c_int64),_C.POINTER(descriptor_a)]
__TreeGetNumSegments=__TreeShr._TreeGetNumSegments
__TreeGetNumSegments.argtypes=[_C.c_void_p,_C.c_int32,_C.POINTER(_C.c_int32)]
__TreeGetSegment=__TreeShr._TreeGetSegment
__TreeGetSegment.argtypes=[_C.c_void_p,_C.c_int32,_C.c_int32,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__TreeGetSegmentLimits=__TreeShr._TreeGetSegmentLimits
__TreeGetSegmentLimits.argtypes=[_C.c_void_p,_C.c_int32,_C.c_int32,_C.POINTER(descriptor_xd),_C.POINTER(descriptor_xd)]
__TreeStartConglomerate=__TreeShr._TreeStartConglomerate
__TreeStartConglomerate.argtypes=[_C.c_void_p,_C.c_int32]
__TreeEndConglomerate=__TreeShr._TreeEndConglomerate
__TreeEndConglomerate.argtypes=[_C.c_void_p]

try:
    _TreeUsePrivateCtx=__TreeShr.TreeUsePrivateCtx
    _TreeUsePrivateCtx.argtypes=[_C.c_int32]
    def TreeUsePrivateCtx(onoff):
        return _TreeUsePrivateCtx(onoff)
    _TreeUsingPrivateCtx=__TreeShr.TreeUsingPrivateCtx
    def TreeUsingPrivateCtx():
        return _TreeUsingPrivateCtx() == 1
except Exception:
    def TreeUsingPrivateCtx():
        return False
