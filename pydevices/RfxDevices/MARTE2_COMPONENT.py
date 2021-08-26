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
import RfxDevices
import numpy as np
import copy


class BUILDER:
    def __init__(self, clazz, mode, timebaseExpr=None):
        self.clazz = clazz
        self.mode = mode
        self.timebaseExpr = timebaseExpr

    def __call__(self, cls):
        cls.buildGam(cls.parts, self.clazz, self.mode, self.timebaseExpr)
        return cls


class MARTE2_COMPONENT(MDSplus.Device):
 




    """MARTE2 components superclass"""
    MODE_GAM = 1
    MODE_INPUT = 2
    MODE_SYNCH_INPUT = 3
    MODE_OUTPUT = 4
    MODE_INTERFACE = 5

    @classmethod
    def buildParameters(cls, parts, prefix = 'PARAMETERS', buildParameters = None):
        idx = 1
        if buildParameters == None:
            buildParameters = cls.parameters
        for parameter in buildParameters:
            parts.append({'path': prefix + '.PAR_' +
                          str(idx), 'type': 'structure'})
            parts.append({'path': prefix + '.PAR_'+str(idx) +
                          ':NAME', 'type': 'text', 'value': parameter['name']})
            if(parameter['type'] == 'structure'):
                parts.append({'path': prefix + '.PAR_'+str(idx) +
                                  '.VALUE', 'type': 'structure'}) 
                cls.buildParameters(parts,  prefix = prefix + '.PAR_'+str(idx) +'.VALUE', buildParameters = parameter['value'])
            elif(parameter['type'] == 'string'):
                if 'value' in parameter:
                    parts.append({'path': prefix + '.PAR_'+str(idx) +
                                  ':VALUE', 'type': 'text', 'value': parameter['value']})
                else:
                    parts.append({'path': prefix + '.PAR_' +
                                  str(idx)+':VALUE', 'type': 'text'})
            else:
                if 'value' in parameter:
                    parts.append({'path': prefix + '.PAR_'+str(idx)+':VALUE',
                                  'type': 'numeric', 'value': parameter['value']})
                else:
                    parts.append({'path': prefix + '.PAR_' +
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
        nameList = []
        cls.buildOutputsRec(parts, currOutputs = cls.outputs, prefix = '.OUTPUTS')

    @classmethod
    def buildOutputsRec(cls, parts, currOutputs = None, prefix = '', nameList = []):
        for output in currOutputs:
            #        sigName = output['name'][0:12]
            sigName = cls.convertName(output['name'], nameList)
            parts.append({'path': prefix + '.'+sigName, 'type': 'structure'})
            parts.append({'path': prefix + '.'+sigName+':NAME',
                          'type': 'text', 'value': output['name']})
            parts.append({'path': prefix + '.'+sigName+':TYPE',
                          'type': 'text', 'value': output['type']})
            parts.append({'path': prefix + '.'+sigName+':SAMPLES',
                          'type': 'numeric', 'value': 1})
            parts.append({'path': prefix + '.'+sigName+':DIMENSIONS',
                          'type': 'numeric', 'value': MDSplus.Data.compile(str(output['dimensions']))})
            if 'seg_len' in output:
                parts.append({'path': prefix + '.'+sigName+':SEG_LEN',
                              'type': 'numeric', 'value': output['seg_len']})
            else:
                parts.append({'path': prefix + '.'+sigName +
                              ':SEG_LEN', 'type': 'numeric', 'value': 0})
            if 'stream' in output:
                parts.append({'path': prefix + '.'+sigName+':STREAM',
                              'type': 'text', 'value': output['stream']})
            else:
                parts.append({'path': prefix + '.'+sigName +
                              ':STREAM', 'type': 'text'})

            if(output['type'] == 'string'):
                parts.append(
                    {'path': prefix + '.'+sigName+':VALUE', 'type': 'text'})
            else:
                parts.append({'path': prefix + '.'+sigName +
                              ':VALUE', 'type': 'signal'})
            try:
                pars = output['parameters']
            except:
                pars = []
            parts.append({'path': prefix + '.'+sigName +
                          '.PARAMETERS', 'type': 'structure'})
            if len(pars) > 0:
                cls.buildParameters(parts, prefix = prefix + '.'+sigName+'.PARAMETERS', buildParameters = output['parameters'])

            # structured outputs
            try:
                fields = output['fields']
            except:
                fields = []
            parts.append({'path': prefix+'.'+sigName + '.FIELDS', 'type': 'structure'})
            if len(fields) > 0:
                cls.buildOutputsRec(parts, currOutputs = fields, prefix = prefix+'.'+sigName+'.FIELDS', nameList=nameList)

    @classmethod
    def buildInputs(cls, parts):
        parts.append({'path': '.INPUTS', 'type': 'structure'})
        nameList = []
        cls.buildInputsRec(parts, inputs = cls.inputs, prefix = '.INPUTS')

    @classmethod
    def buildInputsRec(cls, parts, nameList = [], prefix = '', inputs = None):

        for input in inputs:
            #        sigName = input['name'][0:12]
            sigName = cls.convertName(input['name'], nameList)
            parts.append({'path': prefix+'.'+sigName, 'type': 'structure'})
            parts.append({'path': prefix+'.'+sigName+':TYPE',
                          'type': 'text', 'value': input['type']})
            parts.append({'path': prefix+'.'+sigName+':DIMENSIONS',
                          'type': 'numeric', 'value': MDSplus.Data.compile(str(input['dimensions']))})
            parts.append({'path': prefix+'.'+sigName+':COL_ORDER',
                          'type': 'text', 'value': 'NO'})
            parts.append({'path': prefix+'.'+sigName +
                          ':VALUE', 'type': 'signal'})
            if 'name' in input:
                parts.append({'path': prefix+'.'+sigName+':NAME',
                              'type': 'text', 'value': input['name']})
            else:
                parts.append(
                    {'path': prefix+'.'+sigName+':NAME', 'type': 'text'})

            try:
                pars = input['parameters']
            except:
                pars = []
            parts.append({'path': prefix+'.'+sigName +
                          '.PARAMETERS', 'type': 'structure'})
            if len(pars) > 0:
                cls.buildParameters(parts, prefix = prefix + '.'+sigName+'.PARAMETERS', buildParameters = input['parameters'])

            try:
                fields = input['fields']
            except:
                fields = []
            parts.append({'path': prefix+'.'+sigName + '.FIELDS', 'type': 'structure'})
            if len(fields) > 0:
                cls.buildInputsRec(parts, nameList = nameList, prefix = prefix + '.'+sigName+'.FIELDS', inputs = fields)

    @classmethod
    def buildGam(cls, parts, clazz, mode, timebaseExpr=None):

        parts.append({'path': ':GAM_CLASS', 'type': 'text', 'value': clazz})
        parts.append({'path': ':MODE', 'type': 'numeric', 'value': mode})
        if mode != MARTE2_COMPONENT.MODE_INTERFACE:
            if timebaseExpr == None:
                parts.append({'path': ':TIMEBASE', 'type': 'numeric'})
            else:
                parts.append({'path': ':TIMEBASE', 'type': 'numeric',
                            'value': MDSplus.Data.compile(timebaseExpr)})

        parts.append({'path': '.PARAMETERS', 'type': 'structure'})
        cls.buildParameters(parts)
        if (mode == MARTE2_COMPONENT.MODE_GAM or mode == MARTE2_COMPONENT.MODE_OUTPUT) and mode != MARTE2_COMPONENT.MODE_INTERFACE:
            cls.buildInputs(parts)

        if mode != MARTE2_COMPONENT.MODE_OUTPUT and mode != MARTE2_COMPONENT.MODE_INTERFACE:
            cls.buildOutputs(parts)
        if mode != MARTE2_COMPONENT.MODE_INTERFACE:
            parts.append({'path': ':PORT', 'type': 'numeric'})
    # add debug definition. When 'ENABLED' input and outputs will be printed via LoggerDataSOurce
            parts.append(
                {'path': ':PRINT_DEBUG', 'type': 'text', 'value': 'DISABLED'})
            parts.append(
                {'path': ':JSCOPE_EV', 'type': 'text', 'value': 'updatejScope'})

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
    # NON USATA QUA PER IL MOMENTO
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
    
    def getParametersDict(self, parRoot):   
        pars = {}
        for parNode in parRoot.getChildren():
            try:
                name = parNode.getNode('name').data()
            except:
                continue #Skip parameter if missing name
            valNode = parNode.getNode('value')
            childPars = valNode.getChildren()
            if len(childPars) > 0:
                val = self.getParametersDict(valNode)
                if val == {}:
                    continue
            else:
                try:
                    val = parNode.getNode('value').getData()
                except:
                    print('Warning: Empty value for parameter '+parNode.getPath())
                    continue
                if isinstance(val, MDSplus.Apd):
                    if not isinstance(val, MDSplus.Dictionary):
                        raise Exception('Wrong parameter in ' + parNode.getPath()+' Only Dictionaries, string, scalar and arrays supported')
                else:
                    val = val.data()
            pars[name] = val
        return pars
    
    # get the name associated with that signal node subtree, including name of nested signals. Valid for bot input and output signals
    def getSignalName(self, sigNode):
   #passed sigNode may either refer to a VALUE field or to its parent
        if sigNode.getName() == 'VALUE':
            currSigNode = sigNode.getParent()
        else:
            currSigNode = sigNode
        try:
            nameNode = currSigNode.getNode(':NAME')
        except:
            raise Exception('Internal error in getSignal node for '+sigNode.getPath())

        try:
            name = nameNode.data()
        except:
            name = currSigNode.getName()

        while not isinstance(currSigNode.getParent().getParent(), RfxDevices.MARTE2_COMPONENT):
            currSigNode = currSigNode.getParent().getParent()
            try:
                currName = currSigNode.getNode('NAME').data()
            except:
                currName = currSigNode.getName()
            name = currName+'_'+name
        
        return name
    


    #return the containing MARTE2 device 
    def getMarteDevice(self, node):
        currNode = node
        while not isinstance(currNode, RfxDevices.MARTE2_COMPONENT):
            if currNode.getParent().getNid() == 0: #TOP reached
                raise Exception('Node '+ node.getPath() +' is not contained in a MARTe2 device')
            currNode = currNode.getParent()
        return currNode


   #return the containing MARTE2 device name. The name is obtained from the full path (except \\experiment::TOP) replacing : and . with _ 
    def getMarteDeviceName(self, node = None):
        if node == None:
            node = self
        currNode = self.getMarteDevice(node)
        devName = currNode.getFullPath()
        return devName[devName.index('::TOP')+6:].replace(':', '_').replace('.', '_')
    
    #Check whether the passed value is a reference to TIMEBASE of this device
    def isTimebase(self, value):
      if isinstance(value, MDSplus.TreeNode) and value.getNid() == self.getNode('TIMEBASE').getNid():
          return True
      return False

    #Check whether the value refers to a MARTe2 device
    def isMarteDeviceRef(self, value):
        try:
            self.getMarteDevice(value)            
        except:
            return False
        return True

    def checkMarteDeviceRef(self, value, sigNode): 
        #If we arrive here, value refers to MARTe2 device.
        #Consistency checks
        refSigNode = value.getParent()
        try:
            refTypeNode = refSigNode.getNode('TYPE')
        except:
            raise Exception ('Not a valid MARTE2 device reference for '+ self.getSignalName(sigNode) + ' of '+self.getPath()+ ':  '+ value()) 

        if refTypeNode.data() != sigNode.getNode('TYPE').data():
            raise Exception ('Invalid type for signal '+ self.getSignalName(sigNode) + ' of '+self.getPath()+':'+value.getPath()+': '+refTypeNode.data()+'  '+sigNode.getNode('TYPE').data()) 
        try:
            refDimNode = refSigNode.getNode('DIMENSIONS')
        except:
            raise Exception ('Not a valid MARTE2 device reference for '+ sigNode + ' of '+self.getPath()) 
        if refDimNode.data() != sigNode.getNode('DIMENSIONS').data():
            raise Exception ('Invalid dimension for signal '+ sigNode.getPath()+ ' of '+self.getPath() ) 
        return True

    #check whether  MARTe2 thread referred by value belongs to the same thread for this device 
    def hostedInSameThread(self, value, threadMap):
        if not self.hostedInSameSupervisor(value, threadMap):
            return False
        # threadMap is a Dictionary with two keys:
        # 'DeviceInfo': defining for every MARTe2 device NID a dictionary:
        #       'ThreadName': name of the thread it belongs to
        #       'SupervisorNid': NID of the supervirsor hosting the device
        #       'SupervisorIp': IP address of the superisor hosting the device
        #       'DevicePort': Posr number foir that device
        # 'ThreadInfo': defining for every thread for this supervisor a dictionary:
        #       'SyncThreadName': The name of the Synchronizing thread or None if this thread is not synchronized
        #       'SyncThreadSupervisor': The NID of the MARTE2 supervisor hosting the synchronizing thread or None if not synchronized
        #       'SyncDiv': Frequency division from synchronizing thread or None is not synchronized
  

        valueNid = self.getMarteDevice(value).getNid()
        if not valueNid in threadMap['DeviceInfo'].keys():
          raise Exception('Internal error: a device input may refer to the output of a device not considered for this state')
        thisNid = self.getNid()
        return (threadMap['DeviceInfo'][valueNid]['ThreadName'] == threadMap['DeviceInfo'][thisNid]['ThreadName']) and \
             (threadMap['DeviceInfo'][valueNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid'])

    #check whether MARTe2 device referred by value belongs to the same supervisor
    def hostedInSameSupervisor(self, value, threadMap):
      valueNid = self.getMarteDevice(value).getNid() 
      if not valueNid in threadMap['DeviceInfo'].keys():
        raise Exception('Internal error: a device input may refer to the output of a device not considered for this state')
      return threadMap['DeviceInfo'][self.getNid()]['SupervisorNid'] == threadMap['DeviceInfo'][valueNid]['SupervisorNid']
      
    #Check  whether MARTe2 thread synchronizes the thread for this device 
    def hostedInSynchronizingThread(self, value, threadMap):   
        valueNid = self.getMarteDevice(value).getNid()
        if not valueNid in threadMap['DeviceInfo'].keys():
          raise Exception('Internal error: a device input may refer to the output of a device not considered for this state')
        thisNid = self.getNid()
        refThreadName = threadMap['DeviceInfo'][valueNid]['ThreadName']
        thisThreadName = threadMap['DeviceInfo'][thisNid]['ThreadName']
        refSupervisorNid = threadMap['DeviceInfo'][valueNid]['SupervisorNid']
        thisSupervisorNid = threadMap['DeviceInfo'][thisNid]['SupervisorNid']

        return threadMap['ThreadInfo'][thisThreadName]['SyncThreadName'] == refThreadName and \
            threadMap['ThreadInfo'][thisThreadName]['SyncThreadSupervisor'] == refSupervisorNid

    #Get subsamplingRatio for synchronized thread
    def getSubsamplingRatio(self, value, threadMap):
        valueNid = self.getMarteDevice(value).getNid()
        thisNid = self.getNid()
        refThreadName = threadMap['DeviceInfo'][valueNid]['ThreadName']
        thisThreadName = threadMap['DeviceInfo'][thisNid]['ThreadName']
        refSupervisorNid = threadMap['DeviceInfo'][valueNid]['SupervisorNid']
        thisSupervisorNid = threadMap['DeviceInfo'][thisNid]['SupervisorNid']

        if not (threadMap['ThreadInfo'][thisThreadName]['SyncThreadName'] == refThreadName and \
            threadMap['ThreadInfo'][thisThreadName]['SyncThreadSupervisor'] == refSupervisorNid):
            raise Exception('Internal error: getSubsamplingRation called for non sychronized threads')

        return  threadMap['ThreadInfo'][thisThreadName]['SyncDiv']

    #returns the type of the referenced output
    def getReferencedType(self, value):
        try:
            return value.getParent().getNode('TYPE').data()
        except:
            raise Exception('Internal error: getReferenced Type failed for node '+value.getPath())


    # extends type definition. TypeDict is organized as a Dictionary whose keys are the assigned type names ('Type1, Type2, ....') and the corresponding 
    # contents are lists of dicts corresponding to the type fields. At every field corresponds a dictionaty with the following fields
    # 'Name': name of the filed
    # 'Type' : type of the field. Can be a native type or in turn the assigned name of an existing type
    # 'NumberOfElememts': number of instances (array)
    # buildTypes makes sure that the same structure is never replicated in diferent types
    def getTypeName(self, fieldsRoot, typesDict):
        typeName = fieldsRoot.getParent().getNode('NAME').data()+'_Type'
        retType = []
        for fieldNode in fieldsRoot.getChildren():
            currField = {}
            currField['Name'] = fieldNode.getNode('NAME').data()
            try:
                numFields = len(fieldNode.getNode('FIELDS').getChildren())
            except:
                numFields = 0
            if numFields > 0:
                currField['Type'] = self.getTypeName(fieldNode.getNode('FIELDS'), typesDict)
            else:
                try:
                    currField['Type'] = fieldNode.getNode('TYPE').data()
                except:
                    raise Exception("Missing required type definition for "+fieldNode.getPath())
            numDims, numEls = self.parseDimension(fieldNode.getNode('DIMENSIONS').data())
            currField['NumberOfElements'] = numEls
            retType.append(currField)
        
        typesDict[typeName] = retType
        return typeName 

    #Check whether the type is a legal one (either builtin or defines in typesDict)
    def checkNativeType(self, inType, typesDict): 
        builtinTypes = ['int8', 'uint8', 'int16', 'uint16', 'char8', 'int32', 'uint32', 'int64', 'uint64', 'float32', 'float64']
        if inType in builtinTypes:
            return
        if inType in typesDict.keys():
            return
        raise Exception('Unrecognized type for '+self.getPath()+' : '+inType)

    # Returns None if the passed definition is not contained in typesDict
    def checkTypeName(self, typeDef, typesDict):
        for typeName in typesDict.keys():
            if self.sameType(typesDict[typeName], typeDef):
                return typeName
        return None


    # Check if the two types are idenycal (including field names)
    def sameType(self, typeDef1, typeDef2):
        if len(typeDef1) != len(typeDef2):
            return False
        for idx in range(len(typeDef1)):
            if typeDef1[idx]['Name'] != typeDef2[idx]['Name']:
                return False
            if typeDef1[idx]['Type'] != typeDef2[idx]['Type']:
                return False
            if typeDef1[idx]['NumberOfElements'] != typeDef2[idx]['NumberOfElements']:
                return False
        return True

    # Convert dimensions array into NumberOfDimensions, NumberOfElements 
    def parseDimension(self, dimensions):
        if np.isscalar(dimensions):
            if dimensions != 0:
                raise Exception('Scalar value for dimensions can be only 0 '+str(dimensions) + '   in  '+self.getPath()) 
            numEls = 1
            numDims = 0
        else:
            numDims = len(dimensions)
            numEls = 1
            for dim in dimensions:
                numEls *= dim
        return numDims, numEls

    #return a flattened list of structure fields ORDERED BY NID NUMBER. Valid for both Inputs and Outputs
    def getFlattenedFields(self, fieldRoot):
        fieldNodes = fieldRoot.getChildren()
        fieldNodes.sort()
        retFields = []
        for fieldNode in fieldNodes:
            try:
                numFields = len(fieldNode.getNode('FIELDS').getChildren())
            except:
                numFields = 0
            if numFields > 0:
                retFields += self.getFlattenedFields(fieldNode.getNode('FIELDS'))
            else:
                retFields.append(fieldNode)
        return retFields


    #Build the representation of the Input signals of a MARTe2 device (Array of dictionaries) and complete the auxiliary
    #passed information 
    def getInputSignalsDict(self, sigNodes, threadMap, typesDict, resampledSyncSigs, syncInputsToBeReceived, 
                            asyncInputsToBeReceived, treeRefs, constRefs, inputsToBePacked, isFieldCheck = False):
        sigDicts = []
        for sigNode in sigNodes:
            currSig = {}
#Value. Missing value forces skipping the input. Here skipping will be carried out also for struct inputs that are defined element by element
            try:
                value = sigNode.getNode(':VALUE').getData()
            except:
                if isFieldCheck:
                    #If isFieldCheck getInputSignalsDict is working on the flattened field nodes that are assured not to  be empty
                    raise Exception('Internal error: Unexpected empty field for '+sigNode.getPath())
                try:
                    numFields = len(sigNode.getNode('FIELDS').getChildren())
                except:
                    numFields = 0
                if numFields == 0:
                    continue #Missing value for non strct inputs forces skipping the input.
 # we need to handle here the possible case of a structured input
 # getFlattenedFields check also that if a field is non empty, then all fields must be nonempty. If all fields are empty, [] is returned
                fieldNodes = self.getFlattenedFields(sigNode.getNode('FIELDS'))
                fieldDicts = {}
                fieldDicts['Inputs'] = self.getInputSignalsDict(fieldNodes, threadMap, typesDict, resampledSyncSigs, syncInputsToBeReceived,
                                                     asyncInputsToBeReceived, treeRefs, constRefs, [], True)
                fieldDicts['Name'] = self.getSignalName(sigNode)
                fieldDicts['Type'] = self.getTypeName(sigNode.getNode('FIELDS'), typesDict)
                inputsToBePacked.append(fieldDicts)
                value = None
#Name
            currSig['Name'] = self.getSignalName(sigNode)
#Type
            try:
                numFields = len(sigNode.getNode('FIELDS').getChildren())
            except:
                numFields = 0

            if numFields > 0:
                type = self.getTypeName(sigNode.getNode('FIELDS'), typesDict)
            else:
                try:
                    type = sigNode.getNode(':TYPE').data()
                except:
                    raise Exception('Missing type definition for '+sigNode.getNode(':TYPE').getPath())

            currSig['Type'] = type

#NumberOfElements, NumberOfDimensions
            if numFields == 0:
                try:
                    dimensions = sigNode.getNode(':DIMENSIONS').data()
                except:
                    raise Exception('Missing dimension definition for '+sigNode.getNode(':DIMENSIONS').getPath())
                currSig['NumberOfDimensions'], currSig['NumberOfElements'] = self.parseDimension(dimensions)
            else: #Arrays of Structures not suppported 
                currSig['NumberOfDimensions'] = 0
                currSig['NumberOfElements'] = 1
#samples    
            try:
                samples = sigNode.getNode(':SAMPLES').data()
            except:
                samples = 1
            currSig['Samples'] = samples
 #Parameters
            if not isFieldCheck:
                try:
                    numParameters = len(sigNode.getNode('PARAMETERS').getChildren())
                except:
                    numParameters = 0
            else:
                numParameters = 0
            if numParameters > 0:
                currSig['Parameters'] = self.getParametersDict(sigNode.getNode('PARAMETERS'))
#Data Source
            if value == None: #Handle the case the structured input has been defined by means of its fields
                currSig['DataSource'] = self.getMarteDeviceName(sigNode)+'_Input_Bus_DDB'
                sigDicts.append(currSig)
                continue

            if self.isTimebase(value):
                currSig['DataSource'] = self.timerDDB
                currSig['Type'] = self.timerType
                currSig['Alias'] = 'Time'
                currSig['NumberOfElements'] = 1
                currSig['NumberOfDimensions'] = 0
                sigDicts.append(currSig)
                continue
                
                
            if self.isMarteDeviceRef(value): #If it is the reference to the output of another MARTe2 device (consistency checks also carried out) 
                self.checkMarteDeviceRef(value, sigNode) #Perform consistecny checks, raise Exception on case
                currSig['Alias'] = self.getSignalName(value)
                if self.hostedInSameThread(value, threadMap):  #Reference within same thread
                    if isFieldCheck:
                        currSig['DataSource'] = self.getMarteDeviceName(value)+'_Expanded_Output_DDB'
                    else:
                        currSig['DataSource'] = self.getMarteDeviceName(value)+'_Output_DDB'
                else:
                    if self.hostedInSameSupervisor(value, threadMap): #Reference within same supervisor
                        if self.hostedInSynchronizingThread(value, threadMap):
                            subsamplingRatio = self.getSubsamplingRatio(value, threadMap)
                            if  subsamplingRatio > 1:
                                currSig['DataSource'] = self.getMarteDeviceName(sigNode)+'_Res_DDB'
                                resampledSyncSigs.append({'Name':currSig['Name'], 
                                                      'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                                      'Type': currSig['Type'],
                                                      'NumberOfDimensions': currSig['NumberOfDimensions'],
                                                      'NumberOfElements' : currSig['NumberOfElements'],
                                                      'Alias' : currSig['Alias'],
                                                      'Samples': subsamplingRatio * samples,
                                                      })
                            else:
                                currSig['DataSource'] = self.getMarteDeviceName(value)+'_Output_Sync'
                        else:
                            currSig['DataSource'] = self.getMarteDeviceName(value)+'_Output_Async'
                    else: #Reference from another supervisor
                        if self.hostedInSynchronizingThread(value, threadMap):
                            subsamplingRatio = self.getSubsamplingRatio(value, threadMap)
                            syncInputsToBeReceived.append({'Name':currSig['Name'], 
                                                      'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                                      'Type': currSig['Type'],
                                                      'NumberOfDimensions': currSig['NumberOfDimensions'],
                                                      'NumberOfElements' : currSig['NumberOfElements'],
                                                      'Alias' : currSig['Alias'],
                                                      'Samples': subsamplingRatio * samples,
                                                      })
                            subsamplingRatio = self.getSubsamplingRatio(value, threadMap)
                            if subsamplingRatio > 1:
                                currSig['DataSource'] = self.getMarteDeviceName(sigNode)+'_Res_DDB'
                                resampledSyncSigs.append({'Name':currSig['Name'], 
                                                        'DataSource': self.getMarteDeviceName(self)+'_RTN_IN_DDB',
                                                        'Type': currSig['Type'],
                                                        'NumberOfDimensions': currSig['NumberOfDimensions'],
                                                        'NumberOfElements' : currSig['NumberOfElements'],
                                                        'Alias' : currSig['Alias'],
                                                        'Samples': subsamplingRatio*samples,
                                                        })
                            else: #Not subsampled
                                currSig['DataSource'] = self.getMarteDeviceName(self)+'_RTN_IN_DDB'
                        else: #not synchronous
                            asyncInputsToBeReceived.append({'Name':currSig['Name'], 
                                    'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                    'Type': currSig['Type'],
                                    'NumberOfDimensions': currSig['NumberOfDimensions'],
                                    'NumberOfElements' : currSig['NumberOfElements'],
                                    'Alias' : currSig['Alias'],
                                    'Samples': samples,
                                    })
                            currSig['DataSource'] = self.getMarteDeviceName(self)+'_RTN_IN_DDB'
            else: #Not a reference to the output of a MARTe2 device
                try:
                    currInput = value.evaluate()
                except:
                    raise Exception('Missing Input signal for '+self.getPath())
                if isinstance(currInput, MDSplus.Signal):
                    currSig['DataSource'] = self.getMarteDeviceName(self)+'_TreeInDDB'
                    try:
                        if sigNode.getNode['COL_ORDER'].data() == 'YES':
                            useColumnOrder = 1
                        else:
                            useColumnOrder = 0
                    except:
                        useColumnOrder = 0
                    treeRefs.append({
                        'Name':currSig['Name'], 
                        'Expression': value, 
                        'UseColumnOrder': useColumnOrder, 
                        'Type': currSig['Type']})
                else: #It is a constant
                    if not isinstance(value.evaluate(), MDSplus.Scalar):
                        raise Exception('Invalid input for '+sigNode.getPath()+': '+value.decompile())
                    currSig['DataSource'] = self.getMarteDeviceName(sigNode)+'_ConstInDDB'
                    constRefs.append({
                        'Name':currSig['Name'], 
                        'Value': value.data(), 
                        'Type': currSig['Type']})
            sigDicts.append(currSig)
        #endfor
        return sigDicts

    #Management of the trigger input for storing outputs, if defined. It does not declares anything but possibly affects the passed structures
    def handleOutputTrigger(self, threadMap, resampledSyncSigs, syncInputsToBeReceived, 
                            asyncInputsToBeReceived, treeRefs):
        try:
            value = self.getNode('OUTPUTS:TRIGGER').getData()
        except:
            return #No trigger defined
        if self.isMarteDeviceRef(value): #If it is the reference to the output of another MARTe2 device (consistency checks also carried out) 
            alias = self.getSignalName(value)
            if self.hostedInSameSupervisor(value, threadMap): #Reference within same supervisor
                if self.hostedInSynchronizingThread(value, threadMap):
                    subsamplingRatio = self.getSubsamplingRatio(value, threadMap)
                    if  subsamplingRatio > 1:
                        resampledSyncSigs.append({'Name':'OutputTrigger', 
                                                      'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                                      'Type': self.getReferencedType(value),
                                                      'NumberOfDimensions': 0,
                                                      'NumberOfElements' : 1,
                                                      'Alias' : alias,
                                                      'Samples': subsamplingRatio,
                                                      })
            else: #Reference from another supervisor
                if self.hostedInSynchronizingThread(value, threadMap):
                    syncInputsToBeReceived.append({'Name':'OutputTrigger', 
                                                'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                                'Type': self.getReferencedType(value),
                                                'NumberOfDimensions': 0,
                                                'NumberOfElements' : 1,
                                                'Alias' : alias,
                                                'Samples': 1,
                                                })
                    subsamplingRatio = self.getSubsamplingRatio(value, threadMap)
                    if subsamplingRatio > 1:
                        resampledSyncSigs.append({'Name':'OutputTrigger', 
                                                'DataSource': self.getMarteDeviceName(self)+'_RTN_IN_DDB',
                                                'Type': self.getReferencedType(value),
                                                'NumberOfDimensions': 0,
                                                'NumberOfElements' : 1,
                                                'Alias' : alias,
                                                'Samples': subsamplingRatio,
                                                })
                else: #Not synchornized to other supervisor's thread
                    asyncInputsToBeReceived.append({'Name':'OutputTrigger', 
                                                'DataSource': self.getMarteDeviceName(value)+'_Output_Sync',
                                                'Type': self.getReferencedType(value),
                                                'NumberOfDimensions': 0,
                                                'NumberOfElements' : 1,
                                                'Alias' : alias,
                                                'Samples': 1,
                                                })

        else: #Not a reference to the output of a MARTe2 device
            if isinstance(value.evaluate(), MDSplus.Signal):
                treeRefs.append({'Name':'OutputTrigger', 'Expression': value, 'UseColumnOrder': 0, 'Type': 'uint8'})
                          
    #Return DataSource definition (dictionary) handling the inputs to be received form other supervisors
    def handleInputsToBeReceived(self, inputs, isSync):
        retDataSources = []
        retGams = []
        retData = {}
        if isSync:
            retData['Name'] = self.getMarteDeviceName(self)+'_SYNC_RTN_IN'
        else:
            retData['Name'] = self.getMarteDeviceName(self)+'_ASYNC_RTN_IN'

        try :
            port = self.getNode('PORT').data()
        except:
            raise Exception('Missing Port definition for device '+ self.getPath())
        retData['Class'] = 'RTNIn'
        if isSync:
            retData['Parameters'] = {'IsSynch':1, 'CircuitId': self.getNid(), 'Port':port}
        else:
            retData['Parameters'] = {'IsSynch':0, 'CircuitId': self.getNid(), 'Port':port}
        signals = []
        for syncInput in inputs:
            currInput = copy.deepcopy(syncInput)
            currInput['Name'] = currInput['Alias']
            currInput.pop('Alias')
            currInput.pop('DataSource')
            currInput.pop('Samples')
            signals.append(currInput)
        retData['Signals'] = signals
        retDataSources.append(retData)
        retDataSources.append({
            'Name': self.getMarteDeviceName(self)+'_RTN_IN_DDB',
            'Class': 'GAMDataSource'
        })
        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)+'_RTN_IN_IOGAM'
        retGam['Class'] = 'IOGAM'
        gamInputs = []
        for syncInput in inputs:
            currInput = copy.deepcopy(syncInput)
            currInput['DataSource'] = retData['Name']
            gamInputs.append(currInput)
        retGam['Inputs'] = gamInputs
        gamOutputs = []
        for syncInput in inputs:
            currOutput = copy.deepcopy(syncInput)
 #           currOutput['Name'] = currInput['Alias']
 #           currOutput.pop('Alias')
            currOutput['DataSource'] = self.getMarteDeviceName(self)+'_RTN_IN_DDB'
            gamOutputs.append(currOutput)
        retGam['Outputs'] = gamOutputs
        retGams.append(retGam)
        return {
            'DataSources': retDataSources,
            'Gams': retGams
        }

    #Build PickSampleGAM instance required to handle resampling
    def handleResampledSyncSigs(self, resampledSyncSigs):
        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)+'_Resampler'
        retGam['Class'] = 'PickSampleGAM'
        inputs = []
        for resSig in resampledSyncSigs:
            inputs.append(resSig)
        retGam['Inputs'] = inputs
        outputs = []
        for resSig in resampledSyncSigs:
            outputs.append({'Name': resSig['Alias'], 'Type': resSig['Type'], 'NumberOfDimensions': resSig['NumberOfDimensions'],
                'NumberOfElements': resSig['NumberOfElements'], 'Samples': 1, 'DataSource': self.getMarteDeviceName(self)+'_Res_DDB'})
        retGam['Outputs'] = outputs
        return retGam

    # Build IOGAM required to pack struct inputs when their fields are indivividually defined
    def handleInputPacking(self, inputsToBePacked):
        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)+'_Input_Bus_IOGAM'
        retGam['Class'] = 'IOGAM'
        inputs = []
        for inputToBePacked in inputsToBePacked:
            for currSig in inputToBePacked['Inputs']:
                inputs.append(self.removeParametersFromDict(currSig))
        retGam['Inputs'] = inputs
        outputs = []
        for inputToBePacked in inputsToBePacked:
            outputs.append({'Name': inputToBePacked['Name'], 'Type': inputToBePacked['Type'], 'NumberOfDimensions':0, 
                'NumberOfElements': 1, 'DataSource': self.getMarteDeviceName(self)+'_Input_Bus_DDB'})
        retGam['Outputs'] = outputs
        return retGam

    #Build the MDSReaderGAM instance for hanling readout of signals defined in treeRefs 
    def handleTreeRefs(self, treeRefs):
        outGam = {}
        outGam['Name'] = self.getMarteDeviceName(self)+'_TreeIn'
        outGam['Class'] = 'MDSReaderGAM'
        outGam['Parameters'] = {'TreeName': self.getTree().name, 'ShotNumber': self.getTree().shot}
        outGam['Inputs'] = [{'Name':'Time', 'DataSource':self.timerDDB, 'Type': self.timerType, 'NumberOfDimensions': 0, 'NumberOfElements': 1}]
        outputs = []
        for treeRef in treeRefs:
            outputs.append({
                'Name': treeRef['Name'], 
                'NumberOfDimensions': 0, 
                'NumberOfElements': 1, 
                'Type': treeRef['Type'],
                'UseColumnOrder': treeRef['UseColumnOrder'], 
                'DataExpr': treeRef['Expression'].decompile(), 
                'TimebaseExpr': '\"DIM_OF('+treeRef['Expression'].decompile()+')\"', 
                'DataSource': self.getMarteDeviceName(self)+'_TreeInDDB'})
        outGam['Outputs'] = outputs
        return outGam

    #Build a ConstantGAM instance to provide constant inputs
    def handleConstRefs(self, constRefs):  
        outGam = {}
        outGam['Class'] = 'ConstantGAM'
        outGam['Name'] = self.getMarteDeviceName(self)+'_ConstInGAM'
        outputs = []
        for constRef in constRefs:
            outputs.append({
                'Name': constRef['Name'], 
                'Type': constRef['Type'], 
                'Default': constRef['Value'], 
                'DataSource': self.getMarteDeviceName(self)+'_ConstInDDB'})
        outGam['Outputs'] = outputs
        return outGam
 
#****************OUTPUTS
    # Check if the passed output value node is referenced by any of the inputs passed in inputs
    def isReferenced(self, outValNode, inputNodes):
        outValNid = outValNode.getNid()
        for currInputNode in inputNodes:
            if currInputNode.getName() == 'TRIGGER': 
                try :
                    refTrigger = currInputNode.getData()
                    if isinstance(refTrigger, MDSplus.TreeNode) and refTrigger.getNid() == outValNid:
                        return True
                except:
                    continue
            else:
                try:
                    if currInputNode.getNode('VALUE').getData().getNid() == outValNid:
                        return True
                except:
                    try:
                        numFields = len(currInputNode.getNode('FIELDS').getChildren())
                    except:
                        numFields = 0
                    if numFields > 0:
                        if self.isReferenced(outValNode, currInputNode.getNode('FIELDS').getChildren()):
                            return True
        return False

    # Check if any output value node is referenced by any of the inputs passed in inputs
    def isAnyReferenced(self, outValNodes, inputNodes):
        for outValNode in outValNodes:
            if self.isReferenced(outValNode, inputNodes):
                return True
        return False



    # Check if the passed output value node is referenced another MARTe2 device in the same Thread
    def isReferencedByThisThread(self, outValNode, threadMap):
        # threadMap is a Dictionary with two keys:
        # 'DeviceInfo': defining for every MARTe2 device NID a dictionary:
        #       'ThreadName': name of the thread it belongs to
        #       'SupervisorNid': NID of the supervirsor hosting the device
        #       'SupervisorIp': IP address of the superisor hosting the device
        #       'DevicePort': Posr number foir that device
        # 'ThreadInfo': defining for every thread for this supervisor a dictionary:
        #       'SyncThreadName': The name of the Synchronizing thread or None if this thread is not synchronized
        #       'SyncThreadSupervisor': The NID of the MARTE2 supervisor hosting the synchronizing thread or None if not synchronized
        #       'SyncDiv': Frequency division from synchronizing thread or None is not synchronized
    # Check if the passed output value node is referenced another MARTe2 device in a different Thread
        thisNid = self.getNid()
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            if marteNid == thisNid:
                continue
            inputNodes = MDSplus.TreeNode(marteNid).getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                return True
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [marteDevice.getNode('OUTPUTS:TRIGGER')]):
                return True
        return False

  # Check if the passed output value node is referenced another MARTe2 device in any Thread
    def isReferencedByAnyThread(self, outValNode, threadMap):
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            inputNodes = MDSplus.TreeNode(marteNid).getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                return True
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [marteDevice.getNode('OUTPUTS:TRIGGER')]):
                return True
        return False

  # Check if any passed output value node is referenced another MARTe2 device in any Thread
    def isAnyOutputReferencedByAnyThread(self, outValNodes, threadMap):
        for outValNode in outValNodes:
            if self.isReferencedByAnyThread(outValNode.getNode('VALUE'), threadMap):
                return True
 
    #get IP, Port of referenced device and Nid (CircuitId)of target device
    def getReferencedNetInfo(self, outValNode, threadMap):
        outValNid = outValNode.getNid()
        netInfos = {}
        ips = []
        ports = []
        circuitIds = []
        thisNid = self.getNid()
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            if marteNid == thisNid:
                continue
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode == MARTE2_COMPONENT.MODE_INPUT or mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                continue
            inputNodes = marteDevice.getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] !=  threadMap['DeviceInfo'][thisNid]['SupervisorNid']:
                    ips.append(threadMap['DeviceInfo'][marteNid]['SupervisorIp'])
                    ports.append(threadMap['DeviceInfo'][marteNid]['DevicePort'])
                    circuitIds.append(marteDevice.getNid())
            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [marteDevice.getNode('OUTPUTS:TRIGGER')]):
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] !=  threadMap['DeviceInfo'][thisNid]['SupervisorNid']:
                    ips.append(threadMap['DeviceInfo'][marteNid]['SupervisorIp'])
                    ports.append(threadMap['DeviceInfo'][marteNid]['DevicePort'])
                    circuitIds.append(marteDevice.getNid())
        netInfos['Ip'] = ips
        netInfos['Port'] = ports
        netInfos['CircuitId'] = circuitIds
        return netInfos

 
    # Check if the passed output value node is referenced another MARTe2 device in the same supervisor
    def isReferencedByThisSupervisor(self, outValNode, threadMap):
        thisNid = self.getNid()
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            if marteNid == thisNid:
                continue
            inputNodes = MDSplus.TreeNode(marteNid).getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid']:
                    return True
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [marteDevice.getNode('OUTPUTS:TRIGGER')]):
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid']:
                    return True
        return False


    # Check if the passed output value node is referenced another MARTe2 device of the same supervisor in a different Thread
    def isReferencedByNonSynchronizedThreadSameSupervisor(self, outValNode, threadMap):
        thisNid = self.getNid()
        thisThreadName = threadMap['DeviceInfo'][thisNid]['ThreadName']
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            if marteNid == thisNid:
                continue
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode == MARTE2_COMPONENT.MODE_INPUT or mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                continue
            inputNodes = marteDevice.getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                currThreadName =  threadMap['DeviceInfo'][marteNid]['ThreadName']
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid'] and \
                    threadMap['DeviceInfo'][marteNid]['ThreadName'] != threadMap['DeviceInfo'][thisNid]['ThreadName'] and  \
                    threadMap['ThreadInfo'][currThreadName]['SyncThreadName'] != thisThreadName:
                    return True
            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [marteDevice.getNode('OUTPUTS:TRIGGER')]):
                currThreadName =  threadMap['DeviceInfo'][marteNid]['ThreadName']
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid'] and  \
                    threadMap['DeviceInfo'][marteNid]['ThreadName'] != threadMap['DeviceInfo'][thisNid]['ThreadName'] and  \
                    threadMap['ThreadInfo'][currThreadName]['SyncThreadName'] != thisThreadName:
                    return True
        return False

    #Check if the passed output is referenced by a sychornized thread of the same supervisor
    def isReferencedBySynchronizedThreadSameSupervisor(self, outValNode, threadMap):
        thisNid = self.getNid()
        thisThreadName = threadMap['DeviceInfo'][thisNid]['ThreadName']
        thisSupervisorNid = threadMap['DeviceInfo'][thisNid]['SupervisorNid']
        marteNids = threadMap['DeviceInfo'].keys()
        for marteNid in marteNids:
            if marteNid == thisNid:
                continue
            marteDevice = MDSplus.TreeNode(marteNid)
            mode = marteDevice.getNode('MODE').data()
            if mode == MARTE2_COMPONENT.MODE_INPUT or mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
                continue
            inputNodes = marteDevice.getNode('INPUTS').getChildren()
            if self.isReferenced(outValNode, inputNodes):
                currThreadName =  threadMap['DeviceInfo'][marteNid]['ThreadName']
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid'] and  \
                    threadMap['ThreadInfo'][currThreadName]['SyncThreadName']== thisThreadName:
                    return True

            if mode != MARTE2_COMPONENT.MODE_OUTPUT and self.isReferenced(outValNode, [MDSplus.TreeNode(marteNid).getNode('OUTPUTS:TRIGGER')]):
                currThreadName =  threadMap['DeviceInfo'][marteNid]['ThreadName']
                if threadMap['DeviceInfo'][marteNid]['SupervisorNid'] == threadMap['DeviceInfo'][thisNid]['SupervisorNid'] and  \
                    threadMap['ThreadInfo'][currThreadName]['SyncThreadName'] == thisThreadName:
                    return True
        return False

    #Check if if any output is requested data storage (seg_len > 0)
    def isAnyOutputStored(self, fieldNodes):
        for fieldNode in fieldNodes:
            try:
                if fieldNode.getNode('SEG_LEN').data() > 0:
                    return True
            except:
                pass
        return False

        #Convert the passed list into a MARTe array
    def toMarteArray(self, list):
        marteArr = '{'
        for l in list:
            marteArr += ' '+str(l)+' '
        marteArr += '}'
        return marteArr

    def getOutputSignalsDict(self, sigNodes, threadMap, typesDict, syncThreadSignals, asyncThreadSignals, outputsToBeSent, 
        signalsToBeStored, outputsToBeUnpacked, isFieldCheck = False):
        sigDicts = []
        for sigNode in sigNodes:
            currSig = {}
            try:
                currDimension = sigNode.getNode('DIMENSIONS').data()
            except:
                currDimension = -1
            if currDimension == -1:
                continue

            currName = self.getSignalName(sigNode)
            if isFieldCheck:
                numFields = 0
            else:
                try:
                    numFields = len(sigNode.getNode('FIELDS').getChildren())
                except:
                    numFields = 0
            if numFields == 0:
                try:
                    currType = sigNode.getNode('TYPE').data()
                except:
                    raise Exception('Missing type definition for '+ sigNode.getPath())
                self.checkNativeType(currType, typesDict)

                try:
                    dimensions = sigNode.getNode('DIMENSIONS').data()
                except:
                    raise Exception('Missing dimensios for '+ sigNode.getPath())
                numDims, numEls = self.parseDimension(sigNode.getNode('DIMENSIONS').data())
                try:
                    samples = sigNode.getNode('SAMPLES').data()
                except:
                    samples = 1
            else: #structured output. Not occurring if isCheckField == True
                currType = self.getTypeName(sigNode.getNode('FIELDS'), typesDict)
                numDims = 0
                numEls = 1
                samples = 1
                fieldNodes = self.getFlattenedFields(sigNode.getNode('FIELDS'))
                if self.isAnyOutputReferencedByAnyThread(fieldNodes, threadMap) or self.isAnyOutputStored(fieldNodes):
                    unpackedOutputs = self.getOutputSignalsDict(fieldNodes, threadMap, typesDict, syncThreadSignals, asyncThreadSignals, outputsToBeSent, 
                        signalsToBeStored, [], isFieldCheck = True)
                    outputsToBeUnpacked.append({'Input':{'Name': currName, 'Type': currType}, 'Outputs': unpackedOutputs})

            currSig['Name'] = currName
            currSig['Type'] = currType
            currSig['NumberOfDimensions'] = numDims
            currSig['NumberOfElements'] = numEls
            currSig['Samples'] = samples
            currSig['DataSource'] = self.getMarteDeviceName(sigNode)+'_Output_DDB'

            if sigNode.getParent().getName() == 'FIELDS': #If it is an expanded signal
                currDataSource = self.getMarteDeviceName(sigNode)+'_Expanded_Output_DDB'
            else:
                currDataSource= self.getMarteDeviceName(self)+'_Output_DDB'
            outValNode = sigNode.getNode('VALUE')
            if self.isReferencedBySynchronizedThreadSameSupervisor(outValNode, threadMap):
 
                syncThreadSignals.append({
                    'Name': currSig['Name'],
                    'Type': currSig['Type'],
                    'NumberOfDimensions': currSig['NumberOfDimensions'],
                    'NumberOfElements': currSig['NumberOfElements'],
                    'Samples': currSig['Samples'],
                    'DataSource': currDataSource,
                })
            if self.isReferencedByNonSynchronizedThreadSameSupervisor(outValNode, threadMap):
                asyncThreadSignals.append({
                    'Name': currSig['Name'],
                    'Type': currSig['Type'],
                    'NumberOfDimensions': currSig['NumberOfDimensions'],
                    'NumberOfElements': currSig['NumberOfElements'],
                    'Samples': currSig['Samples'],
                    'DataSource': currDataSource,
                })
            netInfos = self.getReferencedNetInfo(outValNode, threadMap)
            if len(netInfos['Ip']) > 0:
                if isFieldCheck:
                    dataSource = self.getMarteDeviceName(sigNode)+'_Expanded_Output_DDB'
                else:
                    dataSource = self.getMarteDeviceName(sigNode)+'_Output_DDB'
                
                outputsToBeSent.append({
                    'Name': currSig['Name'],
                    'Type': currSig['Type'],
                    'NumberOfDimensions': currSig['NumberOfDimensions'],
                    'NumberOfElements': currSig['NumberOfElements'],
                    'Samples': currSig['Samples'],
                    'DataSource': dataSource,
                    'Ip': self.toMarteArray(netInfos['Ip']),
                    'Port': self.toMarteArray(netInfos['Port']),
                    'CircuitId': self.toMarteArray(netInfos['CircuitId'])
                }) 
            try:
                segLen = sigNode.getNode('SEG_LEN').data()
            except:
                segLen = 0
            if segLen > 0:
                signalsToBeStored.append(sigNode)     

            if not isFieldCheck:
                try:
                    numParameters = len(sigNode.getNode('PARAMETERS').getChildren())
                except:
                    numParameters = 0
            else:
                numParameters = 0
            if numParameters > 0:
                currSig['Parameters'] = self.getParametersDict(sigNode.getNode('PARAMETERS'))
            
            sigDicts.append(currSig)
        return sigDicts
    
    #Returns the dict definition of theIOGAM carrying out signal unpacking
    def handleOutputUnpack(self, outputsToBeUnpacked):
        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)+'_Output_Bus_IOGAM'
        retGam['Class'] = 'IOGAM'
        inputs = []
        outputs = []
        for currUnpacked in outputsToBeUnpacked:
            inputs.append({
                'Name': currUnpacked['Input']['Name'],
                'Type': currUnpacked['Input']['Type'], 
                'DataSource': self.getMarteDeviceName(self)+'_Output_DDB'})
            outputs += currUnpacked['Outputs']
        retGam['Inputs'] = inputs
        for currOutput in outputs:
            currOutput['DataSource'] = self.getMarteDeviceName(self)+'_Expanded_Output_DDB'
        retGam['Outputs'] = self.removeParametersFromList(outputs)

        return retGam

    #remove 'Parameters' key fron signal list
    def removeParametersFromList(self,sigs):
        outSigs = []
        for sig in sigs:
            if 'Parameters' in sig:
                sig.pop('Parameters')
            outSigs.append(sig)
        return outSigs
    def removeParametersFromDict(self,sig):
        if 'Parameters' in sig:
            sig.pop('Parameters')
        return sig

    #Returns the dict definition of theIOGAM carrying out synchronized signals
    def handleSynchronoutOutputs(self, syncSignals):
        retGam = {}
        retGam['Class'] = 'IOGAM'
        retGam['Name'] = self.getMarteDeviceName(self)+'_Output_Sync_IOGAM'
        inputs = []
        outputs = []
        for syncSignal in syncSignals:
            inputs.append(self.removeParametersFromDict(copy.deepcopy(syncSignal)))
            currOutput = self.removeParametersFromDict(copy.deepcopy(syncSignal))
            currOutput['DataSource'] =  self.getMarteDeviceName(self)+'_Output_Sync'
            outputs.append(currOutput)
        retGam['Inputs'] = inputs
        retGam['Outputs'] = outputs

        return retGam
 
    #Returns the dict definition of theIOGAM carrying out synchronized signals
    def handleAsynchronoutOutputs(self, asyncSignals):
        retGam = {}
        retGam['Class'] = 'IOGAM'
        retGam['Name'] = self.getMarteDeviceName(self)+'_Output_Async_IOGAM'
        inputs = []
        outputs = []
        for asyncSignal in asyncSignals:
            inputs.append(self.removeParametersFromDict(copy.deepcopy(asyncSignal)))
            currOutput = self.removeParametersFromDict(copy.deepcopy(asyncSignal))
            currOutput['DataSource'] =  self.getMarteDeviceName(self)+'_Output_Async'
            outputs.append(currOutput)
        retGam['Inputs'] = inputs
        retGam['Outputs'] = outputs

        return retGam

    #Return the definitions of MDSWriter DataSource and ConversionGAM GAM for handling the stored outputs
    def handleOutputsStorage(self, signalsToBeStored, threadMap):
        retDataSource = {}
        retDataSource['Class'] = 'MDSWriter'
        retDataSource['Name'] = self.getMarteDeviceName(self)+'_TreeOut'
        parameters = {}
        try:
            cpuMask = self.getNode('OUTPUTS:CPU_MASK').data()
            parameters['CPUMask'] = cpuMask
        except:
            raise Exception("Output CPU Mask must be defined in device "+self.getPath())
        parameters['StackSize'] = 10000000
        parameters['TimeRefresh'] = 5
        try:
            jScopeEvent = self.getNode('JSCOPE_EV').data()
            parameters['EventName'] = jScopeEvent
        except:
            parameters['EventName'] = 'updatejScope'
        parameters['PulseNumber'] = self.getTree().shot
        parameters['TreeName'] = self.getTree().name
        try:
            trigger = self.getNode('OUTPUTS:TRIGGER').getData()
        except:
            trigger = None
        if trigger != None:
            parameters['StoreOnTrigger'] = 1
        else:
            parameters['StoreOnTrigger'] = 0
        if trigger != None:
            try:
                postTrigSamples = self.getNode('OUTPUTS:POST_TRIGGER').data()
                preTrigSamples = self.getNode('OUTPUTS:PRE_TRIGGER').data()
            except:
                raise Exception('When trigger defined, pre and post trigger must be set in device '+self.getPath())
            parameters['NumberOfPreTriggers'] = preTrigSamples
            parameters['NumberOfPostTriggers'] = postTrigSamples
            parameters['NumberOfBuffers'] = postTrigSamples + 10
        else:
            parameters['NumberOfBuffers'] = 10000
        retDataSource['Parameters'] = parameters

        signals = []
        if trigger != None:
            signals.append({'Name': 'Trigger', 'Type': 'uint8'})
        try:
            numSamples = signalsToBeStored[0].getNode('SAMPLES')
        except:
            numSamples = 1
        signals.append({
            'Name': 'Time', 
            'Type': self.timerType, 
            'NodeName': self.getNode('OUTPUTS:OUT_TIME').getFullPath(),
            'AutomaticSegmentation': 0,
            'TimeSignal': 1,
            'Period': str(self.timerPeriod * numSamples).replace('D', 'E'),
            'MakeSegmentAfterNWrites': signalsToBeStored[0].getNode('SEG_LEN').data(),
            'DiscontinuityFactor': 10
            })
        for sigNode in signalsToBeStored:
            sigName = self.getSignalName(sigNode)
            if sigName == 'Time':   #Time signal has been already stored
                continue
            sigDef = {}
            sigDef['Name'] = sigName
            sigDef['Period'] = str(self.timerPeriod * numSamples).replace('D', 'E')
            sigDef['MakeSegmentAfterNWrites'] = sigNode.getNode('SEG_LEN').data()
            sigDef['NodeName'] = sigNode.getNode('VALUE').getFullPath()
            sigDef['AutomaticSegmentation'] = 0
            sigDef['DiscontinuityFactor'] = 10
            signals.append(sigDef)
        retDataSource['Signals'] = signals

        retGam = {}
        if trigger == None:
            retGam['Class'] = 'IOGAM'
        else:
            retGam['Class'] = 'ConversionGAM'
        retGam['Name'] = self.getMarteDeviceName(self)+'_TreeOut_IOGAM'

        inputs = []
        if trigger != None:
            currInput={'Name': self.getSignalName(trigger), 'Type':  self.getReferencedType(trigger)}
            if self.hostedInSameThread(trigger, threadMap):
                currInput['DataSource'] = self.getMarteDeviceName(trigger)+'_Output_DDB'
            elif self.hostedInSameSupervisor(trigger, threadMap):
                if self.hostedInSynchronizingThread(trigger, threadMap):
                    currInput['DataSource'] = self.getMarteDeviceName(trigger)+'_Output_Sync'
                else:
                    currInput['DataSource'] = self.getMarteDeviceName(trigger)+'_Output_Async'
            else: #Hosted in another supervisor
                if self.hostedInSynchronizingThread(trigger, threadMap):
                    currInput['DataSource'] = self.getMarteDeviceName(self)+'_RTN_IN_DDB'
                else:
                    currInput['DataSource'] = self.getMarteDeviceName(self)+'_RTN_IN_DDB'
            inputs.append(currInput)

        inputs.append({'Name': 'Time', 'Type': self.timerType, 'DataSource': self.timerDDB})
        for sigNode in signalsToBeStored:
            try:
                samples = sigNode.getNode('Samples').data()
            except:
                samples = 1
            sigName = self.getSignalName(sigNode)
            if sigName == 'Time':
                continue
            sigDef = {}
            sigDef['Name'] = sigName
            sigDef['Type'] = sigNode.getNode('Type').data()

            if sigNode.getParent().getName() == 'FIELDS': #If it is an expanded signal
                sigDef['DataSource'] = self.getMarteDeviceName(self)+'_Expanded_Output_DDB'
            else:
                sigDef['DataSource'] = self.getMarteDeviceName(self)+'_Output_DDB'
            numDims, numEls = self.parseDimension(sigNode.getNode('DIMENSIONS').data())
