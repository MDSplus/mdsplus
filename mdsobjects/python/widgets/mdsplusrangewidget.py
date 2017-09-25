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
