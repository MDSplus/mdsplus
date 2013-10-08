from gtk import Entry,Label,Table,EXPAND,FILL,SHRINK
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Data,Dispatch
import os

class MDSplusSequentialWidget(Table):

    def __init__(self,value=None):
        super(MDSplusSequentialWidget,self).__init__(rows=4,columns=2,homogeneous=False)
        self.ident=Entry()
        self.phase=Entry()
        self.sequence=Entry()
        self.event=Entry()
        self.attach(Label("Server:    "),0,1,0,1,0,0)
        self.attach(self.ident,1,2,0,1,EXPAND|FILL,0)
        self.attach(Label("Phase:     "),0,1,1,2,0,0)
        self.attach(self.phase,1,2,1,2,EXPAND|FILL,0)
        self.attach(Label("Sequence:"),0,1,2,3,0,0)
        self.attach(self.sequence,1,2,2,3,EXPAND|FILL,0)
        self.attach(Label("Event:      "),0,1,3,4,0,0)
        self.attach(self.event,1,2,3,4,EXPAND|FILL,0)
        self.set_row_spacings(5)
        self.value=value

    def getValue(self):
        ans=Dispatch(dispatch_type=2)
        ans.ident=self.ident.get_text()
        try:
            ans.ident=Data.compile(ans.ident)
        except Exception:
            pass
        ans.phase=self.phase.get_text()
        try:
            ans.phase=Data.compile(ans.phase)
        except Exception:
            pass
        try:
            ans.when=Data.compile(self.sequence.get_text())
        except Exception:
            msg="Invalid sequence specified.\n\n%s" % (sys.exc_info(),)
            MDSplusErrorMsg('Invalid Sequence',msg)
            raise
        ans.completion=self.event.get_text()
        if ans.completion == '':
            ans.completion=None
        else:
            try:
                ans.completion=Data.compile(ans.event)
            except Exception:
                pass
        return ans
        
    def setValue(self,d):
        self._value=d
        self.reset()

    def reset(self):
        if isinstance(self._value,Dispatch):
            self.ident.set_text(self._value.ident.decompile())
            self.phase.set_text(self._value.phase.decompile())
            self.sequence.set_text(self._value.when.decompile())
            if self._value.completion is None:
                self.event.set_text('')
            else:
                self.event.set_text(self._value.completion.decompile())
        else:
            self.ident.set_text('')
            self.phase.set_text('')
            self.sequence.set_text('')
            self.event.set_text('')
            
    value=property(getValue,setValue)
        
    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)
