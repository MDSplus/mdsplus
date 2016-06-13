def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import os as _os
from os import getenv as _getenv
_treeshr=_mimport('_treeshr')
_treenode=_mimport('treenode')
_compound=_mimport('compound')
_ident=_mimport('ident')
_mdsarray=_mimport('mdsarray')
_mdsdata=_mimport('mdsdata')

class Device(_treenode.TreeNode):
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

    def __class_init__(cls):
        if not hasattr(cls,'initialized'):
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
            cls.initialized=True
    __class_init__=classmethod(__class_init__)

    def __new__(cls,node):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param node: Node of device
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        if cls.__name__ == 'Device':
            try:
                head=_treenode.TreeNode(node.conglomerate_nids.nid_number[0],node.tree)
                model=str(head.record.model)
                return cls.importPyDeviceModule(model).__dict__[model.upper()](head)
            except:
                pass
            raise TypeError("Cannot create instances of Device class")
        else:
            cls.__class_init__();
            return super(Device,cls).__new__(cls,node)

    def __init__(self,node,tree=None):
        """Initialize a Device instance
        @param node: Conglomerate node of this device
        @type node: TreeNode
        @rtype: None
        """
        if isinstance(node,_treenode.TreeNode):
            try:
                self.nids=node.conglomerate_nids.nid_number
                self.head=int(self.nids[0])
            except Exception:
                self.head=node.nid
            super(Device,self).__init__(node.nid,node.tree)

    def ORIGINAL_PART_NAME(self):
        """Method to return the original part name.
        Will return blank string if part_name class attribute not defined or node used to create instance is the head node or past the end of part_names tuple.
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
        @return: Device instance of device part
        @rtype: Device
        """
        if name == 'part_name' or name == 'original_part_name':
            return self.ORIGINAL_PART_NAME(None)
        try:
            return self.__class__(_treenode.TreeNode(self.part_dict[name]+self.head,self.tree))
        except KeyError:
            return super(Device,self).__getattr__(name)

    def __setattr__(self,name,value):
        """Set value into device subnode if name matches a mangled subpart node name. Otherwise assign value to class instance attribute.
        @param name: Name of attribute or device subpart
        @type name: str
        @param value: Value of the attribute or device subpart
        @type value: varied
        @rtype: None
        """
        try:
            _treenode.TreeNode(self.part_dict[name]+self.head,self.tree).record=value
        except KeyError:
            super(Device,self).__setattr__(name,value)

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
        if isinstance(tree, _treenode.TreeNode): tree = tree.tree
        cls.__class_init__()
        _treeshr.TreeStartConglomerate(tree,len(cls.parts)+1)
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
        _treeshr.TreeEndConglomerate(tree)
        return head
    Add=classmethod(Add)


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
        except Exception:
            import sys
            e=sys.exc_info()[1]
            print( e)
            raise Exception("No setup available, %s" % (str(e),))

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

    def waitForSetups(cls):
        Device.gtkThread.join()
    waitForSetups=classmethod(waitForSetups)


    def importPyDeviceModule(name):
        """Find a device support module with a case insensitive lookup of
        'model'.py in the MDS_PYDEVICE_PATH environment variable search list."""

        import __builtin__
        import sys
        check_name=name.lower()+".py"
        if "MDS_PYDEVICE_PATH" in _os.environ:
            path=_os.environ["MDS_PYDEVICE_PATH"]
            parts=path.split(';')
            for part in parts:
                w=_os.walk(part)
                for dp,dn,fn in w:
                    for fname in fn:
                        if fname.lower() == check_name:
                            sys.path.insert(0,dp)
                            try:
                                ans=__builtin__.__import__(fname[:-3])
                            finally:
                                sys.path.remove(dp)
                            return ans
    importPyDeviceModule=staticmethod(importPyDeviceModule)

    def findPyDevices():
        """Find all device support modules in the MDS_PYDEVICE_PATH environment variable search list."""
        ans=list()
        import __builtin__
        import sys
        if "MDS_PYDEVICE_PATH" in _os.environ:
            path=_os.environ["MDS_PYDEVICE_PATH"]
            parts=path.split(';')
            for part in parts:
                w=_os.walk(part)
                for dp,dn,fn in w:
                    for fname in fn:
                        if fname.endswith('.py'):
                            sys.path.insert(0,dp)
                            try:
                                devnam=fname[:-3].upper()
                                device=__builtin__.__import__(fname[:-3]).__dict__[devnam]
                                ans.append(devnam+'\0')
                                ans.append('\0')
                            except:
                                pass
                            finally:
                                sys.path.remove(dp)
        if len(ans) == 0:
            return None
        else:
            return _mdsdata.Data.execute(str(ans))


    findPyDevices=staticmethod(findPyDevices)

