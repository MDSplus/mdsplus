def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import threading as _threading
import ctypes as _C
import numpy as _N

_ver=_mimport('version')
_Exceptions=_mimport('mdsExceptions')
_mdsshr=_mimport('_mdsshr')
_treeshr=_mimport('_treeshr')
_data=_mimport('mdsdata')
_scalar=_mimport('mdsscalar')
_treenode=_mimport('treenode')
_tdishr=_mimport('_tdishr')

class _ThreadData(_threading.local):
    def __init__(self):
        self.private=False
_thread_data=_ThreadData()

_hard_lock=_threading.Lock()

class _TreeCtx(object): # HINT: _TreeCtx begin
    """ The TreeCtx class is used to manage proper garbage collection
    of open trees. It retains reference counts of tree contexts and
    closes and frees tree contexts when no longer being used. """
    lock = _threading.Lock()
    ctxs={}
    order=[]
    def __new__(cls,ctx,opened):
        if not ctx or (opened is None and ctx in _TreeCtx.ctxs):
            return None
        return super(_TreeCtx,cls).__new__(cls)
    def __init__(self,ctx,opened=True):
        self.ctx = ctx
        self.register(opened)
        self.open = True
    @staticmethod
    def canClose(ctx):
        return ctx not in _TreeCtx.ctxs or _TreeCtx.ctxs[ctx] == 1
    def register(self,opened):
        self.lock.acquire()
        try:
            if self.ctx in _TreeCtx.ctxs:
                _TreeCtx.ctxs[self.ctx]+=1
            else:
                _TreeCtx.ctxs[self.ctx] = 1 if opened else 2
            # generate ordered set
            _TreeCtx.order = [self.ctx]+[c for c in _TreeCtx.order if c!=self.ctx]
        finally:
            self.lock.release()
    def __del__(self):
        if not self.open: return
        self.open = False
        self.lock.acquire()
        try:
            _TreeCtx.ctxs[self.ctx]-=1
            if _TreeCtx.ctxs[self.ctx]==0:
                self._closeDbid()
        finally:
            self.lock.release()
    def _closeDbid(self):
        del(_TreeCtx.ctxs[self.ctx])
        _TreeCtx.order = [c for c in _TreeCtx.order if c!=self.ctx]
        # make sure current Dbid is not active - tdishr
        ctx = _treeshr.switchDbid()
        if ctx != 0 and ctx!=self.ctx:
            _treeshr.switchDbid(ctx)
        elif len(_TreeCtx.order):
            _treeshr.switchDbid(_TreeCtx.order[0])
        # apparently this was opened by python - so close all trees
        while True:
           try:
               _treeshr.TreeClose(_C.c_void_p(self.ctx),None,0)
               print("An unexpectedly open tree has been closed!!")
           except: break
        # now free current Dbid
        _treeshr._TreeFreeDbid(_C.c_void_p(self.ctx))

    local = _threading.local()
    @staticmethod
    def gettctx():
        return getattr(_TreeCtx.local,'tctx',None)

    @staticmethod
    def setUpCtx(ctx):
        """The dcl interface has its own TreeCtx in case no tree is open
        This method will set up the context of the current tree if any of
        default to its own context. One may also provide a context ctx,
        as done by _tree.Tree.tcl()
        """
        if ctx:
            _TreeCtx.local.tctx = _TreeCtx(_treeshr.switchDbid(ctx),None)
            return None
        dbid = _treeshr.switchDbid()
        if dbid:
            _treeshr.switchDbid(dbid)
            return None
        return True

    @staticmethod
    def restoreCtx(ctx,opened):
        if not ctx:
            dbid =_treeshr.switchDbid()
            if dbid: _treeshr.switchDbid(dbid)
            _TreeCtx.local.tctx = _TreeCtx(dbid,opened)

