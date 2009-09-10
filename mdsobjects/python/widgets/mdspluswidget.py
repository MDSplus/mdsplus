import gobject

class MDSplusWidget(object):


    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,0,gobject.PARAM_READWRITE),
        }
    #sender=EventSender()

    def doToAll(cls,obj,method):
        status = True
        if hasattr(obj,"get_children"):
            for child in obj.get_children():
                status = status and MDSplusWidget.doToAll(child,method)
                if isinstance(child,MDSplusWidget):
                    try:
                        child.__getattribute__(method)()
                    except Exception,e:
                        status = False
        return status
    doToAll=classmethod(doToAll)
    
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
            try:
                value=self.value
                if self.node.compare(value) != 1:
                    self.node.record=self.value
                self.reset()
            except Exception,e:
                self.popupError(e)
                raise

    def handleReset(self,obj,top):
        if self.get_toplevel()  == top:
            self.reset()

    def handleApply(self,obj,top):
        if self.get_toplevel() == top:
            self.apply()

    def errorWindow(self,msg):
        import gtk
        window=gtk.Dialog(title="Error")
        window.connect("destroy",self.closeErrorWindow)
        window.set_modal(True)
        text=gtk.Label(msg)
        button=gtk.Button(stock=gtk.STOCK_CLOSE)
        button.connect("clicked",self.closeErrorWindow)
        window.vbox.add(text)
        window.vbox.add(button)
        window.show_all()

    def closeErrorWindow(self,button):
        button.get_toplevel().destroy()

    def popupError(self,msg):
        if 'TreeINVDTPUSG' in str(msg):
            msg="Invalid data type for a %s node" % (self.node.usage.lower(),)
        self.errorWindow(("Error writing to node: %s\n"+
                              "Error message is:      %s") % (self.node.node_name,msg))
