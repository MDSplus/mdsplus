from gtk import Window,VBox,HBox,ComboBox,STOCK_CLOSE,STOCK_CANCEL,STOCK_REDO,Button
from gtk.gdk import KEY_RELEASE
import gobject
from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg
from mdsplusexpr import MDSplusExprWidget

class MDSplusNidOptionWidget(MDSplusWidget,ComboBox):

    __gtype_name__ = 'MDSplusNidOptionWidget'
    __gproperties__ = MDSplusWidget.__gproperties__

    def xdbox_cancel(self,button):
        self.xdbox.set_modal(False)
        self.expr.set_text(self.node_value())
        button.get_toplevel().hide()

    def xdbox_close(self,button):
        try:
            value=self.value
            self.xdbox.set_modal(False)
            button.get_toplevel().hide()
        except:
            pass

    def xdbox_redo(self,button):
        self.expr.set_text(self.node_value())

    def button_press(self,cb,event):
        if event.type == KEY_RELEASE:
            if self.get_model()[self.get_active()][0] == "Computed":
                self.get_toplevel().set_focus(None)
                self.xdbox.set_modal(True)
                self.xdbox.show_all()

    def node_value(self):
        value=self.getNode().record
        if value is None:
            value=''
        else:
            value=str(self.getNode().makeData(value).decompile())
        return value
    
    def reset(self):
        value=self.node_value()
        m=self.get_model()
        if not hasattr(self,"has_been_initialized"):
            self.values=list()
            active=-1
            idx=0
            for i in m:
                item=i[0]
                j = item.find('|')
                if j > -1:
                    m[idx][0]=item[0:j]
                    menu_value=item[j+1:]
                    self.values.append(menu_value)
                idx=idx+1
            self.connect('event',self.button_press)
            self.xdbox=Window()
            vbox=VBox()
            self.expr=MDSplusExprWidget()
            vbox.pack_start(self.expr,True,True,20)
            close=Button(stock=STOCK_CLOSE)
            close.connect("clicked",self.xdbox_close)
            redo=Button(stock=STOCK_REDO)
            redo.connect("clicked",self.xdbox_redo)
            cancel=Button(stock=STOCK_CANCEL)
            cancel.connect("clicked",self.xdbox_cancel)
            hbox=HBox()
            hbox.pack_start(close,False,False,20)
            hbox.pack_start(redo,False,False,20)
            hbox.pack_start(cancel,False,False,20)
            vbox.pack_start(hbox,False,False,20)
            self.xdbox.add(vbox)
            self.has_been_initialized=True
            m.append(row=('Computed',))
        self.set_active(len(self.values))
        for idx in range(len(self.values)):
            val=self.values[idx]
            if val != '':
                val=self.getNode().compile(val).decompile()
            if value == val:
                self.set_active(idx)
                break
        self.expr.set_text(value)
        
    def getValue(self):
        idx=self.get_active()
        if idx < len(self.values):
            value=self.values[idx]
        else:
            value=self.expr.get_text()
        if value == '':
            return None
        else:
            try:
                return self.node.compile(value)
            except Exception,e:
                MDSplusErrorMsg('Invalid value','Invalid value specified.\n\n%s\n\n%s' % (value,e))
                raise

    value=property(getValue)

gobject.type_register(MDSplusNidOptionWidget) 
