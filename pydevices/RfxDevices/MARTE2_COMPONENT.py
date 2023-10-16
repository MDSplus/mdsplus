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

from MDSplus import Device, Data, TreeNode, Range, TreePath, Int8, Uint8, Int16, Uint16, Int32, Uint32, Int64, Uint64, Float32, Float64, Int8Array, Uint8Array, Int16Array, Uint16Array, Int32Array, Uint32Array, Int64Array, Uint64Array, Float32Array, Float64Array
import numpy as np


class MARTE2_COMPONENT(Device):
    """MARTE2 components superclass"""
    MODE_GAM = 1
    MODE_INPUT = 2
    MODE_SYNCH_INPUT = 3
    MODE_OUTPUT = 4

    TIMEBASE_GENERATOR = 1
    TIMEBASE_FROM_SAME_THREAD = 2
    TIMEBASE_FROM_ANOTHER_THREAD = 2

    @classmethod
    def buildParameters(cls, parts):
        parts.append({'path': '.PARAMETERS', 'type': 'structure'})
        idx = 1
        for parameter in cls.parameters:
            parts.append({'path': '.PARAMETERS.PAR_' +
                          str(idx), 'type': 'structure'})
            parts.append({'path': '.PARAMETERS.PAR_'+str(idx) +
                          ':NAME', 'type': 'text', 'value': parameter['name']})
            if(parameter['type'] == 'string'):
                if 'value' in parameter:
                    parts.append({'path': '.PARAMETERS.PAR_'+str(idx) +
                                  ':VALUE', 'type': 'text', 'value': parameter['value']})
                else:
                    parts.append({'path': '.PARAMETERS.PAR_' +
                                  str(idx)+':VALUE', 'type': 'text'})
            else:
                if 'value' in parameter:
                    parts.append({'path': '.PARAMETERS.PAR_'+str(idx)+':VALUE',
                                  'type': 'numeric', 'value': parameter['value']})
                else:
                    parts.append({'path': '.PARAMETERS.PAR_' +
                                  str(idx)+':VALUE', 'type': 'numeric'})
            idx = idx+1

    @classmethod
    def buildOutputs(cls, parts):
        parts.append({'path': '.OUTPUTS', 'type': 'structure'})
        parts.append({'path': '.OUTPUTS:TRIGGER', 'type': 'numeric'})
        parts.append({'path': '.OUTPUTS:PRE_TRIGGER',
                      'type': 'numeric', 'value': 0})
        parts.append({'path': '.OUTPUTS:POST_TRIGGER',
                      'type': 'numeric', 'value': 100})
        # reference time for the device valid for all devices except SynchInput
        parts.append({'path': '.OUTPUTS:OUT_TIME', 'type': 'signal'})
        # Used only by SynchInput devices to identify which output is the time
        parts.append({'path': '.OUTPUTS:TIME_IDX',
                      'type': 'numeric', 'value': 0})
        # CPU Mask for MdsWriter thread
        parts.append({'path': '.OUTPUTS:CPU_MASK',
                      'type': 'numeric', 'value': 15})
        idx = 1
        nameList = []
        for output in cls.outputs:
            #        sigName = output['name'][0:12]
            sigName = cls.convertName(output['name'], nameList)
            parts.append({'path': '.OUTPUTS.'+sigName, 'type': 'structure'})
            parts.append({'path': '.OUTPUTS.'+sigName+':NAME',
                          'type': 'text', 'value': output['name']})
            parts.append({'path': '.OUTPUTS.'+sigName+':TYPE',
                          'type': 'text', 'value': output['type']})
            parts.append({'path': '.OUTPUTS.'+sigName+':SAMPLES',
                          'type': 'numeric', 'value': 1})
            parts.append({'path': '.OUTPUTS.'+sigName+':DIMENSIONS',
                          'type': 'numeric', 'value': Data.compile(str(output['dimensions']))})
            if 'seg_len' in output:
                parts.append({'path': '.OUTPUTS.'+sigName+':SEG_LEN',
                              'type': 'numeric', 'value': output['seg_len']})
            else:
                parts.append({'path': '.OUTPUTS.'+sigName +
                              ':SEG_LEN', 'type': 'numeric', 'value': 0})
            if 'stream' in output:
                parts.append({'path': '.OUTPUTS.'+sigName+':STREAM',
                              'type': 'text', 'value': output['stream']})
            else:
                parts.append({'path': '.OUTPUTS.'+sigName +
                              ':STREAM', 'type': 'text'})

            if(output['type'] == 'string'):
                parts.append(
                    {'path': '.OUTPUTS.'+sigName+':VALUE', 'type': 'text'})
            else:
                parts.append({'path': '.OUTPUTS.'+sigName +
                              ':VALUE', 'type': 'signal'})
            pars = output['parameters']
            idx = 1
            parts.append({'path': '.OUTPUTS.'+sigName +
                          '.PARAMETERS', 'type': 'structure'})
            if len(pars) > 0:
                for par in pars:
                    parts.append(
                        {'path': '.OUTPUTS.'+sigName+'.PARAMETERS.PAR_'+str(idx), 'type': 'structure'})
                    parts.append({'path': '.OUTPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                  str(idx)+':NAME', 'type': 'text', 'value': par['name']})
                    if(par['type'] == 'string'):
                        parts.append({'path': '.OUTPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                      str(idx)+':VALUE', 'type': 'text', 'value': par['value']})
                    else:
                        parts.append({'path': '.OUTPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                      str(idx)+':VALUE', 'type': 'numeric', 'value': par['value']})

                    idx = idx + 1
            # structured outputs
            try:
                fields = output['fields']
            except:
                fields = []
            if len(fields) > 0:
                parts.append({'path': '.OUTPUTS.'+sigName +
                              '.FIELDS', 'type': 'structure'})
                for field in fields:
                    fieldName = cls.convertName(field['name'], nameList)
                    parts.append({'path': '.OUTPUTS.'+sigName +
                                  '.FIELDS.'+fieldName, 'type': 'structure'})
                    parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.' +
                                  fieldName+':NAME', 'type': 'text', 'value': field['name']})
                    parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.' +
                                  fieldName+':TYPE', 'type': 'text', 'value': field['type']})
                    parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.' +
                                  fieldName+':SAMPLES', 'type': 'numeric', 'value': 1})
                    parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.'+fieldName+':DIMENSIONS',
                                  'type': 'numeric', 'value': Data.compile(str(field['dimensions']))})
                    if 'seg_len' in field:
                        parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.'+fieldName +
                                      ':SEG_LEN', 'type': 'numeric', 'value': field['seg_len']})
                    else:
                        parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.' +
                                      fieldName+':SEG_LEN', 'type': 'numeric', 'value': 0})
                    if 'stream' in field:
                        parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.'+fieldName +
                                      ':STREAM', 'type': 'text', 'value': field['stream']})
                    else:
                        parts.append({'path': '.OUTPUTS.'+sigName+'.FIELDS.' +
                                      fieldName+':STREAM', 'type': 'text'})

                    if(field['type'] == 'string'):
                        parts.append(
                            {'path': '.OUTPUTS.'+sigName+'.FIELDS.'+fieldName+':VALUE', 'type': 'text'})
                    else:
                        parts.append(
                            {'path': '.OUTPUTS.'+sigName+'.FIELDS.'+fieldName+':VALUE', 'type': 'signal'})

    @classmethod
    def buildInputs(cls, parts):
        parts.append({'path': '.INPUTS', 'type': 'structure'})
        idx = 1
        nameList = []
        for input in cls.inputs:
            #        sigName = input['name'][0:12]
            sigName = cls.convertName(input['name'], nameList)
            parts.append({'path': '.INPUTS.'+sigName, 'type': 'structure'})
            parts.append({'path': '.INPUTS.'+sigName+':TYPE',
                          'type': 'text', 'value': input['type']})
            parts.append({'path': '.INPUTS.'+sigName+':DIMENSIONS',
                          'type': 'numeric', 'value': Data.compile(str(input['dimensions']))})
            parts.append({'path': '.INPUTS.'+sigName+':COL_ORDER',
                          'type': 'text', 'value': 'NO'})
            parts.append({'path': '.INPUTS.'+sigName +
                          ':VALUE', 'type': 'signal'})
            if 'name' in input:
                parts.append({'path': '.INPUTS.'+sigName+':NAME',
                              'type': 'text', 'value': input['name']})
            else:
                parts.append(
                    {'path': '.INPUTS.'+sigName+':NAME', 'type': 'text'})

            pars = input['parameters']
            idx = 1
            parts.append({'path': '.INPUTS.'+sigName +
                          '.PARAMETERS', 'type': 'structure'})
            if len(pars) > 0:
                for par in pars:
                    parts.append(
                        {'path': '.INPUTS.'+sigName+'.PARAMETERS.PAR_'+str(idx), 'type': 'structure'})
                    parts.append({'path': '.INPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                  str(idx)+':NAME', 'type': 'text', 'value': par['name']})
                    if(par['type'] == 'string'):
                        parts.append({'path': '.INPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                      str(idx)+':VALUE', 'type': 'text', 'value': par['value']})
                    else:
                        parts.append({'path': '.INPUTS.'+sigName+'.PARAMETERS.PAR_' +
                                      str(idx)+':VALUE', 'type': 'numeric', 'value': par['value']})
                    idx = idx + 1

            try:
                fields = input['fields']
            except:
                fields = []
            if len(fields) > 0:
                parts.append({'path': '.INPUTS.'+sigName +
                              '.FIELDS', 'type': 'structure'})
                for field in fields:
                    fieldName = cls.convertName(field['name'], nameList)
                    parts.append({'path': '.INPUTS.'+sigName +
                                  '.FIELDS.'+fieldName, 'type': 'structure'})
                    parts.append({'path': '.INPUTS.'+sigName+'.FIELDS.' +
                                  fieldName+':TYPE', 'type': 'text', 'value': field['type']})
                    parts.append({'path': '.INPUTS.'+sigName+'.FIELDS.'+fieldName+':DIMENSIONS',
                                  'type': 'numeric', 'value': Data.compile(str(field['dimensions']))})
                    parts.append({'path': '.INPUTS.'+sigName+'.FIELDS.' +
                                  fieldName+':COL_ORDER', 'type': 'text', 'value': 'NO'})
                    if 'name' in field:
                        parts.append({'path': '.INPUTS.'+sigName+'.FIELDS.' +
                                      fieldName+':NAME', 'type': 'text', 'value': field['name']})
                    else:
                        parts.append(
                            {'path': '.INPUTS.'+sigName+'.FIELDS.'+fieldName+':NAME', 'type': 'text'})
                    parts.append(
                        {'path': '.INPUTS.'+sigName+'.FIELDS.'+fieldName+':VALUE', 'type': 'signal'})

    @classmethod
    def buildGam(cls, parts, clazz, mode, timebaseExpr=None):
        parts.append({'path': ':GAM_CLASS', 'type': 'text', 'value': clazz})
        parts.append({'path': ':MODE', 'type': 'numeric', 'value': mode})
        if timebaseExpr == None:
            parts.append({'path': ':TIMEBASE', 'type': 'numeric'})
        else:
            parts.append({'path': ':TIMEBASE', 'type': 'numeric',
                          'value': Data.compile(timebaseExpr)})

        cls.buildParameters(parts)
        if mode == MARTE2_COMPONENT.MODE_GAM or mode == MARTE2_COMPONENT.MODE_OUTPUT:
            cls.buildInputs(parts)

        if mode != MARTE2_COMPONENT.MODE_OUTPUT:
            cls.buildOutputs(parts)

# add timebase division, valid only if the timebase refers to a MASRTE2 device belonging to a different thread
        parts.append({'path': ':TIMEBASE_DIV', 'type': 'numeric'})
# add debug definition. When 'ENABLED' input and outputs will be printed via LoggetDataSOurce
        parts.append(
            {'path': ':PRINT_DEBUG', 'type': 'text', 'value': 'DISABLED'})

#        for part in parts:
#          print(part)

    name = None
    inputs = []
    outputs = []
    parameters = []

    def convertVal(self, valStr):
        # distantiate [ and ]
        currValStr = ''
        for char in valStr:
            if char == '[' or char == ']':
                currValStr += ' '+char+' '
            else:
                currValStr += char
        tokens = currValStr.split()
        if len(tokens) <= 1:
            return valStr
        outVal = ''
        tokCounts = [0]
        for token in tokens:
            if token[0] == '[':
                if tokCounts[-1] > 0:
                    outVal += ','
                outVal += '{'
                tokCounts.append(0)
            elif token == ']':
                outVal += '}'
                tokCounts.pop()
                tokCounts[-1] = tokCounts[-1] + 1
            else:
                if tokCounts[-1] > 0:
                    outVal += ','
                outVal += token
                tokCounts[-1] = tokCounts[-1] + 1
        return outVal

    def convertPath(self, path):
        name = path[path.find('::TOP')+6:]
        name = name.replace(':', '_')
        name = name.replace('.', '_')
        return name

    def addSignalParametersOLD(self, parsNode, text):
        for parNode in parsNode.getChildren():
            name = parNode.getNode(':NAME').data()
            value = parNode.getNode(':VALUE').data()
            if parNode.getNode(':VALUE').getUsage() == 'TEXT':
                text += '        '+name+' = "'+str(value)+'"\n'
            else:
                text += '        '+name+' = '+str(value)+'\n'
        return text

    def addSignalParameters(self, parsNode, text):
        paramDicts = []
        for parNode in parsNode.getChildren():
            name = parNode.getNode(':NAME').data()
            # value = parNode.getNode(':VALUE').data()
            value = parNode.getNode(':VALUE').getData()
            if parNode.getNode(':VALUE').getUsage() == 'TEXT':
                paramDicts.append(
                    {'name': name, 'value': value, 'is_text': True})
            else:
                paramDicts.append(
                    {'name': name, 'value': value, 'is_text': False})

        text = self.reportParameters(paramDicts, text, 2)
        return text

    def reportParameters(self, paramDicts, outText, nTabs=1):
        np.set_printoptions(threshold=np.inf)
        rootParDict = {}
        for paramDict in paramDicts:
            currName = paramDict['name']
            names = currName.split('.')
            if len(names) == 1:  # resolve current level of parameters
                for tab in range(nTabs):
                    outText += '    '
                if paramDict['is_text']:
                    outText += paramDict['name']+' = "' + \
                        str(paramDict['value'])+'"\n'
                else:
                    if isinstance(paramDict['value'], Int8) or isinstance(paramDict['value'], Int8Array):
                        outText += paramDict['name']+' = (int8)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Uint8) or isinstance(paramDict['value'], Uint8Array):
                        outText += paramDict['name']+' = (uint8)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Int16) or isinstance(paramDict['value'], Int16Array):
                        outText += paramDict['name']+' = (int16)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Uint16) or isinstance(paramDict['value'], Uint16Array):
                        outText += paramDict['name']+' = (uint16)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Int32) or isinstance(paramDict['value'], Int32Array):
                        outText += paramDict['name']+' = (int32)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Uint32) or isinstance(paramDict['value'], Uint32Array):
                        outText += paramDict['name']+' = (uint32)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Int64) or isinstance(paramDict['value'], Int64Array):
                        outText += paramDict['name']+' = (int64)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Uint64) or isinstance(paramDict['value'], Uint64Array):
                        outText += paramDict['name']+' = (uint64)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    elif isinstance(paramDict['value'], Float32) or isinstance(paramDict['value'], Float32Array):
                        outText += paramDict['name']+' = (float32)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
                    else:
                        outText += paramDict['name']+' = (float64)'+self.convertVal(
                            str(paramDict['value'].data()))+'\n'
            else:
                if names[0] in rootParDict:
                    rootParDict[names[0]].append({'name': currName[currName.find(
                        '.')+1:], 'is_text': paramDict['is_text'], 'value': paramDict['value']})
                else:
                    rootParDict[names[0]] = [{'name': currName[currName.find(
                        '.')+1:], 'is_text': paramDict['is_text'], 'value': paramDict['value']}]
        for key in rootParDict.keys():
            for tab in range(nTabs):
                outText += '   '
            outText += key+' = {\n'
            outText = self.reportParameters(
                rootParDict[key], outText, nTabs + 1)
            for tab in range(nTabs):
                outText += '   '
            outText += '}\n'
        return outText

    @classmethod
    def convertName(cls, name, nameList):
        # remove angulars (possibly from simuluink)
        if name[0] == '<':
            name = name[1:-1]
        if len(name) <= 12:
            nameList.append(name)
            return name
        name1 = name[:12]
        if not name1 in nameList:
            nameList.append(name1)
            return name1
        i = 0
        while name1 in nameList:
            name1 = name[:10]+str(i)
            i = i+1
        nameList.append(name1)
        return name1

    def getGamInfo(self):
        try:
            isDebug = (self.print_debug.data().upper() == 'ENABLED')
        except:
            isDebug = False
        try:
            timebase = self.timebase.getData()
        except:
            timebase = None
        mode = self.mode.getData()
        gamName = self.convertPath(self.getFullPath())
        gamClass = self.gam_class.data()  # class is a py
        gamMode = self.mode.data()

        paramDicts = []
        for parameter in self.getNode('parameters').getChildren():
            paramDict = {}
            try:
                paramDict['name'] = parameter.getNode('name').data()
                paramDict['value'] = parameter.getNode('value').getData()
                paramDict['is_text'] = (
                    parameter.getNode('value').getUsage() == 'TEXT')
                paramDicts.append(paramDict)
            except:
                pass
        inputDicts = []
        if mode == MARTE2_COMPONENT.MODE_GAM or mode == MARTE2_COMPONENT.MODE_OUTPUT:
            inputNids = np.sort(self.getNode('inputs').getChildren())
            for inputNid in inputNids:
                try:
                    input = TreeNode(inputNid, self.getTree())
                    inputDict = {}
                    inputDict['type'] = input.getNode('type').data()
                    try:
                        isStruct = (
                            len(input.getNode('fields').getChildren()) > 0)
                    except:
                        isStruct = False

                    if not isStruct:
                        inputDict['dimensions'] = input.getNode(
                            'dimensions').data()
                        try:
                            inputDict['value'] = input.getNode('value').getData()  # NOT data()
                        except:
                            pass
                        inputDict['value_nid'] = input.getNode('value')
                        inputDict['col_order'] = input.getNode(
                            'col_order').data().upper() == 'YES'
                        try:
                            inputDict['name'] = input.getNode(
                                'name').data()  # Name is optional
                        except:
                            pass
                        inputDict['fields'] = []
                    else:  # structured input
                        inputDict['dimensions'] = 0
                        inputDict['value_nid'] = input.getNode('value')
                        inputDict['name'] = input.getNode(
                            'name').data()  # Name is optional
                        fields = []
                        try:
                            inputDict['value'] = input.getNode(
                                'value').getData()
                        except:  # node not directly connected, but its fields may

                            # Handle possibly structured data. In thos case node 'fields' will contain a list of fields
                            fieldsNode = input.getNode('fields')
                            fieldNids = np.sort(fieldsNode.getChildren())
                            for fieldNid in fieldNids:
                                field = TreeNode(fieldNid, self.getTree())
                                fieldDict = {}
                                fieldDict['type'] = field.getNode(
                                    'type').data()
                                fieldDict['dimensions'] = field.getNode(
                                    'dimensions').data()
                                
                                if fieldDict['dimensions'] == 0:
                                    numberOfElements = 1
                                    numberOfDimensions = 0
                                else:
                                    numberOfDimensions = len(fieldDict['dimensions'])
                                    numberOfElements = 1
                                    for currDim in fieldDict['dimensions']:
                                        numberOfElements *= currDim
                                fieldDict['elements'] = numberOfElements
                                try:
                                    fieldDict['value'] = field.getNode('value').getData()  # NOT data()
                                except:
                                    pass
                                fieldDict['value_nid'] = field.getNode('value')
                                fieldDict['col_order'] = field.getNode(
                                    'col_order').data().upper() == 'YES'
                                fieldDict['name'] = field.getNode(
                                    'name').data()  # Name is mandatory here
                                fields.append(fieldDict)
                        # end except
                        inputDict['fields'] = fields
                    # end if len(inputDict['fields'] == 0)
                    inputDicts.append(inputDict)
                except:
                    pass
        outputDicts = []
        if mode != MARTE2_COMPONENT.MODE_OUTPUT:
            storeSignals = False
            streamSignals = False
            outputNids = np.sort(self.getNode('outputs').getChildren())
            for outputNid in outputNids:
                output = TreeNode(outputNid, self.getTree())
                try:
                    outputDict = {}
                    outputDict['name'] = output.getNode('name').data()
                    outputDict['type'] = output.getNode('type').data()
                    outputDict['dimensions'] = output.getNode(
                        'dimensions').data()
                    if(outputDict['dimensions'] == -1):
                        continue  # dimensions set to -1 means that the output is not used
                    #outputDict['value'] = output.getNode('value').getData()  # NOT data()
                    outputDict['value_nid'] = output.getNode(':value')
                    outputDict['seg_len'] = output.getNode(':seg_len').data()
                    if outputDict['seg_len'] > 0:
                        storeSignals = True
                    outputDict['samples'] = output.getNode(':samples').data()
                    try:
                      streamName = output.getNode(':stream').data()
                      if len(streamName) > 0:
                        outputDict['stream'] = streamName
                        streamSignals = True
                      else:
                        outputDict['stream'] = None  
                    except:
                      outputDict['stream'] = None 

# Handle possibly structured data. In this case node 'fields' will contain a list of fields
                    fields = []
                    try:
                        fieldsNode = output.getNode('fields')
                        fieldNids = np.sort(fieldsNode.getChildren())
                        for fieldNid in fieldNids:
                            field = TreeNode(fieldNid, self.getTree())
                            fieldDict = {}
                            fieldDict['name'] = field.getNode(':name').data()
                            fieldDict['type'] = field.getNode(':type').data()
                            fieldDict['dimensions'] = field.getNode(
                                ':dimensions').data()
                            if fieldDict['dimensions'] == 0:
                                numberOfElements = 1
                                numberOfDimensions = 0
                            else:
                                numberOfDimensions = len(fieldDict['dimensions'])
                                numberOfElements = 1
                                for currDim in fieldDict['dimensions']:
                                    numberOfElements *= currDim
                            fieldDict['elements'] = numberOfElements
                            try:
                                fieldDict['value'] = field.getNode('value').getData()  # NOT data()
                            except:
                                pass
                            fieldDict['value_nid'] = field.getNode(':value')
                            fieldDict['seg_len'] = field.getNode(
                                ':seg_len').data()
                            if fieldDict['seg_len'] > 0:
                                storeSignals = True
                            try:
                              streamName = field.getNode(':stream').data()
                              if len(streamName) > 0:
                                fieldDict['stream'] = field.getNode(':stream').data()
                                streamSignals = True
                              else:
                                fieldDict['stream'] = None
                            except:
                              fieldDict['stream'] = None
                            fields.append(fieldDict)
                    except:
                        pass
                    outputDict['fields'] = fields

                    outputDicts.append(outputDict)
                except:
                    pass
        try:
            outputTrigger = self.outputs_trigger.getData()
        except:
            outputTrigger = None

        if mode != MARTE2_COMPONENT.MODE_OUTPUT:
            outTimeNid = self.outputs_out_time
            outTimeIdx = self.outputs_time_idx.data()
            try:
                preTrigger = self.outputs_pre_trigger.data()
            except:
                preTrigger = 0
            try:
                postTrigger = self.outputs_post_trigger.data()
            except:
                postTrigger = 100

            cpuMask = self.outputs_cpu_mask.data()

            return {'gamName': gamName, 'gamClass': gamClass, 'gamMode': gamMode,
                    'timebase': timebase, 'paramDicts': paramDicts, 'inputDicts': inputDicts, 'outputDicts': outputDicts,
                    'outputTrigger': outputTrigger, 'outTimeNid': outTimeNid, 'outTimeIdx': outTimeIdx, 'preTrigger': preTrigger,
                    'postTrigger': postTrigger, 'storeSignals': storeSignals, 'streamSignals': streamSignals, 'cpuMask': cpuMask, 'debug': isDebug}
        else:
            return {'gamName': gamName, 'gamClass': gamClass, 'gamMode': gamMode,
                    'timebase': timebase, 'paramDicts': paramDicts, 'inputDicts': inputDicts, 'outputDicts': outputDicts}

    def onSameThread(self, threadMap, node, otherNode = None):
        if otherNode == None:
            otherNode = self
        nid1 = otherNode.getNid()
        nid2 = node.getNid()
        try:
            if len(threadMap[nid1]) != len(threadMap[nid2]):
                return False
        except:
            return False
        for idx in range(len(threadMap[nid1])):
            if threadMap[nid1][idx] != threadMap[nid2][idx]:
                return False
        return True

    def sameSynchSource(self, dev):
        timebase = self.timebase.getData()
        if not isinstance(timebase, TreeNode):
            synch1 = self.getNid()
        else:
            prevTimebase = timebase
            while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                if isinstance(timebase, TreeNode):
                    prevTimebase = timebase
                    timebase = timebase.getData()
                else:
                    prevTimebase = TreeNode(timebase, self.getTree())
                    timebase = prevTimebase.getData()
            synch1 = prevTimebase.getParent().getNid()

        timebase = dev.getNode('TIMEBASE').getData()
        if not isinstance(timebase, TreeNode):
            synch2 = dev.getNid()
        else:
            prevTimebase = timebase
            while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                if isinstance(timebase, TreeNode):
                    prevTimebase = timebase
                    timebase = timebase.getData()
                else:
                    prevTimebase = TreeNode(timebase, self.getTree())
                    timebase = prevTimebase.getData()
            synch2 = prevTimebase.getParent().getNid()
        return synch1 == synch2
      
    def getDevList(self, threadMap):
        devList = []
        for nid in threadMap:
            devList.append(TreeNode(nid, self.getTree()))
        return devList


#GAB NOVEMBER 2022     
#Get Synchronizing device. Note that for Synchronized inputs timebase will contain a range descriptor      
    def getSynchDev(self):
        timebase = self.timebase.getData()
        if not isinstance(timebase, TreeNode):
            synch = self
        else:
            prevTimebase = timebase
            while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                if isinstance(timebase, TreeNode):
                    prevTimebase = timebase
                    timebase = timebase.getData()
                else:
                    prevTimebase = TreeNode(timebase, self.getTree())
                    timebase = prevTimebase.getData()
            synch = prevTimebase.getParent()  
        return synch

#GAB OCTOBER 2023
#Get type (as string) of the Time field of the synchronizing device
    def getTimebaseType(self):
        synchDev = self.getSynchDev()
        try:
            return synchDev.outputs_time_type.data()
        except:
            return 'int32'

    #Get devices connected to the passed node 
    def getConnectedDev(self, threadMap, outValueNode):
        outDevs = []
        devList = self.getDevList(threadMap)
        for dev in devList:
            try:
                timebaseNode = TreeNode(dev, self.getTree()).getNode(':TIMEBASE')
                if timebaseNode.getData().getNid() == outValueNode.getNid():
                    outDevs.append(dev)
            except:
                print('WARNING: timebase not defined for '+str(timebaseNode))
                pass
            try:  # If it is an input device it has an INPUTS subtree
                inputSigs = dev.getNode('.INPUTS')
                for inputChan in inputSigs.getChildren():
                    # check first the case of a structure
                    try:
                        fieldsChan = inputChan.getNode('FIELDS')
                        for fieldChan in fieldsChan.getChildren():
                            try:
                                fieldNid = fieldChan.getNode('VALUE').getData().getNid()
                                if fieldNid == outValueNode.getNid():
                                    outDevs.append(dev)
                            except:
                                continue
                    except:
                        pass
                    # Non structure case
                    try:
                        inputNid = inputChan.getNode('VALUE').getData().getNid()
                    except:
                        continue
                    if inputNid == outValueNode.getNid():
                        outDevs.append(dev)
            except:
                pass
                # We need to check also Output Trigger
            try:
                outputTriggerNid = dev.getNode('.OUTPUTS:TRIGGER').getData().getNid()
                if outputTriggerNid == outValueNode.getNid():
                    outDevs.append(dev)
            except:
                pass

        return outDevs              
        

    def isUsedOnAnotherThreadSynch(self, threadMap, outValueNode): 
#return True if outValueNode is refrenced by a device in a different synchronized thread, BUT NOT the sychronizing one
        connectedDev = self.getConnectedDev(threadMap, outValueNode)
        if len(connectedDev) == 0:
            return False  #out not referenced by any other devices
        isUsed = False
        for dev in connectedDev:
            if not self.onSameThread(threadMap, dev):
                if self.sameSynchSource(dev): #if referenced by a device belonging to a synchronized thread
                    synchDev = self.getSynchDev()
                    if not self.onSameThread(threadMap, synchDev, dev): 
                        #out referenced by a synchronized device not belonging to the synchronizing Thread
                        isUsed = True
        return isUsed    

    def isUsedOnAnotherThreadAsynch(self, threadMap, outValueNode): 
#return True if outValueNode is refrenced by a device in a different synchronized thread, BUT NOT the sychronizing one
        connectedDev = self.getConnectedDev(threadMap, outValueNode)
        if len(connectedDev) == 0:
            return False  #out not referenced by any other devices
        isUsed = False
        for dev in connectedDev:
            if not self.onSameThread(threadMap, dev):
                if self.sameSynchSource(dev): #if referenced by a device belonging to a synchronized thread
                    synchDev = self.getSynchDev()
                    if self.onSameThread(threadMap, synchDev, dev): 
                        #out referenced by a synchronized device  belonging to the synchronizing Thread
                        isUsed = True
                else:
                    isUsed = True
        return isUsed    

    def isUsedOnAnotherThread(self, threadMap, outValueNode): 
#return True if refers to a device in another thread
        connectedDev = self.getConnectedDev(threadMap, outValueNode)
        if len(connectedDev) == 0:
            return False  #out not referenced by any other devices
        isUsed = False
        for dev in connectedDev:
            if not self.onSameThread(threadMap, dev):
                isUsed = True
        return isUsed

 
    def isUsed(self, threadMap, outValueNode):
        devList = self.getDevList(threadMap)
        for dev in devList:
                # Check first timebase dependency
            try:
                timebaseNode = TreeNode(dev, self.getTree).getNode(':TIMEBASE')
                if timebaseNode.getData().getNid() == outValueNode.getNid():
                    return True
            except:
                pass
            try:  # If it is an input device it has an INPUTS subtree
                inputSigs = dev.getNode('.INPUTS')
                for inputChan in inputSigs.getChildren():
                        # check first the case of a structure
                    try:
                        fieldsChan = inputChan.getNode('FIELDS')
                        for fieldChan in fieldsChan.getChildren():
                            try:
                                fieldNid = fieldChan.getNode(
                                    'VALUE').getData().getNid()
                                if fieldNid == outValueNode.getNid():
                                    return True
                            except:
                                pass
                    except:
                        pass
                        # Non structure case
                    try:
                        inputNid = inputChan.getNode(
                            'VALUE').getData().getNid()
                    except:
                        continue
                    if inputNid == outValueNode.getNid():
                        return True
            except:
                pass
        return False  # this output is not used by any input

# Check if any field of this output structure is used
    def isAnyFieldUsed(self, threadMap, outputDict):
        for fieldDict in outputDict['fields']:
            if self.isUsed(threadMap, fieldDict['value_nid']):
                return True
        return False

#get alias for struct fields
    def getFieldAliasName(self, fieldValNode):
        aliasName = fieldValNode.getParent().getParent().getParent().getNode(
                            ':name').data()+'_'+fieldValNode.getParent().getNode(':name').data()
        return aliasName



# Add a new type to the current type list. If a type with the same name is not found, the dictionary (name, fields) is added,
# otherwise its fields are checked for consistency. Return false mismatch deteched, true otherwise
    def addType(self, name, fieldDicts, typeDicts):
        for typeDict in typeDicts:
            if typeDict['name'] == name:
                if len(typeDict['fields']) != len(fieldDicts):
                    print('Number of fields mismatch for type '+name)
                    return False
                for typeIdx in range(len(typeDict['fields'])):
                    if typeDict['fields'][typeIdx]['name'] != fieldDicts[typeIdx]['name']:
                        print('Field name mismatch for Type '+name+'  ' +
                              fieldDicts[typeIdx]['name'] + '  ' + typeDict['fields'][typeIdx]['name'])
                        return False
                    if typeDict['fields'][typeIdx]['type'] != fieldDicts[typeIdx]['type']:
                        print('Field type mismatch for Type '+name+'  ' +
                              fieldDicts[typeIdx]['type'] + '  ' + typeDict['fields'][typeIdx]['type'])
                        return False
                    if typeDict['fields'][typeIdx]['dimensions'] != fieldDicts[typeIdx]['dimensions']:
                        print('Field dimensions mismatch for Type '+name+'  ' +
                              fieldDicts[typeIdx]['dimensions'] + '  ' + typeDict['fields'][typeIdx]['dimensions'])
                        return False
                return True
       # If we arrive here, the type is new
        typeDicts.append({'name': name, 'fields': fieldDicts})
        return True


# GAM

    def getMarteGamInfo(self, threadMap, gams, dataSources, gamList, typeDicts):
        configDict = self.getGamInfo()
        gamName = configDict['gamName']
        gamClass = configDict['gamClass']
        timebase = configDict['timebase']
        paramDicts = configDict['paramDicts']
        inputDicts = configDict['inputDicts']
        outputDicts = configDict['outputDicts']
        outputTrigger = configDict['outputTrigger']
        debugEnabled = configDict['debug']

        outPeriod = 0  # If different from 0, this means that the corresponing component is driving the thread timing

#GAB Oct 2022: define preGamList and postGamList for a proper order of generated GAMs
        preGamList = []
        postGamList = []


        try:
            syncDiv = self.timebase_div.data()
        except:
            syncDiv = 1
        resampledSyncSigs = []  #Input Signals for which PickSampleGAM is required
# timebase
        if isinstance(timebase, Range):
            period = timebase.getDescAt(2).data()
            try:
                startTime = timebase.getDescAt(0).data()
            except:
                startTime = 0
            outPeriod = period  # Driving thread timing
            dataSourceText = '  +'+gamName+'_Timer' + ' = {\n'
            dataSourceText += '    Class = LinuxTimer\n'
            dataSourceText += '    SleepNature = "Default"\n'
            dataSourceText += '    Signals = {\n'
            dataSourceText += '      Counter = {\n'
            dataSourceText += '        Type = uint32\n'
            dataSourceText += '      }\n'
            dataSourceText += '      Time = {\n'
            dataSourceText += '        Type = uint32\n'
            dataSourceText += '      }\n'
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

            dataSourceText = '  +'+gamName+'_Timer_DDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

#            gamList.append(gamName+'Timer_IOGAM')
            preGamList.append(gamName+'Timer_IOGAM')
            gamText = '  +'+gamName+'Timer_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            gamText += '      Counter = {\n'
            gamText += '        DataSource = '+gamName+'_Timer\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '      }\n'
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+gamName+'_Timer\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '        Frequency = '+str(round(1./period, 4))+'\n'
            gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            gamText += '      Counter = {\n'
            gamText += '        DataSource = '+gamName+'_Timer_DDB\n'
            gamText += '        Type = uint32\n'
            gamText += '      }\n'
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+gamName+'_Timer_DDB\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

            # Check if time information is required by another synchronized thread
            if self.isUsedOnAnotherThreadSynch(threadMap, self.timebase):

                dataSourceText = '  +'+gamName+'_Timer_Synch = {\n'
                dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
                dataSourceText += '    Timeout = 1000000\n'
                dataSourceText += ' }\n'
                dataSources.append(dataSourceText)

                preGamList.append(gamName+'Timer_Synch_IOGAM')
                gamText = '  +'+gamName+'Timer_Synch_IOGAM = {\n'
                gamText += '    Class = IOGAM\n'
                gamText += '    InputSignals = {\n'
                gamText += '      Counter = {\n'
                gamText += '        DataSource = '+gamName+'_Timer_DDB\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
                gamText += '      }\n'
                gamText += '      Time = {\n'
                gamText += '        DataSource = '+gamName+'_Timer_DDB\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
       #         gamText += '        Frequency = '+str(round(1./period,4))+'\n'
                gamText += '      }\n'
                gamText += '    }\n'
                gamText += '    OutputSignals = {\n'
                gamText += '      Counter = {\n'
                gamText += '        DataSource = '+gamName+'_Timer_Synch\n'
                gamText += '        Type = uint32\n'
                gamText += '      }\n'
                gamText += '      Time = {\n'
                gamText += '        DataSource = '+gamName+'_Timer_Synch\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
                gamText += '      }\n'
                gamText += '    }\n'
                gamText += '  }\n'
                gams.append(gamText)

            timerDDB = gamName+'_Timer_DDB'

        # Link to other component up in the chain
        elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
            prevTimebase = timebase
            while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                if isinstance(timebase, TreeNode):
                    prevTimebase = timebase
                    timebase = timebase.getData()
                else:
                    prevTimebase = TreeNode(timebase, self.getTree())
                    timebase = prevTimebase.getData()
            origName = self.convertPath(prevTimebase.getParent().getFullPath())
            # Check whether the synchronization source is a Synch Input. Only in this case, the origin DDB is its output DDB since that device is expected to produce Time
            originMode = prevTimebase.getParent().getNode('mode').data()
            try:
                startTime = timebase.getDescAt(0).data()
            except:
                startTime = 0
            if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                if self.onSameThread(threadMap, prevTimebase.getParent()):
                    timerDDB = origName+'_Output_DDB'
                else:
                    timerDDB = gamName+'_Res_DDB'
                    resampledSyncSigs.append({'name': 'Time', 'datasource': origName+'_Output_Synch',
                             'type': 'uint32', 'dimensions': 0, 'elements': 1, 'samples':syncDiv})
#                   timerDDB = origName+'_Output_Synch'
                    try:
                        # Get period from driving synchronizing device
                        outPeriod = timebase.getDescAt(2).data()
                    except:
                        outPeriod = 0
            else:
                if self.onSameThread(threadMap, prevTimebase.getParent()):
                    timerDDB = origName+'_Timer_DDB'
                else:
                    timerDDB = origName+'_Timer_Synch'
                    try:
                        # Get period from driving synchronizing device
                        outPeriod = timebase.getDescAt(2).data()
                    except:
                        outPeriod = 0

        else:
            print('ERROR: Invalid timebase definition')
            return 0

       # check for inpu bus conversions (defined later)  ADDED AFTERWARDS
        needInputBusConversion = False
        for inputDict in inputDicts:
            if len(inputDict['fields']) > 0 and not 'value' in inputDict:
                needInputBusConversion = True
#        if needInputBusConversion:
            # Will be defined later, but must execute before this GAM
#            gamList.append(gamName+'_Input_Bus_IOGAM')
#            preGamList.append(gamName+'_Input_Bus_IOGAM')

       #Head and parameters
        gamText = '  +'+gamName+' = {\n'
        gamText += '    Class = '+gamClass+'\n'
        gamText = self.reportParameters(paramDicts, gamText, 1)
        # for paramDict in paramDicts:
        #  if paramDict['is_text']:
        #    gamText += '    '+paramDict['name']+' = "'+str(paramDict['value'])+'"\n'
        #  else:
        #    gamText += '    '+paramDict['name']+' = '+self.convertVal(str(paramDict['value']))+'\n'

# input Signals
        inputSignals = []  # Used for debug printout
        inputSignals.append({'name': 'Time', 'datasource': timerDDB,
                             'type': 'uint32', 'dimensions': 0, 'elements': 1})

        if len(inputDicts) > 0:
            gamText += '    InputSignals = {\n'
        nonGamInputNodes = []
        for inputDict in inputDicts:
            if len(inputDict['fields']) == 0 and not 'value' in inputDict:  #no value, skip
                continue
            signalDict = {}
            # This is a Time field referring to this timebase
            if 'value' in inputDict and isinstance(inputDict['value'], TreeNode) and inputDict['value'].getNodeName() == 'TIMEBASE' and inputDict['value'].getParent() == self:
                gamText += '      Time = {\n'
                gamText += '      DataSource = ' + timerDDB+'\n'
                gamText += '      Samples = '+str(syncDiv)+'\n'
            else:  # Normal reference
                isTreeRef = False
                forceUsingSamples = False
                # --------------------------------------------If this is a structured input
                if len(inputDict['fields']) > 0:
                    if not self.addType(inputDict['type'], inputDict['fields'], typeDicts):
                        raise Exception(
                            'Incompatible Type definition: '+inputDict['type'])
                    if not 'value' in inputDict:  # it shall be specified in its fields
                        gamText += '      '+inputDict['name']+' = {\n'
                        gamText += '        DataSource = '+gamName+'_Input_Bus_DDB\n'
                        gamText += '        Type = '+inputDict['type']+'\n'
                        gamText += '        NumberOfDimensions = 0\n'
                        # Arrays of structures are not suppprted
                        gamText += '        NumberOfElements = 1\n'
                        signalDict['name'] = inputDict['name']
                        signalDict['datasource'] = gamName+'_Input_Bus_DDB'
                    else:
                        sourceNode = inputDict['value'].getParent(
                        ).getParent().getParent()
                        if sourceNode.getUsage() != 'DEVICE':
                            raise Exception(
                                'Invalid value for struct input ' + inputDict['name'])
                        sourceGamName = self.convertPath(
                            sourceNode.getFullPath())
                        if 'name' in inputDict:
                            signalGamName = inputDict['name']
                            aliasName = inputDict['value'].getParent().getNode(
                                ':name').data()
                            signalDict['alias'] = aliasName
                        else:
                            signalGamName = inputDict['value'].getParent().getNode(
                                ':name').data()

                        gamText += '      '+signalGamName+' = {\n'
                        if self.onSameThread(threadMap, sourceNode):
                            gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
                            signalDict['name'] = signalGamName
                            signalDict['datasource'] = sourceGamName + \
                                '_Output_DDB'
                        ##elif self.sameSynchSource(sourceNode):
                        elif self.isUsedOnAnotherThreadSynch(threadMap, sourceNode):
                            signalDict['type'] = inputDict['type']
                            if syncDiv > 1:
                                gamText += '        DataSource = '+gamName+'_Res_DDB\n'
                                signalDict['datasource'] = sourceGamName + '_Output_Synch'
                                signalDict['samples'] = syncDiv
                                signalDict['dimensions'] = inputDict['dimensions']
                                signalDict['elements'] = inputDict['elements']
                                resampledSyncSigs.append(signalDict)
                            else:
                                gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                signalDict['name'] = signalGamName
                                signalDict['datasource'] = sourceGamName + '_Output_Synch'
                        else:
                            gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                        if 'name' in inputDict:
                            gamText += '        Alias = "'+aliasName+'"\n'
                        gamText += '        Type = '+inputDict['type']+'\n'
                        gamText += '        NumberOfDimensions = 0\n'
                        # Arrays of structures are not suppprted
                        gamText += '        NumberOfElements = 1\n'
                    # endif direct structured link
                # endif structured input
                else:  # Non structured input
                    try:
                        # it may be a struct field
                        try:
                            isInputStructField = (inputDict['value'].getParent().getParent().getName() == 'FIELDS')
                        except:
                            isInpuStructField = False
                        if isInputStructField:
                            sourceNode = inputDict['value'].getParent(
                            ).getParent().getParent().getParent().getParent()
                        else:  # normal field
                            sourceNode = inputDict['value'].getParent(
                            ).getParent().getParent()
                        if sourceNode.getUsage() != 'DEVICE':
                            isTreeRef = True
                        else:
                            sourceGamName = self.convertPath(
                                sourceNode.getFullPath())
                            if 'name' in inputDict:
                                signalGamName = inputDict['name']
                                if isInputStructField:
                                    aliasName = inputDict['value'].getParent().getParent().getParent().getNode(
                                        ':name').data()+'_'+inputDict['value'].getParent().getNode(':name').data()
                                else:
                                    aliasName = inputDict['value'].getParent().getNode(
                                        ':name').data()
                                signalDict['alias'] = aliasName
                            else:
                                signalGamName = inputDict['value'].getParent().getNode(
                                    ':name').data()
                    except:
                        isTreeRef = True
                        
                    if isTreeRef:
                        if 'name' in inputDict:
                            signalName = inputDict['name']
                            aliasName = self.convertPath(
                                inputDict['value_nid'].getPath())
                            signalDict['alias'] = aliasName
                            try:
                                nonGamInputNodes.append(
                                    {'expr': inputDict['value'], 'dimensions': inputDict['dimensions'], 'name': aliasName, 'col_order': inputDict['col_order']})
                            except:
                                pass
                        else:
                            signalName = self.convertPath(
                                inputDict['value_nid'].getPath())
                            nonGamInputNodes.append(
                                {'expr': inputDict['value'], 'dimensions': inputDict['dimensions'], 'name': signalName, 'col_order': inputDict['col_order']})
                        gamText += '      '+signalName+' = {\n'
                        gamText += '        DataSource = '+gamName+'_TreeInDDB\n'
                        signalDict['name'] = signalName
 #GAB 2022                       signalDict['datasource'] = gamName+'_TreeInput_Logger'
                        signalDict['datasource'] = gamName+'_TreeInput_LoggerDDB'
                    else:
                        gamText += '      '+signalGamName+' = {\n'
                        if isInputStructField:
                            if self.onSameThread(threadMap, sourceNode):
                                gamText += '        DataSource = '+sourceGamName+'_Expanded_Output_DDB\n'
                                signalDict['name'] = signalGamName
                                signalDict['datasource'] = sourceGamName + '_Expanded_Output_DDB'
                                aliasName = inputDict['value'].getParent().getParent().getParent().getNode(
                                        ':name').data()+'_'+inputDict['value'].getParent().getNode(':name').data()
                                gamText += '        Alias = '+aliasName+'\n'
                               
                            else:
                                synchDev = self.getSynchDev()
                                if self.isUsedOnAnotherThreadSync(threadMap, sourceNode):
                                #fromDev = self.getDevForOutput(threadMap, sourceNode)
                                #if synchDev.getNid() == fromDev.getNid():  #If input derives from synchronizing thread
                                    if syncDiv > 1:
                                        gamText += '        DataSource = '+gamName+'_Res_DDB\n'
                                        signalDict['datasource'] = sourceGamName + '_OutputSynch'
                                        signalDict['samples'] = syncDiv
                                        signalDict['type'] = inputDict['type']
                                        signalDict['dimensions'] = inputDict['dimensions']
                                        try:
                                            signalDict['elements'] = inputDict['elements']
                                        except:
                                            signalDict['elements'] = 1
                                        resampledSyncSigs.append(signalDict)
                                    else:
                                        gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                        signalDict['datasource'] = sourceGamName + '_Output_Synch'
                                if self.isUsedOnAnotherThreadAsync(threadMap, sourceNode):
                                    gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                                    signalDict['datasource'] = sourceGamName + '_Output_Asynch'
                               
                         
                          
                            # NOTE: for expanded outputs communication is supported only within the same thread!!!!!!!!!!!!
                           # gamText += '        DataSource = '+sourceGamName+'_Expanded_Output_DDB\n'
                            signalDict['name'] = signalGamName
                        else:
                            if self.onSameThread(threadMap, sourceNode):
                                gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
                                signalDict['name'] = signalGamName
                                signalDict['datasource'] = sourceGamName + \
                                    '_Output_DDB'
                            elif self.sameSynchSource(sourceNode):
                                if syncDiv > 1:
                                    gamText += '        DataSource = '+gamName+'_Res_DDB\n'
                                elif self.isUsedOnAnotherThreadSynch(threadMap, sourceNode):
                                    gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                else:
                                    gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                                signalDict['name'] = signalGamName
                                signalDict['type'] = inputDict['type']
                                signalDict['datasource'] = sourceGamName + '_Output_Synch'
                                if syncDiv > 1:
                                    signalDict['datasource'] = sourceGamName+'_Output_Synch'
                                    signalDict['samples'] = syncDiv
                                    signalDict['dimensions'] = inputDict['dimensions']
                                    signalDict['elements'] = inputDict['elements']
                                    resampledSyncSigs.append(signalDict)
                            else:
                                gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                    if 'name' in inputDict:
                        gamText += '        Alias = "'+aliasName+'"\n'

                if 'type' in inputDict:
                    gamText += '        Type = '+inputDict['type']+'\n'

                signalDict['type'] = inputDict['type']
                if 'dimensions' in inputDict and not forceUsingSamples:
                    dimensions = inputDict['dimensions']
                    if dimensions == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(inputDict['dimensions'])
                        numberOfElements = 1
                        for currDim in inputDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                        str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                        str(numberOfElements)+'\n'
                    gamText = self.addSignalParameters(
                        inputDict['value_nid'].getParent().getNode('parameters'), gamText)
                    signalDict['dimensions'] = numberOfDimensions
                    signalDict['elements'] = numberOfElements
# endif len(inputDict['fields']) > 0
# endif Normal Reference
            gamText += '      }\n'

            inputSignals.append(signalDict)
        if len(inputDicts) > 0:
            gamText += '    }\n'
            
            

#        print('*****NON GAM INPUT NODES******')
#        print(nonGamInputNodes)
#        if len(nonGamInputNodes) > 0:
#            gamList.append(gamName+'_TreeIn')
#        gamList.append(gamName)
        ######################################################### Output Signals
        outputSignals = []  # For debug printout

        synchThreadSignals = []
        asynchThreadSignals = []
        outputsToBeExpanded = []

        if len(outputDicts) > 0:
            gamText += '    OutputSignals = {\n'
        for outputDict in outputDicts:
            outputSignalDict = {}
            outputSignalDict['name'] = outputDict['name']
            gamText += '      '+outputDict['name']+' = {\n'
            gamText += '        DataSource = '+gamName+'_Output_DDB\n'
            gamText += '        Type = '+outputDict['type']+'\n'
            outputSignalDict['type'] = outputDict['type']
            if outputDict['dimensions'] == 0:
                numberOfElements = 1
                numberOfDimensions = 0
                outputSignalDict['dimensions'] = 0
                outputSignalDict['elements'] = 1
            else:
                numberOfDimensions = len(outputDict['dimensions'])
                numberOfElements = 1
                for currDim in outputDict['dimensions']:
                    numberOfElements *= currDim
            gamText += '        NumberOfDimensions = ' + \
                str(numberOfDimensions)+'\n'
            gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
            outputSignalDict['dimensions'] = numberOfDimensions
            outputSignalDict['elements'] = numberOfElements
            gamText = self.addSignalParameters(
                outputDict['value_nid'].getParent().getNode('parameters'), gamText)
            gamText += '      }\n'
            if self.isUsedOnAnotherThreadSynch(threadMap, outputDict['value_nid']):
                synchThreadSignals.append(outputSignalDict)
            elif self.isUsedOnAnotherThread(threadMap, outputDict['value_nid']):
                asynchThreadSignals.append(outputSignalDict)
            outputSignals.append(outputSignalDict)
            # --------------------------------------------If this is a structured output
            if len(outputDict['fields']) > 0:
                if not self.addType(outputDict['type'], outputDict['fields'], typeDicts):
                    raise Exception(
                        'Incompatible Type definition: '+inputDict['type'])
            # Check if any expanded field of thid output is used by other components
            if self.isAnyFieldUsed(threadMap, outputDict):
                outputsToBeExpanded.append(outputDict)

        if len(outputDicts) > 0:
            gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

        dataSourceText = '  +'+gamName+'_Output_DDB = {\n'
        dataSourceText += '    Class = GAMDataSource\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)

        # If any structured output has to be expanded because any of its fields is referenced, create DDB and relative IOGAM
        if len(outputsToBeExpanded) > 0:
            gamText = '  +'+gamName+'_Output_Bus_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for outputDict in outputsToBeExpanded:
                gamText += '      '+outputDict['name']+' = {\n'
                gamText += '        Type = '+outputDict['type']+'\n'
                gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for outputDict in outputsToBeExpanded:
                for fieldDict in outputDict['fields']:
                    if self.isUsedOnAnotherThreadSynch(threadMap, fieldDict['value_nid']):
                        synchThreadSignals.append(fieldDict)
                    elif self.isUsedOnAnotherThread(threadMap, fieldDict['value_nid']):
                        asynchThreadSignals.append(fieldDict)
                        
                    gamText += '      ' + \
                        outputDict['name']+'_'+fieldDict['name'] + ' = {\n'
                    gamText += '        Type = '+fieldDict['type']+'\n'
                    gamText += '        DataSource = '+gamName+'_Expanded_Output_DDB\n'
                    gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)
            # NOTE: for expanded outputs communication is supported only within the same thread!!!!!!!!!!!!
#            gamList.append(gamName+'_Output_Bus_IOGAM')
            postGamList.append(gamName+'_Output_Bus_IOGAM')
            dataSourceText = '  +'+gamName+'_Expanded_Output_DDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

    # If any output has to be stored We need to declare out DDB, out MdsWriter and relative IOGAM
        if configDict['storeSignals']:
            dataSourceText = '  +'+gamName+'_TreeOutput = {\n'
            dataSourceText += '    Class = MDSWriter\n'

            if outputTrigger == None:
                dataSourceText += '    NumberOfBuffers = 20000\n'
            else:
                dataSourceText += '    NumberOfBuffers = ' + \
                    str(configDict['preTrigger'] +
                        configDict['postTrigger']+1)+'\n'

            dataSourceText += '    NumberOfPreTriggers = ' + \
                str(configDict['preTrigger'])+'\n'
            dataSourceText += '    NumberOfPostTriggers = ' + \
                str(configDict['postTrigger'])+'\n'
            dataSourceText += '    CPUMask = ' + \
                str(configDict['cpuMask'])+'\n'
            dataSourceText += '    StackSize = 10000000\n'
            dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
            dataSourceText += '    PulseNumber = ' + \
                str(self.getTree().shot)+'\n'
            if outputTrigger == None:
                dataSourceText += '    StoreOnTrigger = 0\n'
            else:
                dataSourceText += '    StoreOnTrigger = 1\n'
            dataSourceText += '    EventName = "'+gamName+'UpdatejScope"\n'
            dataSourceText += '    TimeRefresh = 1\n'
            dataSourceText += '    Signals = {\n'
            currTimebase = self.timebase.evaluate()
            if isinstance(currTimebase, Range):
                period = currTimebase.delta.data()
            else:
                currTimebase = currTimebase.data()
                period = currTimebase[1] - currTimebase[0]

            try:
                syncDiv = self.timebase_div.data()
                period = period * syncDiv
            except:
                pass


# If trigger is defined put it as first signal
            if outputTrigger != None:
                dataSourceText += '      Trigger = {\n'
                dataSourceText += '        Type = uint8\n'
                dataSourceText += '      }\n'
# Time Management
            dataSourceText += '      Time = {\n'
            dataSourceText += '        DiscontinuityFactor = 1\n'
            dataSourceText += '        Type = '+self.getTimebaseType()+'\n'
            dataSourceText += '        NodeName = "' + \
                configDict['outTimeNid'].getFullPath()+'"\n'
# keep into account possibl sample information for that GAM
            currSamples = 1
            try:
                currSamples = outputDict['samples']
            except:
                currSamples = 1
            timeSegLen = 1000
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    timeSegLen = outputDict['seg_len']

            dataSourceText += '        Period = '+str(period/currSamples)+'\n'
            dataSourceText += '        MakeSegmentAfterNWrites = '+str(timeSegLen) + '\n'
            dataSourceText += '        AutomaticSegmentation = 0\n'
            if outputTrigger != None:
                dataSourceText += '        TimeSignal = 1\n'
            #if startTime != 0:
            #    dataSourceText += '        SamplePhase = ' + \
            #        str(int(round(startTime/period)))+'\n'
            dataSourceText += '      }\n'

            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    dataSourceText += '      '+outputDict['name']+' = {\n'
                    dataSourceText += '        NodeName = "' + \
                        outputDict['value_nid'].getFullPath()+'"\n'
                    dataSourceText += '        Period = ' + \
                        str(period/currSamples)+'\n'
                    dataSourceText += '        DiscontinuityFactor = 1\n'
                    dataSourceText += '        MakeSegmentAfterNWrites = ' + \
                        str(outputDict['seg_len'])+'\n'
                    dataSourceText += '        AutomaticSegmentation = 0\n'
                    dataSourceText += '      }\n'

                # Check if the output is a struct and seglen is > 0 for one o more fields
                for fieldDict in outputDict['fields']:
                    if fieldDict['seg_len'] > 0:
                        dataSourceText += '      ' + \
                            outputDict['name']+'_'+fieldDict['name']+' = {\n'
                        dataSourceText += '        NodeName = "' + \
                            fieldDict['value_nid'].getFullPath()+'"\n'
                        dataSourceText += '        Period = ' + \
                            str(period/currSamples)+'\n'
                        dataSourceText += '        MakeSegmentAfterNWrites = ' + \
                            str(fieldDict['seg_len'])+'\n'
                        dataSourceText += '        AutomaticSegmentation = 0\n'
                        dataSourceText += '      }\n'
            # end for fieldDict in outputDict['fields']:
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

#            gamList.append(gamName+'_TreeOutIOGAM')
            postGamList.append(gamName+'_TreeOutIOGAM')
            gamText = '  +'+gamName+'_TreeOutIOGAM = {\n'
            if outputTrigger != None  and syncDiv == 1:  # If using output trigger, the trigger must be converted to uint8 GAB OCTOBER 2023
                gamText += '    Class = ConversionGAM\n'
            else:
                if syncDiv > 1:
                  gamText += '    Class = PickSampleGAM\n'
                else:
                  gamText += '    Class = IOGAM\n'

            gamText += '    InputSignals = {\n'

# MdsWriter Trigger management
            if outputTrigger != None:
                physicalTrigger = True
                try:
                    triggerNode = outputTrigger.getParent().getParent().getParent()

                    # If the trigger is pysically generated, i.e. it is derived from another device (GAM or Input)
                    if triggerNode.getUsage() == 'DEVICE':

                        triggerGamName = self.convertPath(
                            triggerNode.getFullPath())
                        triggerSigName = outputTrigger.getParent().getNode(':name').data()
                        gamText += '      '+triggerSigName+' = {\n'
                        if self.onSameThread(threadMap, triggerNode):
                            gamText += '        DataSource = '+triggerGamName+'_Output_DDB\n'
                        elif self.sameSynchSource(sourceNode):
                            gamText += '        DataSource = '+triggerGamName+'_Output_Synch\n'

                            try:
                                syncDiv = self.timebase_div.data()
                                gamText += '        Samples = ' + \
                                    str(syncDiv)+'\n'
                            except:
                                pass  # Consider RealTimeSynchronization downsampling only if timebase_div is defined

                        else:
                            gamText += '        DataSource = '+triggerGamName+'_Output_Asynch\n'
                        gamText += '      }\n'
                    else:
                        physicalTrigger = False
                except:
                    physicalTrigger = False

                if(not physicalTrigger):  # Trigger source is derived from a stored input waveform
                    nonGamInputNodes.append({'expr': outputTrigger.decompile(
                    ), 'dimensions': 0, 'name': 'Trigger', 'col_order': False})
                    gamText += '      '+'Trigger'+' = {\n'
                    gamText += '        DataSource = '+gamName+'_TreeInDDB\n'
                    gamText += '        Type = uint8\n'
                    gamText += '      }\n'
# end Trigger Management

# Time signal management
            gamText += '      Time = {\n'
            gamText += '        DataSource = ' + timerDDB+'\n'
#            gamText += '        Type = uint32\n' GAB2022
            gamText += '        Type = int32\n'
            gamText += '        Samples = '+str(syncDiv)+'\n'
            gamText += '      }\n'
# Other output signals
# first non struct outputs
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0 and len(outputDict['fields']) == 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                    gamText += '      }\n'
# then struct outputs for which at least one field has seg_len > 0
            for outputDict in outputDicts:
                fieldsToStore = False
                for fieldDict in outputDict['fields']:
                    if fieldDict['seg_len'] > 0:
                        fieldsToStore = True
                if fieldsToStore:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                    gamText += '        Type = ' + outputDict['type']+'\n'
                    gamText += '      }\n'
               # end for outputDict in outputDicts:

            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            if outputTrigger != None:
                gamText += '      Trigger = {\n'
                gamText += '      DataSource = '+gamName+'_TreeOutput\n'
                gamText += '      type = uint8\n'
                gamText += '    }\n'
# Time signal
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+gamName+'_TreeOutput\n'
#            gamText += '        Type = uint32\n'  GAB2022
            gamText += '        Type = int32\n' 
            gamText += '      }\n'
# Other signals
            for outputDict in outputDicts:
                # first non struct outputs
                if outputDict['seg_len'] > 0 and len(outputDict['fields']) == 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_TreeOutput\n'
                    gamText += '        Type = '+outputDict['type']+'\n'


# If the GAM device defines Samples in its output, take precedence over dimensions information
                    hasSamples = False
                    try:
                        currSamples = outputDict['samples']
                        if currSamples > 1:
                            hasSamples = True
                    except:
                        pass

                    if hasSamples:  # E.g. MARTE2_RESAMPLER
                        gamText += '        NumberOfDimensions = 0\n'
                        gamText += '        Samples = '+str(currSamples)+'\n'
                    else:  # store single sample of scalar or array
                        if outputDict['dimensions'] == 0:
                            numberOfElements = 1
                            numberOfDimensions = 0
                        else:
                            numberOfDimensions = len(outputDict['dimensions'])
                            numberOfElements = 1
                            for currDim in outputDict['dimensions']:
                                numberOfElements *= currDim
                        gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                        gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'

                    gamText += '      }\n'

            needsOutput_Bus_DDB = False
            for outputDict in outputDicts:  # split fileds of structured outputs for which at least one field has seg_len > 0
                fieldsToStore = False
                for fieldDict in outputDict['fields']:
                    if fieldDict['seg_len'] > 0:
                        fieldsToStore = True
                if fieldsToStore:
                    for fieldDict in outputDict['fields']:
                        gamText += '      ' + \
                            outputDict['name']+'_'+fieldDict['name']+' = {\n'
                        gamText += '        Type = ' + fieldDict['type']+'\n'
                        if fieldDict['dimensions'] == 0:
                            numberOfElements = 1
                            numberOfDimensions = 0
                        else:
                            numberOfDimensions = len(fieldDict['dimensions'])
                            numberOfElements = 1
                            for currDim in fieldDict['dimensions']:
                                numberOfElements *= currDim
                        gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                        gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                        if fieldDict['seg_len'] > 0:
                            gamText += '        DataSource = '+gamName+'_TreeOutput\n'
                        else:
                            needsOutput_Bus_DDB = True
                            gamText += '        DataSource = '+gamName+'_Output_Bus_DDB\n'
                        gamText += '      }\n'
                # end if fieldsToStore
            # end for outputDict in outputDicts:
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

            if needsOutput_Bus_DDB:
                dataSourceText = '  +'+gamName+'_Output_Bus_DDB = {\n'
                dataSourceText += '    Class = GAMDataSource\n'
                dataSourceText += '  }\n'
                dataSources.append(dataSourceText)

        # endif configDict['storeSignals']
        ###########Handle possible data streaming: declare IOGAM and StreamOut DataStream
        if configDict['streamSignals']:
            dataSourceText = '  +'+gamName+'_StreamOutput = {\n'
            dataSourceText += '    Class = StreamOut\n'
            dataSourceText += '    TimeIdx = 0\n'
            dataSourceText += '    TimeStreaming = 1\n'
            dataSourceText += '    CpuMask = ' + str(configDict['cpuMask'])+'\n'
            dataSourceText += '    StackSize = 10000000\n'
            dataSourceText += '    NumberOfBuffers = 10\n'
            dataSourceText += '    Signals = {\n'
            dataSourceText += '      Time = {\n'
            dataSourceText += '        NumberOfDimensions = 0\n'
            dataSourceText += '        NumberOfElements = 1\n'
            dataSourceText += '        Channel = "'+gamName+'_Time"\n'
            dataSourceText += '      }\n'
            for outputDict in outputDicts:
                if outputDict['stream']  != None:
                    dataSourceText += '      '+outputDict['name']+' = {\n'
                    dataSourceText += '        Channel = "' + outputDict['stream']+'"\n'
                    dataSourceText += '      }\n'
 # Check if the output is a struct and seglen is > 0 for one o more fields
                for fieldDict in outputDict['fields']:
                    if fieldDict['stream'] != None:
                        dataSourceText += '      ' +  outputDict['name']+'_'+fieldDict['name']+' = {\n'
                        dataSourceText += '        Channel = "' + fieldDict['stream']+'"\n'
                        dataSourceText += '      }\n'
            # end for fieldDict in outputDict['fields']:
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

#            gamList.append(gamName+'_StreamOutIOGAM')
            postGamList.append(gamName+'_StreamOutIOGAM')
            gamText = '  +'+gamName+'_StreamOutIOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
# Time signal management
            gamText += '      Time = {\n'
            gamText += '        DataSource = ' + timerDDB+'\n'
            gamText += '        Type = uint32\n'
            gamText += '      }\n'
# first non struct outputs
            for outputDict in outputDicts:
                if outputDict['stream'] != None and len(outputDict['fields']) == 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                    gamText += '        Type = ' + outputDict['type']+'\n'
                    gamText += '      }\n'
# then struct outputs for which at least one field has stream != None
            for outputDict in outputDicts:
                fieldsToStream = False
                for fieldDict in outputDict['fields']:
                    if fieldDict['stream'] != None:
                        fieldsToStream = True
                if fieldsToStream:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                    gamText += '        Type = ' + outputDict['type']+'\n'
                    gamText += '      }\n'
               # end for outputDict in outputDicts:

            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+gamName+'_StreamOutput\n'
            gamText += '        Type = uint32\n'
            gamText += '      }\n'
# Other signals
            for outputDict in outputDicts:
                # first non struct outputs
                if outputDict['stream'] != None and len(outputDict['fields']) == 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+gamName+'_StreamOutput\n'
                    gamText += '        Type = '+outputDict['type']+'\n'
                    if outputDict['dimensions'] == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(outputDict['dimensions'])
                        numberOfElements = 1
                        for currDim in outputDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'

                    gamText += '      }\n'

            needsOutStream_Bus_DDB = False
            for outputDict in outputDicts:  # split fileds of structured outputs for which at least one field has seg_len > 0
                fieldsToStream = False
                for fieldDict in outputDict['fields']:
                    if fieldDict['stream'] != None:
                        fieldsToStream = True
                if fieldsToStream:
                    for fieldDict in outputDict['fields']:
                        gamText += '      ' + \
                            outputDict['name']+'_'+fieldDict['name']+' = {\n'
                        gamText += '        Type = ' + fieldDict['type']+'\n'
                        if fieldDict['dimensions'] == 0:
                            numberOfElements = 1
                            numberOfDimensions = 0
                        else:
                            numberOfDimensions = len(fieldDict['dimensions'])
                            numberOfElements = 1
                            for currDim in fieldDict['dimensions']:
                                numberOfElements *= currDim
                        gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                        gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                        if fieldDict['stream'] != None:
                            gamText += '        DataSource = '+gamName+'_StreamOutput\n'
                        else:
                            needsOutStream_Bus_DDB = True
                            gamText += '        DataSource = '+gamName+'_OutStream_Bus_DDB\n'
                        gamText += '      }\n'
                # end if fieldsToStore
            # end for outputDict in outputDicts:
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

            if needsOutStream_Bus_DDB:
                dataSourceText = '  +'+gamName+'_OutStream_Bus_DDB = {\n'
                dataSourceText += '    Class = GAMDataSource\n'
                dataSourceText += '  }\n'
                dataSources.append(dataSourceText)

        # endif configDict['streamSignals']

# Check now if there are structured input signals not directly linked. For these signals, all their fields must be linked. In thic case a new
# IOGAM and DDB DDB will be created
        if needInputBusConversion:
            gamText = '  +'+gamName+'_Input_Bus_IOGAM = {\n'
            if syncDiv > 1:
                gamText += '    Class = PickSampleGAM\n'
            else:
                gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for inputDict in inputDicts:
                if len(inputDict['fields']) > 0 and not 'value' in inputDict:
                    for fieldDict in inputDict['fields']:

                        # it may be a reference to a struct field
                        try:
                            isInputStructField = (fieldDict['value'].getParent().getParent().getName() == 'FIELDS')
                        except:
                            isInputStructField = False
                        try:
                            if isInputStructField:
                                sourceNode = fieldDict['value'].getParent(
                                ).getParent().getParent().getParent().getParent()
                            else:  # normal field
                                sourceNode = fieldDict['value'].getParent(
                                ).getParent().getParent()
                            if sourceNode.getUsage() != 'DEVICE':
                                isTreeRef = True
                            else:
                                sourceGamName = self.convertPath(
                                    sourceNode.getFullPath())
                                if 'name' in fieldDict:
                                    signalGamName = fieldDict['name']
                                    if isInputStructField:
                                        aliasName = fieldDict['value'].getParent().getParent().getParent().getNode(
                                            ':name').data()+'_'+fieldDict['value'].getParent().getNode(':name').data()
                                    else:
                                        aliasName = fieldDict['value'].getParent().getNode(
                                            ':name').data()
                                else:
                                    signalGamName = fieldDict['value'].getParent().getNode(
                                        ':name').data()
                        except:
                            isTreeRef = True
                        if isTreeRef:
                            if 'name' in fieldDict:
                                signalName = inputDict['name'] + \
                                    '_'+fieldDict['name']
                                aliasName = inputDict['name']+'_'+self.convertPath(
                                    fieldDict['value_nid'].getPath())
                                nonGamInputNodes.append(
                                    {'expr': fieldDict['value'], 'dimensions': fieldDict['dimensions'], 'name': aliasName, 'col_order': fieldDict['col_order']})
                            else:
                                signalName = inputDict['name']+'_'+self.convertPath(
                                    fieldDict['value_nid'].getPath())
                                nonGamInputNodes.append(
                                    {'expr': fieldDict['value'], 'dimensions': fieldDict['dimensions'], 'name': signalName, 'col_order': fieldDict['col_order']})
                            gamText += '      '+signalName+' = {\n'
# GAB 2022                           gamText += '        DataSource = '+gamName+'_TreeInput\n'
                            gamText += '        DataSource = '+gamName+'_TreeInDDB\n'
                        else:
                            gamText += '      '+signalGamName+' = {\n'
                            if isInputStructField:
                                if self.onSameThread(threadMap, sourceNode): 
                                    aliasName = fieldDict['value'].getParent().getParent().getParent().getNode(
                                        ':name').data()+'_'+fieldDict['value'].getParent().getNode(':name').data()
                                    gamText += '        DataSource = '+sourceGamName+'_Expanded_Output_DDB\n'
                                    gamText += '        Alias = '+aliasName+'\n'
                                else:
                                    synchDev = self.getSynchDev()
                                    if synchDev.sameSynchSource(sourceNode):
                                    #if synchDev.getNid() == sourceNode.getNid():  #If input derives from synchronizing thread
                                        if syncDiv > 1:
                                            gamText += '        DataSource = '+gamName+'_Res_DDB\n'
                                            signalDict = {}
                                            signalDict['name'] = fieldDict['name'] 
                                            signalDict['datasource'] = sourceGamName + '_Output_Synch'
                                            signalDict['samples'] = syncDiv  
                                            signalDict['type'] = fieldDict['type']
                                            signalDict['dimensions'] = fieldDict['dimensions']
                                            try:
                                                signalDict['elements'] = fieldDict['elements']
                                            except:
                                                signalDict['elements'] = 1
                                            resampledSyncSigs.append(signalDict)
                                        else:
                                            gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                    else:
                                        gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                            else:
                                if self.onSameThread(threadMap, sourceNode):
                                    gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
                                else:
                                    synchDev = self.getSynchDev()
                                    if synchDev.sameSynchSource(sourceNode):
                                   # if synchDev.getNid() == sourceNode.getNid():  #If input derives from syncheonizing thread
                                        gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                        try:
                                            syncDiv = self.timebase_div.data()
                                            gamText += '        Samples = ' + str(syncDiv)+'\n'
                                            forceUsingSamples = True
                                        except:
                                            pass  # Consider RealTimeSynchronization downsampling only if timebase_div is defined
                                    else:
                                        gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                        if 'name' in fieldDict and not isInputStructField:  #if struct field, alias has already been resolved
                            gamText += '        Alias = "'+aliasName+'"\n'
                        if 'type' in fieldDict:
                            gamText += '        Type = '+fieldDict['type']+'\n'

                        if 'dimensions' in fieldDict and not forceUsingSamples:
                            dimensions = fieldDict['dimensions']
                            if dimensions == 0:
                                numberOfElements = 1
                                numberOfDimensions = 0
                            else:
                                numberOfDimensions = len(
                                    fieldDict['dimensions'])
                                numberOfElements = 1
                                for currDim in fieldDict['dimensions']:
                                    numberOfElements *= currDim
                            gamText += '        NumberOfDimensions = ' + \
                                str(numberOfDimensions)+'\n'
                            gamText += '        NumberOfElements = ' + \
                                str(numberOfElements)+'\n'
                        gamText += '      }\n'
                        # endif 'dimensions' in fieldDict and not forceUsingSamples:
                    # endif for fieldDict in inputDict['fields']:
                # endif len(fieldDict['fields']) > 0
            # end for inputDict in inputDicts:

            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for inputDict in inputDicts:
                if len(inputDict['fields']) > 0 and not 'value' in inputDict:
                    gamText += '      '+inputDict['name']+' = {\n'
                    gamText += '        DataSource = '+gamName+'_Input_Bus_DDB\n'
                    gamText += '        Type = '+inputDict['type']+'\n'
                    gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            
            
            gams.append(gamText)

            dataSourceText = '  +'+gamName+'_Input_Bus_DDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)
        # endif needInputBusConversion



#If some inputs derive from resampled synch sources, instantiate PickSampleGAM
        if len(resampledSyncSigs) > 0:
#            gamList.append(gamName+'Resampler')
            preGamList.append(gamName+'Resampler')
            pickGamText = '  +'+gamName+'Resampler = {\n'
            pickGamText += '    Class = PickSampleGAM\n'
            pickGamText += '    InputSignals = {\n'
            for sigDict in resampledSyncSigs:
                pickGamText += '      '+sigDict['name']+' = {\n'
                pickGamText +=  '        DataSource = '+sigDict['datasource']+'\n'
                if 'alias' in sigDict:
                    pickGamText +=  '        Alias = '+sigDict['alias']+'\n'
                pickGamText +=  '        Samples = '+str(sigDict['samples'])+'\n'
                pickGamText +=  '        Type = '+sigDict['type']+'\n'
                pickGamText +=  '        NumberOfDimensions = '+str(sigDict['dimensions'])+'\n'
                pickGamText +=  '        NumberOfElements = '+str(sigDict['elements'])+'\n'
                pickGamText +=  '      }\n'
            pickGamText += '    }\n' 
            pickGamText += '    OutputSignals = {\n'
            for sigDict in resampledSyncSigs:
                if 'alias' in sigDict:
                    pickGamText +=  '      '+sigDict['alias']+' = {\n'
                else:
                    pickGamText += '      '+sigDict['name']+' = {\n'
                pickGamText +=  '        DataSource = '+gamName+'_Res_DDB\n'
                pickGamText +=  '        Samples = 1\n'
                pickGamText +=  '        Type = '+sigDict['type']+'\n'
                pickGamText +=  '        NumberOfDimensions = '+str(sigDict['dimensions'])+'\n'
                pickGamText +=  '        NumberOfElements = '+str(sigDict['elements'])+'\n'
                pickGamText +=  '      }\n'
            pickGamText += '    }\n' 
            pickGamText += '  }\n' 
            gams.append(pickGamText)            
            dataSourceText = '  +'+gamName+'_Res_DDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)



















        # There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        # GAB 2022 here are input references to tree nodes, we need to build a MDSReaderGAM instance named <gam name>_TreeInput and a DDB
        # named <gam name>_TreeInput_DDB
        if len(nonGamInputNodes) > 0:
            # if debugEnabled, we need to write TWO instances of MDSReaderGAM (_TreeInput and _TreeInput_Debug)
            treeInputExtensions = []
            treeInputExtensions.append('_TreeIn')
            if debugEnabled:
                treeInputExtensions.append('_TreeIn_Logger')

            for treeInputExtension in treeInputExtensions:
                dataSourceText = '  +'+gamName+treeInputExtension+'DDB = {\n'
                dataSourceText += '    Class = GAMDataSource\n'
                dataSourceText += ' }\n'
                dataSources.append(dataSourceText)

                mdsReaderText =  '  +'+gamName+treeInputExtension + ' = {\n'
                mdsReaderText += '    Class = MDSReaderGAM\n'
                mdsReaderText += '    TreeName = "'+self.getTree().name+'"\n'
                mdsReaderText += '    ShotNumber = ' + \
                    str(self.getTree().shot)+'\n'
                mdsReaderText += '    InputSignals = { \n'
                mdsReaderText += '      Time = {\n'
                mdsReaderText += '          DataSource = '+timerDDB + '\n' 
                mdsReaderText += '      } \n'
                mdsReaderText += '    } \n'
                mdsReaderText += '    OutputSignals = { \n'
                for nodeDict in nonGamInputNodes:
                    mdsReaderText += '      '+nodeDict['name']+' = {\n'
                    mdsReaderText += '        DataSource = '+gamName+treeInputExtension+'DDB\n'
                    valExpr = nodeDict['expr']
                    if isinstance(valExpr, TreeNode):
                        valExpr = str(valExpr)
                    else:
                        valExpr = str(valExpr)
                    valExpr = valExpr.replace('"', "'")
                    mdsReaderText += '        DataExpr = "'+valExpr+'"\n'
                    mdsReaderText += '        TimebaseExpr = "dim_of(' + \
                        valExpr+')"\n'
                    numberOfElements = 1
                    if not (np.isscalar(nodeDict['dimensions'])):
                        for currDim in nodeDict['dimensions']:
                            numberOfElements *= currDim
                    mdsReaderText += '        NumberOfElements = ' + \
                        str(numberOfElements)+'\n'
                    if nodeDict['col_order']:
                        mdsReaderText += '        UseColumnOrder = 1\n'
                    else:
                        mdsReaderText += '        UseColumnOrder = 0\n'
                    mdsReaderText += '        DataManagement = 1\n'
                    mdsReaderText += '      }\n'
                mdsReaderText += '    }\n'
                mdsReaderText += '  }\n'
                gams.append(mdsReaderText)
                preGamList.append(gamName+treeInputExtension)
# Some outputs are connected to devices on separate synchronized threads
        if needInputBusConversion:
            preGamList.append(gamName+'_Input_Bus_IOGAM')

        if len(synchThreadSignals) > 0:
            dataSourceText = '  +'+gamName+'_Output_Synch = {\n'
            dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
            dataSourceText += '    Timeout = 1000000\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

#            gamList.append(gamName+'_Output_Synch_IOGAM')
            postGamList.append(gamName+'_Output_Synch_IOGAM')
            gamText = '  +'+gamName+'_Output_Synch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signalDict in synchThreadSignals:
                try:
                    isInputStructField = (signalDict['value_nid'].getParent().getParent().getName() == 'FIELDS')
                except:
                    isInputStructField = False
                gamText += '      '+signalDict['name']+' = {\n'
                if isInputStructField:
                    gamText += '        DataSource = '+gamName+'_Expanded_Output_DDB\n'
                    gamText += '        Alias = '+self.getFieldAliasName(signalDict['value_nid'])+'\n'
                else:
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                if 'dimensions' in signalDict:
                    dimensions = signalDict['dimensions']
                    if dimensions == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(signalDict['dimensions'])
                        numberOfElements = 1
                        for currDim in signalDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signalDict in synchThreadSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_Synch\n'
                #Check if it is the reference to a resampled timebase
                gamText += '        Type = '+signalDict['type']+'\n'
                if 'dimensions' in signalDict:
                    dimensions = signalDict['dimensions']
                    if dimensions == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(signalDict['dimensions'])
                        numberOfElements = 1
                        for currDim in signalDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

        # Some outputs are connected to devices on separate synchronized theads
        if len(asynchThreadSignals) > 0:
            dataSourceText = '  +'+gamName+'_Output_Asynch = {\n'
            dataSourceText += '    Class = RealTimeThreadAsyncBridge\n'
            dataSourceText += '    BlockingMode = 1\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

#            gamList.append(gamName+'_Output_Asynch_IOGAM')
            postGamList.append(gamName+'_Output_Asynch_IOGAM')
            gamText = '  +'+gamName+'_Output_Asynch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signalDict in asynchThreadSignals:
                try:
                    isInputStructField = (signalDict['value'].getParent().getParent().getName() == 'FIELDS')
                except:
                    isInputStructField = False
                gamText += '      '+signalDict['name']+' = {\n'
                if isInputStructField:
                    gamText += '        DataSource = '+gamName+'_Expanded_Output_DDB\n'
                else:
                    gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signalDict in asynchThreadSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_Asynch\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '      }\n'

            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

# If debug enabled we need one IOGAM for printing inputs and outputs
        if debugEnabled:
            gamText = '  +'+gamName+'_Logger_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signalDict in inputSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = ' + \
                    signalDict['datasource']+'\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                try:
                    gamText += '        Alias = "'+signalDict['alias']+'"\n'
                except:
                    pass  # Time signal shall not have value_nid key
                gamText += '      }\n'
            for signalDict in outputSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signalDict in inputSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+gamName+'_Logger\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '      }\n'
            for signalDict in outputSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+gamName+'_Logger\n'
                gamText += '        NumberOfDimensions = ' + \
                    str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

            dataSourceText = '  +'+gamName+'_Logger = {\n'
            dataSourceText += '    Class = LoggerDataSource\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)
#            gamList.append(gamName+'_Logger_IOGAM')
            postGamList.append(gamName+'_Logger_IOGAM')

        for currGam in preGamList:
          gamList.append(currGam)
        gamList.append(gamName)
        for currGam in postGamList:
          gamList.append(currGam)
        return outPeriod


# SYNCH and NON SYNCH  INPUT
    def getMarteInputInfo(self, threadMap, gams, dataSources, gamList, isSynch):
        configDict = self.getGamInfo()
        dataSourceName = configDict['gamName']
        dataSourceClass = configDict['gamClass']
        timebase = configDict['timebase']
        paramDicts = configDict['paramDicts']
        outputDicts = configDict['outputDicts']
        outputTrigger = configDict['outputTrigger']
        outPeriod = 0  # If different from 0, this means that the corresponing component is driving the thread timing
        synchThreadSignals = []
        asynchThreadSignals = []

        nonGamInputNodes = []  #Only used for trigger signal for MdsWriter 

        startTime = 0
        if not isSynch:

            # handle  timebase as GAM for non synchronizing inputs
            if isinstance(timebase, Range):
                period = timebase.getDescAt(2).data()
                try:
                    startTime = timebase.getDescAt(0).data()
                except:
                    startTime = 0
                outPeriod = period  # Driving thread timing
                dataSourceText = '  +'+dataSourceName+'_Timer' + ' = {\n'
                dataSourceText += '    Class = LinuxTimer\n'
                dataSourceText += '    SleepNature = "Default"\n'
                dataSourceText += '    Signals = {\n'
                dataSourceText += '      Counter = {\n'
                dataSourceText += '        Type = uint32\n'
                dataSourceText += '      }\n'
                dataSourceText += '      Time = {\n'
                dataSourceText += '        Type = uint32\n'
                dataSourceText += '      }\n'
                dataSourceText += '    }\n'
                dataSourceText += '  }\n'
                dataSources.append(dataSourceText)

                dataSourceText = '  +'+dataSourceName+'_Timer_DDB = {\n'
                dataSourceText += '    Class = GAMDataSource\n'
                dataSourceText += ' }\n'
                dataSources.append(dataSourceText)

                gamList.append(dataSourceName+'Timer_IOGAM')
                gamText = '  +'+dataSourceName+'Timer_IOGAM = {\n'
                gamText += '    Class = IOGAM\n'
                gamText += '    InputSignals = {\n'
                gamText += '      Counter = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Timer\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
                gamText += '      }\n'
                gamText += '      Time = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Timer\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
                gamText += '        Frequency = '+str(round(1./period, 4))+'\n'
                gamText += '      }\n'
                gamText += '    }\n'
                gamText += '    OutputSignals = {\n'
                gamText += '      Counter = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
                gamText += '        Type = uint32\n'
                gamText += '      }\n'
                gamText += '      Time = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
                gamText += '        Type = uint32\n'
                gamText += '        NumberOfElements = 1\n'
                gamText += '      }\n'
                gamText += '    }\n'
                gamText += '  }\n'
                gams.append(gamText)

                # Check if time information is required by another synchronized thread
                if self.isUsedOnAnotherThreadSynch(threadMap, self.timebase):
                    dataSourceText = '  +'+dataSourceName+'_Timer_Synch = {\n'
                    dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
                    dataSourceText += '    Timeout = 1000000\n'
                    dataSourceText += ' }\n'
                    dataSources.append(dataSourceText)

                    gamList.append(dataSourceName+'Timer_Synch_IOGAM')
                    gamText = '  +'+dataSourceName+'Timer_Synch_IOGAM = {\n'
                    gamText += '    Class = IOGAM\n'
                    gamText += '    InputSignals = {\n'
                    gamText += '      Counter = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
                    gamText += '        Type = uint32\n'
                    gamText += '        NumberOfElements = 1\n'
                    gamText += '      }\n'
                    gamText += '      Time = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
                    gamText += '        Type = uint32\n'
                    gamText += '        NumberOfElements = 1\n'
#           gamText += '        Frequency = '+str(round(1./period,4))+'\n'
                    gamText += '      }\n'
                    gamText += '    }\n'
                    gamText += '    OutputSignals = {\n'
                    gamText += '      Counter = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Timer_Synch\n'
                    gamText += '        Type = uint32\n'
                    gamText += '      }\n'
                    gamText += '      Time = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Timer_Synch\n'
                    gamText += '        Type = uint32\n'
                    gamText += '        NumberOfElements = 1\n'
                    gamText += '      }\n'
                    gamText += '    }\n'
                    gamText += '  }\n'
                    gams.append(gamText)

                timerDDB = dataSourceName+'_Timer_DDB'

            elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):  
                if not self.onSameThread(threadMap, timebase.getParent()):
                    #Handle the case this async input is the first in the thread chain and synchronized to another thread
                    origName = self.convertPath(timebase.getParent().getFullPath())
                    timerDDB = origName+'_Output_Synch'
                    originMode = timebase.getParent().getNode('mode').data()
                    if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                        inTimerDDB = origName+'_Output_Synch'
                    else:
                        inTimerDDB = origName+'_Timer_Synch'

                    try:
                        syncDiv = self.timebase_div.data()
                    except:
                        syncDiv = 1

                    gamText = '  +'+dataSourceName+'Timer_Synch_IOGAM = {\n'
                    gamText += '    Class = PickSampleGAM\n'
                    gamText += '    InputSignals = {\n'
                    gamText += '      Time = {\n'
                    gamText += '        DataSource = '+inTimerDDB+'\n'
                    gamText += '        Type = '+self.getTimebaseType()+'\n'
                    gamText += '        NumberOfElements = 1\n'
                    gamText += '        Samples = ' + str(syncDiv)+'\n'
                    gamText += '      }\n'
                    gamText += '    }\n'
                    gamText += '    OutputSignals = {\n'
                    gamText += '      Time = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Timer_Sync\n'
                    gamText += '        Type = '+self.getTimebaseType()+'\n'
                    gamText += '        NumberOfElements = 1\n'
                    gamText += '      }\n'
                    gamText += '    }\n'
                    gamText += '  }\n'
                    gams.append(gamText)
                    gamList.append(dataSourceName+'Timer_Synch_IOGAM')

                    dataSourceText = '  +'+dataSourceName+'_Timer_Sync = {\n'
                    dataSourceText += '    Class = GAMDataSource\n'
                    dataSourceText += ' }\n'
                    dataSources.append(dataSourceText)

                    timerDDB = dataSourceName+'_Timer_Sync' 

                    outPeriod = timebase.getData().getDescAt(2).data()

                else:
            # Link to other component up in the chain

                    prevTimebase = timebase
                    while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                        if isinstance(timebase, TreeNode):
                            prevTimebase = timebase
                            timebase = timebase.getData()
                        else:
                            prevTimebase = TreeNode(timebase, self.getTree())
                            timebase = prevTimebase.getData()
                    origName = self.convertPath(
                        prevTimebase.getParent().getFullPath())
                # Check whether the synchronization source is a Synch Input. Only in this case, the origin DDB is its output DDB since that device is expected to produce Time
                    originMode = prevTimebase.getParent().getNode('mode').data()
                    try:
                        startTime = timebase.getDescAt(0).data()
                    except:
                        startTime = 0
                    if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                        if self.onSameThread(threadMap, prevTimebase.getParent()):
                            timerDDB = origName+'_Output_DDB'
                        else:
                            timerDDB = origName+'_Output_Synch'
                            try:
                                # Get period from driving synchronizing device
                                outPeriod = timebase.getDescAt(2).data()
                            except:
                                outPeriod = 0
                    else:
                        if self.onSameThread(threadMap, prevTimebase.getParent()):
                            timerDDB = origName+'_Timer_DDB'
                        else:
                            timerDDB = origName+'_Timer_Synch'
                            try:
                                # Get period from driving synchronizing device
                                outPeriod = timebase.getDescAt(2).data()
                            except:
                                outPeriod = 0

            else:
                print('ERROR: Invalid timebase definition')
                return 0

# Unlike GAM no other  configuration needs to be considered since there are no inputs
# if isSynch,  timebase Will contain the defintion of the time that will be generated. Specific subclasses will connect it to DataSource specific parameters
# therefore no actions are taken here in addition. In this case, however, the component is driving thread timing
        else:  # isSynch
            currTimebase = self.getNode('timebase').evaluate()
            if isinstance(currTimebase, Range):
                outPeriod = currTimebase.delta.data()
                try:
                    startTime = currTimebase.begin.data()
                except:
                    startTime = 0
            else:
                currTimebase = currTimebase.data()
                outPeriod = currTimebase[1] - currTimebase[0]
                startTime = currTimebase[0]
                
                
                
          # Check if time information is required by another synchronized thread Gabriele Jan 2022
          # for sync input devices, the check has to be performed on output Time
            if self.isUsedOnAnotherThreadSynch(threadMap, self.timebase):
                synchThreadSignals.append({'name':'Time', 'type':self.getTimebaseType(), 'dimensions':0, 'elements': 1}) 
# GAB OCTOBER 2023               synchThreadSignals.append({'name':'Time', 'type':'uint32', 'dimensions':0, 'elements': 1}) 
# endif isSynch

#Head and parameters
        dataSourceText = '  +'+dataSourceName+' = {\n'
        dataSourceText += '    Class = '+dataSourceClass+'\n'
        dataSourceText = self.reportParameters(paramDicts, dataSourceText, 1)
#      for paramDict in paramDicts:
#        if paramDict['is_text']:
#          dataSourceText += '    '+paramDict['name']+' = "'+str(paramDict['value'])+'"\n'
#        else:
#          dataSourceText += '    '+paramDict['name']+' = '+self.convertVal(str(paramDict['value']))+'\n'

    # Output Signals
        dataSourceText += '    Signals = {\n'
        for outputDict in outputDicts:
            dataSourceText += '      '+outputDict['name']+' = {\n'
            dataSourceText += '        Type = '+outputDict['type']+'\n'
            if outputDict['dimensions'] == 0:
                numberOfElements = 1
                numberOfDimensions = 0
            else:
                numberOfDimensions = len(outputDict['dimensions'])
                numberOfElements = 1
                for currDim in outputDict['dimensions']:
                    numberOfElements *= currDim
            dataSourceText += '        NumberOfDimensions = ' + \
                str(numberOfDimensions)+'\n'
            dataSourceText += '        NumberOfElements = ' + \
                str(numberOfElements)+'\n'
            dataSourceText = self.addSignalParameters(
                outputDict['value_nid'].getParent().getNode('parameters'), dataSourceText)
            dataSourceText += '      }\n'
        dataSourceText += '    }\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)

# MDSWriter management
        if configDict['storeSignals']:
            dataSourceText = '  +'+dataSourceName+'_TreeOutput = {\n'
            dataSourceText += '    Class = MDSWriter\n'

            if outputTrigger == None:
                dataSourceText += '    NumberOfBuffers = 20000\n'
            else:
                dataSourceText += '    NumberOfBuffers = ' + \
                    str(configDict['preTrigger'] +
                        configDict['postTrigger']+1)+'\n'

            dataSourceText += '    NumberOfPreTriggers = ' + \
                str(configDict['preTrigger'])+'\n'
            dataSourceText += '    NumberOfPostTriggers = ' + \
                str(configDict['postTrigger'])+'\n'
            dataSourceText += '    CPUMask = ' + \
                str(configDict['cpuMask'])+'\n'
            dataSourceText += '    StackSize = 10000000\n'
            dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
            dataSourceText += '    PulseNumber = ' + \
                str(self.getTree().shot)+'\n'
            if outputTrigger == None:
                dataSourceText += '    StoreOnTrigger = 0\n'
            else:
                dataSourceText += '    StoreOnTrigger = 1\n'
            dataSourceText += '    EventName = "'+dataSourceName+'UpdatejScope"\n'
            dataSourceText += '    TimeRefresh = 1\n'
            dataSourceText += '    Signals = {\n'
            currTimebase = self.getNode('timebase').evaluate()

            if isinstance(currTimebase, Range):
                period = currTimebase.delta.data()
            else:
                currTimebase = currTimebase.data()
                period = currTimebase[1] - currTimebase[0]


# If trigger is defined put it as first signal
            if outputTrigger != None:
                dataSourceText += '      Trigger = {\n'
                dataSourceText += '          Type = uint8\n'
                dataSourceText += '      }\n'

# If the Input device is not synchronising, store time in outputs:time
            if not isSynch:
                dataSourceText += '      Time = {\n'
                dataSourceText += '        DiscontinuityFactor = 1\n'
                dataSourceText += '        Type = '+self.getTimebaseType()+'\n'
                dataSourceText += '        NodeName = "' + \
                    configDict['outTimeNid'].getFullPath()+'"\n'
                # We must keep into account the number of samples in an input device
                dataSourceText += '        Period = ' + \
                    str(period/outputDict['samples'])+'\n'


                timeSegLen = 1000
                for outputDict in outputDicts:
                    if outputDict['seg_len'] > 0:
                        timeSegLen = outputDict['seg_len']
                dataSourceText += '        MakeSegmentAfterNWrites = '+str(timeSegLen) + '\n'
 #               dataSourceText += '        MakeSegmentAfterNWrites = 100\n'
                dataSourceText += '        AutomaticSegmentation = 0\n'
#                dataSourceText += '        Type = uint32\n'
                #if startTime != 0:
                #    dataSourceText += '        SamplePhase = ' + \
                #        str(int(round(startTime/period)))+'\n'
                if outputTrigger != None:
                    dataSourceText += '        TimeSignal = 1\n'
                dataSourceText += '      }\n'

            outIdx = 0
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    dataSourceText += '      '+outputDict['name']+' = {\n'
                    dataSourceText += '        DiscontinuityFactor = 1\n'
                    dataSourceText += '        NodeName = "' + \
                        outputDict['value_nid'].getFullPath()+'"\n'
                    # We must keep into account the number of samples in an input device
                    dataSourceText += '        Period = ' + \
                        str(period/outputDict['samples'])+'\n'
                    dataSourceText += '       MakeSegmentAfterNWrites = ' + \
                        str(outputDict['seg_len'])+'\n'
                    dataSourceText += '        AutomaticSegmentation = 0\n'
# Keep track of index of time signal for synchronizing input devices
                    if isSynch and outIdx == configDict['outTimeIdx'] and outputTrigger != None:
                        dataSourceText += '        TimeSignal = 1\n'

                   # if startTime != 0:
                   #     dataSourceText += '        SamplePhase = ' + \
                   #         str(int(round(startTime/period)))+'\n'
                    dataSourceText += '      }\n'
                outIdx = outIdx + 1
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)
# endif configDict['storeSignals']

        dataSourceText = '  +'+dataSourceName+'_Output_DDB = {\n'
        dataSourceText += '    Class = GAMDataSource\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)
        gamList.append(dataSourceName+'_DDBOutIOGAM')
        gamText = '  +'+dataSourceName+'_DDBOutIOGAM = {\n'
        gamText += '    Class = IOGAM\n'
        gamText += '    InputSignals = {\n'

        firstOut = True
        for outputDict in outputDicts:
            gamText += '      '+outputDict['name'] + ' = {\n'
            gamText += '        DataSource = '+dataSourceName+'\n'
            samples = outputDict['samples']
            gamText += '        Samples = '+str(samples)+'\n'
            if isSynch and firstOut:
                firstOut = False
                # Must be correct(will be checked before)
                period = timebase.getDescAt(2).data()
                frequency = 1./period

                gamText += '        Frequency = '+str(round(frequency, 4))+'\n'

            gamText += '      }\n'
        gamText += '    }\n'
        gamText += '    OutputSignals = {\n'

        for outputDict in outputDicts:
            gamText += '      '+outputDict['name'] + ' = {\n'
            gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
            gamText += '        Type = '+outputDict['type']+'\n'
            if outputDict['dimensions'] == 0:
                numberOfElements = 1
                numberOfDimensions = 0
            else:
                numberOfDimensions = len(outputDict['dimensions'])
                numberOfElements = 1
                for currDim in outputDict['dimensions']:
                    numberOfElements *= currDim
                    
            outputDict['elements'] = numberOfElements        
            samples = outputDict['samples']
            if samples > 1:
                gamText += '        NumberOfDimensions = 1\n'
            else:
                gamText += '        NumberOfDimensions = ' + \
                    str(numberOfDimensions)+'\n'
            gamText += '        NumberOfElements = ' + \
                str(numberOfElements * samples)+'\n'
            gamText += '      }\n'
            if self.isUsedOnAnotherThreadSynch(threadMap, outputDict['value_nid']):
                synchThreadSignals.append(outputDict)
            elif self.isUsedOnAnotherThread(threadMap, outputDict['value_nid']):
                asynchThreadSignals.append(outputDict)
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

        if configDict['storeSignals']:
            gamList.append(dataSourceName+'_TreeOutIOGAM')
            gamText = '  +'+dataSourceName+'_TreeOutIOGAM = {\n'

