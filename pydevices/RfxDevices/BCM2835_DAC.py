from MDSplus import mdsExceptions, Device, Data, Dimension, Range, Window
from MDSplus import Int32, Float32, Float32Array, Float64, Event
from ctypes import *
from threading import Thread
import time
import os
import errno
import traceback
import numpy as np
import json
import sys
import socket


class BCM2835_DAC(Device):
    """BCM 2835 Serial Periferal Interface Analog Output Waveform Generation"""
    parts = [{'path': ':COMMENT',      'type': 'text'},
             {'path': ':BOARD_IP',  'type': 'text'},
             {'path': ':WAVE_POINT',   'type': 'numeric', 'value': 1000},
             {'path': ':TRIG_MODE',    'type': 'text', 'value': 'EXTERNAL'},
             {'path': ':TRIG_SOURCE',  'type': 'numeric', 'value': 0},
             {'path': ':GAIN',         'type': 'numeric', 'value': 1},
             {'path': ':CMD_EVENT',    'type': 'text',    'value': 'WAVE_UPDATE'}]

    for i in range(0, 4):
        parts.append({'path': '.AO_%d' % (i+1),        'type': 'structure'})
        parts.append({'path': '.AO_%d:MAX' % (i+1),
                      'type': 'numeric', 'value': 1.})
        parts.append({'path': '.AO_%d:MIN' % (i+1),
                      'type': 'numeric', 'value': 0.})
        parts.append({'path': '.AO_%d:X' % (i+1),      'type': 'numeric'})
        parts.append({'path': '.AO_%d:Y' % (i+1),      'type': 'numeric'})
        parts.append({'path': '.AO_%d:TYPE' % (i+1),
                      'type': 'text', 'value': 'SIN'})
        parts.append({'path': '.AO_%d:FREQ' % (i+1),
                      'type': 'numeric', 'value': 1.})
        parts.append({'path': '.AO_%d:GAIN' % (i+1),
                      'type': 'numeric', 'value': 1.})
        parts.append({'path': '.AO_%d:OFFSET' % (
            i+1), 'type': 'numeric', 'value': 0.})
    del (i)

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','READY',50,None),Method(None,'start_gen',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PXI_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_gen',head))",
                  'options': ('no_write_shot',)})


# File descriptor
    fd = 0

# Driver constants
    bcm2835Fds = {}

#Dictionaries and maps
    bcm2835InterfaceLib = None
    asyncStoreManagerLib = None
    updateEvents = {}

    DEV_IS_OPEN = 1
    DEV_OPEN = 2

    MIN_FREQ = 0.01
    MAX_FREQ = 100.
    DAC_MAX_FREQ = 10000.

#Dictionaries and maps
    waveTypeDict = {'AS_IS': 1, 'SIN': 2, 'COS': 3,
                    'TRIANGULAR': 4, 'SQUARE': 5, 'SAWTOOTH': 6}
    triggerModeDict = {'INTERNAL': 0, 'EXTERNAL': 1}

    HANDLE_NOT_FOUND = 1
    HANDLE_FOUND = 2
    NO_ERROR = 1
    ERROR = -1

    def MCD(a, b):
        if(b != 0):
            r = a % b
            a = b
            b = r
        return a

    def mcm(a, b):
        return a*b/MCD(a, b)

    def getIpAddress(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 1))  # connect() for UDP doesn't send packets
        local_ip_address = s.getsockname()[0]
        s.close()
        return local_ip_address

    def debugPrint(self, msg="", obj=""):
        msger = ""
        """ 
          if BCM2835_DAC.bcm2835InterfaceLib is not None:
              errno = BCM2835_DAC.bcm2835InterfaceLib.getErrno();
              print "erno ", errno
              if errno is not None:
                  msger = 'Error (%d) %s' % (errno, os.strerror( errno ))
          """
        print(self.name + ":" + msg, obj, msger)

    def saveInfo(self):
        BCM2835_DAC.bcm2835Fds[self.getNid()] = self.fd

    def restoreInfo(self, openFlag):
        if BCM2835_DAC.bcm2835InterfaceLib is None:
            BCM2835_DAC.bcm2835InterfaceLib = CDLL("libBCM2835Interface.so")
        if BCM2835_DAC.asyncStoreManagerLib is None:
            BCM2835_DAC.asyncStoreManagerLib = CDLL("libAsyncStoreManager.so")

        try:
            self.fd = BCM2835_DAC.bcm2835Fds[self.getNid()]
            print('RESTORE INFO HANDLE FOUND')
            return self.DEV_IS_OPEN
        except:
            if openFlag:
                try:
                    boardIp = self.board_ip.data()
                    IPAddr = self.getIpAddress()
                    if boardIp != IPAddr:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Invalid Board IP (%s) device run on IP %s' % (boardIp, IPAddr))
                        return self.ERROR
                except Exception as ex:
                    emsg = 'Missing Board IP '+str(ex)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    return self.ERROR

                try:
                    self.fd = BCM2835_DAC.bcm2835InterfaceLib.spiConfig()
                except Exception as e:
                    emsg = 'Cannot initialized device %s : %s' % (
                        boardIp, str(e))
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise DevBAD_PARAMETER

            return self.DEV_OPEN

    def closeInfo(self):
        if self.nid in BCM2835_DAC.bcm2835Fds.keys():
            self.fd = BCM2835_DAC.bcm2835Fds[self.nid]
            del(BCM2835_DAC.bcm2835Fds[self.nid])
            try:
                if BCM2835_DAC.bcm2835InterfaceLib.spiClose() < 0:
                    emsg = 'Cannot CLOSE device id ' + str(self.fd)
                    Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                    raise
            except Exception as ex:
                emsg = 'Cannot CLOSE device Exception : %s' % str(ex)
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise mdsExceptions.DevBAD_PARAMETER(emsg)
        return