#            sigDef['NumberOfDimensions'] = numDims
#            sigDef['NumberOfElements'] = numEls
 #            if samples > 1:
#                sigDef['Samples'] = samples
            inputs.append(sigDef)
        retGam['Inputs'] = inputs

        outputs = []
        if trigger != None:
            outputs.append({
                'Name':'Trigger',
                'Type':'uint8', 
                 'DataSource':self.getMarteDeviceName(self)+'_TreeOut'})
        outputs.append({
            'Name': 'Time', 
            'Type': self.timerType, 
            'DataSource': self.getMarteDeviceName(self)+'_TreeOut',
            'Samples': samples
            })
        for sigNode in signalsToBeStored:
            sigName = self.getSignalName(sigNode)
            if sigName == 'Time':
                continue
            sigDef = {}
            sigDef['Name'] = sigName
            sigDef['Type'] = sigNode.getNode('Type').data()
            sigDef['DataSource'] = self.getMarteDeviceName(self)+'_TreeOut'
            numDims, numEls = self.parseDimension(sigNode.getNode('DIMENSIONS').data())
            sigDef['NumberOfDimensions'] = numDims
            sigDef['NumberOfElements'] = numEls
            try:
                samples = sigNode.getNode('Samples').data()
            except:
                samples = 1
            if samples > 1:
                sigDef['Samples'] = samples
            outputs.append(sigDef)
        retGam['Outputs'] = outputs

        return retDataSource, retGam


    #Return the definitions of RTNOut DataSource and IOGAM for handling data sending to other supervisors
    def handleOutputsToBeSent(self, outputsToBeSent):
        retDataSource = {}
        retDataSource['Class'] = 'RTNOut'
        retDataSource['Name'] = self.getMarteDeviceName(self)+'_RTN_OUT'
        signals = []
        for currOutput in outputsToBeSent:
            signals.append({
                'Name': currOutput['Name'],
                'Type': currOutput['Type'],
                'NumberOfDimensions': currOutput['NumberOfDimensions'],
                'NumberOfElements': currOutput['NumberOfElements'],
                'Samples': currOutput['Samples'],
                'Ip': currOutput['Ip'],
                'Port': currOutput['Port'],
                'CircuitId': currOutput['CircuitId'],
            })
        retDataSource['Signals'] = signals

        retGam = {}
        retGam['Class'] = 'IOGAM'
        retGam['Name'] = self.getMarteDeviceName(self)+'_RTN_OUT_IOGAM'
        inputs = []
        for currOutput in outputsToBeSent:
            inputs.append({
                'Name': currOutput['Name'],
                'Type': currOutput['Type'],
                'NumberOfDimensions': currOutput['NumberOfDimensions'],
                'NumberOfElements': currOutput['NumberOfElements'],
                'Samples': currOutput['Samples'],
                'DataSource': currOutput['DataSource'],
            })
        retGam['Inputs'] = inputs
        outputs = []
        for currOutput in outputsToBeSent:
            outputs.append({
                'Name': currOutput['Name'],
                'Type': currOutput['Type'],
                'NumberOfDimensions': currOutput['NumberOfDimensions'],
                'NumberOfElements': currOutput['NumberOfElements'],
                'Samples': currOutput['Samples'],
                'DataSource': self.getMarteDeviceName(self)+'_RTN_OUT'
            })
        retGam['Outputs'] = outputs

        return retDataSource, retGam


 ###########Overall Generation

    #return the list of GAM and DataSource (dictionaries) corresponding to this GAM MARTe2 device 
    def generateMarteGamConfiguration(self, threadMap, timerDDB, timerType, timerPeriod, typesDict):
        self.timerDDB = timerDDB
        self.timerType = timerType
        self.timerPeriod = timerPeriod

        retGams = []
        retDataSources = []

        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)
        try:
            retGam['Class'] = self.getNode('GAM_CLASS').data()
        except:
            raise Exception('Missing GAM class definition for '+self.getPath())
        try:
            numPars = len(self.getNode('PARAMETERS').getChildren())
        except:
            numPars = 0
        if numPars > 0:
            retGam['Parameters'] = self.getParametersDict(self.getNode('PARAMETERS'))