# GAB OCTOBER 2023
            try:
                syncDiv = self.timebase_div.data()
            except:
                synchDiv = 1

            if outputTrigger != None  and syncDiv == 1:  # If using output trigger, the trigger must be converted to uint8 GAB OCTOBER 2023
                gamText += '    Class = ConversionGAM\n'
            else:
                if syncDiv > 1:
                    gamText += '    Class = PickSampleGAM\n'
                else:
                    gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'

# MdsWriter Trigger management
            if outputTrigger != None:
                physicalTrigger = True
                try:
                    triggerNode = outputTrigger.getParent().getParent().getParent()
                    # If the trigger is pysically generated, i.e. it is derived rom another device (GAM or Input)
                    if triggerNode.getUsage() == 'DEVICE':
                        triggerGamName = self.convertPath(
                            triggerNode.getFullPath())
                        triggerSigName = outputTrigger.getParent().getNode(':name').data()
                        gamText += '      '+triggerSigName+' = {\n'
                        if self.onSameThread(threadMap, triggerNode):
                            gamText += '        DataSource = '+triggerGamName+'_Output_DDB\n'
                        # self.sameSynchSource(sourceNode):  # FIXME: sourceNode undefined
                        elif self.sameSynchSource(triggerNode):   #GAB OCTOBER 2023
                            gamText += '        DataSource = '+triggerGamName+'_Output_Synch\n'

 # GAB OCTOBER 2023                           try:
 #                               syncDiv = self.timebase_div.data()
 #                               gamText += '        Samples = ' + \
 #                                   str(syncDiv)+'\n'
 #                           except:
 #                               pass  # Consider ealTimeSynchronization downsampling only if timebase_div is defined

                        else:
                            gamText += '        DataSource = '+triggerGamName+'_Output_Asynch\n'
                        gamText += '      }\n'
                    else:
                        physicalTrigger = False
                except:
                    physicalTrigger = False

                if(not physicalTrigger):  # Trigger source is derived from a stored input waveformTimebaseTy
                    nonGamInputNodes.append({'expr': outputTrigger.decompile(
                    ), 'dimensions': 0, 'name': 'Trigger', 'col_order': False})
                    gamText += '      '+'Trigger'+' = {\n'
