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
import ctypes
import numpy as np

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('PyGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_PYTHON_GENERIC(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'In1', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In2', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In3', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In4', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In5', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In6', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In7', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'In8', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
    ]
    outputs = [
        {'name': 'Out1', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out2', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out3', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out4', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out5', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out6', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out7', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Out8', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
    ]
    parameters = [
        {'name': 'FileName', 'type': 'string'},
        {'name': 'Parameter1', 'type': 'int32'},
        {'name': 'Parameter2', 'type': 'int32'},
        {'name': 'Parameter3', 'type': 'int32'},
        {'name': 'Parameter4', 'type': 'int32'},
        {'name': 'Parameter5', 'type': 'int32'},
        {'name': 'Parameter6', 'type': 'int32'},
        {'name': 'Parameter7', 'type': 'int32'},
        {'name': 'Parameter8', 'type': 'int32'},
    ]
    parts = []

    def prepareMarteInfo(self):
        for i in range(8):  # Append 'Parameters.' in from to every parameter name for defined parameters (i.e. with non empty value)
            try:
                parVal = self.getNode(
                    'parameters.par_'+str(2+i)+':value').data()
                # if code arrives here, append 'Paratemeters.' in front if not already done
                parName = self.getNode(
                    'parameters.par_'+str(2+i)+':name').data()
                if parName[:11] != 'Parameters.':
                    parName = 'Parameters.'+parName
                    self.getNode('parameters.par_'+str(2+i) +
                                 ':name').putData(parName)
            except:
                pass

    def getConfigurationFromPython(self):
        try:
            moduleName = self.getNode('parameters.par_1:value').data()
        except:
            raise Exception('Cannot retrieve python module name')
        try:
            import pygam
        except:
            raise Exception('Cannot import support module pygam')
        try:
            module = __import__(moduleName)
        except:
            raise Exception('Cannot import module '+moduleName)

        types = {
            int: 'int32',
            float: 'float64',
            np.int16: 'int16',
            np.int32: 'int32',
            np.float32: 'float32',
            np.float64: 'float64',
        }

        def convert_io(types, name, dim, dtype):
            if dim[0] == 1:
                if dim[1] == 1:
                    dim = 0
                else:
                    dim = [dim[1]]
            try:
                dtype = types[dtype]
            except KeyError:
                dtype = 'float64'
            return {
                'name': name,
                'type': dtype,
                'dimensions': dim,
                'parameters': [],
            }

        def convert_param(types, name, dim, dtype, val):
            try:
                dtype = types[dtype]
            except KeyError:
                dtype = 'int32'
            if dim[0] == 1 and dim[1] == 1:
                outVal = np.dtype(dtype).type(val[0])
            else:
                outVal = np.array(val, dtype)
            return {
                'name': 'Parameters.' + name,
                'type': dtype,
                'value': outVal,
            }

        pygam.initialize(moduleName)
        inputDicts = [
            convert_io(
                types,
                pygam.getInputName(moduleName, i),
                pygam.getInputDimensions(moduleName, i),
                pygam.getInputType(moduleName, i),
            )
            for i in range(pygam.getNumberOfInputs(moduleName))
        ]
        outputDicts = [
            convert_io(
                types,
                pygam.getOutputName(moduleName, i),
                pygam.getOutputDimensions(moduleName, i),
                pygam.getOutputType(moduleName, i),
            )
            for i in range(pygam.getNumberOfOutputs(moduleName))
        ]
        paramDicts = [
            convert_param(
                types,
                pygam.getParameterName(moduleName, i),
                pygam.getParameterDimensions(moduleName, i),
                pygam.getParameterType(moduleName, i),
                pygam.getParameterDefaultValue(moduleName, i),
            )
            for i in range(pygam.getNumberOfParameters(moduleName))
        ]
        # Fill Parameters
        for i in range(8):
            self.getNode('parameters.par_'+str(i+2)+':name').deleteData()
            self.getNode('parameters.par_'+str(i+2)+':value').deleteData()
            self.getNode('inputs.in'+str(i+1)+':dimensions').deleteData()
            self.getNode('inputs.in'+str(i+1)+':name').deleteData()
            self.getNode('inputs.in'+str(i+1)+':type').putData('int32')
            self.getNode('outputs.out'+str(i+1)+':dimensions').putData(-1)
            self.getNode('outputs.out'+str(i+1)+':name').deleteData()
            self.getNode('outputs.out'+str(i+1)+':type').putData('int32')

        if len(paramDicts) > 8:
            raise Exception('Number '+str(len(paramDicts)) +
                            ' of parameters cannot be greater than 8')
        if len(inputDicts) > 8:
            raise Exception('Number '+str(len(inputDicts)) +
                            ' of inputs cannot be greater than 8')
        if len(outputDicts) > 8:
            raise Exception('Number '+str(len(outputDicts)) +
                            ' of outputs cannot be greater than 8')

        parIdx = 2
        for paramDict in paramDicts:
            self.getNode('parameters.par_'+str(parIdx) +
                         ':name').putData(paramDict['name'])
            self.getNode('parameters.par_'+str(parIdx) +
                         ':value').putData(paramDict['value'])
            parIdx += 1

        inputIdx = 1
        for inputDict in inputDicts:
            self.getNode('inputs.in'+str(inputIdx) +
                         ':name').putData(inputDict['name'])
            self.getNode('inputs.in'+str(inputIdx) +
                         ':type').putData(inputDict['type'])
            self.getNode('inputs.in'+str(inputIdx) +
                         ':dimensions').putData(inputDict['dimensions'])
            inputIdx += 1

        outputIdx = 1
        for outputDict in outputDicts:
            self.getNode('outputs.out'+str(outputIdx) +
                         ':name').putData(outputDict['name'])
            self.getNode('outputs.out'+str(outputIdx) +
                         ':type').putData(outputDict['type'])
            self.getNode('outputs.out'+str(outputIdx) +
                         ':dimensions').putData(outputDict['dimensions'])
            outputIdx += 1
