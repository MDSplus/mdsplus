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
from ctypes import CDLL, Structure, c_int, c_uint, c_char, c_byte, c_ubyte
from ctypes import c_float, byref, c_char_p, c_void_p, c_short, c_ulonglong
import os

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('NI6259::NI6259ADC', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, 1./(build_path(".parameters:par_1:value")*build_path(".outputs.adc0_0:samples")))')
class MARTE2_NI6259_ADC(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC0_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC1_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId','type': 'int', 'value': 1},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC2_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 2},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC3_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 3},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC4_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 4},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC5_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 5},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC6_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 6},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC7_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 7},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC8_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 8},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC9_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 9},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC10_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 10},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC11_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 11},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC12_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 12},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC13_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 13},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC14_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 14},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC15_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 15},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC16_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 16},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC17_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 17},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC18_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 18},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC19_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 19},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC20_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 20},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC21_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 21},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC22_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 22},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC23_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 23},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC24_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 24},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC25_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 25},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC26_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 26},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC27_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 27},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC28_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 28},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC29_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 29},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC30_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 30},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
        {'name': 'ADC31_0', 'type': 'int16', 'dimensions': 0, 'parameters': [{'name': 'InputRange', 'type': 'float32', 'value': 10},
                                                                            {'name': 'ChannelId',
                                                                                'type': 'int', 'value': 31},
                                                                            {'name': 'InputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                            {'name': 'InputMode', 'type': 'string', 'value': 'RSE'},
                                                                            ]},
    ]
    parameters = [
        {'name': 'DeviceName', 'type': 'string', 'value': '/dev/pxi6259'},
        {'name': 'BoardId', 'type': 'int32', 'value': 0},
        {'name': 'DelayDivisor', 'type': 'int32', 'value': 3},
        {'name': 'ClockSampleSource', 'type': 'string', 'value': 'SI_TC'},
        {'name': 'TriggerSource', 'type': 'string',
            'value': 'PFI1'},
        {'name': 'ClockConvertSource', 'type': 'string', 'value': 'SI2TC'},
        {'name': 'ExportTriggerTo', 'type': 'string', 'value': 'NO'},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xf},
        {'name': 'SamplingFrequency', 'type': 'int32', 'value': 1000000},
        {'name': 'AcquisitionMode', 'type': 'string', 'value':'CONTINUOUS'},
        {'name': 'PreTriggerSamples', 'type': 'int32', 'value':0},
        {'name': 'PostTriggerSamples', 'type': 'int32', 'value':1000},
    ]
    parts = []

    @classmethod
    def postBuild(cls):
        cls.parts.append({'path': 'ACQ_MODE', 'type': 'text', 'value': "CONTINUOUS"})
        cls.parts.append({'path': 'PRE_TRIG', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'POST_TRIG', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'SEGMENT_LEN', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'BUFFER_LEN', 'type': 'numeric', 'value': 1000})
        cls.parts.append({'path': 'FREQUENCY', 'type': 'numeric'})
        cls.parts.append({'path': 'CLOCK_MODE', 'type': 'text', 'value':'INTERNAL'})
        cls.parts.append({'path': 'CLOCK_SOURCE', 'type': 'numeric'})
        cls.parts.append({'path': 'TRIG_TIME', 'type': 'numeric'})
        cls.parts.append({'path': '.CHANNELS', 'type': 'structure'})
        for i in range(32):
            cls.parts.extend([
                {'path': '.CHANNELS.CHANNEL_%d' % (i+1), 'type': 'structure'},
                {'path': '.CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1), 'type': 'numeric'},
                {'path': '.CHANNELS.CHANNEL_%d:ENABLED' % (i+1), 'type': 'text', 'value':'ENABLED'},
                {'path': '.CHANNELS.CHANNEL_%d:DATA' % (i+1), 'type': 'signal', 'options': (
                'compress_on_put')},
            ])
        cls.parts.append({'path': 'SERIAL_NUM', 'type': 'numeric'})

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
        print('SEGMENT LEN: ', segmentLen)
        print('BUFFER LEN: ', bufferLen)
        blocksInSegment = segmentLen / bufferLen
        if blocksInSegment > 1:
            self.getNode('OUTPUTS:SEG_BLOCKS').putData(MDSplus.Int32(blocksInSegment))
        else:
            self.getNode('OUTPUTS:SEG_BLOCKS').deleteData()
#        samples = segmentLen/blocksInSegment
        samples = bufferLen
        for i in range(32):
            self.getNode('OUTPUTS:ADC%d_0.SAMPLES'%i).putData(MDSplus.Int32(samples))
        numEnabledChannels = 0
        for i in range(32):
            try:
                if self.getNode('CHANNELS.CHANNEL_%d:ENABLED' % (i+1)).data() == 'ENABLED':
                    self.getNode('OUTPUTS.ADC%d_0:SEG_LEN'%(i)).putData(MDSplus.Int32(blocksInSegment))
                    numEnabledChannels += 1
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

            self.getNode('.PARAMETERS.PAR_4:VALUE').putData(MDSplus.String('SI_TC'))
        elif clockMode == 'EXTERNAL':
            self.getNode('.PARAMETERS.PAR_4:VALUE').putData(MDSplus.String('PFI0'))
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
                    endings = endings.data()
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
                period = 1./frequency
        elif acquisitionMode == 'TRIGGERED' or acquisitionMode == 'TRIGGERED_PFI1' or acquisitionMode == 'TRIGGERED_PFI1_R_RTSI1' :
            self.getNode('.PARAMETERS.PAR_5:VALUE').putData("PFI1")
            if acquisitionMode == 'TRIGGERED_PFI1_R_RTSI1': 
                self.getNode('.PARAMETERS.PAR_7:VALUE').putData("RTSI1")
            else:
                self.getNode('.PARAMETERS.PAR_7:VALUE').putData("NO")
            if clockMode == 'INTERNAL':
                period = 1./frequency
            else:
                if not np.isscalar(deltas):
                    raise Exception('In TRIGGERED acquisition mode the external clock must be single speed for '+self.getPath())
                period = deltas
                frequency = 1./period
            try:
                triggerTime = self.getNode('TRIG_TIME').data()
            except:
                triggerTime = 0
            try:
                preTriggerSamples = self.getNode('PRE_TRIG').data()
            except:
                raise  Exception('Cannot get the number of pre trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_11:VALUE').putData(MDSplus.Int32(preTriggerSamples))
            try:
                postTriggerSamples = self.getNode('POST_TRIG').data()
            except:
                raise  Exception('Cannot get the number of post trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_12:VALUE').putData(MDSplus.Int32(postTriggerSamples))
            triggerTime -= period * preTriggerSamples
        
        elif acquisitionMode == 'TRIGGERED_RTSI1':
            self.getNode('.PARAMETERS.PAR_7:VALUE').putData("NO")
            self.getNode('.PARAMETERS.PAR_5:VALUE').putData("RTSI1")
            if clockMode == 'INTERNAL':
                period = 1./frequency
            else:
                if not np.isscalar(deltas):
                    raise Exception('In TRIGGERED acquisition mode the external clock must be single speed for '+self.getPath())
                period = deltas
                frequency = 1./period
            try:
                triggerTime = self.getNode('TRIG_TIME').data()
            except:
                triggerTime = 0
            try:
                preTriggerSamples = self.getNode('PRE_TRIG').data()
            except:
                raise  Exception('Cannot get the number of pre trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_11:VALUE').putData(MDSplus.Int32(preTriggerSamples))
            try:
                postTriggerSamples = self.getNode('POST_TRIG').data()
            except:
                raise  Exception('Cannot get the number of post trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_12:VALUE').putData(MDSplus.Int32(postTriggerSamples))
            triggerTime -= period * preTriggerSamples
        else:
            raise  Exception('Invalid Acquisition Mode '+acquisitionMode+' for '+self.getPath())
        if acquisitionMode == 'CONTINUOUS':
            self.getNode('.PARAMETERS.PAR_10:VALUE').putData(MDSplus.String(acquisitionMode))
        else:
            self.getNode('.PARAMETERS.PAR_10:VALUE').putData(MDSplus.String('TRIGGERED'))



#At this point triggerTime contains the (array of) trigger time(s)
        print('SCRIVO TRIGGER TIME', triggerTime)
        if acquisitionMode == 'TRIGGERED' or np.isscalar(triggerTime):
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDSplus.Float64(triggerTime))
        else:
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDSplus.Float64Array(triggerTime))
            self.getNode('OUTPUTS:TRIGGER_SAMP').putData(MDSplus.Int32Array(samplesPerTrigger))

