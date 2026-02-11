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
import numpy as np
import MDSplus
from MDSplus import mdsExceptions
from ctypes import CDLL, Structure, c_int, c_uint, c_char, c_byte, c_ubyte
from ctypes import c_float, byref, c_char_p, c_void_p, c_short, c_ulonglong
import os

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('NI6368::NI6368ADC', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, 1./(build_path(".parameters:par_1:value")*build_path(".outputs.adc0_0:samples")))')
class MARTE2_NI6368_ADC(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'uint64', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC0_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'InputRange', 'type': 'float32', 'value': 10},
            {'name': 'ChannelId', 'type': 'int', 'value': 0},
        ]},
        {'name': 'ADC1_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 1},
                                                                            ]},
        {'name': 'ADC2_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 2},
                                                                            ]},
        {'name': 'ADC3_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 3},
                                                                            ]},
        {'name': 'ADC4_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 4},
                                                                            ]},
        {'name': 'ADC5_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 5},
                                                                            ]},
        {'name': 'ADC6_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 6},
                                                                            ]},
        {'name': 'ADC7_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 7},
                                                                            ]},
        {'name': 'ADC8_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 8},
                                                                            ]},
        {'name': 'ADC9_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 9},
                                                                            ]},
        {'name': 'ADC10_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 10},
                                                                            ]},
        {'name': 'ADC11_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 11},
                                                                            ]},
        {'name': 'ADC12_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 12},
                                                                            ]},
        {'name': 'ADC13_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 13},
                                                                            ]},
        {'name': 'ADC14_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 14},
                                                                            ]},
        {'name': 'ADC15_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 15},
                                                                            ]},
    ]
    parameters = [
        {'name': 'DeviceName', 'type': 'string', 'value': '/dev/pxie-6368'},
        {'name': 'BoardId', 'type': 'int32', 'value': 0},
        {'name': 'DMABufferSize', 'type': 'int32', 'value': 100},
        {'name': 'ExecutionMode', 'type': 'string', 'value': 'RealTimeThread'},
        {'name': 'ClockSampleSource', 'type': 'string', 'value': 'INTERNALTIMING'},
        {'name': 'ClockSamplePolarity', 'type': 'string',
            'value': 'ACTIVE_HIGH_OR_RISING_EDGE'},
        {'name': 'ClockConvertSource', 'type': 'string', 'value': 'INTERNALTIMING'},
        {'name': 'ClockConvertPolarity', 'type': 'string', 'value': 'ACTIVE_HIGH_OR_RISING_EDGE'},
        {'name': 'ScanIntervalCounterSource',
            'type': 'string', 'value': 'COUNTER_TB3'},
        {'name': 'ScanIntervalCounterPolarity',
            'type': 'string', 'value': 'RISING_EDGE'},
        {'name': 'ScanIntervalCounterPeriod', 'type': 'int32', 'value': 50},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xf},
        {'name': 'SamplingFrequency', 'type': 'int32', 'value':10000},
        {'name': 'AcquisitionMode', 'type': 'string', 'value':'CONTINUOUS'},
        {'name': 'PostTriggerSamples', 'type': 'int32', 'value':1000},
    ]
    parts = []

    def prepareMarteInfo(self):
######## Enabled channels, segment len and samples
# set samples equal to SEG_LEN / SEG_BLOCKS and the same value of samples and segment lengthin all output nodes except time
        try:
            segmentLen = self.getNode('SEGMENT_LEN').data()
        except:
            raise Exception('Missing or invalid segment length for '+self.getPath())
        try:
            bufferLen = int(self.getNode('BUFFER_LEN').data())
        except:
            raise Exception('Missing or buffer segment length for '+self.getPath())
        if (segmentLen % bufferLen) != 0:
            raise Exception('Segment Len must be a multiple of buffer len')
        blocksInSegment = segmentLen / bufferLen
        if blocksInSegment > 1:
            self.getNode('OUTPUTS:SEG_BLOCKS').putData(MDSplus.Int32(blocksInSegment))
        else:
            self.getNode('OUTPUTS:SEG_BLOCKS').deleteData()
        samples = segmentLen/blocksInSegment
        for i in range(16):
            self.getNode('OUTPUTS:ADC%d_0.SAMPLES'%i).putData(MDSplus.Int32(samples))
        for i in range(16):
            try:
                if self.getNode('CHANNELS.CHANNEL_%d:ENABLED' % (i+1)).data() == 'ENABLED':
                    self.getNode('OUTPUTS.ADC%d_0:SEG_LEN'%(i)).putData(MDSplus.Int32(blocksInSegment))
                else:
                    self.getNode('OUTPUTS.ADC%d_0:SEG_LEN'%(i)).putData(MDSplus.Int32(0))
                    self.getNode('OUTPUTS.ADC%d_0:DIMENSIONS'%(i)).putData(MDSplus.Int32(-1))
            except:
                raise Exception('Missing or invalid enable specification for channel %d of '%(i+1) + self.getPath())
