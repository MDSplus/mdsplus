import gtk
import gobject

from mdspluswidget import MDSplusWidget

class MDSplusOnOff(MDSplusWidget,gtk.CheckButton):

    __gtype_name__ = 'MDSplusOnOff'

    __gproperties__ = MDSplusWidget.__gproperties__
    
    def reset(self):
        try:
            self.set_active(not self.getNode().state)
        except Exception,e:
            print "onoff reset had  problem: %s" % (str(e),)
            raise

    def apply(self):
        if self.putOnApply:
            try:
                self.getNode().state=not self.get_active()
            except Exception,e:
                print "onooff apply had problem; %s" % (str(e),)
        
    def __init__(self):
        gtk.CheckButton.__init__(self)
        MDSplusWidget.__init__(self)
        self.putOnApply = True
        self.nidOffset = 0
        

gobject.type_register(MDSplusOnOff) 
