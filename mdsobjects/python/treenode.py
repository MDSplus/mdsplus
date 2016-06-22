def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import sys as _sys

_mdsshr=_mimport('_mdsshr')
_array=_mimport('mdsarray')
_compound=_mimport('compound')
_data=_mimport('mdsdata')
_Exceptions=_mimport('mdsExceptions')
_scalar=_mimport('mdsscalar')
_tree=_mimport('tree')
_treeshr=_mimport('_treeshr')
_mdsdcl=_mimport('mdsdcl')
_ver=_mimport('version')



class TreeNodeException(_Exceptions.MDSplusException):
  pass

_nciAttributes = ('BROTHER','CACHED','CHILD','CHILDREN_NIDS','MCLASS','CLASS_STR',
                 'COMPRESSIBLE','COMPRESS_ON_PUT','CONGLOMERATE_ELT','CONGLOMERATE_NIDS',
                 'DATA_IN_NCI','DEPTH','DISABLED','DO_NOT_COMPRESS','DTYPE','DTYPE_STR',
                 'ERROR_ON_PUT','ESSENTIAL','FULLPATH','GET_FLAGS','INCLUDE_IN_PULSE',
                 'IO_STATUS','IO_STV','IS_CHILD','IS_MEMBER','LENGTH','MEMBER','MEMBER_NIDS',
                 'MINPATH','NID_NUMBER','NID_REFERENCE','NODE_NAME','NO_WRITE_MODEL',
                 'NO_WRITE_SHOT','NUMBER_OF_CHILDREN','NUMBER_OF_ELTS','NUMBER_OF_MEMBERS','ON',
                 'ORIGINAL_PART_NAME','OWNER_ID','PARENT','PARENT_DISABLED','PARENT_RELATIONSHIP',
                 'PARENT_STATE','PATH','PATH_REFERENCE','RECORD','RFA','RLENGTH','SEGMENTED',
                 'SETUP_INFORMATION','STATE','STATUS','TIME_INSERTED','USAGE','USAGE_ANY',
                 'USAGE_AXIS','USAGE_COMPOUND_DATA','USAGE_DEVICE','USAGE_DISPATCH','USAGE_NONE',
                 'USAGE_NUMERIC','USAGE_STR','USAGE_STRUCTURE','USAGE_SUBTREE','USAGE_TASK',
                 'USAGE_TEXT','USAGE_WINDOW','VERSIONS','WRITE_ONCE','COMPRESS_SEGMENTS')

_usage_table={'ANY':0,'NONE':1,'STRUCTURE':1,'ACTION':2,'DEVICE':3,'DISPATCH':4,'NUMERIC':5,'SIGNAL':6,
             'TASK':7,'TEXT':8,'WINDOW':9,'AXIS':10,'SUBTREE':11,'COMPOUND_DATA':12,'SUBTREE':-1}

