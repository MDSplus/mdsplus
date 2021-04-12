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


@MC.BUILDER('FFTGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_FFT(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'In', 'type': 'float64', 'dimensions': Data.compile('[256]'), 'parameters': []}]
    outputs = [
        {'name': 'OutModule', 'type': 'float64',
            'dimensions': Data.compile('[256]'), 'parameters': []},
        {'name': 'OutPhase', 'type': 'float64', 'dimensions': Data.compile('[256]'), 'parameters': []}]
    parameters = [{'name': 'NumberOfSamples', 'type': 'int32', 'value': 256}]
    parts = []

    def prepareMarteInfo(self):
        self.outputs_outphase_seg_len.putData(
            self.outputs_outmodule_seg_len.getData())
# put the same value of elements for input and outputs based on NumberOfSamples parameter
        numSamples = self.parameters_parameter_1_value.data()
        print('Num. Samples: '+str(numSamples))
        self.inputs_in_dimensions.putData(Data.compile('[$]', numSamples))
        self.outputs_outmodule_dimensions.putData(
            Data.compile('[$]', numSamples))
        self.outputs_outphase_dimensions.putData(
            Data.compile('[$]', numSamples))

# Specific consistency check to be implemented by MARTE2_COMPONENT subclasses
    def checkSpecific(self):
        # Check whether in field is defined and if it is a path or nid reference
        try:
            inSig = self.inputs_in_value.getData()
            if not(isinstance(inSig, TreeNode) or isinstance(inSig, TreePath)):
                return 'MARTE2_FFT IN field must be a reference to the output field of another MARTe2 device'
        except:
            return 'MARTE2_FFT IN field must be defined'
        return ''
