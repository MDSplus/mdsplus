import traceback as _tb
if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_data=_mimport('mdsdata',1)
from threading import RLock,Thread,local
thread_data=local()

class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock=RLock()
    _activeTree=None
    def __del__(self):
        """Delete Tree instance
        @rtype: None
        """
        try:
            if self.close:
                status=_mimport('_treeshr',1).TreeCloseAll(self.ctx)
                if (status & 1):
                    TreeFreeDbid(ctx)
                if Tree.getActiveTree() == self:
                    Tree.setActiveTree(None)
        except:
            pass
        return

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
        if name.lower() == 'default':
            ans=self.getDefault()
        else:
            if name.lower() == 'shot':
                name='shotid'
            elif name.lower() == 'tree':
                name='name'
            try:
                ans = _mimport('_treeshr',1).TreeGetDbi(self,name)
            except KeyError:
                try:
                    ans = self.__dict__[name]
                except:
                    raise AttributeError('No such attribute: '+name)
        return ans

    def usePrivateCtx(cls,on=True):
        global thread_data
        if not hasattr(thread_data,"activeTree"):
            thread_data.activeTree=None
        thread_data.private=on
        _mimport('_treeshr',1).TreeUsePrivateCtx(on)
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
        _treeshr=_mimport('_treeshr',1)
        if tree is None:
            self.close=False
            try:
                self.ctx=_treeshr.TreeGetContext()
            except:
                try:
                    self.ctx=Tree.getActiveTree().ctx
                except:
                    raise _treeshr.TreeException('tree not open')
        else:
            self.close=True
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
                raise _treeshr.TreeException('Invalid mode specificed, use "Normal","Edit","New" or "ReadOnly".')
        Tree.setActiveTree(self)
        return

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
                _mimport('_treeshr',1).TreeSetDbi(self,name,value)
            except KeyError:
                self.__dict__[name]=value

    def __str__(self):
        """Return string representation
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

    def addDevice(self,nodename,model):
        """Add a device to the tree of the specified device model type.
        @param nodename: Absolute or relative path specification of the head node of the device. All ancestors of node must exist.
        @type nodename: str
        @param model: Model name of the device being added.
        @type model: str
        @return: Head node of device added
        @rtype: TreeNode
        """
        try:
            Tree.lock()
            nid=_mimport('_treeshr',1).TreeAddConglom(self,str(nodename),str(model))
        finally:
            Tree.unlock()
        return _mimport('treenode',1).TreeNode(nid,self)

    def addNode(self,nodename,usage='ANY'):
        """Add a node to the tree. Tree must be in edit mode.
        @param nodename: Absolute or relative path specification of new node. All ancestors of node must exist.
        @type nodename: str
        @param usage: Usage of node.
        @type usage: str
        @return: Node created.
        @rtype: TreeNode
        """
        try:
            Tree.lock()
            nid = _mimport('_treeshr',1).TreeAddNode(self,str(nodename),str(usage))
        finally:
            Tree.unlock()
        return _mimport('treenode',1).TreeNode(nid,self)

    def createPulse(self,shot):
        """Create pulse.
        @param shot: Shot number to create
        @type shot: int
        @rtype: None
        """
        _treeshr=_mimport('_treeshr',1)
        import ctypes as _C
        from numpy import array
        try:
            Tree.lock()
            try:
                subtrees=self.getNodeWild('***','subtree')
                included=subtrees.nid_number.compress(subtrees.include_in_pulse)
                included=included.toList()
                included.insert(0,0)
                included=array(included)
                status = _treeshr.TreeCreatePulseFile(self.ctx,shot,len(included),_C.c_void_p(included.ctypes.data))
            except:
                status = _treeshr.TreeCreatePulseFile(self.ctx,shot,0,_C.c_void_p(0))
        finally:
            pass
            Tree.unlock()
        if not (status & 1):
            raise _treeshr.TreeException("Error creating pulse: %s" % (_mimport('_mdsshr',1).MdsGetMsg(status),))

    def deleteNode(self,wild):
        """Delete nodes (and all their descendants) from the tree. Note: If node is a member of a device,
        all nodes from that device are also deleted as well as any descendants that they might have.
        @param wild: Wildcard path speficier of nodes to delete from tree.
        @type wild: str
        @rtype: None
        """
        _treeshr=_mimport('_treeshr',1)
        try:
            Tree.lock()
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
        try:
            Tree.lock()
            _mimport('_treeshr',1).TreeDeletePulse(self,shot)
        finally:
            Tree.unlock()

    def doMethod(self,nid,method):
        """For internal use only. Support for PyDoMethod.fun used for python device support"""
        n=_mimport('treenode',1).TreeNode(nid,self)
        top=n.conglomerate_nids[0]
        c=top.record
        q=c.qualifiers
        model=c.model

        for i in range(len(q)):
            exec( str(q[0]))
        try:
            exec( str('status=_data.makeData('+model+'(n).'+method+'(_data.Data.getTdiVar("__do_method_arg__")))'))
            status.setTdiVar('_result')
            if isinstance(status,_mimport('mdsscalar',1).Int32):        
                status.setTdiVar("_method_status")
            else:
                _data.makeData(1).setTdiVar("_method_status")            
        except AttributeError:
            _data.makeData(0xfd180b0).setTdiVar("_method_status")
        except Exception:
            print("Error doing %s on node %s" % (str(method),str(n)))
            _tb.print_exc()
            _data.makeData(0).setTdiVar("_method_status")
            raise
        return _data.Data.getTdiVar("_result")

    def edit(self):
        """Open tree for editing.
        @rtype: None"""
        try:
            Tree.lock()
            _mimport('_treeshr',1).TreeOpenEdit(self)
        finally:
            Tree.unlock()

    def findTagsIter(self, wild):
        """An iterator for the tagnames from a tree given a wildcard specification.
        @param wild: wildcard spec.
        @type name: str
        @return: iterator of tagnames (strings) that match the wildcard specification
        @rtype: iterator
        """

        for n in _mimport('_treeshr',1).TreeFindTagWild(self.ctx, wild):
            yield n

    def findTags(self,wild):
        """Find tags matching wildcard expression
        @param wild: wildcard string to match tagnames.
        @type wild: str
        @return: Array of tag names matching wildcard expression
        @rtype: ndarray
        """
        return tuple(self.findTagsIter(wild))

    def getActiveTree():
        """Get active tree.
        @return: Current active tree
        @rtype: Tree
        """
        global thread_data
        if not hasattr(thread_data,"activeTree"):
            thread_data.activeTree=None
            thread_data.private=False
        if thread_data.private:
          return thread_data.activeTree
        else:
          return Tree._activeTree
    getActiveTree=staticmethod(getActiveTree)
    
    def getCurrent(treename):
        """Return current shot for specificed treename
        @param treename: Name of tree
        @type treename: str
        @return: Current shot number for the specified tree
        @rtype: int
        """
        try:
            Tree.lock()
            shot=_mimport('_treeshr',1).TreeGetCurrentShotId(str.encode(treename))
        finally:
            Tree.unlock()
        if shot==0:
            raise _mimport('_treeshr',1).TreeException("Error obtaining current shot of %s" % (treename,))
        return shot
    getCurrent=staticmethod(getCurrent)

    def getDefault(self):
        """Return current default TreeNode
        @return: Current default node
        @rtype: TreeNode
        """
        try:
            Tree.lock()
            ans = _mimport('treenode',1).TreeNode(_mimport('_treeshr',1).TreeGetDefault(self.ctx),self)
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
        if isinstance(name,int):
            return _mimport('treenode',1).TreeNode(name,self)
        else:
            try:
                Tree.lock()
                ans=_mimport('treenode',1).TreeNode(_mimport('_treeshr',1).TreeFindNode(self.ctx,str(name)),self)
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
        for n in _mimport('_treeshr',1).TreeFindNodeWild(self.ctx, name, *usage):
            yield _mimport('treenode',1).TreeNode(n,self)

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
        return _mimport('treenode',1).TreeNodeArray(nids,self)

    def getVersionDate():
        """Get date used for retrieving versions
        @return: Reference date for retrieving data is versions enabled
        @rtype: str
        """
        return _mimport('_treeshr',1).TreeGetVersionDate()
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
        if not _mimport('_treeshr',1).TreeUsingPrivateCtx():
            cls._lock.acquire()
    lock=classmethod(lock)

    def quit(self):
        """Close edit session discarding node structure and tag changes.
        @rtype: None
        """
        try:
            Tree.lock()
            _mimport('_treeshr',1).TreeQuitTree(self)
        finally:
            Tree.unlock()

    def removeTag(self,tag):
        """Remove a tagname from the tree
        @param tag: Tagname to remove.
        @type tag: str
        @rtype: None
        """
        _mimport('_treeshr',1).TreeRemoveTag(self,tag)


    def _setActiveTree(tree):
        global thread_data
        if not hasattr(thread_data,"activeTree"):
            thread_data.activeTree=None
            thread_data.private=False
        if thread_data.private:
          old = thread_data.activeTree
          thread_data.activeTree=tree
        else:
          old=Tree._activeTree
          Tree._activeTree=tree
        return old
    _setActiveTree=staticmethod(_setActiveTree)

    def restoreContext(self):
        """Internal use only. Use internal context associated with this tree."""
        Tree._setActiveTree(self)
        return _mimport('_treeshr',1).TreeRestoreContext(self.ctx)

    def setActiveTree(cls,tree):
        """Set active tree. Use supplied tree context when performing tree operations in tdi expressions.
        @param tree: Tree to use as active tree
        @type tree: Tree
        @return: Previous active tree
        @rtype: Tree
        """
        old = cls._setActiveTree(tree)
        if tree is not None:
          tree.restoreContext()
        return old
    setActiveTree=classmethod(setActiveTree)

    def setCurrent(treename,shot):
        """Set current shot for specified treename
        @param treename: Name of tree
        @type treename: str
        @param shot: Shot number
        @type shot: int
        @rtype None
        """
        try:
            Tree.lock()
            status=_mimport('_treeshr',1).TreeSetCurrentShotId(str.encode(treename),shot)
        finally:
            Tree.unlock()
        if not (status & 1):
            raise _mimport('_treeshr',1).TreeException('Error setting current shot of %s: %s' % (treename,_mimport('_mdsshr',1).MdsGetMsg(status)))
    setCurrent=staticmethod(setCurrent)

    def setDefault(self,node):
        """Set current default TreeNode.
        @param node: Node to make current default. Relative node paths will use the current default when resolving node lookups.
        @type node: TreeNode
        @return: Previous default node
        @rtype: TreeNode
        """
        old=self.default
        if isinstance(node,_mimport('treenode',1).TreeNode):
            if node.tree is self:
                _mimport('_treeshr',1).TreeSetDefault(self.ctx,node.nid)
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
        _mimport('_treeshr',1).TreeSetTimeContext(begin,end,delta)
    setTimeContext=staticmethod(setTimeContext)

    def setVersionDate(date):
        """Set date for retrieving versions if versioning is enabled in tree.
        @param date: Reference date for data retrieval. Must be specified in the format: 'mmm-dd-yyyy hh:mm:ss' or 'now','today'
        or 'yesterday'.
        @type date: str
        @rtype: None
        """
        _mimport('_treeshr',1).TreeSetVersionDate(date)
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
        if not _mimport('_treeshr',1).TreeUsingPrivateCtx():
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
        try:
            name=self.tree
            shot=self.shot
            Tree.lock()
            _mimport('_treeshr',1).TreeWriteTree(self,name,shot)
        finally:
            Tree.unlock()

