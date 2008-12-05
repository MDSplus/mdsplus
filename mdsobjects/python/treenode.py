from data import Data,makeData
from scalar import Uint8
from array import Int32Array
from tree import Tree
nciAttributes = ('BROTHER','CACHED','CHILD','CHILDREN_NIDS','MCLASS','CLASS_STR',
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
                     'USAGE_TEXT','USAGE_WINDOW','VERSIONS','WRITE_ONCE')

class TreeNode(Data):
    """Class to represent an MDSplus node reference (nid).
    @ivar nid: node index of this node.
    @type nid: int
    @ivar tree: Tree instance that this node belongs to.
    @type tree: Tree
    """

    def __init__(self,n,tree=None):
        """Initialze TreeNode
        @param n: Index of the node in the tree.
        @type n: int
        @param tree: Tree associated with this node
        @type tree: Tree
        """
        self.__dict__['nid']=int(n);
        if tree is None:
            self.tree=Tree._activeTree
        else:
            self.tree=tree
    
    def __str__(self):
        """Convert TreeNode to string."""
        from _treeshr import TreeGetPath
        if self.nid is None:
            ans="NODEREF(*)"
        else:
            ans=TreeGetPath(self)
        return ans

    def __getattr__(self,name):
        """
        Attributes defined:

        brother               - TreeNode,Next node in tree
        child                 - TreeNode,First ancestor
        children_nids         - TreeNodeArray, Children nodes
        mclass                - Uint8, Internal numeric MDSplus descriptor class
        class_str             - String, Name of MDSplus descriptor class
        compressible          - Bool, Flag indicating node contains compressible data
        compress_on_put       - Bool, Flag indicating automatic compression
        conglomerate_elt      - Int32, Index of node in a conglomerate
        conglomerate_nids     - TreeNodeArray, Nodes in same conglomerate
        data_in_nci           - Uint32, Flag indicating data stored in nci record
        descendants           - TreeNodeArray, One level descendants of this node
        depth                 - Int32, Level of node from the top node of the tree
        disabled              - Bool, Opposite of on flag
        do_not_compress       - Bool, Flag indicating data should not be compressed
        dtype                 - Uint8, Internal numeric MDSplus data type
        dtype_str             - String, Name of MDSplus data type
        essential             - Bool, Flag indicating node is essential, used for actions
        fullpath              - String, Full absolute path of node in tree
        include_in_pulse      - Bool, Flag indicating node should be included in pulse
        is_child              - Bool, Flag indicating node is a child node
        is_member             - Bool, Flag indicating node is a member bnode
        length                - Int32, Length of data in bytes (uncompressed)
        member                - TreeNode, First member of this node
        member_nids           - TreeNodeArray, Members nodes
        minpath               - String, Minimum length string representing path to node
        nid_number            - Int32, Internal index to node in tree
        nid_reference         - Bool, Flag indicating that data contains references to nodes
        node_name             - String, This nodes name
        no_write_model        - Bool, Flag indicating that data cannot be written into model
        no_write_shot         - Bool, Flag indicating that data cannot be written into shot
        number_of_children    - Int32, Number of children of this node
        number_of_descentants - Int32, Numer of first level descendants
        number_of elts        - Int32, Number of nodes in this conglomerate
        number_of_members     - Int32, Number of members of this node
        on                    - Bool, Flag indicating if this node is turned on
        original_part_name    - String, Original node name when conglomerate was created
        owner_id              - Int32, Id/gid of account which stored the data
        parent                - TreeNode, Parent of this node
        parent_disabled       - Bool, Flag indicating parent is turned off
        path                  - String, Path to this node
        path_reference        - Bool, Flag indicating that data contains references to nodes
        record                - Data, Contents of this node
        rfa                   - Int64, Byte offset to this node
        rlength               - Int32, Data length in bytes (compressed)
        segmented             - Bool, Flag indicating node contains segmented records
        setup_information     - Bool, Flag indicating node contains setup information
        status                - Int32, Completion status of action
        tags                  - StringArray, List of tagnames for this node
        time_inserted         - Uint64, Time data was inserted
        usage                 - String, Usage of this node
        versions              - Bool, Flag indicating that data contains versions
        write_once            - Bool, Flag indicating that data can only be written if node is empty."""

        from scalar import String
        if name.lower() == 'nid':
            return self.tree.getNode(str(self))
        if name.lower() == 'tags':
            return self.getTags()
        if name.lower() == 'usage':
            return String(self.usage_str.value[10:])
        if name.lower() == 'descendants':
            return self.getDescendants()
        if name.lower() == 'number_of_descendants':
            return self.number_of_members+self.number_of_children
        if name.lower() == 'segmented':
            return self.getNumSegments().value > 0
        if name.upper() in nciAttributes:
            from _tdishr import TdiException
            try:
                if name.lower() == 'mclass':
                    name='class';
                Tree.lock()
                self.restoreContext()
                try:
                    ans = Data.execute('getnci($,$)',self,name)
                    if isinstance(ans,Uint8):
                        if name not in ('class','dtype'):
                            ans = bool(ans)
                except TdiException,e:
                    if 'TreeNNF' in str(e):
                        ans=None
                    else:
                        raise
            finally:
                Tree.unlock()
            if isinstance(ans,String):
                return String(ans.rstrip())
            return ans
        raise AttributeError,'Attribute %s is not defined' % (name,)

    def __setattr__(self,name,value):
        """

        Attributes which can be set:

        compress_on_put  - Bool, Flag indicating whether data should be compressed when written
        do_not_compress  - Bool, Flag to disable compression
        essential        - Bool, Flag indicating successful action completion required
        include_in_pulse - Bool, Flag to include in pulse
        no_write_model   - Bool, Flag to disable writing in model
        no_write_shot    - Bool, Flag to disable writing in pulse file
        record           - Data, Data contents of node
        write_once       - Bool, Flag to only allow writing to empty node
        """
        if name.upper()=="RECORD":
            self.putData(value)
            return
        if name.upper()=="ON":
            self.setOn(value)
            return
        if name.upper()=="NO_WRITE_MODEL":
            self.setNoWriteModel(value)
            return
        if name.upper()=="NO_WRITE_SHOT":
            self.setNoWriteShot(value)
            return
        if name.upper()=="WRITE_ONCE":
            self.setWriteOnce(value)
            return
        if name.upper()=="INCLUDE_IN_PULSE":
            self.setIncludeInPulse(value)
            return
        if name.upper()=="COMPRESS_ON_PUT":
            self.setCompressOnPut(value)
            return
        if name.upper()=="DO_NOT_COMPRESS":
            self.setDoNotCompress(value)
            return
        if name.upper()=="ESSENTIAL":
            self.setEssential(value)
            return
        if name.upper() in nciAttributes:
            raise AttributeError,'Attribute %s is read only' %(name,)
        if name.lower() == "nid":
            raise AttributeError,'Attribute nid is read only'
        else:
            self.__dict__[name]=value

    def restoreContext(self):
        """Restore tree context. Used by internal functions."""
        self.tree.restoreContext()

    def getClass(self):
        """Return MDSplus class name of this node
        @return: MDSplus class name of the data stored in this node.
        @rtype: String
        """
        return self.class_str

    def isCompressOnPut(self):
        """Return true if node is set to compress on put
        @return: True if compress on put
        @rtype: bool
        """
        return self.compress_on_put

    def setCompressOnPut(self,flag):
        """Set compress on put state of this node
        @param flag: State to set the compress on put characteristic
        @type flag: bool
        """
        self.__setNode('compress_on_put',flag)

    def getConglomerateElt(self):
        """Return index of this node in a conglomerate
        @return: element index of this node in a conglomerate. 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.conglomerate_elt
    
    def getNumElts(self):
        """Return number of nodes in this conglomerate
        @return: Number of nodes in this conglomerate or 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.number_of_elts

    def getConglomerateNodes(self):
        """Return TreeNodeArray of conglomerate elements
        @return: Nodes in this conglomerate.
        @rtype: TreeNodeArray
        """
        return self.conglomerate_nids

    def getOriginalPartName(self):
        """Return the original part name of node in conglomerate
        @return: Original part name of this node when conglomerate was first instantiated.
        @rtype: String
        """
        return self.original_part_name

    def getDtype(self):
        """Return the name of the data type stored in this node
        @return: MDSplus data type name of data stored in this node.
        @rtype: String
        """
        return self.dtype_str
    
    def isEssential(self):
        """Return true if successful action completion is essential
        @return: True if this node is marked essential.
        @rtype: bool
        """
        return self.essential

    def setEssential(self,flag):
        """Set essential state of this node
        @param flag: State to set the essential characteristic. This is used on action nodes when phases are dispacted.
        @type flag: bool
        """
        return self.__setNode('essential',flag)

    def getFullPath(self):
        """Return full path of this node
        @return: full path specification of this node.
        @rtype: String
        """
        return self.fullpath

    def getMinPath(self):
        """Return shortest path string for this node
        @return: shortest path designation depending on the current node default and whether the node has tag names or not.
        @rtype: String
        """
        return self.minpath
    
    def getPath(self):
        """Return path of this node
        @return: Path to this node.
        @rtype: String
        """
        return self.path

    def getNodeName(self):
        """Return node name
        @return: Node name of this node. 1 to 12 characters
        @rtype: String
        """
        return self.node_name
    
    def isIncludedInPulse(self):
        """Return true if this subtree is to be included in pulse file
        @return: True if subtree is to be included in pulse file creation.
        @rtype: bool
        """
        return self.include_in_pulse

    def setIncludedInPulse(self,flag):
        """Set include in pulse state of this node
        @param flag: State to set the include in pulse characteristic. If true and this node is the top node of a subtree the subtree will be included in the pulse.
        @type flag: bool
        """
        return self.__setNode('include_in_pulse',flag)

    def getDepth(self):
        """Get depth of this node in the tree
        @return: number of levels between this node and the top of the currently opened tree.
        @rtype: Int32
        """
        return self.depth

    def isChild(self):
        """Return true if this is a child node
        @return: True if this is a child node instead of a member node.
        @rtype: bool
        """
        return self.is_child

    def getChild(self):
        """Return first child of this node.
        @return: Return first child of this node or None if it has no children.
        """
        return self.child

    def getNumChildren(self):
        """Return number of children nodes.
        @return: Number of children
        @rtype: Int32
        """
        return self.number_of_children

    def getChildren(self):
        """Return TreeNodeArray of children nodes.
        @return: Children of this node
        @rtype: TreeNodeArray
        """
        return self.children_nids

    def isMember(self):
        """Return true if this is a member node"""
        return self.is_member

    def getMember(self):
        """Return first member node"""
        return self.member

    def getNumMembers(self):
        """Return number of members"""
        return self.number_of_members

    def getMembers(self):
        """Return TreeNodeArray of this nodes members"""
        return self.member_nids

    def getNumDescendants(self):
        """Return number of first level descendants (children and members)"""
        return self.number_of_descendants

    def getDescendants(self):
        """Return TreeNodeArray of first level descendants (children and members)."""
        try:
            members=self.member_nids
            try:
                children=self.children_nids
                ans=list()
                for i in range(len(members)):
                    ans.append(members.nids[i].value)
                for i in range(len(children)):
                    ans.append(children.nids[i].value)
                return TreeNodeArray(Int32Array(ans))
            except:
                return members
        except:
            try:
                return self.children_nids
            except:
                return None
            
    def getParent(self):
        """Return parent of this node"""
        return self.parent

    def getBrother(self):
        """Return sibling of this node"""
        return self.brother

    def getLength(self):
        """Return uncompressed data length of this node"""
        return self.length

    def getCompressedLength(self):
        """Return compressed data length of this node"""
        return self.rlength

    def getNid(self):
        """Return node index"""
        return self.nid

    def isNoWriteModel(self):
        """Return true if data storage to model is disabled for this node"""
        return self.no_write_model

    def setNoWriteModel(self,flag):
        """Set no write model state for this node
        @param flag: State to set the no write in model characteristic. If true then no data can be stored in this node in the model.
        @type flag: bool
        """
        self.__setNode('no_write_model',flag)
        return

    def isNoWriteShot(self):
        """Return true if data storage to pulse file is disabled for this node"""
        return self.no_write_shot

    def setNoWriteShot(self,flag):
        """Set no write shot state for this node
        @param flag: State to set the no write in shot characteristic. If true then no data can be stored in this node in a shot file.
        @type flag: bool
        """
        self.__setNode('no_write_shot',flag)
        return

    def isWriteOnce(self):
        """Return true if node is set write once"""
        return self.write_once

    def setWriteOnce(self,flag):
        """Set write once state of node
        @param flag: State to set the write once characteristic. If true then data can only be written if the node is empty.
        @type flag: bool
        """
        self.__setNode('write_once',flag)
        return
    
    def isOn(self):
        """Return True if node is turned on, False if not."""
        return self.on

    def setOn(self,flag):
        """Turn node on or off
        @param flag: State to set the on characteristic. If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        """
        from _treeshr import TreeTurnOn,TreeTurnOff
        try:
            Tree.lock()
            if flag is True:
                TreeTurnOn(self)
            else:
                if flag is False:
                    TreeTurnOff(self)
                else:
                    raise TypeError,'argument must be True or False'
        finally:
            Tree.unlock()
        return

    def getOwnerId(self):
        """Get id/gid value of account which wrote data to this node"""
        return self.owner_id
    
    def getData(self):
        """Return data"""
        return self.record

    def putData(self,data):
        """Store data
        @param data: Data to store in this node.
        @type data: Data
        """
        from _treeshr import TreePutRecord
        try:
            Tree.lock()
            TreePutRecord(self,data)
        finally:
            Tree.unlock()
        return

    def deleteData(self):
        """Delete data from this node"""
        self.putData(None)
        return

    def getTimeInserted(self):
        """Return time data was written"""
        return self.time_inserted

    def doMethod(self,method,arg=None):
        """Execute method on conglomerate element"""
        from _treeshr import TreeDoMethod
        try:
            Tree.lock()
            self.restoreContext()
            TreeDoMethod(self,str(method),arg)
        finally:
            Tree.unlock()
        return

    def isSetup(self):
        """Return true if data is setup information."""
        return self.setup_information

    def getStatus(self):
        """Return action completion status"""
        return self.status
    
    def getUsage(self):
        """Return usage of this node"""
        return self.usage

    def getTags(self):
        """Return tags of this node"""
        from _treeshr import TreeFindNodeTags
        try:
            Tree.lock()
            ans=TreeFindNodeTags(self)
        finally:
            Tree.unlock()
        return ans

    def containsVersions(self):
        """Return true if this node contains data versions"""
        return self.versions

    def isSegmented(self):
        """Return true if this node contains segmented records"""
        return self.segmented
    
    def __setNode(self,qualifier,flag,reversed=False):
        self.restoreContext()
        if flag is True or flag is False:
            if reversed is True:
                flag = not flag
        if flag is True:
            switch="/"
        else:
            if flag is False:
                switch="/no"
            else:
                raise TypeError,'Argument must be True or False'
        try:
            Tree.lock()
            self.restoreContext()
            status = Data.compile('tcl("set node \\'+self.fullpath+switch+qualifier+'")').evaluate()
            if not (status & 1):
                from _descriptor import MdsGetMsg
                from _treeshr import TreeException
                msg=MdsGetMsg(int(status))
                if 'TreeFAILURE' in msg:
                    raise TreeException,'Error writing to tree, possibly file protection problem'
                else:
                    raise TreeException,msg
        finally:
            Tree.unlock()
        return

    def beginSegment(self,start,end,dimension,initialValueArray,idx=-1):
        """Begin a record segment"""
        try:
            Tree.lock()
            self.restoreContext()
            status=Data.execute('BeginSegment($,$,$,$,$,$)',self.nid,start,end,dimension,initialValue,idx)
            if not (status & 1)==1:
                raise TreeException,'Error beginning segment:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return

    def putSegment(self,data,idx):
        """Load a segment in a node"""
        try:
            Tree.lock()
            self.restoreContext()
            status=Data.execute('PutSegment($,$,$)',self.nid,idx,data)
            if not (status & 1)==1:
                raise TreeException,'Error putting segment:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return

    def updateSegment(self,start,end,dim,idx):
        """Update a segment"""
        try:
            Tree.lock()
            self.restoreContext()
            status=Data.execute('TreeShr->TreeUpdateSegment(val($),descr($),descr($),descr($),val($))',self.nid,start,end,dim,idx)
            if not (status & 1)==1:
                raise TreeException,'Error beginning segment:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return

    def beginTimestampedSegment(self,array,idx=-1):
        """Allocate space for a timestamped segment"""
        try:
            Tree.lock()
            self.restoreContext()
            status=Data.execute('BeginTimestampedSegment($,$,$)',self.nid,array,idx)
            if not (status & 1)==1:
                raise TreeException,'Error beginning timestampd segment:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return

    def putTimestampedSegment(self,array,timestampArray):
        """Load a timestamped segment"""
        try:
            Tree.lock()
            self.restoreContext()
            status=Data.execute('putTimestampedSegment($,$,$)',self.nid,timestampArray,array)
            if not (status & 1)==1:
                raise TreeException,'Error putting timestampd segment:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return

    def putRow(self,bufsize,array,timestamp):
        """Load a timestamped segment row"""
        try:
            Tree.lock()
            status=Data.execute('putRow($,$,$,$)',self.nid,bufsize,timestamp,array)
            if not (status & 1)==1:
                raise TreeException,'Error putting timestampd row:'+MdsGetMsg(int(status))
        finally:
            Tree.unlock()
        return
                
    def getNumSegments(self):
        """return number of segments contained in this node"""
        try:
            Tree.lock()
            self.restoreContext()
            ans=Data.execute('getNumSegments($)',self.nid)
        finally:
            Tree.unlock()
        return ans

    def getSegmentStart(self,idx):
        """return start of segment"""
        num=self.getNumSegments()
        if num > 0 and idx < num:
            try:
                Tree.lock()
                self.restoreContext()
                l=Data.execute('getSegmentLimits($,$)',self.nid,idx)
            finally:
                Tree.unlock()
            return l[0]
        else:
            return None

    def getSegmentEnd(self,idx):
        """return end of segment"""
        num=self.getNumSegments()
        if num > 0 and idx < num:
            try:
                Tree.lock()
                self.restoreContext()
                l=Data.execute('getSegmentLimits($,$)',self.nid,idx)
            finally:
                Tree.unlock()
            return l[1]
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
            return self.getSegment(idx).getDimension(0)
        else:
            return None

    def getSegment(self,idx):
        """Return segment
        @param idx: The index of the segment to return. Indexes start with 0.
        @type idx: int
        @return: Data segment
        @rtype: Signal | None
        """
        num=self.getNumSegments()
        if num > 0 and idx < num:
            try:
                Tree.lock()
                self.restoreContexT()
                ans = Data.execute('getSegment($,$)',self.nid,idx)
            finally:
                Tree.unlock()
            return ans
        else:
            return None

    def getTree(self):
        """Return Tree associated with this node"""
        return self.tree

    def setTree(self,tree):
        """Set Tree associated with this node"""
        self.tree=tree

    def isCompressible(self):
        """Return true if node contains data which can be compressed"""
        return self.compressible

    def isDoNotCompress(self):
        """Return true of compression is disabled for this node"""
        return self.do_not_compress

    def isSetup(self):
        """Return true if this node's data was written to the model"""
        return self.setup_information

    def setDoNotCompress(self,flag):
        """Set do not compress state of this node"""
        self.__setNode('do_not_compress',flag)
        return
    
class TreePath(TreeNode):
    """Class to represent an MDSplus node reference (path)."""
    def __init__(self,path,tree=None):
        self.tree_path=makeData(path);
        if tree is None:
            self.tree=Tree._activeTree
        else:
            self.tree=tree
        return

    def __str__(self):
        """Convert path to string."""
        return str(self.tree_path)

class TreeNodeArray(Data):
    def __init__(self,nids,tree=None):
        self.nids=nids
        if tree is None:
            self.tree=Tree._activeTree
        else:
            self.tree=tree

    def __getitem__(self,n):
        ans=TreeNode(self.nids[n],self.tree)
        return ans

    def restoreContext(self):
        self.tree.restoreContext()

    def __getattr__(self,name):
        try:
            Tree.lock()
            try:
                self.restoreContext()
                ans = Data.execute('getnci($,$)',self.nids,name)
            except Exception,e:
                if 'TdiBAD_INDEX' in str(e):
                    raise AttributeError,'Attribute %s is not defined' % (name,)
                else:
                    raise
        finally:
            Tree.unlock()
        return ans
