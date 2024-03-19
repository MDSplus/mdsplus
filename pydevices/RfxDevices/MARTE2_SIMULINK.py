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
import ctypes

MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_SIMULINK(MC.MARTE2_COMPONENT):
    pass

# TODO: device class need <lib_name>.so to get node struct,
#       thats bad without <lib_name>.so


def BUILDER(cls):
    try:
        ModelLib = ctypes.cdll.LoadLibrary(cls.lib_name + '.so')
    except OSError:
        cls.inputs = []
        cls.outputs = []
        cls.parameters = []
    else:
        initializeFuncName = cls.lib_name + '_initialize'

        # By default functions are assumed to return the C int type.
        # Other return types can be specified by setting the restype attribute.
        InstFunc = getattr(ModelLib, cls.lib_name)
        InstFunc.argtypes = []
        InstFunc.restype = ctypes.POINTER(None)
        states = InstFunc()

        GetMmiPtr = getattr(ModelLib, cls.lib_name+'_GetCAPImmi')
#      ModelLib.GetMmiPtr.argtypes = []
#      ModelLib.GetMmiPtr.restype = ctypes.POINTER(None)
#      GetMmiPtr.argtypes = []
        GetMmiPtr.argtypes = [ctypes.POINTER(None)]
        GetMmiPtr.restype = ctypes.POINTER(None)
#      mmi = ModelLib.GetMmiPtr()
        mmi = GetMmiPtr(states)

