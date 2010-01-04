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
