def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C
import numpy as _N
import threading as _threading
from os import getenv as _getenv

#### Load other python modules referenced ###
#
_data=_mimport('mdsdata')
_exceptions=_mimport('mdsExceptions')
_array=_mimport('mdsarray')
_mdsdcl=_mimport('mdsdcl')
_ver=_mimport('version')
_compound=_mimport('compound')
_descriptor=_mimport('descriptor')
_mdsshr=_mimport('_mdsshr')
_compound=_mimport('compound')
_ident=_mimport('ident')

#
#############################################

#### Load Shared Libraries Referenced #######
#
_TreeShr=_ver.load_library('TreeShr')
_MdsShr=_ver.load_library('MdsShr')
#
#############################################

#### hidden module variables ################
#
class _ThreadData(_threading.local):

    """Contains thread specific information"""

    def __init__(self):
        self._activeTree=0
        self.private=False
_thread_data=_ThreadData()

_hard_lock=_threading.Lock() #### Thread lock

_activeTree={} # Holds the active global tree contexts

_usage_table={'ANY':0,'NONE':1,'STRUCTURE':1,'ACTION':2,      # Usage name to codenum table
              'DEVICE':3,'DISPATCH':4,'NUMERIC':5,'SIGNAL':6,
             'TASK':7,'TEXT':8,'WINDOW':9,'AXIS':10,
              'SUBTREE':11,'COMPOUND_DATA':12}

#
###################################################


class TreeNodeException(_exceptions.MDSplusException): pass


def _getThreadName(thread=None):

    """ Get a string to use to identify a thread. Used to index
    into thread specific data such as active tree contexts. """

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

    """ Set the active tree (possibly thread specific).
    The active tree is the last tree opened by a Tree
    instance creation or execution of tdi expressions.
    The active tree is loaded into the global TreeShr
    DBID context before executing tdi expressions. """

    _activeTree[_getThreadName(thread)]=ctx

def _getActiveTree(thread=None):

    """ Return the current active tree context for
    a particular thread. """

    threadName=_getThreadName(thread)
    if threadName in _activeTree:
        ctx = _activeTree[threadName]
    else:
        ctx = 0
    return _C.c_void_p(ctx)

class _TreeCtx(object): # HINT: _TreeCtx begin

    """ The TreeCtx class is used to manage proper garbage collection
    of open trees. It retains reference counts of tree contexts and
    closes and frees tree contexts when no longer being used. """
    ctxs=[]

    def __init__(self,ctx):
        self.ctx=ctx
        _TreeCtx.ctxs.append(ctx)

    def __del__(self):
        _TreeCtx.ctxs.remove(self.ctx)
        if self.ctx not in _TreeCtx.ctxs:
            status = None
            while True:
                stat=_TreeShr._TreeClose(_C.pointer(_C.c_void_p(self.ctx)),
                                         _C.c_void_p(0),
                                         _C.c_int32(0))
                if status is None:
                    status=stat
                if not stat & 1:
                    break
            if status & 1:
                _TreeShr.TreeFreeDbid(_C.c_void_p(self.ctx))

class _DBI_ITM_INT(_C.Structure): # HINT: _DBI_ITM_INT begin

    """ Ctype structure class for making calls into _TreeGetDbi() for integer values """

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

class _DBI_ITM_CHAR(_C.Structure): # HINT: _DBI_ITM_CHAR begin

    """ Ctype structure class for making calls into _TreeGetDbi() for string values """

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

class Tree(object): # HINT: Tree begin

    """Open an MDSplus Data Storage Hierarchy"""

    _lock=_threading.RLock()
    _id=0

    # support for the with-structure
    def __enter__(self):
        """ referenced if using "with Tree() ... " block"""
        return self

    def __exit__(self, type, value, traceback):
        """ Cleanup for with statement. If tree is open for edit close it. """
        if self.open_for_edit:
            self.quit()

