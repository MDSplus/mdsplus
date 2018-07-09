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
