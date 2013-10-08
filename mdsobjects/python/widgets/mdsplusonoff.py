import gtk
import gobject
import sys

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'showPath' : (gobject.TYPE_BOOLEAN, 'showPath','include path with checkbutton',False,gobject.PARAM_READWRITE),
        }


class MDSplusOnOffWidget(props,MDSplusWidget,gtk.CheckButton):

    __gtype_name__ = 'MDSplusOnOffWidget'
    __gproperties__ = props.__gproperties__
    
    def reset(self):
        try:
            self.set_active(self.getNode().on)
            if self.showPath:
                self.set_label(str(self.getNode().minpath))
        except Exception:
            print("onoff reset had  problem: %s" % (str(sys.exc_info()),))
            raise

    def apply(self):
        if self.putOnApply:
            if self.getNode().on != self.get_active():
                try:
                    self.getNode().on=self.get_active()
                except Exception:
                    if self.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.getNode.minpath,state,sys.exc_info()))
                    raise
            if self.getNode().on != self.get_active():
                try:
                    self.getNode().on=self.get_active()
                except Exception:
                    if self.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.getNode.minpath,state,sys.exc_info()))
                    raise
        
    def __init__(self):
        gtk.CheckButton.__init__(self)
        MDSplusWidget.__init__(self)
        self.showPath = False
        

gobject.type_register(MDSplusOnOffWidget) 

if guibuilder:
    class MDSplusOnOffWidgetAdaptor(glade.get_adaptor_for_type('GtkCheckButton')):
        __gtype_name__='MDSplusOnOffWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'showPath':
                widget.showPath=value
                if value:
                    widget.set_label('the.node:path')
                else:
                    widget.set_label('')
