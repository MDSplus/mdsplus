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
import numpy as np
class MARTE2_COMPONENT(Device):
    """MARTE2 components superclass"""
    MODE_GAM = 1
    MODE_INPUT = 2
    MODE_SYNCH_INPUT = 3
    MODE_OUTPUT = 4
    @classmethod
    def buildParameters(cls, parts):
      parts.append({'path':'.PARAMETERS', 'type': 'structure'})
      idx = 1
      for parameter in cls.parameters:
	parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx), 'type':'structure'})
	parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx)+':NAME', 'type':'text', 'value':parameter['name']})
        if(parameter['type'] == 'string'):
	  if 'value' in parameter:
	    parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'text', 'value': parameter['value'] })
	  else:
	    parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'text'})
	else:
	  if 'value' in parameter:
	    parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'numeric', 'value':parameter['value']})
	  else:
	    parts.append({'path':'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'numeric'})
	idx = idx+1

    @classmethod
    def buildOutputs(cls, parts):
      parts.append({'path':'.OUTPUTS', 'type': 'structure'})
      parts.append({'path':'.OUTPUTS:TRIGGER', 'type': 'numeric'})
      idx = 1
      nameList = []
      for output in cls.outputs:
#        sigName = output['name'][0:12]
        sigName = cls.convertName(output['name'], nameList)
        parts.append({'path':'.OUTPUTS.'+sigName, 'type':'structure'})
	parts.append({'path':'.OUTPUTS.'+sigName+':NAME', 'type':'text', 'value':output['name']})
	parts.append({'path':'.OUTPUTS.'+sigName+':TYPE', 'type':'text', 'value':output['type']})
	parts.append({'path':'.OUTPUTS.'+sigName+':DIMENSIONS', 'type':'numeric', 'value':Data.compile(str(output['dimensions']))})
	if 'seg_len' in output:
	  parts.append({'path':'.OUTPUTS.'+sigName+':SEG_LEN', 'type':'numeric', 'value':output['seg_len']})
	else:
	  parts.append({'path':'.OUTPUTS.'+sigName+':SEG_LEN', 'type':'numeric', 'value':100})
        if(output['type'] == 'string'):
	  parts.append({'path':'.OUTPUTS.'+sigName+':VALUE', 'type':'text'})
	else:
	  parts.append({'path':'.OUTPUTS.'+sigName+':VALUE', 'type':'numeric'})
	pars = output['parameters']
	idx = 1
	parts.append({'path':'.OUTPUTS.'+sigName+'.PARAMETERS', 'type':'structure'})
	if len(pars) > 0:
	  for par in pars:
	    parts.append({'path':'.OUTPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx), 'type':'structure'})
	    parts.append({'path':'.OUTPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':NAME', 'type':'text', 'value':par['name']})
            if(par['type'] == 'string'):
	       parts.append({'path':'.OUTPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'text', 'value':par['value']})
	    else:
	       parts.append({'path':'.OUTPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'numeric', 'value':par['value']})

	    idx = idx + 1

    @classmethod
    def buildInputs(cls, parts):
      parts.append({'path':'.INPUTS', 'type': 'structure'})
      idx = 1
      nameList = []
      for input in cls.inputs:
#        sigName = input['name'][0:12]
        sigName = cls.convertName(input['name'], nameList)
        parts.append({'path':'.INPUTS.'+sigName, 'type':'structure'})
	parts.append({'path':'.INPUTS.'+sigName+':TYPE', 'type':'text', 'value':input['type']})
	parts.append({'path':'.INPUTS.'+sigName+':DIMENSIONS', 'type':'numeric', 'value':Data.compile(str(input['dimensions']))})
        parts.append({'path':'.INPUTS.'+sigName+':COL_ORDER', 'type':'text', 'value':'NO'})
 	parts.append({'path':'.INPUTS.'+sigName+':VALUE', 'type':'numeric'})
 	pars = input['parameters']
	idx = 1
	parts.append({'path':'.INPUTS.'+sigName+'.PARAMETERS', 'type':'structure'})
	if len(pars) > 0:
	  for par in pars:
	    parts.append({'path':'.INPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx), 'type':'structure'})
	    parts.append({'path':'.INPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':NAME', 'type':'text', 'value':par['name']})
            if(par['type'] == 'string'):
	       parts.append({'path':'.INPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'text', 'value':par['value']})
	    else:
	       parts.append({'path':'.INPUTS.'+sigName+'.PARAMETERS.PARAMETER_'+str(idx)+':VALUE', 'type':'numeric', 'value':par['value']})
            idx = idx + 1

    @classmethod
    def buildGam(cls, parts, clazz, mode, timebaseExpr=None):
      parts.append({'path':':GAM_CLASS', 'type':'text', 'value':clazz})
      parts.append({'path':':MODE', 'type':'numeric', 'value':mode})
      if timebaseExpr == None:
        parts.append({'path':':TIMEBASE', 'type': 'numeric'})
      else:
	parts.append({'path':':TIMEBASE', 'type': 'numeric', 'value':Data.compile(timebaseExpr)})

      cls.buildParameters(parts)
      if mode == MARTE2_COMPONENT.MODE_GAM or mode == MARTE2_COMPONENT.MODE_OUTPUT:
          cls.buildInputs(parts)

      if mode != MARTE2_COMPONENT.MODE_OUTPUT:
          cls.buildOutputs(parts)

#      for part in parts:
#	print(part)

    name = None
    inputs = []
    outputs = []
    parameters = []


    def convertPath(self, path):
      name = path[path.find('::TOP')+6:]
      name = name.replace(':', '_')
      name = name.replace('.', '_')
      return name

    def addSignalParameters(self, parsNode, text):
      for parNode in parsNode.getChildren():
        name = parNode.getNode(':NAME').data()
        value = parNode.getNode(':VALUE').data()
        if parNode.getNode(':VALUE').getUsage() == 'TEXT':
          text += '        '+name+' = "'+str(value)+'"\n'
	else:
          text += '        '+name+' = '+str(value)+'\n'
      return text

    @classmethod
    def convertName(cls, name, nameList):
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
        timebase = self.timebase.getData()
      except:
        timebase = None
      mode = self.mode.getData()
      gamName = self.convertPath(self.getFullPath().data())

      gamClass = self.gam_class.data() #class is a py
      gamMode = self.mode.data()

      paramDicts = []
      for parameter in self.getNode('parameters').getChildren():
        paramDict = {}
        try:
          paramDict['name'] = parameter.getNode('name').data()
          paramDict['value'] = parameter.getNode('value').data()
          paramDict['is_text']= (parameter.getNode('value').getUsage() == 'TEXT')
          paramDicts.append(paramDict)
        except:
          pass

      inputDicts = []
      if mode == MARTE2_COMPONENT.MODE_GAM or mode == MARTE2_COMPONENT.MODE_OUTPUT:
        inputNids = np.sort(self.getNode('inputs').getChildren())
        for inputNid in inputNids:
	  input = TreeNode(inputNid, self.getTree())
          inputDict = {}
          inputDict['type'] = input.getNode('type').data()
          inputDict['dimensions'] = input.getNode('dimensions').data()
          inputDict['value'] = input.getNode('value').getData() #NOT data()
          inputDict['value_nid'] = input.getNode('value')
          inputDict['col_order'] = input.getNode('col_order').data().upper()=='YES'
          inputDicts.append(inputDict)

      outputDicts = []
      if mode != MARTE2_COMPONENT.MODE_OUTPUT:
        outputNids = np.sort(self.getNode('outputs').getChildren())
        for outputNid in outputNids:
	  output = TreeNode(outputNid, self.getTree())
          outputDict = {}
          outputDict['name'] = output.getNode('name').data()
          outputDict['type'] = output.getNode('type').data()
          outputDict['dimensions'] = output.getNode('dimensions').data()
          outputDict['value_nid'] = output.getNode(':value')
          outputDict['seg_len'] = output.getNode(':seg_len').data()
          outputDicts.append(outputDict)
      try:
	outputTrigger = self.outputs_trigger.getData()
      except:
	outputTrigger = None

      return {'gamName':gamName, 'gamClass':gamClass , 'gamMode':gamMode,
	  'timebase':timebase, 'paramDicts':paramDicts, 'inputDicts':inputDicts, 'outputDicts':outputDicts, 'outputTrigger':outputTrigger}



    def onSameThread(self, threadMap, node):
      nid1 = self.getNid()
      nid2 = node.getNid()
      if len(threadMap[nid1]) != len(threadMap[nid2]):
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


    def isUsedOnAnotherThread(self, threadMap, outValueNode, isSynch):
      devList = self.getDevList(threadMap)
      for dev in devList:
        if not self.onSameThread(threadMap, dev):
          inputSigs = dev.getNode('.INPUTS')
          for inputChan in inputSigs.getChildren():
            try:
              inputNid =  inputChan.getNode('VALUE').getData().getNid()
            except:
              print('ERROR: Inconsistent Input for device '+ dev.getFullPath()+ ' : '+ inputChan)
              return False
            if inputNid == outValueNode.getNid():
              if self.sameSynchSource(dev):
                return isSynch
              else:
                return not isSynch
	  # We need to check also Output Trigger
	    try:
	      outputTriggerNid = dev.getNode('.OUTPUTS:TRIGGER').getData().getNid()
	      if outputTriggerNid == outValNode.getNid():
                if self.sameSynchSource(dev):
                  return isSynch
                else:
                  return not isSynch
	    except: #No Output Trigger defined
	      pass

      return False


#######################GAM

    def getMarteGamInfo(self, threadMap, gams, dataSources, gamList):
      configDict = self.getGamInfo()
      gamName = configDict['gamName']
      gamClass = configDict['gamClass']
      timebase = configDict['timebase']
      paramDicts = configDict['paramDicts']
      inputDicts = configDict['inputDicts']
      outputDicts = configDict['outputDicts']
      outputTrigger = configDict['outputTrigger']

# timebase
      if isinstance(timebase, Range):
        period = timebase.getDescAt(2).data()
        dataSourceText = '  +'+gamName+'_Timer'+ ' = {\n'
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

        gamList.append(gamName+'Timer_IOGAM')
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
        gamText += '        Frequency = '+str(int(round(1./period)))+'\n'
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


        if self.isUsedOnAnotherThread(threadMap, self.timebase, True): #Check if time information is required by another synchronized thread

          dataSourceText = '  +'+gamName+'_Timer_Synch = {\n'
          dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
          dataSourceText += '    Timeout = 10000\n'
          dataSourceText += ' }\n'
          dataSources.append(dataSourceText)

          gamList.append(gamName+'Timer_Synch_IOGAM')
          gamText = '  +'+gamName+'Timer_Synch_IOGAM = {\n'
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
          gamText += '        Frequency = '+str(int(round(1./period)))+'\n'
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

      elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):  #Link to other component up in the chain
        prevTimebase = timebase
        while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
          if isinstance(timebase, TreeNode):
            prevTimebase = timebase
            timebase = timebase.getData()
          else:
            prevTimebase = TreeNode(timebase, self.getTree())
            timebase = prevTimebase.getData()
        origName = self.convertPath(prevTimebase.getParent().getFullPath().data())
        #Check whether the synchronization source is a Sunch Input. Only in this case, the origin DDB is its output DDB since that device is expected to produce Time
        originMode = prevTimebase.getParent().getNode('mode').data()
        if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
          if self.onSameThread(threadMap, prevTimebase.getParent()):
            timerDDB = origName+'_Output_DDB'
          else:
            timerDDB = orig_name+'_Output_Synch'
        else:
          if self.onSameThread(threadMap, prevTimebase.getParent()):
            timerDDB = origName+'_Timer_DDB'
          else:
            timerDDB = origName+'_Timer_Synch'

      else:
        print('ERROR: Invalid timebase definition')
        return 0

 #Head and parameters
      gamList.append(gamName)
      gamText = '  +'+gamName+' = {\n'
      gamText += '    Class = '+gamClass+'\n'
      for paramDict in paramDicts:
	if paramDict['is_text']:
          gamText += '    '+paramDict['name']+' = "'+str(paramDict['value'])+'"\n'
        else:
          gamText += '    '+paramDict['name']+' = '+str(paramDict['value'])+'\n'

#input Signals
      gamText += '    InputSignals = {\n'
      treeInputs = False
      nonGamInputNodes = []
      for inputDict in inputDicts:
        if inputDict['value'].getName() == 'TIMEBASE' and inputDict['value'].getParent().getParent() == self: #This is a Time field referring to this timebase
          gamText += '      Time = {\n'
          gamText += '      DataSource = '+ timerDDB+'\n'
        else:  #Normal reference
	  isTreeRef = False
	  try:
	    sourceNode = inputDict['value'].getParent().getParent().getParent()
	    if sourceNode.getUsage() != 'DEVICE':
	      isTreeRef = True
	    else:
              sourceGamName = self.convertPath(sourceNode.getFullPath().data())
              signalGamName = inputDict['value'].getParent().getNode(':name').data()
          except:
	    isTreeRef = True
	  if isTreeRef:
	    signalName = self.convertPath(inputDict['value_nid'].getPath().data())
            nonGamInputNodes.append({'expr':inputDict['value'], 'dimensions': inputDict['dimensions'], 'name':signalName, 'col_order':inputDict['col_order']})
            gamText += '      '+signalName+' = {\n'
            gamText += '        DataSource = '+gamName+'_TreeInput\n'
	  else:
            gamText += '      '+signalGamName+' = {\n'
            if self.onSameThread(threadMap, sourceNode):
	      gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
            elif self.sameSynchSource(sourceNode):
	      gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
            else:
 	      gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'

        if 'type' in inputDict:
          gamText += '        Type = '+inputDict['type']+'\n'
        if 'dimensions' in inputDict:
	  print(inputDict['dimensions'])
	  dimensions = inputDict['dimensions']
          if dimensions == 0:
            numberOfElements = 1
            numberOfDimensions = 0
          else:
            numberOfDimensions = len(inputDict['dimensions'])
            numberOfElements = 1
            for currDim in inputDict['dimensions']:
              numberOfElements *= currDim
          gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
          gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
          gamText = self.addSignalParameters(inputDict['value_nid'].getParent().getNode('parameters'), gamText)
        gamText += '      }\n'
      gamText += '    }\n'

      if len(nonGamInputNodes) > 0:  #There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        dataSourceText = '  +'+gamName+'_TreeInput = {\n'
        dataSourceText += '    Class = MDSReaderNS\n'
        dataSourceText += '    UseColumnOrder = 0\n'
        dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
        dataSourceText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
        currTimebase = self.getNode('timebase').evaluate()
        if isinstance(currTimebase, Range):
          startTime = currTimebase.getBegin().data()
          period = currTimebase.getDelta().data()
        else:
          currTimebase = currTimebase.data()
          startTime - currTimebase[0]
          period = currTimebase[1] - currTimebase[0]
        frequency = 1./period

        dataSourceText += '    StartTime = '+str(startTime)+'\n'
        dataSourceText += '    Frequency = '+str(int(round(frequency)))+'\n'
        dataSourceText += '    Signals = { \n'
        for nodeDict in nonGamInputNodes:
          dataSourceText += '      '+nodeDict['name']+' = {\n'
          valExpr = nodeDict['expr']
          if isinstance(valExpr, TreeNode):
	    valExpr = valExpr.getFullPath().data()
          dataSourceText += '        DataExpr = "'+valExpr+'"\n'
          dataSourceText += '        TimebaseExpr = "dim_of('+valExpr+')"\n'
          numberOfElements = 1
          if not (np.isscalar(nodeDict['dimensions'])):
	    for currDim in nodeDict['dimensions']:
              numberOfElements *= currDim
          dataSourceText += '        NumberOfElements = '+str(numberOfElements)+'\n'
          if nodeDict['col_order']:
	    dataSourceText += '        UseColumnOrder = 1\n'
	  else:
	    dataSourceText += '        UseColumnOrder = 0\n'
          dataSourceText += '        DataManagement = 1\n'
          dataSourceText += '      }\n'
        dataSourceText += '      timebase = {\n'
	dataSourceText += '        NumberOfElements = 1\n'
	dataSourceText += '        Type = uint64\n'
	dataSourceText += '	 }\n'
        dataSourceText += '    }\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)
    #Output Signals
      synchThreadSignals = []
      asynchThreadSignals = []
      gamText += '    OutputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name']+' = {\n'
        gamText += '        DataSource = '+gamName+'_Output_DDB\n'
        gamText += '        Type = '+outputDict['type']+'\n'
        if outputDict['dimensions'] == 0:
          numberOfElements = 1
          numberOfDimensions = 0
        else:
          numberOfDimensions = len(outputDict['dimensions'])
          numberOfElements = 1
          for currDim in outputDict['dimensions']:
            numberOfElements *= currDim
        gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText = self.addSignalParameters(outputDict['value_nid'].getParent().getNode('parameters'), gamText)
        gamText += '      }\n'
        if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], True):
          synchThreadSignals.append(outputDict['name'])
        if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], False):
          asynchThreadSignals.append(outputDict['name'])
      gamText += '    }\n'
      gamText += '  }\n'
      gams.append(gamText)
    #We need to declare out DDB, out MdsWriter and relative IOGAM
      dataSourceText = '  +'+gamName+'_Output_DDB = {\n'
      dataSourceText += '    Class = GAMDataSource\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)

      dataSourceText = '  +'+gamName+'_TreeOutput = {\n'
      dataSourceText += '    Class = MDSWriter\n'
      dataSourceText += '    NumberOfBuffers = 1000\n'
      dataSourceText += '    CPUMask = 15\n'
      dataSourceText += '    StackSize = 10000000\n'
      dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
      dataSourceText += '    PulseNumber = '+str(self.getTree().shot)+'\n'
      if outputTrigger == None:
        dataSourceText += '    StoreOnTrigger = 0\n'
      else:
        dataSourceText += '    StoreOnTrigger = 1\n'
      dataSourceText += '    EventName = "'+gamName+'UpdatejScope"\n'
      dataSourceText += '    TimeRefresh = 1\n'
      dataSourceText += '    Signals = {\n'
      currTimebase = self.timebase.evaluate()
      if isinstance(currTimebase, Range):
         period = currTimebase.getDelta().data()
      else:
         currTimebase = currTimebase.data()
         period = currTimebase[1] - currTimebase[0]

#If trigger is defined put it as first signal
      if outputTrigger != None:
	dataSourceText += '      Trigger = {\n'
	dataSourceText += '      type = uint8\n'
	dataSourceText += '    }\n'
      for outputDict in outputDicts:
        dataSourceText += '      '+outputDict['name']+' = {\n'
        dataSourceText += '        NodeName = "'+outputDict['value_nid'].getFullPath().data()+'"\n'
        dataSourceText += '        Period = '+str(period)+'\n'
        dataSourceText += '        MakeSegmentAfterNWrites = '+str(outputDict['seg_len'])+'\n'
        dataSourceText += '        AutomaticSegmentation = 0\n'
        dataSourceText += '      }\n'
      dataSourceText += '    }\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)

      gamList.append(gamName+'_TreeOutIOGAM')
      gamText ='  +'+gamName+'_TreeOutIOGAM = {\n'
      gamText += '    Class = IOGAM\n'
      gamText += '    InputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name'] + ' = {\n'
        gamText += '        DataSource = '+gamName+'_Output_DDB\n'
        gamText += '      }\n'
#MdsWriter Trigger management
      if outputTrigger != None:
        triggerNode = outputTrigger.getParent().getParent().getParent()
        triggerGamName = self.convertPath(sourceNode.getFullPath().data())
        triggerSigName = outputTrigger.getParent().getNode(':name').data()
        gamText += '      '+triggerSigName+' = {\n'
        if self.onSameThread(threadMap, triggerNode):
	  gamText += '        DataSource = '+triggerGamName+'_Output_DDB\n'
        elif self.sameSynchSource(sourceNode):
	  gamText += '        DataSource = '+triggerGamName+'_Output_Synch\n'
        else:
 	  gamText += '        DataSource = '+triggerGamName+'_Output_Asynch\n'
        gamText += '      }\n'

      gamText += '    }\n'
      gamText += '    OutputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name'] + ' = {\n'
        gamText += '        DataSource = '+gamName+'_TreeOutput\n'
        gamText += '        Type = '+outputDict['type']+'\n'
        if outputDict['dimensions'] == 0:
          numberOfElements = 1
          numberOfDimensions = 0
        else:
          numberOfDimensions = len(outputDict['dimensions'])
          numberOfElements = 1
          for currDim in outputDict['dimensions']:
            numberOfElements *= currDim
        gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText += '      }\n'

      if outputTrigger != None:
	gamText += '      Trigger = {\n'
	gamText += '      DataSource = '+gamName+'_TreeOutput\n'
	gamText += '      type = uint8\n'
	gamText += '    }\n'
      gamText += '    }\n'
      gamText += '  }\n'
      gams.append(gamText)

      if len(synchThreadSignals) > 0:  # Some outputs are connected to devices on separate synchronized theads
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
          gamText += '      }\n'
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

      if len(asynchThreadSignals) > 0:  # Some outputs are connected to devices on separate synchronized theads
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
          gamText += '      }\n'
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)



#############################SYNCH and NON SYNCH  INPUT
    def getMarteInputInfo(self, threadMap, gams, dataSources, gamList, isSynch):
      configDict = self.getGamInfo()
      dataSourceName = configDict['gamName']
      dataSourceClass = configDict['gamClass']
      timebase = configDict['timebase']
      paramDicts = configDict['paramDicts']
      outputDicts = configDict['outputDicts']
      outputTrigger = configDict['outputTrigger']

      if not isSynch:
	# timebase must be considered only if not synchronizing
        if isinstance(timebase, Range):
          period = timebase.getDescAt(2).data()
          dataSourceText = '  +'+dataSourceName+'_Timer'+ ' = {\n'
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
          gamText += '        Frequency = '+str(int(round(1./period)))+'\n'
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
       #Unlike GAM no other  configuration needs to be considered since there are no inputs
# if isSynch,  timebase Will contain the defintion of the time that will be generated. Specific subclasses will connect it to DataSource specific parameters
#therefore no actions are taken here in addition

#Head and parameters
      dataSourceText = '  +'+dataSourceName+' = {\n'
      dataSourceText += '    Class = '+dataSourceClass+'\n'
      for paramDict in paramDicts:
	if paramDict['is_text']:
          dataSourceText += '    '+paramDict['name']+' = "'+str(paramDict['value'])+'"\n'
        else:
          dataSourceText += '    '+paramDict['name']+' = '+str(paramDict['value'])+'\n'

    #Output Signals
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
        dataSourceText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        dataSourceText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        dataSourceText = self.addSignalParameters(outputDict['value_nid'].getParent().getNode('parameters'), dataSourceText)
        dataSourceText += '      }\n'
      dataSourceText += '    }\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)

    #We need to declare out DDB, out MdsWriter and 2 IOGAMs (From this DataSource to DDB and from DDB to MdsWriter)
      dataSourceText = '  +'+dataSourceName+'_Output_DDB = {\n'
      dataSourceText += '    Class = GAMDataSource\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)

      dataSourceText = '  +'+dataSourceName+'_TreeOutput = {\n'
      dataSourceText += '    Class = MDSWriter\n'
      dataSourceText += '    NumberOfBuffers = 100\n'
      dataSourceText += '    CPUMask = 15\n'
      dataSourceText += '    StackSize = 10000000\n'
      dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
      dataSourceText += '    PulseNumber = '+str(self.getTree().shot)+'\n'
      if outputTrigger == None:
        dataSourceText += '    StoreOnTrigger = 0\n'
      else:
        dataSourceText += '    StoreOnTrigger = 1\n'
      dataSourceText += '    EventName = "'+dataSourceName+'UpdatejScope"\n'
      dataSourceText += '    TimeRefresh = 1\n'
      dataSourceText += '    Signals = {\n'
      currTimebase = self.getNode('timebase').evaluate()

      if isinstance(currTimebase, Range):
         period = currTimebase.getDelta().data()
      else:
         currTimebase = currTimebase.data()
         period = currTimebase[1] - currTimebase[0]

#If trigger is defined put it as first signal
      if outputTrigger != None:
	dataSourceText += '      Trigger = {\n'
	dataSourceText += '      type = uint8\n'
	dataSourceText += '    }\n'
      for outputDict in outputDicts:
        dataSourceText += '      '+outputDict['name']+' = {\n'
        dataSourceText += '        NodeName = "'+outputDict['value_nid'].getFullPath().data()+'"\n'
        dataSourceText += '        Period = '+str(period)+'\n'
        dataSourceText += '        MakeSegmentAfterNWrites = '+str(outputDict['seg_len'])+'\n'
        dataSourceText += '        AutomaticSegmentation = 0\n'
        dataSourceText += '      }\n'
      dataSourceText += '    }\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)

      gamList.append(dataSourceName+'_DDBOutIOGAM')
      gamText ='  +'+dataSourceName+'_DDBOutIOGAM = {\n'
      gamText += '    Class = IOGAM\n'
      gamText += '    InputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name'] + ' = {\n'
        gamText += '        DataSource = '+dataSourceName+'\n'
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
        gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText += '      }\n'
        if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], True):
          synchThreadSignals.append(outputDict['name'])
        if self.isUsedOnAnotherThread(threadMap, outputDict['value_nid'], False):
          asynchThreadSignals.append(outputDict['name'])
      gamText += '    }\n'
      gamText += '  }\n'
      gams.append(gamText)

      gamList.append(dataSourceName+'_TreeOutIOGAM')
      gamText ='  +'+dataSourceName+'_TreeOutIOGAM = {\n'
      gamText += '    Class = IOGAM\n'
      gamText += '    InputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name'] + ' = {\n'
        gamText += '        DataSource = '+dataSourceName+'_Output_DDB\n'
        gamText += '      }\n'
#MdsWriter Trigger management
      if outputTrigger != None:
        triggerNode = outputTrigger.getParent().getParent().getParent()
        triggerGamName = self.convertPath(sourceNode.getFullPath().data())
        triggerSigName = outputTrigger.getParent().getNode(':name').data()
        gamText += '      '+triggerSigName+' = {\n'
        if self.onSameThread(threadMap, triggerNode):
	  gamText += '        DataSource = '+triggerGamName+'_Output_DDB\n'
        elif self.sameSynchSource(sourceNode):
	  gamText += '        DataSource = '+triggerGamName+'_Output_Synch\n'
        else:
 	  gamText += '        DataSource = '+triggerGamName+'_Output_Asynch\n'
 	gamText += '      }\n'

      gamText += '    }\n'
      gamText += '    OutputSignals = {\n'
      for outputDict in outputDicts:
        gamText += '      '+outputDict['name'] + ' = {\n'
        gamText += '        DataSource = '+dataSourceName+'_TreeOutput\n'
        gamText += '        Type = '+outputDict['type']+'\n'
        if outputDict['dimensions'] == 0:
          numberOfElements = 1
          numberOfDimensions = 0
        else:
          numberOfDimensions = len(outputDict['dimensions'])
          numberOfElements = 1
          for currDim in outputDict['dimensions']:
            numberOfElements *= currDim
        gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText += '      }\n'

      if outputTrigger != None:
	gamText += '      Trigger = {\n'
	gamText += '      DataSource = '+gamName+'_TreeOutput\n'
	gamText += '      type = uint8\n'
	gamText += '    }\n'

      gamText += '    }\n'
      gamText += '  }\n'
      gams.append(gamText)

      if len(synchThreadSignals) > 0:  # Some outputs are connected to devices on separate synchronized theads
        dataSourceText = '  +'+gamName+'_Output_Synch = {\n'
        dataSourceText += '    Class = RealTimeThreadSynchronisation\n'
        dataSourceText += ' }\n'
        dataSources.append(dataSourceText)

        gamList.append(gamName+'_Output_Synch_IOGAM')
        gamText = '  +'+gamName+'_output_Synch_IOGAM = {\n'
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
          gamText += '      }\n'
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)

      if len(asynchThreadSignals) > 0:  # Some outputs are connected to devices on separate synchronized theads
        dataSourceText = '  +'+gamName+'_Output_Asynch = {\n'
        dataSourceText += '    Class = RealTimeThreadAsyncBridge\n'
        dataSourceText += ' }\n'
        dataSources.append(dataSourceText)

        gamList.append(gamName+'_Output_Asynch_IOGAM')
        gamText = '  +'+gamName+'_output_Asynch_IOGAM = {\n'
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
          gamText += '        DataSource = '+gamName+'_Output_Asynch\n'
          gamText += '      }\n'
        gamText += '    }\n'
        gamText += '  }\n'
        gams.append(gamText)




#######################OUTPUT

    def getMarteOutputInfo(self, threadMap, gams, dataSources, gamList):
      configDict = self.getGamInfo()
      dataSourceName = configDict['gamName']
      dataSourceClass = configDict['gamClass']
      timebase = configDict['timebase']
      paramDicts = configDict['paramDicts']
      inputDicts = configDict['inputDicts']
      outputDicts = configDict['outputDicts']


# timebase
      if isinstance(timebase, Range):
        period = timebase.getDescAt(2).data()
        dataSourceText = '  +'+dataSourceName+'_Timer'+ ' = {\n'
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
        gamText += '        Frequency = '+str(int(round(1./period)))+'\n'
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

      elif isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):  #Link to other component up in the chain
        prevTimebase = timebase
        while isinstance(timebase, TreeNode) or isinstance(timebase, TreePath):
          if isinstance(timebase, TreeNode):
            prevTimebase = timebase
            timebase = timebase.getData()
          else:
            prevTimebase = TreeNode(timebase, self.getTree())
            timebase = prevTimebase.getData()
        origName = prevTimebase.getParent().getFullPath().data()
        #Check whether the synchronization source is a Sunch Input. Only in this case, the origin DDB is its output DDB
        originMode = prevTimebase.getParent().getNode('mode').data()
        if originMode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
          timerDDB = origName+'_Output_DDB'
        else:
          timerDDB = origName+'_Timer_DDB'
      else:
        print('ERROR: Invalid timebase definition')
        return 0

 #Head and parameters
      gamList.append(dataSourceName+'_IOGAM')
      gamText = '  +'+dataSourceName+'_IOGAM = {\n'
      gamText += '    Class = IOGAM\n'

#input Signals
      gamText += '    InputSignals = {\n'
      treeInputs = False
      nonGamInputNodes = []
      signalNames = []
      for inputDict in inputDicts:
        if inputDict['value'].getName() == 'TIMEBASE' and inputDict['value'].getParent().getParent() == self: #This is a Time field referring to this timebase
	  signalNames.append('Time')
          gamText += '      Time = {\n'
          gamText += '      DataSource = '+ timerDDB+'\n'
        else:  #Normal reference
          isTreeRef = False
          try:
	    sourceNode = inputDict['value'].getParent().getParent().getParent()
            sourceGamName = self.convertPath(sourceNode.getFullPath().data())
            signalGamName = inputDict['value'].getParent().getNode(':name').data()
          except:
            isTreeRef = True
	  if isTreeRef:
	      signalName = self.convertPath(inputDict['value_nid'].getPath().data())
	      signalNames.append(signalName)
              nonGamInputNodes.append({'expr':inputDict['value'], 'dimensions': inputDict['dimensions'], 'name':signalName, 'col_order':inputDict['col_order']})
              gamText += '      '+signalName+' = {\n'
              gamText += '        DataSource = '+dataSourceName+'_TreeInput\n'
	  else:
            signalNames.append(signalGamName)
            gamText += '      '+signalGamName+' = {\n'
            if self.onSameThread(threadMap, sourceNode):
	      gamText += '        DataSource = '+sourceGamName+'_Output_DDB\n'
            elif self.sameSynchSource(sourceNode):
	      gamText += '        DataSource = '+sourceGamName+'_Output_Synch\n'
            else:
 	      gamText += '        DataSource = '+sourceGamName+'_Output_Asynch\n'
            if 'type' in inputDict:
              gamText += '        Type = '+inputDict['type']+'\n'
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
              gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
              gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText = self.addSignalParameters(inputDict['value_nid'].getParent().getNode('parameters'), gamText)
        gamText += '      }\n'
      gamText += '    }\n'

    #Output Signals IOGAM
      gamText += '    OutputSignals = {\n'
      idx = 0
      for outputDict in inputDicts:
        gamText += '      '+signalNames[idx]+' = {\n'
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
        gamText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
        gamText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        gamText += '      }\n'
      gamText += '    }\n'
      gamText += '  }\n'
      gams.append(gamText)

      if len(nonGamInputNodes) > 0:  #There are input references to tree nodes, we need to build a MdsReader DataSource named <gam name>_TreeInput
        dataSourceText = '  +'+dataSourceName+'_TreeInput = {\n'
        dataSourceText += '    Class = MDSReaderNS\n'
        dataSourceText += '    UseColumnOrder = 0\n'
        dataSourceText += '    TreeName = "'+self.getTree().name+'"\n'
        dataSourceText += '    ShotNumber = '+str(self.getTree().shot)+'\n'
        currTimebase = self.getNode('timebase').evaluate()
        if isinstance(currTimebase, Range):
          startTime = currTimebase.getBegin()
          period = currTimebase.getDelta()
        else:
          currTimebase = currTimebase.data()
          startTime - currTimebase[0]
          period = currTimebase[1] - currTimebase[0]
        frequency = 1./period

        dataSourceText += '    StartTime = '+str(startTime)+'\n'
        dataSourceText += '    Frequency = '+str(int(round(frequency)))+'\n'
        dataSourceText += '    Signals = { \n'
        for nodeDict in nonGamInputNodes:
          dataSourceText += '      '+nodeDict['name']+' = {\n'
          valExpr = nodeDict['expr']
          if isinstance(valExpr, TreeNode):
	    valExpr = valExpr.getFullPath().data()
          dataSourceText += '        DataExpr = "'+valExpr+'"\n'
          numberOfElements = 1
          if not (np.isscalar(nodeDict['dimensions'])):
	    for currDim in nodeDict['dimensions']:
              numberOfElements *= currDim
          dataSourceText += '        NumberOfElements = '+str(numberOfElements)+'\n'
          dataSourceText += '        DataManagement = 1\n'
          if nodeDict['col_order']:
	    dataSourceText += '        UseColumnOrder = 1\n'
	  else:
	    dataSourceText += '        UseColumnOrder = 0\n'
          dataSourceText += '      }\n'
        dataSourceText += '    }\n'
        dataSourceText += '  }\n'
        dataSources.append(dataSourceText)


 #Head and parameters
      dataSourceText = '  +'+dataSourceName+' = {\n'
      dataSourceText += '    Class = '+dataSourceClass+'\n'

#input Signals
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
          dataSourceText += '        NumberOfDimensions = '+str(numberOfDimensions)+'\n'
          dataSourceText += '        NumberOfElements = '+str(numberOfElements)+'\n'
        dataSourceText = self.addSignalParameters(inputDict['value_nid'].getParent().getNode('parameters'), dataSourceText)
        dataSourceText += '      }\n'
      dataSourceText += '    }\n'
      dataSourceText += '  }\n'
      dataSources.append(dataSourceText)



    def getMarteInfo(self, threadMap, gams, dataSources, gamList):
      self.prepareMarteInfo()
      mode = self.mode.data()
      if mode == MARTE2_COMPONENT.MODE_GAM:
        self.getMarteGamInfo(threadMap, gams, dataSources, gamList)
      elif mode == MARTE2_COMPONENT.MODE_SYNCH_INPUT:
	self.getMarteInputInfo(threadMap, gams, dataSources, gamList, True)
      elif mode == MARTE2_COMPONENT.MODE_INPUT:
	self.getMarteInputInfo(threadMap, gams, dataSources, gamList, False)
      else:
	self.getMarteOutputInfo(threadMap, gams, dataSources, gamList)

#Prpeparatory work before reading MARTe2 information. To be overridden by subclasses/
    def prepareMarteInfo(self):
      pass