################Inputs
        resampledSyncSigs = []
        syncInputsToBeReceived = []
        asyncInputsToBeReceived = []
        treeRefs = []
        constRefs = []
        inputsToBePacked = []
        syncThreadSignals = []
        asyncThreadSignals = []
        outputsToBeSent = []

        try:
            inputNodes = self.getNode('INPUTS').getChildren()
            inputNodes.sort()
        except:
            raise Exception('No Inputs specified for GAM device '+self.getPath())
        retGam['Inputs'] = self.getInputSignalsDict(inputNodes, threadMap, typesDict, resampledSyncSigs, syncInputsToBeReceived, 
                            asyncInputsToBeReceived, treeRefs, constRefs, inputsToBePacked, isFieldCheck = False)
        self.handleOutputTrigger(threadMap, resampledSyncSigs, syncInputsToBeReceived, 
                            asyncInputsToBeReceived, treeRefs)

        #Handle reception of input signals from devices belonging to a different supervisor
        if len(syncInputsToBeReceived) > 0:
            rtnInInfo = self.handleInputsToBeReceived(syncInputsToBeReceived, isSync = True)
            retDataSources += rtnInInfo['DataSources']
            retGams += rtnInInfo['Gams']
        if len(asyncInputsToBeReceived) > 0:
            rtnInInfo = self.handleInputsToBeReceived(asyncInputsToBeReceived, isSync = False)
            retDataSources += rtnInInfo['DataSources']
            retGams += rtnInInfo['Gams']

            
        #Handle resampling of Input Signals
        if len(resampledSyncSigs) > 0:
            retDataSources.append({'Name':  self.getMarteDeviceName(self)+'_Res_DDB', 'Class': 'GAMDataSource'})
            retGams.append(self.handleResampledSyncSigs(resampledSyncSigs))

        #Handle real-time input signals readout from pulse file
        if len(treeRefs) > 0:
            retGams.append(self.handleTreeRefs(treeRefs))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_TreeInDDB', 'Class': 'GAMDataSource'})

        #Handle constant inputs
        if len(constRefs) > 0:
            retGams.append(self.handleConstRefs(constRefs))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_ConstInDDB', 'Class': 'GAMDataSource'})

        #Handle Packing of input structures defined in separate fields
        if len(inputsToBePacked) > 0:
            retGams.append(self.handleInputPacking(inputsToBePacked))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Input_Bus_DDB', 'Class': 'GAMDataSource'})

