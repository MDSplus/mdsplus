from gtk import Window,VBox,HBox,CheckButton,Label,Entry,combo_box_new_text,Button,STOCK_APPLY,STOCK_REDO,STOCK_CANCEL
import gobject
from mdspluswidget import MDSplusWidget
from mdsplusactionwidget import MDSplusActionWidget
from mdsplusdispatchwidget import MDSplusDispatchWidget
from mdsplusmethodwidget import MDSplusMethodWidget
from mdsplusroutinewidget import MDSplusRoutineWidget
from mdsplussequentialwidget import MDSplusSequentialWidget
from mdspluserrormsg import MDSplusErrorMsg
from mdsplusexpressionwidget import MDSplusExpressionWidget
from mdsplusdtypeselwidget import MDSplusDtypeSelWidget
from mdsplusrangewidget import MDSplusRangeWidget
from mdspluswindowwidget import MDSplusWindowWidget
from mdspluspathwidget import MDSplusPathWidget

class MDSplusXdBox(MDSplusDtypeSelWidget,MDSplusWidget,Window):

    __gtype_name__ = 'MDSplusXdBox'

    __gproperties__ = MDSplusWidget.__gproperties__

    def inHBox(self,w):
        hb=HBox()
        hb.pack_start(w,False,False,0)
        return hb
        
    def __init__(self,node=None,value=None):
        Window.__init__(self)
        MDSplusWidget.__init__(self)
        hbtop=HBox(homogeneous=False)
        self.on=CheckButton(label="On")
        self.parent_on=CheckButton(label="Parent")
        self.parent_on.set_sensitive(False)
        self.path=MDSplusPathWidget()
        hbtags=HBox(homogeneous=False)
        self.tags=Entry()
        self.tags.set_width_chars(60)
        expression_menu=self.dtype_menu(tuple(),varname='dtype_expression',no_show=True)
        axis_menu=self.dtype_menu(('Range',),varname='dtype_axis',no_show=True)
        window_menu=self.dtype_menu(('Window',),varname='dtype_window',no_show=True)
        dispatch_menu=self.dtype_menu(('Dispatch',),varname='dtype_dispatch',no_show=True)
        action_menu=self.dtype_menu(('Action',),varname='dtype_action',no_show=True)
        task_menu=self.dtype_menu(('Method','Routine'),varname='dtype_task',no_show=True)
        any_menu=self.dtype_menu(('Range','Window','Dispatch','Action','Method','Routine'),varname='dtype_any',no_show=True)
        menus=(self.dtype_expression,self.dtype_axis,self.dtype_dispatch,self.dtype_action,self.dtype_task,self.dtype_window,self.dtype_any)
        hbtop.pack_start(self.on,False,False,10)
        hbtop.pack_start(self.parent_on,False,False,10)
        hbtop.pack_start(self.path,False,False,0)
        hbtags.pack_start(Label("Tags:"),False,False,10)
        hbtags.pack_start(self.tags,False,False,0)
        self.action=MDSplusActionWidget()
        self.windoww=MDSplusWindowWidget()
        self.sequential=MDSplusSequentialWidget()
        self.expression=MDSplusExpressionWidget()
        self.method=MDSplusMethodWidget()
        self.routine=MDSplusRoutineWidget()
        self.dispatch=MDSplusDispatchWidget()
        self.range=MDSplusRangeWidget()
        self.widgets=(self.action,self.sequential,self.expression,self.method,self.routine,self.dispatch,self.range,self.windoww)
        apply=Button(stock=STOCK_APPLY)
        cancel=Button(stock=STOCK_CANCEL)
        redo=Button(stock=STOCK_REDO)
        apply.connect("clicked",self.do_apply)
        cancel.connect("clicked",self.do_cancel)
        redo.connect("clicked",self.do_redo)
        hb2=HBox()
        hb2.add(apply)
        hb2.add(redo)
        hb2.add(cancel)
        vb=VBox(homogeneous=False)
        vb.set_border_width(10)
        vb.pack_start(hbtop,False,False,0)
        vb.pack_start(hbtags,False,False,0)
        vb.pack_start(expression_menu,False,False,0)
        vb.pack_start(axis_menu,False,False,0)
        vb.pack_start(window_menu,False,False,0)
        vb.pack_start(dispatch_menu,False,False,0)
        vb.pack_start(action_menu,False,False,0)
        vb.pack_start(task_menu,False,False,0)
        vb.pack_start(any_menu,False,False,0)
        for w in self.widgets:
            w.set_no_show_all(True)
            vb.pack_start(w,False,False,0)
        vb.pack_start(hb2,False,False,20)
        self.add(vb)
        if node is not None:
            self.node=node
        else:
            self.value=value
        self.do_redo(redo)
        self.putOnApply = True
        self.nidOffset = 0

    def set_dtype(self):
        if hasattr(self,'_node'):
            try:
                exec 'self.dtype=self.dtype_%s' % (self.node.usage.lower(),)
            except Exception,e:
                self.dtype=self.dtype_expression
        else:
            self.dtype=self.dtype_any
        self.dtype.set_no_show_all(False)
            
    def do_apply(self,button):
        try:
            value=self.value
            if hasattr(self,'_node'):
                try:
                    self.node.record=value
                except Exception,e:
                    self.popupError(e)
                else:
                    self.value=value
        except:
            pass

    def do_redo(self,button):
        if not hasattr(self,'_node'):
            self.value=_value
            self.set_dtype()
        else:
            self.path.reset()
            self.on.set_active(self.node.disabled==False)
            self.parent_on.set_active(self.node.parent_disabled==False)
            tags = self.node.tags
            if tags is None:
                self.tags.set_text('')
            else:
                taglist=''
                for tag in tags:
                    if taglist == '':
                        taglist=tag
                    else:
                        taglist=taglist+','+tag
                self.tags.set_text(taglist)
            self.set_dtype()
            try:
                self.value=self.record
            except:
                self.value=None
        self.reset()

    def do_cancel(self,button):
        self.destroy()

        
gobject.type_register(MDSplusXdBox) 
