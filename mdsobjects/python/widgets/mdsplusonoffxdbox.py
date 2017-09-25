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

from gtk import HBox,CheckButton,Button,Label
import gobject

from mdspluswidget import MDSplusWidget
from mdsplusxdbox import MDSplusXdBox
from mdspluserrormsg import MDSplusErrorMsg
import sys

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'buttonLabel' : (gobject.TYPE_STRING, 'buttonLabel','Label on popup button','',gobject.PARAM_READWRITE),
        }

class MDSplusOnOffXdBoxWidget(props,MDSplusWidget,HBox):

    __gtype_name__ = 'MDSplusOnOffXdBoxWidget'
    __gproperties__= props.__gproperties__

    def reset(self):
        self.node_state.set_active(self.node.on)
        self.node_state.set_label('')
        if hasattr(self,'xdbox'):
            self.xdbox.reset()

    def apply(self):
        if self.putOnApply:
            if self.node.on != self.node_state.get_active():
                try:
                    self.node.on=self.node_state.get_active()
                except Exception:
                    if self.node_state.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.node.minpath,state,sys.exc_info()))
                    raise
            if hasattr(self,'xdbox'):
                try:
                    if self.node.compare(self.xdbox.value) != 1:
                        self.node.record=self.xdbox.value
                    self.reset()
                except Exception:
                    MDSplusErrorMsg('Error storing value','Error storing value in to %s\n\n%s' % (self.node.minpath,sys.exc_info()))

    def xd_state_changed(self,button):
        self.node_state.set_active(self.xdbox.on.get_active())

    def node_state_changed(self,button):
        self.xdbox.on.set_active(self.node_state.get_active())
        
    def popupXd(self,button):
        if not hasattr(self,'xdbox'):
            self.xdbox=MDSplusXdBox(self.node)
            self.xdbox.putOnApply=False
            self.xdbox.on.connect('toggled',self.xd_state_changed)
            self.node_state.connect('toggled',self.node_state_changed)
        self.xdbox.node=self.getNode()
        self.xdbox.set_title(self.buttonLabel)
        self.xdbox.on.set_active(self.node_state.get_active())
        self.xdbox.show()

    def setButtonLabel(self,button):
        self.button.set_label(self.buttonLabel)

    def __init__(self):
        HBox.__init__(self)
        MDSplusWidget.__init__(self)
        HBox.set_homogeneous(self,False)
        self.node_state=CheckButton('')
        self.button=Button()
        HBox.pack_start(self,self.node_state,False,False,0)
        HBox.pack_start(self,self.button,False,False,0)
        HBox.pack_start(self,Label(''),True,False,0)
        if not guibuilder:
            self.button.connect("clicked",self.popupXd)
        self.button.connect("realize",self.setButtonLabel)

    def show(self):
        self.show_all()
        
        

gobject.type_register(MDSplusOnOffXdBoxWidget) 

if guibuilder:

    class MDSplusOnOffXdboxWidgetAdaptor(glade.get_adaptor_for_type('GtkHBox')):
        __gtype_name__='MDSplusOnOffXdBoxWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'buttonLabel':
                widget.buttonLabel=value
                widget.button.set_label(value)