###############Outputs
        syncThreadSignals = []
        asyncThreadSignals = []
        outputsToBeSent = []
        signalsToBeStored = []
        outputsToBeUnpacked = []
        try:
            outputNodes = self.getNode('OUTPUTS').getChildren()
            outputNodes.sort()
        except:
            raise Exception('No outputs defined for GAM device '+self.getPath())

        retGam['Outputs'] = self.getOutputSignalsDict(outputNodes, threadMap, typesDict, syncThreadSignals, asyncThreadSignals, 
            outputsToBeSent, signalsToBeStored, outputsToBeUnpacked, isFieldCheck = False)

        retGams.append(retGam)
        retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Output_DDB', 'Class': 'GAMDataSource'})


        #Handle unpacking of output structures
        if len(outputsToBeUnpacked) > 0:
            retGams.append(self.handleOutputUnpack(outputsToBeUnpacked))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Expanded_Output_DDB', 'Class': 'GAMDataSource'})
        #Handle Synchronous Outputs
        if len(syncThreadSignals) > 0:
            retGams.append(self.handleSynchronoutOutputs(syncThreadSignals))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Output_Sync', 'Class': 'RealTimeThreadSynchronisation'})
        #Handle Asynchronous Outputs
        if len(asyncThreadSignals) > 0:
            retGams.append(self.handleAsynchronoutOutputs(asyncThreadSignals))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Output_Async', 'Class': 'RealTimeThreadAsyncBridge'})
        #Handle output storage
        if len(signalsToBeStored) > 0:
            storeDataSource, storeGam = self.handleOutputsStorage(signalsToBeStored, threadMap)
            retDataSources.append(storeDataSource)
            retGams.append(storeGam)
        #Handle Output network send for signals
        if len(outputsToBeSent) > 0:
            sendDataSource, sendGam = self.handleOutputsToBeSent(outputsToBeSent)
            retDataSources.append(sendDataSource)
            retGams.append(sendGam)
            
    ############All Done!!
        return retDataSources, retGams

    #return the list of GAM and DataSource (dictionaries) and time period corresponding to this Synchronized Input device 
    def generateMarteInputConfiguration(self, threadMap, timerDDB, timerType, timerPeriod, typesDict):
        self.timerDDB = timerDDB
        self.timerType = timerType
        self.timerPeriod = timerPeriod

        retGams = []
        retDataSources = []

        retDataSource = {}
        retDataSource['Name'] = self.getMarteDeviceName(self)
        try:
            retDataSource['Class'] = self.getNode('GAM_CLASS').data()
        except:
            raise Exception('Missing DataSource class definition for '+self.getPath())
        try:
            numPars = len(self.getNode('PARAMETERS').getChildren())
        except:
            numPars = 0
        if numPars > 0:
            retDataSource['Parameters'] = self.getParametersDict(self.getNode('PARAMETERS'))


