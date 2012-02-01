from MDSplus import *
from numpy import *
from threading import *
from ctypes import *
from time import *

class NI6682(Device):

    print "NI6682"
    #print "NI6682:: PRIMA di Int32(1).setTdiVar('_PyReleaseThreadLock')"
  
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    
    #print "NI6682:: DOPO Int32(1).setTdiVar('_PyReleaseThreadLock')"
    
    """National Instrument 6682 device"""
    parts = [{'path':':BOARD_ID', 'type':'numeric', 'value':0},
        {'path':':COMMENT', 'type':'text'}, 
        {'path':':CLOCK_SOURCE', 'type':'text', 'value':'PTP'}] 

    for i in range(1,4):
        parts.append({'path':'.CHANNEL_%d'%(i), 'type':'structure'})
        parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i), 'type':'text', 'value':'EDGE_RISING'})
        parts.append({'path':'.CHANNEL_%d:DECIMATION'%(i), 'type':'numeric', 'value':1})
        parts.append({'path':'.CHANNEL_%d:TIME_SEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
        parts.append({'path':'.CHANNEL_%d:TIME_NSEC'%(i), 'type':'numeric', 'options':('no_write_model',)})
        
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'INIT',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','STORE',50,None),Method(None,'STORE',head))",
        'options':('no_write_shot',)})
    
    triggerModeDict = {'EDGE_RISING':0, 'EDGE_FALLING':1, 'EDGE_ANY':2}
    channelsDict = {'PFI0':0, 'PFI1':1, 'PFI2':2}
    
    #print "NI6682:: DOPO inizializzazioni e PRIMA di CDLL()"
    
    libts = CDLL("/usr/local/lib/libtimestamping.so");
    #print libts
    #print "NI6682:: DOPO CDLL()"
    
    def saveInfo(self):
        
        global ni6682Nids
        global ni6682Ptrs
#        global ni6682Storemen
        
        try:
            ni6682Nids
        except:
            ni6682Nids = []
            ni6682Ptrs = []
#            ni6682Storemen = []
            
        try:
            idx = ni6682Nids.index(self.getNid())
        except:
            print 'SAVE INFO: SAVING HANDLE'
            ni6682Nids.append(self.getNid())
            ni6682Ptrs.append(self.ni6682Ptr)
#            ni6682Storemen.append(self.ni6682Storeman)
            return
        
        ni6682Ptrs[idx] = self.ni6682Ptr
        return

    def restoreInfo(self):
        
        global ni6682Nids
        global ni6682Ptrs
 #       global ni6682Storemen
        
        try:
            idx = ni6682Nids.index(self.getNid())
            self.ni6682Ptr = ni6682Ptrs[idx]
 #           self.ni6682Storeman = ni6682Storemen[idx]
            print 'RESTORE INFO HANDLE FOUND'
        except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot find 6682 Object. Maybe STORE called before INIT?' )
            return 0
        return 1
            
# INIT
    def INIT(self, arg):
        print 'INIT'
        try:
            boardId = self.board_id.data() 
            print 'BOARD_ID: ' + str(boardId)
        except: 
            Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid BOARD_ID')
            return 0
        print 'INIT1'
        self.ni6682Ptr = self.libts.PXI6682_Create(c_int(boardId));
        print 'INIT2'
        self.ni6682Storeman = Storeman();
        
        self.libts.PXI6682_Initialise(self.ni6682Ptr);
        
        for i in range(0,3):
            try:
                triggerMode = self.triggerModeDict[getattr(self, 'channel_%d_trig_mode'%(i+1)).data()]
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                return 0
            print 'CHANNEL_%d.TRIG_MODE: '%(i+1), getattr(self, 'channel_%d_trig_mode'%(i+1)).data()
            #TODO decimation must be greather than 0
            try:
                decimation = getattr(self, 'channel_%d_decimation'%(i+1)).data();
                if decimation < 1:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Decimation must be greater than 0: ', decimation)
                    return 0
            except:
                Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid TRIG_MODE')
                return 0
            print 'CHANNEL_%d.DECIMATION: '%(i+1), decimation
            print 'INIT4'
            self.libts.PXI6682_ArmChannel(self.ni6682Ptr, c_int(i), c_int(triggerMode), c_int(decimation))
        self.saveInfo()
        return 1

# STORE
    def STORE(self, arg):
        print 'STORE'
        self.restoreInfo()

        for i in range(0,3):
            sec = c_int();
            nsec = c_int();
            print 'STORE1'
            self.libts.PXI6682_GetTimestamp(self.ni6682Ptr, c_int(i), byref(sec), byref(nsec));
            print 'STORE2'
            getattr(self, 'channel_%d_time_sec'%(i+1)).putData(sec.value);
            print 'STORE3'
            getattr(self, 'channel_%d_time_nsec'%(i+1)).putData(nsec.value);

        for i in range(0,3):
            print 'STORE4'
            self.libts.PXI6682_DisarmChannel(self.ni6682Ptr, c_int(i));

        print 'STORE5'
        self.libts.PXI6682_Finalise(self.ni6682Ptr);
        print 'STORE6'
        self.libts.PXI6682_Destroy(self.ni6682Ptr);
        print 'STORE7'
        
        return 1
        
    ## 
    #class Storeman(Tread):
        
        #keepWorking = True;
        
        #def __init__():
            #Thread.__init__();
            
            
        #def run(self, libts, devicePtr, channelNumber):
            
            ## Arm the channels
            
            #while keepWorking:
                
                #sec = c_int();
                #nsec = c_int();
                
                #while libts.PXI6682_GetTimestamp(devicePtr, channelNumber, byref(sec), byref(nsec)) == -1;
                
            ## Disarm the channels
            
        #def stop(self):
            
            #self.isWorking = False;
            
    #storeman = Storeman();
    
    #def START_STORE(self):
        #self.storeman.start();
    
    #def STOP_STORE(self):
        #self.storeman.stop();
    
    