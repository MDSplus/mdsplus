def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import sys as _sys
import ctypes as _C
import numpy as _N

_compound=_mimport('compound')
_data=_mimport('mdsdata')
_Exceptions=_mimport('mdsExceptions')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_tree=_mimport('tree')
_mdsdcl=_mimport('mdsdcl')
_ver=_mimport('version')
_descriptor=_mimport('_descriptor')
_TreeShr=_ver.load_library('TreeShr')



class TreeNodeException(_Exceptions.MDSplusException): pass


_usage_table={'ANY':0,'NONE':1,'STRUCTURE':1,'ACTION':2,'DEVICE':3,'DISPATCH':4,'NUMERIC':5,'SIGNAL':6,
             'TASK':7,'TEXT':8,'WINDOW':9,'AXIS':10,'SUBTREE':11,'COMPOUND_DATA':12,'SUBTREE':-1}


class TreeNode(object):
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
      d=_data._Descriptor()
      d.length=4
      d.dtype=self.dtype_mds
      d.dclass=self.mdsclass
      d.pointer=_C.cast(_C.pointer(_C.c_int32(self.nid)),_C.c_void_p)
      d.original=self
      d.tree=self.tree
      return _data._descrWithUnitsAndError(self,d)
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
        if self.nid is None:
            return None
        else:
            return _scalar.Int32(self.nid)
    nid_reference=nciProp("nid_reference","node data contains nid references")
    node_name=name=nciProp("node_name","node name")
    no_write_model=nciProp("no_write_model","is storing data in this node disabled if model tree (settable)")
    @no_write_model.setter
    def no_write_model(self,value): self.setNoWriteModel(value)
    no_write_shot=nciProp("no_write_shot",
                          "is storing data in this node disabled if not model tree (settable)")
    @no_write_shot.setter
    def no_write_shot(self,value): set.setNoWriteShot(value)
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
    def set_on(self,value): self.setOn(value)
    original_part_name=nciProp("original_part_name","original part name of this node")
    owner_id=nciProp("owner_id","id of the last person to write to this node")
    parent=nciProp("parent","parent node of this node")
    parent_disable=nciProp("parent_disabled","is parent disabled")
    parent_relationship=nciProp("parent_relationship","parent relationship")
    parent_state=nciProp("parent_state","state of parent")
    path=nciProp("path","path to this node")
    path_reference=nciProp("path_reference","node data contains path references")
    @property
    def record(self):
        "Data contents of node (settable)"
        return self.getRecord()
    @record.setter
    def record(self,value): self.putData(value)
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
            except:
                ans=TreePath(path,tree)
            return ans

    def __getattr__(self,name):
        """ Return a member or child node for
            node.abc if abc is uppercase and
            matches the name of an immediate
            ancester to the node.
        """
        if name.upper() == name:
            try:
                return self.getNode(name)
            except:
                pass
        raise AttributeError('Attribute %s is not defined' % (name,))


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


    def __repr__(self):
        if self.nid is None:
            return str(self)
        else:
            if self.nid == 0:
                return str(self.fullpath)
            if self.isChild():
                prefix="."
            else:
                prefix=""
            return prefix+str(self.node_name)

    def __str__(self):
      """Convert TreeNode to string."""
      if self.nid is None:
        ans="NODEREF(*)"
      else:
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
        ok=True
        if isinstance(names,(list,tuple)):
            for name in names:
                if not isinstance(name,str):
                    ok=False
        elif not isinstance(names,str):
            ok=False
        if not ok:
            raise Exception("Tag names must be a string or list of strings")
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
        _TreeShr._TreeAddTag(self.tree.ctx,self.nid,_C.c_char_p(str.encode(tag)))

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
          _tree.Tree.lock()
          status=_TreeShr._TreeBeginSegment(self.tree.ctx,_C.c_int32(self.nid),
                                            _C.pointer(_data.makeData(start).descriptor),
                                            _C.pointer(_data.makeData(end).descriptor),
                                            _C.pointer(_data.makeData(dimension).descriptor),
                                            _C.pointer(_data.makeData(initialValueArray).descriptor),
                                            _C.c_int32(idx))
        finally:
          _tree.Tree.unlock()
        if (status & 1) == 0:
          raise TreeException("Error beginning segment: %s" % _data.getStatusMsg(status))

    def beginTimestampedSegment(self,array,idx=-1):
        """Allocate space for a timestamped segment
        @param array: Initial data array to define shape of segment
        @type array: Array
        @param idx: Optional segment index. Defaults to -1 meaning next segment.
        @type idx: int
        @rtype: None
        """
        try:
          _tree.Tree.lock()
          status=_TreeShr._TreeBeginTimestampedSegment(self.tree.ctx,_C.c_int32(self.nid),
                                                       _C.pointer(_data.makeData(value).descriptor),
                                                       _C.c_int32(idx))
        finally:
          _tree.Tree.unlock()
        if (status & 1)==0:
          raise TreeException("Error beginning timestamped segment: %s" % _data.getStatusMsg(status))

    def compare(self,value):
        """Returns True if this node contains the same data as specified in the value argument
        @param value: Value to compare contents of the node with
        @type value: Data
        @rtype: Bool
        """
        if isinstance(value,TreePath) and isinstance(self,TreePath):
          ans=str(self)==str(value)
        elif type(self)==TreeNode and type(value)==TreeNode:
          ans=self.nid==value.nid and self.tree==value.tree
        else:
          try:
            ans=value.compare(self.record)
          except:
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
                status=_data.Data.execute("tcl('dispatch/wait "+str(self.fullpath).replace('\\','\\\\')+"')")
            else:
                status=_data.Data.execute("tcl('dispatch/nowait "+str(self.fullpath).replace('\\','\\\\')+"')")
            if not (status & 1):
                raise TreeNodeException(status)

    def doMethod(self,method,*args):
        """Execute method on conglomerate element
        @param method: method name to perform
        @type method: str
        @param arg: Optional argument for method
        @type arg: Data
        @rtype: None
        """
        try:
            _tree.Tree.lock()
            self.restoreContext()
            arglist=[n.tree.ctx,_C.pointer(_descriptor.descriptor(n)),_C.pointer(_descriptor.descriptor(method))]
            ans_d=descriptor_xd()
            for arg in args:
                arglist.append(_C.pointer(_descriptor.descriptor(arg)))
            arglist.append(_C.pointer(ans_d))
            arglist.append(_C.c_void_p(0xffffffff))
            status=_TreeShr.__TreeDoMethod(*arglist)
            if (status & 1):
                return ans_d.value
            else:
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()
        return

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
        child=self.child
        if child is not None:
          nids=[child.nid,]
          brother=child.brother
          while brother is not None:
            nids.append(brother.nid)
            brother=brother.brother
          return TreeNodeArray(nids,self.tree)
        else:
          return None

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
        num_elts=int(self.number_of_elts)
        if num_elts > 0:
            first=self.nid-int(self.conglomerate_elt)+1
            last=first+num_elts
            return TreeNodeArray(list(range(first,last)),self.tree)
        else:
            return None

    def getRecord(self, *altvalue):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        value=_descriptor.descriptor_xd()
        status=_TreeShr._TreeGetRecord(self.tree.ctx,self.nid,_C.pointer(value))
        if (status & 1):
            ans = value.value
            return ans
        elif len(altvalue)==1 and status == _Exceptions.treeshrExceptions.TreeNODATA.status:
            return altvalue[0]
        else:
            raise _Exceptions.statusToException(status)
    getData=getRecord

    def getDataNoRaise(self):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        return self.getData(None)

    def _getDescriptor(self):
        return None

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
        return TreeNodeArray(self.member_nids.nids + self.children_nids.nids,self.tree)

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
        member=self.member
        if member is not None:
          nids=[member.nid,]
          brother=member.brother
          while brother is not None:
            nids.append(brother.nid)
            brother=brother.brother
          return TreeNodeArray(nids,self.tree)
        else:
          return None

    def getMinPath(self):
        """Return shortest path string for this node
        @return: shortest path designation depending on the current node default and whether the node has tag names or not.
        @rtype: String
        """
        return self.minpath

    def getNci(self,items,returnDict=True):
        """Return dictionary of nci items"""
        status=0
        class NCI_ITEMS(_C.Structure):
            _fields_=list()
            for idx in range(50):
                _fields_+=[("buflen%d"%idx,_C.c_ushort),
                           ("code%d"%idx,_C.c_ushort),
                           ("pointer%d"%idx,_C.c_void_p),
                           ("retlen%d"%idx,_C.POINTER(_C.c_int32))]
                
            def __init__(self,items):
                retlens=(_C.c_uint16*50)()
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

        if isinstance(items,str):
            items=[items]
        elif isinstance(items,tuple):
            items=list(items)
        elif not isinstance(items,list):
            raise Exception("items must be a list,tuple or str")
        real_items=list()
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

        itmlst=NCI_ITEMS(items)
        try:
            _tree.Tree.lock()
            if len(items) > 0:
                status=_TreeShr._TreeGetNci(self.tree.ctx,_C.c_int32(self.nid),_C.pointer(itmlst))
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
                        val=_scalar.makeScalar(val)
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
            _tree.Tree.unlock()
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
        _tree.Tree.lock()
        try:
            num=_C.c_int32(0)
            status=_TreeShr._TreeGetNumSegments(self.tree.ctx,self.nid,_C.pointer(num))
        finally:
            _tree.Tree.unlock()
        if (status & 1):
            return num.value
        else:
            raise _Exceptions.statusToException(status)

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
            value=_descriptor.descriptor_xd()
            dim=_descriptor.descriptor_xd()
            status=_TreeShr._TreeGetSegment(self.tree.ctx,self.nid,idx,_C.pointer(value),_C.pointer(dim))
            if (status & 1):
                return _compound.Signal(value.value,None,dim.value)
            else:
                raise _Exceptions.statusToException(status)
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
        start=_descriptor.descriptor_xd()
        end=_descriptor.descriptor_xd()
        status=_TreeShr._TreeGetSegmentLimits(self.tree.ctx,self.nid,idx,_C.pointer(start),_C.pointer(end))
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
        _tree.Tree.lock()
        try:
            ctx=_C.c_void_p(0)
            tags=list()
            done=False
            fnt=_TreeShr._TreeFindNodeTags
            fnt.restype=_C.c_void_p
            while not done:
                tag_ptr=_TreeShr._TreeFindNodeTags(self.tree.ctx,_C.c_int32(self.nid),_C.pointer(ctx))
                try:
                    tags.append(_ver.tostr(_C.cast(tag_ptr,_C.c_char_p).value.rstrip()))
                    _TreeFree(tag_ptr)
                except:
                    done=True
            tags = _array.makeArray(tags).astype(_ver.npstr)
            return tags
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
      if self.parent:
          return self.parent.disabled
      else:
          return False

    def isParentOn(self):
      """Return True if parent is on
      @return: Return True if parent is turned on
      @rtype: bool
      """
      return not self.parent_state

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
        _tree.Tree.lock()
        try:
            if isinstance(initialValue,_compound.Compound):
                status=_TreeShr._TreeMakeSegment(self.tree.ctx,
                                                 _C.c_int32(self.nid),
                                                 _C.pointer(_descriptor.descriptor(start)),
                                                 _C.pointer(_descriptor.descriptor(end)),
                                                 _C.pointer(_descriptor.descriptor(dimension)),
                                                 _C.pointer(_descriptor.descriptor(initialValue)),
                                                 _C.c_int32(idx),_C.c_int32(1))
            else:
                status=_TreeShr._TreeMakeSegment(self.tree.ctx,
                                                 _C.c_int32(self.nid),
                                                 _C.pointer(_descriptor.descriptor(start)),
                                                 _C.pointer(_descriptor.descriptor(end)),
                                                 _C.pointer(_descriptor.descriptor(dimension)),
                                                 _C.pointer(_descriptor.descriptor_a(initialValue)),
                                                 _C.c_int32(idx),
                                                 _C.c_int32(initialValue.shape[0]))
        finally:
            _tree.Tree.unlock()
        if not (status & 1):
            raise _Exceptions.statusToException(status)

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
            delim="."
        else:
            delim=":"
        _tree.Tree.lock()
        try:
            status = _TreeShr._TreeRenameNode(self.tree.ctx,self.nid,_ver.tobytes(name))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

    def putData(self,value):
        """Store data
        @param data: Data to store in this node.
        @type data: Data
        @rtype: None
        """
        _tree.Tree.lock()
        try:
            if isinstance(value,_data.Data) and value.__hasBadTreeReferences__(self.tree):
                value=value.__fixTreeReferences__(self.tree)
            status=_TreeShr._TreePutRecord(self.tree.ctx,self.nid,_C.pointer(_descriptor.descriptor(value)),0)
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

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
        _tree.Tree.lock()
        try:
            array=_array.makeArray(array)
            status=_TreeShr._TreePutRow(self.tree.ctx,_C.c_int32(self.nid),_C.c_int32(bufsize),
                                        _C.pointer(_C.c_int64(int(timestamp))),
                             _C.pointer(_descriptor.descriptor_a(array)))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

    def putSegment(self,data,idx):
        """Load a segment in a node
        @param data: data to load into segment
        @type data: Array or Scalar
        @param idx: index into segment to load data
        @type idx: int
        @rtype: None
        """
        _tree.Tree.lock()
        try:
            status=_TreeShr._TreePutSegment(self.tree.ctx,self.nid,idx,_C.pointer(_descriptor.descriptor(value)))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

    def putTimestampedSegment(self,timestampArray,array):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @rtype: None
        """
        timestampArray=_array.Int64Array(timestampArray)
        value=_array.makeArray(value)
        _tree.Tree.lock()
        try:
            status=__TreePutTimestampedSegment(n.tree.ctx,n.nid,_descriptor.descriptor_a(timestampArray).pointer,
                                               _C.pointer(_descriptor.descriptor_a(value)))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

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
        _tree.Tree.lock()
        try:
            status=_TreeShr._TreeMakeTimestampedSegment(self.tree.ctx,self.nid,
                                                        _descriptor.descriptor_a(timestamps).pointer,
                                                        _C.pointer(_descriptor.descriptor_a(value)),
                                                        _C.c_int32(idx),_C.c_int32(rows_filled))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()

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
            node.tree.lock()
            olddefault=self.tree.default
            self.tree.setDefault(self.parent)
            if self.isChild():
                newname="."+_ver.tostr(newname)
            status = _TreeShr._TreeRenameNode(node.tree.ctx,node.nid,_ver.tobytes(name))
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            self.tree.setDefault(olddefault)
            node.tree.unlock()
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
          _tree.Tree.lock()
          status = _TreeShr._TreeSetNci(self.tree.ctx,_C.c_int32(self.nid),_C.pointer(itmlst))
        finally:
          _tree.Tree.unlock()
        if not (status & 1):
            raise _Exceptions.statusToException(status)

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

    def setOn(self,flag):
        """Turn node on or off
        @param flag: State to set the on characteristic. 
        If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: None
        """
        _tree.Tree.lock()
        try:
            if flag:
                status=_TreeShr._TreeTurnOn(self.tree.ctx,self.nid)
            else:
                status=_TreeShr._TreeTurnOff(self.tree.ctx,self.nid)
            if not (status & 1):
                raise _Exceptions.statusToException(status)
        finally:
            _tree.Tree.unlock()


    def setSubtree(self,flag):
        """Enable/Disable node as a subtree
        @param flag: True to make node a subtree reference. 
        Node must be a child node with no descendants.
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
        self._setNciFlag(0x80, flag)

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
        try:
            self.nid=self.tree.getNode(self.tree_path).nid
        except:
            pass
        return

    def __hasBadTreeReferences__(self,tree):
       return False


    def __str__(self):
        """Convert path to string."""
        return self.tree_path.value