#force segmentUpdate for time in step with channel segment update
        timeSegLen = blocksInSegment
        if timeSegLen < 10:
            timeSegLen = 10
        self.getNode('OUTPUTS.TIME:SEG_LEN').putData(MDSplus.Int32(timeSegLen))

############### ADC Setting
        try:
            clockMode = self.getNode(':CLOCK_MODE').data().upper()
        except:
            raise Exception('Cannot read clock mode for '+self.getPath())
        if clockMode == 'INTERNAL':
            try:
                frequency = self.getNode('FREQUENCY').data()
            except:
                raise Exception('Invalid frequency in internal clock  on '+self.getPath())

            self.getNode('.PARAMETERS.PAR_5:VALUE').putData(MDSplus.String('INTERNALTIMING'))
            self.getNode('.PARAMETERS.PAR_9:VALUE').putData(MDSplus.String('COUNTER_TB3'))
            self.getNode('.PARAMETERS.PAR_10:VALUE').putData(MDSplus.String('RISING_EDGE'))
        elif clockMode == 'EXTERNAL':
            self.getNode('.PARAMETERS.PAR_5:VALUE').putData(MDSplus.String('PFI0'))
            self.getNode('.PARAMETERS.PAR_6:VALUE').putData(MDSplus.String('ACTIVE_HIGH_OR_RISING_EDGE'))
            self.getNode('.PARAMETERS.PAR_11:VALUE').putData(MDSplus.Int32(100))
            try:
                clockSource = self.getNode('CLOCK_SOURCE').evaluate()
            except:
                raise Exception('Cannot read clock source for '+self.getPath())
            if not isinstance(clockSource, MDSplus.Range):
                raise Exception('Invalid clock source for '+self.getPath()+' Must be a Range descriptor')
            begins = clockSource.getBegin()
            if not begins is None:
                try:
                    begins = begins.data()
                except:
                    raise Exception('Cannot get begin time(s) for clock source in '+self.getPath())
            endings = clockSource.getEnding()
            if not endings is None:
                try:
                    ending = endings.data()
                except:
                    raise Exception('Cannot get end time(s) for clock source in '+self.getPath())
            try:
                deltas = clockSource.getDelta().data()
            except:
                raise Exception('Cannot get delta time(s) for clock source in '+self.getPath())

        else:
            raise Exception('Invalid clock mode '+clockMode+' for '+self.getPath())
        try:
            acquisitionMode = self.getNode('ACQ_MODE').data().upper()
        except:
            raise Exception('Cannot read acquisition mode for '+self.getPath())
        if acquisitionMode == 'CONTINUOUS':
            if clockMode == 'EXTERNAL':
                if begins is None:
                    try:
                        triggerTime = self.getNode('TRIG_TIME').data()
                    except:
                        triggerTime = 0
                    if not np.isscalar(triggerTime):
                         raise Exception('When clock mode is EXTERNAL, TRIGGER_TIME shall be either undefined or scalar '+self.getPath())
                elif np.isscalar(begins):       #Single gated clock
                    triggerTime = begins
                else:                           #Multiple gated clocks
                    if endings is None or len(endings) != len(begins):
                        raise Exception('Number of begin times different from number of end times for clock source in '+self.getPath())
                    currTrigs = []
                    currTrigSamples = []
                    for currPulse in range(len(begins)):
                        currPulseSamples = (endings[currPulse] - begins[currPulse])/deltas[currPulse]
                        if currPulseSamples % segmentLen != 0:
                            raise Exception('The number of pulse samples must be a multiple of Segment len  in multi gated external clock in '+self.getPath())
                        currTrigs.append(begins[currPulse])
                        currTrigSamples.append(currPulseSamples)
 
                    triggerTime = np.array(currTrigs, dtype=float) 
                    samplesPerTrigger = np.array(currTrigSamples, dtype=float) 
                if np.isscalar(deltas):
                    period = deltas
                else:
                    for delta in deltas:
                        if delta != deltas[0]:
                            raise Exception ('In multiple clock runs mode the external clock speed must be the same for '+self.getPath())
                    period = deltas[0]
                frequency = 1/period
            else: #clock mode INTERNAL
                try:
                    triggerTime = self.getNode('TRIG_TIME').data()
                except:
                    triggerTime = 0
        elif acquisitionMode == 'TRIGGERED':
            if clockMode != 'INTERNAL':
                if not np.isscalar(deltas):
                    raise Exception('In TRIGGERED acquisition mode the external clock must be single speed for '+self.getPath())
                period = deltas
                frequency = 1/period
            try:
                triggerTime = self.getNode('TRIG_TIME').data()
            except:
                triggerTime = 0
            try:
                postTriggerSamples = self.getNode('POST_TRIG').data()
            except:
                raise  Exception('Cannot get the number of post trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_15:VALUE').putData(MDSplus.Int32(postTriggerSamples))
        elif acquisitionMode == 'MULTI_TRIGGERED':
            if clockMode != 'INTERNAL':
                if not np.isscalar(deltas):
                    raise Exception('In MULTI_TRIGGERED acquisition mode the external clock must be single speed for '+self.getPath())
                period = deltas
                frequency = 1/period
            try:
                pulseTriggerTime = self.getNode('TRIG_TIME').data()
            except:
                pulseTriggerTime = 0
            try:
                postTriggerSamples = self.getNode('POST_TRIG').data()
            except:
                raise  Exception('Cannot get the number of post trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_15:VALUE').putData(MDSplus.Int32(postTriggerSamples))

            if postTriggerSamples % segmentLen != 0:
                raise  Exception('The number of post trigger samples must be a multiple of segment len for '+self.getPath())
            if not np.isscalar(pulseTriggerTime):
                currTrigs = []
                currTrigSamples = []
                for currPulse in range(len(pulseTriggerTime)):
                    currTrigs.append(pulseTriggerTime[currPulse])
                    currTrigSamples.append(postTriggerSamples)
                triggerTime = np.array(currTrigs, dtype=float) 
                samplesPerTrigger = np.array(currTrigSamples, dtype=float) 
            else:
                triggerTime = pulseTriggerTime
        else:
            raise  Exception('Invalid Acquisition Mode for '+self.getPath())
        self.getNode('.PARAMETERS.PAR_14:VALUE').putData(MDSplus.String(acquisitionMode))
        self.getNode('.PARAMETERS.PAR_13:VALUE').putData(MDSplus.Int32(int(frequency)))

