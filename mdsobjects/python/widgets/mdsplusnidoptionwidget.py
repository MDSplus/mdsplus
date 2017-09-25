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

from gtk import Window,VBox,HBox,ComboBox,STOCK_CLOSE,STOCK_CANCEL,STOCK_REDO,Button,ListStore,CellRendererText
from gtk.gdk import KEY_RELEASE
import gobject
from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg
from mdsplusexpr import MDSplusExprWidget
import sys

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    if guibuilder:
        __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        }
    else:
        __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'values' : (gobject.TYPE_STRING, 'values','Value expressions','',gobject.PARAM_READWRITE),
        }

class MDSplusNidOptionWidget(props,MDSplusWidget,ComboBox):

    __gtype_name__ = 'MDSplusNidOptionWidget'
    __gproperties__ = props.__gproperties__

    def xdbox_cancel(self,button):
        self.xdbox.set_modal(False)
        self.xdbox.expr.set_text(self.node_value())
        button.get_toplevel().hide()

    def xdbox_close(self,button):
        try:
            value=self.value
            self.xdbox.set_modal(False)
            button.get_toplevel().hide()
        except:
            pass

    def xdbox_redo(self,button):
        self.xdbox.expr.set_text(self.node_value())

    def button_press(self,cb,event):
        if self.get_active_text() == "Computed":
            if self.get_property('popup-shown'):
                self.popup_xd=True
            else:
                if self.popup_xd:
                    if not hasattr(self,'xdbox'):
                        self.initXdbox()
                    self.get_toplevel().set_focus(None)
                    self.xdbox.set_modal(False)
                    self.xdbox.show_all()
                    self.popup_xd=False

    def node_value(self):
        value=self.record
        if value is None:
            value=''
        else:
            value=str(value.decompile())
        return value

    def fixValues(self):
        idx=0
        for value in self.values:
            if value.find('<DEVTOP>') > -1:
                try:
                    self.values[idx]=value.replace('<DEVTOP>',str(self.devnode))
                except Exception:
                    pass
            idx=idx+1


    def updateItems(self):
        if not hasattr(self,'values'):
            return
        m=self.get_model()
        if isinstance(self.values,str):
            self.values=self.values.split('\n')
        self.fixValues()
        if len(self.values) > len(m):
            self.values=self.values[0:len(m)-1]
        self.append_text('Computed')


    def initXdbox(self):
        self.xdbox=Window()
        try:
            self.xdbox.set_title(str(self.node))
        except:
            pass
        vbox=VBox()
        self.xdbox.expr=MDSplusExprWidget()
        vbox.pack_start(self.xdbox.expr,True,True,20)
        close=Button(stock=STOCK_CLOSE)
        close.connect("clicked",self.xdbox_close)
        redo=Button(stock=STOCK_REDO)
        redo.connect("clicked",self.xdbox_redo)
        cancel=Button(stock=STOCK_CANCEL)
        cancel.connect("clicked",self.xdbox_cancel)
        hbox=HBox()
        hbox.pack_start(close,False,False,20)
        hbox.pack_start(redo,False,False,20)
        hbox.pack_start(cancel,False,False,20)
        vbox.pack_start(hbox,False,False,20)
        self.xdbox.add(vbox)
        self.xdbox.expr.set_text(self.node_value())

    def reset(self):
        value=self.node_value()
        if not hasattr(self,"has_been_initialized"):
            self.updateItems()
            self.has_been_initialized=True
            if not guibuilder:
                self.popup_xd=True
                self.connect('event',self.button_press)
        if hasattr(self,'values'):
            self.set_active(len(self.values))
            if not guibuilder:
                for idx in range(len(self.values)):
                    val=self.values[idx]
                    if val != '':
                        try:
                            newval=self.getNode().compile(val).decompile()
                            val=str(newval)
                        except Exception:
                            print("Invalid value specified for an option item. Value string was <%s>. \nError was %s." % (val,sys.exc_info()))
                    if value == val:
                        self.set_active(idx)
                        break
        if hasattr(self,'xdbox'):
            self.xdbox.expr.set_text(value)

    def getValue(self):
        idx=self.get_active()
        if idx < len(self.values):
            value=self.values[idx]
        elif hasattr(self,'xdbox'):
            value=self.xdbox.expr.get_text()
        else:
            value=self.node_value()
        if value == '':
            return None
        else:
            try:
                return self.node.compile(value)
            except Exception:
                MDSplusErrorMsg('Invalid value','Invalid value specified.\n\n%s\n\n%s' % (value,sys.exc_info()))
                raise

    value=property(getValue)

gobject.type_register(MDSplusNidOptionWidget) 

if guibuilder:
    class MDSplusNidOptionWidgetAdaptor(glade.get_adaptor_for_type('GtkComboBox')):
        __gtype_name__='MDSplusNidOptionWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'items':
                widget.get_model()
                for item in value:
                    widget.append_text(item)
                widget.updateItems()
                if hasattr(widget,'values'):
                    widget.set_active(len(widget.values))
            elif prop == 'values':
                widget.values=value
