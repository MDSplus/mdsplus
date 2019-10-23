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

from MDSplus import *
from MARTE2_COMPONENT import *
class MARTE2_PYTHON(MARTE2_COMPONENT):
    @classmethod
    def convertType(cls, type,):
      typeConv = {int:'int32', float: 'float64'}
      return typeConv[type]
  
    @classmethod
    def buildPythonGam(cls, module):
      MARTE2_COMPONENT.inputs = []
      for i in range(0, module.getNumberOfInputs()):
	MARTE2_COMPONENT.inputs.append({'name': module.getInputName(i), 'type': cls.convertType(module.getInputType(i)), 'dimensions': module.getInputDimensions(i), 'parameters':[]})

      MARTE2_COMPONENT.outputs = []
      for i in range(0, module.getNumberOfOutputs()):
	MARTE2_COMPONENT.outputs.append({'name': module.getOutputName(i), 'type': cls.convertType(module.getOutputType(i)), 'dimensions': module.getOutputDimensions(i), 'parameters':[]})

      MARTE2_COMPONENT.parameters = [{'name': 'FileName', 'type': 'string', 'value': module.__file__.split('/')[-1]}]
      for i in range(0, module.getNumberOfParameters()):
	MARTE2_COMPONENT.parameters.append({'name': module.getParameterName(i), 'type': 'float64', 'value': module.getParameterDefaultValue(i)})
      parts = []
      MARTE2_COMPONENT.buildGam(parts, 'PyGAM', MARTE2_COMPONENT.MODE_GAM)
      return parts

 
