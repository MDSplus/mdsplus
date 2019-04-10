
from MDSplus import *
import subprocess
import numpy as np
import time

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
	parts.append({'path':'.STATE_'+str(stateIdx+1)+'.THREAD_'+str(threadIdx+1)+':GAMS', 'type':'text'})
    parts.append({'path':':INIT','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PON',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PON',51,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PPC',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})


    def getInfo(self):
      info = {}
      t=self.getTree()
      numStates = self.num_states.data()
      statesInfo = []
      retData = []
      retGams = []
      gamNids = []
      threadMap = {}

      #first iteration to get threadMap
      for state in range(numStates):
        numThreads = getattr(self, 'state_%d_num_threads'%(state+1)).data()
        for thread in range(numThreads):
          gams = getattr(self, 'state_%d_thread_%d_gams'%(state+1, thread+1)).data()
          threadName = getattr(self, 'state_%d_thread_%d_name'%(state+1, thread+1)).data()
          for gam in gams:
            currGamNid = t.getNode(gam);
            nid = currGamNid.getNid()
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
          gams = getattr(self, 'state_%d_thread_%d_gams'%(state+1, thread+1)).data()
          threadName = getattr(self, 'state_%d_thread_%d_name'%(state+1, thread+1)).data()
          try:
	    core = getattr(self, 'state_%d_thread_%d_core'%(state+1, thread+1)).data()
	    threadInfo['core'] = core
	  except:
	    pass
          threadInfo['name'] = threadName
          gamNames = []
          for gam in gams:
            currGamNid = t.getNode(gam);
            nid = currGamNid.getNid()
            if currGamNid.isOn():
 #             try:
                gamClass = currGamNid.getData().getDevice()
                gamInstance = gamClass(currGamNid)
                gamList = []
                if not (currGamNid.getNid() in gamNids):
                  gamInstance.getMarteInfo(threadMap, retGams, retData, gamList)
                  gamNids.append(currGamNid.getNid())
                else:
                  dummyGams = []
                  dummyData = []
                  gamInstance.getMarteInfo(threadMap, dummyGams, dummyData, gamList)
                gamNames += gamList
#              except:
#	        print('Cannot Get GAM/DataSource information from '+str(gamClass))

	  threadInfo['gams'] = gamNames
	  stateThreads.append(threadInfo)
        stateInfo['threads'] = stateThreads
        statesInfo.append(stateInfo)
      info['states'] = statesInfo

      info['gams'] = retGams
      info['data_sources'] = retData
      info['name'] = self.getNode('name').data()

      print('#######################')
      print(info)
      print('@@@@@@@@@@@@@@@@@@@@@@@')
      print(threadMap)
      return info, threadMap






    def buildConfiguration(self):
      info, threadMap = self.getInfo()
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
      print confText
      f = open(info['name']+'_marte_configuration.cfg', 'w')
      f.write(confText)
      f.close()
      return 1


    def startMarte(self):
       self.buildConfiguration()
       subprocess.Popen(['$MARTE_DIR/Playground.sh -f '+self.getNode('name').data()+'_marte_configuration.cfg -s IDLE'], shell=True)
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
      print(eventString1)
      print(eventString2)
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

