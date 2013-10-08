import gtk
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Data
import sys

class MDSplusExpressionWidget(gtk.ScrolledWindow):
    
    def __init__(self,value=None):
        super(MDSplusExpressionWidget,self).__init__()
        self.set_policy(gtk.POLICY_AUTOMATIC,gtk.POLICY_AUTOMATIC)
        tv=gtk.TextView()
        tv.set_wrap_mode(gtk.WRAP_WORD)
        self.buffer=tv.get_buffer()
        self.add(tv)
        self.value=value

    def get_text(self):
        return self.buffer.get_text(self.buffer.get_start_iter(),self.buffer.get_end_iter())

    def set_text(self,text):
        self.buffer.set_text(text)

    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)

    def getValue(self):
        try:
            return Data.compile(self.get_text())
        except Exception:
            MDSplusErrorMsg('Invalid Expression','Invalid expression specified.\n\n%s\n\n%s' % (self.get_text(),sys.exc_info()))
            raise
        
    def setValue(self,d):
        self._value=d
        self.reset()


    def reset(self):
        if self._value is None:
            self.set_text('')
        else:
            self.set_text(str(self._value))

    value=property(getValue,setValue)
