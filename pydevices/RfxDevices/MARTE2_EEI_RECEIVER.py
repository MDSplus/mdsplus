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

from MDSplus import Data, Int32Array
MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('EEIIn', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT,
            'build_range(0, 1000000, build_path(".parameters:par_3:value"))')
class MARTE2_EEI_RECEIVER(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Time', 'type': 'int32', 'dimensions': 0, 'parameters': []},
        {'name': 'RECT_DC_CURR', 'type': 'float32', 'dimensions': Int32Array([3]), 'parameters': []},
        {'name': 'RECT_DC_VOLT', 'type': 'float32', 'dimensions': Int32Array([3]), 'parameters': []},
        {'name': 'DCLINK_VOLT', 'type': 'float32', 'dimensions': Int32Array([3]), 'parameters': []},
        {'name': 'PR_INV_CUR', 'type': 'float32', 'dimensions': Int32Array([27]), 'parameters': []},
        {'name': 'INV_CUR_RED', 'type': 'float32', 'dimensions': Int32Array([27]), 'parameters': []},
        {'name': 'PR_INV_VOLT', 'type': 'float32', 'dimensions': Int32Array([27]), 'parameters': []},
        {'name': 'INV_VOLT_RED', 'type': 'float32', 'dimensions': Int32Array([27]), 'parameters': []},
        {'name': 'CROWBAR_STAT', 'type': 'uint8', 'dimensions': Int32Array([3]), 'parameters': []},
        {'name': 'LIM_END_REF', 'type': 'float32', 'dimensions': Int32Array([27]), 'parameters': []},
        {'name': 'RECT_STATUS', 'type': 'uint8', 'dimensions': Int32Array([3]), 'parameters': []},
        {'name': 'INV_STATUS', 'type': 'uint8', 'dimensions': Int32Array([27]), 'parameters': []}]
    
    parameters = [
        {'name': 'Ip', 'type': 'string'},
        {'name': 'Port', 'type': 'int32', 'value': 44488},
        {'name': 'Period', 'type': 'float32', 'value': 1E-3},
        {'name': 'CpuMask', 'type': 'int32', 'value': 255},
        {'name': 'IsSync', 'type': 'int32', 'value': 1},
        {'name': 'CircuitId', 'type': 'int32', 'value': 1},
    ]
    parts = []

    def prepareMarteInfo(self):
        print('0 : 1000000 : '+self.getFullPath()+'.parameters:par_3:value')
        self.timebase.putData(Data.compile('0 : 1000000 : '+self.getFullPath()+'.parameters:par_3:value'))
