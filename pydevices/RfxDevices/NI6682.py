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

"""
RfxDevices
==========
@authors: Anton Soppelsa (IGI Padova)
@copyright: 2012
@license: GNU GPL
"""
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array
from threading import Thread
from ctypes import CDLL, c_int, byref
from time import sleep

#class NI6682:
class NI6682(Device):
    """National Instrument 6682 device. This implementation uses only a limited set of the hardware facilities.
       In particular the board is just used as a Time-to-Digital Converter (TDC). """
    parts = [{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'},
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'PTP'}]

    for i in range(0,3):
        parts.append({'path':'.CHANNEL_PFI%d'%(i), 'type':'structure'})
        parts.append({'path':'.CHANNEL_PFI%d:TRIG_MODE'%(i), 'type':'text', 'value':'EDGE_RISING'})
        parts.append({'path':'.CHANNEL_PFI%d:DECIMATION'%(i), 'type':'numeric', 'value':1})
        parts.append({'path':'.CHANNEL_PFI%d:TIME_SEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
        parts.append({'path':'.CHANNEL_PFI%d:TIME_NSEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
    del(i)
    parts.append({'path':':INITIALISE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INITIALISE',50,None),Method(None,'INITIALISE',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':FINALISE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','FINALISE',50,None),Method(None,'FINALISE',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','START_STORE',50,None),Method(None,'START_STORE',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','STOP_STORE',50,None),Method(None,'STOP_STORE',head))",
        'options':('no_write_shot',)})

    # Corresponds to the nisync_edge_t type in nisynch.h
    EDGE_RISING = 0;
    EDGE_FALLING = 1;
    EDGE_ANY = 2;

    # A small subset of the enum type nisync_terminal_t in nisynch.h
    PFI0 = 0;
    PFI1 = 1;
    PFI2 = 2;

    triggerModeDict = {'EDGE_RISING':EDGE_RISING, 'EDGE_FALLING':EDGE_FALLING, 'EDGE_ANY':EDGE_ANY}
    channelsDict = {'PFI0':PFI0, 'PFI1':PFI1, 'PFI2':PFI2}

    #libts = CDLL("/usr/local/lib/libtimestamping.so");
    libts = None

    # Declaration of global dictionary of storemen
    ni6682Storemen = {}

    # Why this init is not working!!!!! Also __new__ is called many times the action is
    #def __init__(self, node):

        #print "NI6682::__init__(): INIZIO."

        #print "node.getNid() = ", node.getNid()
        #print "node.getFullPath() = ", node.getFullPath()

        #super(NI6682, self).__init__(node);

        #print "self.nid = ", self.nid, " self.getNid() = ", self.getNid()
	#try:
            #NI6682.ni6682Storemen[self.getNid()];
        #except:

            #NI6682.ni6682Storemen[self.getNid()] = self.Storeman(self);
            #print 'NI6682::__init__(): INFO CREATED'

        #print NI6682.ni6682Storemen
        #self.ni6682Storeman = NI6682.ni6682Storemen[self.getNid()];
        #print 'NI6682::__init__(): INFO RESTORED'

        #print "NI6682::__init__(): FINE."


    def manageInfo(self):

        print("NI6682::manageInfo(): BEGIN")

        n = self.getNid()
        #print "self.getNid() = ", n
        #print "self.getFullPath() = ", self.getFullPath()

        try:
            NI6682.ni6682Storemen[n];
        except:
            NI6682.ni6682Storemen[n] = self.Storeman(self);
            print('NI6682::manageInfo(): INFO CREATED')

        self.ni6682Storeman = NI6682.ni6682Storemen[n];

        print('NI6682::manageInfo(): INFO RESTORED')

        print("NI6682::manageInfo(): END")


# INIT
    def INITIALISE(self):

        print('INITIALISE')
        if NI6682.libts is None:
            NI6682.libts = CDLL("libtimestamping.so")

        self.manageInfo();

        try:
            boardId = self.board_id.data()
            print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        res = NI6682.libts.NI6682_Create(c_int(boardId));
        print('NI6682_Create(): ', res)

        res = NI6682.libts.NI6682_Initialise(c_int(boardId));
        print('NI6682_Initialise(): ', res)


        for i in range(3):

            tm = getattr(self, 'channel_pfi%d_trig_mode'%(i)).data();

            try:
                self.triggerModeDict[tm]
                # Better using __dict__ because getattr calls the constructor. Not possible why?
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            print('CHANNEL_PFI%d.TRIG_MODE: '%(i), tm)

            try:
                decimation = getattr(self, 'channel_pfi%d_decimation'%(i)).data();
                if decimation < 1: # decimation must be greather than 0
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Decimation must be greater than 0: ', decimation)
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            print('CHANNEL_PFI%d.DECIMATION: '%(i), decimation)

        # If executed for the first time

        return;

# STORE
    def FINALISE(self):

        print('FINALISE')

        self.manageInfo();

        #self.restoreInfo();
        try:
            boardId = self.board_id.data()
            print('BOARD_ID: ' + str(boardId))
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        res = NI6682.libts.NI6682_Finalise(c_int(boardId));
        print('NI6682_Finalise(): ', res)

        res = NI6682.libts.NI6682_Destroy(c_int(boardId));
        print('NI6682_Destroy(): ', res)

        return;


    class Storeman(Thread):
        ''' This class is used to create the thread keeping empty the NI6682 buffer. '''

        def __init__(self, ni6682Obj):

            super(self.__class__, self).__init__();
            self.ni6682Obj = ni6682Obj;
            self.keepWorking = True;

            print("Storeman::__init__():", self.ni6682Obj)

        def run(self):

            print("Storeman::run(): STARTED")
            boardId = self.ni6682Obj.board_id.data();

            # Arm the channels
            for i in range(0,3):

                tm = self.ni6682Obj.triggerModeDict[getattr(self.ni6682Obj, 'channel_pfi%d_trig_mode'%(i)).data()];
                decimation = getattr(self.ni6682Obj, 'channel_pfi%d_decimation'%(i)).data();

                print("Storeman::run(): boardId: ", boardId, "channel PFI%d,"%(i), " tm: ", tm, " decimation: ", decimation)

                self.ni6682Obj.libts.NI6682_ArmChannel(c_int(boardId), c_int(i), c_int(tm), c_int(decimation));

            # Main loop of the thread
            while self.keepWorking:

                for i in range(0,3):

                    sec = c_int();
                    nsec = c_int();

                    # Get the data
                    tmp = self.ni6682Obj.libts.NI6682_GetTimestamp(c_int(boardId), c_int(i), byref(sec), byref(nsec));
                    while tmp != -1:

                        t = Int64(sec.value*1000000000) + Int64(nsec.value);
                        #t = Int64(sec.value<<32) + Int64(nsec.value);
                        #print "Storeman::run(): time = ", t.value

                        # Int64Array([sec.value]) is necessary, Int64Array(sec.value) does not work.
                        getattr(self.ni6682Obj, 'channel_pfi%d_time_sec'%(i)).putRow(Int64(1), Int64Array([sec.value]), t);
                        getattr(self.ni6682Obj, 'channel_pfi%d_time_nsec'%(i)).putRow(Int64(1), Int64Array([nsec.value]), t);

                        tmp = self.ni6682Obj.libts.NI6682_GetTimestamp(c_int(boardId), c_int(i), byref(sec), byref(nsec));

                sleep(0.25);

            # Disarm the channels
            for i in range(0,3):

                self.ni6682Obj.libts.NI6682_DisarmChannel(c_int(boardId), c_int(i));
                pass

            print("Storeman::run(): STOPPED")

        def stop(self):
            ''' Stops the thread emptying the NI6682 buffers. '''
            self.keepWorking = False;


    def START_STORE(self):

        print('START_STORE')

        self.manageInfo();

        # Check data
        try:
            self.board_id.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        #self.restoreInfo();
        self.ni6682Storeman.start();

        return;

    def STOP_STORE(self):

        print('STOP_STORE')

        self.manageInfo();

        # Check data
        try:
            self.board_id.data()
        except:
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        #self.restoreInfo();
        self.ni6682Storeman.stop();

        del self.ni6682Storeman;
        del NI6682.ni6682Storemen[self.getNid()];

        return;
