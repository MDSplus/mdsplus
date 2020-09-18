
from MDSplus import Device, Event, VECTOR
import subprocess
import numpy as np
import time
import traceback
MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_SUPERVISOR(Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [{'path':':NAME', 'type':'text'},{'path':':COMMENT', 'type':'text'}, {'path':':NUM_STATES', 'type':'numeric'}]
    for stateIdx in range(10):
      parts.append({'path':'.STATE_'+str(stateIdx+1), 'type':'structure'})
      parts.append({'path':'.STATE_'+str(stateIdx+1)+':NAME', 'type':'text'})
      parts.append({'path':'.STATE_'+str(stateIdx+1)+':NUM_THREADS', 'type':'numeric'})
      for threadIdx in range(10):
        parts.append({'path':'.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1), 'type':'structure'})
        parts.append({'path':'.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':NAME', 'type':'text'})
        parts.append({'path':'.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':CORE', 'type':'numeric'})
        parts.append({'path':'.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAMS', 'type':'numeric'})
    parts.append({'path':'.TIMES', 'type':'structure'})
    for stateIdx in range(10):
      parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1), 'type':'structure'})
      for threadIdx in range(10):
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1), 'type':'structure'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':SEG_LEN', 'type':'numeric', 'value':0})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':CPU_MASK', 'type':'numeric', 'value':15})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':CYCLE', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM1', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM2', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM3', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM4', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM5', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM6', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM7', 'type':'signal'})
        parts.append({'path':'.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAM8', 'type':'signal'})


    parts.append({'path':':INIT','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PON',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PON',51,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PPC',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})

    MODE_GAM = 1
    MODE_INPUT = 2
    MODE_SYNCH_INPUT = 3
    MODE_OUTPUT = 4

    def getGamList(self, state, thread):
      t = self.getTree()
      gams = getattr(self, 'state_%d_thread_%d_gams'%(state+1, thread+1)).getData()
      gamNids = []
      if isinstance(gams, VECTOR):
        for i in range(gams.getNumDescs()):
          currGamNid = gams.getDescAt(i)
          gamNids.append(currGamNid)
      else:
        for gam1 in gams.data():
          if isinstance(gam1, str):
            gam = gam1
          else:
            gam = str(gam1,'utf_8')
          currGamNid = t.getNode(gam);
          gamNids.append(currGamNid)
      print(gamNids)
      return gamNids
    

    def getInfo(self):
      try:
        error = ''
        info = {}
        t=self.getTree()
        numStates = self.num_states.data()
        statesInfo = []
        retData = []
        retGams = []
        threadMap = {}

      #first iteration to get threadMap
        for state in range(numStates):
          numThreads = getattr(self, 'state_%d_num_threads'%(state+1)).data()
          for thread in range(numThreads):
            threadName = getattr(self, 'state_%d_thread_%d_name'%(state+1, thread+1)).data()
            try:
              gamNodes = self.getGamList(state, thread)
            except:
              raise Exception('Cannot get GAM list for state: '+ str(state + 1) + ', thread: '+str(thread + 1))
            for currGamNode in gamNodes:
              nid = currGamNode.getNid()
              if nid in threadMap:
                threadMap[nid] += [threadName]
              else:
                threadMap[nid] = [threadName]


      #Second iteration, build the remaining
        for state in range(numStates):
          stateInfo = {}
          stateInfo['name'] = getattr(self, 'state_%d_name'%(state+1)).data()
          numThreads = getattr(self, 'state_%d_num_threads'%(state+1)).data()
          stateThreads = []
          for thread in range(numThreads):
            threadInfo = {}
            threadName = getattr(self, 'state_%d_thread_%d_name'%(state+1, thread+1)).data()
            try:
              core = getattr(self, 'state_%d_thread_%d_core'%(state+1, thread+1)).data()
              threadInfo['core'] = core
            except:
              pass
            threadInfo['name'] = threadName
            gamNames = []
            threadPeriod = 0
            gamNids = []
            gamNodes = self.getGamList(state, thread)
            for currGamNode in gamNodes:
              nid = currGamNode.getNid()
              if currGamNode.isOn():
                try:
                  gamClass = currGamNode.getData().getDevice()
                  gamInstance = gamClass(currGamNode)
                except:
                  raise Exeption('Cannot instantiate device for node '+currGamNode.getFullPath())
                gamList = []
                if not (currGamNode.getNid() in gamNids):
                  #try:
                  gamInstance.prepareMarteInfo()
                  currPeriod = gamInstance.getMarteInfo(threadMap, retGams, retData, gamList)
                  #except:
                    #return 'Cannot get timebase for ' + gam, {},{}
                  gamNids.append(currGamNode.getNid())
                  if currPeriod > 0 and threadPeriod > 0:
                    raise Exception('More than one component driving thread timing for state: '+str(state+1)+', thread: '+str(thread+1))
                  else:
                    if currPeriod > 0:
                      threadPeriod = currPeriod
                else:
                  dummyGams = []
                  dummyData = []
                  gamInstance.getMarteInfo(threadMap, dummyGams, dummyData, gamList)
                gamNames += gamList