########### Tree instance properties #######################
    @property
    def name(self):
        "Tree name"
        return self.getDbi("name")
    treename=name
    expt=name
    #tree=name

    @property
    def shotid(self):
        "Shot number of tree"
        return self.getDbi("shotid")
    #shot=shotid

    @property
    def modified(self):
        "True if open for edit and modifications made to tree structure."
        return self.getDbi("modified")

    @property
    def open_for_edit(self):
        "True if tree is opened for edit"
        return self.getDbi("open_for_edit")

    @property
    def number_opened(self):
        "Number of open trees on tree stack"
        return self.getDbi("number_opened")

    @property
    def max_open(self):
        "Max number of trees to keep open on stack (settable)"
        return self.getDbi("max_open")
    @max_open.setter
    def max_open(self,value):
        self.setDbi("max_open",value)

    @property
    def default(self):
        "current default node position in tree (settable)"
        return self.getDefault()
    @default.setter
    def default(self,treenode):
        self.setDefault(treenode)

    @property
    def open_readonly(self):
        "True of tree is open readonly"
        return self.getDbi("open_readonly")

    @property
    def versions_in_model(self):
        "Support versioning of data in model. (settable)"
        return self.getDbi("versions_in_model")
    @versions_in_model.setter
    def versions_in_model(self,value):
        self.setDbi("versions_in_model",value)

    @property
    def versions_in_pulse(self):
        "Support versioning of data in pulse. (settable)"
        return self.getDbi("versions_in_pulse")
    @versions_in_pulse.setter
    def versions_in_pulse(self,value):
        self.setDbi("versions_in_pulse",value)

    def getDbi(self,itemname):

        """
        Get tree information such as:

        treename/name/expt - name of the tree
        shotid/shot        - shot number
        default/pwd        - default node
        modified           - true if modified during edit
        open_for_edit      - true if tree is opened for edit
        open_readonly      - true if tree is opened readonly
        versions_in_model  - true if data versions is enabled in the model
        versions_in_pulse  - true if data versions is enabled in the pulse

        itemname can be a single string or a list/tuple of strings
        """
        if isinstance(itemname,(list,tuple)):
            ans={}
            for item in itemname:
                ans[item]=self.getDbi(item)
            return ans
        else:
            itemlist={'NAME':(1,str,12),'EXPT':(1,str,12),
                      'SHOTID':(2,int),'SHOT':(2,int),
                      'MODIFIED':(3,bool),
                      'OPEN_FOR_EDIT':(4,bool),
                      'DEFAULT':(8,str,256),
                      'OPEN_READONLY':(9,bool),
                      'VERSIONS_IN_MODEL':(10,bool),
                      'VERSIONS_IN_PULSE':(11,bool)}
            try:
                item=itemlist[itemname.upper()]
            except KeyError:
                raise KeyError('Item name must be one of %s' % list(itemlist.keys()))
            if item[1] is str:
                ans=_C.c_char_p(_ver.tobytes('x'.rjust(item[2])))
                retlen=_C.c_int32(0)
                itmlst=_DBI_ITM_CHAR(item[0],item[2],ans,retlen)
            else:
                ans=_C.c_int32(0)
                itmlst=_DBI_ITM_INT(item[0],ans)
            Tree.lock()
            try:
                status=_TreeShr._TreeGetDbi(self.ctx,
                                            _C.pointer(itmlst))
                if not (status & 1):
                    raise _exceptions.statusToException(status)
            finally:
                Tree.unlock()
            if item[1] is str:
                return _ver.tostr(ans.value)
            else:
                return item[1](ans.value)

    @property
    def top(self):  # compatibility
        "Tree root"
        return TreeNode(0,self)
    def __getattr__(self,name):

        """Support for referencing an immediate child or
        member of current default node of the tree by
        specifying an uppercase property. For example:

        t = Tree('mytree',shot)
        node = t.NODENAME

        If the tree has a top leve child or member with
        the name "NODENAME" t.NODENAME will return a
        TreeNode instance."""

        if name.upper() == name:
            try:
                return self.getNode(name)
            except _exceptions.TreeNNF:
                pass
        raise AttributeError('No such attribute: '+name)

    @classmethod
    def usePrivateCtx(cls,on=True):
        _thread_data.private=on
        if on:
            val=_C.c_int32(1)
        else:
            val=_C.c_int32(0)
        _TreeShr.TreeUsePrivateCtx(val)

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
                SwitchDbid=_TreeShr.TreeSwitchDbid
                SwitchDbid.restype=_C.c_void_p
                ctx=_C.c_void_p(SwitchDbid(_C.c_void_p(0)))
                if ctx is None:
                    ctx=_getActiveTree()
                else:
                    _TreeShr.TreeSwitchDbid(ctx)

                if ctx is None:
                    raise _exceptions.TreeNOT_OPEN()
                else:
                    self.ctx=ctx
            else:
                self.ctx=_C.c_void_p(0)
                mode=mode.upper()
                if mode == 'NORMAL':
                    status=_TreeShr._TreeOpen(_C.pointer(self.ctx),
                                              _C.c_char_p(_ver.tobytes(tree)),
                                              _C.c_int32(shot),
                                              _C.c_int32(0))
                elif mode == 'EDIT':
                    status=_TreeShr._TreeOpenEdit(_C.pointer(self.ctx),
                                              _C.c_char_p(_ver.tobytes(tree)),
                                              _C.c_int32(shot),
                                              _C.c_int32(0))
                elif mode == 'READONLY':
                    status=_TreeShr._TreeOpen(_C.pointer(self.ctx),
                                              _C.c_char_p(_ver.tobytes(tree)),
                                              _C.c_int32(shot),
                                              _C.c_int32(1))
                elif mode == 'NEW':
                    status=_TreeShr._TreeOpenNew(_C.pointer(self.ctx),
                                                 _C.c_char_p(_ver.tobytes(tree)),
                                                 _C.c_int32(shot))
                else:
                    raise TypeError('Invalid mode specificed, use "Normal","Edit","New" or "ReadOnly".')
                if not status & 1:
                    raise _exceptions.statusToException(status)
            if isinstance(self.ctx,_C.c_void_p) and self.ctx.value is not None:
                _setActiveTree(self.ctx.value)
                _TreeShr.TreeSwitchDbid(self.ctx)
                if tree is not None:
                    self.tctx=_TreeCtx(self.ctx.value)
            self.tree = self.name
            self.shot = self.shotid
        finally:
            _hard_lock.release()

    def __deepcopy__(self,memo):
        return self

    def __eq__(self,obj):
        if isinstance(obj,(Tree,)):
            return self.ctx.value == obj.ctx.value
        return False

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
            nid=_C.c_int32(0)
            status = _TreeShr._TreeAddConglom(self.ctx,
                                              _ver.tobytes(nodename),
                                              _ver.tobytes(model),
                                              _C.pointer(nid))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()
        return TreeNode(nid.value, self)

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
            nid=_C.c_int32(0)
            try:
                usagenum=_usage_table[usage.upper()]
                if usagenum==11:
                    status = _TreeShr._TreeAddNode(self.ctx,_ver.tobytes(nodename),
                                                   _C.pointer(nid),
                                                   _C.c_int32(1))
                    if status & 1:
                        status = _TreeShr._TreeSetSubtree(self.ctx,nid)
                else:
                    status = _TreeShr._TreeAddNode(self.ctx,
                                                   _ver.tobytes(nodename),
                                                   _C.pointer(nid),
                                                   _C.c_int32(usagenum))
                if not status & 1:
                    raise _exceptions.statusToException(status)
            except KeyError:
                raise KeyError('Invalid usage must be one of: %s' % list(_usage_table.keys()))
        finally:
            Tree.unlock()
        return TreeNode(nid.value,self)

    def createPulse(self,shot):
        """Create pulse.
        @param shot: Shot number to create
        @type shot: int
        @rtype: None
        """
        Tree.lock()
        try:
            status=_TreeShr._TreeCreatePulseFile(self.ctx,
                                                 _C.c_int32(shot),
                                                 _C.c_int32(0),
                                                 _C.c_void_p(0))
            if not (status & 1):
                raise _exceptions.statusToException(status)
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
                if first:
                    reset=_C.c_int32(1)
                    first=False
                else:
                    reset=_C.c_int32(0)
                status = _TreeShr._TreeDeleteNodeInitialize(self.ctx,
                                                            node._nid,
                                                            reset)
                if not status & 1:
                    raise _exceptions.statusToException(status)
            status = _TreeShr._TreeDeleteNodeExecute(self.ctx)
            if not status & 1:
                raise _exceptions.statusToException(status)
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
            status = _TreeShr._TreeDeletePulse(self.ctx,
                                               _C.c_int32(int(shot)),
                                               _C.c_int32(1))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def dir(self):
        """list descendants of top"""
        self.top.dir()

    def edit(self):
        """Open tree for editing.
        @rtype: None"""
        Tree.lock()
        try:
            status = _TreeShr._TreeOpenEdit(_C.pointer(self.ctx),
                                            _C.c_char_p(_ver.tobytes(self.tree)),
                                            _C.c_int32(int(self.shot)))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def findTagsIter(self, wild):
        """An iterator for the tagnames from a tree given a wildcard specification.
        @param wild: wildcard spec.
        @type name: str
        @return: iterator of tagnames (strings) that match the wildcard specification
        @rtype: iterator
        """
        nid=_C.c_int32(0)
        tagctx=_C.c_void_p(0)
        _TreeShr._TreeFindTagWild.restype=_C.c_char_p
        try:
            while True:
                tag_ptr = _TreeShr._TreeFindTagWild(self.ctx,
                                                    _C.c_char_p(_ver.tobytes(wild)),
                                                    _C.pointer(nid),
                                                    _C.pointer(tagctx))
                if tag_ptr is None:
                    break
                yield tag_ptr.rstrip()
        except GeneratorExit:
            pass
        _TreeShr.TreeFindTagEnd(_C.pointer(tagctx))

    def findTags(self,wild):
        """Find tags matching wildcard expression
        @param wild: wildcard string to match tagnames.
        @type wild: str
        @return: Array of tag names matching wildcard expression
        @rtype: ndarray
        """
        return tuple(self.findTagsIter(wild))

    @staticmethod
    def getCurrent(treename):
        """Return current shot for specificed treename
        @param treename: Name of tree
        @type treename: str
        @return: Current shot number for the specified tree
        @rtype: int
        """
        Tree.lock()
        try:
            shot=_TreeShr.TreeGetCurrentShotId(_ver.tobytes(treename))
        finally:
            Tree.unlock()
        if shot==0:
            raise _exceptions.TreeNOCURRENT()
        return shot

    def getDefault(self):
        """Return current default TreeNode
        @return: Current default node
        @rtype: TreeNode
        """
        Tree.lock()
        try:
            nid=_C.c_int32(0)
            status = _TreeShr._TreeGetDefaultNid(self.ctx,_C.pointer(nid))
            if status & 1:
                ans = TreeNode(nid.value,self)
            else:
                raise _exceptions.statusToException(status)
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
            ans = TreeNode(name,self)
        else:
            Tree.lock()
            try:
                n=_C.c_int32(0)
                status = _TreeShr._TreeFindNode(self.ctx,
                                                _ver.tobytes(str(name)),
                                                _C.pointer(n))
                if status & 1:
                    ans = TreeNode(int(n.value),self)
                else:
                    raise _exceptions.statusToException(status)
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
        if len(usage) == 0:
            usage_mask=0xFFFF
        else :
            try:
                usage_mask=0
                for u in usage:
                    usage_mask |= 1 << _usage_table[u.upper()]
            except KeyError:
                raise KeyError('Invalid usage must be one of: %s' % list(_usage_table.keys()))

        nid=_C.c_int32(0)
        ctx=_C.c_void_p(0)
        try:
            while _TreeShr._TreeFindNodeWild(self.ctx,
                                     _ver.tobytes(name),
                                     _C.pointer(nid),
                                     _C.pointer(ctx),
                                     _C.c_int32(usage_mask)) & 1 != 0:
                yield TreeNode(nid.value,self)
        except GeneratorExit:
            pass
        _TreeShr._TreeFindNodeEnd(self.ctx, _C.pointer(ctx))

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
        return TreeNodeArray(nids,self)

    @staticmethod
    def getVersionDate():
        """Get date used for retrieving versions
        @return: Reference date for retrieving data is versions enabled
        @rtype: str
        """
        dt=_C.c_ulonglong(0)
        status = _TreeShr._TreeGetViewDate(dt)
        if not status & 1:
            raise _exceptions.statusToException(status)
        return _scalar.Uint64(dt.value).date

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

    @classmethod
    def lock(cls):
        """Internal use only. Thread synchronization locking.
        """
        if not _TreeShr.TreeUsingPrivateCtx() == 1:
            cls._lock.acquire()

    def quit(self):
        """Close edit session discarding node structure and tag changes.
        @rtype: None
        """
        if self.open_for_edit:
            Tree.lock()
            try:
                status = _TreeShr._TreeQuitTree(_C.pointer(self.ctx),
                                                _C.c_char_p(_ver.tobytes(self.tree)),
                                                _C.c_int32(int(self.shot)))
                if not status & 1:
                    raise _exceptions.statusToException(status)
            finally:
                Tree.unlock()

    def removeTag(self,tag):
        """Remove a tagname from the tree
        @param tag: Tagname to remove.
        @type tag: str
        @rtype: None
        """
        Tree.lock()
        try:
            status = _TreeShr._TreeRemoveTag(self.ctx,
                                             _C.c_char_p(_ver.tobytes(tag)))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def restoreContext(self):
        """Internal use only. Use internal context associated with this tree."""
        _hard_lock.acquire()
        try:
            if isinstance(self.ctx,_C.c_void_p) and self.ctx.value is not None:
                _TreeShr.TreeSwitchDbid(self.ctx)
                _setActiveTree(self.ctx.value)
            else:
                _setActiveTree(0)
        finally:
            _hard_lock.release()

    @staticmethod
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
            status=_TreeShr.TreeSetCurrentShotId(_C.c_char_p(_ver.tobytes(treename)),
                                                 _C.c_int32(shot))
        finally:
            Tree.unlock()
        if not (status & 1):
            raise _exceptions.statusToException(status)

    def setDefault(self,node):
        """Set current default TreeNode.
        @param node: Node to make current default. Relative node paths will use the current default when resolving node lookups.
        @type node: TreeNode
        @return: Previous default node
        @rtype: TreeNode
        """
        old=self.default
        if isinstance(node,TreeNode):
            if node.tree.ctx == self.ctx:
                Tree.lock()
                try:
                    status = _TreeShr._TreeSetDefaultNid(self.ctx,
                                                      node._nid)
                    if not status & 1:
                        raise _exceptions.statusToException(status)
                finally:
                    Tree.unlock()
            else:
                raise TypeError('TreeNode must be in same tree')
        else:
            raise TypeError('default node must be a TreeNode')
        return old

    @staticmethod
    def setTimeContext(begin,end,delta):
        """Set time context for retrieving segmented records
        @param begin: Time value for beginning of segment.
        @type begin: str, Uint64, Float32 or Float64
        @param end: Ending time value for segment of data
        @type end: str, Uint64, Float32 or Float64
        @param delta: Delta time for sampling segment
        @type delta: Uint64, Float32 or Float64
        @rtype: None
        """
        begin = _mdsshr.DateToQuad(begin)
        end = _mdsshr.DateToQuad(end)
        delta = _data.Data(delta)
        status = _TreeShr.TreeSetTimeContext(_C.pointer(begin.descriptor),
                                             _C.pointer(end.descriptor),
                                             _C.pointer(delta.descriptor))
        if not status & 1:
            raise _exceptions.statusToException(status)

    @staticmethod
    def setVersionDate(date):
        """Set date for retrieving versions if versioning is enabled in tree.
        @param date: Reference date for data retrieval. Must be specified in the format: 'mmm-dd-yyyy hh:mm:ss' or 'now','today'
        or 'yesterday'.
        @type date: str
        @rtype: None
        """
        status = _TreeShr.TreeSetViewDate(_C.pointer(_C.c_int64(_mdsshr.DateToQuad(date).data())))
        if not status & 1:
            raise _exceptions._exceptions.statusToExceptions(status)

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

    @classmethod
    def unlock(cls):
        """Internal use only. Thread synchronization locking.
        """
        if not _TreeShr.TreeUsingPrivateCtx():
            cls._lock.release()

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
            status = _TreeShr._TreeWriteTree(_C.pointer(self.ctx),
                                             _C.c_char_p(_ver.tobytes(self.tree)),
                                             _C.c_int32(int(self.shot)))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def cleanDatafile(self):
        """Clean up data file.
        @rtype: None
        """
        Tree.lock()
        try:
             status = _TreeShr._TreeCleanDatafile(0,
                                                  _C.c_char_p(_ver.tobytes(self.tree)),
                                                  _C.c_int32(int(self.shot)))
             if not status & 1:
                 raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def compressDatafile(self):
        """Compress data file.
        @rtype: None
        """
        Tree.lock()
        try:
            status = _TreeShr._TreeCompressDatafile(0,
                                                    _C.c_char_p(_ver.tobytes(self.tree)),
                                                    _C.c_int32(int(self.shot)))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

