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

import MDSplus
MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('NI6368::NI6368ADC', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, 1./(build_path(".parameters:par_1:value")*build_path(".outputs.adc0_0:samples")))')
class MARTE2_NI6368_ADC(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
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
        {'name': 'DMABufferSize', 'type': 'int32', 'value': 1000},
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
        self.getNode('OUTPUTS:SEG_BLOCKS').putData(MDSplus.Int32(blocksInSegment))
        samples = segmentLen/blocksInSegment
        for i in range(16):
            self.getNode('OUTPUTS:ADC%d_0.SAMPLES'%i).putData(MDSplus.Int32(samples))
        for i in range(16):
            try:
                if self.getNode('CHANNELS.CHANNEL_%d:ENABLED' % (i+1)).data() == 'ENABLED':
                    self.getNode('OUTPUTS.ADC%d_0:SEG_LEN'%(i)).putData(MDSplus.Int32(segmentLen))
                else:
                    self.getNode('OUTPUTS.ADC%d_0:SEG_LEN'%(i)).putData(MDSplus.Int32(0))
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
            self.getNode('.PARAMETERS.PAR_13:VALUE').putData(MDSplus.Int32(int(frequency)))
        elif clockMode == 'EXTERNAL':
            self.getNode('.PARAMETERS.PAR_5:VALUE').putData(MDSplus.String('PFI0'))
            self.getNode('.PARAMETERS.PAR_6:VALUE').putData(MDSplus.String('ACTIVE_HIGH_OR_RISING_EDGE'))
            self.getNode('.PARAMETERS.PAR_11:VALUE').putData(MDSplus.Int32(100))
            try:
                clockSource = self.getNode('CLOCK_SOURCE').getData()
            except:
                raise Exception('Cannot read clock source for '+self.getPath())
            if not isinstance(clockSource, MDSplus.Range):
                raise Exception('Invalid clock source for '+self.getPath()+' Must be a Range descriptor')
            begins = clockSource.getBegin()
            if begins != None:
                try:
                    begins = begins.data()
                except:
                    raise Exception('Cannot get begin time(s) for clock source in '+self.getPath())
            endings = clockSource.getEnding()
            if endings != None:
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
            self.getNode('.PARAMETERS.PAR_14:VALUE').putData(MDSplus.String('CONTINUOUS'))
            if clockMode == 'EXTERNAL':
                if begins == None:
                    try:
                        triggerTime = self.getNode('TRIG_TIME').data()
                    except:
                        triggerTime = 0
                elif np.isscalar(begins):       #Single gated clock
                    triggerTime = begins
                else:                           #Multiple gated clocks
                    if endings == None or len(endings) != len(begins):
                        raise Exception('Number of begin times different from number of end times for clock source in '+self.getPath())
                    currTrigs = []
                    currTrigSegments = []
                    for currPulse in range(len(begins)):
                        currPulseSamples = (endings[currPulse] - begins[currPulse])/deltas[currPulse]
                        if currPulseSamples % segmentLen != 0:
                            raise Exception('The number of pulse samples must be a multiple of Segment len  in multi gated external clock in '+self.getPath())
                        segmentsPerPulse = currPulseSamples/segmentLen
                        currTrigs.append(begins[currPulse])
                        currTrigSegments.append(segmentsPerPulse)
 
                    triggerTime = np.array(currTrigs, dtype=float) 
                    triggerSegments =  np.array(currTrigSegments, dtype=float)    
            else: #clock mode INTERNAL
                try:
                    triggerTime = self.getNode('TRIG_TIME').data()
                except:
                    triggerTime = 0
        elif acquisitionMode == 'TRIGGERED' or acquisitionMode == 'SOFTWARE_TRIGGERED':
            if clockMode == 'INTERNAL':
                period = 1./Frequency
            else:
                if not np.isscalar(deltas):
                    raise Exception('In TRIGGERED acquisition mode the external clock must be single speed for '+self.getPath())
                period = deltas
            try:
                triggerTime = self.getNode('TRIG_TIME').data()
            except:
                triggerTime = 0
            try:
                postTriggerSamples = self.getNode('POST_TRIG').data()
            except:
                raise  Exception('Cannot get the number of post trigger samples for '+self.getPath())
            self.getNode('.PARAMETERS.PAR_15:VALUE').putData(MDSplus.Int32(postTriggerSamples))
        elif acquisitionMode == 'MULTIPLE_TRIGGERED':
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
                segmentsPerPulse = postTriggerSamples / segmentLen
                currTrigs = []
                currTrigSegments = []
                for currPulse in range(len(pulseTriggerTime)):
                    currTrigs.append(pulseTriggerTime[currPulse])
                    currTrigSegments.append(segmentsPerPulse)
                triggerTime = np.array(currTrigs, dtype=float) 
                segmentsPerTrigger = np.array(currTrigSegments, dtype=float) 
            else:
                triggerTime = pulseTriggerTime

#At this point triggerTime contains the (array of) trigger time(s)
        if np.isscalar(triggerTime):
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDplus.Float64(triggerTime))
        else:
            self.getNode('OUTPUTS:TRIGGER_TIME').putData(MDplus.Float64Array(triggerTime))
            self.getNode('OUTPUTS:TRIGGER_SEGS').putData(MDplus.Float64Array(segmentsPerTrigger))

####### Data expressions
        t = self.getTree()
        for i in range(16):
            dataExpr = 'Build_With_Units(NIanalogInputScaled('+self.getNode('OUTPUTS.ADC%d_0:VALUE'%(i)).getFullPath()+','+ self.getNode('CHANNELS.CHANNEL_%d:CALIB_PARAM' % (i+1)).getFullPath()+'), "Volts")'
            self.getNode('CHANNELS.CHANNEL_%d:DATA' % (i+1)).putData(t.tdiCompile(dataExpr))
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
