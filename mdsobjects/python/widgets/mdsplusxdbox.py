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

from gtk import Window,VBox,HBox,CheckButton,Label,Entry,combo_box_new_text,Button,STOCK_OK,STOCK_REDO,STOCK_CANCEL
import gobject
from mdspluswidget import MDSplusWidget
from mdsplusactionwidget import MDSplusActionWidget
from mdsplusdispatchwidget import MDSplusDispatchWidget
from mdsplusmethodwidget import MDSplusMethodWidget
from mdsplusroutinewidget import MDSplusRoutineWidget
from mdsplussequentialwidget import MDSplusSequentialWidget
from mdsplusexpressionwidget import MDSplusExpressionWidget
from mdsplusdtypeselwidget import MDSplusDtypeSelWidget
from mdsplusrangewidget import MDSplusRangeWidget
from mdspluswindowwidget import MDSplusWindowWidget
from mdspluspathwidget import MDSplusPathWidget
import sys

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        }

class MDSplusXdBox(props,MDSplusDtypeSelWidget,MDSplusWidget,Window):

    __gtype_name__ = 'MDSplusXdBox'

    def inHBox(self,w):
        hb=HBox()
        hb.pack_start(w,False,False,0)
        return hb
        
    def __init__(self,node=None,value=None):
        Window.__init__(self)
        MDSplusWidget.__init__(self)
        if node is not None:
            self.node=node
        else:
            self.value=value
        hbtop=HBox(homogeneous=False)
        self.on=CheckButton(label="On")
        self.parent_on=CheckButton(label="Parent")
        self.parent_on.set_sensitive(False)
        self.path=MDSplusPathWidget()
        if node is not None:
            self.path._node=self._node
        hbtags=HBox(homogeneous=False)
        self.tags=Entry()
        self.tags.set_width_chars(60)
        expression_menu=self.dtype_menu(tuple(),varname='dtype_expression',no_show=True)
        axis_menu=self.dtype_menu(('Range',),varname='dtype_axis',no_show=True)
        window_menu=self.dtype_menu(('Window',),varname='dtype_window',no_show=True)
        dispatch_menu=self.dtype_menu(('Dispatch',),varname='dtype_dispatch',no_show=True)
        action_menu=self.dtype_menu(('Action',),varname='dtype_action',no_show=True)
        task_menu=self.dtype_menu(('Method','Routine'),varname='dtype_task',no_show=True)
        any_menu=self.dtype_menu(('Range','Window','Dispatch','Action','Method','Routine'),varname='dtype_any',no_show=True)
        menus=(self.dtype_expression,self.dtype_axis,self.dtype_dispatch,self.dtype_action,self.dtype_task,self.dtype_window,self.dtype_any)
        hbtop.pack_start(self.on,False,False,10)
        hbtop.pack_start(self.parent_on,False,False,10)
        hbtop.pack_start(self.path,False,False,0)
        hbtags.pack_start(Label("Tags:"),False,False,10)
        hbtags.pack_start(self.tags,False,False,0)
        self.action=MDSplusActionWidget()
        self.windoww=MDSplusWindowWidget()
        self.sequential=MDSplusSequentialWidget()
        self.expression=MDSplusExpressionWidget()
        self.method=MDSplusMethodWidget()
        self.routine=MDSplusRoutineWidget()
        self.dispatch=MDSplusDispatchWidget()
        self.range=MDSplusRangeWidget()
        self.widgets=(self.action,self.sequential,self.expression,self.method,self.routine,self.dispatch,self.range,self.windoww)
        self.ok=Button(stock=STOCK_OK)
        self.cancel=Button(stock=STOCK_CANCEL)
        self.redo=Button(stock=STOCK_REDO)
        self.ok.connect("clicked",self.do_ok)
        self.cancel.connect("clicked",self.do_cancel)
        self.redo.connect("clicked",self.do_redo)
        hb2=HBox()
        hb2.add(self.ok)
        hb2.add(self.redo)
        hb2.add(self.cancel)
        vb=VBox(homogeneous=False)
        vb.set_border_width(10)
        vb.pack_start(hbtop,False,False,0)
        vb.pack_start(hbtags,False,False,0)
        vb.pack_start(expression_menu,False,False,0)
        vb.pack_start(axis_menu,False,False,0)
        vb.pack_start(window_menu,False,False,0)
        vb.pack_start(dispatch_menu,False,False,0)
        vb.pack_start(action_menu,False,False,0)
        vb.pack_start(task_menu,False,False,0)
        vb.pack_start(any_menu,False,False,0)
        for w in self.widgets:
            w.set_no_show_all(True)
            vb.pack_start(w,False,False,0)
        vb.pack_start(hb2,False,False,20)
        self.add(vb)
        self.do_redo(self.redo)
        self.putOnApply = True
        self.nidOffset = -1

    def set_dtype(self):
        if hasattr(self,'_node'):
            try:
                exec('self.dtype=self.dtype_%s' % (self.node.usage.lower(),))
            except Exception:
                self.dtype=self.dtype_expression
        else:
            self.dtype=self.dtype_any
        self.dtype.set_no_show_all(False)
            
    def do_ok(self,button):
        try:
            value=self.value
            if not self.putOnApply:
                self.hide()
            elif hasattr(self,'_node'):
                try:
                    self.node.record=value
                    self.hide()
                except Exception:
                    self.popupError(sys.exc_info())
            else:
                self.value=value
                self.hide()
        except:
            pass

    def do_redo(self,button):
        if not hasattr(self,'_node'):
            self.value=_value
            self.set_dtype()
        else:
            self.path.reset()
            self.on.set_active(self.node.disabled==False)
            self.parent_on.set_active(self.node.parent_disabled==False)
            tags = self.node.tags
            if tags is None:
                self.tags.set_text('')
            else:
                taglist=''
                for tag in tags:
                    if taglist == '':
                        taglist=tag
                    else:
                        taglist=taglist+','+tag
                self.tags.set_text(taglist)
            self.set_dtype()
            try:
                self.value=self.record
            except:
                self.value=None
        self.reset()

    def do_cancel(self,button):
        self.hide()

        
gobject.type_register(MDSplusXdBox) 