class TreeNode(object): # HINT: TreeNode begin
    """Class to represent an MDSplus node reference (nid).
    @ivar nid: node index of this node.
    @type nid: int
    @ivar tree: Tree instance that this node belongs to.
    @type tree: Tree
    """
    dtype_id=192
    _units=None
    _error=None
    _help=None
    _validation=None

    def __new__(cls,nid,*tree):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param node: Node of device
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        node = super(TreeNode,cls).__new__(cls)
        if type(node) is TreeNode:
            TreeNode.__init__(node,nid,*tree)
            try:
                if node.usage == "DEVICE":
                    return node.record.getClass(node)
            except _exceptions.TreeNODATA:
                pass
            except _exceptions.DevNOT_A_PYDEVICE:
                pass
            except _exceptions.DevPYDEVICE_NOT_FOUND:
                pass
        return node

    def __init__(self,nid,*tree):
        """Initialze TreeNode
        @param n: Index of the node in the tree.
        @type n: int
        @param tree: Tree associated with this node
        @type tree: Tree
        """
        if nid is None:
            self._nid = None
        else:
            self._nid = _C.c_int32(int(nid))
        if len(tree)<1 or not isinstance(tree[0],(Tree,)):
            self.tree=Tree(*tree)
        else:
            self.tree=tree[0]

    def __eq__(self,obj):
        if isinstance(obj,(TreeNode,)):
            return self.tree == obj.tree and self.nid == obj.nid
        return False

    ###################################
    ### Data Properties
    ###################################

    @property
    def units(self):
        """units associated with this data."""
        return _compound.UNITS(self).evaluate()
    @units.setter
    def units(self,units):
        if units is None:
            if hasattr(self,'_units'):
                delattr(self,'_units')
        else:
            self._units=units
    def setUnits(self,units):
        self.units=units
        return self

    @property
    def error(self):
        """error property of this data."""
        return _compound.ERROR_OF(self).evaluate()
    @error.setter
    def error(self,error):
        if error is None:
            if hasattr(self,'_error'):
                delattr(self,'_error')
        else:
            self._error=error
    def setError(self,error):
        self.error=error
        return self

    @property
    def help(self):
        """help property of this node."""
        return _compound.HELP_OF(self).evaluate()
    @help.setter
    def help(self,help):
        if help is None:
            if hasattr(self,'_help'):
                delattr(self,'_help')
        else:
            self._help=help
    def setHelp(self,help):
        self.help=help
        return self

    @property
    def validation(self):
        """Validation property of this node"""
        return _compound.VALIDATION_OF(self).evaluate()
    @validation.setter
    def validation(self,validation):
        if validation is None:
            if hasattr(self,'_validation'):
                delattr(self,'_validation')
        else:
            self._validation=validation
    def setValidation(self,validation):
        self.validation=validation
        return self

    ###################################
    ### Node Properties
    ###################################

    def nciProp(name,doc):
        def get(self):
            return self.getNci(name,False)
        setattr(get,'__doc__',doc)
        return property(get)

    brother=nciProp("brother","brother node of this node")

    child=nciProp("child","child node of this node")

    children_nids=nciProp("children_nids","children nodes of this node")

    mclass=_class=nciProp("class","class of the data stored in this node")

    class_str=nciProp("class_str","class name of the data stored in this node")

    compressible=nciProp("compressible","is the data stored in this node compressible")

    compress_on_put=nciProp("compress_on_put",
                            "should data be compressed when stored in this node (settable)")
    @compress_on_put.setter
    def compress_on_put(self,value): self.setCompressOnPut(value)

    conglomerate_elt=nciProp("conglomerate_elt",
                             "what element of a conglomerate is this node")

    conglomerate_nids=nciProp("conglomerate_nids",
                              "what are the nodes of the conglomerate this node belongs to")

    data_in_nci=nciProp("data_in_nci",
                        "is the data of this node stored in its nci")

    depth=nciProp("depth","what is the depth of this node in the tree structure")

    @property
    def descendants(self):
        "Get all the immediate descendants of this node"
        return self.getDescendants()

    @property
    def descriptor(self):
      """Return a MDSplus descriptor"""
      d=_descriptor.Descriptor_s()
      d.length=4
      d.dtype=self.dtype_id
      d.pointer=_C.cast(_C.pointer(self._nid),_C.c_void_p)
      d.original=self
      d.tree=self.tree
      if self._units or self._error is not None or self._help is not None or self._validation is not None:
          return _compound.Compound.descriptorWithProps(self,d)
      else:
          return d



    @property
    def disabled(self):
      "is this node disabled (settable)"
      return self.isDisabled()
    @disabled.setter
    def disabled(self,value): self.setOn(not value)

    do_not_compress=nciProp("do_not_compress",
                            "is this node set to disable any compression of data stored in it. (settable)")
    @do_not_compress.setter
    def do_not_compress(self,value): self.setDoNotCompress(value)

    dtype=nciProp("dtype","the numeric value of the data type stored in this node")

    dtype_str=nciProp("dtype_str","the name of the data type stored in this node")

    error_on_put=nciProp("error_on_put","was there an error storing data for this node")

    essential=nciProp("essential","essential action defined in this node (settable)")
    @essential.setter
    def essential(self,value): self.setEssential(value)

    fullpath=nciProp("fullpath","full node path")

    get_flags=nciProp("get_flags","numeric flags mask for this node")

    include_in_pulse=nciProp("include_in_pulse","include subtree in pulse")
    @include_in_pulse.setter
    def include_in_pulse(self,value): self.setIncludeInPulse(value)

    @property
    def is_child(self):
        "is this a child node?"
        return self.isChild()

    @property
    def is_member(self):
        "is this a member node?"
        return self.isMember()
    length=nciProp("length","length of data stored in this node (uncompressed)")

    @property
    def local_path(self):
        "Return path relative to top of tree this node is part of."
        return self.getLocalPath()

    @property
    def local_tree(self):
        "Return name of the tree this node is part of."
        return self.getLocalTree()

    member=nciProp("member","first member immediate descendant of this node")

    member_nids=nciProp("member_nids","all member immediate descendants of this node")

    minpath=nciProp("minpath","minimum path string for this node based on current default node")

    @property
    def nid_number(self):
        "the node id of the node"
        if self._nid is None:
            return None
        else:
            return _scalar.Int32(self._nid.value)

    @property
    def nid(self):
        "the node id of the node"
        if self._nid is None:
            return None
        else:
            return self._nid.value
    @nid.setter
    def nid(self,value):
        if value is None:
            self._nid=None
        else:
            self._nid=_C.c_int32(int(value))

    nid_reference=nciProp("nid_reference","node data contains nid references")

    node_name=name=nciProp("node_name","node name")

    no_write_model=nciProp("no_write_model","is storing data in this node disabled if model tree (settable)")
    @no_write_model.setter
    def no_write_model(self,value): self.setNoWriteModel(value)

    no_write_shot=nciProp("no_write_shot",
                          "is storing data in this node disabled if not model tree (settable)")
    @no_write_shot.setter
    def no_write_shot(self,value): self.setNoWriteShot(value)
    number_of_children=nciProp("number_of_children","number of children")

    @property
    def number_of_descendants(self):
        "Number of immediate descendants of this node."
        return self.getNumDescendants()

    number_of_elts=nciProp("number_of_elts","number of nodes in a conglomerate")

    number_of_members=nciProp("number_of_members","number of members")

    @property
    def on(self):
        "Is this node turned on (settable)"
        return self.isOn()
    @on.setter
    def on(self,value): self.setOn(value)

    original_part_name=nciProp("original_part_name","original part name of this node")

    owner_id=nciProp("owner_id","id of the last person to write to this node")

    parent=nciProp("parent","parent node of this node")

    parent_state=nciProp("parent_state","is parent disabled")

    parent_relationship=nciProp("parent_relationship","parent relationship")

    @property
    def parent_disabled(self):
        return self.isParentDisabled()
    @parent_disabled.setter
    def parent_disabled(self,value): self.setParentOn(not value)

    @property
    def parent_on(self):
        return self.isParentOn()
    @parent_on.setter
    def parent_on(self,value):  self.setParentOn(value)

    path=nciProp("path","path to this node")

    path_reference=nciProp("path_reference","node data contains path references")

    @property
    def record(self):
        "Data contents of node (settable)"
        return self.getRecord()
    @record.setter
    def record(self,value):
        self.putData(value)

    rfa=nciProp("rfa","data offset in datafile")

    rlength=nciProp("rlength","length of data in node")

    segmented=nciProp("segmented","is data segmented")

    setup=nciProp("setup_information","was this data present in the model")

    setup_information=setup

    state=nciProp("state","Use on property instead. on/off state of this node. False=on,True=off. (settable)")
    @state.setter
    def state(self,value): self.on=not value

    status=nciProp("status","status of action execution")

    @property
    def subtree(self):
        "Is this node a subtree reference. (settable)"
        return self.usage == "SUBTREE"
    @subtree.setter
    def subtree(self,value): self.setSubtree(value)

    @property
    def tags(self):
        "All tags defined for this node. (settable - note existing tags replace by new tags"
        return self.getTags()
    @tags.setter
    def tags(self,names): self.addTags(names,replace=True)

    @property
    def tag(self):
        "tags defined for this node (settable to add a tag)."
        return self.tags
    @tag.setter
    def tag(self,names): self.addTags(names,replace=False)
    time_inserted=nciProp("time_inserted","64-bit timestamp when data was stored")

    @property
    def usage(self):
        "Usage of this node."
        return _scalar.String(str(self.usage_str)[10:])
    @usage.setter
    def usage(self,usage): self.setUsage(usage)

    usage_str=nciProp("usage_str","formal name of the usage of this node")

    versions=nciProp("version","does the data contain versions")

    write_once=nciProp("write_once","is no write once (settable)")
    @write_once.setter
    def write_once(self,value): self.setWriteOnce(value)

    compress_segments=nciProp("compress_segments","should segments be compressed (settable)")
    @compress_segments.setter
    def compress_segments(self,value): self.setCompressSegments(value)

    ########################################
    ### End of Node Properties
    ########################################

    def __hasBadTreeReferences__(self,tree):
       return self.tree != tree

    def __deepcopy__(self,dummy):
       return self

    def __fixTreeReferences__(self,tree):
        if (self.nid >> 24) != 0:
            return TreePath(str(self))
        else:
            relpath=str(self.fullpath)
            relpath=relpath[relpath.find('::TOP')+5:]
            path='\\%s::TOP%s' % (tree.tree,relpath)
            try:
                ans=tree.getNode(str(self))
            except _exceptions.TreeNNF:
                ans=TreePath(path,tree)
            return ans

    def __getattr__(self,name):
        """ Return a member or child node for
            node.abc if abc is uppercase and
            matches the name of an immediate
            ancester to the node.
        """
        if name=='tree':
            return Tree()
        if name.upper() == name:
            try:
                return self.getNode(name)
            except _exceptions.TreeNNF:
                pass
        try:
            return super(TreeNode,self).__getattr__(name)
        except AttributeError:
            pass
        if self.length>0:
            rec = self.record
            try:
                return rec.__getattribute__(name)
            except AttributeError:
                return rec.__getattr__(name)
        raise AttributeError('No such attribute: '+name)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
      """Convert TreeNode to string."""
      if self._nid is None:
          return "NODEREF(*)"
      return str(self.path)

    def addDevice(self,name,model):
        """Add device descendant.
        @param name: Node name of device. 1-12 characters, no path delimiters
        @type name: str
        @param model: Type of device to add
        @type model: str
        @return: Head node of device
        @rtype: TreeNode
        """
        if name.find(':') >=0 or name.find('.') >= 0:
            raise TreeNodeException("Invalid node name, do not include path delimiters in nodename")
        return self.tree.addDevice(self.fullpath+":"+name.upper(),model)

    def addNode(self,name,usage='ANY'):
        """Add node
        @param name: Node name of new node to add
        @type name: str
        @param usage: Usage of the new node. If omitted set to ANY.
        @type usage: str
        @return: New now added
        @rtype: TreeNode
        """
        try:
            usagenum=_usage_table[usage.upper()]
        except KeyError:
            raise KeyError('Invalid usage specified. Use one of %s' % (str(_usage_table.keys()),))
        name=str(name).upper()
        if name[0]==':' or name[0]=='.':
            name=str(self.fullpath)+name
        elif name[0] != "\\":
            if usagenum==1 or usagenum==-1:
                name=str(self.fullpath)+"."+name
            else:
                name=self.fullpath+":"+name
        return self.tree.addNode(name,usage)

    def addTags(self, names, replace=False):
        """Add tag or list of tags to a node.
        Use replace=True if any existing tags should be removed.
        Tag names must be either a str or a list or tuple
        containing str instances."""
        def checkstr(s):
            if not isinstance(s,str):
                raise TypeError("Tag names must be a string or list of strings")
        if isinstance(names,(list,tuple)):
            for name in names:
                checkstr(name)
        else:
            checkstr(names)
        if replace:
            for name in self.tags:
                self.removeTag(name)
        if isinstance(names,str):
            self.addTag(names)
        else:
            for name in names:
                self.addTag(name)

    def addTag(self,tag):
        """Add a tagname to this node
        @param tag: tagname for this node
        @type tag: str
        @rtype: None
        """
        Tree.lock()
        try:
            status =_TreeShr._TreeAddTag(self.tree.ctx,
                                         self._nid,
                                         _C.c_char_p(str.encode(tag)))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def beginSegment(self,start,end,dimension,initialValueArray,idx=-1):
        """Begin a record segment
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dimension: Dimension information of segment
        @type dimension: Dimension
        @param initialValueArray: Initial data array. Defines shape of segment
        @type initialValueArray: Array
        @rtype: None
        """
        try:
          Tree.lock()
          status=_TreeShr._TreeBeginSegment(self.tree.ctx,
                                            self._nid,
                                            _C.pointer(_data.Data(start).descriptor),
                                            _C.pointer(_data.Data(end).descriptor),
                                            _C.pointer(_data.Data(dimension).descriptor),
                                            _C.pointer(_data.Data(initialValueArray).descriptor),
                                            _C.c_int32(idx))
        finally:
          Tree.unlock()
        if (status & 1) == 0:
          raise _exceptions.TreeException("Error beginning segment: %s" % _data.getStatusMsg(status))

    def beginTimestampedSegment(self,array,idx=-1):
        """Allocate space for a timestamped segment
        @param array: Initial data array to define shape of segment
        @type array: Array
        @param idx: Optional segment index. Defaults to -1 meaning next segment.
        @type idx: int
        @rtype: None
        """
        try:
          Tree.lock()
          status=_TreeShr._TreeBeginTimestampedSegment(self.tree.ctx,
                                                       self._nid,
                                                       _C.pointer(_data.Data(array).descriptor),
                                                       _C.c_int32(idx))
        finally:
          Tree.unlock()
        if (status & 1)==0:
          raise _exceptions.TreeException("Error beginning timestamped segment: %s" % _data.getStatusMsg(status))

    def compare(self,value):
        """Returns True if this node contains the same data as specified in the value argument
        @param value: Value to compare contents of the node with
        @type value: Data
        @rtype: Bool
        """
        if isinstance(value,TreePath) and isinstance(self,TreePath):
          ans=str(self)==str(value)
        elif type(self)==TreeNode and type(value)==TreeNode:
          ans=self._nid==value._nid and self.tree==value.tree
        else:
          try:
            ans=value.compare(self.record)
          except _exceptions.treeshrExceptions.TreeNODATA:
            ans=value is None
        return ans

    def containsVersions(self):
        """Return true if this node contains data versions
        @return: True if node contains versions
        @rtype: bool
        """
        return self.versions

    def delete(self):
        """Delete this node from the tree
        @rtype: None
        """
        self.tree.deleteNode(self.fullpath)

    def deleteData(self):
        """Delete data from this node
        @rtype: None
        """
        self.putData(None)
        return

    def setDoNotCompress(self,flag):
        """Set do not compress state of this node
        @param flag: True do disable compression, False to enable compression
        @type flag: bool
        @rtype: None
        """
        self._setNciFlag(0x200,flag)

    def dir(self):
        """list descendants"""
        for desc in self.descendants:
            print('%-12s    %s'%(desc.node_name,desc.usage))

    def dispatch(self,wait=True):
        """Dispatch an action node
        @rtype: None
        """
        a=self.record
        if not isinstance(a,_compound.Action):
            raise TreeNodeException("Node does not contain an action description")
        else:
            if wait:
                _mdsdcl.tcl("dispatch/wait %s"%(self.fullpath,),0,0,1)
            else:
                _mdsdcl.tcl("dispatch %s"     %(self.fullpath,),0,0,1)

    @property
    def deref(self):
        """ resolves as many node references as possible
            device nodes wil be conserved
        """
        if self.usage=='DEVICE':
            return self
        try:     ans = self.record
        except:  return self
        if isinstance(ans, (_data.Data,TreeNode)):
            return ans.deref
        return ans

    def doMethod(self,method,*args):
        """Execute method on conglomerate element
        @param method: method name to perform
        @type method: str
        @param arg: Optional argument for method
        @type arg: Data
        @rtype: None
        """
        try:
            Tree.lock()
            self.restoreContext()
            arglist=[self.tree.ctx,
                     _C.pointer(_scalar.Int32(self.nid).descriptor),
                     _C.pointer(_scalar.String(method).descriptor)]
            ans_d=_descriptor.descriptor_xd()
            for arg in args:
                arglist.append(_descriptor.objectToPointer(_data.Data(arg)))
            arglist.append(_C.pointer(ans_d))
            arglist.append(_C.c_void_p(0xffffffff))
            status=_TreeShr._TreeDoMethod(*arglist)
            if (status & 1):
                return ans_d.value
            else:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()
        return

    @classmethod
    def fromDescriptor(cls,d):
        return cls(_C.cast(d.pointer,_C.POINTER(_C.c_int32)).contents.value)

    def getBrother(self):
        """Return sibling of this node
        @return: Sibling of this node
        @rtype: TreeNode
        """
        return self.brother

    def getChild(self):
        """Return first child of this node.
        @return: Return first child of this node or None if it has no children.
        @rtype: TreeNode
        """
        return self.child

    def getChildren(self):
        """Return TreeNodeArray of children nodes.
        @return: Children of this node
        @rtype: TreeNodeArray
        """
        return self.children_nids

    def getClass(self):
        """Return MDSplus class name of this node
        @return: MDSplus class name of the data stored in this node.
        @rtype: String
        """
        return self.class_str

    def getCompressedLength(self):
        """Return compressed data length of this node
        @return: Compress data length of this node
        @rtype: int
        """
        return self.rlength

    def getConglomerateElt(self):
        """Return index of this node in a conglomerate
        @return: element index of this node in a conglomerate. 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.conglomerate_elt

    def getConglomerateNodes(self):
        """Return TreeNodeArray of conglomerate elements
        @return: Nodes in this conglomerate.
        @rtype: TreeNodeArray
        """
        return self.conglomerate_nids

    def getRecord(self, *altvalue):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        try:
            xd=_descriptor.Descriptor_xd()
            xd.tree=self.tree
            status=_TreeShr._TreeGetRecord(self.tree.ctx,
                                           self._nid,_C.pointer(xd))
            if (status & 1):
                return xd.value
            elif len(altvalue)==1 and status == _exceptions.treeshrExceptions.TreeNODATA.status:
                return altvalue[0]
            else:
                raise _exceptions.statusToException(status)
        except _exceptions.MDSplusException:
            raise  # no need for traceback as this is expected to happen
        except Exception:
            import traceback
            traceback.print_exc()
            raise
    getData=getRecord

    def getDataNoRaise(self):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        return self.getData(None)

    def getDepth(self):
        """Get depth of this node in the tree
        @return: number of levels between this node and the top of the currently opened tree.
        @rtype: Int32
        """
        return self.depth

    def getDescendants(self):
        """Return TreeNodeArray of first level descendants (children and members).
        @return: First level descendants of this node
        @rtype: TreeNodeArray
        """
        return self.member_nids + self.children_nids

    def getDtype(self):
        """Return the name of the data type stored in this node
        @return: MDSplus data type name of data stored in this node.
        @rtype: String
        """
        return self.dtype_str

    def getFullPath(self):
        """Return full path of this node
        @return: full path specification of this node.
        @rtype: String
        """
        return self.fullpath

    def getLength(self):
        """Return uncompressed data length of this node
        @return: Uncompressed data length of this node
        @rtype: int
        """
        return self.length

    def getLocalTree(self):
        """Return tree containing this node
        @return: Name of tree containing this node
        @rtype: str
        """
        top=self
        while top.nid & 0xffffff:
            top=top.parent
        if top.node_name == 'TOP':
            return self.tree.tree
        else:
            return top.node_name

    def getLocalPath(self):
        """Return path relative to top of local tree
        @return: Path relative to top of local tree
        @rtype: str
        """
        path=''
        top=self
        while top.nid & 0xffffff:
            if top.is_member:
                delim=':'
            else:
                delim='.'
            path=delim + top.node_name + path
            top=top.parent
        return path

    def getMember(self):
        """Return first member node
        @return: First member of thie node
        @rtype: TreeNode
        """
        return self.member

    def getMembers(self):
        """Return TreeNodeArray of this nodes members
        @return: members of this node
        @rtype: TreeNodeArray
        """
        return self.member_nids

    def getMinPath(self):
        """Return shortest path string for this node
        @return: shortest path designation depending on the current node default and whether the node has tag names or not.
        @rtype: String
        """
        return self.minpath

    class NCI_ITEMS(_C.Structure):
        _fields_=list()
        for idx in range(50):
            _fields_+=[("buflen%d"%idx,_C.c_ushort),
                       ("code%d"%idx,_C.c_ushort),
                       ("pointer%d"%idx,_C.c_void_p),
                       ("retlen%d"%idx,_C.POINTER(_C.c_int32))]

        def __init__(self,items):
            self.ans=list()
            self.retlen=list()
            self.rettype=list()
            if not isinstance(items,(list,tuple)):
                items=tuple(items)
            for idx in range(len(items)):
                item="Nci"+items[idx].upper()
                item_info=TreeNode.__dict__[item]
                code=item_info[0]
                item_type=item_info[1]
                item_length=item_info[2]
                self.retlen.append(_C.c_int32(0))
                self.rettype.append(item_info[3])
                if item_type == _C.c_char_p:
                    self.ans.append(item_type(str.encode(' ')*item_length))
                    self.__setattr__('pointer%d'%idx,
                                     _C.cast(self.ans[idx],_C.c_void_p))
                else:
                    self.ans.append(item_type())
                    self.__setattr__('pointer%d'%idx,
                                     _C.cast(_C.pointer(self.ans[idx]),_C.c_void_p))
                self.__setattr__('buflen%d'%idx,_C.c_ushort(item_length))
                self.__setattr__('code%d'%idx,_C.c_ushort(code))
                self.__setattr__('retlen%d'%idx,_C.pointer(self.retlen[idx]))
            self.__setattr__('buflen%d'%len(items),_C.c_ushort(0))
            self.__setattr__('code%d'%len(items),_C.c_ushort(TreeNode.NciEND_OF_LIST[0]))
            self.__setattr__('pointer%d'%len(items),_C.c_void_p(0))
            self.__setattr__('retlen%d'%len(items),_C.cast(_C.c_void_p(0),_C.POINTER(_C.c_int32)))

    def getNci(self,items,returnDict=True):
        """Return dictionary of nci items"""
        status=0

        if isinstance(items,str):
            items=[items]
        elif isinstance(items,tuple):
            items=list(items)
        elif not isinstance(items,list):
            raise TypeError("items must be a list,tuple or str")
        flag_items=list()
        flags=None
        ans=dict()
        flag_items=list()
        for item in items:
            if "NciM_"+item.upper() in TreeNode.__dict__:
                flag_items.append(item)
                if flags is None:
                    flags=int(self.get_flags)
                ans[item]=(flags & TreeNode.__dict__["NciM_"+item.upper()]) != 0
        for item in flag_items:
            items.remove(item)

        itmlst=self.NCI_ITEMS(items)
        try:
            Tree.lock()
            if len(items) > 0:
                if not isinstance(self.tree.ctx,_C.c_void_p):
                    raise TypeError("tree ctx is type %s, must be a c_void_p" % str(type(self.tree.ctx)))
                status=_TreeShr._TreeGetNci(self.tree.ctx,
                                            self._nid,
                                            _C.pointer(itmlst))
            else:
                status=1
            if (status & 1)==1:
                for idx in range(len(items)):
                    val=itmlst.ans[idx]
                    rettype=itmlst.rettype[idx]
                    retlen=itmlst.retlen[idx].value
                    if rettype == _scalar.String:
                        val=_scalar.String(val.value[0:retlen].rstrip())
                    elif issubclass(rettype,_scalar.Scalar):
                        val=_scalar.Scalar(val)
                    elif rettype == TreeNode:
                        if retlen == 4:
                            val=TreeNode(int(val.value),self.tree)
                        else:
                            val=None
                    elif rettype == TreeNodeArray:
                        nids=list()
                        for n in range(retlen//4):
                            nids.append(int(val[n]))
                        val=TreeNodeArray(nids,self.tree)
                    elif rettype == bool:
                        val=val.value != 0
                    ans[items[idx]]=val
        finally:
            Tree.unlock()
        if (status & 1)==0:
            raise TreeNodeException(status)
        if not returnDict and len(ans) == 1:
            return list(ans.values())[0]
        return ans


    def getNid(self):
        """Return node index
        @return: Internal node index of this node
        @rtype: int
        """
        return self.nid

    def getNode(self,path):
        """Return tree node where path is relative to this node
        @param path: Path relative to this node
        @type path: str
        @return: node matching path
        @rtype: TreeNode
        """
        if path[0] == '\\':
            return self.tree.getNode(path)
        elif not path[0]  in ':.':
            path=':'+path
        return self.tree.getNode(self.fullpath+path)

    def getNodeName(self):
        """Return node name
        @return: Node name of this node. 1 to 12 characters
        @rtype: String
        """
        return self.node_name

    def getNodeWild(self,path,*usage):
        """Return tree nodes where path is relative to this node
        @param path: Path relative to this node
        @type path: str
        @return: node matching path
        @rtype: TreeNodeArray
        """
        try:
            olddef=self.tree.default
            self.tree.default=self
            ans = self.tree.getNodeWild(path,*usage)
        finally:
            self.tree.default=olddef
        return ans

    def getNumChildren(self):
        """Return number of children nodes.
        @return: Number of children
        @rtype: Int32
        """
        return self.number_of_children

    def getNumDescendants(self):
        """Return number of first level descendants (children and members)
        @return: total number of first level descendants of this node
        @rtype: int
        """
        return self.number_of_children+self.number_of_members

    def getNumElts(self):
        """Return number of nodes in this conglomerate
        @return: Number of nodes in this conglomerate or 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.number_of_elts

    def getNumMembers(self):
        """Return number of members
        @return: number of members
        @rtype: int
        """
        return self.number_of_members

    def getNumSegments(self):
        """return number of segments contained in this node
        @rtype: int
        """
        Tree.lock()
        try:
            num=_C.c_int32(0)
            status=_TreeShr._TreeGetNumSegments(self.tree.ctx,
                                                self._nid,
                                                _C.pointer(num))
        finally:
            Tree.unlock()
        if (status & 1):
            return num.value
        else:
            raise _exceptions.statusToException(status)

    def getOriginalPartName(self):
        """Return the original part name of node in conglomerate
        @return: Original part name of this node when conglomerate was first instantiated.
        @rtype: String
        """
        return self.original_part_name

    def getOwnerId(self):
        """Get id/gid value of account which wrote data to this node
        @return: Return user id of last account used to write data to this node
        @rtype: int
        """
        return self.owner_id

    def getParent(self):
        """Return parent of this node
        @return: Parent of this node
        @rtype: TreeNode
        """
        return self.parent

    def getPath(self):
        """Return path of this node
        @return: Path to this node.
        @rtype: String
        """
        return self.path

    def getSegment(self,idx):
        """Return segment
        @param idx: The index of the segment to return. Indexes start with 0.
        @type idx: int
        @return: Data segment
        @rtype: Signal | None
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            value=_descriptor.Descriptor_xd()
            dim=_descriptor.Descriptor_xd()
            status=_TreeShr._TreeGetSegment(self.tree.ctx,
                                            self._nid,
                                            _C.c_int32(idx),
                                            _C.pointer(value),
                                            _C.pointer(dim))
            if (status & 1):
                return _compound.Signal(value.value,None,dim.value)
            else:
                raise _exceptions.statusToException(status)
        else:
            return None

    def getSegmentDim(self,idx):
        """Return dimension of segment
        @param idx: The index of the segment to return. Indexes start with 0.
        @type idx: int
        @return: Segment dimension
        @rtype: Dimension
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            return self.getSegment(idx).getDimensionAt(0)
        else:
            return None

    def getSegmentLimits(self,idx):
        start=_descriptor.Descriptor_xd()
        end=_descriptor.Descriptor_xd()
        status=_TreeShr._TreeGetSegmentLimits(self.tree.ctx,
                                              self._nid,
                                              _C.c_int32(idx),
                                              _C.pointer(start),
                                              _C.pointer(end))
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
            raise _exceptions.statusToException(status)

    def getSegmentEnd(self,idx):
        """return end of segment
        @param idx: segment index to query
        @type idx: int
        @rtype: Data
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            limits=self.GetSegmentLimits(idx)
            if limits is not None:
                return limits[1]
            else:
                return None
        else:
            return None

    def getSegmentStart(self,idx):
        """return start of segment
        @param idx: segment index to query
        @type idx: int
        @rtype: Data
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            limits=self.GetSegmentLimits(idx)
            if limits is not None:
                return limits[0]
            else:
                return None
        else:
            return None

    def getStatus(self):
        """Return action completion status
        @return: action completion status stored by dispatcher if this node is a dispacted action. Low bit set is success.
        @rtype: int
        """
        return self.status

    def getTimeInserted(self):
        """Return time data was written
        @return: time data was written to this node as Uint64. Use answer.date to retrieve date/time string
        @rtype: Uint64
        """
        return self.time_inserted

    def getTags(self):
        """Return tags of this node
        @return: Tag names pointing to this node
        @rtype: ndarray
        """
        Tree.lock()
        try:
            ctx=_C.c_void_p(0)
            tags=list()
            fnt=_TreeShr._TreeFindNodeTags
            fnt.restype=_C.c_void_p
            while True:
                tag_ptr=_TreeShr._TreeFindNodeTags(self.tree.ctx,
                                                   self._nid,
                                                   _C.pointer(ctx))
                value = _C.cast(tag_ptr,_C.c_char_p).value
                if value is None:
                    break;
                tags.append(_ver.tostr(value.rstrip()))
                _TreeShr.TreeFree(_C.c_void_p(tag_ptr))
        finally:
            Tree.unlock()
        tags = _array.Array(tags).astype(_ver.npstr)
        return tags


    def getTree(self):
        """Return Tree associated with this node
        @return: Tree associated with this node
        @rtype: Tree
        """
        return self.tree

    def getUsage(self):
        """Return usage of this node
        @return: usage of this node
        @rtype: str
        """
        return _scalar.String(str(self.usage_str)[10:])

    def hasNodeReferences(self):
      """Return True if this node contains data that includes references
      to other nodes in the same tree
      @return: True of data references other nodes
      @rtype: bool
      """
      return self.node_reference

    def hasPathReferences(self):
      """Return True if this node contains node references using paths.
      This usually means the data references nodes from other subtrees.
      @return: True if data contains node path references
      @rtype: bool
      """
      return self.path_reference

    def isChild(self):
        """Return true if this is a child node
        @return: True if this is a child node instead of a member node.
        @rtype: bool
        """
        return int(self.parent_relationship)==TreeNode.NciK_IS_CHILD

    def isCompressible(self):
        """Return true if node contains data which can be compressed
        @return: True of this node contains compressible data
        @rtype: bool
        """
        return self.compressible

    def isCompressOnPut(self):
        """Return true if node is set to compress on put
        @return: True if compress on put
        @rtype: bool
        """
        return self.compress_on_put

    def isCompressSegments(self):
        """Return true if node is set to compress segments"""
        return self.compress_segments

    def isDisabled(self):
      """Return true if this node is disabled (opposite of isOn)
      @return: True if node is off
      @rtype: bool
      """
      return not self.isOn()

    def isDoNotCompress(self):
        """Return true if compression is disabled for this node
        @return: True if this node has compression disabled
        @rtype: bool
        """
        return self.do_not_compress

    def isEssential(self):
        """Return true if successful action completion is essential
        @return: True if this node is marked essential.
        @rtype: bool
        """
        return self.essential

    def isIncludeInPulse(self):
        """Return true if this subtree is to be included in pulse file
        @return: True if subtree is to be included in pulse file creation.
        @rtype: bool
        """
        return self.include_in_pulse

    def isMember(self):
        """Return true if this is a member node
        @return:  True if this is a member node
        @rtype: bool
        """
        return int(self.parent_relationship)==TreeNode.NciK_IS_MEMBER

    def isNoWriteModel(self):
        """Return true if data storage to model is disabled for this node
        @return: Return True if storing data in this node in the model tree is disabled
        @rtype: bool
        """
        return self.no_write_model

    def isNoWriteShot(self):
        """Return true if data storage to pulse file is disabled for this node
        @return: Return True if storing data in this node in the pulse tree is disabled
        @rtype: bool
        """
        return self.no_write_shot

    def isOn(self):
        """Return True if node is turned on, False if not.
        @return: Return True if node is turned on
        @rtype: bool
        """
        return (int(self.get_flags) & 3)==0

    def isParentDisabled(self):
      """Return True if parent is disabled
      @return: Return True if parent is turned off
      @rtype: bool
      """
      return not self.isParentOn()

    def isParentOn(self):
      """Return True if parent is on
      @return: Return True if parent is turned on
      @rtype: bool
      """
      return (int(self.get_flags) & 2)==0

    def isSegmented(self):
        """Return true if this node contains segmented records
        @return: True if node contains segmented records
        @rtype: bool
        """
        return self.segmented

    def isSetup(self):
        """Return true if data is setup information.
        @return: True if data is setup information (originally written in the model)
        @rtype: bool
        """
        return self.setup

    def isWriteOnce(self):
        """Return true if node is set write once
        @return: Return True if data overwrite in this node is disabled
        @rtype: bool
        """
        return self.write_once

    def makeSegment(self,start,end,dimension,valueArray,idx=-1):
        """Make a record segment
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dimension: Dimension information of segment
        @type dimension: Dimension
        @param valueArray: Contents of segment
        @type valueArray: Array
        @rtype: None
        """
        Tree.lock()
        try:
            shape = 1 if isinstance(valueArray,_compound.Compound) else valueArray.shape[0]
            status=_TreeShr._TreeMakeSegment(self.tree.ctx,
                                             self._nid,
                                             _descriptor.objectToPointer(_data.Data(start)),
                                             _descriptor.objectToPointer(_data.Data(end)),
                                             _descriptor.objectToPointer(_data.Data(dimension)),
                                             _descriptor.objectToPointer(_data.Data(valueArray)),
                                             _C.c_int32(idx),
                                             _C.c_int32(shape))
        finally:
            Tree.unlock()
        if not (status & 1):
            raise _exceptions.statusToException(status)

    def move(self,parent,newname=None):
        """Move node to another location in the tree and optionally rename the node
        @param parent: New parent of this node
        @type parent: TreeNode
        @param newname: Optional new node name of this node. 1-12 characters, no path delimiters.
        @type newname: str
        @rtype: None
        """
        if newname is None:
            newname = _ver.tostr(self.node_name)
        newpath = _ver.tostr(parent.path)
        newpath+= "." if self.isChild() else ":"
        newpath+= newname
        Tree.lock()
        try:
            status = _TreeShr._TreeRenameNode(self.tree.ctx,
                                              self._nid,
                                              _ver.tobytes(newpath))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def putData(self,value):
        """Store data
        @param data: Data to store in this node.
        @type data: Data
        @rtype: None
        """
        Tree.lock()
        try:
            if isinstance(value,_data.Data) and value.__hasBadTreeReferences__(self.tree):
                value=value.__fixTreeReferences__(self.tree)
            status=_TreeShr._TreePutRecord(self.tree.ctx,
                                           self._nid,
                                           _descriptor.objectToPointer(_data.Data(value)),
                                           0)
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def putRow(self,bufsize,array,timestamp):
        """Load a timestamped segment row
        @param bufsize: number of rows in segment
        @type bufsize: int
        @param array: data for this row
        @type array: Array or Scalar
        @param timestamp: Timestamp of this row
        @type timestamp: Uint64
        @rtype: None
        """
        Tree.lock()
        try:
            status=_TreeShr._TreePutRow(self.tree.ctx,
                                        self._nid,
                                        _C.c_int32(bufsize),
                                        _C.c_void_p(_scalar.Uint64(timestamp).descriptor.pointer),
                                        _C.pointer(_array.Array(array).descriptor))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def putSegment(self,data,idx):
        """Load a segment in a node
        @param data: data to load into segment
        @type data: Array or Scalar
        @param idx: index into segment to load data
        @type idx: int
        @rtype: None
        """
        Tree.lock()
        try:
            status=_TreeShr._TreePutSegment(self.tree.ctx,
                                            self._nid,
                                            _C.c_int32(idx),
                                            _C.pointer(_data.Data(data).descriptor))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def putTimestampedSegment(self,timestampArray,array):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @rtype: None
        """
        timestampArray=_array.Uint64Array(timestampArray)
        array=_array.Array(array)
        Tree.lock()
        try:
            status=_TreeShr._TreePutTimestampedSegment(self.tree.ctx,
                                               self._nid,
                                               _descriptor.objectToPointer(timestampArray),
                                               _descriptor.objectToPointer(array))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def makeTimestampedSegment(self,timestampArray,array,idx,rows_filled):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @param idx: Segment number
        @param idx: int
        @param rows_filled: Number of rows of segment filled with data
        @type rows_filled: int
        @rtype: None
        """
        timestampArray=_array.Uint64Array(timestampArray)
        array=_array.Array(array)
        Tree.lock()
        try:
            status=_TreeShr._TreeMakeTimestampedSegment(self.tree.ctx,
                                                        self._nid,
                                                        _descriptor.objectToPointer(timestampArray),
                                                        _descriptor.objectToPointer(array),
                                                        _C.c_int32(idx),
                                                        _C.c_int32(rows_filled))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()

    def removeTag(self,tag):
        """Remove a tagname from this node
        @param tag: Tagname to remove from this node
        @type tag: str
        @rtype: original type
        """
        try:
            tag = _ver.tostr(tag)
            n=self.tree.getNode('\\'+tag)
            if n.nid != self.nid:
                raise TreeNodeException("Node %s does not have a tag called %s. That tag refers to %s" % (str(self),tag,str(n)))
        except _exceptions.TreeNNF:
            raise TreeNodeException("Tag %s is not defined" % (tag,))
        self.tree.removeTag(tag)
        return self

    def rename(self,newname):
        """Rename node this node
        @param newname: new name of this node. 1-12 characters, no path delimiters.
        @type newname: str
        @rtype: original type
        """
        if newname.find(':') >=0 or newname.find('.') >= 0:
            raise TreeNodeException("Invalid node name, do not include path delimiters in nodename")
        try:
            self.tree.lock()
            olddefault=self.tree.default
            self.tree.setDefault(self.parent)
            if self.isChild():
                newname="."+_ver.tostr(newname)
            status = _TreeShr._TreeRenameNode(self.tree.ctx,
                                              self._nid,
                                              _C.c_char_p(_ver.tobytes(newname)))
            if not (status & 1):
                raise _exceptions.statusToException(status)
        finally:
            self.tree.setDefault(olddefault)
            self.tree.unlock()
        return self

    def restoreContext(self):
        """Restore tree context. Used by internal functions.
        @rtype: None
        """
        if self.tree is not None:
            self.tree.restoreContext()

    def _setNciFlag(self,mask,setting):
        class NCI_ITEMS(_C.Structure):
          _fields_=[("buflen",_C.c_ushort),("code",_C.c_ushort),
                    ("pointer",_C.POINTER(_C.c_uint32)),("retlen",_C.c_void_p),
                    ("buflen_e",_C.c_ushort),("code_e",_C.c_ushort),
                    ("pointer_e",_C.c_void_p),("retlen_e",_C.c_void_p)]
        itmlst=NCI_ITEMS()
        code={True:1,False:2}
        mask=_C.c_uint32(mask)
        itmlst.buflen=0
        itmlst.code=code[setting]
        itmlst.pointer=_C.pointer(mask)
        itmlst.retlen=0
        itmlst.buflen_e=0
        itmlst.code_e=0
        itmlst.pointer_e=0
        itmlst.retlen_e=0
        try:
          Tree.lock()
          status = _TreeShr._TreeSetNci(self.tree.ctx,
                                        self._nid,
                                        _C.pointer(itmlst))
        finally:
          Tree.unlock()
        if not (status & 1):
            raise _exceptions.statusToException(status)

    def setCompressOnPut(self,flag):
        """Set compress on put state of this node
        @param flag: State to set the compress on put characteristic
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x400,flag)

    def setCompressSegments(self,flag):
        """Set compress segments state of this node
        @param flag: State to set the compress segments characteristic
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x10000,flag)

    def setEssential(self,flag):
        """Set essential state of this node
        @param flag: State to set the essential characteristic. This is used on action nodes when phases are dispacted.
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x04,flag)

    def setIncludeInPulse(self,flag):
        """Set include in pulse state of this node
        @param flag: State to set the include in pulse characteristic.
        If true and this node is the top node of a subtree the subtree will be included in the pulse.
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag( 0x8000,flag)

    def setNoWriteModel(self,flag):
        """Set no write model state for this node
        @param flag: State to set the no write in model characteristic.
        If true then no data can be stored in this node in the model.
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x800,flag)

    def setNoWriteShot(self,flag):
        """Set no write shot state for this node
        @param flag: State to set the no write in shot characteristic.
        If true then no data can be stored in this node in a shot file.
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x1000,flag)

    def setParentOn(self,flag):
        """Turn parent node on or off
        @param flag: State to set the on characteristic.
        If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: None
        """
        self.parent.on = flag

    def setOn(self,flag):
        """Turn node on or off
        @param flag: State to set the on characteristic.
        If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: None
        """
        Tree.lock()
        try:
            if flag:
                status=_TreeShr._TreeTurnOn(self.tree.ctx,
                                            self._nid)
            else:
                status=_TreeShr._TreeTurnOff(self.tree.ctx,
                                             self._nid)
            if not (status & 1):
                if (status==_exceptions.TreeLOCK_FAILURE.status):
                    print(_exceptions.TreeLOCK_FAILURE.message)
                else:
                    raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()


    def setSubtree(self,flag):
        """Enable/Disable node as a subtree
        @param flag: True to make node a subtree reference.
        Node must be a child node with no descendants.
        @type flag: bool
        @rtype: original type
        """
        Tree.lock()
        try:
            if flag:
                status = _TreeShr._TreeSetSubtree(self.tree.ctx,
                                                  self._nid)
            else:
                status = _TreeShr._TreeSetNoSubtree(self.tree.ctx,
                                                    self._nid)
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()
        return self

    def setUsage(self,usage):
        """Set the usage of a node
        @param usage: Usage string.
        @type flag: str
        @rtype: original type
        """
        try:
            usagenum=_usage_table[usage.upper()]
        except KeyError:
            raise KeyError('Invalid usage specified. Use one of %s' % (str(_usage_table.keys()),))
        Tree.lock()
        try:
            status = _TreeShr._TreeSetUsage(self.tree.ctx,
                                            self._nid,
                                            _C.c_int32(usagenum))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock()
        return self

    def setTree(self,tree):
        """Set Tree associated with this node
        @param tree: Tree instance to associated with this node
        @type tree: Tree
        @rtype: original type
        """
        self.tree=tree
        return self

    def setWriteOnce(self,flag):
        """Set write once state of node
        @param flag: State to set the write once characteristic. If true then data can only be written if the node is empty.
        @type flag: bool
        @rtype: original type
        """
        self._setNciFlag(0x80, flag)

    def updateSegment(self,start,end,dimension,idx):
        """Update a segment
        @param start: index of first row of segment
        @type start: Data
        @param end: index of last row of segment
        @type end: Data
        @param dim: Dimension of segment
        @type dim: Dimension
        @param idx: Index of segment
        @type idx: int
        @rtype: None
        """
        Tree.lock()
        try:
            status = _TreeShr._TreeUpdateSegment(self.tree.ctx,
                                             self._nid,
                                             _descriptor.objectToPointer(_data.Data(start)),
                                             _descriptor.objectToPointer(_data.Data(end)),
                                             _descriptor.objectToPointer(_data.Data(dimension)),
                                             _C.c_int32(idx))
            if not status & 1:
                raise _exceptions.statusToException(status)
        finally:
            Tree.unlock

