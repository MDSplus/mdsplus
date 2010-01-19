from gtk import HBox,CheckButton,Button,Label
import gobject

from mdspluswidget import MDSplusWidget
from mdsplusxdbox import MDSplusXdBox
from mdspluserrormsg import MDSplusErrorMsg

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'buttonLabel' : (gobject.TYPE_STRING, 'buttonLabel','Label on popup button','',gobject.PARAM_READWRITE),
        }

class MDSplusOnOffXdBoxWidget(props,MDSplusWidget,HBox):

    __gtype_name__ = 'MDSplusOnOffXdBoxWidget'
    __gproperties__= props.__gproperties__

    def reset(self):
        if not hasattr(self,'xdbox'):
            self.xdbox=MDSplusXdBox(self.node)
            self.xdbox.putOnApply=False
        self.node_state.set_active(self.node.on)
        self.node_state.set_label('')
        self.xdbox.reset()

    def apply(self):
        if self.putOnApply:
            if self.node.on != self.node_state.get_active():
                try:
                    self.node.on=self.node_state.get_active()
                except Exception,e:
                    if self.node_state.get_active():
                        state='on'
                    else:
                        state='off'
                    MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (self.node.minpath,state,e))
                    raise
            try:
                if self.node.compare(self.xdbox.value) != 1:
                    self.node.record=self.xdbox.value
                self.reset()
            except Exception,e:
                MDSplusErrorMsg('Error storing value','Error storing value in to %s\n\n%s' % (self.node.minpath,e))

    def popupXd(self,button):
        self.xdbox.node=self.getNode()
        self.xdbox.set_title(self.buttonLabel)
        self.xdbox.show()

    def setButtonLabel(self,button):
        self.button.set_label(self.buttonLabel)

    def __init__(self):
        HBox.__init__(self)
        MDSplusWidget.__init__(self)
        HBox.set_homogeneous(self,False)
        self.node_state=CheckButton('')
        self.button=Button()
        HBox.pack_start(self,self.node_state,False,False,0)
        HBox.pack_start(self,self.button,False,False,0)
        HBox.pack_start(self,Label(''),True,False,0)
        try:
            import glade
        except:
            self.button.connect("clicked",self.popupXd)
        self.button.connect("realize",self.setButtonLabel)

    def show(self):
        self.show_all()
        
        

gobject.type_register(MDSplusOnOffXdBoxWidget) 

try:
    import glade

    class MDSplusOnOffXdboxWidgetAdaptor(glade.get_adaptor_for_type('GtkHBox')):
        __gtype_name__='MDSplusOnOffXdBoxWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'buttonLabel':
                widget.buttonLabel=value
                widget.button.set_label(value)
except:
    pass
