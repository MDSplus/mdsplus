from MDSplus import mdsExceptions, Device, Data, Tree, Dimension
from MDSplus import Int16Array, Uint16Array, Uint64Array, Float32Array
from threading import Thread
from ctypes import CDLL, byref, c_byte, c_short, c_int, c_double, c_void_p, c_char_p
from tempfile import mkstemp
from time import sleep, time
from os import close, remove

class CYGNET4K(Device):
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text','options':('no_write_shot',)},
      {'path':':COMMENT', 'type':'text'},
      {'path':':DEVICE_ID', 'type':'numeric', 'valueExpr':"Int32(1)",'options':('no_write_shot',)},
      {'path':':TRIGGER_TIME', 'type':'numeric', 'valueExpr':"Float64(0).setUnits('s')",'options':('no_write_shot',)},
      {'path':':DURATION', 'type':'numeric', 'valueExpr':"Float64(3).setUnits('s')",'options':('no_write_shot',)},
      {'path':':EXPOSURE', 'type':'numeric', 'valueExpr':"Float64(90).setUnits('ms')",'options':('no_write_shot',)}, # msec
      {'path':':FRAME_MODE', 'type':'text', 'value':'EXTERNAL RISING','options':('no_write_shot',)},
      {'path':':FRAME_RATE', 'type':'numeric', 'valueExpr':"Float64(10).setUnits('Hz')",'options':('no_write_shot',)}, # Hz
      {'path':':TREND', 'type':'structure'},
      {'path':':TREND:TREE', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:SHOT', 'type':'numeric','options':('no_write_shot',)},
      {'path':':TREND:PCB', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:CMOS', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:PERIOD', 'type':'numeric','valueExpr':"Float32(1.).setUnits('s')",'options':('no_write_shot',)},
      {'path':':TREND:ACT_START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'start_trend',head))",'options':('no_write_shot','write_once')},
      {'path':':TREND:ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',50,None),Method(None,'stop_trend',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_IDENT', 'type':'text','value':'CAMERA_SERVER','options':('no_write_shot',)},
      {'path':':ACT_INIT','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',40,None),Method(None,'init',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'start_store',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',50,None),Method(None,'stop_store',head))",'options':('no_write_shot','write_once')},
      {'path':':BINNING', 'type':'text','options':('no_write_model','write_once')},
      {'path':':ROI_RECT', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_CMOS', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_PCB', 'type':'numeric','options':('no_write_model','write_once')},
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
                return 1
            except:
                return -1
        def camStartSaveDeferred(self,*argin):
            return 1
        def camStopSave(self,*argin):
            return 1

    class _raptorLib(object):
        def __init__(self):
            self.clock = time()
            self.t0 = 0
        def epixClose(self):
            return 1
        def epixOpen(self,char_p_tmpPath):
            return 1
        def epixSetConfiguration(self, iID, float_frameRate, int_codedTrigMode):
            self.float_frameRate = float_frameRate
            return 1
        def epixGetConfiguration(self,iID, ref_binning, ref_sRoiXSize, ref_sRoiXOffset, ref_sRoiYSize, ref_sRoiYOffset):
            ref_binning._obj.value = 0x00
            ref_sRoiXSize._obj.value = 2048
            ref_sRoiXOffset._obj.value = 0
            ref_sRoiYSize._obj.value = 2048
            ref_sRoiYOffset._obj.value = 0
            return 1
        def epixStartVideoCapture(self,iID):
            return 1
        def epixCaptureFrame(self, iID, iFramesNid, dTriggerTime, iTimeoutMs, pTree, pList, piBufIdx, piFrameIdx, piBaseTicks, pdCurrTime):
            for i in range(100):
                now = time()
                if(piFrameIdx._obj.value == 0):
                    self.t0 = now;
                pdCurrTime._obj.value = now - self.t0 + dTriggerTime.value
                if now-self.clock>1./self.float_frameRate.value:
                    self.clock = now
                    piFrameIdx._obj.value = piFrameIdx._obj.value+1
                    if Device.debug:
                        print("FRAME %d READ AT TIME %f" % (piFrameIdx._obj.value, pdCurrTime._obj.value))
                    return 1
                sleep(0.005)
            return 0
        def epixStopVideoCapture(self,iID):
            return 1
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

    def genconf(self):
        confPath = self.conf_file.data()
        exposure = float(self.exposure.data())
        if exposure < 0.     : exposure = 0.     # must avoid negative numbers
        if exposure > 13000. : exposure = 13000. # tested with a (80 MHz) config file specifiying 10s exposures at 0.1Hz, so this is a safe limit at 60MHz
        if Device.debug: print("EXPOSURE (SET): %f" % exposure)
        exp_clks = '%08X' % int(exposure * 60e3)
        byte_str = [exp_clks[0:2], exp_clks[2:4], exp_clks[4:6], exp_clks[6:8]]
        if Device.debug: print(byte_str)
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

    """methods for action nodes"""

    def init(self):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0.')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        CYGNET4K.checkLibraries()
        tmpPath = self.genconf()
        CYGNET4K.raptorLib.epixClose() # as config file is dynamically generated we want to force a re-open
        CYGNET4K.isOpen = False
        CYGNET4K.isOpen = 0<=CYGNET4K.raptorLib.epixOpen(c_char_p(tmpPath))
        remove(tmpPath)
        frameRate = self.frame_rate.data()
        trigMode = self.frame_mode.data()
        codedTrigMode = 0
        if Device.debug: print('TriggerMode: %s' % trigMode)
        if(trigMode == 'EXTERNAL RISING'):
            codedTrigMode = 0xC0
        elif(trigMode == 'EXTERNAL FALLING'):
            codedTrigMode = 0x40
        elif(trigMode == 'INTEGRATE THEN READ'):
            codedTrigMode = 0x0C
        elif(trigMode == 'FIXED FRAME RATE'):
            codedTrigMode = 0x0E
        else:
            raise mdsExceptions.DevBAD_MODE
        CYGNET4K.raptorLib.epixSetConfiguration(c_int(idx), c_double(frameRate), c_byte(codedTrigMode))
        cBinning    = c_byte(0)
        sRoiXSize   = c_short(0)
        sRoiXOffset = c_short(0)
        sRoiYSize   = c_short(0)
        sRoiYOffset = c_short(0)
        CYGNET4K.raptorLib.epixGetConfiguration(c_int(idx), byref(cBinning), byref(sRoiXSize), byref(sRoiXOffset), byref(sRoiYSize), byref(sRoiYOffset))
        CYGNET4K.isInitialized[idx] = True
        if(cBinning.value == 0x00):
            binning= '1x1'
        elif(binning.value == 0x11):
            binning = '2x2'
        elif(binning.value == 0x22):
            binning = '4x4'
        else:
            binning = '%x' % binning.value
        if Device.debug: print('binning %s' % binning)
        sRoiRect = Uint16Array([sRoiXOffset.value,sRoiYOffset.value,sRoiXSize.value,sRoiYSize.value])
        sRoiRect.help = '[x,y,width,height]'
        try:
            self.binning.record = binning
            self.roi_rect.record = sRoiRect
        except mdsExceptions.TreeNOOVERWRITE:
            if not (self.binning.data() == binning) and all(self.roi_rect.data() == sRoiRect):
                print('Re-initialization error: Parameter mismatch!')
                raise mdsExceptions.TclFAILED_ESSENTIAL

    def start_store(self):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0.')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if not CYGNET4K.isInitialized.get(idx,False):
            print('Device not initialized: Run init first.')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.frames.deleteData()  # checks if we can write
        self.worker = self.AsynchStore()
        self.worker.configure(self, idx)
        self.saveWorker()
        self.worker.start()

    def stop_store(self):
        if not self.restoreWorker():
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.worker.stop()
        self.worker.join()

    def start_trend(self):
        idx = int(self.device_id.data())
        if idx < 0:
            print('Wrong value of Device Id, must be greater than 0')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        CYGNET4K.checkLibraries()
        if not CYGNET4K.isOpen:
            conffile = self.conf_file.data()
            CYGNET4K.isOpen = 0<=CYGNET4K.raptorLib.epixOpen(c_char_p(conffile))
        try:#test open Nodes
            trendTree = str(self.trend_tree.data())
            trendShot = int(self.trend_shot.data())
            try:
                trendPcb = str(self.trend_pcb.data())
            except:
                trendPcb = None
            try:
                trendCmos = str(self.trend_cmos.data())
            except:
                trendCmos = None
        except:
            print('Check TREND_TREE and TREND_SHOT.')
            raise mdsExceptions.TreeNODATA
        self.trendWorker = self.AsynchTrend()
        self.trendWorker.configure(self, id, trendTree, trendShot, trendPcb, trendCmos)
        self.saveTrendWorker()
        self.TrendWorker.start()

    def stop_trend(self):
        if not self.restoreTrendWorker():
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.trendWorker.stop()
        self.trendWorker.join()

    """worker related methods and classes"""

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
        def configure(self, device, id):
            self.device = device
            self.id = id
            self.framesNid = device.frames.nid
            self.duration = float(device.duration.data())
            self.stopReq = False
            self.daemon = True
            self.triggerTime = float(device.trigger_time.data(0))

        def run(self):
            """prepare capturing"""
            pTree = c_void_p(0)
            status = CYGNET4K.mdsLib.camOpenTree(c_char_p(self.device.tree.name), c_int(self.device.tree.shot), byref(pTree))
            if status == -1:
                Data.execute('DevLogErr($1,$2)', self.device.nid, 'Cannot open tree')
                raise mdsExceptions.TreeFOPENW
            pList = c_void_p(0)
            CYGNET4K.mdsLib.camStartSaveDeferred(byref(pList)) # alt: camStartSave
            iID = c_int(self.id)
            iFramesNid = c_int(self.framesNid)
            dTriggerTime = c_double(self.triggerTime)
            iTimeoutMs = c_int(500)
            iBufIdx = c_int(-1)
            iFrameIdx = c_int(0)
            iBaseTicks = c_int(-1)
            dCurrTime = c_double(0)
            piBufIdx = byref(iBufIdx)
            piFrameIdx = byref(iFrameIdx)
            piBaseTicks = byref(iBaseTicks)
            pdCurrTime = byref(dCurrTime)
            """capture temps before"""
            pcbTemp =  [CYGNET4K.raptorLib.epixGetPCBTemp(iID)/16.,None]
            cmosTemp = [CYGNET4K.raptorLib.epixGetCMOSTemp(iID)   ,None]
            """start capturing frames"""
            CYGNET4K.raptorLib.epixStartVideoCapture(iID)
            while (not self.stopReq) and (self.duration < 0 or dCurrTime.value < self.duration):
                CYGNET4K.raptorLib.epixCaptureFrame(iID, iFramesNid, dTriggerTime, iTimeoutMs, pTree, pList, piBufIdx, piFrameIdx, piBaseTicks, pdCurrTime)
            """Finished storing frames, stop camera integration and store measured frame times"""
            CYGNET4K.raptorLib.epixStopVideoCapture(iID)
            """capture temps after"""
            pcbTemp[1]  = CYGNET4K.raptorLib.epixGetPCBTemp(iID)/16.
            cmosTemp[1] = CYGNET4K.raptorLib.epixGetCMOSTemp(iID)
            self.device.temp_pcb.record  = Float32Array(pcbTemp)
            self.device.temp_cmos.record = Int16Array(cmosTemp)
            """transfer data to tree"""
            CYGNET4K.mdsLib.camStopSave(pList)
            print('done')

        def stop(self):
            self.stopReq = True


    class AsynchTrend(Thread):
        def configure(self, device, id, trendTree, trendShot, trendPcb, trendCmos):
            self.device = device
            self.id = id
            self.period = float(device.trend_period.data())
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
                raise mdsExceptions.TreeTNF
            if self.pcb is None and self.cmos is None:
                print('Cannot access any node for trend. Check TREND:PCB, TREND:CMOS on. Nodes must exist on %s.' % repr(tree))
                raise mdsExceptions.TreeNNF
            if self.pcb is None:
                print('Cannot access node for pcb trend. Check TREND:PCB. Continue with cmos trend.')
            elif self.cmos is None:
                print('Cannot access node for cmos trend. Check TREND:CMOS. Continue with pcb trend.')
            iID = c_int(self.id)
            while (not self.stopReq):
                timeTillNextMeasurement = self.period-(time() % self.period)
                if timeTillNextMeasurement>0.6:
                    sleep(.5)  # listen to stopReq
                else:
                    sleep(timeTillNextMeasurement);  # wait remaining period unit self.period
                    currTime = int(int(time()/self.period+.1)*self.period*1000);  # currTime in steps of self.period
                    try:
                        if self.shot==0:
                            if Tree.getCurrent(self.tree) != tree.shot:
                                tree = Tree(self.tree, self.shot)
                        if self.pcb is not None:
                            pcbTemp = CYGNET4K.raptorLib.epixGetPCBTemp(iID)/16.
                            tree.getNode(self.pcb).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(pcbTemp).setUnits('oC'),-1)
                        if self.cmos is not None:
                            cmosTemp = CYGNET4K.raptorLib.epixGetCMOSTemp(iID)
                            tree.getNode(self.cmos).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Uint16Array(cmosTemp),-1)
                        if Device.debug: print(tree.tree,tree.shot,currTime,pcbTemp,cmosTemp)
                    except Exception as exc:
                        print(exc)
                        print('failure during temperature readout')
                    sleep(0.01)
            print('done')

        def stop(self):
            self.stopReq = True
