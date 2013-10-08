from gtk import Entry,Label,Table,EXPAND,FILL,SHRINK
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Data,Window
import sys

class MDSplusWindowWidget(Table):

    def __init__(self,value=None):
        super(MDSplusWindowWidget,self).__init__(rows=3,columns=2,homogeneous=False)
        self.startIdx=Entry()
        self.endIdx=Entry()
        self.timeAt0=Entry()
        self.attach(Label("StartIdx:    "),0,1,0,1,0,0)
        self.attach(self.startIdx,1,2,0,1,EXPAND|FILL,0)
        self.attach(Label("EndIdx:     "),0,1,1,2,0,0)
        self.attach(self.endIdx,1,2,1,2,EXPAND|FILL,0)
        self.attach(Label("TimeAt0:"),0,1,2,3,0,0)
        self.attach(self.timeAt0,1,2,2,3,EXPAND|FILL,0)
        self.set_row_spacings(5)
        self.value=value

    def getValue(self):
        ans=Window()
        t=self.startIdx.get_text()
        if t == '':
            ans.startIdx=None
        else:
            try:
                ans.startIdx=Data.compile(self.startIdx.get_text())
            except Exception:
                msg="Invalid startIdx specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid StartIdx',msg)
                raise
        t=self.endIdx.get_text()
        if t == '':
            ans.endIdx=None
        else:
            try:
                ans.endIdx=Data.compile(self.endIdx.get_text())
            except Exception:
                msg="Invalid endIdx specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid EndIdx',msg)
                raise
        t=self.timeAt0.get_text()
        if t != '':
            try:
                ans.timeAt0=Data.compile(self.timeAt0.get_text())
            except Exception:
                msg="Invalid timeAt0 specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid TimeAt0',msg)
                raise
        return ans
        
    def setValue(self,d):
        self._value=d
        self.reset()

    def reset(self):
        if isinstance(self._value,Window):
            if self._value.startIdx is None:
                self.startIdx.set_text('')
            else:
                self.startIdx.set_text(self._value.startIdx.decompile())
            if self._value.endIdx is None:
                self.endIdx.set_text('')
            else:
                self.endIdx.set_text(self._value.endIdx.decompile())
            if self._value.timeAt0 is None:
                self.timeAt0.set_text('')
            else:
                self.timeAt0.set_text(self._value.timeAt0.decompile())
        else:
            self.startIdx.set_text('')
            self.endIdx.set_text('')
            self.timeAt0.set_text('')
            
    value=property(getValue,setValue)
        
    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)