#######################TIMINGS
            if threadPeriod == 0:
              raise Exception('No component driving thread timing for state: '+str(state+1)+', thread: '+str(thread+1))
            gamList = []
            self.getTimingInfo(state, thread, threadPeriod, retGams, retData, gamList)
            gamNames += gamList
#############################

            threadInfo['gams'] = gamNames
            stateThreads.append(threadInfo)
          stateInfo['threads'] = stateThreads
          statesInfo.append(stateInfo)
        info['states'] = statesInfo

        info['gams'] = retGams
        info['data_sources'] = retData
        info['name'] = self.getNode('name').data()
        return error, info, threadMap
      except Exception as inst:
        print(traceback.format_exc())
 #       return inst.args[0], None, None
        return str(inst), None, None


#Enrich GAMs and Data Sources with what is required to store timing information (IOGAM + TreeWriter) is seg_len > 0
    def getTimingInfo(self, state, thread, threadPeriod, retGams, dataSources, gamList):
      segLen = getattr(self, 'times_state_%d_thread_%d_seg_len'%(state+1, thread+1)).data()
      if(segLen == 0):
        return
      stateName = getattr(self, 'state_%d_name'%(state+1)).data()
      threadName =  getattr(self, 'state_%d_thread_%d_name'%(state+1, thread+1)).data()
      cpuMask = getattr(self, 'times_state_%d_thread_%d_cpu_mask'%(state+1, thread+1)).data()
      timeSignals = []
      gamNodes = self.getGamList(state, thread)
      for currGamNid in gamNodes:
        if currGamNid.isOn():
          gamName = currGamNid.getNodeName()
          gamClass = currGamNid.getData().getDevice()
          gamInstance = gamClass(currGamNid)
          gamMode = gamInstance.mode.data()
          if gamMode == MARTE2_SUPERVISOR.MODE_GAM:
            timeSignals.append(gamName+'_ReadTime')
            timeSignals.append(gamName+'_ExecTime')
          elif gamMode == MARTE2_SUPERVISOR.MODE_OUTPUT:
            timeSignals.append(gamName+'_IOGAM_WriteTime')
          else:
            timeSignals.append(gamName+'_DDBOutIOGAM_ReadTime')

      if len(timeSignals) == 0:
        return
      currGam = '+State_%d_Thread_%d_TIMES_IOGAM = {\n'%(state+1, thread+1)
      currGam += '  Class = IOGAM\n'
      currGam += '  InputSignals = {\n'
      currGam += '    '+stateName+'_'+threadName+'_CycleTime = {\n'
      currGam += '      Alias = '+stateName+'.'+threadName+'_CycleTime\n'
      currGam += '      DataSource = Timings\n'
      currGam += '      Type = uint32\n'
      currGam += '    }\n'

      for timeSignal in timeSignals:
        currGam += '    '+timeSignal+' = {\n'
        currGam += '      DataSource = Timings\n'
        currGam += '      Type = uint32\n'
        currGam += '    }\n'
      currGam += '  }\n'
      currGam += '  OutputSignals = {\n'
      currGam += '    CycleTime = {\n'
      currGam += '      DataSource = State_%d_Thread_%d_TIMES_WRITER\n'%(state+1, thread+1)
      currGam += '      Type = uint32\n'
      currGam += '  }\n'
      for timeSignal in timeSignals:
        currGam += '    '+timeSignal+' = {\n'
        currGam += '      DataSource = State_%d_Thread_%d_TIMES_WRITER\n'%(state+1, thread+1)
        currGam += '      Type = uint32\n'
        currGam += '    }\n'
      currGam += '  }\n'
      currGam += '}\n'

      retGams.append(currGam)
      gamList.append('State_%d_Thread_%d_TIMES_IOGAM'%(state+1, thread+1))

      dataSource = '  +State_%d_Thread_%d_TIMES_WRITER = {\n'%(state+1, thread+1)
      dataSource += '    Class = MDSWriter\n'
      dataSource += '    NumberOfBuffers = 20000\n'
      dataSource += '    CPUMask = '+ str(cpuMask)+'\n'
      dataSource += '    StackSize = 10000000\n'
      dataSource += '    TreeName = "'+self.getTree().name+'"\n'
      dataSource += '    PulseNumber = '+str(self.getTree().shot)+'\n'
      dataSource += '    StoreOnTrigger = 0\n'
      dataSource += '    TimeRefresh = 1\n'
      dataSource += '        EventName = "'+gamName+'UpdatejScope"\n'
      dataSource += '    Signals = {\n'
      dataSource += '    CycleTime = {\n'
      dataSource += '        NodeName = "'+getattr(self, 'times_state_%d_thread_%d_cycle'%(state+1, thread+1)).getFullPath()+'"\n'
      dataSource += '        Period = '+str(threadPeriod) + '\n'
      dataSource += '        MakeSegmentAfterNWrites = '+str(segLen)+'\n'
      dataSource += '        AutomaticSegmentation = 0\n'
      dataSource += '      }\n'

      sigIdx = 1
      for timeSignal in timeSignals:
        dataSource += '      '+timeSignal + ' = {\n'
        dataSource += '        NodeName = "'+getattr(self, 'times_state_%d_thread_%d_gam'%(state+1, thread+1)+str(sigIdx)).getFullPath()+'"\n'
        dataSource += '        Period = '+str(threadPeriod) + '\n'
        dataSource += '        MakeSegmentAfterNWrites = '+str(segLen)+'\n'
        dataSource += '        AutomaticSegmentation = 0\n'
        dataSource += '      }\n'
        sigIdx = sigIdx + 1
      dataSource += '    }\n'
      dataSource += '  }\n'
      dataSources.append(dataSource)



    def buildConfiguration(self):
      print('START BUILD')
      error, info, threadMap = self.getInfo()
      if error != '':
        return 0
      confText = '+MDS_EVENTS = {\n'
      confText += '  Class = MDSEventManager\n'
      confText += '  StackSize = 1048576\n'
      confText += '  CPUs = 0x1\n'
      confText += '  Name = '+info['name']+'\n'
      confText += '}\n'
      confText += '$'+info['name']+' = {\n'
      confText += ' Class = RealTimeApplication\n'
      confText += ' +Functions = {\n'
      confText += '  Class = ReferenceContainer\n'
      confText += '  +IDLE_MDSPLUS = {\n'
      confText += '    Class = IOGAM\n'
      confText += '    InputSignals = {\n'
      confText += '      Counter = {\n'
      confText += '        DataSource = IDLE_MDSPLUS_TIMER\n'
      confText += '        Type = uint32\n'
      confText += '        NumberOfElements = 1\n'
      confText += '      }\n'
      confText += '      Time = {\n'
      confText += '        DataSource = IDLE_MDSPLUS_TIMER\n'
      confText += '        Type = uint32\n'
      confText += '        NumberOfElements = 1\n'
      confText += '        Frequency = 10\n'
      confText += '      }\n'
      confText += '    }\n'
      confText += '    OutputSignals = {\n'
      confText += '      Counter = {\n'
      confText += '        DataSource = IDLE_MDSPLUS_DDB\n'
      confText += '        Type = uint32\n'
      confText += '      }\n'
      confText += '      Time = {\n'
      confText += '        DataSource = IDLE_MDSPLUS_DDB\n'
      confText += '        Type = uint32\n'
      confText += '        NumberOfElements = 1\n'
      confText += '      }\n'
      confText += '    }\n'
      confText += '  }\n'

      for gam in info['gams']:
        confText += gam
      confText += ' }\n'
      confText += ' +Data = {\n'
      confText += '  Class = ReferenceContainer\n'
      confText += ' +IDLE_MDSPLUS_TIMER = {\n'
      confText += '   Class = LinuxTimer\n'
      confText += '   SleepNature = "Default"\n'
      confText += '   Signals = {\n'
      confText += '     Counter = {\n'
      confText += '       Type = uint32\n'
      confText += '     }\n'
      confText += '     Time = {\n'
      confText += '       Type = uint32\n'
      confText += '     }\n'
      confText += '   }\n'
      confText += ' }\n'
      confText += ' +IDLE_MDSPLUS_DDB = {\n'
      confText += '   Class = GAMDataSource\n'
      confText += ' }\n'
      confText += '  +Timings = {\n'
      confText += '      Class = TimingDataSource\n'
      confText += '  }\n'

      for dataSource in info['data_sources']:
        confText += dataSource
      confText += '  }\n'

      confText += ' +States = {\n'
      confText += '  Class = ReferenceContainer\n'
      confText += '  +IDLE = {\n'
      confText += '    Class = RealTimeState\n'
      confText += '    +Threads = {\n'
      confText += '      Class = ReferenceContainer\n'
      confText += '        +Thread1 = {\n'
      confText += '          Class = RealTimeThread\n'
      confText += '          Functions = {IDLE_MDSPLUS}\n'
      confText += '        }\n'
      confText += '      }\n'
      confText += '    }\n'

      for state in info['states']:
        confText += '  +'+state['name'] + ' = {\n'
        confText += '  Class = RealTimeState\n'
        confText += '  +Threads = {\n'
        confText += '    Class = ReferenceContainer\n'
        for thread in state['threads']:
          confText += '    +'+thread['name']+' = {\n'
          confText += '      Class = RealTimeThread\n'
          if 'core' in thread:
            confText += '      CPUs = '+str(thread['core'])+'\n'
          functionStr = ''
          for gamName in thread['gams']:
           functionStr += gamName + ' '
          confText += '      Functions = {'+functionStr+'}\n'
          confText += '     }\n'
        confText += '   }\n'
        confText += '  }\n'
      confText += ' }\n'
      confText += ' +Scheduler = {\n'
      confText += '   Class = GAMScheduler\n'
      confText += '   TimingDataSource = Timings\n'
      confText += ' }\n'
      confText += '}\n'
      print (confText)
      f = open(info['name']+'_marte_configuration.cfg', 'w')
      f.write(confText)
      f.close()
      print('END BUILD')
      return 1


    def startMarteIdle(self):
       self.buildConfiguration()
       subprocess.Popen(['$MARTE_DIR/Playground.sh -f '+self.getNode('name').data()+'_marte_configuration.cfg -s IDLE'], shell=True)
       return 1

    def startMarte(self):
       self.buildConfiguration()
       subprocess.Popen(['$MARTE_DIR/Playground.sh -f '+self.getNode('name').data()+'_marte_configuration.cfg -s State1'], shell=True)
       return 1

    def doState(self, state):
      marteName = self.getNode('name').data()
      stateName = getattr(self, 'state_%d_name'%(state)).data()
      eventString1 = marteName+':StopCurrentStateExecution:XX'
      eventString2 = marteName+':'+'PrepareNextState:'+stateName
      eventString3 = marteName+':StartNextStateExecution:XX'
      Event.seteventRaw(marteName, np.frombuffer(eventString1, dtype = np.uint8))
      time.sleep(.1)
      Event.seteventRaw(marteName, np.frombuffer(eventString2, dtype = np.uint8))
      time.sleep(.1)
      Event.seteventRaw(marteName, np.frombuffer(eventString3, dtype = np.uint8))
      return 1

    def doState1(self):
      return self.doState(1)

    def doState2(self):
      return self.doState(2)

    def doState3(self):
      return self.doState(2)

    def doState4(self):
      return self.doState(2)

    def doState5(self):
      return self.doState(2)

    def suspendMarte(self):
      marteName = self.getNode('name').data()
      eventString1 = marteName+':StopCurrentStateExecution:XX'
      eventString2 = marteName+':'+'PrepareNextState:IDLE'
      eventString3 = marteName+':StartNextStateExecution:XX'
      Event.seteventRaw(marteName, np.frombuffer(eventString1, dtype = np.uint8))
      time.sleep(0.1)
      Event.seteventRaw(marteName, np.frombuffer(eventString2, dtype = np.uint8))
      time.sleep(0.1)
      Event.seteventRaw(marteName, np.frombuffer(eventString3, dtype = np.uint8))
      return 1

    def stopMarte(self):
      marteName = self.getNode('name').data()
      Event.seteventRaw(marteName, np.frombuffer(b'EXIT', dtype = np.uint8))
      return 1



    def check(self):
      t=self.getTree()
      numStates = self.num_states.data()
      gamInstances = []

      for state in range(numStates):
        numThreads = getattr(self, 'state_%d_num_threads'%(state+1)).data()
        for thread in range(numThreads):
          try:
            gamNids = self.getGamList(state, thread)
            for currGamNid in gamNids:  
              if currGamNid.isOn():
                gamClass = currGamNid.getData().getDevice()
                gamInstance = gamClass(currGamNid)
                gamInstances.append(gamInstance)
          except:
            return 'Cannot get Device list for tread '+str(thread)+' of state '+str(state)
      for gamInstance in gamInstances:
        try:

          gamInstance.prepareMarteInfo()
        except:
          return 'Device ' + gamInstance.getPath() + ' is not a MARTe2 device'

      error, info, threadMap = self.getInfo()
      if error != '':
        return error
      for gamInstance in gamInstances:
        status = gamInstance.check(threadMap)
        if status != '':
          return gamInstance.getPath()+': '+ status
      return 'Configuration OK'


#Check timebases
      for state in range(numStates):
        numThreads = getattr(self, 'state_%d_num_threads'%(state+1)).data()
        for thread in range(numThreads):
          timebaseGenerator = ''
          gamNodes = self.getGamList(state, thread)
          for currGamNid in gamNodes:
            if currGamNid.isOn():
              gamClass = currGamNid.getData().getDevice()
              gamInstance = gamClass(currGamNid)
              timebaseMode = gamInstance.checkTimebase(threadMap)
              if timebaseMode == MC.MARTE2_COMPONENT.TIMEBASE_GENERATOR:
                if timebaseGenerator == '':
                  timebaseGenerator = gamInstance.name.data()
                else:
                  return 'Multiple timebases in state %d, thread %d'%(state+1, thread+1)+': '+ timebaseGenerator + ', ' + gamInstance.name.data()
          if timebaseGenerator == '':
            return 'No Timebase defined in state %d, thread %d'%(state+1, thread+1)
      return 'Configuration OK'


