
from MDSplus import Device, Event, VECTOR, Uint8Array
import subprocess
import numpy as np
import time
import traceback
import os

MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_SUPERVISOR(Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [{'path': ':NAME', 'type': 'text'}, {'path': ':COMMENT',
                                                 'type': 'text'}, {'path': ':NUM_STATES', 'type': 'numeric'}]
    for stateIdx in range(10):
        parts.append({'path': '.STATE_'+str(stateIdx+1), 'type': 'structure'})
        parts.append(
            {'path': '.STATE_'+str(stateIdx+1)+':NAME', 'type': 'text'})
        parts.append({'path': '.STATE_'+str(stateIdx+1) +
                      ':NUM_THREADS', 'type': 'numeric'})
        for threadIdx in range(10):
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1), 'type': 'structure'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':NAME', 'type': 'text'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':CORE', 'type': 'numeric'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAMS', 'type': 'numeric'})
    parts.append({'path': '.TIMES', 'type': 'structure'})
    for stateIdx in range(10):
        parts.append({'path': '.TIMES.STATE_' +
                      str(stateIdx+1), 'type': 'structure'})
        for threadIdx in range(10):
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1), 'type': 'structure'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_' +
                          str(threadIdx+1)+':SEG_LEN', 'type': 'numeric', 'value': 0})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1)+'.THREAD_' +
                          str(threadIdx+1)+':CPU_MASK', 'type': 'numeric', 'value': 15})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':CYCLE', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM1', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM2', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM3', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM4', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM5', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM6', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM7', 'type': 'signal'})
            parts.append({'path': '.TIMES.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAM8', 'type': 'signal'})
    parts.append({'path': ':MARTE_CONFIG', 'type': 'numeric'})


    parts.append({'path': ':INIT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('MARTE_SERVER','INIT',50,None),Method(None,'startMarteIdle',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':GOTORUN', 'type': 'action',
                  'valueExpr': "Action(Dispatch('MARTE_SERVER','PON',20,None),Method(None,'gotorun',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP', 'type': 'action',
                  'valueExpr': "Action(Dispatch('MARTE_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stopMarte',head))",
                  'options': ('no_write_shot',)})

    MODE_GAM = 1
    MODE_INPUT = 2
    MODE_SYNCH_INPUT = 3
    MODE_OUTPUT = 4

    def getGamList(self, state, thread):
        t = self.getTree()
        gams = getattr(self, 'state_%d_thread_%d_gams' %
                       (state+1, thread+1)).getData()
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
                    gam = str(gam1, 'utf_8')
                currGamNid = t.getNode(gam)
                gamNids.append(currGamNid)
        print(gamNids)
        return gamNids

    def getInfo(self):
        try:
            error = ''
            info = {}
            t = self.getTree()
            numStates = self.num_states.data()
            statesInfo = []
            retData = []
            retGams = []
            threadMap = {}
            typeDicts = []

        # first iteration to get threadMap
            for state in range(numStates):
                numThreads = getattr(
                    self, 'state_%d_num_threads' % (state+1)).data()
                for thread in range(numThreads):
                    threadName = getattr(
                        self, 'state_%d_thread_%d_name' % (state+1, thread+1)).data()
                    try:
                        gamNodes = self.getGamList(state, thread)
                    except:
                        raise Exception(
                            'Cannot get GAM list for state: ' + str(state + 1) + ', thread: '+str(thread + 1))
                    for currGamNode in gamNodes:
                        nid = currGamNode.getNid()
                        if nid in threadMap:
                            threadMap[nid] += [threadName]
                        else:
                            threadMap[nid] = [threadName]

        # Second iteration, build the remaining
            for state in range(numStates):
                stateInfo = {}
                stateInfo['name'] = getattr(
                    self, 'state_%d_name' % (state+1)).data()
                numThreads = getattr(
                    self, 'state_%d_num_threads' % (state+1)).data()
                stateThreads = []
                for thread in range(numThreads):
                    threadInfo = {}
                    threadName = getattr(
                        self, 'state_%d_thread_%d_name' % (state+1, thread+1)).data()
                    try:
                        core = getattr(self, 'state_%d_thread_%d_core' %
                                       (state+1, thread+1)).data()
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
                                raise Exception(
                                    'Cannot instantiate device for node '+currGamNode.getFullPath())
                            gamList = []
                            if not (currGamNode.getNid() in gamNids):
                                # try:
                                gamInstance.prepareMarteInfo()
                                currPeriod = gamInstance.getMarteInfo(
                                    threadMap, retGams, retData, gamList, typeDicts)
                                # except:
                                # return 'Cannot get timebase for ' + gam, {},{}
                                gamNids.append(currGamNode.getNid())
                              #  if currPeriod > 0 and threadPeriod > 0:
                                if currPeriod > 0 and threadPeriod > 0 and currPeriod != threadPeriod:
                                    raise Exception('More than one component driving thread timing for state: '+str(
                                        state+1)+', thread: '+str(thread+1))
                                else:
                                    if currPeriod > 0:
                                        threadPeriod = currPeriod
                            else:
                                dummyGams = []
                                dummyData = []
                                gamInstance.getMarteInfo(
                                    threadMap, dummyGams, dummyData, gamList, typeDicts)
                            gamNames += gamList
