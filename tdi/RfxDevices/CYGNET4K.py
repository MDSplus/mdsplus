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

__version__=(2016,10,26,16,00)
from MDSplus import mdsExceptions, Device, Tree, Dimension
from MDSplus import Int16Array, Uint16Array, Uint64Array, Float32Array
from numpy import array
from threading import Thread
from ctypes import CDLL, byref, c_ushort, c_uint32, c_int, c_char_p, c_long, c_double, create_string_buffer
from time import sleep, time
from sys import exc_info, version_info
from platform import uname
if version_info[0]<3:
    from Queue import Queue
else:
    from queue import Queue
def error(msg):
    from sys import stderr
    stderr.write('ERROR: %s\n'%msg)

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
      {'path':'.TREND', 'type':'structure'},
      {'path':'.TREND:TREE', 'type':'text','options':('no_write_shot',)},
      {'path':'.TREND:SHOT', 'type':'numeric','options':('no_write_shot',)},
      {'path':'.TREND:PCB', 'type':'text','options':('no_write_shot',)},
      {'path':'.TREND:CMOS', 'type':'text','options':('no_write_shot',)},
      {'path':'.TREND:PERIOD', 'type':'numeric','valueExpr':"Float32(1.).setUnits('s')",'options':('no_write_shot',)},
      {'path':'.TREND:ACT_START','type':'action','valueExpr':"Action(Dispatch(head.actionserver,'INIT',50,None),Method(None,'trend_start',head))",'options':('no_write_shot','write_once','disabled')},
      {'path':'.TREND:ACT_STOP','type':'action','valueExpr':"Action(Dispatch(head.actionserver,'STORE',50,None),Method(None,'trend_stop',head))",'options':('no_write_shot','write_once','disabled')},
      {'path':':ACTIONSERVER', 'type':'text','options':('no_write_shot','write_once')},
      {'path':':ACTIONSERVER:INIT', 'type':'action','valueExpr':"Action(Dispatch(head.actionserver,'INIT',20,None),Method(None,'init',head))",'options':('no_write_shot','write_once')},
      {'path':':ACTIONSERVER:START','type':'action','valueExpr':"Action(Dispatch(head.actionserver,'INIT',50,None),Method(None,'start',head))",'options':('no_write_shot','write_once')},
      {'path':':ACTIONSERVER:STOP', 'type':'action','valueExpr':"Action(Dispatch(head.actionserver,'DEINIT',20,None),Method(None,'stop',head))",'options':('no_write_shot','write_once')},
      {'path':':ACTIONSERVER:STORE','type':'action','valueExpr':"Action(Dispatch(head.actionserver,'STORE',90,None),Method(None,'store',head))",'options':('no_write_shot','write_once')},
      {'path':':BINNING', 'type':'text','options':('no_write_model','write_once')},
      {'path':':ROI_RECT', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_CMOS', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':TEMP_PCB', 'type':'numeric','options':('no_write_model','write_once')},
      {'path':':FRAMES', 'type':'signal','options':('no_write_model','write_once')},
    ]
    trigModes = {'EXTERNAL RISING':    0b11001000,
                 'EXTERNAL FALLING':   0b01001000,
                 'INTEGRATE THEN READ':0b00001100,
                 'FIXED FRAME RATE':   0b00001110}

    class _xclib(CDLL):
        serialErr = {
        '0x50':'ETX',
        '0x51':'ETX_SER_TIMEOUT',
        '0x52':'ETX_CK_SUM_ERR',
        '0x53':'ETX_I2C_ERR',
        '0x54':'ETX_UNKNOWN_CMD',
        '0x55':'ETX_DONE_LOW'}
        STREAM = True
        device_id = 1
        DRIVERPARMS = '-XU 1 -DM %d'  # allow other applications to share use of imaging boards previously opened for use by the first application
        FORMAT = 'DEFAULT'
        isOpen = False
        isInitSerial = False
        queue = None
        stream = None
        secPerTick = 4E-3;

        class _streamer(Thread):
            """a thread class that will stream frames to MDSplus tree"""
            def __init__ (self, node):
                Thread.__init__ (self)
                self.node = node
                self.daemon = True

            def run(self):
                while True:  # run until None is send
                    frameset = CYGNET4K.xclib.queue.get()
                    if frameset is None: break
                    CYGNET4K.xclib.storeFrame(self.node,frameset[0],frameset[1])
                    CYGNET4K.xclib.queue.task_done()
                CYGNET4K.xclib.queue.task_done()

        def __init__(self):
            try:
                postfix = uname()[4]
                name = '_'.join(['xclib']+[postfix])+'.so'
                super(CYGNET4K._xclib,self).__init__(name)
                self.pxd_mesgErrorCode.restype = c_char_p
            except OSError:
                error('xclib: '+ str(exc_info()[1]))

        def __del__(self):
            if self.isOpen:
                self.pxd_PIXCIclose()

        def printErrorMsg(self,status):
            error(self.pxd_mesgErrorCode(status))
            self.pxd_mesgFault(0xFF)

        def closeDevice(self):
            self.pxd_PIXCIclose();
            CYGNET4K.isOpen = False;

        def openDevice(self,dev_id,formatFile=""):
            """configFile includes exposure time which seems to take precedence over later serial commands"""
            if CYGNET4K.isOpen: return self.device_id==int(dev_id)
            self.device_id = int(dev_id)
            DRIVERPARMS = self.DRIVERPARMS % (1<<(dev_id-1))
            if Device.debug:
                print("Opening EPIX(R) PIXCI(R) Frame Grabber\nDevice parameters: '%s'" % (DRIVERPARMS,))
            status = self.pxd_PIXCIopen(c_char_p(DRIVERPARMS), c_char_p(self.FORMAT), c_char_p(formatFile))
            if status<0:
                self.printErrorMsg(status)
                CYGNET4K.isOpen = False
                return False
            if Device.debug: print("Open OK")
            CYGNET4K.isOpen = True
            self.serialUseAck = self.serialUseChk = True
            status = self.getSystemStateP()
            self.serialUseAck = status['ack']
            self.serialUseChk = status['chksum']
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
                print("buffer memory size: %.1f MB" % (self.memSize[0]/1048576.))
                print("frame buffers:      %d" % self.nBuffer)
                print("image resolution:   %d x %d" % (self.PixelsX, self.PixelsY))
                print("colors:             %d" % self.Colors)
                print("bits per pixel:     %d" % (self.Colors*self.ColorBits))
            ticku = (c_uint32*2)()
            if self.pxd_infoSysTicksUnits(ticku) == 0:
                self.secPerTick = ticku[0] / ticku[1] * 1E-6;
                if Device.debug: print("Microseconds per tick: %.1f" % (self.secPerTick * 1E6))
            return True

        def startVideoCapture(self,node):
            if self.goneLive:
                error('Cannot go live again. Re-init first!')
                raise mdsExceptions.DevERROR_DOING_INIT
            if CYGNET4K.xclib.STREAM:
                self.queue = Queue()
                self.stream = self._streamer(node)
                self.stream.start()
            else:
                self.queue = []
            status = self.pxd_goLivePair(1, c_long(1), c_long(2))
            if status<0:
                self.printErrorMsg(status)
                raise mdsExceptions.DevERROR_DOING_INIT
            self.lastCaptured = self.pxd_buffersFieldCount(1,self.pxd_capturedBuffer(1))
            self.currTime = 0
            self.Frames = 0
            for i in range(10):
                if self.goneLive: break
                else:            sleep(.3)
            if self.goneLive:
                if Device.debug: print("Video capture started.")
            else:
                error('Timeout!')
                raise mdsExceptions.DevERROR_DOING_INIT

        def _goneLive(self):
            return not self.pxd_goneLive(1,0)==0
        goneLive = property(_goneLive)

        def captureFrame(self, TriggerTime):
            currBuffer = self.pxd_capturedBuffer(1)
            currCaptured = self.pxd_buffersFieldCount(1,currBuffer)
            if currCaptured != self.lastCaptured:  # A new frame arrived
                currTicks = self.pxd_buffersSysTicks(1, currBuffer)  # get internal clock of that buffer
                if Device.debug>3: print("%d -> %d @ %d" % (self.lastCaptured, currCaptured, currTicks))
                if self.Frames == 0:  # first frame
                    self.baseTicks = currTicks;
                currTime = (currTicks - (self.baseTicks)) * self.secPerTick + TriggerTime;
                self.lastCaptured = currCaptured;
                usFrame = (c_ushort*self.PixelsToRead)()  # allocate frame
                PixelsRead = self.pxd_readushort(1, c_long(currBuffer), 0, 0, self.PixelsX, self.PixelsY, byref(usFrame), self.PixelsToRead, c_char_p("Grey"))  # get frame
                if PixelsRead != self.PixelsToRead:
                    error('ERROR READ USHORT')
                    if PixelsRead < 0: self.printErrorMsg(PixelsRead)
                    else: error("pxd_readushort error: %d != %d" % (PixelsRead, self.PixelsToRead))
                    return False
                if Device.debug: print("FRAME %d READ AT TIME %f" % (self.Frames,currTime))
                if isinstance(self.queue,Queue):
                    self.queue.put((currTime,usFrame))
                else:
                    self.queue.append((currTime,usFrame))
                self.Frames += 1
                return True
            else:  # No new frame
                return False

        def storeFrames(self,node):
            if isinstance(self.queue,Queue):
                self.stream.join()
            else:
                for frameset in self.queue:
                    self.storeFrame(node,frameset[0],frameset[1])

        def storeFrame(self,node,dim,frame):
            dims = Float32Array([dim]).setUnits('s')
            data = Int16Array(array(frame,'int16').reshape([1,self.PixelsX,self.PixelsY]))
            if Device.debug: print('storeFrame',node.minpath,dim,data.shape)
            node.makeSegment(dim,dim,dims,data)

        def stopVideoCapture(self):
            self.pxd_goUnLive(1)
            if isinstance(self.queue,Queue):
                self.queue.put(None)  # invoke stream closure
            if Device.debug: print("Video capture stopped.")

        def serialIO(self, writeBuf, BytesToRead=0):
            BytesToWrite = len(writeBuf)
            check = 0
            for i in range(BytesToWrite): check ^= ord(writeBuf[i])
            writeBuf += chr(check)
            if not self.isInitSerial:
                status = self.pxd_serialConfigure(1, 0, c_double(115200.), 8, 0, 1, 0, 0, 0)
                if status<0:
                    error("ERROR CONFIGURING SERIAL CAMERALINK PORT")
                    self.printErrorMsg(status)
                    raise mdsExceptions.DevCOMM_ERROR
                self.isInitSerial = True
                sleep(0.02)
            if Device.debug>3: print('serial write: '+' '.join(['%02x' % ord(c) for c  in writeBuf]),BytesToRead)
            while self.pxd_serialRead(1, 0, create_string_buffer(1), 1): pass
            BytesRead = self.pxd_serialWrite(1, 0, c_char_p(writeBuf), BytesToWrite+1)
            if BytesRead < 0:
                error("ERROR IN SERIAL WRITE");
                self.printErrorMsg(BytesRead)
                raise mdsExceptions.DevCOMM_ERROR
            if BytesToRead is None: return  # no response e.g. for resetMicro
            EOC = int(self.serialUseAck)+int(self.serialUseChk)  # ETX and optional check sum
            expected = BytesToRead+EOC
            cReadBuf = create_string_buffer(expected)
            timeout = time()+.01;out=[]
            while timeout>time() and expected>0:
                BytesRead = self.pxd_serialRead(1, 0, cReadBuf, expected)
                if BytesRead < 0:
                    error("ERROR IN SERIAL READ\n");
                    self.printErrorMsg(BytesRead)
                    raise mdsExceptions.DevCOMM_ERROR
                out+= cReadBuf.raw[0:BytesRead]
                expected-= BytesRead
            if Device.debug: print("SERIAL READ: %d of %d" % (len(out)-EOC, BytesToRead))
            return out[0:BytesToRead]

        '''Set Commands'''

        def setValue(self, addrlist, value, useExtReg=False):
            shift = 8*len(addrlist)-8
            for addr in addrlist:
                byte = chr((value>>shift) & 0xFF)
                shift -= 8
                if useExtReg:  # set addr of extended register
                    self.serialIO(b'\x53\xE0\x02\xF3'+addr+b'\x50');
                    addr = b'\xF4'
                self.serialIO(b'\x53\xE0\x02'+addr+byte+b'\x50');
            return self

        def resetMicro(self):
            """Will trap Micro causing watchdog and reset of firmware."""
            # The camera will give no response to this command
            self.serialIO(b'\x55\x99\x66\x11\x50\EB', None)
            return self

        def setSystemState(self,byte):
            self.serialIO(b'\x4F'+chr(byte)+b'\x50')
            return self
        def setSystemStateP(self,chksum,ack,FPGAboot,FPGAcom):
            """
            setSystemState(chksum,ack,FPGAreset,FPGAcomms)
            chksum   Bit 6 = 1 to enable check sum mode
            ack      Bit 4 = 1 to enable command ack
            FPGAboot Bit 1 = 0 to Hold FPGA in RESET; do not boot
            FPGAcom  Bit 0 = 1 to enable comms to FPGA EPROM
            """
            byte = 0
            if chksum:   byte |= 1<<6
            if ack:      byte |= 1<<4
            if FPGAboot: byte |= 1<<1
            if FPGAcom:  byte |= 1<<0
            return self.setSystemState(byte)

        def setFpgaCtrlReg(self,byte):
            """setFpgaCtrlReg(byte)"""
            return self.setValue(b'\x00',byte)
        def setFpgaCtrlRegP(self,enableTEC):
            """setFpgaCtrlRegP(enableTEC)"""
            return self.setFpgaCtrlReg(1 if enableTEC else 0)

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

        def setTrigMode(self, byte):
            """
            setTrigMode(byte)
            raising Bit 7 = 1 to enable rising edge, = 0 falling edge Ext trigger (Default=1)
            ext     Bit 6 = 1 to enable External trigger (Default=0)
            abort   Bit 3 = 1 to Abort current exposure, self-clearing bit (Default=0)
            cont    Bit 2 = 1 to start continuous seq'., 0 to stop (Default=1)
            fixed   Bit 1 = 1 to enable Fixed frame rate, 0 for continuous ITR (Default=0)
            snap    Bit 0 = 1 for snapshot, self-clearing bit (Default=0)
            """
            return self.setValue(b'\xD4',byte)

        def setTrigModeP(self, raising, ext, abort, cont, fixed, snap):
            """setTrigModeP(raising,ext,abort,cont,fixed,snap)"""
            byte = 0
            if raising: byte |= 1<<7
            if ext:     byte |= 1<<6
            if abort:   byte |= 1<<3
            if cont:    byte |= 1<<2
            if fixed:   byte |= 1<<1
            if snap:    byte |= 1<<0
            return self.setTrigMode(byte)

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

        def getSystemState(self):
            """get system state byte"""
            return  ord(self.serialIO(b'\x49\x50',1)[0])

        def getSystemStateP(self):
            """get system state as dict"""
            byte = self.getSystemState()
            return {
            'chksum':  bool(byte & 1<<6),
            'ack':     bool(byte & 1<<4),
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
            """byte = getFpgaCtrlReg()"""
            """TEC bit 0 = 1 enabled"""
            return self.getValue(b'\x00')
        def getFpgaCtrlRegP(self):
            """status = getFpgaCtrlRegP()"""
            return {'TEC':bool(self.getFpgaCtrlReg & 1<<0)}

        def getFrameRate(self):
            """get frameRate in Hz"""
            return 6E7/self.getValue(b'\xDD\xDE\xDF\xE0')

        def getExposure(self):
            """get exposure in ms"""
            return self.getValue(b'\xED\xEE\xEF\xF0\xF1')/6E4;

        def getDigitalVideoGain(self):
            """get digital video gain"""
            return self.getValue(b'\xD5\xD6',2)/512.

        def getPcbTemp(self):
            """get temperature of pc board in degC"""
            self.setValue(b'\x70',0)
            value = self.getByte() & 0xF
            self.setValue(b'\x71',0)
            value = value<<8 | self.getByte()
            return value/16.

        def getCmosTemp(self):
            """get temperature of CMOS chip (raw word)"""
            return self.getValue(b'\x7E\x7F',1,True)

        def getMicroVersion(self):
            """major,minor = getMicroVersion()"""
            return tuple(map(ord,self.serialIO(b'\x56\x50',2)[0:2]))

        def getFpgaVersion(self):
            """major,minor = getFpgaVersion()"""
            return self.getValue(b'\x7E'),self.getValue(b'\x7F')

        def getTrigMode(self):
            """
            get trigger mode byte
            raising Bit 7 = 1 to enable rising edge, = 0 falling edge Ext trigger (Default=1)
            ext     Bit 6 = 1 to enable External trigger (Default=0)
            abort   Bit 3 = 1 to Abort current exposure, self-clearing bit (Default=0)
            cont    Bit 2 = 1 to start continuous seq'., 0 to stop (Default=1)
            fixed   Bit 1 = 1 to enable Fixed frame rate, 0 for continuous ITR (Default=0)
            snap    Bit 0 = 1 for snapshot, self-clearing bit (Default=0)
            """
            return self.getValue(b'\xD4')

        def getTrigModeP(self):
            """get trigger mode as dict"""
            byte = self.getTrigMode()
            return {
            'raising': bool(byte & 1<<7),
            'ext':     bool(byte & 1<<6),
            'abort':   bool(byte & 1<<3),
            'cont':    bool(byte & 1<<2),
            'fixed':   bool(byte & 1<<1),
            'snap':    bool(byte & 1<<0)}

        def getBinning(self):
            """n = getBinning(), => binning: nxn"""
            return 1<<(self.getValue(b'\xDB')&3)

        def getRoiXSize(self):
            """get width of ROI"""
            return self.getValue(b'\xD7\xD8')

        def getRoiXOffset(self):
            """get X offset of ROI"""
            return self.getValue(b'\xD9\xDA')

        def getRoiYSize(self):
            """get height of ROI"""
            return self.getValue(b'\x01\x02',1,True)

        def getRoiYOffset(self):
            """get Y offset of ROI"""
            return self.getValue(b'\x03\x04',1,True)

        ''' currently unsupported: returns error code 0x53 ETX_I2C_ERR
        def getUnitSerialNumber(self):
            """word = getUnitSerialNumber()"""
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
        '''

        def setConfiguration(self, exposure, frameRate, trigMode):
            self.setExposure(exposure)
            self.setFrameRate(frameRate)
            self.setTrigMode(trigMode)

        def getConfiguration(self):
            self.binning    = self.getBinning()
            self.roiXSize   = self.getRoiXSize()
            self.roiXOffset = self.getRoiXOffset()
            self.roiYSize   = self.getRoiYSize()
            self.roiYOffset = self.getRoiYOffset()
            self.exposure   = self.getExposure()
            self.frameRate  = self.getFrameRate()

    isInitialized = {}
    trendworkers = {}
    workers = {}
    handels = {}
    xclib = None

    @staticmethod
    def loadLibrary():
        if CYGNET4K.xclib is None:
            CYGNET4K.xclib = CYGNET4K._xclib()

    """methods for action nodes"""

    def init(self):
        def validate(node,value):
            node.write_once = node.no_write_shot = False
            node.record = node.record.setValidation(value)
            node.write_once = node.no_write_shot = True

        dev_id = int(self.device_id.data())
        if dev_id<=0:
            error('Wrong value for DEVICE_ID, must be a positive integer.')
            raise mdsExceptions.DevINV_SETUP
        CYGNET4K.loadLibrary()
        CYGNET4K.xclib.closeDevice()  # as config file might have changed we re-open
        CYGNET4K.xclib.openDevice(dev_id,self.conf_file.data(""))  # use config file if defined else ""
        if not CYGNET4K.isOpen:
            error('Could not open camera. No camera connected?.')
            raise mdsExceptions.DevCOMM_ERROR
        exposure = self.exposure.data()
        frameRate = self.frame_rate.data()
        trigMode = self.frame_mode.data()
        if Device.debug: print('TriggerMode: %s' % trigMode)
        try:    trigMode = CYGNET4K.trigModes[trigMode.upper()]
        except: raise mdsExceptions.DevBAD_MODE
        CYGNET4K.xclib.setConfiguration(exposure,frameRate,trigMode)
        CYGNET4K.xclib.getConfiguration()
        CYGNET4K.isInitialized[dev_id] = True
        roiRect = (CYGNET4K.xclib.roiXOffset,CYGNET4K.xclib.roiYOffset,CYGNET4K.xclib.roiXSize,CYGNET4K.xclib.roiYSize)
        binning = '%dx%d' % (CYGNET4K.xclib.binning,CYGNET4K.xclib.binning)
        if Device.debug:
            print('binning:    %s' % binning)
            print('ROI:        [%d, %d, %d, %d]' % roiRect)
            print('exposure:   %f ms' % CYGNET4K.xclib.exposure)
            print('int. clock: %f Hz' % CYGNET4K.xclib.frameRate)
        roiRect = Uint16Array(roiRect).setHelp('[x,y,width,height]')
        try:
            self.binning.record = binning
            self.roi_rect.record = roiRect
            validate(self.exposure,CYGNET4K.xclib.exposure)
            validate(self.frame_rate,CYGNET4K.xclib.frameRate)
        except mdsExceptions.TreeNOOVERWRITE:
            if not (self.binning.data() == binning) and all(self.roi_rect.data() == roiRect):
                error('Re-initialization error: Parameter mismatch!')
                raise mdsExceptions.DevINV_SETUP

    def start(self,stream=None):
        dev_id = int(self.device_id.data())
        if dev_id < 0:
            error('Wrong value of DEVICE_ID, must be greater than 0.')
            raise mdsExceptions.DevINV_SETUP
        if not CYGNET4K.isInitialized.get(dev_id,False):
            error("Device not initialized: Run 'init' first.")
            raise mdsExceptions.DevINV_SETUP
        if stream is not None:
            CYGNET4K.xclib.STREAM = bool(int(stream))
        self.frames.deleteData()  # check if we can write
        self.worker = self.AsynchStore(self)
        self.saveWorker()
        self.worker.start()
        for i in range(10):
            if self.worker.running: return
            else:                   sleep(.3)
        raise mdsExceptions.MDSplusERROR

    def stop(self):
        if not self.restoreWorker():
            error('stop - cannot restore worker')
            raise mdsExceptions.MDSplusERROR
        self.worker.stop()
        for i in range(10):
            if self.worker.running: sleep(.3)
            else:                   return
        error('stop - worker stopping time out!')
        raise mdsExceptions.MDSplusERROR

    def store(self,timeout=None):
        if not self.restoreWorker():
            error('store - cannot restore worker')
            raise mdsExceptions.MDSplusERROR
        if (timeout is None):
            self.worker.join() # wait w/o timeout
        else:
            self.worker.join(float(timeout))  # wait for it to complete
            if self.worker.isAlive():  # error on timeout
                error('store - worker join time out!')
                raise mdsExceptions.MDSplusERROR

    def trend_start(self,ns=0):
        dev_id = int(self.device_id.data())
        if dev_id < 0:
            error('Wrong value for DEVICE_ID, must be greater than 0')
            raise mdsExceptions.DevINV_SETUP
        CYGNET4K.loadLibrary()
        if not CYGNET4K.isOpen:
            CYGNET4K.xclib.openDevice(dev_id)
            if not CYGNET4K.isOpen:
                error('Could not open camera. No camera connected?.')
                raise mdsExceptions.DevCOMM_ERROR
            CYGNET4K.xclib.setID(dev_id)
        try:  # test open Nodes
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
            error('Check TREND_TREE and TREND_SHOT.')
            raise mdsExceptions.TreeNODATA
        try:    ns = bool(int(ns))
        except: ns = False
        self.trendWorker = self.AsynchTrend(self, trendTree, trendShot, trendPcb, trendCmos, ns)
        self.saveTrendWorker()
        self.trendWorker.start()
        for i in range(10):
            if self.trendWorker.running: return
            else:                        sleep(.3)
        raise mdsExceptions.MDSplusERROR

    def trend_stop(self):
        if not self.restoreTrendWorker():
            raise mdsExceptions.MDSplusERROR
        self.trendWorker.stop()
        self.trendWorker.join(3)  # wait for it to complete
        if self.trendWorker.isAlive():  # error on timeout
            raise mdsExceptions.MDSplusERROR

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
        error('Cannot restore worker!!\nMaybe no worker has been started.')
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
        error('Cannot restore worker!!\nMaybe no worker has been started.')
        return False


    class AsynchStore(Thread):
        def __init__(self, device):
            Thread.__init__(self)
            self.device = device
            self.duration = float(device.duration.data())
            self.stopReq = False
            self.daemon = True
            self.running = False
            self.triggerTime = float(device.trigger_time.data(0))

        def run(self):
            """capture temps before"""
            self.pcbTemp =  [CYGNET4K.xclib.getPcbTemp(),0.]
            self.cmosTemp = [CYGNET4K.xclib.getCmosTemp(),0.]
            """start capturing frames"""
            CYGNET4K.xclib.startVideoCapture(self.device.frames)
            self.running = True
            while (not self.stopReq) and (self.duration < 0 or self.currTime < self.duration):
                if not CYGNET4K.xclib.captureFrame(self.triggerTime):
                    sleep(0.002)
            """Finished storing frames, stop camera integration and store measured frame times"""
            CYGNET4K.xclib.stopVideoCapture()
            self.running = False
            """capture temps after"""
            self.pcbTemp[1]  = CYGNET4K.xclib.getPcbTemp()
            self.cmosTemp[1] = CYGNET4K.xclib.getCmosTemp()
            print('storing')
            self.store()  # already start upload to save time
            print('run done')

        def stop(self):
            self.stopReq = True

        def store(self):
            """transfer data to tree"""
            self.device.temp_pcb.record  = Float32Array(self.pcbTemp)
            self.device.temp_cmos.record = Int16Array(self.cmosTemp)
            CYGNET4K.xclib.storeFrames(self.device.frames)

    class AsynchTrend(Thread):
        def __init__(self, device, trendTree, trendShot, trendPcb, trendCmos, ns):
            Thread.__init__(self)
            self.device = device
            self.period = float(device.trend_period.data())
            self.tree = trendTree
            self.shot = trendShot
            self.pcb = trendPcb
            self.cmos = trendCmos
            self.ns = ns
            self.stopReq = False
            self.daemon = True
            self.running = False

        def run(self):
            Tree.usePrivateCtx(True)
            try:  # test open Nodes
                tree = Tree(self.tree, self.shot)
                try:
                    tree.getNode(self.pcb)
                except Exception as exc:
                    error(exc)
                    self.pcb = None
                try:
                    tree.getNode(self.cmos)
                except Exception as exc:
                    error(exc)
                    self.cmos = None
            except Exception as exc:
                error(exc)
                error('Cannot access trend tree. Check TREND:TREE and TREND_SHOT.')
                raise mdsExceptions.TreeTNF
            if self.pcb is None and self.cmos is None:
                error('Cannot access any node for trend. Check TREND:PCB, TREND:CMOS on. Nodes must exist on %s.' % repr(tree))
                raise mdsExceptions.TreeNNF
            if self.pcb is None:
                error('Cannot access node for pcb trend. Check TREND:PCB. Continue with cmos trend.')
            elif self.cmos is None:
                error('Cannot access node for cmos trend. Check TREND:CMOS. Continue with pcb trend.')
            print('started trend writing to %s - %s and %s every %fs' % (self.tree,self.pcb,self.cmos,self.period))
            self.running = True
            while (not self.stopReq):
                timeTillNextMeasurement = self.period-(time() % self.period)
                if timeTillNextMeasurement>0.6:
                    sleep(.5)  # listen to stopReq
                else:
                    sleep(timeTillNextMeasurement);  # wait remaining period unit self.period
                    currTime = int(int(time()/self.period+.1)*self.period*1000);  # currTime in steps of self.period
                    if self.ns: currTime*= 1000000 # time in nano seconds
                    try:
                        if self.shot==0:
                            if Tree.getCurrent(self.tree) != tree.shot:
                                tree = Tree(self.tree, self.shot)
                        if self.pcb is not None:
                            pcbTemp = CYGNET4K.xclib.getPcbTemp()
                            tree.getNode(self.pcb).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Float32Array(pcbTemp).setUnits('oC'),-1)
                        if self.cmos is not None:
                            cmosTemp = CYGNET4K.xclib.getCmosTemp()
                            tree.getNode(self.cmos).makeSegment(currTime,currTime,Dimension(None,Uint64Array(currTime)),Uint16Array(cmosTemp),-1)
                        if Device.debug: print(tree.tree,tree.shot,currTime,pcbTemp,cmosTemp)
                    except Exception:
                        error(exc_info()[1])
                        error('failure during temperature readout')
                    sleep(0.01)
            self.running = False
            print('done')

        def stop(self):
            self.stopReq = True