####### Data expressions
        t = self.getTree()
        for i in range(32):
            dataExpr = '_c = data('+self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1)).getFullPath()+');'
            dataExpr += '_s = data('+self.getNode('OUTPUTS.ADC%d_0:VALUE'%(i)).getFullPath()+');'
            dataExpr += 'Build_With_Units(Build_signal((_c[0] + _c[1] * _s + _c[2] * _s * _s + _c[3] * _s * _s * _s),,dim_of(_s)), "Volts")'
            print(dataExpr)
#            dataExpr = 'Build_With_Units(NIanalogInputScaled('+self.getNode('OUTPUTS.ADC%d_0:VALUE'%(i)).getFullPath()+','+ self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1)).getFullPath()+'), "Volts")'
            self.getNode('CHANNELS.CHANNEL_%d:DATA' % (i+1)).putData(t.tdiCompile(dataExpr))
######## Timebase expression
        print('(0 : * : ('+ self.getNode('OUTPUTS.ADC0_0:SAMPLES').getFullPath()+' * 1./'+str(frequency)+'))')
        self.getNode('TIMEBASE').putData(t.tdiCompile('(0 : * : ('+ self.getNode('OUTPUTS.ADC0_0:SAMPLES').getFullPath()+
        ' * 1./'+str(frequency)+'))'))
