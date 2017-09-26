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

class MDSplusErrorMsg(gtk.Dialog):

    def __init__(self,title,msg):
        super(MDSplusErrorMsg,self).__init__(title=title,flags=gtk.DIALOG_NO_SEPARATOR)
        self.connect("destroy",self.closeErrorWindow)
        self.set_modal(True)
        text=gtk.Label(msg)
        text.set_justify(gtk.JUSTIFY_CENTER)
        align=gtk.Alignment(.5,.5,0,0)
        button=gtk.Button(stock=gtk.STOCK_CLOSE)
        button.connect("clicked",self.closeErrorWindow)
        align.add(button)
        self.vbox.pack_start(text,True,True,0)
        self.vbox.pack_start(align,True,True,0)
        #self.action_area.pack_start(button,False,False,0)
        self.show_all()

    def closeErrorWindow(self,button):
        self.get_toplevel().destroy()
