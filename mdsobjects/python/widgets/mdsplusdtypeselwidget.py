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

from gtk import HBox,combo_box_new_text

class MDSplusDtypeSelWidget(object):

    def show_dtype(self):
        for w in self.widgets:
            w.hide()
            w.set_no_show_all(True)
        selection=self.dtype.get_model()[self.dtype.get_active()][0].lower()
        if selection == 'window':
            selection='windoww'
        try:
            exec('self.%s.show()' % (selection,))
            exec('self.%s.set_no_show_all(False)' % (selection,))
        except Exception:
            pass
        try:
            self.resize(1,1)
        except:
            pass

    def dtype_menu(self,itemlist,varname='dtype',no_show=False):
        menu=combo_box_new_text()
        for item in itemlist:
            menu.append_text(item)
        menu.append_text("Expression")
        menu.append_text("Undefined")
        menu.set_active(len(itemlist)+1)
        menu.connect('changed',self.dtype_changed)
        exec('self.%s=menu' % (varname,))
        ans=HBox(homogeneous=False)
        exec('ans.pack_start(self.%s,False,False,0)' % (varname,))
        if no_show:
            menu.set_no_show_all(True)
        return ans
    
    def dtype_changed(self,cb):
        self.reset()
        self.show_dtype()

    def getValue(self):
        selected = self.dtype.get_model()[self.dtype.get_active()][0].lower()
        if selected == 'window':
            selected='windoww'
        return eval('self.%s.value' % (selected,))

    def setValue(self,d):
        self._value=d
        self.reset(set_type=True)

    def reset(self,set_type=False):
        if set_type:
            if self._value is None:
                selected="Undefined"
            else:
                selected="Expression"
                for m in self.dtype.get_model():
                    if self._value.__class__.__name__ == m[0]:
                        selected=m[0]
                        break
            dtype=0
            for m in self.dtype.get_model():
                if selected==m[0]:
                    self.dtype.set_active(-1)
                    self.dtype.set_active(dtype)
                    break
                dtype=dtype+1
        else:
            for w in self.widgets:
                w.value=self._value

    value=property(getValue,setValue)

    def show(self):
        old=self.get_no_show_all()
        self.set_no_show_all(False)
        self.show_dtype()
        self.show_all()
        self.set_no_show_all(old)
