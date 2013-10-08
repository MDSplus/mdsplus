from gtk import Button
import gobject

from mdspluswidget import MDSplusWidget
from mdsplusxdbox import MDSplusXdBox
from mdspluserrormsg import MDSplusErrorMsg
import sys

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        }

class MDSplusXdBoxButtonWidget(props,MDSplusWidget,Button):

    __gtype_name__ = 'MDSplusXdBoxButtonWidget'
    __gproperties__=props.__gproperties__

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
            except Exception:
                MDSplusErrorMsg('Error storing value','Error storing value in to %s\n\n%s' % (self.node.minpath,sys.exc_info()))

    def popupXd(self,button):
        try:
            self.xdbox.node=self.getNode()
        except:
            pass
        self.xdbox.show()

    def __init__(self):
        Button.__init__(self)
        MDSplusWidget.__init__(self)
        if not guibuilder:
            self.connect("clicked",self.popupXd)
        
gobject.type_register(MDSplusXdBoxButtonWidget) 

if guibuilder:
    class MDSplusXdBoxButtonWidgetAdaptor(glade.get_adaptor_for_type('GtkButton')):
        __gtype_name__='MDSplusXdBoxButtonWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'label':
                widget.set_label(value)
