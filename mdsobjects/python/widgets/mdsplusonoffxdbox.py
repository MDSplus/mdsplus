from gtk import HBox,CheckButton,Button,Label
import gobject

from mdspluswidget import MDSplusWidget
from mdsplusxdbox import MDSplusXdBox
from mdspluserrormsg import MDSplusErrorMsg

class MDSplusOnOffXdBoxWidget(MDSplusWidget,HBox):

    __gtype_name__ = 'MDSplusOnOffXdBoxWidget'

    __gproperties__ = MDSplusWidget.__gproperties__.copy()
    __gproperties__['buttonLabel'] = (gobject.TYPE_STRING, 'buttonLabel','Label on popup button','',gobject.PARAM_READWRITE)
    
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

    def set_homogeneous(*args,**kwargs):
        pass

    def pack_start(*args,**kwargs):
        pass

    def add(*args,**kwargs):
        pass

    def __init__(self):
        HBox.__init__(self)
        MDSplusWidget.__init__(self)
        HBox.set_homogeneous(self,False)
        self.node_state=CheckButton('')
        self.button=Button()
        HBox.pack_start(self,self.node_state,False,False,0)
        HBox.pack_start(self,self.button,False,False,0)
        HBox.pack_start(self,Label(''),True,False,0)
        self.button.connect("clicked",self.popupXd)
        self.button.connect("realize",self.setButtonLabel)

    def show(self):
        self.show_all()
        
        

gobject.type_register(MDSplusOnOffXdBoxWidget) 
