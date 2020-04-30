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
import MDSplus
import numpy as np

MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_PYTHON(MC.MARTE2_COMPONENT):
    pass


class BUILDER:
  @staticmethod
  def convertType(cls, type,):
    typeConv = {int:'int32', float: 'float64', np.int16: 'int16', np.int32: 'int32', np.float32: 'float32', np.float64: 'float64'}
    return typeConv[type]

  def __init__(self, module, pygam=None):
      self.module = module
      self.pygam = pygam or module.pygam

  def __call__(self, cls):
    cls.inputs = []
    cls.outputs = []
    cls.parameters = []
    try:
      pygam, module = self.pygam, self.module
      pygam.initialize()
      for i in range(0, pygam.getNumberOfInputs()):
        currDimension = pygam.getInputDimensions(i)
        if currDimension[0] == 1 :
          if currDimension[1] == 1:
            inDimensions = 0
          else:
            inDimensions = [currDimension[1]]
        else:
          inDimensions = [currDimension[0],currDimension[1]]

        cls.inputs.append({'name': pygam.getInputName(i), 'type': self.convertType(pygam.getInputType(i)), 'dimensions': inDimensions, 'parameters':[]})

      for i in range(0, pygam.getNumberOfOutputs()):
        currDimension = pygam.getOutputDimensions(i)
        if currDimension[0] == 1 :
          if currDimension[1] == 1:
            outDimensions = 0
          else:
            outDimensions = [currDimension[1]]
        else:
          outDimensions = [currDimension[0],currDimension[1]]

        cls.outputs.append({'name': pygam.getOutputName(i), 'type': self.convertType(pygam.getOutputType(i)), 'dimensions': outDimensions, 'parameters':[]})

      cls.parameters.append({'name': 'FileName', 'type': 'string', 'value': module.__file__.split('/')[-1].split('.')[0]})
      for i in range(0, pygam.getNumberOfParameters()):
        currVal = pygam.getParameterDefaultValue(i)
        currDims = pygam.getParameterDimensions(i)
        if currDims[0] == 1 and currDims[1] == 1:
          mdsValue = currVal[0]
        else:
          if pygam.getParameterType(i) == np.float32 or pygam.getParameterType(i) == np.float64:
            mdsValue = MDSplus.Float64Array(currVal)
          else:
            mdsValue = MDSplus.Int32Array(currVal)

        cls.parameters.append({'name': 'Parameters.'+pygam.getParameterName(i), 'type': 'float64', 'value': mdsValue})
    except:
        pass
    cls.parts = []
    cls.buildGam(cls.parts, 'PyGAM', MC.MARTE2_COMPONENT.MODE_GAM)
    return cls


def BUILDER_FALLBACK(cls):
    cls.outputs = []
    cls.parameters = []
    cls.parts = []
    cls.buildGam(cls.parts, 'PyGAM', MC.MARTE2_COMPONENT.MODE_GAM)
    return cls