class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock=_threading.RLock()
    _id=0

    def _checkCtx(self,*args):
        if len(args) == 0:
            return 0 if self.ctx is None else self.ctx.value
        oldctx = args[0]
        newctx = self._checkCtx()
        if oldctx == newctx: return
        if newctx == 0:
            import gc
            del(self.tctx)
            gc.collect()
        else: self.tctx = _TreeCtx(newctx)
    # support for the with-structure
    def __enter__(self):
    	return self
    def __del__(self):
        if self.ctx and _TreeCtx.canClose(self.ctx.value):
            self.__exit__()
    def __exit__(self, *args):
        """ Cleanup for with statement. If tree is open for edit close it. """
        try:
            if self.open_for_edit:
                 self.quit()
            else:
                 self.close()
        except _Exceptions.TreeNOT_OPEN:
            pass

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
        if name.startswith('_'):
            namesplit = name.split('__',1)
            if len(namesplit)==2 and namesplit[1]==namesplit[1].upper():
                return self.getNode('\\%s::%s'%tuple(namesplit[1].split('__',1)+['TOP'])[:2])
            if name.upper() == name:
                return self.getNode('\\%s'%name[1:])
        elif name.upper() == name:
            return self.getNode(name)
        if name.lower() == 'default':
            return self.getDefault()
        elif name.lower() == 'top':
            return _treenode.TreeNode(0,self)
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
        self.ctx = None
        try:
            if tree is None:
                ctx = _treeshr.switchDbid()
                if not ctx:
                    ctx = _TreeCtx.gettctx()
                    if not ctx:
                        raise _Exceptions.TreeNOT_OPEN
                    ctx = ctx.ctx
                self.ctx=_C.c_void_p(ctx)
                opened = False
            else:
                if mode.upper() == 'NORMAL':
                    self.ctx=_treeshr.TreeOpen(tree,shot)
                elif mode.upper() == 'EDIT':
                    self.ctx=_treeshr.TreeOpenEdit(tree,shot)
                elif mode.upper() == 'NEW':
                    self.ctx=_treeshr.TreeOpenNew(tree,shot)
                elif mode.upper() == 'READONLY':
                    self.ctx=_treeshr.TreeOpenReadOnly(tree,shot)
                else:
                    raise AttributeError('Invalid mode specificed, use "Normal","Edit","New" or "ReadOnly".')
                opened = True
            if not isinstance(self.ctx,_C.c_void_p) or self.ctx.value is None:
                raise _Exceptions.MDSplusERROR
            self.tctx = _TreeCtx(self.ctx.value,opened)
            self.tree = self.name
            self.shot = self.shotid
            _TreeCtx.local.tctx = _TreeCtx(_treeshr.switchDbid(self.ctx.value),None)
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
        if name.lower() in ('modified','name','open_for_edit','open_readonly','shotid'):
            raise AttributeError('Read only attribute: '+name)
        elif name == 'default':
            self.setDefault(value)
        else:
            try:
                _treeshr.TreeSetDbi(self,name,value)
            except KeyError:
                self.__dict__[name]=value

    def __repr__(self):
        """Return representation
        @return: String representation of open tree
        @rtype: str
        """
        try:
            if self.open_for_edit:
                mode="Edit"
            elif self.open_readonly:
                mode="Readonly"
            else:
                mode="Normal"
        except _Exceptions.TreeNOT_OPEN:
            mode = "Closed"
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

    def dir(self):
        """list descendants of top"""
        _treenode.TreeNode(0,self).dir()


    def edit(self):
        """Open tree for editing.
        @rtype: None"""
        Tree.lock()
        try:
            _treeshr.TreeOpenEdit(self.tree,self.shot,self.ctx)
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
        return _treenode.TreeNodeArray([n for n in _treeshr.TreeFindNodeWild(self.ctx, name, *usage)],self)

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
            oldctx = self._checkCtx()
            try:
                _treeshr.TreeQuitTree(self)
            finally:
                Tree.unlock()
                self._checkCtx(oldctx)
        else: self.close()

    def close(self):
        """Close tree.
        @rtype: None
        """
        oldctx = self._checkCtx()
        try:
            _treeshr.TreeClose(self.ctx,self.tree,self.shot)
        finally:
            self._checkCtx(oldctx)

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

    def tdiCompile(self,*args,**kwargs):
        """Compile a TDI expression. Format: tdiCompile('expression-string',(arg1,...))"""
        kwargs['ctx'] = self.ctx
        return _tdishr.TdiCompile(*args,**kwargs)

    def tdiExecute(self,*args,**kwargs):
        """Compile and execute a TDI expression. Format: tdiExecute('expression-string',(arg1,...))"""
        kwargs['ctx'] = self.ctx
        return _tdishr.TdiExecute(*args,**kwargs)

    def tdiDecompile(self,*args,**kwargs):
        """Decompile a TDI expression. Format: tdiDecompile(tdi_expression)"""
        kwargs['ctx'] = self.ctx
        return _tdishr.TdiDecompile(*args,**kwargs)

    def tdiEvaluate(self,*args,**kwargs):
        """Evaluate and functions. Format: tdiEvaluate(data)"""
        kwargs['ctx'] = self.ctx
        return _tdishr.TdiEvaluate(*args,**kwargs)

    def tdiData(self,*args,**kwargs):
        """Return primitive data type. Format: tdiData(value)"""
        kwargs['ctx'] = self.ctx
        return _tdishr.TdiData(*args,**kwargs)
