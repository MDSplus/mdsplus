from MDSplus import Device, Data, Tree, Int32, Uint64, Float32, Float32Array
from threading import Thread
from ctypes import CDLL, byref, c_float, c_long, c_int, c_void_p, c_char_p
from tempfile import mkstemp
from time import time, sleep
from os import close

class CYGNET4K(Device):
    print('CYGNET4K')
    Int32(1).setTdiVar('_PyReleaseThreadLock')
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':ID', 'type':'numeric', 'value':1},
      {'path':':EXP_TIME', 'type':'numeric', 'value':1.}, # msec
      {'path':':BINNING', 'type':'text', 'value':'1x1'},
      {'path':':ROI_X', 'type':'numeric', 'value':0},
      {'path':':ROI_Y', 'type':'numeric', 'value':0},
      {'path':':ROI_WIDTH', 'type':'numeric', 'value':2048},
      {'path':':ROI_HEIGHT', 'type':'numeric', 'value':2048},
      {'path':':FRAME_SYNC', 'type':'text', 'value':'EXTERNAL RISING'},
      {'path':':FRAME_FREQ', 'type':'numeric', 'value':10.}, # Hz
      {'path':':FRAME_CLOCK', 'type':'numeric'},
      {'path':':TRIG_TIME', 'type':'numeric', 'value':0.},
      {'path':':DURATION', 'type':'numeric', 'value':1.},
      {'path':':CMOS_TEMP', 'type':'numeric'},
      {'path':':PCB_TEMP', 'type':'numeric'},
      {'path':':USE_TREND', 'type':'numeric', 'value':0},
      {'path':':TREND_TREE', 'type':'text'},
      {'path':':TREND_SHOT', 'type':'numeric'},
      {'path':':TREND_PCB', 'type':'text'},
      {'path':':TREND_CMOS', 'type':'text'},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model', 'no_compress_on_put')}]
    parts.append({'path':':INIT_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','PULSE_PREP',50,None),Method(None,'init',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':START_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_store',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_ACT','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_store',head))",
	  'options':('no_write_shot')})
    parts.append({'path':':START_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'start_temp_monitor',head))",
	  'options':('no_write_shot',)})
    parts.append({'path':':STOP_MON_T','type':'action',
	  'valueExpr':"Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stop_temp_monitor',head))",
	  'options':('no_write_shot')})
    raptorLib = None
    mdsLib = None
    tempworkers = {}
    workers = {}
    handels = {}

    def checkLibraries(self):
        if CYGNET4K.raptorLib is None:
            CYGNET4K.raptorLib = CDLL("libRaptor.so")
        if CYGNET4K.mdsLib is None:
            CYGNET4K.mdsLib = CDLL("libcammdsutils.so")

    def init(self,arg):
        self.frames.setCompressOnPut(False)
        self.checkLibraries()
        tmpPath = self.genconf()
        xPixels = c_int(0)
        yPixels = c_int(0)
        CYGNET4K.raptorLib.epixClose() # as config file is dynamically generated we want to force a re-open
        CYGNET4K.raptorLib.epixOpen(c_char_p(tmpPath), byref(xPixels), byref(yPixels))
        print('should uncomment line.263 "os.remove(tmpPath)" at some point --Brian')
