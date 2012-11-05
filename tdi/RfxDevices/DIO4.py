from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
import time

#import pdb

class DIO4(Device):
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """INCAA DIO4 Timing Module"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':SW_MODE', 'type':'text', 'value':'LOCAL'},
        {'path':':IP_ADDR', 'type':'text'},
        {'path':':COMMENT', 'type':'text'},
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'INTERNAL'},
        {'path':':REC_START_EV', 'type':'text','state':'0'},
        {'path':':REC_EVENTS', 'type':'text'},
        {'path':':REC_TIMES', 'type':'numeric'},
        {'path':':SYNCH', 'type':'text', 'value':'NO'},
        {'path':':SYNCH_EVENT', 'type':'text'}]


    for i in range(8):
        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:FUNCTION'%(i+1), 'type':'text', 'value':'CLOCK'})
        parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i+1), 'type':'text', 'value':'SOFTWARE'})
        parts.append({'path':'.CHANNEL_%d:EVENT'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d:CYCLIC'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d:DELAY'%(i+1), 'type':'numeric', 'value':0.})
        parts.append({'path':'.CHANNEL_%d:DURATION'%(i+1), 'type':'numeric', 'value':1.})
        parts.append({'path':'.CHANNEL_%d:FREQ_1'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:FREQ_2'%(i+1), 'type':'numeric', 'value':1000})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_1'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:INIT_LEV_2'%(i+1), 'type':'text', 'value':'LOW'})
        parts.append({'path':'.CHANNEL_%d:DUTY_CYCLE'%(i+1), 'type':'numeric', 'value':50})
        parts.append({'path':'.CHANNEL_%d:TRIGGER'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d:CLOCK'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_1'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:TRIGGER_2'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d:COMMENT'%(i+1), 'type':'text'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV1'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:TERM'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV1:EDGE'%(i+1), 'type':'text', 'value':'RISING'})

        parts.append({'path':'.CHANNEL_%d:OUT_EV2'%(i+1), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:NAME'%(i+1), 'type':'text'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:CODE'%(i+1), 'type':'numeric'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TIME'%(i+1), 'type':'numeric', 'value':0})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:TERM'%(i+1), 'type':'text', 'value':'NO'})
        parts.append({'path':'.CHANNEL_%d.OUT_EV2:EDGE'%(i+1), 'type':'text', 'value':'RISING'})


    parts.append({'path':'.OUT_EV_SW', 'type':'structure'})
    parts.append({'path':'.OUT_EV_SW:NAME', 'type':'text'})
    parts.append({'path':'.OUT_EV_SW:CODE', 'type':'numeric'})
    parts.append({'path':'.OUT_EV_SW:TIME', 'type':'numeric', 'value':0})






    parts.append({'path':'.CHANNEL_1:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_2:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_3:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_4:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_5:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_6:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_7:TERM', 'type':'text', 'value':'NO'})
    parts.append({'path':'.CHANNEL_8:TERM', 'type':'text', 'value':'NO'})








#class DIO4(Device):
#    Int32(1).setTdiVar('_PyReleaseThreadLock')
#    """INCAA DIO4 Timing Module"""
#    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
#        {'path':':SW_MODE', 'type':'text', 'value':'LOCAL'},
#        {'path':':IP_ADDR', 'type':'text'},
#        {'path':':COMMENT', 'type':'text'},
#        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'INTERNAL'},
#        {'path':':REC_START_EV', 'type':'text'},
#        {'path':':REC_EVENTS', 'type':'text'},
#        {'path':':REC_TIMES', 'type':'numeric'},
#        {'path':':SYNCH', 'type':'text', 'value':'NO'},
#        {'path':':SYNCH_EVENT', 'type':'text'}]
#    for i in range(8):
#        parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
#        parts.append({'path':'.CHANNEL_%d:FUNCTION'%(i+1), 'type':'text', 'value':'CLOCK'})
#        parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i+1), 'type':'text', 'value':'SOFTWARE'})
#        parts.append({'path':'.CHANNEL_%d:EVENT'%(i+1), 'type':'text'})
#        parts.append({'path':'.CHANNEL_%d:CYCLIC'%(i+1), 'type':'text', 'value':'NO'})
#        parts.append({'path':'.CHANNEL_%d:DELAY'%(i+1), 'type':'numeric', 'value':0.})
#        parts.append({'path':'.CHANNEL_%d:DURATION'%(i+1), 'type':'numeric', 'value':1.})
#        parts.append({'path':'.CHANNEL_%d:FREQ_1'%(i+1), 'type':'numeric', 'value':1000})
#        parts.append({'path':'.CHANNEL_%d:FREQ_2'%(i+1), 'type':'numeric', 'value':1000})
#        parts.append({'path':'.CHANNEL_%d:INIT_LEV_1'%(i+1), 'type':'text', 'value':'LOW'})
#        parts.append({'path':'.CHANNEL_%d:INIT_LEV_2'%(i+1), 'type':'text', 'value':'LOW'})
#        parts.append({'path':'.CHANNEL_%d:DUTY_CYCLE'%(i+1), 'type':'numeric', 'value':50})
#        parts.append({'path':'.CHANNEL_%d:TRIGGER'%(i+1), 'type':'numeric', 'value':0})
#        parts.append({'path':'.CHANNEL_%d:CLOCK'%(i+1), 'type':'numeric'})
#        parts.append({'path':'.CHANNEL_%d:TRIGGER_1'%(i+1), 'type':'numeric'})
#        parts.append({'path':'.CHANNEL_%d:TRIGGER_2'%(i+1), 'type':'numeric'})
#        parts.append({'path':'.CHANNEL_%d:OUT_EV_1'%(i+1), 'type':'text'})
#        parts.append({'path':'.CHANNEL_%d:OUT_EV_2'%(i+1), 'type':'text'})
#        parts.append({'path':'.CHANNEL_%d:TERM_1'%(i+1), 'type':'text', 'value':'ON'})
#        parts.append({'path':'.CHANNEL_%d:TERM_2'%(i+1), 'type':'text', 'value':'ON'})
#        parts.append({'path':'.CHANNEL_%d:EDGE_1'%(i+1), 'type':'text', 'value':'RISING'})
#        parts.append({'path':'.CHANNEL_%d:EDGE_2'%(i+1), 'type':'text', 'value':'RISING'})
#        parts.append({'path':'.CHANNEL_%d:COMMENT'%(i+1), 'type':'text'})
#
#    del i
#
#
#    parts.append({'path':'.CHANNEL_1.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_1.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_1.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_2.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_2.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_2.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_3.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_3.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_3.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_4.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_4.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_4.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_5.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_5.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_5.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_6.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_6.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_6.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_7.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_7.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_7.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.CHANNEL_8.OUT_EV1', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV1:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV1:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV1:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_8.OUT_EV1:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV1:EDGE', 'type':'text', 'value':'RISING'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2', 'type':'structure'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2:NAME', 'type':'text'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2:CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2:TIME', 'type':'numeric', 'value':0})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2:TERM', 'type':'text', 'value':'ON'})
#    parts.append({'path':'.CHANNEL_8.OUT_EV2:EDGE', 'type':'text', 'value':'RISING'})
#
#    parts.append({'path':'.OUT_EV_SW', 'type':'structure'})
#    parts.append({'path':'.OUT_EV_SW:NAME', 'type':'text'})
#    parts.append({'path':'.OUT_EV_SW:CODE', 'type':'numeric'})
#    parts.append({'path':'.OUT_EV_SW:TIME', 'type':'numeric', 'value':0})

#    parts.append({'path':'.CHANNEL_1:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_1:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_1:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_2:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_3:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_4:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_5:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_6:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_7:OUT_EV2_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8:OUT_EV1_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8:OUT_EV2_CODE', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8:OUT_EV1_TIME', 'type':'numeric'})
#    parts.append({'path':'.CHANNEL_8:OUT_EV2_TIME', 'type':'numeric'})




    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'INIT',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'STORE',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':RESET_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('CPCI_SERVER','RESET',50,None),Method(None,'RESET',head))",
        'options':('no_write_shot',)})



    handle = 0


# INIT
    def INIT(self, arg):
        print 'INIT'


# Board ID
        try:



            boardId = self.board_id.data()
            print 'BOARD_ID: ' + str(boardId)

            #pdb.set_trace();
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
# Software Mode
        try:
            swMode = self.sw_mode.data()
            print 'swMode: ' + str(swMode)
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            return 0
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except: 
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                return 0
            print 'IP_ADDR: ' + ipAddr
# Clock Source
        clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1, 'EXTERNAL':2}
        try:
            clockSource = clockSourceDict[self.clock_source.data()]
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid CLOCK_SOURCE')
            return 0
        print 'CLOCK_SOURCE: ' + self.clock_source.data() + ' - ID: ' + str(clockSource)
# Recorder Event
        recStartEv = 0
        if getattr(self, 'rec_start_ev').isOn():
            try:
                recStartEv = Data.execute('TimingDecodeEvent($1)', self.rec_start_ev)
            except: 
                #recStartEv = -1
                recStartEv = 0
        print 'REC_START_EV: ' + str(recStartEv)
# Synch events
        synchEvents = []
        try:
            synch = self.synch.data()
            print 'synch: ' + synch
            if synch == 'YES':
                synchEvent = self.synch_event.data()
                synchEvSize =Data.execute('size($1)', synchEvent)
                print str(synchEvSize)
                if synchEvSize == 1:
                    synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent))
                else:
                    for i in range(synchEvSize):
                        synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent[i]))
                    del i
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SYNCH_EVENT')
            return 0
# HW Init
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO4HWInit(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, clockSource, recStartEv, makeArray(synchEvents))
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW initialization')
                    return 0
            else:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot connect to remote CPCI system')
                return 0
        else:
            status = Data.execute("DIO4HWInit(0, $1, $2, $3, $4)", boardId, clockSource, recStartEv, makeArray(synchEvents))
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW initialization')
                return 0
# Channels Setup
        channelMask = 0
        huge = Data.execute('HUGE(0)')


# SW EVENT
#        huge = Data.execute('HUGE(0)')
        if getattr(self, 'out_ev_sw').isOn():
            print 'SW EVENT IS ON'
            try:
                evName = getattr(self, 'out_ev_sw_name').data()
            except:
                evName = ''
            if evName != '':
                evCode = Data.execute('TimingDecodeEvent($1)', evName)
            else:
                evCode = 0
            if evCode != 0:
                setattr(self,'out_ev_sw_code', evCode)
            else:
                try:
                    print 'LEGGO evCode'
                    evCode = getattr(self, 'out_ev_sw_code').data()
                except:
                    evCode = 0
            if evCode == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
                return 0
        
            try:
                print 'LEGGO evTime'
                evTime = getattr(self, 'out_ev_sw_time').data()
            except:
                evTime = huge
            if evTime == huge:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
                return 0
            nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
            print 'NAME: ' + evName
            print 'PATH: ' + nodePath
            status = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
            print 'FATTO'
            if status == -1:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
                return 0
        else:
            print 'SW EVENT IS OFF'






        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    return 0
                print 'FUNCTION: ' + function
                if function == 'CLOCK': #Clock Generation
                    try :
                        frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        print 'FREQ: ' + str(frequency)
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()

                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]
                    except:
                        Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid clock parameters for channel %d'%(c+1))
                        return 0


                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4, $5)', boardId, c, frequency, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW clock setup. See CPCI console for details')
                            return 0
                    else:
                        status = Data.execute("DIO4HWSetClockChan(0, $1, $2, $3, $4, $5)", boardId, c, frequency, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                            return 0
                    
                    period = long((1. / frequency) / 1E-7 + 0.5) * 1E-7;
                    #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE("+*+","+*+","+str(period)+")'))
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period)) 

                    #self.clock.putData(Range(None, None, period))

                elif function == 'PULSE':   #Trigger Generation
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eventSize = Data.execute('size($1)', event)
                                print 'event: ' + str(event)
                                print 'event size: ' + str(eventSize)
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print 'eventTime: ' + str(eventTime)
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                                print 'eventCodes: ' + str(eventCodes)
                                trigger = getattr(self,'channel_%d_trigger'%(c+1)).data()
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'error events')
                                return 0
                        cyclicDict = {'NO':0, 'YES':1}
                        levelDict = {'LOW':0, 'HIGH':1}
                        cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
                        print cyclic
                        initLev1 = levelDict[getattr(self,'channel_%d_init_lev_1'%(c+1)).data()]
                        print initLev1
                        initLev2 = levelDict[getattr(self,'channel_%d_init_lev_2'%(c+1)).data()]
                        print initLev2
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        print duration
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        print delay
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]


                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Pulse parameters for channel %d'%(c+1))
                        return 0


                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9,$10)", $1,$2,$3,$4,$5,$6,$7,$8,$9,$10)', boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW pulse setup. See CPCI console for details')
                            return 0
                    else:
                        status = Data.execute("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",boardId, c, trigModeCode, cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW pulse setup')
                            return 0
                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path
                        print trigger_1
                        print trigger_2
                        getattr(self, 'channel_%d_trigger_1'%(c+1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2'%(c+1)).putData(Data.compile(trigger_2))

                        #setattr(self, 'channel_%d_trigger_1'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()))
                        #setattr(self, 'channel_%d_trigger_2'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write Pulse parameters for channel %d'%(c+1))
                        return 0



                elif function == 'GCLOCK':
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                #eventSize = Data.execute('size($1)', event)
                                print eventSize
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print eventTime
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        print eA[i]
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                return 0
                        frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        print 'duration: ' + str(duration)
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        cyclicDict = {'NO':0, 'YES':1}
                        cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]


                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Gated Clock parameters for channel %d'%(c+1))
                        return 0

                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)", $1,$2,$3,$4,$5,$6,$7, $8, $9, $10)', boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle, cyclic, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            return 0
                    else:
                        status = Data.execute("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10)",boardId, c, trigModeCode, frequency, delay, duration, makeArray(eventCodes), dutyCycle, cyclic, evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW GClock setup')
                            return 0

                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path

                        #trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        #trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()

                        period = long((1. / frequency) / 1E-7 + 0.5) * 1E-7;
                        #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE('+trig1+','+trig2+','+str(period)+')'))
                        getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(Data.compile(trigger_1), Data.compile(trigger_2), period))



                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write GCLOCK parameters for channel %d'%(c+1))
                        return 0

                elif function == 'DCLOCK':
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data()
                        trigModeCode = trigModeDict[trigMode]
                        eventCodes = []
                        if trigMode == 'EVENT':
                            try:
                                event = getattr(self,'channel_%d_event'%(c+1)).data()
                                eA = array(event)
                                l = len(event)
                                if l == 0:
                                    raise
                                eventSize = eA.size
                                #eventSize = Data.execute('size($1)', event)
                                print eventSize
                                if eventSize == 1:
                                    eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
                                    eventTime = Data.execute('TimingGetEventTime($1)', event)
                                    print eventTime
                                    if eventTime == huge or eventTime == -huge:
                                        eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
                                    else:
                                        setattr(self,'channel_%d_trigger'%(c+1), eventTime)
                                else:
                                    for i in range(eventSize):
                                        print eA[i]
                                        eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot resolve event(s) for channel %d'%(c+1))
                                return 0
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data()
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data()
                        duration = getattr(self,'channel_%d_duration'%(c+1)).data()
                        delay = getattr(self,'channel_%d_delay'%(c+1)).data()
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Dual Speed Clock parameters for channel %d'%(c+1))
                        return 0

                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)", $1,$2,$3,$4,$5,$6,$7,$8,$9)', boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW GClock setup. See CPCI console for details')
                            return 0
                    else:
                        status = Data.execute("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)",boardId, c, trigModeCode, freq1, freq2, delay, duration, makeArray(eventCodes), evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW DClock setup')
                            return 0

                    try:
                        trig_path = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                        delay_path = getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        duration_path = getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        trigger_1 = trig_path + '+' + delay_path
                        trigger_2 = trigger_1 + '+' +  duration_path
                        getattr(self, 'channel_%d_trigger_1'%(c+1)).putData(Data.compile(trigger_1))
                        getattr(self, 'channel_%d_trigger_2'%(c+1)).putData(Data.compile(trigger_2))

                        period1 = int((1. / freq1) / 1E-7 + 0.5) * 1E-7
                        period2 = int((1. / freq2) / 1E-7 + 0.5) * 1E-7

                        trig1_path = getattr(self,'channel_%d_trigger_1'%(c+1)).getFullPath()
                        trig2_path = getattr(self,'channel_%d_trigger_2'%(c+1)).getFullPath()
                        setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE([-1E6,'+trig1_path+','+trig2_path+'],['+trig1_path+','+trig2_path+',1E6],['+str(period1)+','+str(period2)+','+str(period1)+'])'))


                        #trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
                        #trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
                        #setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE([-1E6,'+trig1+','+trig2+'],['+trig1+','+trig2+',1E6],['+str(period1)+','+str(period2)+','+str(period1)+'])'))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write DCLOCK parameters for channel %d'%(c+1))
                        return 0
                elif function == 'ENCODER':
                    for e in range(2):
                        if getattr(self, 'channel_%d_out_ev%d'%(c+1, e+1)).isOn():
                            try:
                                evName = getattr(self, 'channel_%d_out_ev%d_name'%(c+1, e+1)).data()
                            except:
                                evName = ''
                            if evName != '':
                                evCode = Data.execute('TimingDecodeEvent($1)', evName)
                            else:
                                evCode = 0
                            print 'evCode: ' + str(evCode)
                            if evCode != 0:
                                nodePath = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).getFullPath()
                                print nodePath
                                setattr(self,'channel_%d_out_ev%d_code'%(c+1, e+1), evCode)
                            else:
                                try:
                                    evCode = getattr(self, 'channel_%d_out_ev%d_code'%(c+1, e+1)).data()
                                except:
                                    evCode = 0
                            if evCode == 0:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for channel %d'%(c+1))
                                return 0

                            try:
                                evTime = getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).data()
                            except:
                                evTime = huge
                            if evTime == huge:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for channel %d'%(c+1))
                                return 0
                            nodePath = getattr(self, 'channel_%d_out_ev%d_time'%(c+1, e+1)).getFullPath()
                            status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
                            if status == -1:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register event time')
                                return 0
                            evTermDict = {'NO':0, 'YES':1}
                            try:
                                evTerm = getattr(self, 'channel_%d_out_ev%d_term'%(c+1, e+1)).data()
                                evTermCode = evTermDict[evTerm]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event termination specification for channel %d'%(c+1))
                                return 0
                            evEdgeDict = {'FALLING':0, 'RISING':1}
                            try:
                                evEdge = getattr(self, 'channel_%d_out_ev%d_edge'%(c+1, e+1)).data()
                                evEdgeCode = evEdgeDict[evEdge]
                            except:
                                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event edge specification for channel %d'%(c+1))
                                return 0

                            print 'channel: ' + str(c)
                            realChannel = 2*c+1+e
                            print 'real channel: ' + str(realChannel)
                            print 'code: ' + str(evCode)
                            if swMode == 'REMOTE':
                                #status = Data.execute('MdsValue("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4,$5)', boardId, c, evCode, evTermCode, evEdgeCode)
                                status = Data.execute('MdsValue("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", $1,$2,$3,$4,$5)', boardId, realChannel, evCode, evTermCode, evEdgeCode)
                                if status == 0:
                                    Data.execute('MdsDisconnect()')
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW set event setup.')
                                    return 0
                            else:
                                #status = Data.execute("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", boardId, c, evCode, evTermCode, evEdgeCode)
                                status = Data.execute("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5)", boardId, realChannel, evCode, evTermCode, evEdgeCode)
                                if status == 0:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW set event setup')
                                    return 0


                                
                        else:
                            print 'OFF'

                elif function == 'CLOCK SOURCE + CLOCK':
                    try :
                        freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data() # Frequenza del clock esterno
                        freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data() # Frequenza del clock da generare
                        dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
                        evTermDict = {'NO':0, 'YES':1}
                        evTerm = getattr(self, 'channel_%d_term'%(c+1)).data()
                        evTermCode = evTermDict[evTerm]

                    except:
                        Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid CLOCK SOURCE + CLOCK parameters for channel %d'%(c+1))
                        return 0
                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWSetExternalClockChan(0, $1, $2, $3, $4, $5, $6)", $1,$2,$3,$4,$5,$6)', boardId, c, freq1, freq2, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('MdsDisconnect()')
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW clock setup. See CPCI console for details')
                            return 0
                    else:
                        status = Data.execute("DIO4HWSetExternalClockChan(0, $1, $2, $3, $4, $5, $6)", boardId, c, freq1, freq2, dutyCycle, evTermCode)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW clock setup')
                            return 0

                # DEVO SCRIVERE ANCHE freq1 DA QUALCHE PARTE ??????
                    period = long((1. / freq2) / 1E-7 + 0.5) * 1E-7;
                    getattr(self, 'channel_%d_clock'%(c+1)).putData(Range(None, None, period)) 








# End Channels Setup


                if swMode == 'REMOTE':
                    status = Data.execute('MdsValue("DIO4HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, synch) 
                    Data.execute('MdsDisconnect()')
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO4 device. See CPCI console for details')
                        return 0
                else:
                    status = Data.execute("DIO4HWStartChan(0, $1, $2, $3)",boardId, channelMask, synch)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot start DIO4 Device')
                        return 0




        return 1



# RESET
    def RESET(self, arg):
        print 'RESET'
# Board ID
        try:
            boardId = self.board_id.data()
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            return 0
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except: 
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                return 0
            print 'IP_ADDR: ' + ipAddr
#HW Reset
        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO4HWReset(0, $1)", $1)', boardId)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr', self.nid, 'Cannot execute remote HW reset. See CPCI console for details')
                    return 0
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                return 0
        else:
            status = Data.execute("DIO4HWReset(0, $1)", boardId)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW reset')
                return 0
        return 1



    def STORE(self, arg):
        print 'STORE'
# Board ID
        try:
            boardId = self.board_id.data()
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            return 0
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except: 
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                return 0
#HW
        recEvents = []
        recTimes = []

        if swMode == 'REMOTE':
            status = Data.execute('MdsConnect("'+ ipAddr + '")')
            if status > 0:
                status = Data.execute('MdsValue("DIO4HWGetRecEvents(0, $1)", $1)', boardId)
                if status == 0:
                    Data.execute('MdsDisconnect()')
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO4HWGetRecEvents')
                    return 0
            else:
                Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                return 0
        else:
            status = Data.execute("DIO4HWGetRecEvents(0, $1)", boardId)
            if status == 0:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute DIO4HWGetRecEvents')
                return 0
        recEvents = Data.execute("_DIO4_rec_events")
        recTimes = Data.execute("_DIO4_rec_times")
	recEventNum  = Data.execute("size(_DIO4_rec_times)")
        print "EVENTS ",recEvents
        print "TIMES ", recTimes
        print "NUM   ", recEventNum

        #if recEvents[0] != -1:
        if recEventNum > 0 :
            #setattr(self,'rec_events', recEvents)
            print "rec_data ", self.rec_events.getFullPath()
            #self.rec_events.putData(recEvents)

            try:
                recStartEv = getattr(self, 'rec_start_ev').data()
            except:
                recStartEv = ''
            if recStartEv != '':
                recStartTime = Data.execute('TimingGetEventTime($1)', recStartEv)
            else:
                recStartTime = 0
            #setattr(self,'rec_times', recTimes + recStartTime)
            print "rec_times ", self.rec_times.getFullPath()
            #self.rec_times.putData(recTimes + recStartTime)

        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    return 0
                print 'FUNCTION: ' + function
                if function == 'PULSE' or function == 'GCLOCK' or function == 'DCLOCK':
                    if swMode == 'REMOTE':
                        status = Data.execute('MdsValue("DIO4HWGetPhaseCount(0, $1, $2)", $1,$2)', boardId, channelMask) 
                        Data.execute('MdsDisconnect()')
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot get phase count')
                            return 0
                    else:
                        status = Data.execute("DIO4HWGetPhaseCount(0, $1, $2)",boardId, channelMask)
                        if status == 0:
                            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot get phase count')
                            return 0
                    phases = Data.execute("_DIO4_phases")
                    print phases
                    trigPath = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()
                    trig1 = trigPath + '+' + phases[0]
                    trig2 = trigPath + '+' + phases[1]
                    try:
                        setattr(self, 'channel_%d_trig_1'%(c+1), Data.compile(trig1))
                        setattr(self, 'channel_%d_trig_2'%(c+1), Data.compile(trig2))
                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write trigger parameters for channel %d'%(c+1))
                        return 0

        return 1







    def TRIGGER(self, arg):
        print 'TRIGGER'
# Board ID
        try:
            boardId = self.board_id.data()
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
# Software Mode
        try:
            swMode = self.sw_mode.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid SW_MODE')
            return 0
        if swMode == 'REMOTE':
            try:
                ipAddr = self.ip_addr.data()
            except: 
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP_ADDR')
                return 0
#HW TRIGGER
        channelMask = 0
        for c in range(8):
            if getattr(self, 'channel_%d'%(c+1)).isOn():
                channelMask = channelMask | (1 << c)
                try:
                    function = getattr(self, 'channel_%d_function'%(c+1)).data()
                except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid FUNCTION')
                    return 0
                if (function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK'):
                    trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
                    try:
                        trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
                        trigModeCode = trigModeDict[trigMode]
                        if trigMode == 'SOFTWARE':

                            if swMode == 'REMOTE':
                                status = Data.execute('MdsConnect("'+ ipAddr + '")')
                                if status > 0:
                                    status = Data.execute('MdsValue("DIO4HWTrigger(0, $1, $2)", $1, $2)', boardId, channelMask)
                                    if status == 0:
                                        Data.execute('MdsDisconnect()')
                                        Data.execute('DevLogErr', self.nid, 'Cannot execute remote trigger')
                                        return 0
                                else:
                                    Data.execute('DevLogErr', self.nid, 'Cannot connect to remote CPCI system')
                                    return 0
                            else:
                                status = Data.execute("DIO4HWTrigger(0, $1, $2)", boardId, channelMask)
                                if status == 0:
                                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute trigger')
                                    return 0



                    except:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid trigger mode')
                        return 0
#SW EVENT
        #huge = Data.execute('HUGE(0)')
        #if getattr(self, 'out_ev_sw').isOn():
        #    try:
        #        evName = getattr(self, 'out_ev_sw_name').data()
        #    except:
        #        evName = ''
        #    if evName != '':
        #        evCode = Data.execute('TimingDecodeEvent($1)', evName)
        #    else:
        #        evCode = 0
        #    if evCode != 0:
        #        setattr(self,'out_ev_sw_code', evCode)
        #    else:
        #        try:
        #            evCode = getattr(self, 'out_ev_sw_code').data()
        #        except:
        #            evCode = 0
        #    if evCode == 0:
        #        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event specification for software channel')
        #        return 0
        #
        #    try:
        #        evTime = getattr(self, 'out_ev_sw_time').data()
        #    except:
        #        evTime = huge
        #    if evTime == huge:
        #        Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid event time specification for software channel')
        #        return 0
        #    nodePath = getattr(self, 'out_ev_sw_time').getFullPath()
        #    status = eventTime = Data.execute('TimingRegisterEventTime($1, $2)', evName, nodePath)
        #    if status == -1:
        #        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot register software event time')
        #        return 0
        #
        #    if swMode == 'REMOTE':
        #        status = Data.execute('MdsValue("DIO4HWEventTrigger(0, $1, $2)", $1,$2)', boardId, c, evCode)
        #        if status == 0:
        #            Data.execute('MdsDisconnect()')
        #            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
        #            return 0
        #    else:
        #        status = Data.execute("DIO4HWEventTrigger(0, $1, $2)", boardId, c, evCode)
        #        if status == 0:
        #            Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
        #            return 0
        #
        #
        #        
        #else:
        #    print 'OFF'
        if getattr(self, 'out_ev_sw').isOn():
            try:
                print 'SW CHANNEL IS ON'
                evCode = getattr(self, 'out_ev_sw_code').data()
                print evCode
                if swMode == 'REMOTE':
                    status = Data.execute('MdsValue("DIO4HWEventTrigger(0, $1, $2)", $1,$2)', boardId, evCode)
                    if status == 0:
                        Data.execute('MdsDisconnect()')
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute remote HW event trigger')
                        return 0
                else:
                    status = Data.execute("DIO4HWEventTrigger(0, $1, $2)", boardId, evCode)
                    if status == 0:
                        Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot execute HW event trigger')
                        return 0

            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid Event Code specification for software channel')
                return 0


        return 1






###################################################
    def saveInfo(self):
        global DIO4Handles
        global DIO4Nids
        try:
            DIO4Handles
        except:
            DIO4Handles = []
            DIO4Nids = []
        try:
            idx = DIO4Nids.index(self.getNid())
        except:
            print 'SAVE INFO: SAVING HANDLE'
            DIO4Handles.append(self.handle)
            DIO4Nids.append(self.getNid())
            return
        return

    def restoreInfo(self):
        global DIO4Handles
        global DIO4Nids
        global DIO4Lib
        try:
            DIO4Lib
        except:
            DIO4Lib = CDLL("libDIO4.so")
            print 'carico la libreria'

        try:
            idx = DIO4Nids.index(self.getNid())
            self.handle = DIO4Handles[idx]
            print 'RESTORE INFO HANDLE TROVATO'
        except:
            print 'RESTORE INFO HANDLE NON TROVATO'
            try:
                boardId = self.board_id.data()
                print boardId
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid BOARD_ID')
                return 0
            try:
                DIO4Lib.DIO4_InitLibrary()
                status = DIO4Lib.DIO4_Open(c_int(boardId), byref(c_int(self.handle)))
                print status
                print self.handle
            except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot open device')
                return 0
        return

    def removeInfo(self):
        global DIO4Handles
        global DIO4Nids
        DIO4Nids.remove(self.getNid())
        DIO4Handles.remove(self.handle)
        return




    def start_store(self, arg):
        print 'START STORE'

        global DIO4Lib
        self.restoreInfo()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        self.worker.configure(self, DIO4Lib, self.handle)
        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,arg):
        print 'STOP STORE'
        self.restoreWorker()
        self.worker.stop()
        return 1




    class AsynchStore(Thread):
        stopReq = False

        def configure(self, device, DIO4Lib, handle):
            self.device = device
            self.DIO4Lib = DIO4Lib
            self.handle = handle

        def run(self):
            while not self.stopReq:
                print 'RUN'
                time.sleep(5)

        def stop(self):
            self.stopReq = True



###################################Worker Management
    def saveWorker(self):
        global DIO4Workers
        global DIO4WorkerNids
        try:
            DIO4Workers
        except:
            DIO4WorkerNids = []
            DIO4Workers = []
        try:
            idx = DIO4WorkerNids.index(self.getNid())
            DIO4Workers[idx] = self.worker
        except:
            print 'SAVE WORKER: NEW WORKER'
            DIO4WorkerNids.append(self.getNid())
            DIO4Workers.append(self.worker)
            return
        return

    def restoreWorker(self):
        global DIO4WorkerNids
        global DIO4Workers
      
        try:
            idx = DIO4WorkerNids.index(self.getNid())
            self.worker = DIO4Workers[idx]
        except:
            print 'Cannot restore worker!!'

########################AsynchStore class


