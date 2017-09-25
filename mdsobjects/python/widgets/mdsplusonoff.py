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

import gtk
import gobject
import sys

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'showPath' : (gobject.TYPE_BOOLEAN, 'showPath','include path with checkbutton',False,gobject.PARAM_READWRITE),
        }


class MDSplusOnOffWidget(props,MDSplusWidget,gtk.CheckButton):

    __gtype_name__ = 'MDSplusOnOffWidget'
    __gproperties__ = props.__gproperties__
    
    def reset(self):
        try:
            self.set_active(self.getNode().on)
            if self.showPath:
                self.set_label(str(self.getNode().minpath))
        except Exception:
            print("onoff reset had  problem: %s" % (str(sys.exc_info()),))
            raise

    def apply(self):
        if self.putOnApply:
            if self.getNode().on != self.get_active():
                try:
                    self.getNode().on=self.get_active()
                except Exception:
                    if self.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.getNode.minpath,state,sys.exc_info()))
                    raise
            if self.getNode().on != self.get_active():
                try:
                    self.getNode().on=self.get_active()
                except Exception:
                    if self.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.getNode.minpath,state,sys.exc_info()))
                    raise
        
    def __init__(self):
        gtk.CheckButton.__init__(self)
        MDSplusWidget.__init__(self)
        self.showPath = False
        

gobject.type_register(MDSplusOnOffWidget) 

if guibuilder:
    class MDSplusOnOffWidgetAdaptor(glade.get_adaptor_for_type('GtkCheckButton')):
        __gtype_name__='MDSplusOnOffWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'showPath':
                widget.showPath=value
                if value:
                    widget.set_label('the.node:path')
                else:
                    widget.set_label('')
