from gtk import Label
import gobject

from mdspluswidget import MDSplusWidget

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'useFullPath' : (gobject.TYPE_BOOLEAN, 'useFullPath','display full path',False,gobject.PARAM_READWRITE),
        }

class MDSplusPathWidget(props,MDSplusWidget,Label):

    __gtype_name__ = 'MDSplusPathWidget'
    __gproperties__=props.__gproperties__
    
    def reset(self):
        if guibuilder:
            if self.useFullPath:
                path="\\top.child.child:node"
            else:
                path="device:node"
        else:
            if self.useFullPath:
                path=str(self.node.fullpath)
            else:
                path=str(self.node.minpath)
        self.set_label(path)

    def __init__(self):
        Label.__init__(self)
        self.useFullPath=False

gobject.type_register(MDSplusPathWidget) 

if guibuilder:
    class MDSplusPathWidgetAdaptor(glade.get_adaptor_for_type('GtkLabel')):
        __gtype_name__='MDSplusPathWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'useFullPath':
                widget.useFullPath=value
                widget.reset()
