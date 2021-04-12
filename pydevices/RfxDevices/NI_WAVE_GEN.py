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


class NI_WAVE_GEN(Device):
    """NI PXI-6259 / 6368 Analog Output Waveform Generation"""
    parts = [{'path': ':COMMENT',      'type': 'text'},
             {'path': ':BOARD_ID',  'type': 'numeric', 'value': 0},
             {'path': ':BOARD_TYPE',   'type': 'text', 'value': 'NI6259'},
             {'path': ':WAVE_POINT',   'type': 'numeric', 'value': 1000},
             {'path': ':TRIG_MODE',    'type': 'text', 'value': 'EXTERNAL'},
             {'path': ':TRIG_SOURCE',  'type': 'numeric', 'value': 0},
             {'path': ':SERIAL_NUM',   'type': 'numeric'}]

    for i in range(0, 4):
        parts.append({'path': '.AO_%d' % (i+1),      'type': 'structure'})
        parts.append({'path': '.AO_%d:MAX' % (i+1),
                      'type': 'numeric', 'value': 1.})
        parts.append({'path': '.AO_%d:MIN' % (i+1),
                      'type': 'numeric', 'value': 0.})
        parts.append({'path': '.AO_%d:X' % (i+1),    'type': 'numeric'})
        parts.append({'path': '.AO_%d:Y' % (i+1),    'type': 'numeric'})
        parts.append({'path': '.AO_%d:TYPE' % (i+1),
                      'type': 'text', 'value': 'SIN'})
        parts.append({'path': '.AO_%d:FREQ' % (i+1),
                      'type': 'numeric', 'value': 1.})
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

    parts.append({'path': ':GAIN',         'type': 'numeric', 'value': 1})
    parts.append({'path': ':CMD_EVENT',    'type': 'text',
                  'value': 'WAVE_UPDATE'})

    AO_UPDATE_SOURCE_SELECT_UI_TC = c_int(0)
    AO_UPDATE_SOURCE_POLARITY_RISING_EDGE = c_int(0)

    AO_DAC_POLARITY_UNIPOLAR = c_int(0)
    AO_DAC_POLARITY_BIPOLAR = c_int(1)

    # File descriptor
    #ao_fd = 0
    #boardId = 0
    #sys.tracebacklimit = 0

    waveAOFds = {}
    waveAOChFds = {}
    updateEvents = {}
    niLib6259 = None
    niLib6368 = None
    niInterfaceLib = None

    MIN_FREQ = 0.01
    MAX_FREQ = 1000.


#Dictionaries and maps
    boardTypeDict = {'NI6259': '/dev/pxi6259.', 'NI6368': '/dev/pxie-6368.'}
    waveTypeDict = {'AS_IS': 1, 'SIN': 2, 'COS': 3,
                    'TRIANGULAR': 4, 'SQUARE': 5, 'SAWTOOTH': 6}

    HANDLE_FOUND = 2
    HANDLE_NOT_FOUND = 3
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

    def saveInfo(self):
        NI_WAVE_GEN.waveAOFds[self.getNid()] = self.ao_fd

    def restoreInfo(self, openFlag):
        if NI_WAVE_GEN.niLib6259 is None:
            NI_WAVE_GEN.niLib6259 = CDLL("libpxi6259.so")
        if NI_WAVE_GEN.niInterfaceLib is None:
            NI_WAVE_GEN.niInterfaceLib = CDLL("libNiInterface.so")

        try:
            self.ao_fd = NI_WAVE_GEN.waveAOFds[self.getNid()]
            print('RESTORE INFO HANDLE FOUND')
            return NI_WAVE_GEN.HANDLE_FOUND
        except:
            if openFlag:
                print('RESTORE INFO HANDLE NOT FOUND')
                try:
                    boardId = self.board_id.data()
                except:
                    Data.execute('DevLogErr($1,$2)',
                                 self.getNid(), 'Missing Board Id')
                    return NI_WAVE_GEN.ERROR

                try:
                    boardType = self.board_type.data()
                except:
                    Data.execute('DevLogErr($1,$2)',
                                 self.getNid(), 'Missing Board type')
                    return NI_WAVE_GEN.ERROR

                try:
                    fileName = NI_WAVE_GEN.boardTypeDict[boardType] + str(
                        boardId)+'.ao'
                    self.ao_fd = os.open(fileName, os.O_RDWR | os.O_NONBLOCK)
                    print('Open fileName : ', fileName)
                except IOError(error, message):
                    print (error, repr(message))
                    print (errno.errorcode[error])
                except Exception as exc:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Cannot open device ' + fileName + " " + str(exc))
                    return NI_WAVE_GEN.ERROR
                return NI_WAVE_GEN.NO_ERROR
            return NI_WAVE_GEN.HANDLE_NOT_FOUND

    def closeInfo(self):
        try:
            os.close(NI_WAVE_GEN.waveAOFds[self.getNid()])
            del(NI_WAVE_GEN.waveAOFds[self.getNid()])
            self.ao_fd = -1
        except:
            pass
            #print('CLOSE INFO: HANDLE NOT FOUND')
        return 1

