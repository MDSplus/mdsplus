"""
RfxDevices
==========

@authors: Anton Soppelsa (IGI Padova)
@copyright: 2012
@license: GNU GPL



"""

from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
from time import *

#class NI6682:
class NI6682(Device):
    """National Instrument 6682 device. This implementation uses only a limited set of the hardware facilities.
       In particular the board is just used as a Time-to-Digital Converter (TDC). """
    
    Int32(1).setTdiVar('_PyReleaseThreadLock')

    print "NI6682"
    
    print "NI6682:: BEGIN class NI6682 declaration."

    
    parts = [{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'}, 
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'PTP'}] 

    print "NI6682:: DOPO alcuni parts = [...]."
    
    for i in range(0,3):
        parts.append({'path':'.CHANNEL_PFI%d'%(i), 'type':'structure'})
        parts.append({'path':'.CHANNEL_PFI%d:TRIG_MODE'%(i), 'type':'text', 'value':'EDGE_RISING'})
        parts.append({'path':'.CHANNEL_PFI%d:DECIMATION'%(i), 'type':'numeric', 'value':1})
        parts.append({'path':'.CHANNEL_PFI%d:TIME_SEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
        parts.append({'path':'.CHANNEL_PFI%d:TIME_NSEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
        
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

    print "NI6682:: AT THE END of parts = [...]."

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
    libts = CDLL("libtimestamping.so");
    
    # Declaration of global dictionary of storemen
    ni6682Storemen = {}
    
    print "NI6682:: END of class declaration."
    
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
        
        print "NI6682::manageInfo(): BEGIN"
        
        n = self.getNid()
        #print "self.getNid() = ", n
        #print "self.getFullPath() = ", self.getFullPath()

        try:
            
            NI6682.ni6682Storemen[n];
        except:

            NI6682.ni6682Storemen[n] = self.Storeman(self);
            print 'NI6682::manageInfo(): INFO CREATED'

        self.ni6682Storeman = NI6682.ni6682Storemen[n];
        
        print 'NI6682::manageInfo(): INFO RESTORED'

        print "NI6682::manageInfo(): END"

            
# INIT
    def INITIALISE(self, arg):
        
        print 'INITIALISE'

        self.manageInfo();

        try:
            boardId = self.board_id.data() 
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
        
        
        res = NI6682.libts.NI6682_Create(c_int(boardId));
        print 'NI6682_Create(): ', res
        
        res = NI6682.libts.NI6682_Initialise(c_int(boardId));
        print 'NI6682_Initialise(): ', res
        
        
        for i in range(0,3):
                
            tm = getattr(self, 'channel_pfi%d_trig_mode'%(i)).data();
            
            try:
                triggerMode = self.triggerModeDict[tm]
                # Better using __dict__ because getattr calls the constructor. Not possible why? 
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                return 0
            print 'CHANNEL_PFI%d.TRIG_MODE: '%(i), tm
            
            #TODO decimation must be greather than 0
            try:
                decimation = getattr(self, 'channel_pfi%d_decimation'%(i)).data();
                if decimation < 1:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Decimation must be greater than 0: ', decimation)
                    return 0
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                return 0
            print 'CHANNEL_PFI%d.DECIMATION: '%(i), decimation
            
        # If executed for the first time

        return 1;

# STORE
    def FINALISE(self, arg):
        
        print 'FINALISE'
        
        self.manageInfo();

        #self.restoreInfo();
        try:
            boardId = self.board_id.data() 
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0

        
        res = NI6682.libts.NI6682_Finalise(c_int(boardId));
        print 'NI6682_Finalise(): ', res
        
        res = NI6682.libts.NI6682_Destroy(c_int(boardId));
        print 'NI6682_Destroy(): ', res
        
        return 1;
        
    
    class Storeman(Thread):
        ''' This class is used to create the thread keeping empty the NI6682 buffer. '''
        
        def __init__(self, ni6682Obj):
            
            super(self.__class__, self).__init__();
            self.ni6682Obj = ni6682Obj;
            self.keepWorking = True;
            
            print "Storeman::__init__():", self.ni6682Obj
            
        def run(self):
            
            print "Storeman::run(): STARTED"
            boardId = self.ni6682Obj.board_id.data();
            
            # Arm the channels
            for i in range(0,3):
                
                tm = self.ni6682Obj.triggerModeDict[getattr(self.ni6682Obj, 'channel_pfi%d_trig_mode'%(i)).data()];
                decimation = getattr(self.ni6682Obj, 'channel_pfi%d_decimation'%(i)).data();
                
                print "Storeman::run(): boardId: ", boardId, "channel PFI%d,"%(i), " tm: ", tm, " decimation: ", decimation
                
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
                
            print "Storeman::run(): STOPPED"

        def stop(self):
            ''' Stops the thread emptying the NI6682 buffers. ''' 
            self.keepWorking = False;
    
    
    def START_STORE(self, arg):
        
        print 'START_STORE'

        self.manageInfo();
        
        # Check data
        try:
            self.board_id.data() 
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0

        #self.restoreInfo();
        self.ni6682Storeman.start();
    
        return 1;
    
    def STOP_STORE(self, arg):
        
        print 'STOP_STORE'
        
        self.manageInfo();
        
        # Check data
        try:
            self.board_id.data() 
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
        
        
        #self.restoreInfo();
        self.ni6682Storeman.stop();
        
        del self.ni6682Storeman;
        del NI6682.ni6682Storemen[self.getNid()];
        
        return 1;
    