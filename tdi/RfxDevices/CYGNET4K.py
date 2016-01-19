from MDSplus import mdsExceptions, Device, Tree, Dimension
from MDSplus import Int16Array, Uint16Array, Uint64Array, Float32Array
from numpy import array
from threading import Thread
from ctypes import CDLL, byref, c_ushort, c_uint32, c_int, c_char_p, c_long, c_double, create_string_buffer
from time import sleep, time
from sys import exc_info
from platform import uname


class CYGNET4K(Device):
    """Cygnet 4K sCMOS Camera"""
    parts=[
      {'path':':CONF_FILE', 'type':'text','options':('no_write_shot',)},
      {'path':':COMMENT', 'type':'text'},
      {'path':':DEVICE_ID', 'type':'numeric', 'valueExpr':"Int32(1)",'options':('no_write_shot',)},
      {'path':':TRIGGER_TIME', 'type':'numeric', 'valueExpr':"Float64(0).setUnits('s')",'options':('no_write_shot',)},
      {'path':':DURATION', 'type':'numeric', 'valueExpr':"Float64(1).setUnits('s')",'options':('no_write_shot',)},
      {'path':':EXPOSURE', 'type':'numeric', 'valueExpr':"Float64(4).setUnits('ms')",'options':('no_write_shot',)}, # msec
      {'path':':FRAME_MODE', 'type':'text', 'value':'EXTERNAL RISING','options':('no_write_shot',)},
      {'path':':FRAME_RATE', 'type':'numeric', 'valueExpr':"Float64(10).setUnits('Hz')",'options':('no_write_shot',)}, # Hz
      {'path':':TREND', 'type':'structure'},
      {'path':':TREND:TREE', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:SHOT', 'type':'numeric','options':('no_write_shot',)},
      {'path':':TREND:PCB', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:CMOS', 'type':'text','options':('no_write_shot',)},
      {'path':':TREND:PERIOD', 'type':'numeric','valueExpr':"Float32(1.).setUnits('s')",'options':('no_write_shot',)},
      {'path':':TREND:ACT_START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'trend_start',head))",'options':('no_write_shot','write_once')},
      {'path':':TREND:ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',50,None),Method(None,'trend_stop',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_IDENT', 'type':'text','value':'CAMERA_SERVER','options':('no_write_shot',)},
      {'path':':ACT_INIT','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',20,None),Method(None,'init',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_START','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'INIT',50,None),Method(None,'start',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'DEINIT',20,None),Method(None,'stop',head))",'options':('no_write_shot','write_once')},
      {'path':':ACT_STORE','type':'action','valueExpr':"Action(Dispatch(head.act_ident,'STORE',90,None),Method(None,'store',head))",'options':('no_write_shot','write_once')},
      {'path':':BINNING', 'type':'text','options':('no_write_model','write_once')},
      {'path':':ROI_RECT', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_CMOS', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_PCB', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model','write_once')},
    ]
    trigModes = {'EXTERNAL RISING':    0b11000000,
                 'EXTERNAL FALLING':   0b01000000,
                 'INTEGRATE THEN READ':0b00001100,
                 'FIXED FRAME RATE':   0b00001110}
    binningModes = {0x00: '1x1',
                    0x11: '2x2',
                    0x22: '4x4'}

    class _xclib(CDLL):
        serialErr = {
        '0x50':'ETX',
        '0x51':'ETX_SER_TIMEOUT',
        '0x52':'ETX_CK_SUM_ERR',
        '0x53':'ETX_I2C_ERR',
        '0x54':'ETX_UNKNOWN_CMD',
        '0x55':'ETX_DONE_LOW'}

        DRIVERPARMS = ''
        FORMAT = 'DEFAULT'
        isOpen = False
        isInitSerial = False
        secPerTick = 1E-3;
        def __init__(self,ID=1):
            self.setID(ID)
            try:
                postfix = uname()[4]
                name = '_'.join(['xclib']+[postfix])+'.so'
                super(CYGNET4K._xclib,self).__init__(name)
                self.pxd_mesgErrorCode.restype = c_char_p
            except OSError:
                print('xclib: '+ str(exc_info()[1]))

        def __del__(self):
            if self.isOpen:
                self.pxd_PIXCIclose()
        def setID(self,ID):
            self.unitMap = 1<<(ID-1)

        def store(self,node):
            for frm,dim in zip(self.Frames,self.Times):
                data = Int16Array(array(frm,'int16').reshape([1,self.PixelsX,self.PixelsY]))
                dims = Float32Array([dim]).setUnits('s')
                if Device.debug: print(node,dim,data.shape)
                node.makeSegment(dim,dim,dims,data)

        def epixClose(self):
            self.pxd_PIXCIclose();
            CYGNET4K.isOpen = False;

        def printErrorMsg(self,status):
            print(self.pxd_mesgErrorCode(status))
            self.pxd_mesgFault(0xFF)

        def epixOpen(self,formatFile=""):
            """configFile includes exposure time which seems to take precedence over later serial commands"""
            if CYGNET4K.isOpen: return True
            if Device.debug:
                print("Opening EPIX(R) PIXCI(R) Frame Grabber\nDevice parameters: '%s'" % (self.DRIVERPARMS))
            status = self.pxd_PIXCIopen(c_char_p(self.DRIVERPARMS), c_char_p(self.FORMAT), c_char_p(formatFile))
            if status<0:
                self.printErrorMsg(status)
                CYGNET4K.isOpen = False
                return False
            if Device.debug: print("Open OK")
            CYGNET4K.isOpen = True
            self.nUnits  = self.pxd_infoUnits()
            self.memSize = [self.pxd_infoMemsize(c_int(1<<i)) for i in range(self.nUnits)]
            self.nBuffer = self.pxd_imageZdim()
            self.PixelsX = self.pxd_imageXdim()
            self.PixelsY = self.pxd_imageYdim()
            self.Colors  = self.pxd_imageCdim()
            self.ColorBits = self.pxd_imageBdim()
            self.PixelsToRead = self.PixelsX * self.PixelsY
            if Device.debug:
                print("number of boards:   %d" % self.nUnits)
                print("buffer memory size: %.3f MB" % (self.memSize[0]/1048576.))
                print("frame buffers:      %d" % self.nBuffer)
                print("image resolution:   %d x %d" % (self.PixelsX, self.PixelsY))
                print("colors:             %d" % self.Colors)
                print("bits per pixel:     %d" % (self.Colors*self.ColorBits))
            ticku = (c_uint32*2)()
            if self.pxd_infoSysTicksUnits(ticku) == 0:
                self.secPerTick = ticku[0] / ticku[1] * 1E-6;
                if Device.debug: print("Microseconds per tick: %f" % (self.secPerTick * 1E6))
            return True

        def epixStartVideoCapture(self):
            status = self.pxd_goLive(self.unitMap, c_long(1))
            if status<0:
                self.printErrorMsg(status)
                raise mdsExceptions.DevException
            self.running = False
            if Device.debug: print("Video capture started.")
            self.lastCaptured = self.pxd_capturedFieldCount(self.unitMap)
            self.currTime = 0
            self.FrameIdx = 0
            self.Frames = []
            self.Times = []

        def epixCaptureFrame(self, TriggerTime):
            currCaptured = self.pxd_capturedFieldCount(self.unitMap);
            if currCaptured != self.lastCaptured:  # A new frame arrived
                currBuffer = self.pxd_capturedBuffer(self.unitMap);
                currTicks = self.pxd_buffersSysTicks(self.unitMap, currBuffer)  # get internal clock of that buffer
                print("%d -> %d @ %d\n" % (self.lastCaptured, currCaptured, currTicks))
                if len(self.Frames) == 0:  # first frame
                    self.baseTicks = currTicks;
                currTime = (currTicks - (self.baseTicks)) * self.secPerTick + TriggerTime;
                self.lastCaptured = currCaptured;
                usFrame = (c_ushort*self.PixelsToRead)()  # allocate frame
                PixelsRead = self.pxd_readushort(self.unitMap, c_long(currBuffer), 0, 0, self.PixelsX, self.PixelsY, byref(usFrame), self.PixelsToRead, c_char_p("Grey"))  # get frame
                if Device.debug: print("FRAME %d READ AT TIME %f" % (len(self.Frames),currTime))
                if PixelsRead != self.PixelsToRead:
                    print('ERROR READ USHORT')
                    if PixelsRead < 0: self.printErrorMsg(PixelsRead)
                    else: print("pxd_readushort error: %d != %d" % (PixelsRead, self.PixelsToRead))
                    return 0
                self.Frames.append(usFrame)
                self.Times.append(currTime)
                return True
            else:  # No new frame
                return False

        def epixStopVideoCapture(self):
            self.pxd_goUnLive(self.unitMap)
            self.running = False
            if Device.debug: print("Video capture stopped.")

        def serialIO(self, writeBuf, BytesToRead=None):
            BytesToWrite = len(writeBuf)
            check = 0
            for i in range(BytesToWrite): check ^= ord(writeBuf[i])
            writeBuf += chr(check)
            if not self.isInitSerial:
                status = self.pxd_serialConfigure(self.unitMap, 0, c_double(115200.), 8, 0, 1, 0, 0, 0)
                if status<0:
                    print("ERROR CONFIGURING SERIAL CAMERALINK PORT")
                    self.printErrorMsg(status)
                    raise mdsExceptions.DevDEVICE_CONNECTION_FAILED
                self.isInitSerial = True
                sleep(0.02)
            if Device.debug>3: print('serial write: '+' '.join(['%02x' % ord(c) for c  in writeBuf]),BytesToRead)
            BytesRead = self.pxd_serialWrite(self.unitMap, 0, c_char_p(writeBuf), BytesToWrite+1)
            if BytesRead < 0:
                print("ERROR IN SERIAL WRITE");
                self.printErrorMsg(BytesRead)
                raise mdsExceptions.DevException  # error
            if BytesToRead is None: return  # no response e.g. for resetMicro
            BytesToRead += 10
            cReadBuf = create_string_buffer(BytesToRead)  # ETX and optional check sum
            sleep(0.005)
            BytesRead = self.pxd_serialRead(self.unitMap, 0, cReadBuf, BytesToRead)
            if BytesRead < 0:
                print("ERROR IN SERIAL READ\n");
                self.printErrorMsg(BytesRead)
            if Device.debug: print("SERIAL READ: %d of %d" % (BytesRead, BytesToRead))
            return cReadBuf.raw

        '''Set Commands'''

        def setValue(self, addrlist, value, useExtReg=False):
            shift = 8*len(addrlist)-8
            for addr in addrlist:
                byte = chr((value>>shift) & 0xFF)
                shift -= 8
                if useExtReg:  # set addr of extended register
                    self.serialIO(b'\x53\xE0\x02\xF3'+addr+b'\x50', 0);
                    addr = b'\xF4'
                self.serialIO(b'\x53\xE0\x02'+addr+byte+b'\x50', 0);
            return self

        def resetMicro(self):
            """Will trap Micro causing watchdog and reset of firmware."""
            # The camera will give no response to this command
            self.serialIO(b'\x55\x99\x66\x11\x50\EB')
            return self

        def setSystemState(self,chksum,ack,FPGArst,FPGAcom):
            """
            setSystemState(chksum,ack,FPGAreset,FPGAcomms)
            chksum  Bit 6 = 1 to enable check sum mode
            ack     Bit 4 = 1 to enable command ack
            FPGArst Bit 1 = 0 to Hold FPGA in RESET
            FPGAcom Bit 0 = 1 to enable comms to FPGA EPROM
            """
            byte = 0
            if chksum:  byte |= 1<<6
            if ack:     byte |= 1<<4
            if FPGArst: byte |= 1<<1
            if FPGAcom: byte |= 1<<0
            self.serialIO(b'\x4F'+chr(byte)+b'\x50')
            return self

        def setFpgaCtrlReg(self,enableTEC):
            """setFpgaCtrlReg(enableTEC)"""
            return self.setValue(b'\x00',1 if enableTEC else 0)

        def setFrameRate(self, frameRate):
            """set the frame rate in Hz"""
            # 1 count = 1*60MHz period = 16.66ns
            # 32-bit value, 4 separate commands,
            # Frame rate updated on LSB write
            return self.setValue(b'\xDD\xDE\xDF\xE0', int(6E7/frameRate))

        def setExposure(self, exposureMs):
            """set exposure time in ms"""
            # 40-bit value, 1count = 1*60 MHz period
            # Frame rate updated on LSB write
            return self.setValue(b'\xED\xEE\xEF\xF0\xF1', int(6E4*exposureMs))

        def setTrigMode(self, *args):
            """
            setTrigMode(triggerMode)
            setTrigMode(raising,ext,abort,cont,fixed,snap)
            raising Bit 7 = 1 to enable rising edge, = 0 falling edge Ext trigger (Default=1)
            ext     Bit 6 = 1 to enable External trigger (Default=0)
            abort   Bit 3 = 1 to Abort current exposure, self-clearing bit (Default=0)
            cont    Bit 2 = 1 to start continuous seq'., 0 to stop (Default=1)
            fixed   Bit 1 = 1 to enable Fixed frame rate, 0 for continuous ITR (Default=0)
            snap    Bit 0 = 1 for snapshot, self-clearing bit (Default=0)
            """
            if len(args) == 1:
                byte = args[0]
            else:
                shift = [7,6,3,2,1,0]
                byte = 0
                for i in range(6):
                    if args[i]: byte |= 1<<shift[i]
            return self.setValue(b'\xD4',byte)

        def setDigitalVideoGain(self, gain):
            """set digital video gain"""
            # 16-bit value = gain*512
            # Data updated on write to LSBs
            return self.setValue(b'\xD5\xD6', int(gain*512))

        def setBinning(self, binning):
            """setBinning(N) for NxN,  N in [1,2,4]"""
            if   binning == 1: byte = 0x00  # 1*1
            elif binning == 2: byte = 0x11  # 2*2
            elif binning == 4: byte = 0x22  # 4*4
            else: raise mdsExceptions.DevINV_SETUP
            return self.setValue(b'\xDB',byte)

        def setRoiXSize(self, value):
            """set ROI X size as 12-bit value"""
            return self.setValue(b'\xD7\xD8',min(0xFFF,value))

        def setRoiXOffset(self, value):
            """set ROI X offset as 12-bit value"""
            return self.setValue(b'\81\x82',min(0xFFF,value),True)

        def setRoiYSize(self, value):
            """set ROI Y size as 12-bit value"""
            return self.setValue(b'\x83\x84',min(0xFFF,value),True)

        def setRoiYOffset(self, value):
            """set ROI Y offset as 12-bit value"""
            return self.setValue(b'\xD7\xD8',min(0xFFF,value))

        '''Query Commands'''

        def getSystemStatus(self):
            byte = self.serialIO(b'\x49\x50',1)
            return {
            'chksum':bool(byte & 1<<6),
            'ack':bool(byte & 1<<4),
            'FPGAboot':bool(byte & 1<<2),
            'FPGAhold':bool(byte & 1<<1),
            'FPGAcomm':bool(byte & 1<<0)}

        def getByte(self,n=1):
            return ord(self.serialIO(b'\x53\xE1\x01\x50', n)[0])


        def getValue(self, addrlist, rc=1, useExtReg=False):
            value = 0
            for i,addr in enumerate(addrlist):
                if useExtReg:
                    self.setValue(addr,0x00,True)
                    addr = '\x73'
                self.serialIO(b'\x53\xE0\x01'+addr+b'\x50', rc-1)
                byte = self.getByte(rc)
                if useExtReg:  # LSBF
                    value |= byte << (i*8)
                else:  # MSBF
                    value = value<<8 | byte
            return value

        def getFpgaCtrlReg(self):
            """TECenabled = getFpgaCtrlReg()"""
            return bool(self.serialIO(b'\x00') & 1)

        def getFrameRate(self):
            """get frameRate in Hz"""
            return 6E7/self.getValue(b'\xDD\xDE\xDF\xE0')

        def getExposure(self):
            """get exposure in ms"""
            return self.getValue(b'\xED\xEE\xEF\xF0\xF1')/6E4;

        def getDigitalVideoGain(self):
            """get digital video gain"""
            return self.getValue(b'\xD5\xD6',2)/512.

        def epixGetPCBTemp(self):
            self.setValue(b'0x70',0)
            value = self.getByte() & 0xF
            self.setValue(b'0x71',0)
            value = value<<8 | self.getByte()
            return value/16.

        def epixGetCMOSTemp(self):
            return self.getValue(b'\x7E\x7F',1,True)

        def getMicroVersion(self):
            """major,minor = getMicroVersion()"""
            return tuple(map(ord,self.serialIO(b'\x56\x50',2)[0:2]))

        def getFpgaVersion(self):
            """major,minor = getFpgaVersion()"""
            return self.getValue(b'\x7E'),self.getValue(b'\x7F')

        def getTrigMode(self):
            return self.getValue(b'\xD4')

        def getBinning(self):
            return self.getValue(b'\xDB')

        def getRoiXSize(self):
            return self.getValue(b'\xD7\xD8')

        def getRoiXOffset(self):
            return self.getValue(b'\xD9\xDA')

        def getRoiYSize(self):
            return self.getValue(b'\x01\x02',1,True)

        def getRoiYOffset(self):
            return self.getValue(b'\x03\x04',1,True)

        def getUnitSerialNumber(self):
            self.serialIO(b'\x53\xAE\x05\x01\x00\x00\x02\x00\x50')
            word = self.serialIO(b'\x53\xAF\x02\x50',2)
            return ord(word[0]) | ord(word[1])<<8

        def getBuildDate(self):
            """DD,MM,YY = getBuildDate()"""
            self.serialIO(b'\x53\xAE\x05\x01\x00\x00\x04\x00\x50')
            data = self.serialIO(b'\x53\xAF\x03\x50',2)
            return tuple(map(ord,data[0:3]))

        def getBuildCode(self):
            """'ABCDE' = getBuildCode()"""
            self.serialIO(b'\x53\xAE\x05\x01\x00\x00\x07\x00\x50')
            return self.serialIO(b'\x53\xAF\x05\x50',2)[0:5]

        def getAdcCalib(self):
            """v0degC,v40degC = getAdcCalib()"""
            self.serialIO(b'\x53\xAE\x05\x01\x00\x00\x0C\x00\x50')
            data = map(ord,self.serialIO(b'\x53\xAF\x04\x50',2)[0:4])
            return data[0]|data[1]<<8,data[2]|data[3]<<8

        def getDacCalib(self):
            """v0degC,v40degC = getDacCalib()"""
            self.serialIO(b'\x53\xAE\x05\x01\x00\x00\x10\x00\x50')
            data = map(ord,self.serialIO(b'\x53\xAF\x04\x50',2)[0:4])
            return data[0]|data[1]<<8,data[2]|data[3]<<8

        def epixSetConfiguration(self, exposure, frameRate, trigMode):
            if Device.debug: print('TriggerMode: %s' % trigMode)
            try:    trigMode = CYGNET4K.trigModes[trigMode.upper()]
            except: raise mdsExceptions.DevBAD_MODE
            self.setExposure(exposure)
            self.setFrameRate(frameRate)
            self.setTrigMode(trigMode)

        def epixGetConfiguration(self):
            self.binning    = self.getBinning()
            self.roiXSize   = self.getRoiXSize()
            self.roiXOffset = self.getRoiXOffset()
            self.roiYSize   = self.getRoiYSize()
            self.roiYOffset = self.getRoiYOffset()
            self.exposure   = self.getExposure()
            self.frameRate   = self.getFrameRate()
            if Device.debug: print("EXPOSURE READ AS %f" % self.exposure)

        def epixGetTemp(self):
            self.pcbTemp  = self.epixGetPCBTemp()
            self.cmosTemp = self.epixGetCMOSTemp()

    isInitialized = {}
    trendworkers = {}
    workers = {}
    handels = {}
    xclib = None

    @staticmethod
    def loadLibrary(dev_id):
        if CYGNET4K.xclib is None:
            CYGNET4K.xclib = CYGNET4K._xclib(dev_id)

    """methods for action nodes"""

    def init(self):
        dev_id = int(self.device_id.data())
        if dev_id<0:
            print('Wrong value for DEVICE_ID, must not be negative.')
            raise mdsExceptions.DevINV_SETUP
        CYGNET4K.loadLibrary(dev_id)
        self.xclib.epixClose() # as config file is dynamically generated we want to force a re-open
        self.xclib.epixOpen(self.conf_file.data(""))
        if not CYGNET4K.isOpen:
            print('Could not open camera. No camera connected?.')
            raise mdsExceptions.DevDEVICE_CONNECTION_FAILED
        exposure = self.exposure.data()
        frameRate = self.frame_rate.data()
        trigMode = self.frame_mode.data()
        self.xclib.epixSetConfiguration(exposure,frameRate, trigMode)
        self.xclib.epixGetConfiguration()
        CYGNET4K.isInitialized[dev_id] = True
        binning = CYGNET4K.binningModes[self.xclib.binning]
        if Device.debug: print('binning %s' % binning)
        roiRect = Uint16Array([self.xclib.roiXOffset,self.xclib.roiYOffset,self.xclib.roiXSize,self.xclib.roiYSize])
        roiRect.help = '[x,y,width,height]'
        try:
            self.binning.record = binning
            self.roi_rect.record = roiRect
            self.exposure.write_once = False
            self.exposure.no_write_shot = False
            self.exposure.record = self.exposure.record.setValidation(self.xclib.exposure)
            self.exposure.no_write_shot = True
            self.exposure.write_once = True
            self.frame_rate.write_once = False
            self.frame_rate.no_write_shot = False
            self.frame_rate.record = self.frame_rate.record.setValidation(self.xclib.frameRate)
            self.frame_rate.no_write_shot = True
            self.frame_rate.write_once = True
        except mdsExceptions.TreeNOOVERWRITE:
            if not (self.binning.data() == binning) and all(self.roi_rect.data() == roiRect):
                print('Re-initialization error: Parameter mismatch!')
                raise mdsExceptions.DevINV_SETUP

    def start(self):
        dev_id = int(self.device_id.data())
        if dev_id < 0:
            print('Wrong value of DEVICE_ID, must be greater than 0.')
            raise mdsExceptions.DevINV_SETUP
        if not CYGNET4K.isInitialized.get(dev_id,False):
            print("Device not initialized: Run 'init' first.")
            raise mdsExceptions.DevException
        self.frames.deleteData()  # checks if we can write
        self.worker = self.AsynchStore()
        self.worker.configure(self)
        self.saveWorker()
        self.worker.start()

    def stop(self):
        if not self.restoreWorker():
            raise mdsExceptions.DevException
        self.worker.stop()

    def store(self):
        if not self.restoreWorker():
            raise mdsExceptions.DevException
        self.worker.join()  # wait for it to complete

    def trend_start(self):
        dev_id = int(self.device_id.data())
        if dev_id < 0:
            print('Wrong value for DEVICE_ID, must be greater than 0')
            raise mdsExceptions.DevINV_SETUP
        CYGNET4K.loadLibrary(dev_id)
        if not CYGNET4K.isOpen:
            self.xclib.epixOpen()
            if not CYGNET4K.isOpen:
                print('Could not open camera. No camera connected?.')
                raise mdsExceptions.DevDEVICE_CONNECTION_FAILED
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
        self.trendWorker.configure(self, trendTree, trendShot, trendPcb, trendCmos)
        self.saveTrendWorker()
        self.trendWorker.start()

    def trend_stop(self):
        if not self.restoreTrendWorker():
            raise mdsExceptions.DevException
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
        def configure(self, device):
            self.device = device
            self.duration = float(device.duration.data())
            self.stopReq = False
            self.daemon = True
            self.triggerTime = float(device.trigger_time.data(0))

        def run(self):
            """capture temps before"""
            self.pcbTemp =  [CYGNET4K.xclib.epixGetPCBTemp(),None]
            self.cmosTemp = [CYGNET4K.xclib.epixGetCMOSTemp()   ,None]
            """start capturing frames"""
            CYGNET4K.xclib.epixStartVideoCapture()
            while (not self.stopReq) and (self.duration < 0 or self.currTime < self.duration):
                if not CYGNET4K.xclib.epixCaptureFrame(self.triggerTime):
                    sleep(0.002)
            """Finished storing frames, stop camera integration and store measured frame times"""
            CYGNET4K.xclib.epixStopVideoCapture()
            """capture temps after"""
            self.pcbTemp[1]  = CYGNET4K.xclib.epixGetPCBTemp()
            self.cmosTemp[1] = CYGNET4K.xclib.epixGetCMOSTemp()
            self.stopReq = False
            self.store()  # already start upload to save time


        def stop(self):
            self.stopReq = True
            i = 10
            while self.stopReq and i>0:
                i-=1
                sleep(.3)
            if self.stopReq:
                raise mdsExceptions.DevException

        def store(self):
            """transfer data to tree"""
            #CYGNET4K.xclib.checkframes(self.device)
            self.device.temp_pcb.record  = Float32Array(self.pcbTemp)
            self.device.temp_cmos.record = Int16Array(self.cmosTemp)
            CYGNET4K.xclib.store(self.device.frames)
            print('done')

    class AsynchTrend(Thread):
        def configure(self, device, trendTree, trendShot, trendPcb, trendCmos):
            self.device = device
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
            print('started trend writing to %s - %s and %s every %fs' % (self.tree,self.pcb,self.cmos,self.period))
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
                            pcbTemp = CYGNET4K.xclib.epixGetPCBTemp()
                            tree.getNode(self.pcb).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(pcbTemp).setUnits('oC'),-1)
                        if self.cmos is not None:
                            cmosTemp = CYGNET4K.xclib.epixGetCMOSTemp()
                            tree.getNode(self.cmos).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Uint16Array(cmosTemp),-1)
                        if Device.debug: print(tree.tree,tree.shot,currTime,pcbTemp,cmosTemp)
                    except Exception:
                        print(exc_info()[1])
                        print('failure during temperature readout')
                    sleep(0.01)
            self.stopReq = False
            print('done')

        def stop(self):
            self.stopReq = True
            i = 10
            while self.stopReq and i>0:
                i-=1
                sleep(.3)
            if self.stopReq:
                raise mdsExceptions.DevException