# Initialization function is called with a dynamically-specified name
#        InitializeFunc = getattr(ModelLib, initializeFuncName)
#        InitializeFunc.argtypes = [ctypes.POINTER(None)]
#        InitializeFunc(states)

        WrapperLib = ctypes.cdll.LoadLibrary("rtw_capi_wrapper.so")

        # ** GENERAL MODEL DATA **

        # Number of inputs
        WrapperLib.WCAPI_GetNumRootInputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumRootInputs.restype = ctypes.c_int
        numInputs = WrapperLib.WCAPI_GetNumRootInputs(mmi)

        # Number of outputs
        WrapperLib.WCAPI_GetNumRootOutputs.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumRootOutputs.restype = ctypes.c_int
        numOutputs = WrapperLib.WCAPI_GetNumRootOutputs(mmi)

        # Number of parameters
        WrapperLib.WCAPI_GetNumModelParameters.argtypes = [
            ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetNumModelParameters.restype = ctypes.c_int
        numParameters = WrapperLib.WCAPI_GetNumModelParameters(mmi)

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

        # ElementType structure
        WrapperLib.WCAPI_GetElementMap.argtypes = [ctypes.POINTER(None)]
        WrapperLib.WCAPI_GetElementMap.restype = ctypes.POINTER(None)
        ElementMap = WrapperLib.WCAPI_GetElementMap(mmi)

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
            ctypes.POINTER(None), ctypes.c_uint]
        WrapperLib.WCAPI_GetDataTypeSLId.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataEnumStorageType.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataEnumStorageType.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataTypeCName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeCName.restype = ctypes.c_char_p

        WrapperLib.WCAPI_GetDataTypeMWName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeMWName.restype = ctypes.c_char_p

        WrapperLib.WCAPI_GetDataTypeNumElements.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeNumElements.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetDataTypeElemMapIndex.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetDataTypeElemMapIndex.restype = ctypes.c_uint

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

        # 4. struct
        WrapperLib.WCAPI_GetElementName.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetElementName.restype = ctypes.c_char_p

        WrapperLib.WCAPI_GetElementOffset.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetElementOffset.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetElementDataTypeIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetElementDataTypeIdx.restype = ctypes.c_uint

        WrapperLib.WCAPI_GetElementDimensionIdx.argtypes = [
            ctypes.POINTER(None), ctypes.c_int]
        WrapperLib.WCAPI_GetElementDimensionIdx.restype = ctypes.c_uint16

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
                retrievedName = retrievedName.replace(' ','_')  #remove blacks
                #print('SIGNAL NAME: ' + retrievedName);
               # retrievedName = removeAngular(retrievedName)

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
                    MARTe2Typename = 'uint8'
                elif retrievedSLIdType == 254:
                    fieldEnumType =  WrapperLib.WCAPI_GetDataEnumStorageType(DataTypeMap, retrievedTypeIdx)
                    if fieldEnumType == 0:
                        MARTe2Typename = 'float64'
                    elif fieldEnumType == 1:
                        MARTe2Typename = 'float32'
                    elif fieldEnumType == 2:
                        MARTe2Typename = 'int8'
                    elif fieldEnumType == 3:
                        MARTe2Typename = 'uint8'
                    elif fieldEnumType == 4:
                        MARTe2Typename = 'int16'
                    elif fieldEnumType == 5:
                        MARTe2Typename = 'uint16'
                    elif fieldEnumType == 6:
                        MARTe2Typename = 'int32'
                    elif fieldEnumType == 7:
                        MARTe2Typename = 'uint32'
                    else:
                        print('type '+str(fieldEnumType))
                        raise Exception('Unsupported Enum datatype.')
                elif retrievedSLIdType == 255:
                    MARTe2Typename = WrapperLib.WCAPI_GetDataTypeMWName(
                        DataTypeMap, retrievedTypeIdx)
                    MARTe2Typename = MARTe2Typename.decode("utf-8")
                else:
                    raise Exception('Unsupported datatype.')

                if retrievedSLIdType != 255:

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
                        currDimension.append(
                            DimensionArray[dimArrayIdx + currIdx])
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
                else:  # struct type
                    fieldDicts = []
                    numFields = WrapperLib.WCAPI_GetDataTypeNumElements(
                        DataTypeMap, retrievedTypeIdx)
                    elementMapIndex = WrapperLib.WCAPI_GetDataTypeElemMapIndex(
                        DataTypeMap, retrievedTypeIdx)
                    for fieldIdx in range(numFields):
                        fieldName = WrapperLib.WCAPI_GetElementName(
                            ElementMap, elementMapIndex + fieldIdx)
                        fieldName = fieldName.decode("utf-8")
               #     fieldName = removeAngular(fieldName)
               # type is retrieved
                        fieldTypeIdx = WrapperLib.WCAPI_GetElementDataTypeIdx(
                            ElementMap, elementMapIndex + fieldIdx)
                        fieldSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                            DataTypeMap, fieldTypeIdx)
                        if fieldSLIdType == 0:
                            fieldMARTe2Typename = 'float64'
                        elif fieldSLIdType == 1:
                            fieldMARTe2Typename = 'float32'
                        elif fieldSLIdType == 2:
                            fieldMARTe2Typename = 'int8'
                        elif fieldSLIdType == 3:
                            fieldMARTe2Typename = 'uint8'
                        elif fieldSLIdType == 4:
                            fieldMARTe2Typename = 'int16'
                        elif fieldSLIdType == 5:
                            fieldMARTe2Typename = 'uint16'
                        elif fieldSLIdType == 6:
                            fieldMARTe2Typename = 'int32'
                        elif fieldSLIdType == 7:
                            fieldMARTe2Typename = 'uint32'
                        elif fieldSLIdType == 8:
                            fieldMARTe2Typename = 'uint8'
                        elif fieldSLIdType == 254:
                            fieldEnumType =  WrapperLib.WCAPI_GetDataEnumStorageType(DataTypeMap, fieldTypeIdx)
                            if fieldEnumType == 0:
                                fieldMARTe2Typename = 'float64'
                            elif fieldEnumType == 1:
                                fieldMARTe2Typename = 'float32'
                            elif fieldEnumType == 2:
                                fieldMARTe2Typename = 'int8'
                            elif fieldEnumType == 3:
                                fieldMARTe2Typename = 'uint8'
                            elif fieldEnumType == 4:
                                fieldMARTe2Typename = 'int16'
                            elif fieldEnumType == 5:
                                fieldMARTe2Typename = 'uint16'
                            elif fieldEnumType == 6:
                                fieldMARTe2Typename = 'int32'
                            elif fieldEnumType == 7:
                                fieldMARTe2Typename = 'uint32'
                            else:
                                print('type '+str(fieldEnumType))
                                raise Exception('Unsupported Enum datatype.')
                        else:
                            print('type '+str(fieldSLIdType))
                            raise Exception('Unsupported datatype.')
                # field dimensions
                       # dimensions are retrieved
                        fieldDimIdx = WrapperLib.WCAPI_GetElementDimensionIdx(
                            ElementMap, elementMapIndex + fieldIdx)

                        fieldDimArrayIdx = WrapperLib.WCAPI_GetDimArrayIndex(
                            DimensionMap, fieldDimIdx)  # Starting position in the dimensionArray
                        # Number of elements in the dimensionArray referring to this signal
                        fieldDimNum = WrapperLib.WCAPI_GetNumDims(
                            DimensionMap, fieldDimIdx)

                        currDimension = []
                        for currIdx in range(fieldDimNum):
                            currDimension.append(
                                DimensionArray[fieldDimArrayIdx + currIdx])
                        if currDimension[0] == 1 and currDimension[1] == 1:
                            fieldDimension = 0
                        elif currDimension[0] == 1 or currDimension[1] == 1:
                            fieldDimension = [
                                currDimension[0]*currDimension[1]]
                        else:
                            fieldDimension = currDimension

                        fieldDicts.append(
                            dict(name=fieldName, type=fieldMARTe2Typename, dimensions=fieldDimension))
                # retrieved data is saved to a dictionary
                    signalDict = dict(name=retrievedName, type=MARTe2Typename,
                                      dimensions=0, parameters={}, fields=fieldDicts)

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
                #print('retrievedname: ', retrievedName)
                # type is retrieved
                retrievedTypeIdx = WrapperLib.WCAPI_GetModelParameterDataTypeIdx(
                    ParameterStruct, paramIdx)
                #print('retrievedTypeIdx: ', str(retrievedTypeIdx))
                retrievedSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                    DataTypeMap, retrievedTypeIdx)
                #print('retrievedSLIdType: ', str(retrievedSLIdType))
                #retrievedCTypename = WrapperLib.WCAPI_GetDataTypeCName(
                #    DataTypeMap, retrievedTypeIdx)
                #retrievedCTypename = retrievedCTypename.decode("utf-8")
                #print('retrievedCTypename: '+retrievedCTypename)

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
                    MARTe2Typename = 'uint8'
                    pythonTypename = ctypes.c_bool
                elif retrievedSLIdType == 254:
                    fieldEnumType =  WrapperLib.WCAPI_GetDataEnumStorageType(DataTypeMap, retrievedTypeIdx)
                    if fieldEnumType == 0:
                        MARTe2Typename = 'float64'
                        pythonTypename = ctypes.c_double
                    elif fieldEnumType == 1:
                        MARTe2Typename = 'float32'
                        pythonTypename = ctypes.c_float
                    elif fieldEnumType == 2:
                        MARTe2Typename = 'int8'
                        pythonTypename = ctypes.c_int8
                    elif fieldEnumType == 3:
                        pythonTypename = ctypes.c_uint8
                        MARTe2Typename = 'uint8'
                    elif fieldEnumType == 4:
                        MARTe2Typename = 'int16'
                        pythonTypename = ctypes.c_int16
                    elif fieldEnumType == 5:
                        pythonTypename = ctypes.c_uint16
                        MARTe2Typename = 'uint16'
                    elif fieldEnumType == 6:
                        pythonTypename = ctypes.c_int32
                        MARTe2Typename = 'int32'
                    elif fieldEnumType == 7:
                        pythonTypename = ctypes.c_uint32
                        MARTe2Typename = 'uint32'
                    else:
                        print('type '+str(fieldEnumType))
                        raise Exception('Unsupported Enum datatype.')
                elif retrievedSLIdType == 255:
                    pass
                else:
                    raise Exception('Unsupported parameter datatype.')
                  
                if retrievedSLIdType != 255:  
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
                    elif currDimension[0] == 1 or currDimension[1] == 1:  # vector
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
                            for nCol in range(currDimension[1]):
                                valueRow.append(paramPointer[idx])
                                idx = idx + 1
                            valueList.append(valueRow)
                            valueRow = []

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
                else: #retrievedSLIdType == 255  SRUCTURED PARAMETERS
                    GetModelParameterFields(retrievedTypeIdx, ParameterStruct, paramIdx, 'Parameters.'+retrievedName, paramList, 1)  
                  
                    #numFields = WrapperLib.WCAPI_GetDataTypeNumElements(
                        #DataTypeMap, retrievedTypeIdx)
                    #elementMapIndex = WrapperLib.WCAPI_GetDataTypeElemMapIndex(
                        #DataTypeMap, retrievedTypeIdx)
                  ## actual parameter value is retrieved
                    #paramAddrIdx = WrapperLib.WCAPI_GetModelParameterAddrIdx(
                        #ParameterStruct, paramIdx)
                    #paramDataAddr = WrapperLib.WCAPI_GetDataAddress(
                        #DataAddrMap, paramAddrIdx)
                    #print('NUM FIELDS: '+str(numFields))
                    #for fieldIdx in range(numFields):
                        #fieldName = WrapperLib.WCAPI_GetElementName(
                            #ElementMap, elementMapIndex + fieldIdx)
                        #fieldName = fieldName.decode("utf-8")
                        #print('FIELD: '+fieldName)
                        #fieldOffset = WrapperLib.WCAPI_GetElementOffset(
                            #ElementMap, elementMapIndex + fieldIdx)
                         
                        #fieldTypeIdx = WrapperLib.WCAPI_GetElementDataTypeIdx(
                            #ElementMap, elementMapIndex + fieldIdx)
                        #fieldSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                            #DataTypeMap, fieldTypeIdx)
                        #if fieldSLIdType == 0:
                            #fieldMARTe2Typename = 'float64'
                            #fieldPythonTypename = ctypes.c_double
                        #elif fieldSLIdType == 1:
                            #fieldMARTe2Typename = 'float32'
                            #fieldPythonTypename = ctypes.c_float
                        #elif fieldSLIdType == 2:
                            #fieldMARTe2Typename = 'int8'
                            #fieldPythonTypename = ctypes.c_int8
                        #elif fieldSLIdType == 3:
                            #fieldMARTe2Typename = 'uint8'
                            #fieldPythonTypename = ctypes.c_uint8
                        #elif fieldSLIdType == 4:
                            #fieldMARTe2Typename = 'int16'
                            #fieldPythonTypename = ctypes.c_int16
                        #elif fieldSLIdType == 5:
                            #fieldMARTe2Typename = 'uint16'
                            #fieldPythonTypename = ctypes.c_uint16
                        #elif fieldSLIdType == 6:
                            #fieldMARTe2Typename = 'int32'
                            #fieldPythonTypename = ctypes.c_int32
                        #elif fieldSLIdType == 7:
                            #fieldMARTe2Typename = 'uint32'
                            #fieldPythonTypename = ctypes.c_uint32
                        #elif fieldSLIdType == 8:
                            #fieldMARTe2Typename = 'bool'
                            #fieldPythonTypename = ctypes.c_int8
                        #elif fieldSLIdType == 254:
                            #fieldEnumType =  WrapperLib.WCAPI_GetDataEnumStorageType(DataTypeMap, fieldTypeIdx)
                            #if fieldEnumType == 0:
                                #fieldMARTe2Typename = 'float64'
                                #fieldPythonTypename = ctypes.c_double
                            #elif fieldEnumType == 1:
                                #fieldMARTe2Typename = 'float32'
                                #fieldPythonTypename = ctypes.c_float
                            #elif fieldEnumType == 2:
                                #fieldMARTe2Typename = 'int8'
                                #fieldPythonTypename = ctypes.c_int8
                            #elif fieldEnumType == 3:
                                #fieldMARTe2Typename = 'uint8'
                                #fieldPythonTypename = ctypes.c_uint8
                            #elif fieldEnumType == 4:
                                #fieldMARTe2Typename = 'int16'
                                #fieldPythonTypename = ctypes.c_int16
                            #elif fieldEnumType == 5:
                                #fieldMARTe2Typename = 'uint16'
                                #fieldPythonTypename = ctypes.c_uint16
                            #elif fieldEnumType == 6:
                                #fieldMARTe2Typename = 'int32'
                                #fieldPythonTypename = ctypes.c_int32
                            #elif fieldEnumType == 7:
                                #fieldMARTe2Typename = 'uint32'
                                #fieldPythonTypename = ctypes.c_uint32
                            #else:
                                #print('type '+str(fieldEnumType))
                                #raise Exception('Unsupported Enum datatype.')
                        #else:
                            #print('type '+str(fieldSLIdType))
                            #raise Exception('Unsupported nested structures.')
                ## field dimensions
                       ## dimensions are retrieved
                        #fieldDimIdx = WrapperLib.WCAPI_GetElementDimensionIdx(
                            #ElementMap, elementMapIndex + fieldIdx)

                        #fieldDimArrayIdx = WrapperLib.WCAPI_GetDimArrayIndex(
                            #DimensionMap, fieldDimIdx)  # Starting position in the dimensionArray
                        ## Number of elements in the dimensionArray referring to this signal
                        #fieldDimNum = WrapperLib.WCAPI_GetNumDims(
                            #DimensionMap, fieldDimIdx)

                        #currDimension = []
                        #for currIdx in range(fieldDimNum):
                            #currDimension.append(
                                #DimensionArray[fieldDimArrayIdx + currIdx])
                        #if currDimension[0] == 1 and currDimension[1] == 1:
                            #fieldDimension = 0
                        #elif currDimension[0] == 1 or currDimension[1] == 1:
                            #fieldDimension = [
                                #currDimension[0]*currDimension[1]]
                        #else:
                            #fieldDimension = currDimension

                        #currParAddress = paramDataAddr + fieldOffset
                        #paramPointer = ctypes.cast(
                          #currParAddress, ctypes.POINTER(fieldPythonTypename))

 
                        #if currDimension[0] == 1 and currDimension[1] == 1:  # scalar
                            #dimension = 0
                            #mdsplusValue = paramPointer[0]
                        #elif currDimension[0] == 1:  # vector
                            #dimension = currDimension[0]*currDimension[1]
                            #valueList = []
                            #for idx in range(dimension):
                                #valueList.append(paramPointer[idx])
                        #else:
                            ## matrix or column vector (MARTe2 sees column vectors as matrices)
                            #dimension = currDimension
                            #idx = 0
                            #valueList = []
                            #valueRow = []
                            #for nRow in range(currDimension[0]):
                                #for nCol in range(currDimension[1]):
                                    #valueRow.append(paramPointer[idx])
                                    #idx = idx + 1
                                #valueList.append(valueRow)
                                #valueRow = []
                                
                        #if currDimension[0] != 1 or currDimension[1] != 1:
                            #if fieldSLIdType == 0:
                                #mdsplusValue = MDSplus.Float32Array(valueList)
                            #elif fieldSLIdType == 1:
                                #mdsplusValue = MDSplus.Float64Array(valueList)
                            #elif fieldSLIdType == 2:
                                #mdsplusValue = MDSplus.Int8Array(valueList)
                            #elif fieldSLIdType == 3:
                                #mdsplusValue = MDSplus.Uint8Array(valueList)
                            #elif fieldSLIdType == 4:
                                #mdsplusValue = MDSplus.Int16Array(valueList)
                            #elif fieldSLIdType == 5:
                                #mdsplusValue = MDSplus.Uint16Array(valueList)
                            #elif fieldSLIdType == 6:
                                #mdsplusValue = MDSplus.Int32Array(valueList)
                            #elif fieldSLIdType == 7:
                                #mdsplusValue = MDSplus.Uint32Array(valueList)
                            #else:
                                #raise Exception('Unsupported parameter datatype.')
                              
                        #paramDict = dict(name='Parameters.'+retrievedName+'-'+fieldName,
                                    #type=fieldMARTe2Typename, dimensions=fieldDimension, value=mdsplusValue)
                     ## dictionary is appended to the MDSplus-style list
                        #paramList.append(paramDict)
                 #endif STRUCTURED PARAMETER
            #endfor parameters
            #print('PARAMETRI FATTI')
            return paramList