###############Outputs
        syncThreadSignals = []
        asyncThreadSignals = []
        outputsToBeSent = []
        signalsToBeStored = []
        outputsToBeUnpacked = []
        try:
            outputNodes = self.getNode('OUTPUTS').getChildren()
            outputNodes.sort()
        except:
            raise Exception('No outputs defined for GAM device '+self.getPath())

        outputs = self.getOutputSignalsDict(outputNodes, threadMap, typesDict, syncThreadSignals, asyncThreadSignals, 
            outputsToBeSent, signalsToBeStored, outputsToBeUnpacked, isFieldCheck = False)
        signals = copy.deepcopy(outputs)
        inputs = copy.deepcopy(outputs)

        for currSignal in signals:
            currSignal.pop('DataSource')

        retDataSource['Signals'] = signals
        retDataSources.append(retDataSource)

        retGam = {}
        retGam['Name'] = self.getMarteDeviceName(self)+'_IOGAM'
        retGam['Class'] = 'IOGAM'
        for currInput in inputs:
            currInput['DataSource'] = self.getMarteDeviceName(self)
        retGam['Inputs'] = self.removeParametersFromList(inputs)
        for currOutput in outputs:
            try:
                numSamples = currOutput['Samples']
            except:
                numSamples = 1
            try:
                numElements = currOutput['NumberOfElements']
            except:
                numElements = 1            
            try:
                currOutput.pop('Samples')
            except:
                pass
            currOutput['NumberOfElements'] = numSamples * numElements




        retGam['Outputs'] = self.removeParametersFromList(outputs)
        retGams.append(retGam)

        retDataSources.append({
            'Name': self.getMarteDeviceName(self)+'_Output_DDB',
            'Class': 'GAMDataSource'})

        #Handle Synchronous Outputs
        if len(syncThreadSignals) > 0:
            retGams.append(self.handleSynchronoutOutputs(syncThreadSignals))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Output_Sync', 'Class': 'RealTimeThreadSynchronisation'})
        #Handle Asynchronous Outputs
        if len(asyncThreadSignals) > 0:
            retGams.append(self.handleAsynchronoutOutputs(asyncThreadSignals))
            retDataSources.append({'Name': self.getMarteDeviceName(self)+'_Output_Async', 'Class': 'RealTimeThreadAsyncBridge'})
        #Handle output storage
        if len(signalsToBeStored) > 0:
            storeDataSource, storeGam = self.handleOutputsStorage(signalsToBeStored, threadMap)
            retDataSources.append(storeDataSource)
            retGams.append(storeGam)
        #Handle Output network send for signals
        if len(outputsToBeSent) > 0:
            sendDataSource, sendGam = self.handleOutputsToBeSent(outputsToBeSent)
            retDataSources.append(sendDataSource)
            retGams.append(sendGam)
            
    ############All Done!!
        return retDataSources, retGams


    #return the list of GAM and DataSource (dictionaries) corresponding to this GAM MARTe2 device 
    def generateMarteOutputConfiguration(self, threadMap, timerDDB, timerType, timerPeriod, typesDict):
        self.timerDDB = timerDDB
        self.timerType = timerType
        self.timerPeriod = timerPeriod

        retGams = []
        retDataSources = []

        retDataSource = {}
        retDataSource['Name'] = self.getMarteDeviceName(self)
        try:
            retDataSource['Class'] = self.getNode('GAM_CLASS').data()
        except:
            raise Exception('Missing GAM class definition for '+self.getPath())
        try:
            numPars = len(self.getNode('PARAMETERS').getChildren())
        except:
            numPars = 0
        if numPars > 0:
            retDataSource['Parameters'] = self.getParametersDict(self.getNode('PARAMETERS'))