# Update Event Management

    def saveUpdateEvent(self):
        BCM2835_DAC.updateEvents[self.getNid()] = self.updEvent

    def restoreUpdateEvent(self):
        try:
            self.updEvent = BCM2835_DAC.updateEvents[self.nid]
            return BCM2835_DAC.HANDLE_FOUND
        except:
            print('Cannot restore update event!!')
        return BCM2835_DAC.HANDLE_NOT_FOUND

# AsynchUpdateGen class
    class AsynchUpdateGen(Event):

        ACQ_NOERROR = 0
        ACQ_ERROR = 1
        device = None

        def configure(self, device):
            self.device = device
            return

        def run(self):

            msg = "Operation Executed"

            print("Event %s occurred at %s with data: %s " %
                  (str(self.event), str(self.qtime.date), str(self.raw)))
            jcmd = self.raw.deserialize()
            print (jcmd)

            cmdEventName = self.device.cmd_event.data()

            if jcmd["command"] == 'pause':
                try:
                    self.device.__stopGeneration__()
                except Exception as ex:
                    Event.setevent(cmdEventName+'_REPLY',
                                   'Cannot pause waveform generation : '+str(ex))
            elif jcmd["command"] == 'resume':
                try:
                    self.device.configChannels(False)
                    self.device.__startGeneration__(isUpdate=True)
                except Exception as ex:
                    Event.setevent(cmdEventName+'_REPLY',
                                   'Cannot resume waveform generation : '+str(ex))
            elif jcmd["command"] == 'update':
                wavePoint = self.device.wave_point.data()
                gain = self.device.gain.data()

                chStateAr = np.asarray(jcmd['chState'])
                chMaxAr = np.asarray(jcmd['chMax'])
                chMinAr = np.asarray(jcmd['chMin'])
                chTypeAr = np.asarray(jcmd['chType'])
                chAOXAr = np.asarray(jcmd['chAOX'])
                chAOYAr = np.asarray(jcmd['chAOY'])
                chFreqAr = np.asarray(jcmd['chFreq'])

                minFreq = 1e6
                for ch in range(4):
                    if chStateAr[ch]:
                        if chFreqAr[ch] < minFreq:
                            minFreq = chFreqAr[ch]

                numPeriod = (np.asarray(chFreqAr) / minFreq + 0.5).astype(int)

                periodPoint = []
                for ch in range(4):
                    if getattr(self, 'ao_%d' % (ch+1)).isOn():
                        fr = getattr(self, 'ao_%d_freq' % (ch+1)).data()
                        periodPoint.append(self.DAC_MAX_FREQ / fr)
                    else:
                        periodPoint.append(0)

                wavePoint = (np.asarray(periodPoint) + 0.5).astype(int)
                print ('Wave point ', wavePoint)

                try:
                    self.device.__stopGeneration__()
                    self.device.closeInfo()
                    if self.device.restoreInfo(True) == self.ERROR:
                        Data.execute('DevLogErr($1,$2)',
                                     self.device.getNid(), 'Cannot open device')
                        raise mdsExceptions.DevINV_SETUP
                    self.device.saveInfo()

                    for ch in range(4):
                        self.device.configChannel(
                            ch, False, wavePoint[ch], chMaxAr[ch], chMinAr[ch], chTypeAr[ch].strip(), chAOXAr[ch], chAOXAr[ch], 1)

                    self.device.__startGeneration__(isUpdate=True)

                except Exception as ex:
                    Event.setevent(
                        cmdEventName+'_REPLY', 'Cannot update waveforms generation : '+str(ex))

            Event.setevent(cmdEventName+'_REPLY', msg)
            return

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)

        def closeTree(self):
            # On first test dosen't work
            # NI6259AI.niInterfaceLib.closeTree(self.treePtr)
            self.device.debugPrint('CLOSE TREE')

