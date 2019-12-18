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
import numpy as np

class MARTE2_PYTHON(MARTE2_COMPONENT):
    @classmethod
    def convertType(cls, type,):
      typeConv = {int:'int32', float: 'float64', np.int16: 'int16', np.int32: 'int32', np.float32: 'float32', np.float64: 'float64'}
      return typeConv[type]

 
    @classmethod
    def buildPythonGam(cls, pygam, module):
      pygam.initialize()
      MARTE2_COMPONENT.inputs = []
      for i in range(0, pygam.getNumberOfInputs()):
        currDimension = pygam.getInputDimensions(i)
        if currDimension[0] == 1 :
          if currDimension[1] == 1:
            inDimensions = 0
          else:
            inDimensions = [currDimension[1]]
        else:
          inDimensions = [currDimension[0],currDimension[1]]
          
        MARTE2_COMPONENT.inputs.append({'name': pygam.getInputName(i), 'type': cls.convertType(pygam.getInputType(i)), 'dimensions': inDimensions, 'parameters':[]})

      MARTE2_COMPONENT.outputs = []
      for i in range(0, pygam.getNumberOfOutputs()):
        currDimension = pygam.getOutputDimensions(i)
        if currDimension[0] == 1 :
          if currDimension[1] == 1:
            outDimensions = 0
          else:
            outDimensions = [currDimension[1]]
        else:
          outDimensions = [currDimension[0],currDimension[1]]
          
        MARTE2_COMPONENT.outputs.append({'name': pygam.getOutputName(i), 'type': cls.convertType(pygam.getOutputType(i)), 'dimensions': outDimensions, 'parameters':[]})

      MARTE2_COMPONENT.parameters = [{'name': 'FileName', 'type': 'string', 'value': module.__file__.split('/')[-1].split('.')[0]}]
      for i in range(0, pygam.getNumberOfParameters()):
        currVal = pygam.getParameterDefaultValue(i)
        currDims = pygam.getParameterDimensions(i)
        if currDims == 0:
          mdsValue = currVal
        else:
          if pygam.getParameterType(i) == np.float32 or pygam.getParameterType(i) == np.float64:
            mdsValue = Float64Array(currVal)
          else:
            mdsValue = Int32Array(currVal)
        
        MARTE2_COMPONENT.parameters.append({'name': pygam.getParameterName(i), 'type': 'float64', 'value': mdsValue})

      parts = []
      MARTE2_COMPONENT.buildGam(parts, 'PyGAM', MARTE2_COMPONENT.MODE_GAM)
      return parts

 
