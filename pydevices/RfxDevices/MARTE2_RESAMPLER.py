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

from MDSplus import Data, TreeNode, TreePath

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('ResamplerGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_RESAMPLER(MC.MARTE2_COMPONENT):
    inputs = []
    for i in range(16):
        inputs.append({'name': 'In'+str(i+1), 'type': 'float64',
                       'dimensions': Data.compile('[1000]'), 'parameters': []})
    outputs = []
    for i in range(16):
        outputs.append({'name': 'Out'+str(i+1), 'type': 'float64',
                        'dimensions': -1, 'parameters': []})
    parameters = [{'name': 'SamplingPeriod', 'type': 'float64', 'value': 1},
                  {'name': 'ResamplingFactor', 'type': 'int32', 'value': 10},
                  {'name': 'UseLowPassFilter', 'type': 'int8', 'value': 0}]
    parts = []

    def prepareMarteInfo(self):
        # SamplingPeriod is derived fro timebase
        samplingPeriod = self.timebase.evaluate().getDelta().data()
        currDim = self.inputs_in1_dimensions.data()
        samplingPeriod = (1. * samplingPeriod)/currDim[0]
        self.parameters_par_1_value.putData(samplingPeriod)
# All outputs must have the same type of the corresponding input
# and the dimensior reduced by sampling factor
        resamplingFactor = self.parameters_par_2_value.data()
# propagate first dimension to all active inputs
        for chan in range(15):
            try:
                # Expected fail if no data samples
                getattr(self, 'inputs_in%d_value' % (chan+2)).getData()
                getattr(self, 'inputs_in%d_dimensions' %
                        (chan+1)).putData(currDim)
            except:
                pass
# propagate updated dimension (keeping into account resamplinFactor)
        for chan in range(16):
            try:
                getattr(self, 'outputs_out%d_type' % (
                    chan+1)).putData(getattr(self, 'inputs_in%d_type' % (chan+1)).data())
                getattr(self, 'outputs_out%d_dimensions' %
                        (chan+1)).putData(-1)
                # Expected fail if no data samples
                getattr(self, 'inputs_in%d_value' % (chan+1)).getData()
                currDim = getattr(self, 'inputs_in%d_dimensions' % (
                    chan+1)).data()  # The following should not fail
                currDim[0] /= resamplingFactor
                getattr(self, 'outputs_out%d_dimensions' %
                        (chan+1)).putData(currDim)
                getattr(self, 'outputs_out%d_samples' %
                        (chan+1)).putData(currDim[0])
            except:
                pass
        pass