#At this point triggerTime contains the (array of) trigger time(s)
        if acquisitionMode == 'TRIGGERED' or np.isscalar(triggerTime):
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDSplus.Float64(triggerTime))
        else:
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDSplus.Float64Array(triggerTime))
            self.getNode('OUTPUTS:TRIGGER_SAMP').putData(MDSplus.Int32Array(samplesPerTrigger))

####### Data expressions
        t = self.getTree()
        for i in range(16):
            dataExpr = '_c = data('+self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1)).getFullPath()+');'
            dataExpr += '_s = data('+self.getNode('OUTPUTS.ADC%d_0:VALUE'%(i)).getFullPath()+');'
            dataExpr += 'Build_With_Units(Build_signal((_c[0] + _c[1] * _s + _c[2] * _s * _s + _c[3] * _s * _s * _s),,dim_of(_s)), "Volts")'
            print(dataExpr)
            self.getNode('CHANNELS.CHANNEL_%d:DATA' % (i+1)).putData(t.tdiCompile(dataExpr))            
#            dataExpr = 'Build_With_Units(NIanalogInputScaled('+self.getNode('OUTPUTS.ADC%d_0:VALUE'%(i)).getFullPath()+','+ self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1)).getFullPath()+'), "Volts")'
           
######## Timebase expression
        print('(0 : * : ('+ self.getNode('OUTPUTS.ADC0_0:SAMPLES').getFullPath()+
        ' / '+self.getNode('PARAMETERS.PAR_13:VALUE').getFullPath()+'))')
        self.getNode('TIMEBASE').putData(t.tdiCompile('(0 : * : ('+ self.getNode('OUTPUTS.ADC0_0:SAMPLES').getFullPath()+
        ' /  float('+self.getNode('PARAMETERS.PAR_13:VALUE').getFullPath()+')))'))
            
    @classmethod
    def postBuild(cls):
        cls.parts.append({'path': 'ACQ_MODE', 'type': 'text', 'value': "CONTINUOUS"})
        cls.parts.append({'path': 'POST_TRIG', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'SEGMENT_LEN', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'BUFFER_LEN', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'FREQUENCY', 'type': 'numeric'})
        cls.parts.append({'path': 'CLOCK_MODE', 'type': 'text', 'value':'INTERNAL'})
        cls.parts.append({'path': 'CLOCK_SOURCE', 'type': 'numeric'})
        cls.parts.append({'path': 'TRIG_TIME', 'type': 'numeric'})
        cls.parts.append({'path': '.CHANNELS', 'type': 'structure'})
        for i in range(16):
            cls.parts.extend([
                {'path': '.CHANNELS.CHANNEL_%d' % (i+1), 'type': 'structure'},
                {'path': '.CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1), 'type': 'numeric'},
                {'path': '.CHANNELS.CHANNEL_%d:ENABLED' % (i+1), 'type': 'text', 'value':'ENABLED'},
                {'path': '.CHANNELS.CHANNEL_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'compress_on_put')},
            ])
        cls.parts.append({'path': 'SERIAL_NUM', 'type': 'numeric'})

    class XSERIES_DEV_INFO(Structure):
        _fields_ = [("product_id", c_uint),
                    ("device_name", c_char * 20),
                    ("is_simultaneous", c_ubyte),
                    ("is_pcie", c_ubyte),
                    ("adc_number", c_uint),
                    ("dac_number", c_uint),
                    ("port0_length", c_uint),
                    ("max_ai_channels", c_uint),
                    ("serial_number", c_uint),
                    ("aichan_size", c_uint),
                    ("aochan_size", c_uint),
                    ("ext_cal_time", c_float),
                    ("ext_cal_temp", c_ulonglong),
                    ("self_cal_time", c_float),
                    ("self_cal_temp", c_ulonglong),
                    ("geographical_addr", c_uint)]
    def init(self):
        XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL = c_int(1)
        try:
            niInterfaceLib = CDLL("libNiInterface.so")
            niLib = CDLL("libnixseries.so")
        except:
            print('Cannot link to NIInterface for '+self.getPath()) 
            raise  Exception(MDSplus.mdsExceptions.TclFAILED_ESSENTIAL) 

        boardId = self.getNode('PARAMETERS.PAR_2:VALUE').data()
        gainDict = {10.: c_int(0), 5.: c_int(1), 2.: c_int(2), 1.: c_int(3), 0.5: c_int(4), 0.2: c_int(5), 0.1: c_int(6)}

        fileName = '/dev/pxie-6368.'+str(boardId)+'.ai'
        try:
            fd = os.open(fileName, os.O_RDWR)
        except:
            print('Callot open device')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        status = niLib.xseries_reset_ai(c_int(fd))
        if status != 0:
            print('Cannot reset ai')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        aiConf = c_void_p(0)
        niInterfaceLib.xseries_create_ai_conf_ptr(byref(aiConf), c_int(0), c_int(1000), 0)

        for chan in range(16):
            currGain = self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_1:VALUE'%(chan)).data()
            currGainCode = gainDict[currGain]
            status = niLib.xseries_add_ai_channel(aiConf, c_short(chan), currGainCode, XSERIES_AI_CHANNEL_TYPE_DIFFERENTIAL, c_byte(1))
            if status != 0:
                print('Cannot add ai conf')
                raise mdsExceptions.TclFAILED_ESSENTIAL

        status = niInterfaceLib.xseries_set_ai_conf_ptr(c_int(fd), aiConf)
        if status != 0:
            print('Cannot load ai conf')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        for chan in range(16):
            try:
                currFd = os.open('/dev/pxie-6368.'+str(boardId)+'.ai.'+str(chan), os.O_RDWR | os.O_NONBLOCK)
                print('APERTO CHAN ', chan, currFd)
                currGain = self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_1:VALUE'%(chan)).data()
                currGainCode = gainDict[currGain]
                coeffArray = c_float*4
                coeff = coeffArray()
                status = niInterfaceLib.getCalibrationParams(c_int(currFd), currGainCode, coeff)
                if(status < 0):
                    print('Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/32768)' % (chan))
                    coeff[0] = coeff[2] = coeff[3] = 0
                    coeff[1] = c_float(gain / 32768.)
                self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM'%(chan + 1)).putData(MDSplus.Float32Array(coeff))
                os.close(currFd)
            except:
                print('Cannot get calibration parameters for '+self.getPath()) 
                raise  Exception(MDSplus.mdsExceptions.TclFAILED_ESSENTIAL) 
        deviceInfo = self.XSERIES_DEV_INFO(0, "".encode('utf-8'), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

        os.close(fd)
        # get card info
        fileName = '/dev/pxie-6368.'+str(boardId)
        devFd = os.open(fileName, os.O_RDWR)
        status = niInterfaceLib._xseries_get_device_info(c_int(devFd), byref(deviceInfo))
        os.close(devFd)
        if status < 0:
            print('Cannot get device info for '+self.getPath()) 
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.getNode('SERIAL_NUM').putData(MDSplus.Int32(deviceInfo.serial_number))

