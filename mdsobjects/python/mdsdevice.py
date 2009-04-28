from treenode import TreeNode

class Device(TreeNode):
    """Used for device support classes. Provides ORIGINAL_PART_NAME method and allows referencing of subnodes as conglomerate node attributes.

    Use this class as a superclass for device support classes. When creating a device support class include a class attribute called part_names
    as a tuple of strings of the node names of the devices members (not including the head node). You can also include a part_dict
    class attribute consisting of a dict() instance whose keys are attribute names and whose values are nid offsets. If you do
    not provide a part_dict attribute then one will be created from the part_names attribute where the part names are converted
    to lowercase and the colons and periods are replaced with underscores.

    Sample usage1:
    
       from MDSplus import Device

       class mydev(Device):
           part_names=(':COMMENT',':INIT_ACTION',':STORE_ACTION','.SETTINGS','.SETTINGS:KNOB1','.SIGNALS','.SIGNALS:CHANNEL_1')

           def init(self,arg):
               knob1=self.settings_knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.signals_channel_1=Signal(32,None,42)

    Sample usage2:

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
               
    If you need to reference attributes using computed names you can do something like:

        for i in range(16):
            self.__setattr__('signals_channel_%02d' % (i+1,),Signal(...))
    """
    
    def __fix_part(name):
        name=name[1:].lower()
        name=name.replace(':','_')
        name=name.replace('.','_')
        return name
    __fix_part=staticmethod(__fix_part)

    def __new__(cls,node):
        if hasattr(cls,'part_names') and not hasattr(cls,'part_dict'):
            cls.part_dict=dict()
            for i in range(len(cls.part_names)):
                try:
                    cls.part_dict[Device.__fix_part(cls.part_names[i])]=i+1
                except:
                    pass
        return super(Device,cls).__new__(cls,node)

    def __init__(self,node):
        try:
            self.nids=node.conglomerate_nids.nid_number
            self.head=int(self.nids[0])
        except:
            self.head=node.nid
        self.tree=node.tree
        self.nid=node.nid

    def ORIGINAL_PART_NAME(self,arg):
        name = ""
        if self.nid != self.head:
            try:
                name = self.part_names[self.nid-self.head-1].upper()
            except:
                raise
        return name

    PART_NAME=ORIGINAL_PART_NAME

    def __getattr__(self,name):
        try:
            return self.__dict__[name]
        except:
            if name != 'part_dict':
                return TreeNode(self.part_dict[name]+self.head,self.tree)
            else:
                raise

    def __setattr__(self,name,value):
        try:
            node=TreeNode(self.part_dict[name]+self.head,self.tree)
            node.record=value
        except KeyError:
            self.__dict__[name]=value
            