################Inputs
        resampledSyncSigs = []
        syncInputsToBeReceived = []
        asyncInputsToBeReceived = []
        treeRefs = []
        constRefs = []
        inputsToBePacked = []


        try:
            inputNodes = self.getNode('INPUTS').getChildren()
            inputNodes.sort()
        except:
            raise Exception('No Inputs specified for GAM device '+self.getPath())
        inputs = self.getInputSignalsDict(inputNodes, threadMap, typesDict, resampledSyncSigs, syncInputsToBeReceived, 
                            asyncInputsToBeReceived, treeRefs, constRefs, inputsToBePacked, isFieldCheck = False)
        
        signals = copy.deepcopy(inputs)
        outputs = copy.deepcopy(inputs)

        for currSignal in signals:
            currSignal.pop('DataSource')
            try:
                currSignal.pop('Alias')
            except:
                pass
        retDataSource['Signals'] = signals
        retDataSources.append(retDataSource)

        for currOutput in outputs:
            currOutput['DataSource'] = self.getMarteDeviceName(self)
            try:
                currOutput.pop('Alias')
            except:
                pass


# If trigger is defined put it as first signal
            if outputTrigger != None:
                dataSourceText += '      Trigger = {\n'
                dataSourceText += '        Type = uint8\n'
                dataSourceText += '      }\n'
