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

from gtk import ScrolledWindow,Table,Label,POLICY_NEVER,POLICY_ALWAYS,EXPAND,FILL,Entry,CheckButton
import gobject
import sys

from mdspluswidget import MDSplusWidget
from mdspluserrormsg import MDSplusErrorMsg
from MDSplus import TreeNode,Data

try:
    import glade
    guibuilder=True
except:
    guibuilder=False

class props(object):
    __gproperties__= {
        'putOnApply' : (gobject.TYPE_BOOLEAN, 'putOnApply','put when apply button pressed',True,gobject.PARAM_READWRITE),
        'nidOffset' : (gobject.TYPE_INT, 'nidOffset','Offset of nid in tree',-1,100000,-1,gobject.PARAM_READWRITE),
        'numChannels' : (gobject.TYPE_INT, 'numChannels','number of channels',1,1000,8,gobject.PARAM_READWRITE),
        'nodesPerChannel' : (gobject.TYPE_INT, 'nodesPerChannel','number of nodes per digitizer channel',3,1000,3,gobject.PARAM_READWRITE),
        'dataNidOffset' : (gobject.TYPE_INT, 'dataNidOffset','nid offset to digitizer channel data node',0,1000,1,gobject.PARAM_READWRITE),
        'startIdxNidOffset' : (gobject.TYPE_INT, 'startIdxNidOffset','nid offset to digitizer channel startIdx node',0,1000,2,gobject.PARAM_READWRITE),
        'endIdxNidOffset' : (gobject.TYPE_INT, 'endIdxNidOffset','nid offset to digitizer channel endIdx node',0,1000,3,gobject.PARAM_READWRITE),
        'incNidOffset' : (gobject.TYPE_INT, 'incNidOffset','nid offset to digitizer channel increment node',-1,1000,-1,gobject.PARAM_READWRITE),
        }