class TreePath(TreeNode): # HINT: TreePath begin
    """Class to represent an MDSplus node reference (path)."""

    dtype_id = 193

    def __init__(self,path,*tree):
        self.tree_path=str(path);
        if len(tree)<1 or not isinstance(tree[0],(Tree,)):
            self.tree=Tree(*tree)
        else:
            self.tree=tree[0]
        try:
            self._nid=self.tree.getNode(self.tree_path)._nid
        except:
            self._nid=None
        return

    def __hasBadTreeReferences__(self,tree):
       return False


    def __str__(self):
        """Convert path to string."""
        return self.tree_path

    @property
    def descriptor(self):
        d=_descriptor.Descriptor_s()
        d.length=len(self.tree_path)
        d.dtype=self.dtype_id
        d.pointer=_C.cast(_C.c_char_p(_ver.tobytes(self.tree_path)),_C.c_void_p)
        d.original=self
        d.tree=self.tree
        if self._units or self._error is not None or self._help is not None or self._validation is not None:
            return _compound.Compound.descriptorWithProps(self,d)
        else:
            return d

    @classmethod
    def fromDescriptor(cls,d):
        return cls(_ver.tostr(_C.cast(d.pointer,_C.POINTER(_C.c_char*d.length)).contents.value))


class TreeNodeArray(_array.Int32Array): # HINT: TreeNodeArray begin
    def __init__(self,nids,*tree):
        if isinstance(nids,_C.Array):
            try:
                nids=_N.ctypeslib.as_array(nids)
            except Exception:
                pass
        nids = _N.array(nids)
        if len(nids.shape) == 0:  # happens if value has been a scalar, e.g. int
            nids = nids.reshape(1)
        self._value = nids.__array__(_N.int32)
        if len(tree)<1 or not isinstance(tree[0],(Tree,)):
            self.tree=Tree(*tree)
        else:
            self.tree=tree[0]

    def __eq__(self,obj):
        if isinstance(obj,(TreeNodeArray,)):
            if self.tree == obj.tree:
                return self._value == obj._value
            _N.zeros((max(self._value.size,obj._value.size),),_N.bool)
        return _N.zeros_like(self._value,_N.bool)


    def __getitem__(self,n):
        """Return TreeNode from mdsarray. array[n]
        @param n: Index into array beginning with index 0
        @type n: int
        @return: node
        @rtype: TreeNode
        """
        return TreeNode(self._value[n],self.tree)

    def __add__(self,x):
        if isinstance(x, (TreeNodeArray,)) and self.tree==x.tree:
            return TreeNodeArray(_N.concatenate((self._value,x._value)),self.tree)
        raise Exception('You can only add TreeNodeArrays of the same tree')

    @property
    def nid_number(self):
        return _array.Array(self._value)

    def restoreContext(self):
        self.tree.restoreContext()

    def getPath(self):
        """Return tuple of node names"
        @return: Node names
        @rtype: tuple of String's
        """
        return self.path

    def getFullPath(self):
        """Return tuple of fullpaths
        @return: Full node paths
        @rtype: tuple of String's
        """
        return self.fullpath

    def setOn(self,flag):
        """Turn nodes on or off
        @param flag: True to turn nodes on, False to turn them off
        @type flag: Bool
        @rtype: None
        """
        for node in self:
            node.on = flag

    def getLength(self):
        """Return data lengths
        @return: tuple of data lengths
        @rtype: tuple of Int32's
        """
        return self.length

    def getCompressedLength(self):
        """Return compressed data lengths
        @return: tuple of compressed data lengths
        @rtype: tuple of Int32's
        """
        return self.rlength

    def setWriteOnce(self,flag):
        """Set nodes write once
        @rtype: original type
        """
        for node in self:
            node.write_once=flag
        return self

    def isCompressOnPut(self):
        """Is nodes set to compress on put
        @return: state of compress on put flags
        @rtype: tuple of bools
        """
        return self.compress_on_put

    def setCompressOnPut(self,flag):
        """Set compress on put flag
        @param flag: True if compress on put, False if not
        @type flag: bool
        """
        for node in self:
            node.compress_on_put=flag
        return self

    def setNoWriteModel(self,flag):
        """Set no write model flag
        @param flag: True to disallow writing to model
        @type flag: bool
        @rtype: original type
        """
        for node in self:
            node.no_write_model=flag
        return self

    def setNoWriteShot(self,flag):
        """set no write shot flags
        @param flag: True if setting no write shot
        @type flag: bool
        @rtype: original type
        """
        for node in self:
            node.no_write_shot=flag
        return self

    def getUsage(self):
        """Get usage of nodes
        @return: Usage
        @rtype: StringArray
        """
        self.usage

    def __getattr__(self,name):
        ans=[]
        doArray=None
        for node in self:
            val=node.__getattribute__(name)
            if isinstance(val,_data.Data):
                val=val.data()
                if doArray is None:
                    doArray=True
            else:
                doArray is False
            ans.append(val)
        if doArray:
            ans = _array.Array(ans)
        else:
            ans = tuple(ans)
        return ans


