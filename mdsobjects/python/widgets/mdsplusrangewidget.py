from gtk import HBox,Entry,Label,Table,EXPAND,FILL,SHRINK
import sys
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Data,Range

class MDSplusRangeWidget(Table):

    def __init__(self,value=None):
        super(MDSplusRangeWidget,self).__init__(rows=3,columns=2,homogeneous=False)
        self.begin=Entry()
        self.ending=Entry()
        self.delta=Entry()
        self.attach(Label("Begin:    "),0,1,0,1,0,0)
        self.attach(self.begin,1,2,0,1,EXPAND|FILL,0)
        self.attach(Label("Ending:     "),0,1,1,2,0,0)
        self.attach(self.ending,1,2,1,2,EXPAND|FILL,0)
        self.attach(Label("Delta:"),0,1,2,3,0,0)
        self.attach(self.delta,1,2,2,3,EXPAND|FILL,0)
        self.set_row_spacings(5)
        self.value=value

    def getValue(self):
        ans=Range()
        t=self.begin.get_text()
        if t == '':
            ans.begin=None
        else:
            try:
                ans.begin=Data.compile(self.begin.get_text())
            except Exception:
                msg="Invalid begin specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid Begin',msg)
                raise
        t=self.ending.get_text()
        if t == '':
            ans.ending=None
        else:
            try:
                ans.ending=Data.compile(self.ending.get_text())
            except Exception:
                msg="Invalid ending specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid Ending',msg)
                raise
        t=self.delta.get_text()
        if t != '':
            try:
                ans.delta=Data.compile(self.delta.get_text())
            except Exception:
                msg="Invalid delta specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid Delta',msg)
                raise
        return ans
        
    def setValue(self,d):
        self._value=d
        self.reset()

    def reset(self):
        if isinstance(self._value,Range):
            if self._value.begin is None:
                self.begin.set_text('')
            else:
                self.begin.set_text(self._value.begin.decompile())
            if self._value.ending is None:
                self.ending.set_text('')
            else:
                self.ending.set_text(self._value.ending.decompile())
            if self._value.delta is None:
                self.delta.set_text('')
            else:
                self.delta.set_text(self._value.delta.decompile())
        else:
            self.begin.set_text('')
            self.ending.set_text('')
            self.delta.set_text('')
            
    value=property(getValue,setValue)
        
    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)