class MDSplusDigChansWidget(props,MDSplusWidget,ScrolledWindow):

    __gtype_name__ = 'MDSplusDigChansWidget'

    __gproperties__= props.__gproperties__


    def resetPart(self,channel,field):
        try:
            channel[field].set_text(str(channel[field+'Node'].record.decompile()))
        except Exception:
            channel[field].set_text('')
        

    def reset(self):
        if not hasattr(self,'channels'):
            self.setupChannels()
        if hasattr(self,'channels'):
            for channel in self.channels:
                try:
                    channel['on'].set_active(channel['dataNode'].on)
                except Exception:
                    channel['on'].set_active(True)
                self.resetPart(channel,'startIdx')
                self.resetPart(channel,'endIdx')
                if self.incNidOffset > -1:
                    self.resetPart(channel,'inc')
                try:
                    channel['path'].set_label(str(channel['dataNode'].minpath))
                except Exception:
                    channel['path'].set_label('')

    def applyPart(self,channel,field,idx):
        value=channel[field].get_text()
        if value == '':
            value=None
        else:
            try:
                value=Data.compile(value)
            except Exception:
                MDSplusErrorMsg('Invalid value','Error compiling %s for channel %d\n\n%s\n\n%s' % (field,idx,value,sys.exc_info()))
                raise
        try:
            if channel[field+'Node'].compare(value) != 1:
                channel[field+'Node'].record=value
        except Exception:
            MDSplusErrorMsg('Error storing value','Error storing value %s for channel %d\n\n\%s' % (field,idx,sys.exc_info()))
        
    def apply(self):
        if self.putOnApply:
            idx=0
            for channel in self.channels:
                idx=idx+1
                if channel['dataNode'].on != channel['on'].get_active():
                    try:
                        channel['dataNode'].on=channel['on'].get_active()
                    except Exception:
                        if channel['on'].get_active():
                            state='on'
                        else:
                            state='off'
                        MDSplusErrorMsg('Error setting node on/off state','Error turning node %s %s\n\n%s' % (channel['dataNode'].minpath,state,sys.exc_info()))
                        raise
                self.applyPart(channel,'startIdx',idx)
                self.applyPart(channel,'endIdx',idx)
                if self.incNidOffset > -1:
                    self.applyPart(channel,'inc',idx)

    def adj_changed(self,adj):
        newstep=adj.upper/self.numChannels
        if adj.step_increment != newstep:
            adj.step_increment = newstep
    
    def setupChannels(self):
        for child in self.table.get_children():
            child.destroy()
        if self.incNidOffset > -1:
            columns=6
        else:
            columns=5
        self.table.resize(self.numChannels+1,columns)
        self.table.attach(Label(""),0,1,0,1,0,0,10,0)
        self.table.attach(Label("On"),1,2,0,1,0,0,10,0)
        self.table.attach(Label("StartIdx"),2,3,0,1,EXPAND|FILL,0,10,0)
        self.table.attach(Label("EndIdx"),3,4,0,1,EXPAND|FILL,0,10,0)
        if self.incNidOffset > -1:
            self.table.attach(Label("Increment"),4,5,0,1,EXPAND|FILL,0,10,0)
        self.table.attach(Label("Path"),columns-1,columns,0,1,0,0,10,0)
        self.channels=list()
        for chan in range(self.numChannels):
            channel=dict()
            self.channels.append(channel)
            channel['number']=Label("%d"% (chan+1,))
            self.table.attach(channel['number'],0,1,chan+1,chan+2,0,0,10,0)
            channel['on']=CheckButton('')
            self.table.attach(channel['on'],1,2,chan+1,chan+2,0,0,10,0)
            channel['startIdx']=Entry()
            self.table.attach(channel['startIdx'],2,3,chan+1,chan+2,EXPAND|FILL,0,10,0)
            channel['endIdx']=Entry()
            self.table.attach(channel['endIdx'],3,4,chan+1,chan+2,EXPAND|FILL,0,10,0)
            if self.incNidOffset > -1:
                channel['inc']=Entry()
                self.table.attach(channel['inc'],4,5,chan+1,chan+2,EXPAND|FILL,0,10,0)
            channel['path']=Label('                   ')
            self.table.attach(channel['path'],columns-1,columns,chan+1,chan+2,0,0,10,0)
        self.show_all()
        if not guibuilder:
            for chan in range(self.numChannels):
                chanNidOffset=self.node.nid+chan*self.nodesPerChannel
                channel=self.channels[chan]
                channel['dataNode']=TreeNode(chanNidOffset+self.dataNidOffset,self.node.tree)
                channel['startIdxNode']=TreeNode(chanNidOffset+self.startIdxNidOffset,self.node.tree)
                channel['endIdxNode']=TreeNode(chanNidOffset+self.endIdxNidOffset,self.node.tree)
                if self.incNidOffset > -1:
                    channel['incNode']=TreeNode(chanNidOffset+self.incNidOffset,self.node.tree)

    def __init__(self):
        ScrolledWindow.__init__(self)
        MDSplusWidget.__init__(self)
        self.numChannels=4
        self.nodesPerChannel=3
        self.dataNidOffset=1
        self.startIdxNidOffset=2
        self.endIdxNidOffset=3
        self.incNidOffset=-1
        self.table=Table(homogeneous=False)
        self.table.set_row_spacings(0)
        self.add_with_viewport(self.table)
        self.set_policy(POLICY_NEVER,POLICY_ALWAYS)
        adj=self.get_vadjustment()
        if adj is not None:
            adj.connect("changed",self.adj_changed)

    def show(self):
        self.show_all()
        
gobject.type_register(MDSplusDigChansWidget) 

if guibuilder:
    class MDSplusDigChansWidgetAdaptor(glade.get_adaptor_for_type('GtkTable')):
        __gtype_name__='MDSplusDigChansWidgetAdaptor'

        def do_set_property(self,widget,prop,value):
            if prop == 'nidOffset':
                widget.nidOffset=value
            elif prop == 'putOnApply':
                widget.putOnApply=value
            elif prop == 'startIdxNidOffset':
                widget.startIdxNidOffset=value
            elif prop == 'endIdxNidOffset':
                widget.endIdxNidOffset=value
            elif prop == 'incNidOffset':
                widget.incNidOffset=value
                widget.setupChannels()
            elif prop == 'dataNidOffset':
                widget.dataNidOffset=value
            elif prop == 'nodesPerChannel':
                widget.nodesPerChannel=value
            elif prop == 'numChannels':
                if value > 0:
                    widget.numChannels=value
                    widget.setupChannels()