class TreeNode(_data.Data):
    """Class to represent an MDSplus node reference (nid).
    @ivar nid: node index of this node.
    @type nid: int
    @ivar tree: Tree instance that this node belongs to.
    @type tree: Tree
    """

    def __new__(cls,nid,tree=None):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param node: Node of device
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        _mdsdevice=_mimport('mdsdevice')
        node = super(TreeNode,cls).__new__(cls)
        if not isinstance(node,_mdsdevice.Device):
            try:
                TreeNode.__init__(node,nid,tree=tree)
                if node.usage == "DEVICE":
                    model=str(node.record.model)
                    return _mdsdevice.Device.importPyDeviceModule(model).__dict__[model.upper()](node)
            except:
                pass
        return node

    def __init__(self,n,tree=None):
        """Initialze TreeNode
        @param n: Index of the node in the tree.
        @type n: int
        @param tree: Tree associated with this node
        @type tree: Tree
        """
        self.__dict__['nid']=int(n);
        if tree is None:
            self.tree=_tree.Tree()
        else:
            self.tree=tree

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
            except:
                ans=TreePath(path,tree)
            return ans

    def __getattr__(self,name):
        """
        Implements value=node.attribute

        Attributes defined:

         - brother               - TreeNode,Next node in tree
         - child                 - TreeNode,First ancestor
         - children_nids         - TreeNodeArray, Children nodes
         - mclass                - Uint8, Internal numeric MDSplus descriptor class
         - class_str             - String, Name of MDSplus descriptor class
         - compressible          - Bool, Flag indicating node contains compressible data
         - compress_on_put       - Bool, Flag indicating automatic compression
         - compress_segments     - Bool, Flag indicating compression for segmented data
         - conglomerate_elt      - Int32, Index of node in a conglomerate
         - conglomerate_nids     - TreeNodeArray, Nodes in same conglomerate
         - data_in_nci           - Uint32, Flag indicating data stored in nci record
         - descendants           - TreeNodeArray, One level descendants of this node
         - depth                 - Int32, Level of node from the top node of the tree
         - disabled              - Bool, Opposite of on flag
         - do_not_compress       - Bool, Flag indicating data should not be compressed
         - dtype                 - Uint8, Internal numeric MDSplus data type
         - dtype_str             - String, Name of MDSplus data type
         - essential             - Bool, Flag indicating node is essential, used for actions
         - fullpath              - String, Full absolute path of node in tree
         - include_in_pulse      - Bool, Flag indicating node should be included in pulse
         - is_child              - Bool, Flag indicating node is a child node
         - is_member             - Bool, Flag indicating node is a member bnode
         - length                - Int32, Length of data in bytes (uncompressed)
         - local_path            - str, path to node relative to top of tree containing this node
         - local_tree            - str, Name of tree containing this node
         - member                - TreeNode, First member of this node
         - member_nids           - TreeNodeArray, Members nodes
         - minpath               - String, Minimum length string representing path to node
         - nid_number            - Int32, Internal index to node in tree
         - nid_reference         - Bool, Flag indicating that data contains references to nodes
         - node_name             - String, This nodes name
         - no_write_model        - Bool, Flag indicating that data cannot be written into model
         - no_write_shot         - Bool, Flag indicating that data cannot be written into shot
         - number_of_children    - Int32, Number of children of this node
         - number_of_descentants - Int32, Numer of first level descendants
         - number_of elts        - Int32, Number of nodes in this conglomerate
         - number_of_members     - Int32, Number of members of this node
         - on                    - Bool, Flag indicating if this node is turned on
         - original_part_name    - String, Original node name when conglomerate was created
         - owner_id              - Int32, Id/gid of account which stored the data
         - parent                - TreeNode, Parent of this node
         - parent_disabled       - Bool, Flag indicating parent is turned off
         - path                  - String, Path to this node
         - path_reference        - Bool, Flag indicating that data contains references to nodes
         - record                - Data, Contents of this node
         - rfa                   - Int64, Byte offset to this node
         - rlength               - Int32, Data length in bytes (compressed)
         - segmented             - Bool, Flag indicating node contains segmented records
         - setup_information     - Bool, Flag indicating node contains setup information
         - status                - Int32, Completion status of action
         - tags                  - StringArray, List of tagnames for this node
         - time_inserted         - Uint64, Time data was inserted
         - usage                 - String, Usage of this node
         - versions              - Bool, Flag indicating that data contains versions
         - write_once            - Bool, Flag indicating that data can only be written if node is empty.
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
        name = name.lower()
        if name in self.__dict__.keys():
            return self.__dict__[name]
        if name == 'nid':
            try:
                return self.tree.getNode(self.tree_path)
            except:
                return None
        if name == 'record':
            return self.getData()
        if name == 'tags':
            return self.getTags()
        if name == 'usage':
            return _scalar.String(self.usage_str.value[10:])
        if name == 'descendants':
            return self.getDescendants()
        if name == 'number_of_descendants':
            return self.number_of_members+self.number_of_children
        if name == 'segmented':
            return self.getNumSegments().value > 0
        if name == 'local_tree':
            return self.getLocalTree()
        if name == 'local_path':
            return self.getLocalPath()
        if name.upper() in _nciAttributes:
            if name == 'mclass':
                name='class';
            _tree.Tree.lock()
            try:
                self.restoreContext()
                try:
                    ans = _data.Data.execute('getnci($,$)',self,name)
                    if isinstance(ans,_scalar.Uint8):
                        if name not in ('class','dtype'):
                            ans = bool(ans)
                    if isinstance(ans,(TreeNodeArray,TreeNode)):
                        ans.tree=self.tree
                except _Exceptions.TreeNNF:
                  ans = None
            finally:
                _tree.Tree.unlock()
            if isinstance(ans,_scalar.String):
                return _scalar.String(ans.rstrip())
            return ans
        raise AttributeError('Attribute %s is not defined' % (name,))

    def __setattr__(self,name,value):
        """
        Implements node.attribute=value

        Attributes which can be set:

         - compress_on_put  - Bool, Flag indicating whether data should be compressed when written
         - do_not_compress  - Bool, Flag to disable compression
         - essential        - Bool, Flag indicating successful action completion required
         - include_in_pulse - Bool, Flag to include in pulse
         - no_write_model   - Bool, Flag to disable writing in model
         - no_write_shot    - Bool, Flag to disable writing in pulse file
         - record           - Data, Data contents of node
         - subtree          - Bool, Set node to be subtree or not (edit mode only)
         - tag              - str, Add tag to node (edit mode only)
         - write_once       - Bool, Flag to only allow writing to empty node
        @param name: Name of attribute
        @type name: str
        @param value: Value for attribute
        @type value: various
        @rtype: None
        """
        name = name.lower()
        if name=="record":
            self.putData(value)
        elif name=="on":
            self.setOn(value)
        elif name=="disabled":
            self.setOn(not value)
        elif name=="no_write_model":
            self.setNoWriteModel(value)
        elif name=="no_write_shot":
            self.setNoWriteShot(value)
        elif name=="write_once":
            self.setWriteOnce(value)
        elif name=="include_in_pulse":
            self.setIncludeInPulse(value)
        elif name=="compress_on_put":
            self.setCompressOnPut(value)
        elif name=="compress_segments":
            self.setCompressSegments(value)
        elif name=="do_not_compress":
            self.setDoNotCompress(value)
        elif name=="essential":
            self.setEssential(value)
        elif name=="subtree":
            self.setSubtree(value)
        elif name=="usage":
            self.setUsage(value)
        elif name=="tag":
            self.addTag(value)
        elif name.upper() in _nciAttributes:
            raise AttributeError('Attribute %s is read only' %(name,))
        elif name == "nid":
            raise AttributeError('Attribute nid is read only')
        else:
            self.__dict__[name]=value

    def __setNode(self,qualifier,flag,reversed=False):
        """For internal use only"""
        if flag is True or flag is False:
            if reversed is True:
                flag = not flag
        if flag is True:
            switch="/"
        else:
            if flag is False:
                switch="/no"
            else:
                raise TypeError('Argument must be True or False')
        _tree.Tree.lock()
        try:
            self.restoreContext()
            cmd='set node %s%s%s'% (self.fullpath,switch,qualifier)
            _mdsdcl.tcl(cmd,raise_exception=True)
        finally:
            _tree.Tree.unlock()

    def __str__(self):
        """Convert TreeNode to string."""
        if self.nid is None:
            ans="NODEREF(*)"
        else:
            ans=_treeshr.TreeGetPath(self)
        return _ver.tostr(ans)

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

    def addTag(self,tag):
        """Add a tagname to this node
        @param tag: tagname for this node
        @type tag: str
        @rtype: original type
        """
        _treeshr.TreeAddTag(self.tree,self.nid,str(tag))
        return self

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
        _treeshr.TreeBeginSegment(self,start,end,dimension,initialValueArray,idx)

    def beginTimestampedSegment(self,array,idx=-1):
        """Allocate space for a timestamped segment
        @param array: Initial data array to define shape of segment
        @type array: Array
        @param idx: Optional segment index. Defaults to -1 meaning next segment.
        @type idx: int
        @rtype: None
        """
        _treeshr.TreeBeginTimestampedSegment(self,array,idx)

    def compare(self,value):
        """Returns True if this node contains the same data as specified in the value argument
        @param value: Value to compare contents of the node with
        @type value: Data
        @rtype: Bool
        """
        try:
            oldval=self.record
        except:
            oldval=None
        status = _mdsshr.MdsCompareXd(_data.makeData(oldval),_data.makeData(value))
        if status == 1:
            return True
        else:
            return False

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

    def dir(self):
        """list descendants"""
        for desc in self.getDescendants():
            print('%-12s    %s'%(desc.getNodeName(),desc.getUsage()))

    def dispatch(self,wait=True):
        """Dispatch an action node
        @rtype: None
        """
        a=self.record
        if not isinstance(a,_compound.Action):
            raise TreeNodeException("Node does not contain an action description")
        else:
            if wait:
                status=_data.Data.execute("tcl('dispatch/wait "+str(self.fullpath).replace('\\','\\\\')+"')")
            else:
                status=_data.Data.execute("tcl('dispatch/nowait "+str(self.fullpath).replace('\\','\\\\')+"')")
            if not (status & 1):
                raise TreeNodeException(_mdsshr.MdsGetMsg(status,"Error dispatching node"))

    def doMethod(self,method,*args):
        """Execute method on conglomerate element
        @param method: method name to perform
        @type method: str
        @param arg: Optional argument for method
        @type arg: Data
        @rtype: None
        """
        _tree.Tree.lock()
        try:
            self.restoreContext()
            return _treeshr.TreeDoMethod(self,str(method),*args)
        finally:
            _tree.Tree.unlock()

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
        try:
            children = self.children_nids
        except:
            children = None
        if children is None:
            children = TreeNodeArray(_array.Int32Array([]),self.tree)
        return children

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

    def getData(self, *altvalue):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        return _treeshr.TreeGetRecord(self, *altvalue)

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
        try:
            members = self.member_nids
        except:
            members = None
        try:
            children = self.children_nids
        except:
            children = None
        if members is None and children is None:
            ans = TreeNodeArray(_array.Int32Array([]),self.tree)
        elif members is None:
            ans = children
        elif children is None:
            ans = members
        else:
            nids = members.data().tolist()+children.data().tolist()
            ans  = TreeNodeArray(_array.Int32Array(nids),self.tree)
        return ans

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
        try:
            member = self.member_nids
        except:
            member = None
        if member is None:
            member = TreeNodeArray(_array.Int32Array([]),self.tree)
        return member

    def getMinPath(self):
        """Return shortest path string for this node
        @return: shortest path designation depending on the current node default and whether the node has tag names or not.
        @rtype: String
        """
        return self.minpath

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
        return self.number_of_descendants

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
        return _data.makeData(_treeshr.TreeGetNumSegments(self))

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
            return _treeshr.TreeGetSegment(self,idx)
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

    def getSegmentEnd(self,idx):
        """return end of segment
        @param idx: segment index to query
        @type idx: int
        @rtype: Data
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            limits=_treeshr.TreeGetSegmentLimits(self,idx)
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
            limits=_treeshr.TreeGetSegmentLimits(self,idx)
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
        _tree.Tree.lock()
        try:
            ans=_treeshr.TreeFindNodeTags(self)
        finally:
            _tree.Tree.unlock()
        return ans

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
        return self.usage

    def isChild(self):
        """Return true if this is a child node
        @return: True if this is a child node instead of a member node.
        @rtype: bool
        """
        return self.is_child

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

    def isDoNotCompress(self):
        """Return true of compression is disabled for this node
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
        return self.is_member

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
        return self.on

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
        return self.setup_information

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
        _treeshr.TreeMakeSegment(self,start,end,dimension,valueArray,idx)

    def move(self,parent,newname=None):
        """Move node to another location in the tree and optionally rename the node
        @param parent: New parent of this node
        @type parent: TreeNode
        @param newname: Optional new node name of this node. 1-12 characters, no path delimiters.
        @type newname: str
        @rtype: None
        """
        if newname is None:
            newname=str(self.node_name)
        if self.usage=='SUBTREE' or self.usage=='STRUCTURE':
            _treeshr.TreeRenameNode(self,str(parent.fullpath)+"."+newname)
        else:
            _treeshr.TreeRenameNode(self,str(parent.fullpath)+":"+newname)

    def putData(self,data):
        """Store data
        @param data: Data to store in this node.
        @type data: Data
        @rtype: original type
        """
        _tree.Tree.lock()
        try:
            if isinstance(data,_data.Data) and data.__hasBadTreeReferences__(self.tree):
                data=data.__fixTreeReferences__(self.tree)
            _treeshr.TreePutRecord(self,data)
        finally:
            _tree.Tree.unlock()
        return self

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
        _treeshr.TreePutRow(self,bufsize,array,timestamp)

    def putSegment(self,data,idx):
        """Load a segment in a node
        @param data: data to load into segment
        @type data: Array or Scalar
        @param idx: index into segment to load data
        @type idx: int
        @rtype: None
        """
        _treeshr.TreePutSegment(self,data,idx)

    def putTimestampedSegment(self,timestampArray,array):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @rtype: None
        """
        _treeshr.TreePutTimestampedSegment(self,timestampArray,array)

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
        _treeshr.TreeMakeTimestampedSegment(self,timestampArray,array,idx,rows_filled)

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
        except _Exceptions.TreeNNF:
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
            olddefault=self.tree.default
            self.tree.setDefault(self.parent)
            if self.isChild():
                newname="."+_ver.tostr(newname)
            _treeshr.TreeRenameNode(self,newname)
        finally:
            self.tree.setDefault(olddefault)
        return self

    def restoreContext(self):
        """Restore tree context. Used by internal functions.
        @rtype: None
        """
        if self.tree is not None:
            self.tree.restoreContext()

    def setCompressOnPut(self,flag):
        """Set compress on put state of this node
        @param flag: State to set the compress on put characteristic
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('compress_on_put',flag)
        return self

    def setCompressSegments(self,flag):
        """Set compress segments state of this node
        @param flag: State to set the compress segments characteristic
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('compress_segments',flag)
        return self

    def setDoNotCompress(self,flag):
        """Set do not compress state of this node
        @param flag: True do disable compression, False to enable compression
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('do_not_compress',flag)
        return self

    def setEssential(self,flag):
        """Set essential state of this node
        @param flag: State to set the essential characteristic. This is used on action nodes when phases are dispacted.
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('essential',flag)
        return self

    def setIncludeInPulse(self,flag):
        """Set include in pulse state of this node
        @param flag: State to set the include in pulse characteristic. If true and this node is the top node of a subtree the subtree will be included in the pulse.
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('included',flag)
        return self

    def setNoWriteModel(self,flag):
        """Set no write model state for this node
        @param flag: State to set the no write in model characteristic. If true then no data can be stored in this node in the model.
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('model_write',not flag)
        return self

    def setNoWriteShot(self,flag):
        """Set no write shot state for this node
        @param flag: State to set the no write in shot characteristic. If true then no data can be stored in this node in a shot file.
        @type flag: bool
        @rtype: original type
        """
        self.__setNode('shot_write',not flag)
        return self

    def setOn(self,flag):
        """Turn node on or off
        @param flag: State to set the on characteristic. If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: original type
        """
        _tree.Tree.lock()
        try:
            if flag is True:
                _treeshr.TreeTurnOn(self)
            else:
                if flag is False:
                    _treeshr.TreeTurnOff(self)
                else:
                    raise TypeError('argument must be True or False')
        finally:
            _tree.Tree.unlock()
        return self


    def setSubtree(self,flag):
        """Enable/Disable node as a subtree
        @param flag: True to make node a subtree reference. Node must be a child node with no descendants.
        @type flag: bool
        @rtype: original type
        """
        _treeshr.TreeSetSubtree(self,flag)
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
        _treeshr.TreeSetUsage(self.tree.ctx,self.nid,usagenum)
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
        self.__setNode('write_once',flag)
        return self

    def updateSegment(self,start,end,dim,idx):
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
        _treeshr.TreeUpdateSegment(self,start,end,dim,idx)