class TreeNodeArray(_data.Data):
    def __init__(self,nids,tree=None):
        self.nids=nids
        if tree is None:
            self.tree=_tree.Tree()
        else:
            self.tree=tree

    def __eq__(self,y):
        return(_N.array(self.nids,dtype=_N.uint32)==_N.array(y.nids,dtype=_N.uint32))

    def __len__(self):
        return len(self.nids)
    
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

    def setNoWriteModel(self,flag):
        """Set no write model flag
        @param flag: True to disallow writing to model
        @type flag: bool
        @rtype: original type
        """
        for nid in self:
            nid.setNoWriteModel(flag)
        return self

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
                ans = _data.Data.execute('getnci($,$)',_array.Int32Array(self.nids),name)
            except Exception:
                if 'BAD_INDEX' in str(_sys.exc_info()[1]):
                    raise AttributeError('Attribute %s is not defined' % (name,))
                else:
                    raise
        finally:
            _tree.Tree.unlock()
        return ans


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
    
TreeNode.NciEND_OF_LIST        =(0,_C.c_void_p,4,None)
TreeNode.NciSET_FLAGS          =(1,_C.c_int32,4,_scalar.Uint32)
TreeNode.NciCLEAR_FLAGS        =(2,_C.c_int32,4,_scalar.Uint32)
TreeNode.NciTIME_INSERTED      =(4,_C.c_uint64,8,_scalar.Uint64)
TreeNode.NciOWNER_ID	          =(5,_C.c_int32,4,_scalar.Uint8)
TreeNode.NciCLASS	          =(6,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciDTYPE	          =(7,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciLENGTH	          =(8,_C.c_int32,4,_scalar.Int32)
TreeNode.NciSTATUS	          =(9,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciCONGLOMERATE_ELT	  =(10,_C.c_uint16,2,_scalar.Uint16)
TreeNode.NciGET_FLAGS	  =(12,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciNODE_NAME	  =(13,_C.c_char_p,13,_scalar.String)
TreeNode.NciPATH		  =(14,_C.c_char_p,1024,_scalar.String)
TreeNode.NciDEPTH		  =(15,_C.c_int32,4,_scalar.Int32)
TreeNode.NciPARENT		  =(16,_C.c_uint32,4,TreeNode)
TreeNode.NciBROTHER		  =(17,_C.c_uint32,4,TreeNode)
TreeNode.NciMEMBER		  =(18,_C.c_uint32,4,TreeNode)
TreeNode.NciCHILD		  =(19,_C.c_uint32,4,TreeNode)
TreeNode.NciPARENT_RELATIONSHIP=(20,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciK_IS_CHILD         =1
TreeNode.NciK_IS_MEMBER        =2
TreeNode.NciCONGLOMERATE_NIDS  =(21,_C.c_uint32*1024,1024*4,TreeNodeArray)
TreeNode.NciORIGINAL_PART_NAME =(22,_C.c_char_p,1024,_scalar.String)
TreeNode.NciNUMBER_OF_MEMBERS  =(23,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciNUMBER_OF_CHILDREN =(24,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciMEMBER_NIDS        =(25,_C.c_uint32*4096,4096*4,TreeNodeArray)
TreeNode.NciCHILDREN_NIDS      =(26,_C.c_uint32*4096,4096*4,TreeNodeArray)
TreeNode.NciFULLPATH		  =(27,_C.c_char_p,1024,_scalar.String)
TreeNode.NciMINPATH		  =(28,_C.c_char_p,1024,_scalar.String)
TreeNode.NciUSAGE		  =(29,_C.c_uint8,1,_scalar.Uint8)
TreeNode.NciPARENT_TREE	  =(30,_C.c_char_p,13,_scalar.String)
TreeNode.NciRLENGTH	 	  =(31,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciNUMBER_OF_ELTS	  =(32,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciDATA_IN_NCI	  =(33,_C.c_bool,4,bool)
TreeNode.NciERROR_ON_PUT       =(34,_C.c_uint32,4,bool)
TreeNode.NciRFA		  =(35,_C.c_uint64,8,_scalar.Uint64)
TreeNode.NciIO_STATUS          =(36,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciIO_STV             =(37,_C.c_uint32,4,_scalar.Uint32)
TreeNode.NciDTYPE_STR          =(38,_C.c_char_p,64,_scalar.String)
TreeNode.NciUSAGE_STR          =(39,_C.c_char_p,64,_scalar.String)
TreeNode.NciCLASS_STR          =(40,_C.c_char_p,64,_scalar.String)
TreeNode.NciVERSION            =(41,_C.c_uint32,4,_scalar.Uint32)
