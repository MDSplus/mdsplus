# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import gobject
import traceback
import os
import sys
from MDSplus import TreeNode

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class MDSplusWidget(object):


    def __init__(self):
        self.putOnApply=True
        self.nidOffset=-1

    def doToAll(cls,obj,method):
        status = True
        if hasattr(obj,"get_children"):
            for child in obj.get_children():                
                stat = MDSplusWidget.doToAll(child,method)
                status = status and stat
                if isinstance(child,MDSplusWidget):
                    try:
                        child.__getattribute__(method)()
                    except Exception:
                        traceback.print_exc()
                        print("MDSplusWidget error: %s" % (sys.exc_info(),))
                        status = False
        return status
    doToAll=classmethod(doToAll)

    def startTimer(self):
        self._timer=os.times()

    def getTimer(self):
        now=os.times()
        return (now[0]-self._timer[0],now[4]-self._timer[4])
    
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

    def originalPartNames(self):
        devnode=self.devnode
        if not hasattr(devnode,'original_part_names'):
            devnode.original_part_names=dict()
            devnodes=devnode.conglomerate_nids
            for node in devnodes:
                devnode.original_part_names[str(node.original_part_name)]=node.conglomerate_elt-1
        return devnode.original_part_names

    def setNidOffset(self):
        try:
            node=self.devnode.tree.getNode(str(self.devnode)+self.name.upper())
            self.nidOffset=node.conglomerate_elt-1
        except:
            try:
                self.nidOffset=self.originalPartNames()[self.name.upper()]
            except Exception:
                raise(AttributeError("Was unable to find a node or original part name of /%s/ in the conglomerate" % (self.name.upper(),)))

    def getDevNode(self):
        try:
            return self.get_toplevel().device_node
        except AttributeError:
            if self.parent and isinstance(self.parent,MDSplusWidget):
                return self.parent.getDevNode()
            else:
                if guibuilder:
                    return None
                else:
                    raise(AttributeError("Top level window must have a device_node attribute of type TreeNode which is element of the device."))
    devnode=property(getDevNode)

    def getNode(self):
        try:
            return self._node
        except:
            try:
                devnode=self.get_toplevel().device_node
            except AttributeError:
                if guibuilder:
                    return None
                else:
                    raise(AttributeError("Top level window must have a device_node attribute of type TreeNode which is element of the device."))
            if not hasattr(self,"nidOffset") or self.nidOffset is None or self.nidOffset < 0:
                self.setNidOffset()
            self._node=TreeNode(devnode.nid_number-devnode.conglomerate_elt+1+self.nidOffset,devnode.tree)
            return self._node

    def setNode(self,value):
        self._node=value

    node=property(getNode,setNode)
    
    def do_set_property(self, property, value):
        if property.name in dir(self.props):
            self.__dict__[property.name]=value
        else:
            raise(AttributeError( 'unknown property %s' % property.name))

    def apply(self):
        if self.putOnApply:
            value=self.value
            try:
                if self.node.compare(value) != 1:
                    self.node.record=value
                self.reset()
            except Exception:
                self.popupError(sys.exc_info())
                raise
        return True

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
