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

from gtk import Entry,ScrolledWindow,POLICY_AUTOMATIC,TextView,WRAP_WORD
import gobject
import sys

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class propsExpr(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        }

class propsExprField(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        }




class MDSplusExprFieldWidget(propsExprField,MDSplusWidget,Entry):

    __gtype_name__ = 'MDSplusExprFieldWidget'

    __gproperties__ = propsExprField.__gproperties__
    
    def reset(self):
        try:
            self.set_text(self.record.decompile())
        except Exception:
            self.set_text("")
        
    def getValue(self):
        if self.get_text()=="":
            return None
        else:
            try:
                return self.node.compile(self.get_text())
            except Exception:
                MDSplusErrorMsg('Invalid value','Invalid value specified.\n\n%s\n\n%s' % (self.get_text(),sys.exc_info()))
                raise

    value=property(getValue)
            
gobject.type_register(MDSplusExprFieldWidget) 

class MDSplusExprWidget(propsExpr,MDSplusWidget,ScrolledWindow):

    __gtype_name__ = 'MDSplusExprWidget'
    __gproperties__ = propsExpr.__gproperties__
    
    def get_text(self):
        return self.buffer.get_text(self.buffer.get_start_iter(),self.buffer.get_end_iter())

    def set_text(self,text):
        self.buffer.set_text(text)

    def reset(self):
        try:
            self.set_text(self.record.decompile())
        except Exception:
            self.set_text("")
        
    def getValue(self):
        if self.get_text()=="":
            return None
        else:
            try:
                return self.node.compile(self.get_text())
            except Exception:
                MDSplusErrorMsg('Invalid value','Invalid value specified.\n\n%s\n\n%s' % (self.get_text(),sys.exc_info()))
                raise

    value=property(getValue)

    def __init__(self):
        ScrolledWindow.__init__(self)
        MDSplusWidget.__init__(self)
        tv=TextView()
        tv.set_wrap_mode(WRAP_WORD)
        self.buffer=tv.get_buffer()
        self.add(tv)
        
gobject.type_register(MDSplusExprWidget) 

if guibuilder:
    class MDSplusExprFieldWidgetAdaptor(glade.get_adaptor_for_type('GtkEntry')):
        __gtype_name__='MDSplusExprFieldWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value

    class MDSplusExprWidgetAdaptor(glade.get_adaptor_for_type('GtkScrolledWindow')):
        __gtype_name__='MDSplusExprWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
