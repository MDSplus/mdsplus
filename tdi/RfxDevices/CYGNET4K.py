from MDSplus import mdsExceptions, Device, Data, Tree, Dimension, Signal, mdsExceptions
from MDSplus import Int32, Int16Array, Uint16Array, Uint64Array, Float32Array
from threading import Thread
from ctypes import CDLL, byref, c_float, c_int, c_void_p, c_char_p
from tempfile import mkstemp
from time import sleep, time
from os import close, remove

class CYGNET4K(Device):
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text','options':('no_write_shot',)},
      {'path':':COMMENT', 'type':'text'},
      {'path':':DEVICE_ID', 'type':'numeric', 'value':1,'options':('no_write_shot',)},
      {'path':':TRIGGER_TIME', 'type':'numeric', 'valueExpr':"Float32(0.).setUnits('s')",'options':('no_write_shot',)},
      {'path':':DURATION', 'type':'numeric', 'valueExpr':"Float32(3.).setUnits('s')",'options':('no_write_shot',)},
      {'path':':EXPOSURE', 'type':'numeric', 'valueExpr':"Int32(90).setUnits('ms')",'options':('no_write_shot',)}, # msec
      {'path':':FRAME_MODE', 'type':'text', 'value':'EXTERNAL RISING','options':('no_write_shot',)},
      {'path':':FRAME_RATE', 'type':'numeric', 'valueExpr':"Float32(10.).setUnits('Hz')",'options':('no_write_shot',)}, # Hz
      {'path':':TREND', 'type':'structure'},
      {'path':':TREND:TREE', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:SHOT', 'type':'numeric','options':('no_write_shot',)},
      {'path':':TREND:PCB', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:CMOS', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:PERIOD', 'type':'numeric','valueExpr':"Float32(1.).setUnits('s')",'options':('no_write_shot',)},
      {'path':':TREND:START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'start_trend',head))",'options':('no_write_shot',)},
      {'path':':TREND:STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',50,None),Method(None,'stop_trend',head))",'options':('no_write_shot',)},
      {'path':':ACT_IDENT', 'type':'text','value':'CAMERA_SERVER','options':('no_write_shot',)},
      {'path':':ACT_INIT','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'PULSE_PREP',50,None),Method(None,'init',head))",'options':('no_write_shot',)},
      {'path':':ACT_START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'start_store',head))",'options':('no_write_shot',)},
      {'path':':ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',50,None),Method(None,'stop_store',head))",'options':('no_write_shot',)},
      {'path':':BINNING', 'type':'text','options':('no_write_model','write_once')},
      {'path':':ROI_RECT', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_CMOS', 'type':'signal','options':('no_write_model','write_once')},
      {'path':':TEMP_PCB', 'type':'signal','options':('no_write_model','write_once')},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model','write_once')},
    ]
    raptorLib = None
    mdsLib = None
    isOpen = False
    isInitialized = {}
    trendworkers = {}
    workers = {}
    handels = {}

    """dummy drivers for testing"""
    class _mdsLib(object):
        def __init__(self):
            self.tree = None
        def camOpenTree(self, char_p_name, int_shot, ref_treePtr):
            try:
                self.tree = Tree(char_p_name.value,int_shot.value)
                ref_treePtr._obj.value = self.tree.ctx.value
                return mdsExceptions.TclFAILED_ESSENTIAL.status
            except:
                return -1
        def camStartSaveDeferred(self,*argin):
            return
        def camStopSave(self,*argin):
            return

    class _raptorLib(object):
        def __init__(self):
            self.clock = time()
            self.t0 = 0
        def epixClose(self):
            return
        def epixOpen(self,char_p_tmpPath, ref_xPixels, ref_yPixels):
            ref_xPixels._obj.value=2048
            ref_yPixels._obj.value=2048
            return
        def epixSetConfiguration(self, iID, float_frameRate, int_codedTrigMode):
            self.float_frameRate = float_frameRate

        def epixGetConfiguration(self,iID, ref_binning, ref_roiXSize, ref_roiXOffset, ref_roiYSize, ref_roiYOffset):
            ref_binning._obj.value = 0x00
            ref_roiXSize._obj.value = 2048
            ref_roiXOffset._obj.value = 0
            ref_roiYSize._obj.value = 2048
            ref_roiYOffset._obj.value = 0
            return
        def epixStartVideoCapture(self,*argin):
            return
        def epixCaptureFrame(self, iID, frameIdx, bufIdx, baseTicks, int_xPixels, int_yPixels, int_framesNid, int_timebaseNid, treePtr, listPtr, timeoutMs, ref_frameIdx, ref_bufIdx, ref_baseTicks, ref_currDuration):
            for i in range(100):
                now = time()
                if(frameIdx.value == 0):
                    self.t0 = now;
                currTime = now-self.t0
                if now-self.clock>1./self.float_frameRate.value:
                    self.clock = now
                    ref_frameIdx._obj.value = frameIdx.value+1
                    print("FRAME %d READ AT TIME %f" % (frameIdx.value, currTime))
                    ref_currDuration._obj.value = currTime
                    return 1
                sleep(0.005)
            ref_currDuration._obj.value = currTime
            return 0
        def epixStopVideoCapture(self,iID):
            return
        def getPCBTemp(self,iID):
            return int((36+((time()*1000) % 10)/10.)*16)
        def getCMOSTemp(self,iID):
            return int(1700+((time()*1000) % 100))
    """dummy drivers for testing - end"""

    @staticmethod  # allows it to be called eg in python for direct interaction
    def checkLibraries():
        if CYGNET4K.raptorLib is None:
            try:
                CYGNET4K.raptorLib = CDLL("libRaptor.so")
            except WindowsError as exc:
                print('Raptor: '+exc.strerror+'. Using dummy driver.')
                CYGNET4K.raptorLib = CYGNET4K._raptorLib()
        if CYGNET4K.mdsLib is None:
            try:
                CYGNET4K.mdsLib = CDLL("libcammdsutils.so")
            except WindowsError as exc:
                print('cammdsutils: '+exc.strerror+'. Using dummy driver.')
                CYGNET4K.mdsLib = CYGNET4K._mdsLib()

    def __init__(self, n):
        super(CYGNET4K,self).__init__(n)
        self.trendworker = None
        self.worker = None
        self.handle = None


    def init(self,*arg):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0.')
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        CYGNET4K.checkLibraries()
        tmpPath = self.genconf()
        xPixels = c_int(0)
        yPixels = c_int(0)
        CYGNET4K.raptorLib.epixClose() # as config file is dynamically generated we want to force a re-open
        CYGNET4K.isOpen = False
        CYGNET4K.isOpen = 0<=CYGNET4K.raptorLib.epixOpen(c_char_p(tmpPath), byref(xPixels), byref(yPixels))
        remove(tmpPath)
        frameRate = self.frame_rate.data()
        trigMode = self.frame_mode.data()
        codedTrigMode = 0
        print('TriggerMode: %s' % trigMode)
        if(trigMode == 'EXTERNAL RISING'):
            codedTrigMode = 0xC0
        elif(trigMode == 'EXTERNAL FALLING'):
            codedTrigMode = 0x40
        elif(trigMode == 'INTEGRATE THEN READ'):
            codedTrigMode = 0x0C
        elif(trigMode == 'FIXED FRAME RATE'):
            codedTrigMode = 0x0E
        else:
            return mdsExceptions.DevBAD_MODE.status
        CYGNET4K.raptorLib.epixSetConfiguration(c_int(idx), c_float(frameRate), c_int(codedTrigMode))
        binning = c_int(0)
        roiXSize = c_int(0)
        roiXOffset = c_int(0)
        roiYSize = c_int(0)
        roiYOffset = c_int(0)
        CYGNET4K.raptorLib.epixGetConfiguration(c_int(idx), byref(binning), byref(roiXSize), byref(roiXOffset), byref(roiYSize), byref(roiYOffset))
        CYGNET4K.isInitialized[idx] = True
        if(binning.value == 0x00):
            binning= '1x1'
        elif(binning.value == 0x11):
            binning = '2x2'
        elif(binning.value == 0x22):
            binning = '4x4'
        else:
            binning = '%x' % binning.value
            print('binning %s' % binning)
        roi_rect = Uint16Array([roiXOffset.value,roiYOffset.value,roiXSize.value,roiYSize.value])
        roi_rect.help = '[x,y,width,height]'
        try:
            self.binning.record = binning
            self.roi_rect.record = roi_rect
        except mdsExceptions.TreeNOOVERWRITE:
            if not (self.binning.data() == binning) and all(self.roi_rect.data() == roi_rect):
                print('Re-initialization error: Parameter mismatch!')
                return mdsExceptions.TclFAILED_ESSENTIAL.status
        return mdsExceptions.TreeNORMAL.status

    def genconf(self):
        confPath = self.conf_file.data()
        exposure = float(self.exposure.data())
        if exposure < 0.     : exposure = 0.     # must avoid negative numbers
        if exposure > 13000. : exposure = 13000. # tested with a (80 MHz) config file specifiying 10s exposures at 0.1Hz, so this is a safe limit at 60MHz
        print("EXPOSURE (SET): %f" % exposure)
        exp_clks = '%08X' % int(exposure * 60e3)
        byte_str = [exp_clks[0:2], exp_clks[2:4], exp_clks[4:6], exp_clks[6:8]]
        print(byte_str)
        line0 = '    0x124F0450,     0x53060D50,     0x06D402E0,     0xE0530650,     0x5000ED02,     0x02E05306,     0x0650' + byte_str[0] + 'EE,     0xEF02E053,     \n'
        line1 = '    0x530650' + byte_str[1] + ',     0x' + byte_str[2] + 'F002E0,     0xE0530650,     0x50' + byte_str[3] + 'F102,     0x02E05306,     0x06502FDD,     0xDE02E053,     0x530650AF,     \n'
        fh, abs_path = mkstemp()
        with open(abs_path, 'w') as adjusted:
            with open(confPath) as original:
                for line in original:
                    if   line.startswith('    0x124F04') : adjusted.write(line0)
                    elif line.startswith('    0x530650') : adjusted.write(line1)
                    else                                 : adjusted.write(line)
        close(fh)
        return abs_path

    def start_store(self,*arg):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0.')
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        if not CYGNET4K.isInitialized.get(idx,False):
            print('Device not initialized: Run init first.')
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        self.worker = self.AsynchStore()
        self.worker.configure(self, idx, self.roi_rect.data(), self.duration.data())
        self.saveWorker()
        self.worker.start()
        return mdsExceptions.TreeNORMAL.status

    def stop_store(self,*arg):
        if not self.restoreWorker():
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        self.worker.stop()
        self.worker.join()
        return mdsExceptions.TreeNORMAL.status

    def saveWorker(self):
        if self.nid in CYGNET4K.workers.keys():
            try:
                CYGNET4K.workers[self.nid].stop()
            except:
                pass
        CYGNET4K.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in CYGNET4K.workers.keys():
            self.worker = CYGNET4K.workers[self.nid]
            return True
        print('Cannot restore worker!!\nMaybe no worker has been started.')
        return False

    def start_trend(self,*arg):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0')
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        CYGNET4K.checkLibraries()
        if not CYGNET4K.isOpen:
            conffile = self.conf_file.data()
            xPixels = c_int(0)
            yPixels = c_int(0)
            CYGNET4K.isOpen = 0<=CYGNET4K.raptorLib.epixOpen(c_char_p(conffile), byref(xPixels), byref(yPixels))
        try:#test open Nodes
            trendTree = self.trend_tree.data()
            trendShot = self.trend_shot.data()
            try:
                trendPcb = self.trend_pcb.data()
            except:
                trendPcb = None
            try:
                trendCmos = self.trend_cmos.data()
            except:
                trendCmos = None
        except:
            print('Check TREND_TREE and TREND_SHOT.')
            return mdsExceptions.TreeNODATA.status
        self.trendWorker = self.AsynchTrend()
        self.trendWorker.configure(self, idx, float(self.trend_period.data()), trendTree, trendShot, trendPcb, trendCmos)
        self.saveTrendWorker()
        self.TrendWorker.start()
        return mdsExceptions.TreeNORMAL.status

    def stop_trend(self,*arg):
        if not self.restoreTrendWorker():
            return mdsExceptions.TclFAILED_ESSENTIAL.status
        self.trendWorker.stop()
        return mdsExceptions.TreeNORMAL.status

    def saveTrendWorker(self):
        if self.nid in CYGNET4K.trendworkers.keys():
            try:
                CYGNET4K.trendworkers[self.nid].stop()
            except:
                pass
        CYGNET4K.trendworkers[self.nid] = self.trendworker

    def restoreTrendWorker(self):
        if self.nid in CYGNET4K.trendworkers.keys():
            self.trendworker = CYGNET4K.trendworkers[self.nid]
            return True
        print('Cannot restore worker!!\nMaybe no worker has been started.')
        return False


    class AsynchStore(Thread):
        def configure(self, device, id, rectangle, duration):
            self.device = device
            self.id = id
            self.xPixels = rectangle[2]
            self.yPixels = rectangle[3]
            self.duration = duration
            self.frameIdx = 0
            self.stopReq = False
            self.daemon = True

        def run(self):
            treePtr = c_void_p(0)
            status = CYGNET4K.mdsLib.camOpenTree(c_char_p(self.device.tree.name), c_int(self.device.tree.shot), byref(treePtr))
            if status == -1:
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot open tree')
                return mdsExceptions.TreeFOPENW.status

            listPtr = c_void_p(0)
            CYGNET4K.mdsLib.camStartSaveDeferred(byref(listPtr)) # alt: camStartSave

            frameIdx = c_int(0)
            baseTicks = c_int(-1)
            timeoutMs = c_int(500)
            bufIdx = c_int(-1)
            timebaseNid = -1
            framesNid = self.device.frames.nid
            measuredTimes = []
            currDuration = c_float(0)

            pcbData  = Data.compile('FLOAT($VALUE/16.)').setUnits('oC')
            cmosData = Data.compile('$VALUE')

            measuredPcbTemp = []
            measuredCmosTemp = []

            CYGNET4K.raptorLib.epixStartVideoCapture(c_int(self.id))
            while not self.stopReq:
                if(self.duration < 0 or currDuration < self.duration):
                    if CYGNET4K.raptorLib.epixCaptureFrame(c_int(self.id), frameIdx, bufIdx, baseTicks, c_int(self.xPixels), c_int(self.yPixels), c_int(framesNid), c_int(timebaseNid), treePtr, listPtr, timeoutMs, byref(frameIdx), byref(bufIdx), byref(baseTicks), byref(currDuration)):
                        pcbTemp = CYGNET4K.raptorLib.getPCBTemp(c_int(self.id))
                        cmosTemp = CYGNET4K.raptorLib.getCMOSTemp(c_int(self.id))
                        measuredTimes.append(currDuration.value)
                        measuredPcbTemp.append(pcbTemp)
                        measuredCmosTemp.append(cmosTemp)
                else:
                    break
            # Finished storing frames, stop camera integration and store measured frame times
            CYGNET4K.raptorLib.epixStopVideoCapture(c_int(self.id))
            CYGNET4K.mdsLib.camStopSave(listPtr)
            dim = Dimension(None,Float32Array(measuredTimes)+self.device.trigger_time.data()).setUnits('s')
            self.device.temp_pcb.record = Signal(pcbData,Int16Array(measuredPcbTemp),dim)
            self.device.temp_cmos.record = Signal(cmosData,Int16Array(measuredCmosTemp),dim)
            print('done')
            return mdsExceptions.TreeNORMAL.status

        def stop(self):
            self.stopReq = True


    class AsynchTrend(Thread):
        def configure(self, device, id, trendPeriod, trendTree, trendShot, trendPcb, trendCmos):
            self.device = device
            self.id = id
            self.period = trendPeriod
            self.tree = trendTree
            self.shot = trendShot
            self.pcb = trendPcb
            self.cmos = trendCmos
            self.stopReq = False
            self.daemon = True

        def run(self):
            try:#test open Nodes
                tree = Tree(self.tree, self.shot)
                try:
                    tree.getNode(self.pcb)
                except Exception as exc:
                    print(exc)
                    self.pcb = None
                try:
                    tree.getNode(self.cmos)
                except Exception as exc:
                    print(exc)
                    self.cmos = None
            except Exception as exc:
                print(exc)
                print('Cannot access trend tree. Check TREND:TREE and TREND_SHOT.')
                return mdsExceptions.TreeTNF.status
            if self.pcb is None and self.cmos is None:
                print('Cannot access any node for trend. Check TREND:PCB, TREND:CMOS on. Nodes must exist on %s.' % repr(tree))
                return mdsExceptions.TreeNNF.status
            if self.pcb is None:
                print('Cannot access node for pcb trend. Check TREND:PCB. Continue with cmos trend.')
            elif self.cmos is None:
                print('Cannot access node for cmos trend. Check TREND:CMOS. Continue with pcb trend.')
            while (not self.stopReq):
                sleep(self.period-(time() % self.period));  # wait remaining period unit self.period
                currTime = int(int(time()/self.period+.1)*self.period*1000);  # currTime in steps of self.period
                try:
                    if self.shot==0:
                        if Tree.getCurrent(self.tree) != tree.shot:
                            tree = Tree(self.tree, self.shot)
                    if self.pcb is not None:
                        pcbTemp = CYGNET4K.raptorLib.getPCBTemp(c_int(self.id))/16.
                        tree.getNode(self.pcb).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(pcbTemp).setUnits('oC'),-1)
                    if self.cmos is not None:
                        cmosTemp = CYGNET4K.raptorLib.getCMOSTemp(c_int(self.id))
                        tree.getNode(self.cmos).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Uint16Array(cmosTemp),-1)
                    #print(tree.tree,tree.shot,currTime,pcbTemp,cmosTemp,tree.getNode(self.pcb).data().shape)
                except Exception as exc:
                    print(exc)
                    print('failure during temperature readout')
                sleep(0.01)
            print('done')
            return mdsExceptions.TreeNORMAL.status

        def stop(self):
            self.stopReq = True