class TreePath(TreeNode):
    """Class to represent an MDSplus node reference (path)."""
    def __init__(self,path,tree=None):
        self.tree_path=_data.makeData(str(path));
        if tree is None:
            self.tree=_tree.Tree()
        else:
            self.tree=tree
        return

    def __hasBadTreeReferences__(self,tree):
       return False


    def __str__(self):
        """Convert path to string."""
        return self.tree_path.value

class TreeNodeArray(_data.Data):
    def __init__(self,nids,tree=None):
        self.nids=_array.Int32Array(nids)
        if tree is None:
            self.tree=_tree.Tree()
        else:
            self.tree=tree

    def __getitem__(self,n):
        """Return TreeNode from mdsarray. array[n]
        @param n: Index into array beginning with index 0
        @type n: int
        @return: node
        @rtype: TreeNode
        """
        return TreeNode(self.nids[n],self.tree)

    def restoreContext(self):
        self.tree.restoreContext()

    def getPath(self):
        """Return StringArray of node paths
        @return: Node names
        @rtype: StringArray
        """
        return self.path

    def getFullPath(self):
        """Return StringArray of full node paths
        @return: Full node paths
        @rtype: StringArray
        """
        return self.fullpath

    def getNid(self):
        """Return nid numbers
        @return: nid numbers
        @rtype: Int32Array
        """
        return self.nids

    def isOn(self):
        """Return bool array
        @return: true if node is on
        @rtype: Int8Array
        """
        return self.on

    def setOn(self,flag):
        """Turn nodes on or off
        @param flag: True to turn nodes on, False to turn them off
        @type flag: Bool
        @rtype: None
        """
        for nid in self:
            nid.setOn(flag)

    def getLength(self):
        """Return data lengths
        @return: Array of data lengths
        @rtype: Int32Array
        """
        return self.length

    def getCompressedLength(self):
        """Return compressed data lengths
        @return: Array of compressed data lengths
        @rtype: Int32Array
        """
        return self.rlength

    def isSetup(self):
        """Return array of bool
        @return: True if setup information
        @rtype: Uint8Array
        """
        return self.setup

    def isWriteOnce(self):
        """Return array of bool
        @return: True if node is write once
        @rtype: Uint8Array
        """
        return self.write_once


    def setWriteOnce(self,flag):
        """Set nodes write once
        @rtype: original type
        """
        for nid in self:
            nid.setWriteOnce(flag)
        return self

    def isCompressOnPut(self):
        """Is nodes set to compress on put
        @return: state of compress on put flags
        @rtype: Uint8Array
        """
        return self.compress_on_put

    def setCompressOnPut(self,flag):
        """Set compress on put flag
        @param flag: True if compress on put, False if not
        @type flag: bool
        """
        for nid in self:
            nid.setCompressOnPut(flag)
        return self

    def isNoWriteModel(self):
        """True if nodes set to no write model
        @return: True if nodes set to no write model mode
        @rtype: Uint8Array
        """
        return self.no_write_model

    def setNoWriteModel(self,flag):
        """Set no write model flag
        @param flag: True to disallow writing to model
        @type flag: bool
        @rtype: original type
        """
        for nid in self:
            nid.setNoWriteModel(flag)
        return self

    def isNoWriteShot(self):
        """True if nodes are set no write shot
        @return: 1 of set no write shot
        @rtype: Uint8Array()
        """
        return self.no_write_shot

    def setNoWriteShot(self,flag):
        """set no write shot flags
        @param flag: True if setting no write shot
        @type flag: bool
        @rtype: original type
        """
        for nid in self:
            nid.setNoWriteShot(flag)
        return self

    def getUsage(self):
        """Get usage of nodes
        @return: Usage
        @rtype: StringArray
        """
        a=list()
        for nid in self:
            a.append(str(nid.usage))
        return _array.makeArray(a)

    def __getattr__(self,name):
        _tree.Tree.lock()
        try:
            try:
                self.restoreContext()
                ans = _data.Data.execute('getnci($,$)',self.nids,name)
            except Exception:
                if 'BAD_INDEX' in str(_sys.exc_info()[1]):
                    raise AttributeError('Attribute %s is not defined' % (name,))
                else:
                    raise
        finally:
            _tree.Tree.unlock()
        return ans