# GAB2022                    gamText += '        DataSource = '+dataSourceName+'_TreeInput\n'
                    gamText += '        DataSource = '+dataSourceName+'_TreeInDDB\n'
                    gamText += '        Type = uint8\n'
                    gamText += '      }\n'
# end Trigger Management

# If the Input device is not synchronising, transfer also time towards MdsWriter
            if not isSynch:
                gamText += '      Time = {\n'
# GABRIELE SEPT 2020          gamText += '      DataSource = '+dataSourceName+'_Timer_DDB'
                gamText += '      DataSource = '+timerDDB + '\n'
                gamText += '        Type = '+self.getTimebaseType()+'\n'
# GAB OCTOBER 2023                gamText += '        Type = int32\n'
                gamText += '    }\n'

# Other signals
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
                    gamText += '        NumberOfElemenrs = '+str(outputDict['elements'])
                    gamText += '        Type = '+outputDict['type']
                    gamText += '      }\n'

            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'

            if outputTrigger != None:
                gamText += '      Trigger = {\n'
                gamText += '      DataSource = '+dataSourceName+'_TreeOutput\n'
                gamText += '        type = uint8\n'
                gamText += '      }\n'

       # If the Input device is not synchronising, transfer also time towards MdsWriter
            if not isSynch:
                gamText += '      Time = {\n'
                gamText += '        Type = '+self.getTimebaseType()+'\n'  # GAB OCTOBER 2023
