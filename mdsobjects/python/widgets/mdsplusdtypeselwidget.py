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
