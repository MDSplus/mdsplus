from gtk import Button
import gobject

from mdspluswidget import MDSplusWidget
from mdsplusxdbox import MDSplusXdBox
from mdspluserrormsg import MDSplusErrorMsg

class MDSplusXdBoxButtonWidget(MDSplusWidget,Button):

    __gtype_name__ = 'MDSplusXdBoxButtonWidget'

    __gproperties__ = MDSplusWidget.__gproperties__
    
    def reset(self):
        if not hasattr(self,'xdbox'):
            self.xdbox=MDSplusXdBox(node=self.node)
            self.xdbox.putOnApply=False
        self.xdbox.reset()

    def apply(self):
        if self.putOnApply:
            try:
                if self.node.compare(self.xdbox.value) != 1:
                    self.node.record=self.xdbox.value
                self.reset()
            except Exception,e:
                MDSplusErrorMsg('Error storing value','Error storing value in to %s\n\n%s' % (self.node.minpath,e))

    def popupXd(self,button):
        self.xdbox.node=self.getNode()
        self.xdbox.show()

    def __init__(self):
        Button.__init__(self)
        MDSplusWidget.__init__(self)
        self.connect("clicked",self.popupXd)
        
gobject.type_register(MDSplusXdBoxButtonWidget) 
