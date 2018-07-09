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
