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


@MC.BUILDER('AD20xxADC', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT)
class MARTE2_AD20XXADC(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC0_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 0},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC1_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 1},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC2_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 2},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC3_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 3},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC4_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 4},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC5_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 5},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC6_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 6},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC7_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 7},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC8_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 8},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC9_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 9},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC10_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 10},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC11_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 11},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC12_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 12},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC13_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 13},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC14_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 14},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]},
        {'name': 'ADC15_0', 'type': 'int16', 'dimensions': 0, 'parameters': [
            {'name': 'ChannelId', 'type': 'int32', 'value': 15},
            {'name': 'InputRange', 'type': 'float32', 'value': 10}
        ]}]

    parameters = [
        {'name': 'BoardId', 'type': 'int32', 'value': 0},
        {'name': 'ClockSource', 'type': 'string', 'value': 'InternalTiming'},
        {'name': 'SSIMode', 'type': 'string', 'value': 'Master'},
        {'name': 'TriggerSource', 'type': 'string', 'value': 'ExternalSMB'},
        {'name': 'ExecutionMode', 'type': 'string', 'value': 'RealTimeThread'},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xF},
        {'name': 'ClockDivisor', 'type': 'int32', 'value': 320},
        {'name': 'Cycles', 'type': 'int32', 'value': 300},
        {'name': 'SleepTime', 'type': 'int32', 'value': 0},
        {'name': 'Model', 'type': 'int32', 'value': 2022}
    ]
    parts = []

    def prepareMarteInfo(self):
        self.timebase.putData(Data.compile('0:1000000 : 1.25E-8*'+str(self.outputs_adc0_0_samples.data())+'*build_path("\\'+self.getFullPath()+'.parameters:par_7:value")'))

        self.outputs_time_idx = 1  # The second produced signal is time
# put the same value of Samples and segment lengthin all output nodes
        self.outputs_adc1_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc2_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc3_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc4_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc5_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc6_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc7_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc8_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc9_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc10_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc11_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc12_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc13_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc14_0_samples.putData(self.outputs_adc0_0_samples.data())
        self.outputs_adc15_0_samples.putData(self.outputs_adc0_0_samples.data())
