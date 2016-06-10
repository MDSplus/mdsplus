def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import threading as _threading
import ctypes as _C
import numpy as _N

_mdsshr=_mimport('_mdsshr')
_treeshr=_mimport('_treeshr')
_Exceptions=_mimport('mdsExceptions')
_data=_mimport('mdsdata')
_scalar=_mimport('mdsscalar')
_treenode=_mimport('treenode')
_ver=_mimport('version')

_activeTree={}

class _ThreadData(_threading.local):
    def __init__(self):
        self._activeTree=0
        self.private=False
_thread_data=_ThreadData()

_hard_lock=_threading.Lock()


def _getThreadName(thread=None):
    if isinstance(thread,str):
        threadName=thread
    elif _thread_data.private:
        if thread is None:
            threadName = _threading.current_thread().getName()
        else:
            threadName = thread.getName()
    else:
        threadName = 'main'
    return threadName

def _setActiveTree(ctx,thread=None):
    _activeTree[_getThreadName(thread)]=ctx

def _getActiveTree(thread=None):
    threadName=_getThreadName(thread)
    if threadName in _activeTree:
        ctx = _activeTree[threadName]
    else:
        ctx = 0
    return ctx

class _TreeCtx(object):
    ctxs=[]
    def __init__(self,ctx):
        self.ctx=ctx
        _TreeCtx.ctxs.append(ctx)
    def __del__(self):
        _TreeCtx.ctxs.remove(self.ctx)
        if self.ctx not in _TreeCtx.ctxs:
            status=_treeshr.TreeCloseAll(_C.c_void_p(self.ctx))
            if (status & 1):
                _treeshr._TreeFreeDbid(_C.c_void_p(self.ctx))

class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock=_threading.RLock()
    _id=0

	# support for the with-structure
    def __enter__(self):
    	return self
    def __exit__(self, type, value, traceback):
        """ Cleanup for with statement. If tree is open for edit close it. """
        if self.open_for_edit:
            self.quit()


    def __getattr__(self,name):
        """
        Implements value=tree.attribute

        Attributes defined:

         - default - TreeNode, Current default node
         - modified - bool, True if tree structure has been modified
         - name - str, Tree name
         - open_for_edit - bool, True if tree open for edit
         - open_readonly - boo, True if tree was opened readonly
         - shot - int, Shot number
         - shotid - int, Shot number
         - tree - str, Tree name
         - versions_in_model - bool, True if versions enabled in the model
         - versions_in_pulse - bool, True if versions enabled in the pulse

        @param name: Name of attribute
        @type name: str
        @return: Value of attribute
        @rtype: various
        """
        if name.upper() == name:
            try:
                return self.getNode(name)
            except:
                pass
        if name.lower() == 'default':
            return self.getDefault()
        if name.lower() == 'top':
            return _treenode.TreeNode(0,self)
        if name.lower() == 'shot':
            name='shotid'
        elif name.lower() == 'tree':
            name='name'
        try:
            return _treeshr.TreeGetDbi(self,name)
        except KeyError:
            try:
                return self.__dict__[name]
            except:
                raise AttributeError('No such attribute: '+name)

    def usePrivateCtx(cls,on=True):
        _thread_data.private=on
        _treeshr.TreeUsePrivateCtx(on)
    usePrivateCtx=classmethod(usePrivateCtx)

    def __init__(self, tree=None, shot=-1, mode='NORMAL'):
        """Create a Tree instance. Specify a tree and shot and optionally a mode.
        If providing the mode argument it should be one of the following strings:
        'Normal','Edit','New','ReadOnly'.
        If no arguments provided, create instance of the active tree. (i.e. Tree())
        @param tree: Name of tree to open
        @type tree: str
        @param shot: Shot number
        @type shot: int
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        """
        _hard_lock.acquire()
        try:
            if tree is None:
                try:
                    ctx=_treeshr.TreeGetContext()
                except:
                    ctx=_getActiveTree()
                if ctx == 0:
                    raise _Exceptions.TreeNOT_OPEN()
                else:
                    self.ctx=_C.c_void_p(ctx)
            else:
                if mode.upper() == 'NORMAL':
                    self.ctx=_treeshr.TreeOpen(tree,shot)
                elif mode.upper() == 'EDIT':
                    self.ctx=_treeshr.TreeOpen(tree,shot)
                    self.edit()
                elif mode.upper() == 'NEW':
                    self.ctx=_treeshr.TreeOpenNew(tree,shot)
                elif mode.upper() == 'READONLY':
                    self.ctx=_treeshr.TreeOpenReadOnly(tree,shot)
                else:
                    raise AttributeError('Invalid mode specificed, use "Normal","Edit","New" or "ReadOnly".')
            if isinstance(self.ctx,_C.c_void_p) and self.ctx.value is not None:
                _setActiveTree(self.ctx.value)
                _treeshr.TreeRestoreContext(self.ctx)
                if tree is not None:
                    self.tctx=_TreeCtx(self.ctx.value)
        finally:
            _hard_lock.release()

    def __deepcopy__(self,memo):
        return self

    def __setattr__(self,name,value):
        """
        Implements tree.attribute=value

        Attributes defined:

         - default - TreeNode, default node in tree
         - versions_in_model - bool, True to enable versions in model
         - versions_in_pulse - bool, True to enable versions in pulse

        @param name: Name of attribute to set
        @type name: str
        @param value: Value of attribute
        @type value: various
        @rtype: None
        """
        if name.lower() in ('modified','name','open_for_edit','open_readonly','shot','shotid','tree'):
            raise AttributeError('Read only attribute: '+name)
        elif name == 'default':
            self.setDefault(value)
        else:
            try:
                _treeshr.TreeSetDbi(self,name,value)
            except KeyError:
                self.__dict__[name]=value

