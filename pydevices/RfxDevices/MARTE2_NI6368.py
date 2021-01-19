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

from MDSplus import Data
MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('NI6368::NI6368ADC', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, 1./(build_path(".parameters:par_1:value")*build_path(".outputs.adc0_0:samples")))')
class MARTE2_NI6368(MC.MARTE2_COMPONENT):
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
    ]
    parameters = [
        {'name': 'DeviceName', 'type': 'string', 'value': '/dev/pxie-6368'},
        {'name': 'BoardId', 'type': 'int32', 'value': 0},
        {'name': 'DMABufferSize', 'type': 'int32', 'value': 1000},
        {'name': 'DelayDivisor', 'type': 'int32', 'value': 3},
        {'name': 'ClockSampleSource', 'type': 'string', 'value': 'INTERNALTIMING'},
        {'name': 'ClockSamplePolarity', 'type': 'string',
            'value': 'ACTIVE_HIGH_OR_RISING_EDGE'},
        {'name': 'ClockConvertSource', 'type': 'string', 'value': 'INTERNALTIMING'},
        {'name': 'ClockConvertPolarity', 'type': 'string', 'value': 'RISING_EDGE'},
        {'name': 'ScanIntervalCounterSource',
            'type': 'string', 'value': 'COUNTER_TB3'},
        {'name': 'ScanIntervalCounterPolarity',
            'type': 'string', 'value': 'RISING_EDGE'},
        {'name': 'ScanIntervalCounterPeriod', 'type': 'int32', 'value': 50},
        {'name': 'ScanIntervalCounterDelay', 'type': 'int32', 'value': 2},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xf},
    ]
    parts = []

    def prepareMarteInfo(self):
        print('0:1000000 : (build_path("\\'+self.getFullPath() +
              '.outputs.adc0_0:samples") / 2000000D0)')
        self.timebase.putData(Data.compile(
            '0:1000000 : (build_path("\\'+self.getFullPath()+'.outputs.adc0_0:samples") / 2000000D0)'))
        self.outputs_time_idx = 1  # The second produced signal is time
# put the same value of Samples and segment lengthin all output nodes
        self.outputs_adc1_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc2_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc3_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc4_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc5_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc6_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc7_0_samples.putData(self.outputs_adc0_0_samples.data())
