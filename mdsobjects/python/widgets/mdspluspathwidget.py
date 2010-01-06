from gtk import Label
import gobject

from mdspluswidget import MDSplusWidget

class MDSplusPathWidget(MDSplusWidget,Label):

    __gtype_name__ = 'MDSplusPathWidget'

    __gproperties__ = {
        'useFullPath' : (gobject.TYPE_BOOLEAN, 'useFullPath','display full path instead of minimum path',False,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,0,gobject.PARAM_READWRITE),
        }
    
    def __init__(self):
        Label.__init__(self,'')
        MDSplusWidget.__init__(self)
        self.useFullPath = False
        self.nidOffset = 0

    def reset(self):
        if self.useFullPath:
            self.set_label(str(self.node.fullpath))
        else:
            self.set_label(str(self.node.minpath))

gobject.type_register(MDSplusPathWidget) 