# End Inner class AsynchStore

    def buildWave(self, wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod):

        offset = (chMax + chMin) / 2
        level = (chMax - chMin) / 2

        print ("ch type ", chType)

        # create one complete sine period in volts

        if chType == 'SIN':
            ix = np.arange(wavePoint)
            sigData = offset + level * \
                np.sin(2*np.pi*nPeriod*ix/float(wavePoint))

        elif chType == 'SAWTOOTH':
            wp = wavePoint/nPeriod
            ix = np.arange(wp)
            sigData = (offset + 2 * level * ix/float(wp) - level)
            sigData = np.tile(sigData, nPeriod)

        elif chType == "COS":
            ix = np.arange(wavePoint)
            sigData = offset + level * \
                np.cos(2*np.pi*nPeriod*ix/float(wavePoint))

        elif chType == "TRIANGULAR":
            wp = wavePoint/nPeriod
            ix = np.arange(wp/2)
            ix1 = wp/2 + np.arange(wp/2)
            sigData = (offset + 2*level * ix/float(wp/2) - level)
            sigData = np.append(
                sigData, (offset - 2 * level * ix1/float(wp/2) + 3 * level))
            sigData = np.tile(sigData, nPeriod)

        elif chType == "SQUARE":
            wp = wavePoint/nPeriod
            ix = np.zeros(wp/2) + 1
            sigData = (offset + level * ix)
            sigData = np.append(sigData, (offset - level * ix))
            sigData = np.tile(sigData, nPeriod)

        elif chType == "AS_IS":
            xi = np.linspace(aoX[0], aoX[len(aoX)-1], wavePoint)
            #sigData = offset + level * np.interp(xi, aoX, aoY, wavePoint)
            sigData = np.interp(xi, aoX, aoY, wavePoint)
            print (sigData)
        else:
            return None

        scaledWriteArray = (c_float * len(sigData))(*sigData)

        return scaledWriteArray

    def configChannel(self, ch, reset, wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod, gain, offset):

        isOn = getattr(self, 'ao_%d' % (ch+1)).isOn()

        if not reset and isOn:
            print ("configChannel ", ch, reset, wavePoint,
                   chMax, chMin, chType, aoX, aoY, nPeriod)
            scaledWriteArray = self.buildWave(
                wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod)
        else:
            zero_arr = np.zeros(wavePoint)
            scaledWriteArray = (c_float * len(zero_arr))(*zero_arr)

        if scaledWriteArray == None:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), "Invalid waveform type -%s- for channel %d " % (chType, ch))
            zero_arr = np.zeros(wavePoint)
            scaledWriteArray = (c_float * len(zero_arr))(*zero_arr)

        retval = BCM2835_DAC.bcm2835InterfaceLib.bcm2835_write_ao(
            ch, scaledWriteArray, c_float(gain), c_float(offset), c_uint(wavePoint))
        if retval != wavePoint:
            errno = BCM2835_DAC.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), "Failed while writing! Written samples %d : %s (%d)" % (retval, os.strerror(errno), errno))
            raise mdsExceptions.DevIO_STUCK

        del scaledWriteArray

    def __checkChanneParam__(self, ch, gain):

        chAOX = []
        chAOY = []

        isOn = getattr(self, 'ao_%d' % (ch+1)).isOn()

        try:
            chGain = getattr(self, 'ao_%d_gain' % (ch+1)).data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid channel %d gain value : %s' % (ch+1, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chOffset = getattr(self, 'ao_%d_offset' % (ch+1)).data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Invalid channel %d offset value : %s' % (ch+1, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chMax = getattr(self, 'ao_%d_max' % (ch+1)).data()/gain
            bitAdc = int(chMax * chGain + chOffset)
            if bitAdc < 0 or bitAdc > 4095:
                raise mdsExceptions.DevBAD_PARAMETER
        except Exception as ex:
            val = 0
            if bitAdc > 0:
                val = (4095 - chOffset) / chGain
            getattr(self, 'ao_%d_max' % (ch+1)).putData(Float32(val))
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Invalid channel %d max value : value cut to value %.2f []' % (ch+1, val))
            #raise mdsExceptions.DevBAD_PARAMETER

        try:
            chMin = getattr(self, 'ao_%d_min' % (ch+1)).data()/gain
            bitAdc = int(chMin * chGain + chOffset)
            if bitAdc < 0 or bitAdc > 4095:
                raise mdsExceptions.DevBAD_PARAMETER
        except Exception as ex:
            val = 0
            if bitAdc > 0:
                val = (4095 - chOffset) / chGain
            getattr(self, 'ao_%d_min' % (ch+1)).putData(Float32(val))
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Invalid channel %d min value : value cut to %.2f [V]' % (ch+1, val))
            #raise mdsExceptions.DevBAD_PARAMETER

        try:
            chType = getattr(self, 'ao_%d_type' % (ch+1)).data()
            self.waveTypeDict[chType]
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid channel %d wave type : %s' % (ch+1, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chFreq = getattr(self, 'ao_%d_freq' % (ch+1)).data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Invalid channel %d waveform frequency : %s' % (ch+1, str(ex)))
            raise mdsExceptions.DevBAD_FREQ
        if chFreq < (self.MIN_FREQ - 1e-6) or chFreq > (self.MAX_FREQ + 1e-4):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid channel %d waveform frequency %f out of limit (%0.2f:%0.2f)' % (
                ch+1, chFreq, self.MIN_FREQ, self.MAX_FREQ))
            raise mdsExceptions.DevBAD_FREQ

        if chType == "AS_IS":
            try:
                chAOX = getattr(self, 'ao_%d_x' % (ch+1)).data()
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Invalid channel %d wave aoY : %s' % (ch+1, str(ex)))
                raise mdsExceptions.DevBAD_PARAMETER

            try:
                chAOY = getattr(self, 'ao_%d_y' % (ch+1)).data()
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Invalid channel %d wave aoX : %s' % (ch+1, str(ex)))
                raise mdsExceptions.DevBAD_PARAMETER

        self.chState = isOn
        self.chMax = chMax
        self.chMin = chMin
        self.chType = chType
        self.chAOX = chAOX
        self.chFreq = chFreq
        self.chAOY = chAOY
        self.chGain = chGain
        self.chOffset = chOffset

    def configChannels(self, reset):

        #wavePoint = self.wave_point.data()

        try:
            gain = self.gain.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid gain value : %s' % (str(ex)))
            raise mdsExceptions.DevBAD_GAIN

        periodPoint = []
        for ch in range(4):
           # if getattr(self, 'ao_%d'%(ch+1)).isOn():
            fr = getattr(self, 'ao_%d_freq' % (ch+1)).data()
            periodPoint.append(self.DAC_MAX_FREQ / fr)
           # else :
           #    periodPoint.append( 0 )

        wavePoint = (np.asarray(periodPoint) + 0.5).astype(int)
        print ('Wave point ', wavePoint)

        for ch in range(4):
            self.__checkChanneParam__(ch, gain)
            self.configChannel(ch, reset, wavePoint[ch], self.chMax, self.chMin,
                               self.chType, self.chAOX, self.chAOY, 1, self.chGain, self.chOffset)

    def init(self):

        print ('================= BCM2835_DAC Generation Init ===============')

        if self.restoreInfo(True) == self.ERROR:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open device')
            raise mdsExceptions.DevINV_SETUP
        """
        try:
            wavePoint = self.wave_point.data();
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid waveform points number')
            raise mdsExceptions.DevBAD_PARAMETER
        """

        self.configChannels(False)

        self.saveInfo()

        return self.NO_ERROR


