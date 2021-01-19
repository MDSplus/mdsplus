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

from MDSplus import Data, Float32

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('SWTrig', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT)
class MARTE2_SWTRIG(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Time', 'type': 'int32', 'dimensions': 0, 'parameters': {}}]
    for trigIdx in range(8):
        outputs.append({'name': 'Trigger'+str(trigIdx+1), 'type': 'uint8', 'dimensions': -1, 'parameters': [
            {'name': 'TriggerEvent', 'type': 'string', 'value': ''},
            {'name': 'WaitCycles', 'type': 'int32', 'value': 0},
            {'name': 'TriggerCycles', 'type': 'int32', 'value': 0}]})
    parameters = [
        {'name': 'StartEvent', 'type': 'string'},
        {'name': 'Frequency', 'type': 'float32'},
        {'name': 'StartTime', 'type': 'float32'},
        {'name': 'CpuMask', 'type': 'uint32'},
        {'name': 'StackSize', 'type': 'uint32'}
    ]
    parts = []

    def prepareMarteInfo(self):
        print('('+self.getFullPath()+'.parameters.par_3:value):1000000 : (1./' +
              self.getFullPath()+'.parameters.par_2:value)')
        self.timebase.putData(self.getTree().tdiCompile(
            '('+self.getFullPath()+'.parameters.par_3:value):1000000 : (1./'+self.getFullPath()+'.parameters.par_2:value)'))
# declare only outputs for which event name is specified
        for trigIdx in range(8):
            try:
                eventName = getattr(
                    self, 'outputs_trigger%d_parameters_par_1_value' % (trigIdx + 1)).data()
                print(eventName)
                if eventName == '':
                    getattr(self, 'outputs_trigger%d_dimensions' %
                            (trigIdx + 1)).putData(Float32(-1))
                else:
                    getattr(self, 'outputs_trigger%d_dimensions' %
                            (trigIdx + 1)).putData(Float32(0))
            except:
                getattr(self, 'outputs_trigger%d_dimensions' %
                        (trigIdx + 1)).putData(Float32(-1))