#                gamText += '        Type = int32\n'
                gamText += '      DataSource = '+dataSourceName + \
                    '_TreeOutput'  # GABRIELE SEPT 2020
                gamText += '    }\n'

            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_TreeOutput\n'
                    gamText += '        Type = '+outputDict['type']+'\n'
                    gamText += '        Samples = ' + \
                        str(outputDict['samples'])+'\n'
                    if outputDict['dimensions'] == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(outputDict['dimensions'])
                        numberOfElements = 1
                        for currDim in outputDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                        str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                        str(numberOfElements)+'\n'
                    gamText += '      }\n'

            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

# endif configDict['storeSignals']
# GAB 2022
        if len(nonGamInputNodes) > 0:  # For input devices this happens only if a trigger signal for MdsWriter is derived from a waveform stored in the tree
            dataSourceText = '  +'+DataSourceName+'_TreeInDDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            mdsReaderText =  '  +'+dataSourceName+ '_TreeIn = {\n'
            mdsReaderText += '    Class = MDSReaderGAM\n'
            mdsReaderText += '    TreeName = "'+self.getTree().name+'"\n'
            mdsReaderText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
            mdsReaderText += '    InputSignals = { \n'
            mdsReaderText += '      Time = {\n'
            mdsReaderText += '          DataSource = '+timerDDB + '\n' 
            mdsReaderText += '      } \n'
            mdsReaderText += '    } \n'
            mdsReaderText += '    OutputSignals = { \n'
            for nodeDict in nonGamInputNodes:
                mdsReaderText += '      '+nodeDict['name']+' = {\n'
                mdsReaderText += '        DataSource = '+dataSourceName+ '_TreeInDDB\n'
                valExpr = nodeDict['expr']
                if isinstance(valExpr, TreeNode):
                    valExpr = valExpr.getFullPath()
                valExpr = valExpr.replace('"', "'")
                mdsReaderText += '        DataExpr = "'+valExpr+'"\n'
                mdsReaderText += '        TimebaseExpr = "dim_of(' + \
                    valExpr+')"\n'
                numberOfElements = 1
                if not (np.isscalar(nodeDict['dimensions'])):
                    for currDim in nodeDict['dimensions']:
                        numberOfElements *= currDim
                mdsReaderText += '        NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
                if nodeDict['col_order']:
                    mdsReaderText += '        UseColumnOrder = 1\n'
                else:
                    mdsReaderText += '        UseColumnOrder = 0\n'
                mdsReaderText += '        DataManagement = 1\n'
                mdsReaderText += '      }\n'
                mdsReaderText += '  }\n'
            gams.append(mdsReaderText)
         # Some outputs are connected to devices on separate synchronized theads
        if len(synchThreadSignals) > 0:
            dataSourceText = '  +'+dataSourceName+'_Output_Synch = {\n'
            dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
            dataSourceText += '    Timeout = 1000000\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            
            gamList.append(dataSourceName+'_Output_Synch_IOGAM')
            gamText = '  +'+dataSourceName+'_Output_Synch_IOGAM = {\n'

            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signalDict in synchThreadSignals:
                try:
                    isInputStructField = (signalDict['value_nid'].getParent().getParent().getName() == 'FIELDS')
                except:
                    isInputStructField = False
                gamText += '      '+signalDict['name']+' = {\n'
                if isInputStructField:
                    gamText += '        DataSource = '+dataSourceName+'_Expanded_Output_DDB\n'
                else:
                    gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
                gamText += '        Type = '+signalDict['type']+'\n'
                if 'dimensions' in signalDict:
                    dimensions = signalDict['dimensions']
                    if dimensions == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(signalDict['dimensions'])
                        numberOfElements = 1
                        for currDim in signalDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signalDict in synchThreadSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_Synch\n'
                #Check if it is the reference to a resampled timebase
                gamText += '        Type = '+signalDict['type']+'\n'
                if 'dimensions' in signalDict:
                    dimensions = signalDict['dimensions']
                    if dimensions == 0:
                        numberOfElements = 1
                        numberOfDimensions = 0
                    else:
                        numberOfDimensions = len(signalDict['dimensions'])
                        numberOfElements = 1
                        for currDim in signalDict['dimensions']:
                            numberOfElements *= currDim
                    gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                    gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

        # Some outputs are connected to devices on separate synchronized threads
        if len(asynchThreadSignals) > 0:
            dataSourceText = '  +'+dataSourceName+'_Output_Asynch = {\n'
            dataSourceText += '    Class = RealTimeThreadAsyncBridge\n'
            dataSourceText += '    Timeout = 1000000\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(dataSourceName+'_Output_Asynch_IOGAM')
            gamText = '  +'+dataSourceName+'_Output_Asynch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signalDict in asynchThreadSignals:
                try:
                    isInputStructField = (signalDict['value'].getParent().getParent().getName() == 'FIELDS')
                except:
                    isInputStructField = False
                gamText += '      '+signalDict['name']+' = {\n'
                if isInputStructField:
                    gamText += '        DataSource = '+dataSourceName+'_Expanded_Output_DDB\n'
                else:
                    gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
 #               gamText += '        NumberOfDimensions = ' + \
 #                   str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signalDict in asynchThreadSignals:
                gamText += '      '+signalDict['name']+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_Asynch\n'
 #               gamText += '        NumberOfDimensions = ' + \
 #                   str(signalDict['dimensions'])+'\n'
                gamText += '        NumberOfElements = ' + \
                    str(signalDict['elements'])+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)
        return outPeriod