# Update Event Management

    def saveUpdateEvent(self):
        NI_WAVE_GEN.updateEvents[self.getNid()] = self.updEvent

    def restoreUpdateEvent(self):
        try:
            self.updEvent = NI_WAVE_GEN.updateEvents[self.nid]
            return NI_WAVE_GEN.HANDLE_FOUND
        except:
            print('Cannot restore update event!!')
        return NI_WAVE_GEN.HANDLE_NOT_FOUND

    def closeUpdateEvent(self):
        try:
            self.updEvent = NI_WAVE_GEN.updateEvents[self.nid]
            self.updEvent.cancel()
            del NI_WAVE_GEN.updateEvents[self.nid]
            return NI_WAVE_GEN.NO_ERROR
        except:
            pass
        return NI_WAVE_GEN.ERROR


# AsynchUpdateGen class
    class AsynchUpdateGen(Event):

        ACQ_NOERROR = 0
        ACQ_ERROR = 1
        device = None

        def configure(self, device):
            self.device = device
            return

        def run(self):
            # EVENT data as json object
            #   { "command": 'pause'|'update'|'resume' , "type" : , "Max" : , "Min" : , "Freq" :}
            #
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
                    self.device.__startGeneration__()
                except Exception as ex:
                    Event.setevent(cmdEventName+'_REPLY',
                                   'Cannot resume waveform generation : '+str(ex))
            elif jcmd["command"] == 'update':
                boardId = self.device.board_id.data()
                boardType = self.device.board_type.data()
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

                sampleRate = wavePoint * minFreq
                periodDivisor = int(20000000 / sampleRate)
                print ('==update periodDivisor ', periodDivisor)
                print ('==update Num Period ', chFreqAr, numPeriod, minFreq)

                try:
                    self.device.__stopGeneration__()
                    self.device.closeInfo()
                    if self.device.restoreInfo(True) != NI_WAVE_GEN.NO_ERROR:
                        Data.execute('DevLogErr($1,$2)',
                                     self.device.getNid(), 'Cannot open device')
                        raise mdsExceptions.DevINV_SETUP
                    self.device.saveInfo()

                    aoConfig = c_void_p(0)
                    NI_WAVE_GEN.niInterfaceLib.pxi6259_create_ao_conf_ptr(
                        byref(aoConfig))

                    #  initialize AO configuration
                    self.device.__aoConfiguration__(
                        aoConfig, self.device.AO_DAC_POLARITY_BIPOLAR, wavePoint, periodDivisor)

                    for ch in range(4):
                        self.device.configChannel(boardType, boardId, ch, False, wavePoint, chMaxAr[ch], chMinAr[ch], chTypeAr[ch].strip(
                        ), chAOXAr[ch], chAOXAr[ch], numPeriod[ch])

                    self.device.__startGeneration__()

                except Exception as ex:
                    Event.setevent(
                        cmdEventName+'_REPLY', 'Cannot update waveforms generation : '+str(ex))

            Event.setevent(cmdEventName+'_REPLY', msg)
            return

        def stop(self):
            self.stopReq = True
            NI6259AI.niInterfaceLib.setStopAcqFlag(self.stopAcq)

        def hasError(self):
            return (self.error != self.ACQ_NOERROR)

        def closeTree(self):
            # On first test dosen't work
            NI6259AI.niInterfaceLib.closeTree(self.treePtr)
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
            sigData = offset + level * np.interp(xi, aoX, aoY, wavePoint)
        else:
            return None

        scaledWriteArray = (c_float * len(sigData))(*sigData)

        return scaledWriteArray

    def configChannel(self, boardType, boardId, ch, reset, wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod):

        isOn = getattr(self, 'ao_%d' % (ch+1)).isOn()

        # open file descriptor for each AO channel
        aoChFd = os.open(NI_WAVE_GEN.boardTypeDict[boardType] + str(
            boardId) + '.ao.' + str(ch), os.O_RDWR | os.O_NONBLOCK)
        if aoChFd < 0:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to open channel: " + os.strerror(errno))
            raise mdsExceptions.DevIO_STUCK

        if not reset and isOn:
            print ("configChannel ", boardType, boardId, ch, reset,
                   wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod)
            scaledWriteArray = NI_WAVE_GEN.buildWave(
                self, wavePoint, chMax, chMin, chType, aoX, aoY, nPeriod)
        else:
            zero_arr = np.zeros(wavePoint)
            scaledWriteArray = (c_float * len(zero_arr))(*zero_arr)

        if scaledWriteArray == None:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), "Invalid waveform type -%s- for channel %d " % (chType, ch))
            zero_arr = np.zeros(wavePoint)
            scaledWriteArray = (c_float * len(zero_arr))(*zero_arr)

        retval = NI_WAVE_GEN.niLib6259.pxi6259_write_ao(
            aoChFd, scaledWriteArray, c_uint(wavePoint))
        if retval != wavePoint:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), "Failed while writing! Written samples %d : %s (%d)" % (retval, os.strerror(errno), errno))
            raise mdsExceptions.DevIO_STUCK

        os.close(aoChFd)

    def __checkChanneParam__(self, ch, gain):

        chAOX = []
        chAOY = []

        isOn = getattr(self, 'ao_%d' % (ch+1)).isOn()

        try:
            chMax = getattr(self, 'ao_%d_max' % (ch+1)).data()/gain
            if chMax < -10. or chMax > 10.:
                raise mdsExceptions.DevBAD_PARAMETER
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid channel %d max value : %s' % (ch, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chMin = getattr(self, 'ao_%d_min' % (ch+1)).data()/gain
            if chMin < -10. or chMin > 10.:
                raise mdsExceptions.DevBAD_PARAMETER
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid channel %d min value : %s' % (ch, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chType = getattr(self, 'ao_%d_type' % (ch+1)).data()
            self.waveTypeDict[chType]
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid channel %d wave type : %s' % (ch, str(ex)))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            chFreq = getattr(self, 'ao_%d_freq' % (ch+1)).data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Invalid channel %d waveform frequency : %s' % (ch, str(ex)))
            raise mdsExceptions.DevBAD_FREQ
        if chFreq < (self.MIN_FREQ - 1e-6) or chFreq > (self.MAX_FREQ + 1e-4):
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Invalid channel %d waveform frequency %f out of limit (%0.2f:%0.2f)' % (
                ch, chFreq, self.MIN_FREQ, self.MAX_FREQ))
            raise mdsExceptions.DevBAD_FREQ

        if chType == "AS_IS":
            try:
                chAOX = getattr(self, 'ao_%d_x' % (ch+1)).data()
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid channel %d wave aoY : %s' % (ch, str(ex)))
                raise mdsExceptions.DevBAD_PARAMETER

            try:
                chAOY = getattr(self, 'ao_%d_y' % (ch+1)).data()
            except Exception as ex:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Invalid channel %d wave aoX : %s' % (ch, str(ex)))
                raise mdsExceptions.DevBAD_PARAMETER

        self.chState = isOn
        self.chMax = chMax
        self.chMin = chMin
        self.chType = chType
        self.chAOX = chAOX
        self.chFreq = chFreq
        self.chAOY = chAOY

    def configChannels(self, reset):

        boardId = self.board_id.data()
        boardType = self.board_type.data()
        wavePoint = self.wave_point.data()

        try:
            gain = self.gain.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid gain value : %s' % (str(ex)))
            raise mdsExceptions.DevBAD_GAIN

        freq = []
        minFreq = 1e6
        for ch in range(4):
            fr = getattr(self, 'ao_%d_freq' % (ch+1)).data()
            freq.append(fr)
            if getattr(self, 'ao_%d' % (ch+1)).isOn():
                if fr < minFreq:
                    minFreq = fr

        numPeriod = (np.asarray(freq) / minFreq + 0.5).astype(int)
        print ('Num Period ', freq, numPeriod, minFreq)

        for ch in range(4):
            self.__checkChanneParam__(ch, gain)

            self.configChannel(boardType, boardId, ch, reset, wavePoint, self.chMax,
                               self.chMin, self.chType, self.chAOX, self.chAOY, numPeriod[ch])

    def __aoConfiguration__(self, aoConfig, polarity, wavePoint, periodDivisor):

        # enable waveform generation (Retrasmit mode)
        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_waveform_generation(aoConfig, c_ubyte(1)):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to set AO waveform generation!")
            raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        try:
            wavePoint = self.wave_point.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid waveform points number')
            raise mdsExceptions.DevBAD_PARAMETER

        for ch in range(4):
            # configure AO channel
            if NI_WAVE_GEN.niLib6259.pxi6259_add_ao_channel(aoConfig, c_uint(ch), polarity):
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             "Failed to configure channel! polarity")
                raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        # set AO count numbuffer 1 continuous 1
        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_count(aoConfig, c_uint(wavePoint), c_uint(1), c_ubyte(1)):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to configure AO count!")
            raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        if NI_WAVE_GEN.niLib6259.pxi6259_set_ao_update_clk(aoConfig, self.AO_UPDATE_SOURCE_SELECT_UI_TC, self.AO_UPDATE_SOURCE_POLARITY_RISING_EDGE, c_uint(periodDivisor)) != 0:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to configure AO update clock!")
            raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        # load AO configuration and let it apply
        print ("ao_fd ", self.ao_fd)
        if NI_WAVE_GEN.niLib6259.pxi6259_load_ao_conf(c_int(self.ao_fd), aoConfig) != 0:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Failed to load configuration! (%d) %s' % (errno, os.strerror(errno)))
            raise mdsExceptions.DevCANNOT_LOAD_SETTINGS

        time.sleep(1)

    def init(self):

        print ('================= NI Wavefor Generation Init ===============')

        if self.board_type.data() == 'NI6368':
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Device NI6368 not yet supported')
            raise mdsExceptions.DevINV_SETUP

        if self.restoreInfo(True) == NI_WAVE_GEN.ERROR:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open device')
            raise mdsExceptions.DevINV_SETUP

        # Stop the segment
        """ 
	NI_WAVE_GEN.niLib6259.xseries_stop_ao(c_int(self.ao_fd))

	retval = NI_WAVE_GEN.niLib6259.xseries_reset_ao(self.ao_fd);
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetiing device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        
	retval = NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(c_int(self.ao_fd))
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting device')
            raise mdsExceptions.TclFAILED_ESSENTIAL


	retval = NI_WAVE_GEN.niLib6259.pxi6259_reset_ao(c_int(self.ao_fd));
        if retval :
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Error resetting device')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        """

        try:
            wavePoint = self.wave_point.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid waveform points number')
            raise mdsExceptions.DevBAD_PARAMETER

        # configure AO update clock
        freq = []
        for ch in range(4):
            print (ch)
            if getattr(self, 'ao_%d' % (ch+1)).isOn():
                try:
                    fr = getattr(self, 'ao_%d_freq' % (ch+1)).data()
                    freq.append(fr)
                except Exception as ex:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Invalid channel %d waveform frequency : %s' % (ch, str(ex)))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                if fr < (self.MIN_FREQ - 1e-6) or fr > (self.MAX_FREQ + 1e-4):
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Invalid channel %d waveform frequency %f out of limit (%0.2f:%0.2f)' % (ch, fr, self.MIN_FREQ, self.MAX_FREQ))
                    raise mdsExceptions.DevBAD_FREQ

        minFreq = min(freq)
        print (freq)
        print ((np.asarray(freq)*10).astype(int) % int(minFreq*10))
        if sum((np.asarray(freq)*10).astype(int) % int(minFreq*10)):
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Minimum waveform frequency must be multiple of the other wavefoms frequency')
            raise mdsExceptions.DevBAD_FREQ

        """     
        self.numPeriod = (np.asarray(freq) / minFreq + 0.5).astype(int)
        print 'Num Period ',self.numPeriod
        """

        currWavePoint = wavePoint
        ch = 0
        for c in range(0, 4):
            if getattr(self, 'ao_%d' % (c+1)).isOn():
                fr = freq[ch]
                if freq[ch] < 1.:
                    fr = freq[ch] * 10.
                if wavePoint % freq[ch]:
                    wp = int(int(float(wavePoint) / fr) * fr + fr)
                    if wp % 2:
                        wp = wp + fr
                    if wp > currWavePoint:
                        currWavePoint = wp

                print (currWavePoint, wavePoint, freq[ch])
                """
               if ( wavePoint / freq[ch] ) < 10. :
                   Data.execute('DevLogErr($1,$2)', self.getNid(), 'Waveform %d has less than 10 points for period'%(ch))
                   raise mdsExceptions.TclFAILED_ESSENTIAL;
               """
                ch = ch + 1

        if currWavePoint != wavePoint:
            wavePoint = currWavePoint
            self.wave_point.putData(wavePoint)

        sampleRate = wavePoint * minFreq
        periodDivisor = int(20000000 / sampleRate)
        print ('periodDivisor ', periodDivisor)

        print ('OK 1')

        aoConfig = c_void_p(0)
        NI_WAVE_GEN.niInterfaceLib.pxi6259_create_ao_conf_ptr(byref(aoConfig))

        print ('OK 2')

        #  initialize AO configuration
        self.__aoConfiguration__(
            aoConfig, self.AO_DAC_POLARITY_BIPOLAR, wavePoint, periodDivisor)

        print ('OK 3')

        self.configChannels(False)

        print ('OK 4')

        NI_WAVE_GEN.niInterfaceLib.pxi6259_free_ao_conf_ptr(aoConfig)

        print ('OK 5')

        self.saveInfo()

        print ('OK 6')

        return NI_WAVE_GEN.NO_ERROR