# Time Management
            dataSourceText += '      Time = {\n'
            dataSourceText += '        NodeName = "' + \
                configDict['outTimeNid'].getFullPath()+'"\n'
# keep into account possibl sample information for that GAM
            currSamples = 1
            try:
                currSamples = outputDict['samples']
            except:
                currSamples = 1

            dataSourceText += '        Period = '+str(period/currSamples)+'\n'
            dataSourceText += '        MakeSegmentAfterNWrites = 100\n'
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

            gamList.append(gamName+'_TreeOutIOGAM')
            gamText = '  +'+gamName+'_TreeOutIOGAM = {\n'
            if outputTrigger != None:  # If using output trigger, the trigger must be converted to uint8
                gamText += '    Class = ConversionGAM\n'
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
                    gamText += '        DataSource = '+gamName+'_TreeInput\n'
                    gamText += '        Type = uint8\n'
                    gamText += '      }\n'
# end Trigger Management

# Time signal management
            gamText += '      Time = {\n'
            gamText += '        DataSource = ' + timerDDB+'\n'
            gamText += '        Type = uint32\n'
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
            gamText += '        Type = uint32\n'
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

            gamList.append(gamName+'_StreamOutIOGAM')
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
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for inputDict in inputDicts:
                if len(inputDict['fields']) > 0 and not 'value' in inputDict:
                    for fieldDict in inputDict['fields']:

                        # it may be a reference to a struct field
                        isInputStructField = (
                            fieldDict['value'].getParent().getParent().getName() == 'FIELDS')
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
                            gamText += '        DataSource = '+gamName+'_TreeInput\n'
                        else:
                            gamText += '      '+signalGamName+' = {\n'
                            if isInputStructField:
                                gamText += '        DataSource = '+sourceGamName+'_Expanded_Output_DDB\n'
                            else:
                                if self.onSameThread(threadMap, sourceNode):
                                    gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
                                elif self.sameSynchSource(sourceNode):
                                    gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
                                    try:
                                        syncDiv = self.timebase_div.data()
                                        gamText += '        Samples = ' + \
                                            str(syncDiv)+'\n'
                                        forceUsingSamples = True
                                    except:
                                        pass  # Consider RealTimeSynchronization downsampling only if timebase_div is defined
                                else:
                                    gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
                        if 'name' in fieldDict:
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

        # There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        if len(nonGamInputNodes) > 0:
            # if debugEnabled, we need to write TWO instances of MDSReaderNS (_TreeInput and _TreeInput_Debug)
            treeInputExtensions = []
            treeInputExtensions.append('_TreeInput')
            if debugEnabled:
                treeInputExtensions.append('_TreeInput_Logger')

            for treeInputExtension in treeInputExtensions:
                #dataSourceText = '  +'+gamName+'_TreeInput = {\n'
                dataSourceText = '  +'+gamName+treeInputExtension + ' = {\n'
                dataSourceText += '    Class = MDSReaderNS\n'
                dataSourceText += '    UseColumnOrder = 0\n'
                dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
                dataSourceText += '    ShotNumber = ' + \
                    str(self.getTree().shot)+'\n'
                currTimebase = self.getNode('timebase').evaluate()
                if isinstance(currTimebase, Range):
                    startTime = currTimebase.begin.data()
                    period = currTimebase.delta.data()
                else:
                    currTimebase = currTimebase.data()
                    startTime = currTimebase[0]
                    period = currTimebase[1] - currTimebase[0]
                frequency = 1./period

                dataSourceText += '    StartTime = '+str(startTime)+'\n'
                dataSourceText += '    Frequency = ' + \
                    str(round(frequency, 4))+'\n'
                dataSourceText += '    Signals = { \n'
                for nodeDict in nonGamInputNodes:
                    dataSourceText += '      '+nodeDict['name']+' = {\n'
                    valExpr = nodeDict['expr']
                    if isinstance(valExpr, TreeNode):
#                        valExpr = valExpr.getPath()
                        valExpr = str(valExpr)
                    else:
                        valExpr = str(valExpr)
                    valExpr = valExpr.replace('"', "'")
                    dataSourceText += '        DataExpr = "'+valExpr+'"\n'
                    dataSourceText += '        TimebaseExpr = "dim_of(' + \
                        valExpr+')"\n'
                    numberOfElements = 1
                    if not (np.isscalar(nodeDict['dimensions'])):
                        for currDim in nodeDict['dimensions']:
                            numberOfElements *= currDim
                    dataSourceText += '        NumberOfElements = ' + \
                        str(numberOfElements)+'\n'
                    if nodeDict['col_order']:
                        dataSourceText += '        UseColumnOrder = 1\n'
                    else:
                        dataSourceText += '        UseColumnOrder = 0\n'
                    dataSourceText += '        DataManagement = 1\n'
                    dataSourceText += '      }\n'
                dataSourceText += '      timebase = {\n'
                dataSourceText += '        NumberOfElements = 1\n'
                dataSourceText += '        Type = uint64\n'
                dataSourceText += '         }\n'
                dataSourceText += '    }\n'
                dataSourceText += '  }\n'
                dataSources.append(dataSourceText)

        # Some outputs are connected to devices on separate synchronized theads
        if len(synchThreadSignals) > 0:
            dataSourceText = '  +'+gamName+'_Output_Synch = {\n'
            dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(gamName+'_Output_Synch_IOGAM')
            gamText = '  +'+gamName+'_Output_Synch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signal in synchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signal in synchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_Synch\n'
                gamText += '        Type = '+outputDict['type']+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

        # Some outputs are connected to devices on separate synchronized theads
        if len(asynchThreadSignals) > 0:
            dataSourceText = '  +'+gamName+'_Output_Asynch = {\n'
            dataSourceText += '    Class = RealTimeThreadAsyncBridge\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(gamName+'_Output_Asynch_IOGAM')
            gamText = '  +'+gamName+'_Output_Asynch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signal in asynchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_DDB\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signal in asynchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+gamName+'_Output_Asynch\n'
                gamText += '        Type = '+outputDict['type']+'\n'
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
            gamList.append(gamName+'_Logger_IOGAM')

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
                if self.isUsedOnAnotherThread(threadMap, self.timebase, True):

                    dataSourceText = '  +'+dataSourceName+'_Timer_Synch = {\n'
                    dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
                    dataSourceText += '    Timeout = 10000\n'
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
        nonGamInputNodes = []
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
                dataSourceText += '        NodeName = "' + \
                    configDict['outTimeNid'].getFullPath()+'"\n'
                # We must keep into account the number of samples in an input device
                dataSourceText += '        Period = ' + \
                    str(period/outputDict['samples'])+'\n'
                dataSourceText += '        MakeSegmentAfterNWrites = 100\n'
                dataSourceText += '        AutomaticSegmentation = 0\n'
                dataSourceText += '        Type = uint32\n'
                #if startTime != 0:
                #    dataSourceText += '        SamplePhase = ' + \
                #        str(int(round(startTime/period)))+'\n'
                dataSourceText += '      }\n'

            outIdx = 0
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    dataSourceText += '      '+outputDict['name']+' = {\n'
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

        synchThreadSignals = []
        asynchThreadSignals = []
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
            samples = outputDict['samples']
            if samples > 1:
                gamText += '        NumberOfDimensions = 1\n'
            else:
                gamText += '        NumberOfDimensions = ' + \
                    str(numberOfDimensions)+'\n'
            gamText += '        NumberOfElements = ' + \
                str(numberOfElements * samples)+'\n'
            gamText += '      }\n'
            if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], True):
                synchThreadSignals.append(outputDict['name'])
            if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], False):
                asynchThreadSignals.append(outputDict['name'])
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

        if configDict['storeSignals']:
            gamList.append(dataSourceName+'_TreeOutIOGAM')
            gamText = '  +'+dataSourceName+'_TreeOutIOGAM = {\n'
            if outputTrigger == None:
                gamText += '    Class = IOGAM\n'
            else:
                # Trigger signal must be delivered as uint8
                gamText += '    Class = ConversionGAM\n'
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
                        elif False:
                            gamText += '        DataSource = '+triggerGamName+'_Output_Synch\n'

                            try:
                                syncDiv = self.timebase_div.data()
                                gamText += '        Samples = ' + \
                                    str(syncDiv)+'\n'
                            except:
                                pass  # Consider ealTimeSynchronization downsampling only if timebase_div is defined

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
                    gamText += '        DataSource = '+dataSourceName+'_TreeInput\n'
                    gamText += '        Type = uint8\n'
                    gamText += '      }\n'