#        os.remove(tmpPath)
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
        PCBTemperature = c_float(0)
        CMOSTemperature = c_float(0)
        binning = c_int(0)
        roiXSize = c_int(0)
        roiXOffset = c_int(0)
        roiYSize = c_int(0)
        roiYOffset = c_int(0)
        CYGNET4K.raptorLib.epixGetConfiguration(c_int(idx), byref(PCBTemperature), byref(CMOSTemperature), byref(binning), byref(roiXSize), byref(roiXOffset), byref(roiYSize), byref(roiYOffset))
        if(binning == 0x00):
            self.binning.putData('1x1')
        if(binning == 0x11):
            self.binning.putData('2x2')
        if(binning == 0x22):
            self.binning.putData('4x4')
        self.roi_x.putData(Int32(roiXOffset))
        self.roi_width.putData(Int32(roiXSize))
        self.roi_y.putData(Int32(roiYOffset))
        self.roi_height.putData(Int32(roiYSize))
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

    def start_store(self, arg):
        self.checkLibraries()
        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False
        self.worker.configure(self, self.id.data(), self.roi_width.data(), self.roi_height.data(), self.duration.data())
        self.saveWorker()
        self.worker.start()
        return 1

    def stop_store(self,arg):
        self.restoreWorker()
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
            print('Cannot restore worker!!')

    def start_temp_monitor(self, arg):
        self.checkLibraries()
        self.tempWorker = self.AsynchTemp()
        self.tempWorker.daemon = True
        self.tempWorker.stopReq = False
        useTrend = self.use_trend.data() != 0
        if useTrend:
            trendTree = self.trend_tree.data()
            trendShot = self.trend_shot.data()
            trendPcb = self.trend_pcb.data()
            trendCmos = self.trend_cmos.data()
            try:#test open Nodes
                tree = Tree(trendTree, trendShot)
                tree.getNode(trendPcb)
                tree.getNode(trendCmos)
            except:
                print('Cannot access nodes of temperature trend tree')
                useTrend = False
        self.tempWorker.configure(self, self.id.data(), useTrend, trendTree, trendShot, trendPcb, trendCmos)
        self.saveTempWorker()
        self.tempWorker.start()
        return 1

    def stop_temp_monitor(self,arg):
        self.restoreTempWorker()
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
            print('Cannot restore worker!!')


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
            prevBufIdx = -1
            timebaseNid = self.device.frame_clock.getNid()
            framesNid = self.device.frames.getNid()
            measuredTimes = []
            currDuration = c_float(0)

            CYGNET4K.raptorLib.epixStartVideoCapture(c_int(self.id))
            while not self.stopReq:
                if(self.duration < 0 or currDuration < self.duration):
                    CYGNET4K.raptorLib.epixCaptureFrame(c_int(self.id), frameIdx, bufIdx, baseTicks, c_int(self.xPixels), c_int(self.yPixels), c_int(framesNid), c_int(timebaseNid), treePtr, listPtr, timeoutMs, byref(frameIdx), byref(bufIdx), byref(baseTicks), byref(currDuration))
                    if bufIdx.value != prevBufIdx: # detect new frame
                        prevBufIdx = bufIdx.value
                        measuredTimes.append(self.device.trig_time.data() + currDuration.value)
                else:
                    break
            # Finished storing frames, stop camera integration and store measured frame times
            CYGNET4K.raptorLib.epixStopVideoCapture(c_int(self.id))
            self.device.frame_clock.putData(Float32Array(measuredTimes))
            CYGNET4K.mdsLib.camStopSave(listPtr)
            return 0

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

        def run(self):
            pcbTemp = c_float(0)
            cmosTemp = c_float(0)
            currTime = c_long(0)
            startTime = int(time())
            while (not self.stopReq):
                CYGNET4K.raptorLib.epixGetTemp(c_int(1), c_int(0), byref(pcbTemp), byref(cmosTemp), byref(currTime))
                self.device.pcb_temp.putRow(1024, Float32(pcbTemp), Uint64(currTime))
                self.device.cmos_temp.putRow(1024, Float32(cmosTemp), Uint64(currTime))
                curr_time_ms = int((currTime.value + startTime))
                if (self.hasTrend):
                    tree = Tree(self.trendTree, self.trendShot)
                    tree.getNode(self.trendPcb).putRow(1024, Float32(pcbTemp), Uint64(curr_time_ms))
                    tree.getNode(self.trendCmos).putRow(1024, Float32(cmosTemp), Uint64(curr_time_ms))
                sleep(0.5)
            return 0

        def stop(self):
            self.stopReq = True