# OUTPUT

    def getMarteOutputInfo(self, threadMap, gams, dataSources, gamList):
        configDict = self.getGamInfo()
        dataSourceName = configDict['gamName']
        dataSourceClass = configDict['gamClass']
        timebase = configDict['timebase']
        paramDicts = configDict['paramDicts']
        inputDicts = configDict['inputDicts']
        outputDicts = configDict['outputDicts']  # TODO: unused
        outPeriod = 0  # If different from 0, this means that the corresponing component is driving the thread timing

        nonGamInputNodes = []
# timebase
        if isinstance(timebase, Range):
            period = timebase.getDescAt(2).data()
            outPeriod = period  # driving thread timing
            dataSourceText = '  +'+dataSourceName+'_Timer' + ' = {\n'
            dataSourceText += '    Class = LinuxTimer\n'
            dataSourceText += '    SleepNature = "Default"\n'
            dataSourceText += '    Signals = {\n'
            dataSourceText += '      Counter = {\n'
            dataSourceText += '        Type = uint32\n'
            dataSourceText += '      }\n'
            dataSourceText += '      Time = {\n'
            dataSourceText += '        Type = uint32\n'
            dataSourceText += '      }\n'
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

            dataSourceText = '  +'+dataSourceName+'_Timer_DDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(dataSourceName+'Timer_IOGAM')
            gamText = '  +'+dataSourceName+'Timer_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            gamText += '      Counter = {\n'
            gamText += '        DataSource = '+dataSourceName+'_Timer\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '      }\n'
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+dataSourceName+'_Timer\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '        Frequency = '+str(round(1./period, 4))+'\n'
            gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            gamText += '      Counter = {\n'
            gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
            gamText += '        Type = uint32\n'
            gamText += '      }\n'
            gamText += '      Time = {\n'
            gamText += '        DataSource = '+dataSourceName+'_Timer_DDB\n'
            gamText += '        Type = uint32\n'
            gamText += '        NumberOfElements = 1\n'
            gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

            timerDDB = dataSourceName+'_Timer_DDB'

        # Link to other component up in the chain
        elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
            prevTimebase = timebase
            while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                if isinstance(timebase, TreeNode):
                    prevTimebase = timebase
                    timebase = timebase.getData()
                else:
                    prevTimebase = TreeNode(timebase, self.getTree())
                    timebase = prevTimebase.getData()
            origName = self.convertPath(prevTimebase.getParent().getFullPath())
            # Check whether the synchronization source is a Synch Input. Only in this case, the origin DDB is its output DDB
            originMode = prevTimebase.getParent().getNode('mode').data()

            if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                if self.onSameThread(threadMap, prevTimebase.getParent()):
                    timerDDB = origName+'_Output_DDB'
                else:
                   # timerDDB = origName+'_Output_Synch' Gabriele Jan 2022
                    timerDDB = origName+'_Output_Synch'
                    try:
                        # Get period from driving synchronizing device
                        outPeriod = timebase.getDescAt(2).data()
                    except:
                        outPeriod = 0
            else:
                if self.onSameThread(threadMap, prevTimebase.getParent()):
                    timerDDB = origName+'_Timer_DDB'
                else:
                    timerDDB = origName+'_Timer_Synch'
                    try:
                        # Get period from driving synchronizing device
                        outPeriod = timebase.getDescAt(2).data()
                    except:
                        outPeriod = 0
        else:
            print('ERROR: Invalid timebase definition')
            return 0

       #Head and parameters
        gamText = '  +'+dataSourceName+'_IOGAM = {\n'
