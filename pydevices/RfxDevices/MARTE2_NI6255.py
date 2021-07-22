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


@MC.BUILDER('NI6255', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, 1./(build_path(".parameters:par_1:value")*build_path(".outputs.adc0_0:samples")))')
class MARTE2_NI6255(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'ADC0', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC1', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC2', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC3', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC4', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC5', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC6', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC7', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC8', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC9', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC10', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC11', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC12', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC13', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC14', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC15', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC16', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC17', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC18', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC19', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC20', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC21', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC22', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC23', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC24', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC25', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC26', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC27', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC28', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC29', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC30', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'ADC31', 'type': 'int16', 'dimensions': 0, 'parameters': []},
    ]
    parameters = [
        {'name': 'Address', 'type': 'string'},
        {'name': 'Frequency', 'type': 'float32', 'value': 1000.}
    ]
    parts = []

    def prepareMarteInfo(self):
        print('0:1000000 : 1. / (build_path("\\'+self.getFullPath()+'.parameters:par_2:value"))')
        self.timebase.putData(Data.compile('0:1000000 : 1. / (build_path("\\'+self.getFullPath()+'.parameters:par_2:value"))'))
 