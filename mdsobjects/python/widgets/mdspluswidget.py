import gobject

class EventSender(gobject.GObject):
    def __init__(self):
        self.__gobject_init__()

gobject.type_register(EventSender)

gobject.signal_new("mdsplus_widget_reset",EventSender,gobject.SIGNAL_RUN_FIRST,
                   gobject.TYPE_NONE,(gobject.TYPE_PYOBJECT,))

gobject.signal_new("mdsplus_widget_apply",EventSender,gobject.SIGNAL_RUN_FIRST,
                   gobject.TYPE_NONE,(gobject.TYPE_PYOBJECT,))

class MDSplusWidget(object):


    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,0,gobject.PARAM_READWRITE),
        }
    sender=EventSender()

    def resetAll(cls,top):
        cls.sender.emit("mdsplus_widget_reset",top)
    resetAll=classmethod(resetAll)
    
    def applyAll(cls,top):
        cls.sender.emit("mdsplus_widget_apply",top)
    applyAll=classmethod(applyAll)
    
    def __init__(self):
        MDSplusWidget.sender.connect("mdsplus_widget_apply",self.handleApply)
        MDSplusWidget.sender.connect("mdsplus_widget_reset",self.handleReset)

    def reset(self):
        pass
    
    def getRecord(self):
        try:
            return self.node.record
        except:
            return None
        
    def setRecord(self,value):
        self.node.record=value
        
    record=property(getRecord,setRecord)

    def setNidOffset(self,devnode):
        found=False
        devnodes=devnode.conglomerate_nids
        for node in devnodes:
            if node.original_part_name == self.name.upper():
                found=True
                self.nidOffset=node.conglomerate_elt-1
                break
        if not found:
            print "Was unable to find an original part name of /%s/ in the conglomerate" % (self.name.upper(),)

    def getNode(self):
        try:
            return self._node
        except:
            try:
                devnode=self.get_toplevel().device_node
            except AttributeError:
                print "Top level window must have a device_node attribute of type TreeNode which is element of the device."
                raise
            if not hasattr(self,"nidOffset") or self.nidOffset is None or self.nidOffset <= 0:
                self.setNidOffset(devnode)
            self._node=devnode.parent.__class__(devnode.nid_number-devnode.conglomerate_elt+1+self.nidOffset,devnode.tree)
            return self._node

    def setNode(self,value):
        self._node=value

    node=property(getNode,setNode)
    
    def do_set_property(self, property, value):
        if property.name in dir(self.props):
            self.__dict__[property.name]=value
        else:
            raise AttributeError, 'unknown property %s' % property.name

    def apply(self):
        if self.putOnApply:
            self.node.value=self.value
            self.reset(self)

    def handleReset(self,obj,top):
        if self.get_toplevel()  == top:
            self.reset()

    def handleApply(self,obj,top):
        if self.get_toplevel() == top:
            self.apply()

