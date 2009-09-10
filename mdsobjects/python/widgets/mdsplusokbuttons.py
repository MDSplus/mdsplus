import gtk
import gtk.glade
import gobject
import inspect
import os
from mdspluswidget import MDSplusWidget

class MDSplusOkButtons(gtk.HButtonBox):

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
        tip=gtk.Tooltips()
        ok=gtk.Button(stock=gtk.STOCK_OK)
        ok.connect("clicked",self.ok_pressed)
        apply=gtk.Button(stock=gtk.STOCK_APPLY)
        apply.connect("clicked",self.apply_pressed)
        reset=gtk.Button(stock=gtk.STOCK_REDO)
        reset.connect("clicked",self.reset_pressed)
        cancel=gtk.Button(stock=gtk.STOCK_CANCEL)
        cancel.connect("clicked",self.cancel_pressed)
        self.add(ok)
        self.add(apply)
        self.add(reset)
        self.add(cancel)

gobject.type_register(MDSplusOkButtons) 
