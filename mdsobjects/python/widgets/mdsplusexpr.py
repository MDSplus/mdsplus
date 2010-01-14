from gtk import Entry,ScrolledWindow,POLICY_AUTOMATIC,TextView,WRAP_WORD
import gobject

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg

class MDSplusExprFieldWidget(MDSplusWidget,Entry):

    __gtype_name__ = 'MDSplusExprFieldWidget'
    __gproperties__ = MDSplusWidget.__gproperties__

    def reset(self):
        try:
            self.set_text(self.record.decompile())
        except Exception,e:
            self.set_text("")
        
    def getValue(self):
        if self.get_text()=="":
            return None
        else:
            try:
                return self.node.compile(self.get_text())
            except Exception,e:
                MDSplusErrorMsg('Invalid value','Invalid value specified.\n\n%s\n\n%s' % (self.get_text(),e))
                raise

    value=property(getValue)
            
gobject.type_register(MDSplusExprFieldWidget) 

class MDSplusExprWidget(MDSplusWidget,ScrolledWindow):

    __gtype_name__ = 'MDSplusExprWidget'
    __gproperties__ = MDSplusWidget.__gproperties__
    
    def get_text(self):
        return self.buffer.get_text(self.buffer.get_start_iter(),self.buffer.get_end_iter())

    def set_text(self,text):
        self.buffer.set_text(text)

    def reset(self):
        try:
            self.set_text(self.record.decompile())
        except Exception,e:
            self.set_text("")
        
    def getValue(self):
        if self.get_text()=="":
            return None
        else:
            try:
                return self.node.compile(self.get_text())
            except Exception,e:
                MDSplusErrorMsg('Invalid value','Invalid valu specified.\n\n%s\n\n%s' % (self.get_text(),e))
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