#        gamText += '    Class = IOGAM\n'
#NOTE: default behavior for Output is different from that of GAM. Here a single sample is picked when subsampling is defined (syncDiv > 1)
        gamText += '    Class = PickSampleGAM\n'

# input Signals
        gamText += '    InputSignals = {\n'
#        nonGamInputNodes = []
        signalNames = []
        signalSamples = []  # to record the same number of samples for the output IOGAM fields

        for inputDict in inputDicts:
            if not 'value' in inputDict:
                continue
            # This is a Time field referring to this timebase
            if 'value' in inputDict and isinstance(inputDict['value'], TreeNode) and inputDict['value'].getNodeName() == 'TIMEBASE' and inputDict['value'].getParent().getNid() == self.getNid():
                signalNames.append('Time')
                gamText += '      Time = {\n'
                gamText += '      DataSource = ' + timerDDB+'\n'
                try:
                    syncDiv = self.timebase_div.data()
                    gamText += '        Samples = ' + \
                        str(syncDiv)+'\n'
                    signalSamples.append(syncDiv)
                    forceUsingSamples = True
                except:
                    signalSamples.append(1)
                    pass  # Consider ealTimeSynchronization downsampling only if timebase_div is defined
               # signalSamples.append(1) Gabriele Jan 2022
            else:  # Normal reference
                isTreeRef = False
                isInputStructField = (
                    ('value' in inputDict) and isinstance(inputDict['value'], TreeNode) and inputDict['value'].getParent().getParent().getName() == 'FIELDS')
                try:
                    if isInputStructField:
                        sourceNode = inputDict['value'].getParent(
                        ).getParent().getParent().getParent().getParent()
                        aliasName = inputDict['value'].getParent().getParent().getParent().getNode(
                            ':name').data()+'_'+inputDict['value'].getParent().getNode(':name').data()
                    else:  # normal field
                        sourceNode = inputDict['value'].getParent(
                        ).getParent().getParent()
                        aliasName = inputDict['value'].getParent().getNode(
                            ':name').data()

               #   signalGamName = inputDict['value'].getParent().getNode(':name').data()
                    signalGamName = inputDict['name']
                    aliasName = inputDict['value'].getParent().getNode(
                        ':name').data()
                    sourceGamName = self.convertPath(sourceNode.getFullPath())
                except:
                    isTreeRef = True
                if isTreeRef:
                      signalName = self.convertPath(
                            inputDict['value_nid'].getPath())
                      signalNames.append(signalName)
                      signalSamples.append(1)
                      if 'value' in inputDict:
                        nonGamInputNodes.append(
                            {'expr': inputDict['value'], 'dimensions': inputDict['dimensions'], 'name': signalName, 'col_order': inputDict['col_order']})
                        gamText += '      '+signalName+' = {\n'