# Start generation
    def __startGeneration__(self):

        if self.restoreInfo(False) != NI_WAVE_GEN.HANDLE_FOUND:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not initialized')
            raise mdsExceptions.DevINV_SETUP

        # start AO segment (signal generation)
        if NI_WAVE_GEN.niLib6259.pxi6259_start_ao(c_int(self.ao_fd)) != 0:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to start AO!: %s" % (os.strerror(errno)))
            raise mdsExceptions.DevIO_STUCK

        return NI_WAVE_GEN.NO_ERROR

    def start_gen(self):
        print ("================ NI Waveform Geneation Start Store =============")

        self.__startGeneration__()

        try:
            cmdEventName = self.cmd_event.data()
        except Exception as ex:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Invalid update event : '+str(ex))
            raise mdsExceptions.DevBAD_PARAMETER

        self.updEvent = self.AsynchUpdateGen(cmdEventName)
        self.updEvent.configure(self)

        self.saveUpdateEvent()

        return NI_WAVE_GEN.NO_ERROR

# Stop generation
    # private method
    def __stopGeneration__(self):

        if self.restoreInfo(False) != NI_WAVE_GEN.HANDLE_FOUND:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Module not initialized')
            raise mdsExceptions.DevINV_SETUP

        # stop AO segment
        if NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(c_int(self.ao_fd)) != 0:
            errno = NI_WAVE_GEN.niInterfaceLib.getErrno()
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to stop AO!: %s" % (os.strerror(errno)))
            raise mdsExceptions.DevIO_STUCK

        self.configChannels(True)

        # start AO segment (signal generation)
        if NI_WAVE_GEN.niLib6259.pxi6259_start_ao(self.ao_fd):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Failed to start AO!")
            raise mdsExceptions.DevIO_STUCK

        time.sleep(1)

        if NI_WAVE_GEN.niLib6259.pxi6259_stop_ao(self.ao_fd):
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         "Error stopping AO generation")
            raise mdsExceptions.DevIO_STUCK

    def stop_gen(self):
        print ("================ NI Waveform Geneation Stop Store =============")

        self.__stopGeneration__()

        if self.restoreInfo(False) != NI_WAVE_GEN.HANDLE_FOUND:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Wavefor generation not initialized')
            raise mdsExceptions.DevINV_SETUP

        self.closeInfo()

        if self.closeUpdateEvent() == NI_WAVE_GEN.ERROR:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Wavefor generation not started')
            raise mdsExceptions.DevINV_SETUP

        return NI_WAVE_GEN.NO_ERROR

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
            return NI_WAVE_GEN.ERROR

        Event.setevent(cmdEventName, cmd)

        rep = self.waitForAnswer(cmdEventName+'_REPLY')
        timeout = 5
        rep.join(timeout)
        time.sleep(.1)
        if rep.isAlive():
            rep.cancel()
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Time out, command reply not received within the last %f [sec]' % (timeout))
            return NI_WAVE_GEN.ERROR

        return NI_WAVE_GEN.NO_ERROR

    def pause_gen(self):

        if self.__cmdEvent__({"command": 'pause', "data": [1, 2, 3, 4]}) == NI_WAVE_GEN.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return NI_WAVE_GEN.NO_ERROR

    def resume_gen(self):
        if self.__cmdEvent__({"command": 'resume'}) == NI_WAVE_GEN.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return NI_WAVE_GEN.NO_ERROR

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

        if self.__cmdEvent__(cmd) == NI_WAVE_GEN.ERROR:
            raise mdsExceptions.DevCOMM_ERROR
        return NI_WAVE_GEN.NO_ERROR
