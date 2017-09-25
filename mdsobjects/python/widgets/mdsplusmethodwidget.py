# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from gtk import Table,Label,Entry,ScrolledWindow,POLICY_NEVER,POLICY_ALWAYS,EXPAND,FILL
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import Method,Data
import sys

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
            self.argTable.attach(Label("Arg %d:       " % (i+1,)),0,1,i,i+1,0,0)
            self.argTable.attach(self.args[i],1,2,i,i+1,EXPAND|FILL,0)
        self.scrolledWindow=ScrolledWindow()
        self.scrolledWindow.add_with_viewport(self.argTable)
        self.scrolledWindow.set_policy(POLICY_NEVER,POLICY_ALWAYS)
        adj=self.scrolledWindow.get_vadjustment()
        adj.connect("changed",adj_changed)
        self.timeout=Entry()
        self.attach(Label("Device:"),0,1,0,1,0,0)
        self.attach(self.device,1,2,0,1,EXPAND|FILL,0)
        self.attach(Label("Method:"),0,1,1,2,0,0)
        self.attach(self.method,1,2,1,2,EXPAND|FILL,0)
        self.attach(self.scrolledWindow,0,2,2,3)
        self.attach(Label("Timeout:"),0,1,3,4,0,0)
        self.attach(self.timeout,1,2,3,4,EXPAND|FILL,0)
        self.set_row_spacings(5)
        self.value=value

    def getValue(self):
        ans=Method()
        ans.method=self.method.get_text()
        try:
            ans.method=Data.compile(ans.method)
        except Exception:
            pass
        try:
            ans.object=Data.compile(self.device.get_text())
        except Exception:
            msg="Invalid device specified.\n\n%s" % (sys.exc_info(),)
            MDSplusErrorMsg('Invalid Device',msg)
            raise
        if self.timeout.get_text() == '' or self.timeout.get_text() == '*':
            ans.timeout=None
        else:
            try:
                ans.timeout=Data.compile(self.timeout.get_text())
            except Exception:
                msg="Invalid timeout specified.\n\n%s" % (sys.exc_info(),)
                MDSplusErrorMsg('Invalid Timeout',msg)
                raise
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
                except Exception:
                    msg="Invalid argument (%d) specified.\n\n%s" % (idx+1,sys.exc_info(),)
                    MDSplusErrorMsg('Invalid Argument',msg)
                    raise
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
                idx=idx+1
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