# GAB 2022                    gamText += '        DataSource = '+dataSourceName+'_TreeInput\n'
                        gamText += '        DataSource = '+dataSourceName+'_TreeInDDB\n'
                        if 'type' in inputDict:
                            gamText += '        Type = '+inputDict['type']+'\n'
                else:

                    # Used to force the use of Samples instead of dimensions in case of SYnch datasource
                    forceUsingSamples = False

                    signalNames.append(signalGamName)
                    gamText += '      '+signalGamName+' = {\n'
                    if isInputStructField:  
                        gamText += '        DataSource = '+sourceGamName+'_Expanded_Output_DDB\n'
                        signalSamples.append(1)
                    else:
                        if self.onSameThread(threadMap, sourceNode):
                            gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
                            signalSamples.append(1)
                        elif self.sameSynchSource(sourceNode):
                            gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'

                            try:
                                syncDiv = self.timebase_div.data()
                                gamText += '        Samples = ' + \
                                    str(syncDiv)+'\n'
                                signalSamples.append(syncDiv)
                                forceUsingSamples = True
                            except:
                                signalSamples.append(1)
                                pass  # Consider ealTimeSynchronization downsampling only if timebase_div is defined
                        else:
                            gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                            signalSamples.append(1)
                    gamText += '        Alias = ' + aliasName + '\n'
                    if 'type' in inputDict:
                        gamText += '        Type = '+inputDict['type']+'\n'

                    if 'dimensions' in inputDict and not forceUsingSamples:

                        dimensions = inputDict['dimensions']
                        if dimensions == 0:
                            numberOfElements = 1
                            numberOfDimensions = 0
                        else:
                            numberOfDimensions = len(inputDict['dimensions'])
                            numberOfElements = 1
                            for currDim in inputDict['dimensions']:
                                numberOfElements *= currDim
                        gamText += '        NumberOfDimensions = ' + \
                            str(numberOfDimensions)+'\n'
                        gamText += '        NumberOfElements = ' + \
                            str(numberOfElements)+'\n'
       #       gamText = self.addSignalParameters(inputDict['value_nid'].getParent().getNode('parameters'), gamText)
            gamText += '      }\n'
        gamText += '    }\n'

    # Output Signals IOGAM
        gamText += '    OutputSignals = {\n'
        idx = 0
        for outputDict in inputDicts:
            if not 'value' in outputDict:
                continue
            gamText += '      '+signalNames[idx]+' = {\n'
            gamText += '        Samples = 1\n'

            idx = idx+1
            gamText += '        DataSource = '+dataSourceName+'\n'
            gamText += '        Type = '+outputDict['type']+'\n'

            if outputDict['dimensions'] == 0:
                numberOfElements = 1
                numberOfDimensions = 0
            else:
                numberOfDimensions = len(outputDict['dimensions'])
                numberOfElements = 1
                for currDim in outputDict['dimensions']:
                    numberOfElements *= currDim
            gamText += '        NumberOfDimensions = ' + \
                str(numberOfDimensions)+'\n'
            gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
            gamText = self.addSignalParameters(outputDict['value_nid'].getParent().getNode(
                'parameters'), gamText)  # For output devices parameters are copied to out
            gamText += '      }\n'
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

        if len(nonGamInputNodes) > 0: #GAB 2022
            gamList.append(dataSourceName+'_TreeIn')
        gamList.append(dataSourceName+'_IOGAM')

        # There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        if len(nonGamInputNodes) > 0:
            dataSourceText = '  +'+DataSourceName+'_TreeInDDB = {\n'
            dataSourceText += '    Class = GAMDataSource\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            mdsReaderText =  '  +'+dataSourceName+ '_TreeIn = {\n'
            mdsReaderText += '    Class = MDSReaderGAM\n'
            mdsReaderText += '    TreeName = "'+self.getTree().name+'"\n'
            mdsReaderText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
            mdsReaderText += '    InputSignals = { \n'
            mdsReaderText += '      Time = {\n'
            mdsReaderText += '          DataSource = '+timerDDB + '\n' 
            mdsReaderText += '      } \n'
            mdsReaderText += '    } \n'
            mdsReaderText += '    OutputSignals = { \n'
            for nodeDict in nonGamInputNodes:
                mdsReaderText += '      '+nodeDict['name']+' = {\n'
                mdsReaderText += '        DataSource = '+dataSourceName+ '_TreeInDDB\n'
                valExpr = nodeDict['expr']
                if isinstance(valExpr, TreeNode):
                    valExpr = valExpr.getFullPath()
                valExpr = valExpr.replace('"', "'")
                mdsReaderText += '        DataExpr = "'+valExpr+'"\n'
                mdsReaderText += '        TimebaseExpr = "dim_of(' + \
                    valExpr+')"\n'
                numberOfElements = 1
                if not (np.isscalar(nodeDict['dimensions'])):
                    for currDim in nodeDict['dimensions']:
                        numberOfElements *= currDim
                mdsReaderText += '        NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
                if nodeDict['col_order']:
                    mdsReaderText += '        UseColumnOrder = 1\n'
                else:
                    mdsReaderText += '        UseColumnOrder = 0\n'
                mdsReaderText += '        DataManagement = 1\n'
                mdsReaderText += '      }\n'
                mdsReaderText += '  }\n'
            gams.append(mdsReaderText)
       #Head and parameters
        dataSourceText = '  +'+dataSourceName+' = {\n'
        dataSourceText += '    Class = '+dataSourceClass+'\n'
# parameters
        dataSourceText = self.reportParameters(paramDicts, dataSourceText, 1)

# input Signals
        dataSourceText += '    Signals = {\n'
        idx = 0
        for inputDict in inputDicts:
            if not 'value' in inputDict:
                continue
            dataSourceText += '      '+signalNames[idx]+' = {\n'
            idx = idx+1
            if 'type' in inputDict:
                dataSourceText += '        Type = '+inputDict['type']+'\n'
            if 'dimensions' in inputDict:
                dimensions = inputDict['dimensions']
                if dimensions == 0:
                    numberOfElements = 1
                    numberOfDimensions = 0
                else:
                    numberOfDimensions = len(inputDict['dimensions'])
                    numberOfElements = 1
                    for currDim in inputDict['dimensions']:
                        numberOfElements *= currDim
                dataSourceText += '        NumberOfDimensions = ' + \
                    str(numberOfDimensions)+'\n'
                dataSourceText += '        NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
            dataSourceText = self.addSignalParameters(
                inputDict['value_nid'].getParent().getNode('parameters'), dataSourceText)
            dataSourceText += '      }\n'
        dataSourceText += '    }\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)
        return outPeriod

    def getMarteInfo(self, threadMap, gams, dataSources, gamList, typeDicts):
        self.prepareMarteInfo()
        mode = self.mode.data()
        if mode == MARTE2_COMPONENT.MODE_GAM:
            return self.getMarteGamInfo(threadMap, gams, dataSources, gamList, typeDicts)
        elif mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:

            return self.getMarteInputInfo(threadMap, gams, dataSources, gamList, True)
        elif mode == MARTE2_COMPONENT.MODE_INPUT:
            return self.getMarteInputInfo(threadMap, gams, dataSources, gamList, False)
        else:
            return self.getMarteOutputInfo(threadMap, gams, dataSources, gamList)


# Utility methods

    # return information about the connection. Returned dictionary fields: gam_class, gam_nid, dimensions, type, samples. None if the input is not connected
    def getInputChanInfo(self, valNid):
        try:
            chanNid = valNid.getData()
            # Link to other component up in the chain
            if not(isinstance(chanNid, TreeNode) or isinstance(chanNid, TreePath)):
                return None
            if isinstance(chanNid, TreePath):
                chanNid = TreeNode(chanNid, self.getTree())
            chanNid = chanNid.getParent()
            retInfo = {}
            retInfo['gam_nid'] = chanNid.getParent().getParent()
            retInfo['gam_class'] = chanNid.getParent().getParent().getNode(
                ':GAM_CLASS').data()  # Must not generate exception
            try:
                retInfo['dimensions'] = chanNid.getNode(':DIMENSIONS').data()
            except:
                retInfo['dimensions'] = None
            try:
                retInfo['type'] = chanNid.getNode(':TYPE').data()
            except:
                retInfo['type'] = None
            try:
                retInfo['samples'] = chanNid.getNode(':SAMPLES').data()
            except:
                retInfo['samples'] = None
        except:  # not connected
            return None
        return retInfo


# Prpeparatory work before reading MARTe2 information. To be overridden by subclasses/
    def prepareMarteInfo(self):
        pass


# Check timebase generation
    def checkTimebase(self, threadMap):
        mode = self.mode.getData()
        if mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
            return MARTE2_COMPONENT.TIMEBASE_GENERATOR

        # Will always succeed since it is called AFTER checkGeneric
        timebase = self.timebase.getData()
        if isinstance(timebase, Range):
            # Note this will be overridden by Sync
            return MARTE2_COMPONENT.TIMEBASE_GENERATOR

        prevTimebase = timebase  # Checks no more needed here
        while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
            if isinstance(timebase, TreeNode):
                prevTimebase = timebase
                timebase = timebase.getData()
            else:
                prevTimebase = TreeNode(timebase, self.getTree())
                timebase = prevTimebase.getData()

        if self.onSameThread(threadMap, prevTimebase.getParent()):
            return MARTE2_COMPONENT.TIMEBASE_FROM_SAME_THREAD
        else:
            return MARTE2_COMPONENT.TIMEBASE_FROM_ANOTHER_THREAD

# Generic consistency check valid for every MARTE2_COMPONENT instance
    def checkGeneric(self, threadMap):

       # Check Timebase
        try:
            timebase = self.timebase.getData()
        except:
            return "Missing timebase"
        if isinstance(timebase, Range):
            try:
                period = timebase.getDescAt(2).data()
                if(period <= 0):
                    return 'Invalid timebase period: '+str(period)
            except:
                return 'Invalid timebase period'
        # Link to other component up in the chain
        elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
            prevTimebase = timebase
            try:
                while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
                    if isinstance(timebase, TreeNode):
                        prevTimebase = timebase
                        timebase = timebase.getData()
                    else:
                        prevTimebase = TreeNode(timebase, self.getTree())
                        timebase = prevTimebase.getData()
                if prevTimebase.getParent().getUsage() != 'DEVICE':
                    return 'Invalid timebase reference '+prevTimebase.getParent().getUsage()
                gamClass = prevTimebase.getParent().getNode(
                    ':GAM_CLASS')  # Make sure it is a MARTe2 device
                if prevTimebase.getNodeName() != 'TIMEBASE':
                    return 'Invalid timebase reference: ' + prevTimebase().getNodeName()
            except:
                return 'Invalid timebase reference'
# Check Inputs

        configDict = self.getGamInfo()
        inputDicts = configDict['inputDicts']
        inputIdx = 0
        for inputDict in inputDicts:
            inputIdx += 1
            if 'value' in inputDict and isinstance(inputDict['value'], TreeNode) and inputDict['value'].getNodeName() == 'TIMEBASE':
                # Must a Time field referring to this timebase
                if inputDict['value'].getParent().getNid() != self.getNid():
                    return 'Invalid input: TIMEBASE must refer to the same device'
                continue
            isTreeRef = False
            try:
                sourceNode = inputDict['value'].getParent(
                ).getParent().getParent()
                if sourceNode.getUsage() != 'DEVICE':
                    isTreeRef = True
                else:
                        # FIXME: all three vars unused
                    # Make sure it is a MARTe2 device
                    gamClass = sourceNode.getNode('GAM_CLASS')
                    sourceGamName = self.convertPath(sourceNode.getFullPath())
                    signalGamName = inputDict['value'].getParent().getNode(
                        ':name').data()
            except:
                isTreeRef = True
            if isTreeRef:
                continue  # We cannot say anything if it is a reference to a a node that is not the output of a MARTe2 device

            if inputDict['value'].getParent().getParent().getNodeName() != 'OUTPUTS':
                return 'Input '+str(inputIdx)+' is not the output field of a MARTe2 device'
            inType = inputDict['value'].getParent().getNode(':TYPE').getData()
            if inType != inputDict['type']:
                return 'Type mismatch for input '+str(inputIdx)+': expected '+inputDict['type']+'  found '+inType
            if not np.isscalar(inputDict['dimensions']) and len(inputDict['dimensions']) == 1 and inputDict['dimensions'][0] == inputDict['value'].getParent().getNode(':SAMPLES').getData():
                continue  # for ADC producing a set of sampled every cycle

            if self.checkTimebase(threadMap) == MARTE2_COMPONENT.TIMEBASE_FROM_ANOTHER_THREAD:
                try:
                    syncDiv = self.timebase_div.data()
                except:
                    syncDiv = 1
            else:
                syncDiv = 1
            inDimensions = inputDict['value'].getParent().getNode(
                ':DIMENSIONS').getData()
            if inDimensions != inputDict['dimensions']:
                return 'Dimension mismatch for input '+str(inputIdx)+': expected '+str(inputDict['dimensions'])+'  found '+str(inDimensions)

        return ''

# Specific consistency check to be implemented by MARTE2_COMPONENT subclasses
    def checkSpecific(self):
        return ''

# Final check method

    def check(self, threadMap):
        status = self.checkGeneric(threadMap)

        if status != '':
            return status
        return self.checkSpecific()


class BUILDER:
    def __init__(self, clazz, mode, timebaseExpr=None):
        self.clazz = clazz
        self.mode = mode
        self.timebaseExpr = timebaseExpr

    def __call__(self, cls):
        cls.buildGam(cls.parts, self.clazz, self.mode, self.timebaseExpr)
        return cls
