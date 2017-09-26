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
