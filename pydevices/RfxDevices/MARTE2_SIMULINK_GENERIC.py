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
import MDSplus

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('SimulinkWrapperGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_SIMULINK_GENERIC(MC.MARTE2_COMPONENT):
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
        {'name': 'Verbosity', 'type': 'int32', 'value': 2},
        {'name': 'Library', 'type': 'string'},
        {'name': 'SymbolPrefix', 'type': 'string'},
        {'name': 'Parameter1', 'type': 'int32'},
        {'name': 'Parameter2', 'type': 'int32'},
        {'name': 'Parameter3', 'type': 'int32'},
        {'name': 'Parameter4', 'type': 'int32'},
        {'name': 'Parameter5', 'type': 'int32'},
        {'name': 'Parameter6', 'type': 'int32'},
        {'name': 'Parameter7', 'type': 'int32'},
        {'name': 'SkipInvalidTunableParams', 'type': 'int32', 'value': 0},

    ]
    parts = []

    def prepareMarteInfo(self):
        # By convention, the library name is assumed to have the same name of prefix and in $LD_LIBRARY_PATH
        self.getNode('parameters.par_2:value').putData(
            self.getNode('parameters.par_3:value').data() + '.so')
        for i in range(8):  # Append 'Parameters.' in from to every parameter name for defined parameters (i.e. with non empty value)
            try:
                parVal = self.getNode(
                    'parameters.par_'+str(4+i)+':value').data()
                # if code arrives here, append 'Paratemeters.' in front if not already done
                parName = self.getNode(
                    'parameters.par_'+str(4+i)+':name').data()
                if parName[:11] != 'Parameters.':
                    parName = 'Parameters.'+parName
                    self.getNode('parameters.par_'+str(4+i) +
                                 ':name').putData(parName)
            except:
                pass

    def getConfigurationFromSimulink(self):
        try:
            modelName = self.getNode('parameters.par_3:value').data()
            libraryName = modelName + '.so'
            ModelLib = ctypes.cdll.LoadLibrary(libraryName)
        except:
            raise Exception(
                'Cannot retrieve shared library for the Simulink model ')
        initializeFuncName = modelName + '_initialize'
        # By default functions are assumed to return the C int type.
        # Other return types can be specified by setting the restype attribute.
        InstFunc = getattr(ModelLib, modelName)
        InstFunc.argtypes = []
        InstFunc.restype = ctypes.POINTER(None)
        states = InstFunc()

        GetMmiPtr = getattr(ModelLib, modelName+'_GetCAPImmi')
        GetMmiPtr.argtypes = [ctypes.POINTER(None)]
        GetMmiPtr.restype = ctypes.POINTER(None)
        mmi = GetMmiPtr(states)

