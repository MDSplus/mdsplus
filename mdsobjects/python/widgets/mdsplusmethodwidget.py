from gtk import Table,HBox,Label,Entry,ScrolledWindow,POLICY_NEVER,POLICY_ALWAYS,EXPAND,FILL
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Method,Data

class __label__(HBox):
    
    def __init__(self,label):
        super(__label__,self).__init__(False)
        self.pack_start(Label(label),False,False,0)

def adj_changed(adj):
    newstep=adj.upper/8
    if adj.step_increment != newstep:
        adj.step_increment = newstep
        
class MDSplusMethodWidget(Table):

    def __init__(self,value=None):
        super(MDSplusMethodWidget,self).__init__(rows=4,columns=2,homogeneous=False)
        self.device=Entry()
        self.method=Entry()
        self.args=list()
        self.argTable=Table(rows=8,columns=2,homogeneous=False)
        for i in range(8):
            self.args.append(Entry())
            self.argTable.attach(__label__("Arg %d:" % (i+1,)),0,1,i,i+1,0,0)
            self.argTable.attach(self.args[i],1,2,i,i+1,EXPAND|FILL,0)
        self.scrolledWindow=ScrolledWindow()
        self.scrolledWindow.add_with_viewport(self.argTable)
        self.scrolledWindow.set_policy(POLICY_NEVER,POLICY_ALWAYS)
        adj=self.scrolledWindow.get_vadjustment()
        adj.connect("changed",adj_changed)
        self.timeout=Entry()
        self.attach(__label__("Device:"),0,1,0,1,0,0)
        self.attach(self.device,1,2,0,1,EXPAND|FILL,0)
        self.attach(__label__("Method:"),0,1,1,2,0,0)
        self.attach(self.method,1,2,1,2,EXPAND|FILL,0)
        self.attach(self.scrolledWindow,0,2,2,3)
        self.attach(__label__("Timeout:"),0,1,3,4,0,0)
        self.attach(self.timeout,1,2,3,4,EXPAND|FILL,0)
        self.set_row_spacings(5)
        self.value=value

    def getValue(self):
        ans=Method()
        ans.method=self.method.get_text()
        try:
            ans.method=Data.compile(ans.method)
        except Exception,e:
            pass
        try:
            ans.object=Data.compile(self.device.get_text())
        except Exception,e:
            msg="Invalid device specified.\n\n%s" % (e,)
            MDSplusErrorMsg('Invalid Device',msg)
            return None
        if self.timeout.get_text() == '' or self.timeout.get_text() == '*':
            ans.timeout=None
        else:
            try:
                ans.timeout=Data.compile(self.timeout.get_text())
            except Exception,e:
                msg="Invalid timeout specified.\n\n%s" % (e,)
                MDSplusErrorMsg('Invalid Timeout',msg)
                return None
        idx=len(self.args)-1
        found=False
        while idx >= 0:
            t = self.args[idx].get_text()
            if t == '':
                if found:
                    ans.setArgumentAt(idx,None)
            else:
                try:
                    a=Data.compile(t)
                except Exception,e:
                    msg="Invalid argument specified.\n\n%s" % (e,)
                    MDSplusErrorMsg('Invalid Argument',msg)
                    return None
                ans.setArgumentAt(idx,a)
                found=True
            idx=idx-1
        return ans
        
    def setValue(self,d):
        self._value=d
        self.reset()

    def reset(self):
        if isinstance(self._value,Method):
            self.method.set_text(self._value.method.decompile())
            self.device.set_text(self._value.object.decompile())
            if self._value.timeout is None:
                self.timeout.set_text('')
            else:
                self.timeout.set_text(self._value.timeout.decompile())
            idx=0
            for arg in self.args:
                if self._value.getArgumentAt(idx) is None:
                    arg.set_text('')
                else:
                    arg.set_text(self._value.getArgumentAt(idx).decompile())
        else:
            self.method.set_text('')
            self.device.set_text('')
            self.timeout.set_text('')
            for arg in self.args:
                arg.set_text('')

    value=property(getValue,setValue)
        

    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_all()
        self.set_no_show_all(old)

