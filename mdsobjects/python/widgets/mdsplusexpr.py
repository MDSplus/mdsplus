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