# Initialization function is called with a dynamically-specified name
        InitializeFunc = getattr(ModelLib, initializeFuncName)
        InitializeFunc.argtypes = [ctypes.POINTER(None)]
        InitializeFunc(states)

        try:
            WrapperLib = ctypes.cdll.LoadLibrary("rtw_capi_wrapper.so")
        except:
            raise Exception(
                'Cannot link to rtw_capi_wrapper.so, required to get information from '+libraryName)
        # ** GENERAL MODEL DATA **

        # Number of inputs
        WrapperLib.WCAPI_GetNumRootInputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumRootInputs.restype = ctypes.c_int
        numInputs = WrapperLib.WCAPI_GetNumRootInputs(mmi)
        if(numInputs > 8):
            raise Exception('Number '+str(numInputs) +
                            ' of inputs cannot be greater than 8')

        # Number of outputs
        WrapperLib.WCAPI_GetNumRootOutputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumRootOutputs.restype = ctypes.c_int
        numOutputs = WrapperLib.WCAPI_GetNumRootOutputs(mmi)
        if(numOutputs > 8):
            raise Exception('Number '+str(numOutputs) +
                            ' of outputs cannot be greater than 8')

        # Number of parameters
        WrapperLib.WCAPI_GetNumModelParameters.argtypes = [
            ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumModelParameters.restype = ctypes.c_int
        numParameters = WrapperLib.WCAPI_GetNumModelParameters(mmi)
        if(numParameters > 8):
            raise Exception('Number '+str(numParameters) +
                            ' of parameters cannot be greater than 8')

        # Input structure
        WrapperLib.WCAPI_GetRootInputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetRootInputs.restype = ctypes.POINTER(None)
        RootInputStruct = WrapperLib.WCAPI_GetRootInputs(mmi)

        # Output structure
        WrapperLib.WCAPI_GetRootOutputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetRootOutputs.restype = ctypes.POINTER(None)
        RootOutputStruct = WrapperLib.WCAPI_GetRootOutputs(mmi)

        # Parameter structure
        WrapperLib.WCAPI_GetModelParameters.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetModelParameters.restype = ctypes.POINTER(None)
        ParameterStruct = WrapperLib.WCAPI_GetModelParameters(mmi)

        # DataType structure
        WrapperLib.WCAPI_GetDataTypeMap.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetDataTypeMap.restype = ctypes.POINTER(None)
        DataTypeMap = WrapperLib.WCAPI_GetDataTypeMap(mmi)

        # Dimension structure
        WrapperLib.WCAPI_GetDimensionMap.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetDimensionMap.restype = ctypes.POINTER(None)
        DimensionMap = WrapperLib.WCAPI_GetDimensionMap(mmi)

        # Dimension structure
        WrapperLib.WCAPI_GetDataAddressMap.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetDataAddressMap.restype = ctypes.POINTER(
            ctypes.POINTER(None))
        DataAddrMap = WrapperLib.WCAPI_GetDataAddressMap(mmi)

        # Dimension array
        WrapperLib.WCAPI_GetDimensionArray.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetDimensionArray.restype = ctypes.POINTER(
            ctypes.c_uint)
        DimensionArray = WrapperLib.WCAPI_GetDimensionArray(mmi)

        # ** SIGNAL DATA **

        # Functions are imported from the library:

        # 1. Name
        WrapperLib.WCAPI_GetSignalName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetSignalName.restype = ctypes.c_char_p

        # 2. Type
        WrapperLib.WCAPI_GetSignalDataTypeIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetSignalDataTypeIdx.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataTypeSLId.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeSLId.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataTypeCName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeCName.restype = ctypes.c_char_p

        # 3. Dimensions
        WrapperLib.WCAPI_GetDimArrayIndex.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDimArrayIndex.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetNumDims.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetNumDims.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetSignalDimensionIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetSignalDimensionIdx.restype = ctypes.c_uint16

        def removeAngular(strIn):
            if strIn[0] == '<':
                return strIn[1:-1]
            return strIn

        # Function to retrieve data on inputs and outputs
        def GetSignalData(SignalStruct, numOfSignals):

            signalList = []

            for signalIdx in range(numOfSignals):

                # name is retrieved
                retrievedName = WrapperLib.WCAPI_GetSignalName(
                    SignalStruct, signalIdx)
                retrievedName = retrievedName.decode("utf-8")
                retrievedName = removeAngular(retrievedName)

                # type is retrieved
                retrievedTypeIdx = WrapperLib.WCAPI_GetSignalDataTypeIdx(
                    SignalStruct, signalIdx)
                retrievedSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                    DataTypeMap, retrievedTypeIdx)

                retrievedCTypename = WrapperLib.WCAPI_GetDataTypeCName(
                    DataTypeMap, retrievedTypeIdx)
                retrievedCTypename = retrievedCTypename.decode("utf-8")

                if retrievedSLIdType == 0:
                    MARTe2Typename = 'float64'
                elif retrievedSLIdType == 1:
                    MARTe2Typename = 'float32'
                elif retrievedSLIdType == 2:
                    MARTe2Typename = 'int8'
                elif retrievedSLIdType == 3:
                    MARTe2Typename = 'uint8'
                elif retrievedSLIdType == 4:
                    MARTe2Typename = 'int16'
                elif retrievedSLIdType == 5:
                    MARTe2Typename = 'uint16'
                elif retrievedSLIdType == 6:
                    MARTe2Typename = 'int32'
                elif retrievedSLIdType == 7:
                    MARTe2Typename = 'uint32'
                elif retrievedSLIdType == 8:
                    MARTe2Typename = 'bool'
                else:
                    raise Exception('Unsupported datatype.')

                # dimensions are retrieved
                dimIdx = WrapperLib.WCAPI_GetSignalDimensionIdx(
                    SignalStruct, signalIdx)

                dimArrayIdx = WrapperLib.WCAPI_GetDimArrayIndex(
                    DimensionMap, dimIdx)  # Starting position in the dimensionArray
                # Number of elements in the dimensionArray referring to this signal
                dimNum = WrapperLib.WCAPI_GetNumDims(
                    DimensionMap,       dimIdx)

                currDimension = []
                for currIdx in range(dimNum):
                    currDimension.append(DimensionArray[dimArrayIdx + currIdx])
                if currDimension[0] == 1 and currDimension[1] == 1:
                    dimension = 0
                elif currDimension[0] == 1 or currDimension[1] == 1:
                    dimension = [currDimension[0]*currDimension[1]]
                else:
                    dimension = currDimension

                # retrieved data is saved to a dictionary
                signalDict = dict(
                    name=retrievedName, type=MARTe2Typename, dimensions=dimension, parameters={})

                # dictionary is appended to the MDSplus-style list
                signalList.append(signalDict)

            return signalList

        # ** PARAMETER DATA **

        # Name
        WrapperLib.WCAPI_GetModelParameterName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetModelParameterName.restype = ctypes.c_char_p

        # Type
        # same as the ones used for signals

        # Dimension
        WrapperLib.WCAPI_GetModelParameterDimensionIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetModelParameterDimensionIdx.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetModelParameterDataTypeIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetModelParameterDataTypeIdx.restype = ctypes.c_uint

        # Value
        WrapperLib.WCAPI_GetModelParameterAddrIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetModelParameterAddrIdx.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataAddress.argtypes = [
            ctypes.POINTER(ctypes.POINTER(None)), ctypes.c_int]
        WrapperLib.WCAPI_GetDataAddress.restype = ctypes.c_void_p

        # Function to retrieve data on parameters
        def GetModelParameterData(ParameterStruct, numOfParameters):

            paramList = []

            for paramIdx in range(numOfParameters):

                # name is retrieved
                retrievedName = WrapperLib.WCAPI_GetModelParameterName(
                    ParameterStruct, paramIdx)
                retrievedName = retrievedName.decode("utf-8")

                # type is retrieved
                retrievedTypeIdx = WrapperLib.WCAPI_GetModelParameterDataTypeIdx(
                    ParameterStruct, paramIdx)
                retrievedSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                    DataTypeMap, retrievedTypeIdx)

                retrievedCTypename = WrapperLib.WCAPI_GetDataTypeCName(
                    DataTypeMap, retrievedTypeIdx)
                retrievedCTypename = retrievedCTypename.decode("utf-8")

                if retrievedSLIdType == 0:
                    MARTe2Typename = 'float64'
                    pythonTypename = ctypes.c_double
                elif retrievedSLIdType == 1:
                    MARTe2Typename = 'float32'
                    pythonTypename = ctypes.c_float
                elif retrievedSLIdType == 2:
                    MARTe2Typename = 'int8'
                    pythonTypename = ctypes.c_int8
                elif retrievedSLIdType == 3:
                    MARTe2Typename = 'uint8'
                    pythonTypename = ctypes.c_uint8
                elif retrievedSLIdType == 4:
                    MARTe2Typename = 'int16'
                    pythonTypename = ctypes.c_int16
                elif retrievedSLIdType == 5:
                    MARTe2Typename = 'uint16'
                    pythonTypename = ctypes.c_uint16
                elif retrievedSLIdType == 6:
                    MARTe2Typename = 'int32'
                    pythonTypename = ctypes.c_int32
                elif retrievedSLIdType == 7:
                    MARTe2Typename = 'uint32'
                    pythonTypename = ctypes.c_uint32
                elif retrievedSLIdType == 8:
                    MARTe2Typename = 'bool'
                    pythonTypename = ctypes.c_bool
                else:
                    raise Exception('Unsupported parameter datatype.')

                # actual parameter value is retrieved
                paramAddrIdx = WrapperLib.WCAPI_GetModelParameterAddrIdx(
                    ParameterStruct, paramIdx)
                paramDataAddr = WrapperLib.WCAPI_GetDataAddress(
                    DataAddrMap, paramAddrIdx)
                paramPointer = ctypes.cast(
                    paramDataAddr, ctypes.POINTER(pythonTypename))

                # dimensions are retrieved
                dimIdx = WrapperLib.WCAPI_GetModelParameterDimensionIdx(
                    ParameterStruct, paramIdx)

                dimArrayIdx = WrapperLib.WCAPI_GetDimArrayIndex(
                    DimensionMap, dimIdx)  # Starting position in the dimensionArray
                # Number of elements in the dimensionArray referring to this signal
                dimNum = WrapperLib.WCAPI_GetNumDims(
                    DimensionMap,       dimIdx)

                currDimension = []
                for currIdx in range(dimNum):
                    currDimension.append(DimensionArray[dimArrayIdx + currIdx])

                if currDimension[0] == 1 and currDimension[1] == 1:  # scalar
                    dimension = 0
                    mdsplusValue = paramPointer[0]
                elif currDimension[0] == 1:  # vector
                    dimension = currDimension[0]*currDimension[1]
                    valueList = []
                    for idx in range(dimension):
                        valueList.append(paramPointer[idx])
                else:
                    # matrix or column vector (MARTe2 sees column vectors as matrices)
                    dimension = currDimension
                    idx = 0
                    valueList = []
                    valueRow = []
                    for nRow in range(currDimension[0]):
                        if currDimension[1] == 1:
                            valueList.append(paramPointer[idx])
                            idx = idx + 1
                        else:
                            for nCol in range(currDimension[1]):
                                valueRow.append(paramPointer[idx])
                                idx = idx + 1
                            valueList.append(valueRow)
                            valueRow = []
                    # Transpose matrix
                    if currDimension[0] > 1 and currDimension[1] > 1:
                        transList = []
                        transRow = []
                        for nRow in range(currDimension[1]):
                            for nCol in range(currDimension[0]):
                                transRow.append(
                                    paramPointer[nCol * currDimension[1] + nRow])
                            transList.append(transRow)
                            transRow = []
                        valueList = transList

                if currDimension[0] != 1 or currDimension[1] != 1:
                    if retrievedSLIdType == 0:
                        mdsplusValue = MDSplus.Float64Array(valueList)
                    elif retrievedSLIdType == 1:
                        mdsplusValue = MDSplus.Float32Array(valueList)
                    elif retrievedSLIdType == 2:
                        mdsplusValue = MDSplus.Int8Array(valueList)
                    elif retrievedSLIdType == 3:
                        mdsplusValue = MDSplus.Uint8Array(valueList)
                    elif retrievedSLIdType == 4:
                        mdsplusValue = MDSplus.Int16Array(valueList)
                    elif retrievedSLIdType == 5:
                        mdsplusValue = MDSplus.Uint16Array(valueList)
                    elif retrievedSLIdType == 6:
                        mdsplusValue = MDSplus.Int32Array(valueList)
                    elif retrievedSLIdType == 7:
                        mdsplusValue = MDSplus.Uint32Array(valueList)
                    else:
                        raise Exception('Unsupported parameter datatype.')

                # retrieved data is saved to a dictionary
                paramDict = dict(name='Parameters.'+retrievedName,
                                 type=MARTe2Typename, dimensions=dimension, value=mdsplusValue)

                # dictionary is appended to the MDSplus-style list
                paramList.append(paramDict)

            return paramList

        # Fill Parameters
        for i in range(8):
            self.getNode('parameters.par_'+str(i+4)+':name').deleteData()
            self.getNode('parameters.par_'+str(i+4)+':value').deleteData()
            self.getNode('inputs.in'+str(i+1)+':dimensions').deleteData()
            self.getNode('inputs.in'+str(i+1)+':name').deleteData()
            self.getNode('inputs.in'+str(i+1)+':type').putData('int32')
            self.getNode('outputs.out'+str(i+1)+':dimensions').putData(-1)
            self.getNode('outputs.out'+str(i+1)+':name').deleteData()
            self.getNode('outputs.out'+str(i+1)+':type').putData('int32')

        paramDicts = GetModelParameterData(ParameterStruct, numParameters)
        parIdx = 4
        for paramDict in paramDicts:
            self.getNode('parameters.par_'+str(parIdx) +
                         ':name').putData(paramDict['name'])
            self.getNode('parameters.par_'+str(parIdx) +
                         ':value').putData(paramDict['value'])
            parIdx += 1

        inputIdx = 1
        inputDicts = GetSignalData(RootInputStruct,  numInputs)
        for inputDict in inputDicts:
            self.getNode('inputs.in'+str(inputIdx) +
                         ':name').putData(inputDict['name'])
            self.getNode('inputs.in'+str(inputIdx) +
                         ':type').putData(inputDict['type'])
            inDim = inputDict['dimensions']
            if inDim != 0:
                inDim = MDSplus.Int32Array(inDim)
            self.getNode('inputs.in'+str(inputIdx) +
                         ':dimensions').putData(inDim)
            inputIdx += 1

        outputIdx = 1
        outputDicts = GetSignalData(RootOutputStruct,  numOutputs)
        for outputDict in outputDicts:
            self.getNode('outputs.out'+str(outputIdx) +
                         ':name').putData(outputDict['name'])
            self.getNode('outputs.out'+str(outputIdx) +
                         ':type').putData(outputDict['type'])
            outDim = outputDict['dimensions']
            if outDim != 0:
                outDim = MDSplus.Int32Array(outDim)
            self.getNode('outputs.out'+str(outputIdx) +
                         ':dimensions').putData(outDim)
            outputIdx += 1
