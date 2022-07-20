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

from MDSplus import Data, TreeNode, TreePath, Float64

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('RTSMGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_RTSM(MC.MARTE2_COMPONENT):
    inputs = [{'name': 'InBits', 'type': 'int32',
               'dimensions': 0, 'parameters': []}]
    outputs = [{'name': 'OutBits', 'type': 'int32',
                'dimensions': 0, 'parameters': []}]
    for i in range(8):
        outputs.append({'name': 'OutWave'+str(i+1),
                        'type': 'float32', 'dimensions': -1, 'parameters': []})
    parameters = [{'name': 'TriggerIdx', 'type': 'int32', 'value': 0},
                  {'name': 'TriggerTime', 'type': 'float32', 'value': 0},
                  {'name': 'Period', 'type': 'float32', 'value': 0},
                  {'name': 'NumStates', 'type': 'int32', 'value': 0}]

    for stateIdx in range(16):
        parameters.append({'name': 'State'+str(stateIdx+1) +
                           '.DeadTime', 'type': 'float32', 'value': 0})
        for waveIdx in range(8):
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Wave'+str(waveIdx+1)+'_x', 'type': 'float32'})
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Wave'+str(waveIdx+1)+'_y', 'type': 'float32'})
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Wave'+str(waveIdx+1)+'_mode', 'type': 'int32'})
        parameters.append({'name': 'State'+str(stateIdx+1) +
                           '.NumNext', 'type': 'int32', 'value': 0})
        for nextIdx in range(16):
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Next'+str(nextIdx+1)+'.Mask', 'type': 'int32'})
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Next'+str(nextIdx+1)+'.Pattern', 'type': 'int32'})
        parameters.append({'name': 'State'+str(stateIdx+1) +
                           '.OutBits', 'type': 'int32', 'value': 0})

    for stateIdx in range(16):
        for nextIdx in range(16):
            parameters.append({'name': 'State'+str(stateIdx+1) +
                               '.Next'+str(nextIdx+1)+'.State', 'type': 'int32'})
            
    parameters.append({'name': 'OutBitsMask', 'type': 'int32', 'value': 0})
    for stateIdx in range(16):
        parameters.append({'name': 'State'+str(stateIdx+1) +
                           '.Comment', 'type': 'string'})
      

    parts = []

    def prepareMarteInfo(self):
     # try:
        period = self.timebase.evaluate().getDelta().data()
        self.parameters_par_3_value.putData(Float64(period))
        print('****************PERIOD:', period)
     # except:
        #  pass
