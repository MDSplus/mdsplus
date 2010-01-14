import gtk
import gobject

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg

class MDSplusOnOffWidget(MDSplusWidget,gtk.CheckButton):

    __gtype_name__ = 'MDSplusOnOffWidget'

    __gproperties__ = MDSplusWidget.__gproperties__.copy()
    __gproperties__['showPath'] = (gobject.TYPE_BOOLEAN, 'showPath','include path with checkbutton',False,gobject.PARAM_READWRITE)
    
    def reset(self):
        try:
            self.set_active(self.getNode().on)
            if self.showPath:
                self.set_label(str(self.getNode().minpath))
        except Exception,e:
            print "onoff reset had  problem: %s" % (str(e),)
            raise

    def apply(self):
        if self.putOnApply:
            if self.getNode().on != self.get_active():
                try:
                    self.getNode().on=self.get_active()
                except Exception,e:
                    if self.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.getNode.minpath,state,e))
                    raise
        
    def __init__(self):
        gtk.CheckButton.__init__(self)
        MDSplusWidget.__init__(self)
        self.showPath = False
        

gobject.type_register(MDSplusOnOffWidget) 
