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

from gtk import HButtonBox,Tooltips,Button,STOCK_OK,STOCK_APPLY,STOCK_REDO,STOCK_CANCEL
import gobject
from mdspluswidget import MDSplusWidget

class MDSplusOkButtons(HButtonBox):

    __gtype_name__ = 'MDSplusOkButtons'
    
    def do_apply(self):
        return MDSplusWidget.doToAll(self.get_toplevel(),"apply")
        
    def do_reset(self):
        return MDSplusWidget.doToAll(self.get_toplevel(),"reset")

    def closeTop(self):
        self.get_toplevel().destroy()
        
    def ok_pressed(self,button):
        if self.do_apply():
            self.closeTop()

    def apply_pressed(self,button):
        self.do_apply()

    def reset_pressed(self,button):
        self.do_reset()
    
    def cancel_pressed(self,button):
        self.closeTop()

    def __init__(self):
        tip=Tooltips()
        ok=Button(stock=STOCK_OK)
        ok.connect("clicked",self.ok_pressed)
        apply=Button(stock=STOCK_APPLY)
        apply.connect("clicked",self.apply_pressed)
        reset=Button(stock=STOCK_REDO)
        reset.connect("clicked",self.reset_pressed)
        cancel=Button(stock=STOCK_CANCEL)
        cancel.connect("clicked",self.cancel_pressed)
        self.add(ok)
        self.add(apply)
        self.add(reset)
        self.add(cancel)

gobject.type_register(MDSplusOkButtons) 