# TIMINGS
                    if threadPeriod == 0:
                        raise Exception(
                            'No component driving thread timing for state: '+str(state+1)+', thread: '+str(thread+1))
                    gamList = []
                    self.getTimingInfo(
                        state, thread, threadPeriod, retGams, retData, gamList)
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
            return error, info, threadMap, typeDicts
        except Exception as inst:
            print(traceback.format_exc())
     #       return inst.args[0], None, None
            return str(inst), None, None, None


# Enrich GAMs and Data Sources with what is required to store timing information (IOGAM + TreeWriter) is seg_len > 0

    def getTimingInfo(self, state, thread, threadPeriod, retGams, dataSources, gamList):
        segLen = getattr(self, 'times_state_%d_thread_%d_seg_len' %
                         (state+1, thread+1)).data()
        if(segLen == 0):
            return
        stateName = getattr(self, 'state_%d_name' % (state+1)).data()
        threadName = getattr(self, 'state_%d_thread_%d_name' %
                             (state+1, thread+1)).data()
        cpuMask = getattr(self, 'times_state_%d_thread_%d_cpu_mask' %
                          (state+1, thread+1)).data()
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
        currGam = '+State_%d_Thread_%d_TIMES_IOGAM = {\n' % (state+1, thread+1)
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
        currGam += '      DataSource = State_%d_Thread_%d_TIMES_WRITER\n' % (
            state+1, thread+1)
        currGam += '      Type = uint32\n'
        currGam += '  }\n'
        for timeSignal in timeSignals:
            currGam += '    '+timeSignal+' = {\n'
            currGam += '      DataSource = State_%d_Thread_%d_TIMES_WRITER\n' % (
                state+1, thread+1)
            currGam += '      Type = uint32\n'
            currGam += '    }\n'
        currGam += '  }\n'
        currGam += '}\n'

        retGams.append(currGam)
        gamList.append('State_%d_Thread_%d_TIMES_IOGAM' % (state+1, thread+1))

        dataSource = '  +State_%d_Thread_%d_TIMES_WRITER = {\n' % (
            state+1, thread+1)
        dataSource += '    Class = MDSWriter\n'
        dataSource += '    NumberOfBuffers = 20000\n'
        dataSource += '    CPUMask = ' + str(cpuMask)+'\n'
        dataSource += '    StackSize = 10000000\n'
        dataSource += '    TreeName = "'+self.getTree().name+'"\n'
        dataSource += '    PulseNumber = '+str(self.getTree().shot)+'\n'
        dataSource += '    StoreOnTrigger = 0\n'
        dataSource += '    TimeRefresh = 1\n'
        dataSource += '        EventName = "'+gamName+'UpdatejScope"\n'
        dataSource += '    Signals = {\n'
        dataSource += '    CycleTime = {\n'
        dataSource += '        NodeName = "' + \
            getattr(self, 'times_state_%d_thread_%d_cycle' %
                    (state+1, thread+1)).getFullPath()+'"\n'
        dataSource += '        Period = '+str(threadPeriod) + '\n'
        dataSource += '        MakeSegmentAfterNWrites = '+str(segLen)+'\n'
        dataSource += '        AutomaticSegmentation = 0\n'
        dataSource += '      }\n'

        sigIdx = 1
        for timeSignal in timeSignals:
            dataSource += '      '+timeSignal + ' = {\n'
            dataSource += '        NodeName = "' + \
                getattr(self, 'times_state_%d_thread_%d_gam' %
                        (state+1, thread+1)+str(sigIdx)).getFullPath()+'"\n'
            dataSource += '        Period = '+str(threadPeriod) + '\n'
            dataSource += '        MakeSegmentAfterNWrites = '+str(segLen)+'\n'
            dataSource += '        AutomaticSegmentation = 0\n'
            dataSource += '      }\n'
            sigIdx = sigIdx + 1
        dataSource += '    }\n'
        dataSource += '  }\n'
        dataSources.append(dataSource)

    def declareTypes(self, typeDicts):
        if len(typeDicts) == 0:
            return ''
        typeDecl = '+Types = {\n'
        typeDecl += '  Class = ReferenceContainer\n'
        for typeDict in typeDicts:
            typeDecl += '  +'+typeDict['name'] + ' = {\n'
            typeDecl += '    Class = IntrospectionStructure\n'
            for fieldDict in typeDict['fields']:
                typeDecl += '    '+fieldDict['name'] + ' = {\n'
                typeDecl += '      Type = '+fieldDict['type']+'\n'
                dimensions = fieldDict['dimensions']
                if dimensions == 0:
                    numberOfElements = 1
                    numberOfDimensions = 0
                else:
                    numberOfDimensions = len(fieldDict['dimensions'])
                    numberOfElements = 1
                    for currDim in fieldDict['dimensions']:
                        numberOfElements *= currDim
                typeDecl += '      NumberOfDimensions = ' + \
                    str(numberOfDimensions)+'\n'
                typeDecl += '      NumberOfElements = ' + \
                    str(numberOfElements)+'\n'
                typeDecl += '    }\n'
            typeDecl += '  }\n'
        typeDecl += '}\n'
        return typeDecl

    def buildConfiguration(self):
        print('START BUILD')
        error, info, threadMap, typeDicts = self.getInfo()
        if error != '':
            return 0
        confText = self.declareTypes(typeDicts)
        confText += '+MDS_EVENTS = {\n'
        confText += '  Class = MDSEventManager\n'
        confText += '  StackSize = 1048576\n'
        confText += '  CPUs = 0x1\n'
        confText += '  Name = '+info['name']+'\n'
        confText += '}\n'
        confText += '+WebRoot = {\n'
        confText += '    Class = HttpObjectBrowser\n'
        confText += '    Root = "."\n'
        confText += '    +ObjectBrowse = {\n'
        confText += '        Class = HttpObjectBrowser\n'
        confText += '        Root = "/"\n'
        confText += '    }\n'
        confText += '    +ResourcesHtml = {\n'
        confText += '        Class = HttpDirectoryResource\n'
        confText += '        BaseDir = "/opt/MARTe2/MARTe2/Resources/HTTP/"\n'
        confText += '    } \n'
        confText += '}\n'
        confText += '+WebServer = {\n'
        confText += '    Class = HttpService\n'
        confText += '    Port = 8085\n'
        confText += '    WebRoot = WebRoot\n'
        confText += '    Timeout = 0\n'
        confText += '    ListenMaxConnections = 255\n'
        confText += '    AcceptTimeout = 1000\n'
        confText += '    MaxNumberOfThreads = 8\n'
        confText += '    MinNumberOfThreads = 1\n'
        confText += '}    \n'

        confText += ' +StateMachine = {\n'
        confText += '    Class = StateMachine\n'
        confText += '    +INITIAL = {\n'
        confText += '        Class = ReferenceContainer    \n'
        confText += '        +START = {\n'
        confText += '            Class = StateMachineEvent\n'
        confText += '            NextState = "IDLE"\n'
        confText += '            NextStateError = "IDLE"\n'
        confText += '            Timeout = 0\n'
        confText += '            +StartHttpServer = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = "WebServer"\n'
        confText += '                Function = "Start"\n'
        confText += '            }            \n'
        confText += '            +ChangeToStateIdleMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = PrepareNextState\n'
        confText += '                +Parameters = {\n'
        confText += '                    Class = ConfigurationDatabase\n'
        confText += '                    param1 = Idle\n'
        confText += '                }\n'
        confText += '            }\n'
        confText += '            +StartNextStateExecutionMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = StartNextStateExecution\n'
        confText += '            }\n'
        confText += '        }\n'
        confText += '    }\n'
        confText += '    +IDLE = {\n'
        confText += '        Class = ReferenceContainer\n'
        confText += '        +GOTORUN = {\n'
        confText += '            Class = StateMachineEvent\n'
        confText += '            NextState = "RUN"\n'
        confText += '            NextStateError = "IDLE"\n'
        confText += '            Timeout = 0 \n'
        confText += '            +ChangeToRunMsg = {\n'
        confText += '                Class = Message\n'
        confText += '               Destination = '+info['name']+'\n'
        confText += '                Function = PrepareNextState\n'
        confText += '                +Parameters = {\n'
        confText += '                   Class = ConfigurationDatabase\n'
        confText += '                    param1 = ' + \
            info['states'][0]['name']+'\n'
        confText += '                }\n'
        confText += '           }\n'
        confText += '            +StopCurrentStateExecutionMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '               Function = StopCurrentStateExecution\n'
        confText += '            }\n'
        confText += '            +StartNextStateExecutionMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = StartNextStateExecution\n'
        confText += '            }\n'
        confText += '        }\n'
        confText += '    }\n'
        confText += '    +RUN = {\n'
        confText += '        Class = ReferenceContainer\n'
        confText += '        +GOTOIDLE = {\n'
        confText += '           Class = StateMachineEvent\n'
        confText += '           NextState = "IDLE"\n'
        confText += '            NextStateError = "IDLE"\n'
        confText += '            Timeout = 0         \n'
        confText += '            +ChangeToIdleMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = PrepareNextState\n'
        confText += '                +Parameters = {\n'
        confText += '                    Class = ConfigurationDatabase\n'
        confText += '                    param1 = Idle\n'
        confText += '                }\n'
        confText += '           }\n'
        confText += '            +StopCurrentStateExecutionMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = StopCurrentStateExecution\n'
        confText += '            }\n'
        confText += '           +StartNextStateExecutionMsg = {\n'
        confText += '                Class = Message\n'
        confText += '                Destination = '+info['name']+'\n'
        confText += '                Function = StartNextStateExecution\n'
        confText += '            }\n'
        confText += '        }   \n'
        confText += '    }\n'
        confText += '}   \n'

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
        confText += '   SleepNature = "Busy"\n'
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
        confText += '  +Idle = {\n'
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
        try:
          os.system('mv /tmp/'+info['name']+'_marte_configuration.cfg '+'/tmp/'+info['name']+'_marte_configuration_OLD.cfg ')
        except:
          pass
        f = open('/tmp/'+info['name']+'_marte_configuration.cfg', 'w')
        self.marte_config.putData(Uint8Array(bytearray(confText.encode())))
        f.write(confText)
        f.close()
        print('END BUILD')

    def startMarteIdle(self):
        self.buildConfiguration()
        subprocess.Popen(['$MARTE_DIR/Playground.sh -f /tmp/'+self.getNode(
            'name').data()+'_marte_configuration.cfg -m StateMachine:START'], shell=True)

    def startMarte(self):
        self.buildConfiguration()
        stateName = self.state_1_name.data()
        subprocess.Popen(['$MARTE_DIR/Playground.sh -f /tmp/'+self.getNode(
            'name').data()+'_marte_configuration.cfg -m StateMachine:START '+stateName], shell=True)
        time.sleep(4)
        self.gotorun()

    def gotorun(self):
        marteName = self.getNode('name').data()
        eventString1 = 'StateMachine:GOTORUN'
        Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))

    def gotoidle(self):
        marteName = self.getNode('name').data()
        eventString1 = 'StateMachine:GOTOIDLE'
        Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))

    def doState(self, state):
        marteName = self.getNode('name').data()
        stateName = getattr(self, 'state_%d_name' % (state)).data()
        eventString1 = marteName+':StopCurrentStateExecution:XX'
        eventString2 = marteName+':'+'PrepareNextState:'+stateName
        eventString3 = marteName+':StartNextStateExecution:XX'
        Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))
        time.sleep(.1)
        Event.seteventRaw(marteName, np.frombuffer(
            eventString2.encode(), dtype=np.uint8))
        time.sleep(.1)
        Event.seteventRaw(marteName, np.frombuffer(
            eventString3.encode(), dtype=np.uint8))

    def doState1(self):
        self.doState(1)

    def doState2(self):
        self.doState(2)

    def doState3(self):
        self.doState(3)

    def doState4(self):
        self.doState(4)

    def doState5(self):
        self.doState(5)

    def suspendMarte(self):
        marteName = self.getNode('name').data()
        eventString1 = marteName+':StopCurrentStateExecution:XX'
        eventString2 = marteName+':'+'PrepareNextState:IDLE'
        eventString3 = marteName+':StartNextStateExecution:XX'
        Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))
        time.sleep(0.1)
        Event.seteventRaw(marteName, np.frombuffer(
            eventString2.encode(), dtype=np.uint8))
        time.sleep(0.1)
        Event.seteventRaw(marteName, np.frombuffer(
            eventString3.encode(), dtype=np.uint8))

    def stopMarte(self):
        marteName = self.getNode('name').data()
        self.suspendMarte()
        time.sleep(2)
        Event.seteventRaw(marteName, np.frombuffer(b'EXIT', dtype=np.uint8))
        time.sleep(2)
        Event.seteventRaw(marteName, np.frombuffer(b'EXIT', dtype=np.uint8))
        # KILL MARTe process
        import subprocess
        import os

        command = 'kill -KILL `ps -Af | grep %s_marte_configuration.cfg | grep MARTeApp.ex | grep -v grep | awk \'{print $2}\'`' % (marteName)
        os.system(command)
        return 1

        command = 'ps -Af | grep %s_marte_configuration.cfg | grep MARTeApp.ex | grep -v grep | awk \'{print $2}\'' % (
            marteName)
        pid, error = subprocess.Popen("{cmd}".format(
            cmd=command), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        if len(pid) == 0:
            if len(error) != 0:
                print('INFO : %s' % (error))
        else:
            for p in pid.split():
                os.kill(int(p), 9)
                print('MARTe Process PID : %s Killed\n' % (p))

    def check(self):
        t = self.getTree()
        numStates = self.num_states.data()
        gamInstances = []

        for state in range(numStates):
            numThreads = getattr(
                self, 'state_%d_num_threads' % (state+1)).data()
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

        error, info, threadMap, typeDicts = self.getInfo()
        if error != '':
            return error
        for gamInstance in gamInstances:
            status = gamInstance.check(threadMap)
            if status != '':
                return gamInstance.getPath()+': ' + status
        return 'Configuration OK'


# Check timebases
        for state in range(numStates):
            numThreads = getattr(
                self, 'state_%d_num_threads' % (state+1)).data()
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
                                return 'Multiple timebases in state %d, thread %d' % (state+1, thread+1)+': ' + timebaseGenerator + ', ' + gamInstance.name.data()
                if timebaseGenerator == '':
                    return 'No Timebase defined in state %d, thread %d' % (state+1, thread+1)
        return 'Configuration OK'