# Start generation
    def __startGeneration__(self, isUpdate):

        if self.restoreInfo(False) != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not initialized')
            raise mdsExceptions.DevINV_SETUP

        if not isUpdate:
            try:
                trigMode = self.triggerModeDict[self.trig_mode.data()]
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid trig mode value : %s' % (str(ex)))
                raise mdsExceptions.DevINV_SETUP
        else:
            # If restart generation from waveform param update, trigger mode must be internal
            trigMode = triggerModeDict['INTERNAL']

        # start AO segment (signal generation)
        if BCM2835_DAC.bcm2835InterfaceLib.bcm2835_start_ao(c_int(self.fd), trigMode) != 0:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to start AO!")
            raise mdsExceptions.DevIO_STUCK

        return self.NO_ERROR

    def start_gen(self):
        print ("================ BCM2835_DAC  Start Generation =============")

        self.__startGeneration__(isUpdate=False)

        try:
            cmdEventName = self.cmd_event.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid update event : '+str(ex))
            raise mdsExceptions.DevBAD_PARAMETER

        self.updEvent = self.AsynchUpdateGen(cmdEventName)
        self.updEvent.configure(self)

        self.saveUpdateEvent()

        return self.NO_ERROR

# Stop generation
    # private method
    def __stopGeneration__(self):

        if self.restoreInfo(False) != self.DEV_IS_OPEN:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not initialized')
            raise mdsExceptions.DevINV_SETUP

        # stop AO segment
        if BCM2835_DAC.bcm2835InterfaceLib.bcm2835_stop_ao(c_int(self.fd)) != 0:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to stop AO!")
            raise mdsExceptions.DevIO_STUCK

    def stop_gen(self):
        print ("================ BCM2835_DAC  Stop Geneation =============")

        self.__stopGeneration__()

        """
        if self.restoreInfo(False) != self.DEV_IS_OPEN :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Waveform generation not started')
            raise mdsExceptions.DevINV_SETUP
        """

        self.closeInfo()

        if self.restoreUpdateEvent() == BCM2835_DAC.HANDLE_FOUND:
            self.updEvent.cancel()
        else:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'BCM2835 DAC start_gen method not executed')
            raise mdsExceptions.DevINV_SETUP

        return self.NO_ERROR

    class waitForAnswer(Event):
        def run(self):
            print("Event %s occurred at %s with data: %s " % (
                str(self.event), str(self.qtime.date), str(self.raw.deserialize())))
            self.cancel()

    def __cmdEvent__(self, cmd):
        try:
            cmdEventName = self.cmd_event.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid update event')
            return self.ERROR

        Event.setevent(cmdEventName, cmd)

        rep = self.waitForAnswer(cmdEventName+'_REPLY')
        timeout = 5
        rep.join(timeout)
        time.sleep(.1)
        if rep.isAlive():
            rep.cancel()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Time out, command reply not received within the last %f [sec]' % (timeout))
            return self.ERROR

        return self.NO_ERROR

    def pause_gen(self):

        if self.__cmdEvent__({"command": 'pause', "data": [1, 2, 3, 4]}) == self.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return self.NO_ERROR

    def resume_gen(self):
        if self.__cmdEvent__({"command": 'resume'}) == self.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return self.NO_ERROR

    def update_gen(self):

        chStateAr = []
        chMaxAr = []
        chMinAr = []
        chTypeAr = []
        chAOXAr = []
        chAOYAr = []
        chFreqAr = []

        gain = self.gain.data()
        for ch in range(4):
            self.__checkChanneParam__(ch, gain)

            chStateAr.append(self.chState)
            chMaxAr.append(self.chMax)
            chMinAr.append(self.chMin)
            chFreqAr.append(self.chFreq)
            chTypeAr.append(self.chType.strip())
            chAOXAr.append(self.chAOX)
            chAOYAr.append(self.chAOY)

        cmd = {"command": 'update'}
        cmd['chState'] = np.asarray(chStateAr)
        cmd['chMax'] = np.asarray(chMaxAr)
        cmd['chMin'] = np.asarray(chMinAr)
        cmd['chType'] = np.asarray(chTypeAr)
        cmd['chAOX'] = np.asarray(chAOXAr)
        cmd['chAOY'] = np.asarray(chAOYAr)
        cmd['chFreq'] = np.asarray(chFreqAr)

        print (cmd)

        if self.__cmdEvent__(cmd) == self.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return self.NO_ERROR
