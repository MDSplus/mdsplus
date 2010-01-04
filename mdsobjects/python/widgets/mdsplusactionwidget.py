from gtk import HBox
from mdsplusdispatchwidget import MDSplusDispatchWidget
from mdsplustaskwidget import MDSplusTaskWidget
from MDSplus import Action

class MDSplusActionWidget(HBox):

    def __init__(self,value=None):
        super(MDSplusActionWidget,self).__init__(False)
        self.dispatch=MDSplusDispatchWidget()
        self.task=MDSplusTaskWidget()
        self.pack_start(self.dispatch,True,True,10)
        self.pack_start(self.task,True,True,10)
        self.set_border_width(10)
        self.value=value

    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)


    def getValue(self):
        return Action(self.dispatch.value,self.task.value,None,None,None)
        
    def setValue(self,d):
        self._value=d
        self.reset()

    def reset(self):
        if isinstance(self._value,Action):
            self.dispatch.value=self._value.dispatch
            self.task.value=self._value.task
        else:
            self.dispatch.value=None
            self.task.value=None

    value=property(getValue,setValue)