# end Trigger Management

# If the Input device is not synchronising, transfer also time towards MdsWriter
            if not isSynch:
                gamText += '      Time = {\n'
# GABRIELE SEPT 2020          gamText += '      DataSource = '+dataSourceName+'_Timer_DDB'
                gamText += '      DataSource = '+timerDDB
                gamText += '    }\n'

# Other signals
            for outputDict in outputDicts:
                if outputDict['seg_len'] > 0:
                    gamText += '      '+outputDict['name'] + ' = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
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

        if len(nonGamInputNodes) > 0:  # For input devices this happens only if a trigger signal for MdsWriter is derived from a waveform stored in the tree
            dataSourceText = '  +'+dataSourceName+'_TreeInput = {\n'
            dataSourceText += '    Class = MDSReaderNS\n'
            dataSourceText += '    UseColumnOrder = 0\n'
            dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
            dataSourceText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
            currTimebase = self.getNode('timebase').evaluate()
            if isinstance(currTimebase, Range):
                startTime = currTimebase.begin.data()
                period = currTimebase.delta.data()
            else:
                currTimebase = currTimebase.data()
                startTime = currTimebase[0]
                period = currTimebase[1] - currTimebase[0]
            frequency = 1./period

            dataSourceText += '    StartTime = '+str(startTime)+'\n'
            dataSourceText += '    Frequency = '+str(round(frequency, 4))+'\n'
            dataSourceText += '    Signals = { \n'
            for nodeDict in nonGamInputNodes:
                dataSourceText += '      '+nodeDict['name']+' = {\n'
                valExpr = nodeDict['expr']
                if isinstance(valExpr, TreeNode):
                    valExpr = valExpr.getFullPath()
                valExpr = valExpr.replace('"', "'")
                dataSourceText += '        DataExpr = "'+valExpr+'"\n'
                dataSourceText += '        TimebaseExpr = "dim_of(' + \
                    valExpr+')"\n'
                numberOfElements = 1
                if not (np.isscalar(nodeDict['dimensions'])):
                    for currDim in nodeDict['dimensions']:
                        numberOfElements *= currDim
                dataSourceText += '        NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
                if nodeDict['col_order']:
                    dataSourceText += '        UseColumnOrder = 1\n'
                else:
                    dataSourceText += '        UseColumnOrder = 0\n'
                dataSourceText += '        DataManagement = 1\n'
                dataSourceText += '      }\n'
            dataSourceText += '      timebase = {\n'
            dataSourceText += '        NumberOfElements = 1\n'
            dataSourceText += '        Type = uint64\n'
            dataSourceText += '         }\n'
            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

        # Some outputs are connected to devices on separate synchronized theads
        if len(synchThreadSignals) > 0:
            dataSourceText = '  +'+dataSourceName+'_Output_Synch = {\n'
            dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(dataSourceName+'_Output_Synch_IOGAM')

            gamText = '  +'+dataSourceName+'_Output_Synch_IOGAM = {\n'

            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signal in synchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signal in synchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_Synch\n'
                gamText += '        Type = '+outputDict['type']+'\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '  }\n'
            gams.append(gamText)

        # Some outputs are connected to devices on separate synchronized theads
        if len(asynchThreadSignals) > 0:
            dataSourceText = '  +'+dataSourceName+'_Output_Asynch = {\n'
            dataSourceText += '    Class = RealTimeThreadAsyncBridge\n'
            dataSourceText += ' }\n'
            dataSources.append(dataSourceText)

            gamList.append(dataSourceName+'_Output_Asynch_IOGAM')
            gamText = '  +'+dataSourceName+'_Output_Asynch_IOGAM = {\n'
            gamText += '    Class = IOGAM\n'
            gamText += '    InputSignals = {\n'
            for signal in asynchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
                gamText += '      }\n'
            gamText += '    }\n'
            gamText += '    OutputSignals = {\n'
            for signal in asynchThreadSignals:
                gamText += '      '+signal+' = {\n'
                gamText += '        DataSource = '+dataSourceName+'_Output_Asynch\n'
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
        gamList.append(dataSourceName+'_IOGAM')
        gamText = '  +'+dataSourceName+'_IOGAM = {\n'
        gamText += '    Class = IOGAM\n'

# input Signals
        gamText += '    InputSignals = {\n'
        nonGamInputNodes = []
        signalNames = []
        signalSamples = []  # to record the same number of samples for the output IOGAM fields

        for inputDict in inputDicts:
            # This is a Time field referring to this timebase
            if inputDict['value'].getNodeName() == 'TIMEBASE' and inputDict['value'].getParent().getNid() == self.getNid():
                signalNames.append('Time')
                gamText += '      Time = {\n'
                gamText += '      DataSource = ' + timerDDB+'\n'
                signalSamples.append(1)
            else:  # Normal reference
                isTreeRef = False
                isInputStructField = (
                    inputDict['value'].getParent().getParent().getName() == 'FIELDS')
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
                    nonGamInputNodes.append(
                        {'expr': inputDict['value'], 'dimensions': inputDict['dimensions'], 'name': signalName, 'col_order': inputDict['col_order']})
                    gamText += '      '+signalName+' = {\n'
                    gamText += '        DataSource = '+dataSourceName+'_TreeInput\n'
                    if 'type' in inputDict:
                        gamText += '        Type = '+inputDict['type']+'\n'
                else:

                    # Used to force the use of Samples instead of dimensions in case of SYnch datasource
                    forceUsingSamples = False

                    signalNames.append(signalGamName)
                    gamText += '      '+signalGamName+' = {\n'
                    if isInputStructField:  # Input struct fields are supported only within the same thread
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
            gamText += '      '+signalNames[idx]+' = {\n'
            if signalSamples[idx] > 1:
                gamText += '        Samples = '+str(signalSamples[idx])+'\n'

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

        # There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        if len(nonGamInputNodes) > 0:
            dataSourceText = '  +'+dataSourceName+'_TreeInput = {\n'
            dataSourceText += '    Class = MDSReaderNS\n'
            dataSourceText += '    UseColumnOrder = 0\n'
            dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
            dataSourceText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
            currTimebase = self.getNode('timebase').evaluate()
            if isinstance(currTimebase, Range):
                startTime = currTimebase.begin.data()
                period = currTimebase.delta.data()
            else:
                currTimebase = currTimebase.data()
                startTime = currTimebase[0]
                period = currTimebase[1] - currTimebase[0]
            frequency = 1./period

            dataSourceText += '    StartTime = '+str(startTime)+'\n'
#        dataSourceText += '    Frequency = '+str(int(round(frequency)))+'\n'
            dataSourceText += '    Frequency = '+str(round(frequency, 4))+'\n'
            dataSourceText += '    Signals = { \n'
            for nodeDict in nonGamInputNodes:
                dataSourceText += '      '+nodeDict['name']+' = {\n'
                valExpr = nodeDict['expr']
                if isinstance(valExpr, TreeNode):
                    valExpr = valExpr.getFullPath()
                valExpr = valExpr.replace('"', "'")
                dataSourceText += '        DataExpr = "'+valExpr+'"\n'
                dataSourceText += '        TimebaseExpr = "dim_of(' + \
                    valExpr+')"\n'
                numberOfElements = 1
                if not (np.isscalar(nodeDict['dimensions'])):
                    for currDim in nodeDict['dimensions']:
                        numberOfElements *= currDim
                dataSourceText += '        NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
                dataSourceText += '        DataManagement = 1\n'
                if nodeDict['col_order']:
                    dataSourceText += '        UseColumnOrder = 1\n'
                else:
                    dataSourceText += '        UseColumnOrder = 0\n'
                dataSourceText += '      }\n'
            dataSourceText += '      timebase = {\n'
            dataSourceText += '        NumberOfElements = 1\n'
            dataSourceText += '        Type = uint64\n'
            dataSourceText += '         }\n'

            dataSourceText += '    }\n'
            dataSourceText += '  }\n'
            dataSources.append(dataSourceText)

       #Head and parameters
        dataSourceText = '  +'+dataSourceName+' = {\n'
        dataSourceText += '    Class = '+dataSourceClass+'\n'
# parameters
        dataSourceText = self.reportParameters(paramDicts, dataSourceText, 1)

# input Signals
        dataSourceText += '    Signals = {\n'
        idx = 0
        for inputDict in inputDicts:
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
            mode = self.getNode('MODE').data()
        except:
            raise Exception('No mode field defined for '+self.getPath())
        if mode == MARTE2_COMPONENT.MODE_GAM:
            return self.generateMarteGamConfiguration(threadMap, timerDDB, timerType, timerPeriod, typesDict)
        elif mode == MARTE2_COMPONENT.MODE_INPUT or mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
            return self.generateMarteInputConfiguration(threadMap, timerDDB, timerType, timerPeriod, typesDict)
        else: #MODE_OUTPUT
           return self.generateMarteOutputConfiguration(threadMap, timerDDB, timerType, timerPeriod, typesDict)

   #return the Description (dictionary) corresponding ot this Interface device 
    def generateMarteInterfaceConfiguration(self):
        retInterface = {}
        retInterface['Name'] = self.getMarteDeviceName(self)
        try:
            retInterface['Class'] = self.getNode('GAM_CLASS').data()
        except:
            raise Exception('Missing class definition for '+self.getPath())
        try:
            numPars = len(self.getNode('PARAMETERS').getChildren())
        except:
            numPars = 0
        if numPars > 0:
            retInterface['Parameters'] = self.getParametersDict(self.getNode('PARAMETERS'))

        return retInterface



    def prepareMarteInfo(self):
        pass



