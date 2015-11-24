from MDSplus import Device, Data, Tree, Dimension
from MDSplus import Int32, Uint16, Float32, Uint64Array, Float32Array
from threading import Thread
from ctypes import CDLL, byref, c_float, c_int, c_void_p, c_char_p
from tempfile import mkstemp
from time import sleep, time
from os import close, remove

class CYGNET4K(Device):
    print('CYGNET4K')
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text','options':('no_write_shot')},
      {'path':':COMMENT', 'type':'text'},
      {'path':':ID', 'type':'numeric', 'value':1,'options':('no_write_shot')},
      {'path':':EXP_TIME', 'type':'numeric', 'value':1.,'options':('no_write_shot')}, # msec
      {'path':':BINNING', 'type':'text','options':('no_write_model','write_once')},
      {'path':':ROI_X', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':ROI_Y', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':ROI_WIDTH', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':ROI_HEIGHT', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'EXTERNAL RISING','options':('no_write_shot')},
      {'path':':FRAME_FREQ', 'type':'numeric', 'value':10.,'options':('no_write_shot')}, # Hz
      {'path':':FRAME_CLOCK', 'type':'numeric','options':('no_write_model')},
      {'path':':TRIG_TIME', 'type':'numeric', 'value':0.,'options':('no_write_shot')},
      {'path':':DURATION', 'type':'numeric', 'value':1.,'options':('no_write_shot')},
      {'path':':CMOS_TEMP', 'type':'numeric','options':('no_write_model')},
      {'path':':PCB_TEMP', 'type':'numeric','options':('no_write_model')},
      {'path':':TREND_TREE', 'type':'text','options':('no_write_shot')},
      {'path':':TREND_SHOT', 'type':'numeric','options':('no_write_shot')},
      {'path':':TREND_PCB', 'type':'text','options':('no_write_shot')},
      {'path':':TREND_CMOS', 'type':'text','options':('no_write_shot')},
      {'path':':FRAMES', 'type':'signal','options':('no_write_shot')}]
    parts.append({'path':':ACT_IDENT', 'type':'text',
      'value':'CAMERA_SERVER',
      'options':('no_write_shot')})
    parts.append({'path':':INIT_ACT','type':'action',
  	  'valueExpr':"Action(Dispatch(head.ACT_IDENT,'PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch(head.ACT_IDENT,'INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch(head.ACT_IDENT,'STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot')})
    parts.append({'path':':START_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch(head.ACT_IDENT,'INIT',50,None),Method(None,'start_temp_monitor',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch(head.ACT_IDENT,'STORE',50,None),Method(None,'stop_temp_monitor',head))",
	  'options':('no_write_shot')})
    raptorLib = None
    mdsLib = None
    tempworkers = {}
    workers = {}
    handels = {}
    tempworker = None
    worker = None
    handle = None


    class _mdsLib(object):
        def camOpenTree(self,*args):
            return
        def camStartSaveDeferred(self,*args):
            return
        def camStopSave(self,*args):
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
        def epixSetConfiguration(self,int_idx, float_frameRate, float_exposure, int_codedTrigMode):
            self.float_frameRate = float_frameRate

        def epixGetConfiguration(self,int_idx, ref_PCBTemperature, ref_CMOSTemperature, ref_binning, ref_roiXSize, ref_roiXOffset, ref_roiYSize, ref_roiYOffset):
            ref_PCBTemperature._obj.value = 37.0
            ref_CMOSTemperature._obj.value = 1800.0
            ref_binning._obj.value = 0
            ref_roiXSize._obj.value = 2048
            ref_roiXOffset._obj.value = 0
            ref_roiYSize._obj.value = 2048
            ref_roiYOffset._obj.value = 0
            return
        def epixStartVideoCapture(self,args):
            return
        def epixCaptureFrame(self, int_id, frameIdx, bufIdx, baseTicks, int_xPixels, int_yPixels, int_framesNid, int_timebaseNid, treePtr, listPtr, timeoutMs, ref_frameIdx, ref_bufIdx, ref_baseTicks, ref_currDuration):
            for i in range(100):
                now = time()
                if(frameIdx == 0):
                    self.t0 = now;
                    ref_currDuration._obj.value = now-self.t0
                if now-self.clock>1./self.float_frameRate.value:
                    self.clock = now
                    ref_frameIdx._obj.value = frameIdx.value+1
                    print('CAPTURED %d' % (frameIdx.value+1))
                    return 1
                sleep(0.005)
            return 0
        def epixStopVideoCapture(self,*args):
            return
        def epixGetTemp(self,int1, int0, ref_pcbTemp, ref_cmosTemp, *args):
            t = int(time()*1000)
            ref_pcbTemp._obj.value = 36+(t % 100)/100.
            ref_cmosTemp._obj.value = 1700+(t % 1000)/100.
            return

    def checkLibraries(self):
        if CYGNET4K.raptorLib is None:
            try:
                CYGNET4K.raptorLib = CDLL("libRaptor.so")
            except Exception as exc:
                print(exc)
                CYGNET4K.raptorLib = CYGNET4K._raptorLib()
        if CYGNET4K.mdsLib is None:
            try:
                CYGNET4K.mdsLib = CDLL("libcammdsutils.so")
            except Exception as exc:
                print(exc)
                CYGNET4K.mdsLib = CYGNET4K._mdsLib()

    def init(self,*args):
        self.frames.setCompressOnPut(False)
        self.checkLibraries()
        tmpPath = self.genconf()
        xPixels = c_int(0)
        yPixels = c_int(0)
        CYGNET4K.raptorLib.epixClose() # as config file is dynamically generated we want to force a re-open
        CYGNET4K.raptorLib.epixOpen(c_char_p(tmpPath), byref(xPixels), byref(yPixels))
        remove(tmpPath)
        idx = self.id.data()
        if idx <= 0:
            print('Wrong value of Device Id, must be greater than 0')
            return 0
        frameRate = self.frame_freq.data()
        exposure = self.exp_time.data()
        trigMode = self.frame_sync.data()
        codedTrigMode = 0
        if(trigMode == 'EXTERNAL RISING'):
            codedTrigMode = 0xC0
        if(trigMode == 'EXTERNAL FALLING'):
            codedTrigMode = 0x40
        if(trigMode == 'INTEGRATE THEN READ'):
            codedTrigMode = 0x0C
        if(trigMode == 'FIXED FRAME RATE'):
            codedTrigMode = 0x0E
        print("EXPOSURE: ", exposure)
        CYGNET4K.raptorLib.epixSetConfiguration(c_int(idx), c_float(frameRate), c_float(exposure), c_int(codedTrigMode))
        PCBTemp = c_float(0)
        CMOSTemp = c_float(0)
        binning = c_int(0)
        roiXSize = c_int(0)
        roiXOffset = c_int(0)
        roiYSize = c_int(0)
        roiYOffset = c_int(0)
        CYGNET4K.raptorLib.epixGetConfiguration(c_int(idx), byref(PCBTemp), byref(CMOSTemp), byref(binning), byref(roiXSize), byref(roiXOffset), byref(roiYSize), byref(roiYOffset))
        if(binning == 0x00):
            self.binning.record = '1x1'
        if(binning == 0x11):
            self.binning.record = '2x2'
        if(binning == 0x22):
            self.binning.record = '4x4'
        self.roi_x.record      = Int32(roiXOffset)
        self.roi_width.record  = Int32(roiXSize)
        self.roi_y.record      = Int32(roiYOffset)
        self.roi_height.record = Int32(roiYSize)
        return 1

    def genconf(self):
        confPath = self.conf_file.data()
        exposure = self.exp_time.data()
        if exposure < 0     : exposure = 0.0     # must avoid negative numbers
        if exposure > 13000 : exposure = 13000.0 # tested with a (80 MHz) config file specifiying 10s exposures at 0.1Hz, so this is a safe limit at 60MHz
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

    def start_store(self,*args):
        self.checkLibraries()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        self.worker.configure(self, self.id.data(), self.roi_width.data(), self.roi_height.data(), self.duration.data())
        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,*args):
        try:
            self.restoreWorker()
        except Exception as exc:
            print(exc)
            return 0
        self.worker.stop()
        self.worker.join()
        return 1

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
        else:
            raise Exception('Cannot restore worker!!\nMaybe no worker has been started.')

    def start_temp_monitor(self, *arg):
        try:#test open Nodes
            trendTree = self.trend_tree.data()
            trendShot = self.trend_shot.data()
            tree = Tree(trendTree, trendShot)
            try:
                trendPcb = self.trend_pcb.data()
                tree.getNode(trendPcb)
            except:
                trendPcb = None
            try:
                trendCmos = self.trend_cmos.data()
                tree.getNode(trendCmos)
            except:
                trendCmos = None
        except:
            print('Cannot access trend tree. Check TREND_TREE and TREND_SHOT.')
            return 0

        if trendPcb is None and trendCmos is None:
            print('Cannot access any nod for trend. Check TREND_PCB, TREND_CMOS on. Nodes must exist on Tree("%s",%d).' % (trendTree,trendShot))
            return 0
        if trendPcb is None:
            print('Cannot access node for pcb trend. Check TREND_PCB. Continue with cmos trend.')
        elif trendCmos is None:
            print('Cannot access node for cmos trend. Check TREND_CMOS. Continue with pcb trend.')
        self.checkLibraries()
        self.tempWorker = self.AsynchTemp()
        self.tempWorker.configure(self, self.id.data(), trendTree, trendShot, trendPcb, trendCmos)
        self.saveTempWorker()
        self.tempWorker.start()
        return 1

    def stop_temp_monitor(self,*args):
        try:
            self.restoreTempWorker()
        except Exception as exc:
            print(exc)
            return 0
        self.tempWorker.stop()
        return 1

    def saveTempWorker(self):
        if self.nid in CYGNET4K.tempworkers.keys():
            try:
                CYGNET4K.tempworkers[self.nid].stop()
            except:
                pass
        CYGNET4K.tempworkers[self.nid] = self.tempworker

    def restoreTempWorker(self):
        if self.nid in CYGNET4K.tempworkers.keys():
            self.tempworkers = CYGNET4K.tempworkers[self.nid]
        else:
            raise Exception('Cannot restore worker!!\nMaybe no worker has been started.')


    class AsynchStore(Thread):
        def configure(self, device, id, xPixels, yPixels, duration):
            self.device = device
            self.id = id
            self.xPixels = xPixels
            self.yPixels = yPixels
            self.duration = duration
            self.frameIdx = 0
            self.stopReq = False

        def run(self):
            treePtr = c_void_p(0)
            status = CYGNET4K.mdsLib.camOpenTree(c_char_p(self.device.getTree().name), c_int(self.device.getTree().shot), byref(treePtr))
            if status == -1:
                Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'Cannot open tree')
                return 0

            listPtr = c_void_p(0)
            CYGNET4K.mdsLib.camStartSaveDeferred(byref(listPtr)) # alt: camStartSave

            frameIdx = c_int(0)
            baseTicks = c_int(-1)
            timeoutMs = c_int(500)
            bufIdx = c_int(-1)
            timebaseNid = self.device.frame_clock.getNid()
            framesNid = self.device.frames.getNid()
            measuredTimes = []
            currDuration = c_float(0)

            pcbTemp = c_float(0)
            cmosTemp = c_float(0)

            measuredPcbTemp = []
            measuredCmosTemp = []

            CYGNET4K.raptorLib.epixStartVideoCapture(c_int(self.id))
            while not self.stopReq:
                if(self.duration < 0 or currDuration < self.duration):
                    if CYGNET4K.raptorLib.epixCaptureFrame(c_int(self.id), frameIdx, bufIdx, baseTicks, c_int(self.xPixels), c_int(self.yPixels), c_int(framesNid), c_int(timebaseNid), treePtr, listPtr, timeoutMs, byref(frameIdx), byref(bufIdx), byref(baseTicks), byref(currDuration)):
                        CYGNET4K.raptorLib.epixGetTemp(c_int(1), c_int(0), byref(pcbTemp), byref(cmosTemp))
                        measuredTimes.append(currDuration.value+self.device.trig_time.data())
                        measuredPcbTemp.append(pcbTemp.value)
                        measuredCmosTemp.append(cmosTemp.value)
                else:
                    break
            # Finished storing frames, stop camera integration and store measured frame times
            CYGNET4K.raptorLib.epixStopVideoCapture(c_int(self.id))
            CYGNET4K.mdsLib.camStopSave(listPtr)
            self.device.frame_clock.record = Float32Array(measuredTimes)
            self.device.pcb_temp.record = Float32Array(measuredPcbTemp)
            self.device.cmos_temp.record = Float32Array(measuredCmosTemp)
            return 1

        def stop(self):
            self.stopReq = True


    class AsynchTemp(Thread):
        def configure(self, device, id, hasTrend, trendTree, trendShot, trendPcb, trendCmos):
            self.device = device
            self.id = id
            self.hasTrend = hasTrend
            self.trendTree = trendTree
            self.trendShot = trendShot
            self.trendPcb = trendPcb
            self.trendCmos = trendCmos
            self.stopReq = False
            self.daemon = True

        def run(self):
            pcbTemp = c_float(0)
            cmosTemp = c_float(0)
            while (not self.stopReq):
                sleep(.5-(time() % .5));  # wait remaining period unit .5 sec step
                currTime = int(time()*2+.1)*500;  # currTime in steps of .5 sec
                CYGNET4K.raptorLib.epixGetTemp(c_int(1), c_int(0), byref(pcbTemp), byref(cmosTemp))
                tree = Tree(self.trendTree, self.trendShot)
                if self.trendPcb is not None:
                    tree.getNode(self.trendPcb).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(pcbTemp),-1)
                if self.trendCmos is not None:
                    tree.getNode(self.trendCmos).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(cmosTemp),-1)
                print(tree.tree,tree.shot,currTime,pcbTemp.value,cmosTemp.value,tree.getNode(self.trendPcb).data().shape)
                sleep(0.01)
            return 1

        def stop(self):
            self.stopReq = True
