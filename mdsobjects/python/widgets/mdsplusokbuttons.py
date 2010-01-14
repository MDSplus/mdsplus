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