class Device(TreeNode): # HINT: Device begin
    """Used for device support classes. Provides ORIGINAL_PART_NAME, PART_NAME and Add methods and allows referencing of subnodes as conglomerate node attributes.

    Use this class as a superclass for device support classes. When creating a device support class include a class attribute called "parts"
    which describe the subnodes of your device implementation. The parts attribute should be a list or tuple of dict objects where each dict is a
    description of each subnode. The dict object should include a minimum of a 'path' key whose value is the relative path of the node (be sure to
    include the leading period or colon) and a 'type' key whose value is the usage type of the node. In addition you may optionally specify a
    'value' key whose value is the actual value to store into the node when it is first added in the tree. Instead of a 'value' key, you can
    provide a 'valueExpr' key whose value is a string which is python code to be evaluated before writing the result into the node. Use a valueExpr
    when you need to include references to other nodes in the device. Lastly the dict instance may contain an 'options' key whose values are
    node options specified as a tuple of strings. Note if you only specify one option include a trailing comma in the tuple.The "parts" attribute
    is used to implement the Add and PART_NAME and ORIGNAL_PART_NAME methods of the subclass.

    You can also include a part_dict class attribute consisting of a dict() instance whose keys are attribute names and whose values are nid
    offsets. If you do not provide a part_dict attribute then one will be created from the part_names attribute where the part names are converted
    to lowercase and the colons and periods are replaced with underscores. Referencing a part name will return another instance of the same
    device with that node as the node in the Device subclass instance. The Device class also supports the part_name and original_part_name
    attributes which is the same as doing devinstance.PART_NAME(None). NOTE: Device subclass names MUST BE UPPERCASE!

    Sample usage1::

       from MDSplus import Device

       class MYDEV(Device):
           parts=[{'path':':COMMENT','type':'text'},
                  {'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)},
                  {'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                  'options':('no_write_shot',)},
                  {'path':'.SETTINGS','type':'structure'},
                  {'path':'.SETTINGS:KNOB1','type':'numeric'},
                  {'path':'.SIGNALS','type':'structure'},
                  {'path':'.SIGNALS:CHANNEL_1','type':'signal','options':('no_write_model','write_once')}]

           def init(self,arg):
               knob1=self.settings_knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.signals_channel_1=Signal(32,None,42)

    Sample usage2::

       from MDSplus import Device

           parts=[{'path':':COMMENT','type':'text'},
                  {'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)},
                  {'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                  'options':('no_write_shot',)},
                  {'path':'.SETTINGS','type':'structure'},
                  {'path':'.SETTINGS:KNOB1','type':'numeric'},
                  {'path':'.SIGNALS','type':'structure'},
                  {'path':'.SIGNALS:CHANNEL_1','type':'signal','options':('no_write_model','write_once')}]

           part_dict={'knob1':5,'chan1':7}

           def init(self,arg):
               knob1=self.knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.chan1=Signal(32,None,42)

    If you need to reference attributes using computed names you can do something like::

        for i in range(16):
            self.__setattr__('signals_channel_%02d' % (i+1,),Signal(...))
    """
    debug = _getenv('DEBUG_DEVICES')
    gtkThread = None

    """ debug safe import """
    if debug:
        if int(debug)<0:
            @staticmethod
            def _debugDevice(dev):
                if not (isinstance(dev,(type,)) and issubclass(dev,(Device,))):
                    return dev
                from types import FunctionType
                def dummy(self,*args,**kvargs):
                    return 1
                db = {}
                for d in dev.mro()[-4::-1]: #mro[-3] is Device
                    for k,v in d.__dict__.items():
                        if isinstance(v,(FunctionType,)):
                            db[k] = dummy
                        else:
                            db[k] = v
                return type(dev.__name__,(Device,),db)
        else:
            @staticmethod
            def _debugDevice(device):
                return device
        @staticmethod
        def _debug(s,p=tuple()):
            from sys import stdout as _stdout
            _stdout.write(s % p)
    else:
        @staticmethod
        def _debug(s,p=tuple()):
            pass
        @staticmethod
        def _debugDevice(device):
            return device
    @staticmethod
    def _mimport(loc,glob,filename,name=None):
        if isinstance(name,(tuple,list)):
            for n in name:
                Device._mimport(loc,glob,filename,n)
            return
        if name is None: name = filename
        Device._debug('loading %-21s',(name+':'))
        try:
            try:
                device = __import__(filename, glob, fromlist=[name], level=1).__getattribute__(name)
            except:
                device = __import__(filename, glob, fromlist=[name]).__getattribute__(name)
            Device._debug(' successful\n')
            loc[name] = Device._debugDevice(device)
        except Exception as exc:
            Device._debug(' failed: %s\n'%exc)
    """ /debug safe import """

    @classmethod
    def __class_init__(cls):
        if not hasattr(cls,'_initialized'):
            cls._initialized=False
            if hasattr(cls,'parts'):
                cls.part_names=list()
                for elt in cls.parts:
                    cls.part_names.append(elt['path'])
            if hasattr(cls,'part_names') and not hasattr(cls,'part_dict'):
                cls.part_dict=dict()
                for i in range(len(cls.part_names)):
                    try:
                        cls.part_dict[cls.part_names[i][1:].lower().replace(':','_').replace('.','_')]=i+1
                    except:
                        pass
            cls._initialized=True

    def __new__(cls,node):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param node: Node of device
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        if cls == Device:
            try:
                head=TreeNode(node.conglomerate_nids.nid_number[0],node.tree)
                model=str(head.record.model)
                return cls.importPyDeviceModule(model).__dict__[model.upper()](head)
            except:
                pass
            raise TypeError("Cannot create instances of Device class")
        else:
            cls.__class_init__();
            return super(Device,cls).__new__(cls,node)

    def __init__(self,node,*tree):
        """Initialize a Device instance
        @param node: Conglomerate node of this device
        @type node: TreeNode
        @rtype: None
        """
        if isinstance(node,TreeNode):
            self.nids = node.conglomerate_nids.nid_number
            if len(self.nids)>0:
                self.head=int(self.nids[0])
                if node.nid>self.head:
                    part_idx = node.nid-self.head-1
                    if part_idx<len(self.part_names):
                        self.__original_node_name = self.part_names[part_idx].upper()
            else:
                self.head = node.nid
            super(Device,self).__init__(node.nid,node.tree)


    def deref(self):
        return self

    def ORIGINAL_PART_NAME(self):
        """Method to return the original part name.
        Will return blank string if part_name class attribute not defined or node used to create instance is the head node or past the end of part_names tuple.
        @return: Part name of this node
        @rtype: str
        """
        if hasattr(self,'__original_node_name'):
            return self.__original_part_name
        return ""
    PART_NAME=ORIGINAL_PART_NAME

    def __getattr__(self,name):
        """Return TreeNode of subpart if name matches mangled node name.
        @param name: part name. Node path with colons and periods replaced by underscores.
        @type name: str
        @return: Device instance of device part
        @rtype: Device
        """
        self.__class_init__()
        if name == 'part_dict': return []
        if name == 'part_name' or name == 'original_part_name':
            return self.ORIGINAL_PART_NAME(None)
        if name in self.part_dict:
            return self.__class__(TreeNode(self.part_dict[name]+self.head,self.tree))
        return super(Device,self).__getattr__(name)

    def __setattr__(self,name,value):
        """Set value into device subnode if name matches a mangled subpart node name. Otherwise assign value to class instance attribute.
        @param name: Name of attribute or device subpart
        @type name: str
        @param value: Value of the attribute or device subpart
        @type value: varied
        @rtype: None
        """
        if name.startswith('_'):
            super(Device,self).__setattr__(name,value)
        else:
            try:
                TreeNode(self.part_dict[name]+self.head,self.tree).record=value
            except KeyError:
                super(Device,self).__setattr__(name,value)

    @classmethod
    def Add(cls,tree,name):
        """Used to add a device instance to an MDSplus tree.
        This method is invoked when a device is added to the tree when using utilities like mdstcl and the traverser.
        For this to work the device class name (uppercase only) and the package name must be returned in the MdsDevices tdi function.
        Also the Device subclass must include the parts attribute which is a list or tuple containing one dict instance per subnode of the device.
        The dict instance should include a 'path' key set to the relative node name path of the subnode. a 'type' key set to the usage string of
        the subnode and optionally a 'value' key or a 'valueExpr' key containing a value to initialized the node or a string containing python
        code which when evaluated during the adding of the device after the subnode has been created produces a data item to store in the node.
        And finally the dict instance can contain an 'options' key which should contain a list or tuple of strings of node attributes which will be turned
        on (i.e. write_once).
        """
        parent = tree
        if isinstance(tree, TreeNode): tree = tree.tree
        cls.__class_init__()
        _TreeShr._TreeStartConglomerate(tree.ctx,len(cls.parts)+1)
        if isinstance(name,_ident.Ident):
            name=name.data()
        head=parent.addNode(name,'DEVICE')
        head=cls(head)
        try:
            import_string="from %s import %s" % (cls.__module__[0:cls.__module__.index('.')],cls.__name__)
        except:
            import_string=None
        head.record=_compound.Conglom('__python__',cls.__name__,None,import_string)
        head.write_once=True
        import MDSplus
        glob = MDSplus.__dict__
        glob['tree'] = tree
        glob['path'] = head.path
        glob['head'] = head
        for elt in cls.parts:  # first add all nodes
            node=head.addNode(elt['path'],elt['type'])
        for elt in cls.parts:  # then you can reference them in valueExpr
            node=head.getNode(elt['path'])
            if 'value' in elt:
                if Device.debug: print(node,node.usage,elt['value'])
                node.record = elt['value']
            elif 'valueExpr' in elt:
                glob['node'] = node
                if Device.debug: print(node,node.usage,elt['valueExpr'])
                node.record = eval(elt['valueExpr'], glob)
            if 'options' in elt:
                for option in elt['options']:
                    node.__setattr__(option,True)
        _TreeShr._TreeEndConglomerate(tree.ctx)
        return head


    def dw_setup(self,*args):
        """Bring up a glade setup interface if one exists in the same package as the one providing the device subclass

        The gtk.main() procedure must be run in a separate thread to avoid locking the main program.
        """
        try:
            from widgets import MDSplusWidget
            import gtk.glade
            import os,gtk,inspect,threading
            import sys
            class gtkMain(threading.Thread):
                def run(self):
                    gtk.main()
            class MyOut:
                def __init__(self):
                    self.content=[]
                def write(self,string):
                    self.content.append(string)

            gtk.gdk.threads_init()
            out=MyOut()
            sys.stdout = out
            sys.stderr = out
            window=gtk.glade.XML(os.path.dirname(inspect.getsourcefile(self.__class__))+os.sep+self.__class__.__name__+'.glade').get_widget(self.__class__.__name__.lower())
            sys.stdout = sys.__stdout__
            sys.stderr = sys.__stderr__
            window.device_node=self
            window.set_title(window.get_title()+' - '+str(self)+' - '+str(self.tree))
            MDSplusWidget.doToAll(window,"reset")
        except Exception as exc:
            print(exc)
            raise Exception("No setup available, %s" % (str(exc),))

        window.connect("destroy",self.onSetupWindowClose)
        window.show_all()
        if Device.gtkThread is None or not Device.gtkThread.isAlive():
            Device.gtkThread=gtkMain()
            Device.gtkThread.start()
        return 1
    DW_SETUP=dw_setup

    def onSetupWindowClose(self,window):
        import gtk
        windows=[toplevel for toplevel in gtk.window_list_toplevels()
                 if toplevel.get_property('type') == gtk.WINDOW_TOPLEVEL]
        if len(windows) == 1:
            gtk.main_quit()

    @classmethod
    def waitForSetups(cls):
        Device.gtkThread.join()

    __cached_py_device_modules = {}
    @staticmethod
    def importPyDeviceModule(name):
        """Find a device support module with a case insensitive lookup of
        'model'.py in the MDS_PYDEVICE_PATH environment variable search list."""
        if name in Device.__cached_py_device_modules:
            return Device.__cached_py_device_modules[name]
        import __builtin__
        import sys,os
        check_name=name.lower()+".py"
        if "MDS_PYDEVICE_PATH" in os.environ:
            path=os.environ["MDS_PYDEVICE_PATH"]
            parts=path.split(';')
            for part in parts:
                w=os.walk(part)
                for dp,dn,fn in w:
                    for fname in fn:
                        if fname.lower() == check_name:
                            sys.path.insert(0,dp)
                            try:
                                ans=__builtin__.__import__(fname[:-3])
                            finally:
                                sys.path.remove(dp)
        Device.__cached_py_device_modules[name] = ans
        return ans

    __cached_py_devices = None
    @staticmethod
    def findPyDevices():
        """Find all device support modules in the MDS_PYDEVICE_PATH environment variable search list."""
        if Device.__cached_py_devices is not None:
            return _data.Data.compile(str(Device.__cached_py_devices))
        ans=list()
        import __builtin__
        import sys,os
        if "MDS_PYDEVICE_PATH" in os.environ:
            path=os.environ["MDS_PYDEVICE_PATH"]
            parts=path.split(';')
            for part in parts:
                for dp,dn,fn in os.walk(part):
                    sys.path.insert(0,dp)
                    for fname in fn:
                        if fname.endswith('.py'):
                            try:
                                devnam=fname[:-3].upper()
                                __builtin__.__import__(fname[:-3]).__dict__[devnam]
                            except:
                                pass
                    sys.path.remove(dp)
        if len(ans) == 0:
            return None
        else:
            Device.__cached_py_devices = ans
            return _data.Data.compile(str(ans))



