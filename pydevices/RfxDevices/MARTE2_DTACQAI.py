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


@MC.BUILDER('DTACQAI', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT)
class MARTE2_DTACQAI(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []}]
    for i in range(64):
        outputs += [
            {'name': 'CHAN_'+str(i+1), 'type': 'float64', 'dimensions': -1, 'parameters': [
            {'name': 'ChannelId', 'type': 'int', 'value': 0},]}]
    for i in range(32):
        outputs += [
            {'name': 'DI_'+str(i+1), 'type': 'uint8', 'dimensions': -1, 'parameters': [
            {'name': 'ChannelId', 'type': 'int', 'value': 0},]}]
    parameters = [
        {'name': 'SamplingFrequency', 'type': 'float64', 'value': 1E3},
        {'name': 'TriggerTime', 'type': 'float64', 'value': 0},
        {'name': 'Mode', 'type': 'int32', 'value': 2},
        {'name': 'NumAdc', 'type': 'int32', 'value': 0},
        {'name': 'NumDi', 'type': 'int32', 'value': 0},
        {'name': 'NumAiChans', 'type': 'int32'},
        {'name': 'CalGains', 'type': 'int32'},
        {'name': 'CalOffsets', 'type': 'int32'},
        {'name': 'IpAddress', 'type': 'string'},
        {'name': 'Port', 'type': 'int32'},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xf},
        {'name': 'SpadSize', 'type': 'int32', 'value': 64},
        {'name': 'NumSamples', 'type': 'int32', 'value': 1},
        {'name': 'FreqDivision', 'type': 'int32', 'value': 1},
    ]
    parts = []

    def prepareMarteInfo(self):
        print('0:1000000 :1. / (build_path("\\'+self.getFullPath()+'.parameters:par_1:value"))')
        self.timebase.putData(Data.compile('0:1000000 : 1. / (build_path("\\'+self.getFullPath()+'.parameters:par_1:value"))'))
        self.outputs_time_samples.putData(self.parameters_par_13_value.data())
        self.outputs_counter_samples.putData(self.parameters_par_13_value.data())
        for i in range(64):
            getattr(self, 'outputs_chan_'+str(i+1)+'_samples').putData(self.parameters_par_13_value.data())
        for i in range(32):
            getattr(self, 'outputs_di_'+str(i+1)+'_samples').putData(self.parameters_par_13_value.data())
       