#    def __str__(self):
#        """Return string
#        @return: String of tree name
#        @rtype: str
#        """
#        return str(self.tree)

    def __repr__(self):
        """Return representation
        @return: String representation of open tree
        @rtype: str
        """
        if self.open_for_edit:
            mode="Edit"
        elif self.open_readonly:
            mode="Readonly"
        else:
            mode="Normal"
        return self.__class__.__name__+'("%s",%d,"%s")' % (self.tree,self.shot,mode)

    __str__=__repr__
    def addDevice(self,nodename,model):
        """Add a device to the tree of the specified device model type.
        @param nodename: Absolute or relative path specification of the head node of the device. All ancestors of node must exist.
        @type nodename: str
        @param model: Model name of the device being added.
        @type model: str
        @return: Head node of device added
        @rtype: TreeNode
        """
        Tree.lock()
        try:
            nid=_treeshr.TreeAddConglom(self,nodename,model)
        finally:
            Tree.unlock()
        return _treenode.TreeNode(nid,self)

    def addNode(self,nodename,usage='ANY'):
        """Add a node to the tree. Tree must be in edit mode.
        @param nodename: Absolute or relative path specification of new node. All ancestors of node must exist.
        @type nodename: str
        @param usage: Usage of node.
        @type usage: str
        @return: Node created.
        @rtype: TreeNode
        """
        Tree.lock()
        try:
            nid = _treeshr.TreeAddNode(self,nodename,usage)
        finally:
            Tree.unlock()
        return _treenode.TreeNode(nid,self)

    def createPulse(self,shot):
        """Create pulse.
        @param shot: Shot number to create
        @type shot: int
        @rtype: None
        """
        Tree.lock()
        try:
            try:
                subtrees=self.getNodeWild('***','subtree')
                included=subtrees.nid_number.compress(subtrees.include_in_pulse)
                included=included.toList()
                included.insert(0,0)
                included=_N.array(included)
            except:
                included = None
            _treeshr.TreeCreatePulseFile(self.ctx,shot,included)
        finally:
            Tree.unlock()

    def deleteNode(self,wild):
        """Delete nodes (and all their descendants) from the tree. Note: If node is a member of a device,
        all nodes from that device are also deleted as well as any descendants that they might have.
        @param wild: Wildcard path speficier of nodes to delete from tree.
        @type wild: str
        @rtype: None
        """
        Tree.lock()
        try:
            first=True
            nodes=self.getNodeWild(wild)
            for node in nodes:
                _treeshr.TreeDeleteNode(self,int(node.nid_number),first)
                first=False
            _treeshr.TreeDeleteNodeExecute(self)
        finally:
            Tree.unlock()

    def deletePulse(self,shot):
        """Delete pulse.
        @param shot: Shot number to delete
        @type shot: int
        @rtype: None
        """
        Tree.lock()
        try:
            _treeshr.TreeDeletePulse(self,shot)
        finally:
            Tree.unlock()

    def edit(self):
        """Open tree for editing.
        @rtype: None"""
        Tree.lock()
        try:
            _treeshr.TreeOpenEdit(self)
        finally:
            Tree.unlock()

    def findTagsIter(self, wild):
        """An iterator for the tagnames from a tree given a wildcard specification.
        @param wild: wildcard spec.
        @type name: str
        @return: iterator of tagnames (strings) that match the wildcard specification
        @rtype: iterator
        """
        for n in _treeshr.TreeFindTagWild(self.ctx, wild):
            yield n

    def findTags(self,wild):
        """Find tags matching wildcard expression
        @param wild: wildcard string to match tagnames.
        @type wild: str
        @return: Array of tag names matching wildcard expression
        @rtype: ndarray
        """
        return tuple(self.findTagsIter(wild))

    def getCurrent(treename):
        """Return current shot for specificed treename
        @param treename: Name of tree
        @type treename: str
        @return: Current shot number for the specified tree
        @rtype: int
        """
        Tree.lock()
        try:
            shot=_treeshr._TreeGetCurrentShotId(_ver.tobytes(treename))
        finally:
            Tree.unlock()
        if shot==0:
            raise _Exceptions.TreeNOCURRENT()
        return shot
    getCurrent=staticmethod(getCurrent)

    def getDefault(self):
        """Return current default TreeNode
        @return: Current default node
        @rtype: TreeNode
        """
        Tree.lock()
        try:
            ans = _treenode.TreeNode(_treeshr.TreeGetDefault(self.ctx),self)
        finally:
            Tree.unlock()
        return ans

    def getNode(self,name):
        """Locate node in tree. Returns TreeNode if found. Use double backslashes in node names.
        @param name: Name of node. Absolute or relative path. No wildcards.
        @type name: str
        @return: Node if found
        @rtype: TreeNode
        """
        if isinstance(name,(int,_scalar.Int32)):
            return _treenode.TreeNode(name,self)
        else:
            Tree.lock()
            try:
                ans=_treenode.TreeNode(_treeshr.TreeFindNode(self.ctx,str(name)),self)
            finally:
                Tree.unlock()
            return ans

    def getNodeWildIter(self, name, *usage):
        """An iterator for the nodes in a tree given a wildcard specification.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: iterator of TreeNodes that match the wildcard and usage specifications
        @rtype: iterator
        """
        for n in _treeshr.TreeFindNodeWild(self.ctx, name, *usage):
            yield _treenode.TreeNode(n,self)

    def getNodeWild(self,name,*usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: TreeNodeArray of nodes matching the wildcard path specification and usage types.
        @rtype: TreeNodeArray
        """
        nids=list()
        for n in self.getNodeWildIter(name,*usage):
            nids.append(n.nid)
        return _treenode.TreeNodeArray(nids,self)

    def getVersionDate():
        """Get date used for retrieving versions
        @return: Reference date for retrieving data is versions enabled
        @rtype: str
        """
        return _treeshr.TreeGetVersionDate()
    getVersionDate=staticmethod(getVersionDate)

    def isModified(self):
        """Check to see if tree is open for edit and has been modified
        @return: True if tree structure has been modified.
        @rtype: bool
        """
        return self.modified

    def isOpenForEdit(self):
        """Check to see if tree is open for edit
        @return: True if tree is open for edit
        @rtype: bool
        """
        return self.open_for_edit

    def isReadOnly(self):
        """Check to see if tree was opened readonly
        @return: True if tree is open readonly
        @rtype: bool
        """
        return self.open_readonly

    def lock(cls):
        """Internal use only. Thread synchronization locking.
        """
        if not _treeshr.TreeUsingPrivateCtx():
            cls._lock.acquire()
    lock=classmethod(lock)

    def quit(self):
        """Close edit session discarding node structure and tag changes.
        @rtype: None
        """
        if self.open_for_edit:
            Tree.lock()
            try:
                _treeshr.TreeQuitTree(self)
            finally:
                Tree.unlock()

    def removeTag(self,tag):
        """Remove a tagname from the tree
        @param tag: Tagname to remove.
        @type tag: str
        @rtype: None
        """
        _treeshr.TreeRemoveTag(self,tag)

    def restoreContext(self):
        """Internal use only. Use internal context associated with this tree."""
        _hard_lock.acquire()
        try:
            if isinstance(self.ctx,_C.c_void_p) and self.ctx.value is not None:
                _treeshr.TreeRestoreContext(self.ctx)
                _setActiveTree(self.ctx.value)
            else:
                _setActiveTree(0)
        finally:
            _hard_lock.release()

    def setCurrent(treename,shot):
        """Set current shot for specified treename
        @param treename: Name of tree
        @type treename: str
        @param shot: Shot number
        @type shot: int
        @rtype None
        """
        Tree.lock()
        try:
            status=_treeshr.TreeSetCurrentShotId(_ver.tobytes(treename), shot)
        finally:
            Tree.unlock()
        if not (status & 1):
            raise _Exceptions.statusToException(status)
    setCurrent=staticmethod(setCurrent)

    def setDefault(self,node):
        """Set current default TreeNode.
        @param node: Node to make current default. Relative node paths will use the current default when resolving node lookups.
        @type node: TreeNode
        @return: Previous default node
        @rtype: TreeNode
        """
        old=self.default
        if isinstance(node,_treenode.TreeNode):
            if node.tree.ctx == self.ctx:
                _treeshr.TreeSetDefault(self.ctx,node.nid)
            else:
                raise TypeError('TreeNode must be in same tree')
        else:
            raise TypeError('default node must be a TreeNode')
        return old

    def setTimeContext(begin,end,delta):
        """Set time context for retrieving segmented records
        @param begin: Time value for beginning of segment.
        @type begin: Data
        @param end: Ending time value for segment of data
        @type end: Data
        @param delta: Delta time for sampling segment
        @type delta: Data
        @rtype: None
        """
        if isinstance(begin,(str,_scalar.String)):
          begin = _mdsshr.DateToQuad(begin).data()
        if isinstance(end,(str,_scalar.String)):
          end = _mdsshr.DateToQuad(begin).data()
        _treeshr.TreeSetTimeContext(begin,end,delta)
    setTimeContext=staticmethod(setTimeContext)

    def setVersionDate(date):
        """Set date for retrieving versions if versioning is enabled in tree.
        @param date: Reference date for data retrieval. Must be specified in the format: 'mmm-dd-yyyy hh:mm:ss' or 'now','today'
        or 'yesterday'.
        @type date: str
        @rtype: None
        """
        _treeshr.TreeSetVersionDate(date)
    setVersionDate=staticmethod(setVersionDate)

    def setVersionsInModel(self,flag):
        """Enable/Disable versions in model
        @param flag: True or False. True enables versions
        @type flag: bool
        @rtype: None
        """
        self.versions_in_model=bool(flag)

    def setVersionsInPulse(self,flag):
        """Enable/Disable versions in pulse
        @param flag: True or False. True enabled versions
        @type flag: bool
        @rtype: None
        """
        self.versions_in_pulse=bool(flag)

    def unlock(cls):
        """Internal use only. Thread synchronization locking.
        """
        if not _treeshr.TreeUsingPrivateCtx():
            cls._lock.release()
    unlock=classmethod(unlock)

    def versionsInModelEnabled(self):
        """Check to see if versions in the model are enabled
        @return: True if versions in model is enabled
        @rtype: bool
        """
        return self.versions_in_model

    def versionsInPulseEnabled(self):
        """Check to see if versions in the pulse are enabled
        @return: True if versions in pulse is enabled
        @rtype: bool
        """
        return self.versions_in_pulse

    def write(self):
        """Write out edited tree.
        @rtype: None
        """
        Tree.lock()
        try:
            _treeshr.TreeWriteTree(self,self.tree,self.shot)
        finally:
            Tree.unlock()

    def cleanDatafile(self):
        """Clean up data file.
        @rtype: None
        """
        Tree.lock()
        try:
             _treeshr.TreeCleanDatafile(self.ctx, self.tree, self.shot)
        finally:
            Tree.unlock()

    def compressDatafile(self):
        """Compress data file.
        @rtype: None
        """
        Tree.lock()
        try:
            _treeshr.TreeCompressDatafile(self.ctx, self.tree, self.shot)
        finally:
            Tree.unlock()
