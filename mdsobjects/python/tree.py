from mdsdata import Data,makeData
from threading import RLock,Thread
class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock=RLock()
    _activeTree=None

    def __del__(self):
        """Delete Tree instance
        @rtype: None
        """
        try:
            try:
                self.lock()
            except:
                pass
            try:
                from _treeshr import RTreeCloseAll,TreeCloseAll,TreeFree
                if self.close:
                    if isinstance(self,CachedTree):
                        RtreeCloseAll(self.ctx)
                    else:
                        TreeCloseAll(self.ctx)
                    if hasattr(self,"ctx") and self.ctx is not None:
                        TreeFree(self.ctx)
                self.ctx=None
            except:
                pass
        finally:
            try:
                self.unlock()
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
        from _treeshr import TreeGetDbi
        if name.lower() == 'default':
            ans=self.getDefault()
        else:
            if name.lower() == 'shot':
                name='shotid'
            elif name.lower() == 'tree':
                name='name'
            try:
                ans = TreeGetDbi(self,name)
            except KeyError:
                try:
                    ans = self.__dict__[name]
                except:
                    raise AttributeError,'No such attribute: '+name
        return ans

    def __init__(self,*args):
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
        from _treeshr import TreeOpen,TreeOpenNew,TreeOpenReadOnly,TreeGetContext,TreeException
        try:
            Tree.lock()
            self.close=False
            if len(args)==2:
                self.ctx=TreeOpen(args[0],args[1])
                self.close=True
            elif len(args)==0:
                try:
                    self.ctx=TreeGetContext()
                except:
                    try:
                        self.ctx=Tree._activeTree.ctx
                    except:
                        raise TreeException,'tree not open'
            elif len(args)==3:
                if args[2].upper()=='NORMAL':
                    self.ctx=TreeOpen(args[0],args[1])
                elif args[2].upper()=='EDIT':
                    self.ctx=TreeOpen(args[0],args[1])
                    self.edit()
                elif args[2].upper()=='NEW':
                    self.ctx=TreeOpenNew(args[0],args[1])
                elif args[2].upper()=='READONLY':
                    self.ctx=TreeOpenReadOnly(args[0],args[1])
                else:
                    raise TreeException,'Invalid mode specificed, use "Normal","Edit","New" or "ReadOnly".'
                self.close=True
            else:
                raise TypeError,"Tree() takes 0,2 or 3 arguments (%d given)" % (len(args),)
        finally:
            Tree.unlock()
        Tree.setActiveTree(self)
        return

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
            raise AttributeError,'Read only attribute: '+name
        elif name == 'default':
            self.setDefault(value)
        else:
            try:
                from _treeshr import TreeSetDbi
                TreeSetDbi(self,name,value)
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
        from _treeshr import TreeAddConglom
        from treenode import TreeNode
        try:
            Tree.lock()
            nid=TreeAddConglom(self,str(nodename),str(model))
        finally:
            Tree.unlock()
        return TreeNode(nid,self)

    def addNode(self,nodename,usage='ANY'):
        """Add a node to the tree. Tree must be in edit mode.
        @param nodename: Absolute or relative path specification of new node. All ancestors of node must exist.
        @type nodename: str
        @param usage: Usage of node.
        @type usage: str
        @return: Node created.
        @rtype: TreeNode
        """
        from _treeshr import TreeAddNode
        from treenode import TreeNode
        try:
            Tree.lock()
            nid = TreeAddNode(self,str(nodename),str(usage))
        finally:
            Tree.unlock()
        return TreeNode(nid,self)

    def createPulse(self,shot):
        """Create pulse.
        @param shot: Shot number to create
        @type shot: int
        @rtype: None
        """
        from _treeshr import TreeException,TreeCreatePulseFile
        from _mdsshr import MdsGetMsg
        import ctypes as _C
        from numpy import array
        try:
            Tree.lock()
            try:
                subtrees=self.getNodeWild('***','subtree')
                included=subtrees.nid_number.compress(subtrees.include_in_pulse)
                included=included.toList()
                included.insert(0,0)
                included=numpy(included)
                status = TreeCreatePulseFile(self.ctx,shot,len(included),_C.c_void_p(included.ctypes.data))
            except:
                status = TreeCreatePulseFile(self.ctx,shot,0,_C.c_void_p(0))
        finally:
            Tree.unlock()
        if not (status & 1):
            raise TreeException,"Error creating pulse: %s" % (MdsGetMsg(status),)

    def deleteNode(self,wild):
        """Delete nodes (and all their descendants) from the tree. Note: If node is a member of a device,
        all nodes from that device are also deleted as well as any descendants that they might have.
        @param wild: Wildcard path speficier of nodes to delete from tree.
        @type wild: str
        @rtype: None
        """
        from _treeshr import TreeDeleteNodeInitialize,TreeDeleteNode,TreeDeleteNodeExecute
        try:
            Tree.lock()
            TreeDeleteNodeInitialize(self)
            nodes=self.getNodeWild(wild)
            for node in nodes:
                TreeDeleteNode(self,int(node.nid_number))
            TreeDeleteNodeExecute(self)
        finally:
            Tree.unlock()

    def deletePulse(self,shot):
        """Delete pulse.
        @param shot: Shot number to delete
        @type shot: int
        @rtype: None
        """
        from _treeshr import TreeDeletePulse
        try:
            Tree.lock()
            TreeDeletePulse(self,shot)
        finally:
            Tree.unlock()

    def doMethod(nid,method):
        """For internal use only. Support for PyDoMethod.fun used for python device support"""
        from treenode import TreeNode
        n=TreeNode(nid,Tree())
        top=n.conglomerate_nids[0]
        c=top.record
        q=c.qualifiers
        model=c.model
        for i in range(len(q)):
            exec str(q[0])
        try:
            exec str('makeData('+model+'(n).'+method+'(Data.getTdiVar("__do_method_arg__"))).setTdiVar("_result")')
            makeData(1).setTdiVar("_method_status")
            
        except AttributeError,e:
            makeData(0xfd180b0).setTdiVar("_method_status")
        except Exception,e:
            print "Error doing %s on node %s" % (str(method),str(n))
            makeData(0).setTdiVar("_method_status")
            raise
        return Data.getTdiVar("_result")
    doMethod=staticmethod(doMethod)

    def edit(self):
        """Open tree for editing.
        @rtype: None"""
        from _treeshr import TreeOpenEdit
        try:
            Tree.lock()
            TreeOpenEdit(self)
        finally:
            Tree.unlock()

    def findTags(self,wild):
        """Find tags matching wildcard expression
        @param wild: wildcard string to match tagnames.
        @type wild: str
        @return: Array of tag names matching wildcard expression
        @rtype: ndarray
        """
        from _treeshr import TreeFindTagWild
        try:
            Tree.lock()
            tags=TreeFindTagWild(self,wild)
        finally:
            Tree.unlock()
        return tags

    def getActiveTree(cls):
        """Get active tree.
        @return: Current active tree
        @rtype: Tree
        """
        return cls._activeTree
    getActiveTree=classmethod(getActiveTree)
    
    def getCurrent(treename):
        """Return current shot for specificed treename
        @param treename: Name of tree
        @type treename: str
        @return: Current shot number for the specified tree
        @rtype: int
        """
        from _treeshr import TreeGetCurrentShotId,TreeException
        try:
            Tree.lock()
            shot=TreeGetCurrentShotId(treename)
        finally:
            Tree.unlock()
        if shot==0:
            raise TreeException,"Error obtaining current shot of %s" % (treename,)
        return shot
    getCurrent=staticmethod(getCurrent)

    def getDefault(self):
        """Return current default TreeNode
        @return: Current default node
        @rtype: TreeNode
        """
        from _treeshr import TreeGetDefault
        from treenode import TreeNode
        try:
            Tree.lock()
            ans = TreeNode(TreeGetDefault(self.ctx),self)
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
        from treenode import TreeNode
        from _treeshr import TreeFindNode
        if isinstance(name,int):
            return TreeNode(name,self)
        else:
            try:
                Tree.lock()
                ans=TreeNode(TreeFindNode(self.ctx,str(name)),self)
            finally:
                Tree.unlock()
            return ans
        
    def getNodeWild(self,name,*usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: TreeNodeArray of nodes matching the wildcard path specification and usage types.
        @rtype: TreeNodeArray
        """
        from treenode import TreeNodeArray
        try:
            Tree.lock()
            self.restoreContext()
            if len(usage) > 0:
                from numpy import array
                for i in range(len(usage)):
                    if not isinstance(usage[i],str):
                        raise TypeError,'Usage arguments must be strings'
                    usage=array(usage)
                    nids=Data.compile('getnci($,"NID_NUMBER",$)',name,usage).evaluate()
            else:
                nids=Data.compile('getnci($,"NID_NUMBER")',(name,)).evaluate()
        finally:
            Tree.unlock()
        return TreeNodeArray(nids,self)

    def getVersionDate():
        """Get date used for retrieving versions
        @return: Reference date for retrieving data is versions enabled
        @rtype: str
        """
        from _treeshr import TreeGetVersionDate
        return TreeGetVersionDate()
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
        cls._lock.acquire()
    lock=classmethod(lock)

    def quit(self):
        """Close edit session discarding node structure and tag changes.
        @rtype: None
        """
        from _treeshr import TreeQuitTree
        try:
            Tree.lock()
            TreeQuitTree(self)
        finally:
            Tree.unlock()

    def removeTag(self,tag):
        """Remove a tagname from the tree
        @param tag: Tagname to remove.
        @type tag: str
        @rtype: None
        """
        from _treeshr import TreeRemoveTag
        TreeRemoveTag(self,tag)

    def restoreContext(self):
        """Internal use only. Use internal context associated with this tree."""
        from _treeshr import TreeRestoreContext
        Tree._activeTree=self
        return TreeRestoreContext(self.ctx)

    def setActiveTree(cls,tree):
        """Set active tree. Use supplied tree context when performing tree operations in tdi expressions.
        @param tree: Tree to use as active tree
        @type tree: Tree
        @return: Previous active tree
        @rtype: Tree
        """
        old=cls._activeTree
        cls._activeTree=tree
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
        from _treeshr import TreeSetCurrentShotId,TreeException
        from _mdsshr import MdsGetMsg
        try:
            Tree.lock()
            status=TreeSetCurrentShotId(treename,shot)
        finally:
            Tree.unlock()
        if not (status & 1):
            raise TreeException,'Error setting current shot of %s: %s' % (treename,MdsGetMsg(status))
    setCurrent=staticmethod(setCurrent)

    def setDefault(self,node):
        """Set current default TreeNode.
        @param node: Node to make current default. Relative node paths will use the current default when resolving node lookups.
        @type node: TreeNode
        @return: Previous default node
        @rtype: TreeNode
        """
        from _treeshr import TreeSetDefault
        from treenode import TreeNode
        old=self.default
        if isinstance(node,TreeNode):
            if node.tree is self:
                TreeSetDefault(self.ctx,node.nid)
            else:
                raise TypeError,'TreeNode must be in same tree'
        else:
            raise TypeError,'default node must be a TreeNode'
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
        from _treeshr import TreeSetTimeContext
        TreeSetTimeContext(begin,end,delta)
    setTimeContext=staticmethod(setTimeContext)

    def setVersionDate(date):
        """Set date for retrieving versions if versioning is enabled in tree.
        @param date: Reference date for data retrieval. Must be specified in the format: 'mmm-dd-yyyy hh:mm:ss' or 'now','today'
        or 'yesterday'.
        @type date: str
        @rtype: None
        """
        from _treeshr import TreeSetVersionDate
        TreeSetVersionDate(date)
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
        from _treeshr import TreeWriteTree
        try:
            name=self.tree
            shot=self.shot
            Tree.lock()
            TreeWriteTree(self,name,shot)
        finally:
            Tree.unlock()

class CachedTree(Tree):
    """MDSplus tree with cached data updating"""

    initialized=False


    def __init__(self,tree,shot):
        """Initialize CachedTree instance.
        @param tree: Tree name
        @type tree: str
        @param shot: Shot number
        @type shot: int
        """
        from _treeshr import RTreeOpen
        try:
            Tree.lock()
            self.ctx=RTreeOpen(tree,shot)
            self.close=True
            Tree._activeTree=self
        finally:
            Tree.unlock()
        return

    def getCachedNode(self,name):
        """Locate node in tree. Returns CachedTreeNode if found. Use double backslashes in node names.
        @param name: Relative or absolute tree path
        @type name: str
        @return: node found
        @rtype: CachedTreeNode
        """
        from treenode import CachedTreeNode
        node=self.getNode(name)
        return CachedTreeNode(node.nid,self)

    def initialize(cachesize,shared):
        """ Configure the cachesize and shared settings. Must be called
        before creating any instance of CachedTree.
        @param cachesize: Size of cache
        @type cachesize: int
        @param shared: Flag indicating whether cache is shared or not.
        @type shared: bool
        @rtype: None
        """
        if CachedTree.initialized is False:
            from _treeshr import RTreeConfigure
            if shared is True:
                is_shared=1
            else:
                is_shared=0
            RTreeConfigure(is_shared,cachesize)
            CachedTree.initialized=True
    initialize=staticmethod(initialize)

    def synch():
        """Wait for all pending write operations in WRITE_THOUGH policy
        @rtype: None
        """
        from _treeshr import RTreeSynch
        RTreeSynch()
    synch=staticmethod(synch)
    
