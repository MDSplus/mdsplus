import gtk
import gobject

from mdspluswidget import MDSplusWidget

class MDSplusExpr(MDSplusWidget,gtk.Entry):

    __gtype_name__ = 'MDSplusExpr'

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
            return self.node.compile(self.get_text())

    value=property(getValue)
    
    def __init__(self):
        gtk.Entry.__init__(self)
        MDSplusWidget.__init__(self)
        self.putOnApply = True
        self.nidOffset = 0
        

gobject.type_register(MDSplusExpr) 