#################################################################
#Recursive method for parameter structures
##################################################################          
        def GetModelParameterFields(structTypeIdx, ParameterStruct, paramIdx, baseParName, paramList, recLev  ):
            if recLev == 10:
              raise Exception
            
            numFields = WrapperLib.WCAPI_GetDataTypeNumElements(
                DataTypeMap, structTypeIdx)
            elementMapIndex = WrapperLib.WCAPI_GetDataTypeElemMapIndex(
                DataTypeMap, structTypeIdx)
          # actual parameter value is retrieved
            paramAddrIdx = WrapperLib.WCAPI_GetModelParameterAddrIdx(
                ParameterStruct, paramIdx)
            paramDataAddr = WrapperLib.WCAPI_GetDataAddress(
                DataAddrMap, paramAddrIdx)
            #print('NUM FIELDS: '+str(numFields))
            for fieldIdx in range(numFields):
                fieldName = WrapperLib.WCAPI_GetElementName(
                    ElementMap, elementMapIndex + fieldIdx)
                fieldName = fieldName.decode("utf-8")
                #print('FIELD: '+baseParName+'-'+fieldName)
                fieldOffset = WrapperLib.WCAPI_GetElementOffset(
                    ElementMap, elementMapIndex + fieldIdx)
                  
                fieldTypeIdx = WrapperLib.WCAPI_GetElementDataTypeIdx(
                    ElementMap, elementMapIndex + fieldIdx)
                #print('fieldTypeIdx: '+str(fieldTypeIdx))
                fieldSLIdType = WrapperLib.WCAPI_GetDataTypeSLId(
                    DataTypeMap, fieldTypeIdx)
                #print('fieldSLIdType: '+str(fieldSLIdType))
                if fieldSLIdType == 0:
                    fieldMARTe2Typename = 'float64'
                    fieldPythonTypename = ctypes.c_double
                elif fieldSLIdType == 1:
                    fieldMARTe2Typename = 'float32'
                    fieldPythonTypename = ctypes.c_float
                elif fieldSLIdType == 2:
                    fieldMARTe2Typename = 'int8'
                    fieldPythonTypename = ctypes.c_int8
                elif fieldSLIdType == 3:
                    fieldMARTe2Typename = 'uint8'
                    fieldPythonTypename = ctypes.c_uint8
                elif fieldSLIdType == 4:
                    fieldMARTe2Typename = 'int16'
                    fieldPythonTypename = ctypes.c_int16
                elif fieldSLIdType == 5:
                    fieldMARTe2Typename = 'uint16'
                    fieldPythonTypename = ctypes.c_uint16
                elif fieldSLIdType == 6:
                    fieldMARTe2Typename = 'int32'
                    fieldPythonTypename = ctypes.c_int32
                elif fieldSLIdType == 7:
                    fieldMARTe2Typename = 'uint32'
                    fieldPythonTypename = ctypes.c_uint32
                elif fieldSLIdType == 8:
                    fieldMARTe2Typename = 'uint8'
                    fieldPythonTypename = ctypes.c_int8
                elif fieldSLIdType == 254:
                    fieldEnumType =  WrapperLib.WCAPI_GetDataEnumStorageType(DataTypeMap, fieldTypeIdx)
                    if fieldEnumType == 0:
                        fieldMARTe2Typename = 'float64'
                        fieldPythonTypename = ctypes.c_double
                    elif fieldEnumType == 1:
                        fieldMARTe2Typename = 'float32'
                        fieldPythonTypename = ctypes.c_float
                    elif fieldEnumType == 2:
                        fieldMARTe2Typename = 'int8'
                        fieldPythonTypename = ctypes.c_int8
                    elif fieldEnumType == 3:
                        fieldMARTe2Typename = 'uint8'
                        fieldPythonTypename = ctypes.c_uint8
                    elif fieldEnumType == 4:
                        fieldMARTe2Typename = 'int16'
                        fieldPythonTypename = ctypes.c_int16
                    elif fieldEnumType == 5:
                        fieldMARTe2Typename = 'uint16'
                        fieldPythonTypename = ctypes.c_uint16
                    elif fieldEnumType == 6:
                        fieldMARTe2Typename = 'int32'
                        fieldPythonTypename = ctypes.c_int32
                    elif fieldEnumType == 7:
                        fieldMARTe2Typename = 'uint32'
                        fieldPythonTypename = ctypes.c_uint32
                    else:
                        print('type '+str(fieldEnumType))
                        raise Exception('Unsupported Enum datatype.')
                else: #NESTED STRUCTURES!!!!!!!!!!!!!!!!
                    #print('TYPE '+str(fieldSLIdType) + '   '+str(fieldTypeIdx))
                    GetModelParameterFields(fieldTypeIdx, ParameterStruct, paramIdx, baseParName + '-'+fieldName, paramList, recLev+1)
                    continue #no direct data associated
        # field dimensions
                # dimensions are retrieved
                fieldDimIdx = WrapperLib.WCAPI_GetElementDimensionIdx(
                    ElementMap, elementMapIndex + fieldIdx)

                fieldDimArrayIdx = WrapperLib.WCAPI_GetDimArrayIndex(
                    DimensionMap, fieldDimIdx)  # Starting position in the dimensionArray
                # Number of elements in the dimensionArray referring to this signal
                fieldDimNum = WrapperLib.WCAPI_GetNumDims(
                    DimensionMap, fieldDimIdx)
                #print('fieldDimNum: '+ str(fieldDimNum))
                currDimension = []
                for currIdx in range(fieldDimNum):
                    currDimension.append(
                        DimensionArray[fieldDimArrayIdx + currIdx])
                if currDimension[0] == 1 and currDimension[1] == 1:
                    fieldDimension = 0
                elif currDimension[0] == 1 or currDimension[1] == 1:
                    fieldDimension = [
                        currDimension[0]*currDimension[1]]
                else:
                    fieldDimension = currDimension

                currParAddress = paramDataAddr + fieldOffset
                paramPointer = ctypes.cast(
                  currParAddress, ctypes.POINTER(fieldPythonTypename))


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
                        for nCol in range(currDimension[1]):
                            valueRow.append(paramPointer[idx])
                            idx = idx + 1
                        valueList.append(valueRow)
                        valueRow = []
                        
                if currDimension[0] != 1 or currDimension[1] != 1:
                    if fieldSLIdType == 0:
                        mdsplusValue = MDSplus.Float32Array(valueList)
                    elif fieldSLIdType == 1:
                        mdsplusValue = MDSplus.Float64Array(valueList)
                    elif fieldSLIdType == 2:
                        mdsplusValue = MDSplus.Int8Array(valueList)
                    elif fieldSLIdType == 3:
                        mdsplusValue = MDSplus.Uint8Array(valueList)
                    elif fieldSLIdType == 4:
                        mdsplusValue = MDSplus.Int16Array(valueList)
                    elif fieldSLIdType == 5:
                        mdsplusValue = MDSplus.Uint16Array(valueList)
                    elif fieldSLIdType == 6:
                        mdsplusValue = MDSplus.Int32Array(valueList)
                    elif fieldSLIdType == 7:
                        mdsplusValue = MDSplus.Uint32Array(valueList)
                    elif fieldSLIdType == 8:
                        mdsplusValue = MDSplus.Uint8Array(valueList)
                    else:
                        raise Exception('Unsupported parameter datatype.')
                      
                paramDict = dict(name=baseParName+'-'+fieldName,
                            type=fieldMARTe2Typename, dimensions=fieldDimension, value=mdsplusValue)
              # dictionary is appended to the MDSplus-style list
                paramList.append(paramDict)
          #end GetModelParameterFields

        # First parameter should be the model name
#      modelNameDict = dict(name = 'ModelName', type = 'string', dimensions = 0, value = cls.lib_name)
        symbolPrefixDict = dict(
            name='SymbolPrefix', type='string', dimensions=0, value=cls.lib_name)
        libraryDict = dict(name='Library', type='string',
                           dimensions=0, value=cls.lib_name+'.so')
        verbosityDict = dict(
            name='Verbosity', type='numeric', dimensions=0, value=2)
        busModeDict = dict(name='NonVirtualBusMode',
                           type='string', value='Structured', dimensions=0)

        modelParameters = GetModelParameterData(ParameterStruct, numParameters)
        modelParameters.insert(0, symbolPrefixDict)
        modelParameters.insert(0, libraryDict)
        modelParameters.insert(0, verbosityDict)
        modelParameters.insert(0, busModeDict)

        cls.inputs = GetSignalData(RootInputStruct,  numInputs)
        cls.outputs = GetSignalData(RootOutputStruct, numOutputs)
        cls.parameters = modelParameters

    cls.parts = []
    cls.buildGam(cls.parts, 'SimulinkWrapperGAM', cls.MODE_GAM)
    return cls
