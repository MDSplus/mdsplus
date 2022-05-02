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


@MC.BUILDER('UDP::UDPReceiver', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, build_path(".parameters:par_3:value"))')
class MARTE2_UDP_RECEIVER(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'OUT_0', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_1', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_2', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_3', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_4', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_5', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_6', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_7', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_8', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_9', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_10', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_11', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_12', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_13', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_14', 'type': 'int16', 'dimensions': -1, 'parameters': []},
        {'name': 'OUT_15', 'type': 'int16', 'dimensions': -1, 'parameters': []}]
    parameters = [
        {'name': 'Address', 'type': 'string'},
        {'name': 'Port', 'type': 'int32', 'value': 44488},
        {'name': 'Period', 'type': 'float32', 'value': 1E-3},
        {'name': 'Timeout', 'type': 'int32'}]
    parts = []

    def prepareMarteInfo(self):
        print('0 : 1000000 : '+self.getFullPath()+'.parameters:par_3:value')
        self.timebase.putData(Data.compile('0 : 1000000 : '+self.getFullPath()+'.parameters:par_3:value'))
        self.outputs_time_idx = 0  # The first produced signal is time
