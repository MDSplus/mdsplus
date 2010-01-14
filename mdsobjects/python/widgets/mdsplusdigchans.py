from gtk import ScrolledWindow,Table,Label,POLICY_NEVER,POLICY_ALWAYS,EXPAND,FILL,Entry,CheckButton
import gobject

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import TreeNode,Data

class MDSplusDigChansWidget(MDSplusWidget,ScrolledWindow):

    __gtype_name__ = 'MDSplusDigChansWidget'

    __gproperties__ = MDSplusWidget.__gproperties__.copy()
    __gproperties__['numChannels'] = (gobject.TYPE_INT, 'numChannels','number of channels',1,1000,8,gobject.PARAM_READWRITE)
    __gproperties__['nodesPerChannel'] = (gobject.TYPE_INT, 'nodesPerChannel','number of nodes per digitizer channel',3,1000,3,gobject.PARAM_READWRITE)
    __gproperties__['dataNidOffset'] = (gobject.TYPE_INT, 'dataNidOffset','nid offset to digitizer channel data node',0,1000,1,gobject.PARAM_READWRITE)
    __gproperties__['startIdxNidOffset'] = (gobject.TYPE_INT, 'startIdxNidOffset','nid offset to digitizer channel startIdx node',0,1000,2,gobject.PARAM_READWRITE)
    __gproperties__['endIdxNidOffset'] = (gobject.TYPE_INT, 'endIdxNidOffset','nid offset to digitizer channel endIdx node',0,1000,3,gobject.PARAM_READWRITE)
    
    def reset(self):
        for channel in self.channels:
            try:
                channel['on'].set_active(channel['dataNode'].on)
            except Exception,e:
                channel['on'].set_active(True)
            try:
                channel['startIdx'].set_text(str(channel['startIdxNode'].record.decompile()))
            except Exception,e:
                channel['startIdx'].set_text('')
            try:
                channel['endIdx'].set_text(str(channel['endIdxNode'].record.decompile()))
            except Exception,e:
                channel['endIdx'].set_text('')
            try:
                channel['path'].set_label(str(channel['dataNode'].minpath))
            except Exception,e:
                channel['path'].set_label('')

    def apply(self):
        if self.putOnApply:
            idx=0
            for channel in self.channels:
                idx=idx+1
                if channel['dataNode'].on != channel['on'].get_active():
                    try:
                        channel['dataNode'].on=channel['on'].get_active()
                    except Exception,e:
                        if channel['on'].get_active():
                            state='on'
                        else:
                            state='off'
                        MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (channel['dataNode'].minpath,state,e))
                        raise
                value=channel['startIdx'].get_text()
                if value == '':
                    value=None
                else:
                    try:
                        value=Data.compile(value)
                    except Exception,e:
                        MDSplusErrorMsg('Invalid value','Error compiling value for startIdx for channel %d\n\n%s\n\n%s' % (idx,value,e))
                        raise
                try:
                    if channel['startIdxNode'].compare(value) != 1:
                        channel['startIdxNode'].record=value
                except Exception,e:
                    MDSplusErrorMsg('Error storing value','Error storing value for startIdx for channel %d\n\n\%s' % (idx,e))
                value=channel['endIdx'].get_text()
                if value == '':
                    value=None
                else:
                    try:
                        value=Data.compile(value)
                    except Exception,e:
                        MDSplusErrorMsg('Invalid value','Error compiling value for endIdx for channel %d\n\n%s\n\n%s' % (idx,value,e))
                        raise
                try:
                    if channel['endIdxNode'].compare(value) != 1:
                        channel['endIdxNode'].record=value
                except Exception,e:
                    MDSplusErrorMsg('Error storing value','Error storing value for endIdx for channel %d\n\n\%s' % (idx,e))

    def adj_changed(self,adj):
        newstep=adj.upper/self.numChannels
        if adj.step_increment != newstep:
            adj.step_increment = newstep
    
    def realize(self,sw):
        import os
        self.table.resize(self.numChannels+1,5)
        self.setupChannels()
        for chan in range(self.numChannels):
            channel=self.channels[chan]
            channel['dataNode']=TreeNode(self.node.nid+chan*self.nodesPerChannel+self.dataNidOffset,self.node.tree)
            channel['startIdxNode']=TreeNode(self.node.nid+chan*self.nodesPerChannel+self.startIdxNidOffset,self.node.tree)
            channel['endIdxNode']=TreeNode(self.node.nid+chan*self.nodesPerChannel+self.endIdxNidOffset,self.node.tree)
        self.show_all()
        self.reset()

    def setupChannels(self):
        for channel in self.channels:
            channel['on'].destroy()
            channel['startIdx'].destroy()
            channel['endIdx'].destroy()
        self.channels=list()
        for chan in range(self.numChannels):
            channel=dict()
            self.channels.append(channel)
            self.table.attach(Label("%d"% (chan+1,)),0,1,chan+1,chan+2,0,0,10,0)
            channel['on']=CheckButton('')
            self.table.attach(channel['on'],1,2,chan+1,chan+2,0,0,10,0)
            channel['startIdx']=Entry()
            self.table.attach(channel['startIdx'],2,3,chan+1,chan+2,EXPAND|FILL,0,10,0)
            channel['endIdx']=Entry()
            self.table.attach(channel['endIdx'],3,4,chan+1,chan+2,EXPAND|FILL,0,10,0)
            channel['path']=Label('                   ')
            self.table.attach(channel['path'],4,5,chan+1,chan+2,0,0,10,0)

    def __init__(self):
        ScrolledWindow.__init__(self)
        MDSplusWidget.__init__(self)
        self.numChannels=4
        self.nodesPerChannel=3
        self.dataNidOffset=0
        self.startIdxNidOffset=1
        self.endIdxNidOffset=2
        self.channels=list()
        self.table=Table(rows=self.numChannels,columns=5,homogeneous=False)
        self.table.attach(Label(""),0,1,0,1,0,0,10,0)
        self.table.attach(Label("On"),1,2,0,1,0,0,10,0)
        self.table.attach(Label("StartIdx"),2,3,0,1,EXPAND|FILL,0,10,0)
        self.table.attach(Label("EndIdx"),3,4,0,1,EXPAND|FILL,0,10,0)
        self.table.attach(Label("Path"),4,5,0,1,0,0,10,0)
        self.table.set_row_spacings(0)
        self.setupChannels()
        self.add_with_viewport(self.table)
        self.set_policy(POLICY_NEVER,POLICY_ALWAYS)
        adj=self.get_vadjustment()
        if adj is not None:
            adj.connect("changed",self.adj_changed)
        self.connect('realize',self.realize)

    def show(self):
        self.show_all()
        
        

gobject.type_register(MDSplusDigChansWidget) 
