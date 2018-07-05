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

from gtk import Label
import gobject

from mdspluswidget import MDSplusWidget

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'useFullPath' : (gobject.TYPE_BOOLEAN, 'useFullPath','display full path',False,gobject.PARAM_READWRITE),
        }

class MDSplusPathWidget(props,MDSplusWidget,Label):

    __gtype_name__ = 'MDSplusPathWidget'
    __gproperties__=props.__gproperties__
    
    def reset(self):
        if guibuilder:
            if self.useFullPath:
                path="\\top.child.child:node"
            else:
                path="device:node"
        else:
            if self.useFullPath:
                path=str(self.node.fullpath)
            else:
                path=str(self.node.minpath)
        self.set_label(path)

    def __init__(self):
        Label.__init__(self)
        self.useFullPath=False

gobject.type_register(MDSplusPathWidget) 

if guibuilder:
    class MDSplusPathWidgetAdaptor(glade.get_adaptor_for_type('GtkLabel')):
        __gtype_name__='MDSplusPathWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'useFullPath':
                widget.useFullPath=value
                widget.reset()