_scalar=_mimport('mdsscalar')

############### Node Characteristic Options ######
#
TreeNode.NciM_STATE            =0x00000001
TreeNode.NciM_PARENT_STATE     =0x00000002
TreeNode.NciM_ESSENTIAL        =0x00000004
TreeNode.NciM_CACHED           =0x00000008
TreeNode.NciM_VERSIONS         =0x00000010
TreeNode.NciM_SEGMENTED        =0x00000020
TreeNode.NciM_SETUP_INFORMATION=0x00000040
TreeNode.NciM_WRITE_ONCE       =0x00000080
TreeNode.NciM_COMPRESSIBLE     =0x00000100
TreeNode.NciM_DO_NOT_COMPRESS  =0x00000200
TreeNode.NciM_COMPRESS_ON_PUT  =0x00000400
TreeNode.NciM_NO_WRITE_MODEL   =0x00000800
TreeNode.NciM_NO_WRITE_SHOT    =0x00001000
TreeNode.NciM_PATH_REFERENCE   =0x00002000
TreeNode.NciM_NID_REFERENCE    =0x00004000
TreeNode.NciM_INCLUDE_IN_PULSE =0x00008000
TreeNode.NciM_COMPRESS_SEGMENTS=0x00010000

TreeNode.NciK_IS_CHILD         =1
TreeNode.NciK_IS_MEMBER        =2

