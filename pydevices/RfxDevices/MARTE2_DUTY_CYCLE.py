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

from MDSplus import Data, TreeNode, TreePath, Uint32

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('DutyCycleGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_DUTY_CYCLE(MC.MARTE2_COMPONENT):
    inputs = [{'name': 'InBits', 'type': 'uint32',
               'dimensions': 0, 'parameters': []}]
    outputs = [{'name': 'OutBits', 'type': 'uint32',
                'dimensions': 0, 'parameters': []}]
    parameters = [{'name': 'InFrequency', 'type': 'float32', 'value': 1000},
                  {'name': 'OutFrequency', 'type': 'float32', 'value': 10},
                  {'name': 'DutyCycle', 'type': 'float32', 'value': 50},
                  {'name': 'ClockIdx', 'type': 'uint32', 'value': 0},
                  {'name': 'EnableIdx', 'type': 'int32', 'value': -1},
                  {'name': 'AndMask', 'type': 'uint32',
                      'value': Uint32(0xFFFFFFFF)},
                  {'name': 'OrMask', 'type': 'uint32', 'value': 0}]
    parts = []

    def prepareMarteInfo(self):
        # InFrequency is derived from timebase
        period = self.timebase.evaluate().getDelta().data()
        inFrequency = 1./period
        self.parameters_par_1_value.putData(inFrequency)
