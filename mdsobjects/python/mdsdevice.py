from treenode import TreeNode

class Device(TreeNode):
    """Used for device support classes. Provides ORIGINAL_PART_NAME method and allows referencing of subnodes as conglomerate node attributes.

    Use this class as a superclass for device support classes. When creating a device support class include a class attribute called part_names
    as a tuple of strings of the node names of the devices members (not including the head node). You can also include a part_dict
    class attribute consisting of a dict() instance whose keys are attribute names and whose values are nid offsets. If you do
    not provide a part_dict attribute then one will be created from the part_names attribute where the part names are converted
    to lowercase and the colons and periods are replaced with underscores.

    Sample usage1::
    
       from MDSplus import Device

       class mydev(Device):
           part_names=(':COMMENT',':INIT_ACTION',':STORE_ACTION','.SETTINGS','.SETTINGS:KNOB1','.SIGNALS','.SIGNALS:CHANNEL_1')

           def init(self,arg):
               knob1=self.settings_knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.signals_channel_1=Signal(32,None,42)

    Sample usage2::

       from MDSplus import Device

       class mydev(Device):
           part_names=(':COMMENT',':INIT_ACTION',':STORE_ACTION','.SETTINGS','.SETTINGS:KNOB1','.SIGNALS','.SIGNALS:CHANNEL_1')
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
    
    def __fix_part(name):
        """Replace colons and periods in node path names by underscores. Used internally.
        @param name: relative node path name of device part
        @type name: str
        @return: name with colons and periods replaced by underscores
        @rtype: str
        """
        name=name[1:].lower()
        name=name.replace(':','_')
        name=name.replace('.','_')
        return name
    __fix_part=staticmethod(__fix_part)

    def __new__(cls,node):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param node: Not used
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        if cls.__name__ == 'Device':
            raise TypeError,"Cannot create instances of Device class"
        if hasattr(cls,'part_names') and not hasattr(cls,'part_dict'):
            cls.part_dict=dict()
            for i in range(len(cls.part_names)):
                try:
                    cls.part_dict[Device.__fix_part(cls.part_names[i])]=i+1
                except:
                    pass
        return super(Device,cls).__new__(cls,node)

    def __init__(self,node):
        """Initialize a Device instance
        @param node: Conglomerate node of this device
        @type node: TreeNode
        @rtype: None
        """
        try:
            self.nids=node.conglomerate_nids.nid_number
            self.head=int(self.nids[0])
        except:
            self.head=node.nid
        self.tree=node.tree
        self.nid=node.nid

    def ORIGINAL_PART_NAME(self,arg):
        """Method to return the original part name.
        Will return blank string if part_name class attribute not defined or node used to create instance is the head node or past the end of part_names tuple.
        @param arg: Not used. Placeholder for do method argument
        @type arg: Use None
        @return: Part name of this node
        @rtype: str
        """
        name = ""
        if self.nid != self.head:
            try:
                name = self.part_names[self.nid-self.head-1].upper()
            except:
                pass
        return name
    PART_NAME=ORIGINAL_PART_NAME

    def __getattr__(self,name):
        """Return TreeNode of subpart if name matches mangled node name.
        @param name: part name. Node path with colons and periods replaced by underscores.
        @type name: str
        @return: TreeNode of device part
        @rtype: TreeNode
        """
        try:
            return TreeNode(self.part_dict[name]+self.head,self.tree)
        except:
            raise AttributeError

    def __setattr__(self,name,value):
        """Set value into device subnode if name matches a mangled subpart node name. Otherwise assign value to class instance attribute.
        @param name: Name of attribute or device subpart
        @type name: str
        @param value: Value of the attribute or device subpart
        @type value: varied
        @rtype: None
        """
        try:
            TreeNode(self.part_dict[name]+self.head,self.tree).record=value
        except KeyError:
            self.__dict__[name]=value
            