TreeNode.NciEND_OF_LIST        =(0,_C.c_void_p,4,None)
TreeNode.NciSET_FLAGS          =(1,_C.c_int32,4,_scalar.Uint32)
TreeNode.NciCLEAR_FLAGS        =(2,_C.c_int32,4,_scalar.Uint32)
TreeNode.NciTIME_INSERTED      =(4,_C.c_uint64,8,_scalar.Uint64)
TreeNode.NciOWNER_ID           =(5,_C.c_int32,4,_scalar.Uint8)
TreeNode.NciCLASS              =(6,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciDTYPE              =(7,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciLENGTH             =(8,_C.c_int32,4,_scalar.Int32)
TreeNode.NciSTATUS             =(9,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciCONGLOMERATE_ELT   =(10,_C.c_uint16,2,_scalar.Uint16)
TreeNode.NciGET_FLAGS          =(12,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciNODE_NAME          =(13,_C.c_char_p,13,_scalar.String)
TreeNode.NciPATH               =(14,_C.c_char_p,1024,_scalar.String)
TreeNode.NciDEPTH              =(15,_C.c_int32,4,_scalar.Int32)
TreeNode.NciPARENT             =(16,_C.c_uint32,4,TreeNode)
TreeNode.NciBROTHER            =(17,_C.c_uint32,4,TreeNode)
TreeNode.NciMEMBER             =(18,_C.c_uint32,4,TreeNode)
TreeNode.NciCHILD              =(19,_C.c_uint32,4,TreeNode)
TreeNode.NciPARENT_RELATIONSHIP=(20,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciCONGLOMERATE_NIDS  =(21,_C.c_uint32*1024,1024*4,TreeNodeArray)
TreeNode.NciORIGINAL_PART_NAME =(22,_C.c_char_p,1024,_scalar.String)
TreeNode.NciNUMBER_OF_MEMBERS  =(23,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciNUMBER_OF_CHILDREN =(24,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciMEMBER_NIDS        =(25,_C.c_uint32*4096,4096*4,TreeNodeArray)
TreeNode.NciCHILDREN_NIDS      =(26,_C.c_uint32*4096,4096*4,TreeNodeArray)
TreeNode.NciFULLPATH           =(27,_C.c_char_p,1024,_scalar.String)
TreeNode.NciMINPATH            =(28,_C.c_char_p,1024,_scalar.String)
TreeNode.NciUSAGE              =(29,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciPARENT_TREE        =(30,_C.c_char_p,13,_scalar.String)
TreeNode.NciRLENGTH            =(31,_C.c_int32,4,_scalar.Int32)
TreeNode.NciNUMBER_OF_ELTS     =(32,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciDATA_IN_NCI        =(33,_C.c_bool,4,bool)
TreeNode.NciERROR_ON_PUT       =(34,_C.c_uint32,4,bool)
TreeNode.NciRFA                =(35,_C.c_uint64,8,_scalar.Uint64)
TreeNode.NciIO_STATUS          =(36,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciIO_STV             =(37,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciDTYPE_STR          =(38,_C.c_char_p,64,_scalar.String)
TreeNode.NciUSAGE_STR          =(39,_C.c_char_p,64,_scalar.String)
TreeNode.NciCLASS_STR          =(40,_C.c_char_p,64,_scalar.String)
TreeNode.NciVERSION            =(41,_C.c_uint32,4,_scalar.Uint32)
#
#################################################################

############# dtype to classes ##################################
#
_descriptor.dtypeToClass[TreeNode.dtype_id]=TreeNode
_descriptor.dtypeToClass[TreePath.dtype_id]=TreePath