#Write SamplingFrequency
        self.getNode('.PARAMETERS.PAR_9:VALUE').putData(MDSplus.Int32(frequency * numEnabledChannels))

####################################### Non MARTe2 Methods #############################################

    def init(self):
        AI_POLARITY_BIPOLAR = c_int(0)
        AI_POLARITY_UNIPOLAR = c_int(1)
        AI_CHANNEL_TYPE_DIFFERENTIAL = c_int(1)
        AI_CHANNEL_TYPE_NRSE = c_int(2)
        AI_CHANNEL_TYPE_RSE = c_int(3)
        AI_SAMPLE_SELECT_SI_TC = c_int(0)
        AI_SAMPLE_POLARITY_RISING_EDGE = c_int(0)


        polarityDict = {'Unipolar': AI_POLARITY_UNIPOLAR,
                        'Bipolar': AI_POLARITY_BIPOLAR}
        inputModeDict = {'RSE': AI_CHANNEL_TYPE_RSE, 'NRSE': AI_CHANNEL_TYPE_NRSE,
                        'Differential': AI_CHANNEL_TYPE_DIFFERENTIAL}
        try:
            niInterfaceLib = CDLL("libNiInterface.so")
        except:
            print('Cannot link to NIInterface for '+self.getPath()) 
            raise  Exception(MDSplus.mdsExceptions.TclFAILED_ESSENTIAL) 
        try:
            niLib = CDLL("libpxi6259.so")
        except:
            print('Cannot link to libpxi6259 for '+self.getPath()) 
            raise  Exception(MDSplus.mdsExceptions.TclFAILED_ESSENTIAL) 

        boardId = self.getNode('PARAMETERS.PAR_2:VALUE').data()
        gainDict = {10.: c_int(1), 5.: c_int(2), 2.: c_int(3), 1.: c_int(4), 0.5: c_int(5), 0.2: c_int(6), 0.1: c_int(7)}

        try:
            devName =  '/dev/pxi6259.'+str(boardId)+'.ai'
            dfd = os.open(devName, os.O_RDWR)
        except:
            raise Exception('Cannot open device for '+self.getPath())
        
        aiConf = c_void_p(0)
        niInterfaceLib.pxi6259_create_ai_conf_ptr(byref(aiConf))

        for chan in range(32):
            gain = self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_1:VALUE'%(chan)).data() 
            polarity = polarityDict[self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_3:VALUE'%(chan)).data()]
            inputMode = inputModeDict[self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_4:VALUE'%(chan)).data()]
            status = niLib.pxi6259_add_ai_channel(aiConf, c_byte(chan), polarity, gainDict[gain], inputMode, c_byte(0))
            if(status != 0):
                raise Exception('Cannot add channel '+str(currChan + 1)+' in '+self.getPath())

        status = niLib.pxi6259_load_ai_conf(c_int(dfd), aiConf)
        if(status != 0):
            raise Exception('Cannot load configuration for '+self.getPath())        


        for chan in range(32):
            try:
                currFd = os.open('/dev/pxi6259.'+str(boardId)+'.ai.'+str(chan), os.O_RDWR | os.O_NONBLOCK)
                currGain = self.getNode('OUTPUTS:ADC%d_0.PARAMETERS.PAR_1:VALUE'%(chan)).data()
                currGainCode = gainDict[currGain]
                n_coeff = c_int(0)
                coeffArray = c_float*4
                coeff = coeffArray()
                status = niInterfaceLib.pxi6259_getCalibrationParams(
                        c_int(currFd), currGainCode, coeff, byref(n_coeff))

                if(status < 0):
                    errno = niInterfaceLib.getErrno()
                    msg = 'Error (%d) %s' % (errno, os.strerror(errno))
                    print('Cannot read calibration values for Channel %d. Default value assumed ( offset= 0.0, gain = range/65536)' % (chan))
                #gainValue = gainValueDict[gain] * 2.
                gainValue = currGain * 2.
                coeff[0] = coeff[2] = coeff[3] = 0
                coeff[1] = c_float(gainValue / 65536.)
                self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM'%(chan + 1)).putData(MDSplus.Float32Array(coeff))
                os.close(currFd)
            except:
                print('Cannot get calibration parameters for '+self.getPath()) 
                raise  Exception(MDSplus.mdsExceptions.TclFAILED_ESSENTIAL) 
        os.close(dfd)

    # get card info
        try:
            devName =  '/dev/pxi6259.'+str(boardId)
            dfd = os.open(devName, os.O_RDWR)
            serialNum = c_int(0)
            if niLib.pxi6259_get_board_serial_number(dfd, byref(serialNum)) == 0:
                self.getNode('SERIAL_NUM').putData(MDSplus.Int32(serialNum))
            os.close(dfd)
        except:
            pass
