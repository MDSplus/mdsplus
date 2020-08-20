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

import numpy as np


MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_PYTHON(MC.MARTE2_COMPONENT):
    pass


class BUILDER:
    types = {
        int:'int32',
        float: 'float64',
        np.int16: 'int16',
        np.int32: 'int32',
        np.float32: 'float32',
        np.float64: 'float64',
    }

    def __init__(self, module, pygam=None):
        self.module = module
        self.pygam = pygam or module.pygam

    @classmethod
    def convert_io(cls, name, dim, dtype):
        if dim[0] == 1 :
            if dim[1] == 1:
              dim = 0
            else:
              dim = [dim[1]]
        try:
            dtype = cls.types[dtype]
        except KeyError:
            dtype = 'float64'
        return {
            'name': name,
            'type': dtype,
            'dimensions': dim,
            'parameters': [],
        }

    @classmethod
    def convert_param(cls, name, dim, dtype, val):
        try:
            dtype = cls.types[dtype]
        except KeyError:
             dtype = 'int32'
        if dim[0] == 1 and dim[1] == 1:
          try:
            outval = val[0]
          except:
            outval = val
        else:
          outval = np.array(val, dtype)
        return {
            'name': 'Parameters.' + name,
            'type': dtype,
            'value': outval,
#            'value': np.array(val, dtype),
        }

    def __call__(self, cls):
        self.pygam.initialize()
        cls.inputs = [
            self.convert_io(
                self.pygam.getInputName(i),
                self.pygam.getInputDimensions(i),
                self.pygam.getInputType(i),
            )
            for i in range(self.pygam.getNumberOfInputs())
        ]
        cls.outputs = [
            self.convert_io(
                self.pygam.getOutputName(i),
                self.pygam.getOutputDimensions(i),
                self.pygam.getOutputType(i),
            )
            for i in range(self.pygam.getNumberOfOutputs())
        ]
        cls.parameters = [{
            'name': 'FileName',
            'type': 'string',
            'value': self.module.__file__.split('/')[-1].split('.')[0]},
        ] + [
            self.convert_param(
                self.pygam.getParameterName(i),
                self.pygam.getParameterDimensions(i),
                self.pygam.getParameterType(i),
                self.pygam.getParameterDefaultValue(i),
            )
            for i in range(self.pygam.getNumberOfParameters())
        ]
        cls.parts = []
        cls.buildGam(cls.parts, 'PyGAM', MC.MARTE2_COMPONENT.MODE_GAM)
        return cls


def BUILDER_FALLBACK(cls):
    cls.inputs = []
    cls.outputs = []
    cls.parameters = []
    cls.parts = []
    cls.buildGam(cls.parts, 'PyGAM', MC.MARTE2_COMPONENT.MODE_GAM)
    return cls
