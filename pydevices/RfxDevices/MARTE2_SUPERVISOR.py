
import  MDSplus
import RfxDevices
import subprocess
import numpy as np
import time
import traceback
import os

MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_SUPERVISOR(MDSplus.Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [
        {'path': ':NAME', 'type': 'text'}, 
        {'path': ':COMMENT','type': 'text'}, 
        {'path': ':IP_ADDRESS','type': 'text', 'value':'localhost'}, 
        {'path': ':NUM_STATES', 'type': 'numeric', 'value': 1},
        {'path': ':INTERFACES', 'type': 'numeric'},
        {'path': ':SUPERVISORS', 'type': 'numeric'},
        ]

    MAX_STATES = 10
    MAX_THREADS = 10

    for stateIdx in range(MAX_STATES):
        parts.append({'path': '.STATE_'+str(stateIdx+1), 'type': 'structure'})
        parts.append(
            {'path': '.STATE_'+str(stateIdx+1)+':NAME', 'type': 'text'})
        parts.append({'path': '.STATE_'+str(stateIdx+1) +
                      ':NUM_THREADS', 'type': 'numeric', 'value': 1})
        for threadIdx in range(MAX_THREADS):
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1), 'type': 'structure'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':NAME', 'type': 'text'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':CORE', 'type': 'numeric'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':GAMS', 'type': 'numeric'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':TIMEBASE_MOD', 'type': 'text', 'value': 'EXTERNAL'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':TIMEBASE_DEF', 'type': 'numeric'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':TIMEBASE_DIV', 'type': 'numeric'})
            parts.append({'path': '.STATE_'+str(stateIdx+1) +
                          '.THREAD_'+str(threadIdx+1)+':TIME_PORT', 'type': 'numeric'})
    parts.append({'path': '.TIMES', 'type': 'structure'})
    for stateIdx in range(MAX_STATES):
        parts.append({'path': '.TIMES.STATE_' +
                      str(stateIdx+1), 'type': 'structure'})
        for threadIdx in range(MAX_THREADS):
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
    MODE_SYNC_INPUT = 3
    MODE_OUTPUT = 4
    MODE_INTERFACE = 5


   #Return the list (TreeNodes) of MARTe2 devices associated based on thread Node
    def getThreadGamNodes(self, threadNode):
        try:
            gams = threadNode.getNode('GAMS').getData()
        except:
            return []
        return self.convertGamNodes(gams)



    def convertGamNodes(self, gams):
        gamNodes = []
        if isinstance(gams, MDSplus.VECTOR):
            for i in range(gams.getNumDescs()):
                currGamNode = gams.getDescAt(i)
                gamNodes.append(currGamNode)
        else:
            for gam1 in gams.data():
                if isinstance(gam1, str):
                    gam = gam1
                else:
                    gam = str(gam1, 'utf_8')
                currGamNode = self.getTree().getNode(gam)
                gamNodes.append(currGamNode)
        #Check
        for currGamNode in gamNodes:
            if not isinstance(currGamNode, RfxDevices.MARTE2_COMPONENT):
                raise Exception('Declared node is not a MARTE2_COMPONENT: '+ currGamNode(getPath()))
            gamMode = currGamNode.getNode('MODE').data()
            if not (gamMode == MARTE2_SUPERVISOR.MODE_GAM or gamMode ==MARTE2_SUPERVISOR. MODE_INPUT 
                or gamMode == MARTE2_SUPERVISOR.MODE_SYNC_INPUT or gamMode == MARTE2_SUPERVISOR.MODE_OUTPUT):
                raise Exception('Declared MARTE2 device can only be GAM, Input or Output: '+ currGamNode.getPath())

        return gamNodes



    #Return the list (TreeNodes) of MARTe2 devices associated with a given thread based on threda and state idx
    def getGamNodes(self, state, thread):
        t = self.getTree()
        try:
            gams = getattr(self, 'state_%d_thread_%d_gams' %
                       (state+1, thread+1)).getData()
        except:
            return []
        return self.convertGamNodes(gams)
 
    #return the list (TreeNodes) of declared interfaces
    def getInterfaceNodes(self):
        t = self.getTree()
        try:
            interfaces = self.getNode('INTERFACES').getData()
        except:
            return []

        interfaceNodes = []
        if isinstance(interfaces, MDSplus.VECTOR):
            for i in range(interfaces.getNumDescs()):
                currInterface = interfaces.getDescAt(i)
                interfaceNodes.append(currInterface)
        else:
            for interf1 in interfaces.data():
                if isinstance(interf1, str):
                    interf = interf1
                else:
                    interf = str(interf1, 'utf_8')
                currInterface = t.getNode(interf)
                interfaceNodes.append(currInterface)
        #Check
        for currInterface in interfaceNodes:
            if not isinstance(currInterface, RfxDevices.MARTE2_COMPONENT):
                raise Exception('Declared node is not a MARTE2_COMPONENT: '+ currInterface(getPath()))
            gamMode = currInterface.getNode('MODE').data()
            if not (gamMode == MARTE2_SUPERVISOR.MODE_INTERFACE):
                raise Exception('Declared MARTE2 device can only be Interface: '+ currInterface.getPath())
        return interfaceNodes

   #return the list (TreeNodes) of associated MARTE2 supervisor
    def getSupervisorNodes(self):
        t = self.getTree()
        try:
            supervisors = self.getNode('SUPERVISORS').getData()
        except:
            return []

        supervisorNodes = []
        if isinstance(supervisors, MDSplus.VECTOR):
            for i in range(supervisors.getNumDescs()):
                currSupervisor = supervisors.getDescAt(i)
                supervisorNodes.append(currSupervisor)
        else:
            for superv1 in supervisors.data():
                if isinstance(superv1, str):
                    superv = superv1
                else:
                    superv = str(superv1, 'utf_8')
                currSupervisor = t.getNode(superv)
                supervisorNodes.append(currSupervisor)
        #Check
        for currSupervisor in supervisorNodes:
            if not isinstance(currSupervisor, MARTE2_SUPERVISOR):
                raise Exception('Declared node is not a MARTE2_SUPERVISOR: '+ currSupervisor(self.getPath()))
        return supervisorNodes

    #Return the target timebase reference dor DERIVED and EXT_DERIVED mode
    def getExtTimebaseRef(self, timebaseMode, stateIdx, threadIdx): 
        if timebaseMode == 'DERIVED':
            try:
                refTimebaseNode =  self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, threadIdx+1))
                refThreadIdx = refTimebaseNode.data()
                timebaseDef = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, refThreadIdx))
            except:
                raise Exception('Invalid derived Thread timebase for thread  in supervisor '+self.getPath())
            supervisorNode = self
        elif timebaseMode == 'EXT_DERIVED':
            supervisors = self.getSupervisorNodes()
            if supervisors == []:
               raise Exception('Supervisor list must be defined when timebase mode is EXD_DERIVED') 
            try:
                refTimebaseNode =  self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, threadIdx+1))
                refThreadInfo = refTimebaseNode.data()
                print('REF RTHREAD INFO')
                print(refThreadInfo)
                supervisorNode = supervisors[refThreadInfo[0]]
                refThreadIdx = refThreadInfo[1]
            except:
                raise Exception('Invalid timebase definition for EXT_DERIVED mode. It must be an array [<supervisorIdx>,<ThreadId>]')
        else:
            raise Exception('Invalid timebase mode  in supervisor '+self.getPath())
        try:
            timebaseDef = supervisorNode.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, refThreadIdx))
        except:
            raise Exception('Missing Thread timebase for thread in supervisor '+self.getPath())
        return timebaseDef

        #return threadMap dictionary
        # threadMap is a Dictionary with two keys:
        # 'DeviceInfo': defining for every MARTe2 device NID a dictionary:
        #       'ThreadName': name of the thread it belongs to
        #       'SupervisorNid': NID of the supervirsor hosting the device
        #       'SupervisorIp': IP address of the superisor hosting the device
        #       'DevicePort': Port number foir that device
        # 'ThreadInfo': defining for every thread for this supervisor a dictionary:
        #       'SyncThreadName': The name of the Synchronizing thread or None if this thread is not synchronized
        #       'SyncThreadSupervisor': The NID of the MARTE2 supervisor hosting the synchronizing thread or None if not synchronized
        #       'SyncDiv': Frequency division from synchronizing thread or None is not synchronized
    def getThreadMap(self, stateIdx):
        threadMap = {}
        threadInfo = {}
        deviceInfo = {}
        print('GET THERAD MAP')
        supervisors = self.getSupervisorNodes()
        print('SUPERVISORS')
        print(supervisors)
        supervisors.append(self)
        for supervisorNode in supervisors:
            print('CHECK')
            print(supervisorNode)
            try: 
                supervisorIp = supervisorNode.getNode('IP_ADDRESS').data()
            except:
                raise Exception('IP ADDRESS not defined foir supervisor '+ supervisorNode.getPath())
            
            threadNames = []
            try:
                numThreads = supervisorNode.getNode('STATE_'+str(stateIdx+1)+':NUM_THREADS')
            except:
                raise Exception("Missing NUM THREADS definition for supervisor "+supervisorNode.getPath()+'  STATE '+str(stateIdx+1))
            for threadIdx in range(numThreads):
                threadDevices = supervisorNode.getGamNodes(stateIdx, threadIdx)
                if len(threadDevices) == 0: #if no components defined for this thread
                    continue
                try:
                    threadName = self.getNode('STATE_%d.THREAD_%d:NAME' % (stateIdx+1, threadIdx+1)).data()
                except:
                    raise Exception('Missing NAME for thread '+str(threadIdx)+' in state '+str(stateIdx))
                if threadName in threadNames:
                    raise Exception('Duplicated thread name: '+threadName)
                threadNames.append(threadName)
                for currDevice in threadDevices:
                    if not isinstance(currDevice, RfxDevices.MARTE2_COMPONENT):
                        raise Exception('Only MARTE2 devices can be declared in e thread list for supervospr '+self.getPath())
                    try:
                        devicePort = currDevice.getNode('PORT').data()
                    except:
                        devicePort = -1
                    deviceNid = currDevice.getNid()
                    if deviceNid in deviceInfo:
                        raise Exception('Duplicated MARTE2 device definition: '+ currDevice.getPath())
                    supervInfo = {
                        'ThreadName': threadName,
                        'SupervisorNid': supervisorNode.getNid(),
                        'SupervisorIp': supervisorIp,
                        'DevicePort': devicePort
                    } 
                    deviceInfo[deviceNid] = supervInfo

        print('\nDEVICE INFO')
        print(deviceInfo)
        print('*****************************')
        threadMap['DeviceInfo'] = deviceInfo
            
        for threadIdx in range(numThreads):
            threadName = self.getNode('STATE_%d.THREAD_%d:NAME' % (stateIdx + 1, threadIdx + 1)).data()
            threadDevices = supervisorNode.getGamNodes(stateIdx, threadIdx)
            if len(threadDevices) == 0:
                continue
            try:
                timebaseMode = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_MOD' % (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception('Missing Thread timebase mode for thread '+threadName+ ' in supervisor '+self.getPath())
            if timebaseMode == 'INTERNAL':
                threadInfo[threadName] = {
                    'SyncThreadName': None,
                    'SyncThreadSupervisor': None,
                    'SyncDiv': None,
                }
            elif timebaseMode == 'EXTERNAL':
                    threadInfo[threadName] = {
                        'SyncThreadName': None,
                        'SyncThreadSupervisor': None,
                        'SyncDiv': None,
                    }
            else:
                timebaseDef = self.getExtTimebaseRef(timebaseMode, stateIdx, threadIdx)
                if not isinstance(timebaseDef, MDSplus.TreeNode):
                    raise Exception('Invalid thread reference for thread '+threadName+ ' in supervisor '+self.getPath())
                supervisorNode = timebaseDef.getParent().getParent().getParent()
                if not isinstance(supervisorNode, MARTE2_SUPERVISOR):
                    raise Exception('Invalid thread reference for thread '+threadName+' supervisor '+self.getPath()+' : '+timebaseDef.getPath())
                try:
                    syncThreadName = timebaseDef.getParent().getNode('NAME').data()
                except:
                    raise Exception('Missing Thread name in supervisor '+supervisorNode.getPath())
                syncThreadSupervisor = supervisorNode.getNid()
                try:
                    syncDiv = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DIV' %  (stateIdx+1, threadIdx+1)).getData()
                except:
                    raise Exception('Missing timebase div for thread '+ threadName+'  in supervisor '+supervisorNode.getPath())
                threadInfo[threadName] = {
                    'SyncThreadName': syncThreadName,
                    'SyncThreadSupervisor': syncThreadSupervisor,
                    'SyncDiv': syncDiv,
                }
        
        threadMap['ThreadInfo'] = threadInfo
        
        return threadMap

    #return the type of the synchronizing time
    def getSynchonizationTimeTypePeriod(self, timebaseDefNode):
        try:
            timebaseMode = timebaseDefNode.getParent().getNode('TIMEBASE_MOD').data()
        except:
            raise Exception('Missing Timebase Mode for supervisor '+self.getPath())
        if timebaseMode == 'INTERNAL':
            try:
                frequency = timebaseDefNode.getParent().getNode('TIMEBASE_DEF').data()
            except:
                raise Exception('Missing frequency spec for supervisor '+self.getPath())
            return  'uint32', 1./frequency
        if timebaseMode == 'EXTERNAL':
            gamNodes = self.getThreadGamNodes(timebaseDefNode.getParent())
            if len(gamNodes) == 0:
                raise Exception('Cannot get synchronizing device in external synchronization '+self.getPath())
            if gamNodes[0].getNode('MODE').data() !=  MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
               raise Exception('Cannot get synchronizing device in external synchronization '+self.getPath())
            try:
                retType =  gamNodes[0].getNode('OUTPUTS.TIME:TYPE').data()
            except:
                raise Exception('Cannot get time type in external synchronization '+self.getPath())
            try:
                period =  gamNodes[0].getNode('TIMEBASE').getData().getDescAt(2).data()
            except:
                raise Exception('Cannot get period external synchronization '+self.getPath())
            return retType, period

        if timebaseMode == 'EXTERNAL':
            try :
                timebaseRef = timebaseDefNode.getData()
            except:
                raise Exception('Cannot get referenced timebase in derived synchronization '+self.getPath())
            syncSupervisor = timebaseRef.getParent().getParent().getParent()
            if not isinstance(syncSupervisor, MARTE2_SUPERVISOR):
                raise Exception('Wrongly referenced timebase in derived synchronization '+self.getPath())

            return syncSupervisor.getSynchonizationTimeTypePeriod(timebaseRef)
        
        raise Exception('Invalid timebase mode '+ timebaseMode+ ' for ' + self.getPath())


    #return the list of GAMs and DataSurces required to handle sychronization for the specified thread
    #returns a Dictionary with fields 'TimerDDB', 'TimerType', 'TimerPeriod', 'DataSources', 'Gams' 
    def getSynchronizationInfo(self, stateIdx, threadIdx):
        retInfo = {}
        retDataSources = []
        retGams = []
        try:
            timebaseMode = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_MOD' % (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing Timebase Mode for supervisor '+self.getPath())
        try:
            threadName = self.getNode('STATE_%d.THREAD_%d:NAME' % (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing thread name for for supervisor '+self.getPath())

        if timebaseMode == 'INTERNAL':
            try:
                frequency = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception('Missing period definition  for Internal timebase mode in thread '+threadName+' supervisor '.self.getPath)
            retDataSources.append( {
                'Name': threadName+'_Timer',
                'Class': 'LinuxTimer',
                'Signals': [{'Name':'Counter', 'Type': 'uint32'}, {'Name':'Time', 'Type': 'uint32'}]
            })
            retDataSources.append( {
                'Name': threadName+'_TimerDDB',
                'Class': 'GAMDataSource'
            })
            retGams.append( {
                'Name': threadName+'_TimerIOGAM',
                'Class': 'IOGAM',
                'Inputs': [{
                    'Name': 'Counter',
                    'Type': 'uint32',
                    'DataSource': threadName+'_Timer',
                },
                {
                    'Name': 'Time',
                    'Type': 'uint32',
                    'DataSource': threadName+'_Timer',
                    'Frequency': frequency
                }],
                'Outputs': [{
                    'Name': 'Counter',
                    'Type': 'uint32',
                    'DataSource': threadName+'_TimerDDB'
                },
                {
                    'Name': 'Time',
                    'Type': 'uint32',
                    'DataSource': threadName+'_TimerDDB'
                }]
            })
            retInfo['TimerDDB'] =  threadName+'_TimerDDB'
            retInfo['TimerType'] = 'uint32'
            retInfo['TimerPeriod'] = 1./frequency
            retInfo['DataSources'] = retDataSources
            retInfo['Gams'] = retGams
            return retInfo
        if timebaseMode == 'EXTERNAL':
            syncDevices = self.getGamNodes(stateIdx, threadIdx)
            if len(syncDevices) == 0:
                raise Exception('Cannot retrieve the synchronizing device for thread '+threadName+' supervisor '+self.getPath()) 
            syncDevice = syncDevices[0]
            gamMode = syncDevice.getNode('MODE').data()    
            if gamMode != MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
                raise Exception('Only Synchronized Input can synchronize thread '+threadName+' supervisor '+self.getPath()) 
            #Check time definition
            try:
                timerType = syncDevice.getNode('OUTPUTS.TIME:TYPE').data()
            except:
                raise Exception('Cannot retrieve time type for thread '+threadName+' supervisor '+self.getPath())
            try:
                timebase = syncDevice.getNode('TIMEBASE').getData()
            except:
                raise Exception('Cannot retrieve timebase for thread '+threadName+' supervisor '+self.getPath())
            if not isinstance(timebase, MDSplus.Range):
                raise Exception('Timebase must be a range descriptor for thread '+threadName+' supervisor '+self.getPath())
            try:
                timerPeriod = timebase.getDescAt(2).data()
            except:
                raise Exception('Cannot retrieve period for thread '+threadName+' supervisor '+self.getPath())

            retInfo['TimerDDB'] =  syncDevice.getMarteDeviceName(syncDevice)+'_Output_DDB'
            retInfo['TimerType'] = timerType
            retInfo['TimerPeriod'] = timerPeriod 
            retInfo['DataSources'] = []
            retInfo['Gams'] = []
            return retInfo

        if timebaseMode == 'DERIVED' or timebaseMode == 'EXT_DERIVED':
            refTimebaseDef = self.getExtTimebaseRef(timebaseMode, stateIdx, threadIdx)
            if not isinstance(refTimebaseDef, MDSplus.TreeNode):
                raise Exception('Invalid timebase reference for thread '+threadName+' supervisor '+self.getPath())
            if not isinstance(refTimebaseDef.getParent().getParent().getParent(), MARTE2_SUPERVISOR):
                raise Exception('Invalid timebase reference for thread '+threadName+' supervisor '+self.getPath())
            refSupervisor = refTimebaseDef.getParent().getParent().getParent()
            try:
                syncDiv = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DIV' % (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception('Invalid timebase div for thread '+threadName+' supervisor '+self.getPath())

            try:
                refThreadName = refTimebaseDef.getParent().getNode('NAME').data()
            except:
                raise Exception('Cannot retrieve the name of the synchronizing thread r thread '+threadName+' supervisor '+self.getPath())
            print(refTimebaseDef)
            timerType, timerPeriod = self.getSynchonizationTimeTypePeriod(refTimebaseDef)
            if refSupervisor.getNid() == self.getNid(): #Thread synchronized by another thread of the same supervisor
                retDataSources.append({
                    'Name': threadName+'_TimerDDB',
                    'Class': 'GAMDataSource'
                })
                retGams.append({
                    'Name': threadName+'_TimerGAM',
                    'Class': 'PickSampleGAM',
                    'Inputs': [{
                        'Name': 'Time',
                        'Type': timerType,
                        'DataSource': refThreadName+'_TimerSync',
                        'Samples': syncDiv
                    }] ,
                    'Outputs': [{
                        'Name': 'Time',
                        'Type': timerType,
                        'DataSource':  threadName+'_TimerDDB',
                        'Samples': 1
                     }] 
                })
                retInfo['TimerDDB'] = threadName+'_TimerDDB'
                retInfo['TimerType'] = timerType
                retInfo['TimerPeriod'] = timerPeriod * syncDiv
                retInfo['DataSources'] = retDataSources
                retInfo['Gams'] = retGams
                return retInfo
            else: #Synchronized on a thread from another supervisor
                try: 
                    port = self.getNode('STATE_%d.THREAD_%d:TIME_PORT' % (stateIdx+1, threadIdx+1)).data()
                except:
                    raise Exception('Cannot get port number for derived timebase in '+self.getPath())
                
                retDataSources.append({
                    'Name': threadName + '_SYNC_IN',
                    'Class': 'RTNIn',
                    'Parameters': {
                        'CircuitId' : self.getNid(),
                        'Port': port,
                        'IsSynch': 1,
                    },
                    'Signals':[{
                        'Name': 'Time',
                        'Type': timerType,
                     }]
                })
                retDataSources.append({
                    'Name': threadName+'_TimerDDB',
                    'Class': 'GAMDataSource'
                })
                retGams.append({
                    'Name': threadName+'_TimerIOGAM',
                    'Class': 'PickSampleGAM',
                    'Inputs': [{
                        'Name': 'Time',
                        'Type': timerType,
                        'DataSource': threadName+'_SYNC_IN',
                        'Samples': syncDiv
                    }],
                    'Outputs': [{
                        'Name': 'Time',
                        'Type': timerType,
                        'DataSource':  threadName+'_TimerDDB',
                        'Samples': 1
                     }] 
                })
                retInfo['TimerDDB'] = threadName+'_TimerDDB'
                retInfo['TimerType'] = timerType
                retInfo['TimerPeriod'] = timerPeriod * syncDiv
                retInfo['DataSources'] = retDataSources
                retInfo['Gams'] = retGams
                return retInfo
        raise Exception('Invalid timebase mode for '+self.getpath())

        #check if this timebase def node is referenced by the timebaseRefNode of another thread of another supervisor
    def getReferencingSupervisorsInfo(self, timebaseDefNode, stateIdx):
        retIps = []
        retPorts = []
        retCircuitIds = []
        extSupervisors = self.getSupervisorNodes()
        for extSupervisor in extSupervisors:
            try:
                supervisorIp = extSupervisor.getNode('IP_ADDRESS').data()
            except:
                raise Exception('Missing IP Address for superisor '+ extSupervisor.gatPath())
            try:
                numThreads = extSupervisor.getNode('STATE_'+str(stateIdx+1)+':NUM_THREADS')
            except:
                raise Exception("Missing NUM THREADS definition for supervisor "+extSupervisor.getPath()+'  STATE '+str(stateIdx+1))

            for threadIdx in range(numThreads):
                if len(extSupervisor.getGamNodes(stateIdx, threadIdx)) > 0:
                    try:
                        extTimebaseMode = extSupervisor.getNode('STATE_%d.THREAD_%d:TIMEBASE_MOD' % (stateIdx+1, threadIdx+1)).data()
                    except:
                        raise Exception('Invalid timebase mode for supervisor '+ extSupervisor.getPath())
                    if extTimebaseMode == 'DERIVED' or extTimebaseMode == 'EXT_DERIVED':
                        extTimebaseRef = extSupervisor.getExtTimebaseRef(extTimebaseMode, stateIdx, threadIdx)
                        if isinstance(extTimebaseRef, MDSplus.TreeNode) and extTimebaseRef.getNid() == timebaseDefNode.getNid():
                            try:
                                threadPort = extSupervisor.getNode('STATE_%d.THREAD_%d:TIME_PORT' % (stateIdx+1, threadIdx+1)).data()
                            except:
                                raise Exception('Missing port for derived timebase mode in '+ extSupervisor.getPath())
                            retIps.append(supervisorIp)
                            retPorts.append(threadPort)
                            retCircuitIds.append(extSupervisor.getNid())
        return {
            'Ips': retIps,
            'Ports': retPorts,
            'CircuitIds': retCircuitIds
        }

    #Convert the passed lits into a MARTe array
    def toMarteArray(self, list):
        marteArr = '{'
        for l in list:
            marteArr += ' '+str(l)+' '
        marteArr += '}'
        return marteArr

 
    #Check if this thread is referenced by another thread of the same supervisor
    def isReferencedByAnotherThreadSameSupervisor(self, timebaseDefNode, stateIdx):
        try:
            numThreads = self.getNode('STATE_'+str(stateIdx+1)+':NUM_THREADS')
        except:
            raise Exception("Missing NUM THREADS definition for supervisor "+self.getPath()+'  STATE '+str(stateIdx+1))

        for threadIdx in range(numThreads):
            if len(self.getGamNodes(stateIdx, threadIdx)) > 0:
                try:
                    extTimebaseMode = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_MOD' % (stateIdx+1, threadIdx+1)).data()
                except:
                    raise Exception('Invalid timebase mode for supervisor '+ self.getPath())
                if extTimebaseMode == 'DERIVED' or extTimebaseMode == 'EXT_DERIVED':
                    extTimebaseRef = self.getExtTimebaseRef(extTimebaseMode, stateIdx, threadIdx)
                    if isinstance(extTimebaseRef, MDSplus.TreeNode) and extTimebaseRef.getNid() == timebaseDefNode.getNid():
                        return True
        return False

 

    #return DataSource and Gam Dict lists (possibly empty) to be added after synchornization has ben establisher
    #before any device of the thread if INTERNAL or DERIVED or after the first SyncInput device for EXTERNAL
    def getPostSynchronizationInfo(self, stateIdx, threadIdx, timerType, timerDDB):
        retGams = []
        retDataSources = []
        try:
            threadName = self.getNode('STATE_%d.THREAD_%d:NAME' % (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing thread name for for supervisor '+self.getPath())
        timebaseDefNode = self.getNode('STATE_%d.THREAD_%d:TIMEBASE_DEF' % (stateIdx+1, threadIdx+1))
        if self.isReferencedByAnotherThreadSameSupervisor(timebaseDefNode, stateIdx):
            retDataSources.append({
                'Name': threadName+'_TimerSync',
                'Class': 'RealTimeThreadSynchronisation',
            })
            retGams.append({
                'Name': threadName+'TimerSync_IOGAM',
                'Class': 'IOGAM',
                'Inputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': timerDDB
                }],
                'Outputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': threadName+'_TimerSync'
                }]
            })
        retInfo = self.getReferencingSupervisorsInfo(timebaseDefNode, stateIdx)
        if len(retInfo['Ips']) > 0:
            retDataSources.append({
                'Name': threadName+'RTN_OUT',
                'Class': 'RTNOut',
                'Signals':[{
                    'Name':'Time',
                    'Type': timerType,
                    'Ip' : self.toMarteArray(retInfo['Ips']),
                    'Port': self.toMarteArray(retInfo['Ports']) ,
                    'CircuitId': self.toMarteArray(retInfo['CircuitIds'])
                }]
            })
            retGams.append({
                'Name': threadName+'RTN_OUT_IOGAM',
                'Class': 'IOGAM',
                'Inputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': timerDDB
                }],
                'Outputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': threadName+'RTN_OUT'
                }]
            })
        return retDataSources, retGams

    #return the list of DataSources and GAMs and the types dict corresponding (in a dictionary) to the given thread in the given state        
    def getThreadInfo(self, threadMap, typesDict, stateIdx, threadIdx):
        deviceNodes = self.getGamNodes(stateIdx, threadIdx)
        if len(deviceNodes) == 0:
            return [],[]
        retSyncInfo = self.getSynchronizationInfo(stateIdx, threadIdx)
        dataSources = retSyncInfo['DataSources']
        gams = retSyncInfo['Gams']
        if deviceNodes[0].getNode('MODE').data ==  MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
            currDataSources, gurrGams = deviceNodes[0].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
            dataSources += (currDataSources)
            gams += currGams
            postSyncDataSources, postSyncGams = self.getPostSynchronizationInfo(stateIdx, threadIdx, 
                retSyncInfo['TimerType'], retSyncInfo['TimerDDB'])
            dataSources += currDataSources
            gams += currGams
            for deviceIdx in range(1, len(deviceNodes)):
                currDataSources, gurrGams = deviceNodes[deviceIdx].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                    retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
                dataSources += currDataSources
                gams += currGams
        else: #thread not synchronized by synch input device
            postSyncDataSources, postSyncGams = self.getPostSynchronizationInfo(stateIdx, threadIdx, 
                retSyncInfo['TimerType'], retSyncInfo['TimerDDB'])
            dataSources += postSyncDataSources
            gams += postSyncGams
            for deviceIdx in range(0, len(deviceNodes)):
                currDataSources, currGams = deviceNodes[deviceIdx].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                    retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
                dataSources += currDataSources
                gams += currGams

        #handle recording of timing information
        timingDataSource, timingGam = self.getTimingInfo(stateIdx, threadIdx, retSyncInfo['TimerPeriod'])
        if timingDataSource != None:
            dataSources.append(timingDataSource)
            gams.append(timingGam)


        return dataSources, gams

    #call prepareMarteInfo method for all involved MARTE2 devic
    def prepareStateInfo(self, stateIdx):
        try:
            numThreads = self.getNode('STATE_'+str(stateIdx+1)+':NUM_THREADS')
        except:
            raise Exception("Missing NUM THREADS definition for supervisor "+self.getPath()+'  STATE '+str(stateIdx+1))
        for threadIdx in range(numThreads):
            marte2Devices = self.getGamNodes(stateIdx, threadIdx)
            for marte2Device in marte2Devices:
                marte2Device.prepareMarteInfo()

    #return gams, DataSources and threadGams dict for a given state
    def getStateInfo(self, typesDict, stateIdx):
        threadMap = self.getThreadMap(stateIdx)
        dataSources = []
        gams = []
        threadGamsDict = {}
        try:
            numThreads = self.getNode('STATE_'+str(stateIdx+1)+':NUM_THREADS')
        except:
            raise Exception("Missing NUM THREADS definition for supervisor "+self.getPath()+'  STATE '+str(stateIdx+1))
        for threadIdx in range(numThreads):
            try:
                currThreadName = self.getNode('STATE_%d.THREAD_%d:NAME' % (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception("Missing name of thread "+str(threadIdx+1)+'  state: '+str(stateIdx+1))

            try:
                threadCpu = self.getNode('STATE_%d.THREAD_%d:CORE' % (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception("Missing CPU mask of thread "+str(threadIdx+1)+'  state: '+str(stateIdx+1))
            currDataSources, currGams = self.getThreadInfo(threadMap, typesDict, stateIdx, threadIdx)
            if len(currGams) == 0:
                continue
            gamNames = []
            for currGam in currGams:
                gamNames.append(currGam['Name'])
            gams += currGams
            dataSources += currDataSources
            threadGamsDict[currThreadName] = {'GamNames':gamNames, 'CpuMask': threadCpu}

        return {
            'DataSources': dataSources,
            'Gams': gams,
            'ThreadGamsDict': threadGamsDict
        }


    #return GAM and DataSource for handling the recording of execution times for this thread
    def getTimingInfo(self, stateIdx, threadIdx, threadPeriod):
        segLen = getattr(self, 'times_state_%d_thread_%d_seg_len' %
                         (stateIdx+1, threadIdx+1)).data()
        if(segLen == 0):
            return None, None
        
        stateName = getattr(self, 'state_%d_name' % (stateIdx+1)).data()
        threadName = getattr(self, 'state_%d_thread_%d_name' %
                             (stateIdx+1, threadIdx+1)).data()
        cpuMask = getattr(self, 'times_state_%d_thread_%d_cpu_mask' %
                          (stateIdx+1, threadIdx+1)).data()
        timeSignals = []
        gamNodes = self.getGamNodes(stateIdx, threadIdx)
        for gamNode in gamNodes:
            gamName = gamNode.getMarteDeviceName()
            gamMode = gamNode.getData('MODE').getDevice()
            if gamMode == MARTE2_SUPERVISOR.MODE_GAM:
                timeSignals.append(gamName+'_ReadTime')
                timeSignals.append(gamName+'_ExecTime')
            elif gamMode == MARTE2_SUPERVISOR.MODE_OUTPUT:
                timeSignals.append(gamName+'_IOGAM_WriteTime')
            else:
                timeSignals.append(gamName+'_DDBOutIOGAM_ReadTime')

        if len(timeSignals) == 0:
            return None, None
        
        retGam = {}
        retGam['Name'] = 'State_%d_Thread_%d_TIMES_IOGAM'%(stateIdx+1, threadIdx+1)
        retGam['Class'] = 'IOGAM'
        gamInputs = []
        gamInputs.append({
            'Name': stateName+'_'+threadName+'_CycleTime',
            'Alias': stateName+'.'+threadName+'_CycleTime',
            'DataSource': 'Timings',
            'Type': 'uint32'
        })
        for timeSignal in timeSignals:
            gamInputs.append({
                'Name': timeSignal,
                'DataSource': 'Timings',
                'Type': 'uint32'
            })
        retGam['Inputs'] = gamInputs
        gamOutputs = []
        gamOutputs.append({
            'Name': 'CycleTime',
            'DataSource': 'State_%d_Thread_%d_TIMES_WRITER\n' % (stateIdx+1, threadIdx+1),
            'Type': 'uint32'
        })
        for timeSignal in timeSignals:
            gamOutputs.append({
                'Name': timeSignal,
                'DataSource': 'State_%d_Thread_%d_TIMES_WRITER\n' % (stateIdx+1, threadIdx+1),
                'Type': 'uint32'
            })
        retGam['Outputs'] = gamOutputs

        retDataSource = {}
        retDataSource['Name'] = 'State_%d_Thread_%d_TIMES_WRITER' % (stateIdx+1, threadIdx+1)
        retDataSource['Class'] = 'MDSWriter'
        retDataSource['Parameters'] = {
            'CPUMask': cpuMask,
            'NumberOfBuffers' : 20000,
            'StackSize': 1000000,
            'TreeName': self.getTree().name,
            'PulseNumber': self.getTree().shot,
            'StoreOnTrigger': 0,
            'TimeRefresh': 5,
            'EventName': 'UpdateTimes',
        }
        retSignals = []
        retSignals.append({
            'Name': 'CycleTime',
            'Parameters': {
                'NodeName':  getattr(self, 'times_state_%d_thread_%d_cycle' % (stateIdx+1, threadIdx+1)).getFullPath(),
                'Period': threadPeriod,
                'MakeSegmentAfterNWrites': segLen,
                'AutomaticSegmentation' : 0,
                'DiscontinuityFactor': 10
            }
        })
        sigIdx = 1
        for timeSignal in timeSignals:
            retSignals.append({
                'Name': timeSignal,
                'NodeName':  getattr(self, 'times_state_%d_thread_%d_gam' % (stateIdx+1, threadIdx+1)+str(sigIdx)).getFullPath(),
                'Period': threadPeriod,
                'MakeSegmentAfterNWrites': segLen,
                'AutomaticSegmentation' : 0,
                'DiscontinuityFactor': 10
           })

        retDataSource['Signals'] = retSignals
        return retDataSource, retGam
    

    #return as a dict the underlying MARTe2 configuration. The fields ot the returned dict are:
    #'DataSources': list of all DataSources
    #'Gams': list of all GAMs
    #'TypeDict': Type dictionary
    #'States': for each declared state a dictionary specifying for each thread of that stets the list of associated GAMs
    #'Interfaces': list of declared interfaces
    def getMarte2ConfigInfo(self):
        typesDict = {}
        dataSources = []
        gams = []
        stateDict = {}
        try:
            numStates = self.getNode('NUM_STATES')
        except:
            raise Exception("Missing NUM STATES definition for supervisor "+self.getPath())
        for stateIdx in range(numStates):
            try:
                currStateName = self.getNode('STATE_%d:NAME' % (stateIdx + 1)).data()
            except:
                raise Exception('Missing name of state '+str(stateIdx+1))

            self.prepareStateInfo(stateIdx)
            currStateDict = self.getStateInfo(typesDict, stateIdx)
            dataSources += (currStateDict['DataSources'])
            gams += (currStateDict['Gams'])
            stateDict[currStateName] = currStateDict['ThreadGamsDict']

        interfaces = []
        interfaceNodes = self.getInterfaceNodes()
        for interfaceNode in interfaceNodes:
            interfaces.append(interfaceNode.generateMarteInterfaceConfiguration())

        return {
            'TypesDict': typesDict,
            'DataSources': dataSources,
            'Gams': gams,
            'Interfaces': interfaces,
            'StateDict': stateDict
        }

    def skipTabs(self, tabCount):
        tabs = ''
        for tabIdx in range(tabCount):
            tabs += '\t'
        return tabs

    def expandTypes(self, typesDict):
        if len(typesDict) == 0:
            return ''
        typeConf = '''
+Types = {
  Class = ReferenceContainer
'''
        for typeKey in typesDict:
            typeConf += '\t+' + typeKey + ' = {\n\t\tClass = IntrospectionStructure\n'
            for typeField in typesDict[typeKey]:
                typeConf += '\t\t'+typeField['Name']+ ' = {\n'
                typeConf += '\t\t\tType = '+typeField['Type']+'\n'
                typeConf += '\t\t\tNumberOfElements = '+str(typeField['NumberOfElements'])+'\n\t\t}\n'
            typeConf += '\t}\n'
        typeConf += '}\n'
        return typeConf

    def getParamType(self, value): #Converty MDSplus type to MARTe2 type
        if isinstance(value, int):
            return 'int32'
        if value.dtype == np.int8:
            return 'int8'
        if value.dtype == np.int16:
            return 'int16'
        if value.dtype == np.int32:
            return 'int32'
        if value.dtype == np.int64:
            return 'int64'
        if value.dtype == np.float32:
            return 'float32'
        if value.dtype == np.float64:
            return 'float64'

    def expandParameters(self, paramDict, tabCount):
        paramConf = ''
        for paramKey in paramDict:
            if isinstance(paramDict[paramKey], dict):  
                paramConf += self.skipTabs(tabCount)+paramKey+' = {\n'
                paramConf += self.expandParameters(paramDict[paramKey], tabCount+1)
                paramConf += self.skipTabs(tabCount)+'}\n'
            else:
                if isinstance(paramDict[paramKey], str):
                    paramConf += self.skipTabs(tabCount)+paramKey+' = \"'+str(paramDict[paramKey])+'\"\n'
                else:
                    currValue = str(paramDict[paramKey])
                    currValue = currValue.replace('[', '{')
                    currValue = currValue.replace(']', '}')
                    paramConf += self.skipTabs(tabCount)+paramKey+' = ('+self.getParamType(paramDict[paramKey])+')'+currValue+'\n'
        return paramConf

    def expandInterfaces(self, interfaces):
        interfaceConf = ''
        for interface in interfaces:
            interfaceConf += '+'+interface['Name']+' = {\n\tClass = '+interface['Class']+'\n' 
            interfaceConf += self.expandParameters(interface['Parameters'], 1)
            interfaceConf += '}\n'
        return interfaceConf

    def expandGams(self, gams):
        gamConf = ''
        for gam in gams:
            gamConf += '\t\t\t+'+gam['Name']+' = {\n'
            gamConf += '\t\t\t\tClass = '+gam['Class']+'\n'
            if 'Parameters' in gam:
                gamConf += self.expandParameters(gam['Parameters'], 4)
            if 'Inputs' in gam:
                gamConf += '\t\t\t\tInputSignals = {\n'
                for inSig in gam['Inputs']:
                    gamConf += '\t\t\t\t\t'+inSig['Name']+ ' = {\n'
                    for inSigKey in inSig:
                        if inSigKey == 'Name':
                            continue
                        if inSigKey == 'Parameters':
                            gamConf += self.expandParameters(inSig['Parameters'], 6)
                        else:
                            gamConf += '\t\t\t\t\t\t'+inSigKey+' = '+str(inSig[inSigKey])+'\n'
                    gamConf += '\t\t\t\t\t}\n'
                gamConf += '\t\t\t\t}\n'
            if 'Outputs' in gam:
                gamConf += '\t\t\t\tOutputSignals = {\n'
                for outSig in gam['Outputs']:
                    gamConf += '\t\t\t\t\t'+outSig['Name']+ ' = {\n'
                    for outSigKey in outSig:
                        if outSigKey == 'Name':
                            continue
                        if outSigKey == 'Parameters':
                            gamConf += self.expandParameters(outSig['Parameters'], 6)
                        else:
                            gamConf += '\t\t\t\t\t\t'+outSigKey+' = '+str(outSig[outSigKey])+'\n'
                    gamConf += '\t\t\t\t\t}\n'
                gamConf += '\t\t\t\t}\n'
            gamConf += '\t\t\t}\n'
        return gamConf

    def expandDataSources(self, dataSources):
        dsConf = ''
        for dataSource in dataSources:
            dsConf += '\t\t\t+'+dataSource['Name']+' = {\n'
            dsConf += '\t\t\t\tClass = '+dataSource['Class']+'\n'
            if 'Parameters' in dataSource:
                dsConf += self.expandParameters(dataSource['Parameters'], 4)
            if 'Signals' in dataSource and len(dataSource['Signals']) >  0:
                dsConf += '\t\t\t\tSignals = {\n'
                if 'Signals' in dataSource:
                    for inSig in dataSource['Signals']:
                        dsConf += '\t\t\t\t\t'+inSig['Name']+ ' = {\n'
                        for inSigKey in inSig:
                            if inSigKey == 'Name':
                                continue
                            if inSigKey == 'Parameters':
                                dsConf += self.expandParameters(inSig['Parameters'], 6)
                            else:
                                dsConf += '\t\t\t\t\t\t'+inSigKey+' = '+str(inSig[inSigKey])+'\n'
                        dsConf += '\t\t\t\t\t}\n'
                    dsConf += '\t\t\t\t}\n'
            dsConf += '\t\t\t}\n'
        return dsConf

    def expandStates(self, statesDict):
        stateConf = ''
        for stateKey in statesDict:
            stateConf += '\t\t\t+'+stateKey+' = {\n'
            stateConf += '\t\t\t\tClass = RealTimeState\n'
            stateConf += '\t\t\t\t+Threads = {\n'
            stateConf += '\t\t\t\t\tClass = ReferenceContainer\n'
            for threadKey in statesDict[stateKey]:
                stateConf += '\t\t\t\t\t+'+threadKey+' = {\n'
                stateConf += '\t\t\t\t\t\tClass = RealTimeThread\n'
                stateConf += '\t\t\t\t\t\tCPUs = '+str(statesDict[stateKey][threadKey]['CpuMask'])+'\n'
                stateConf += '\t\t\t\t\t\tFunctions = {'
                for gamName in statesDict[stateKey][threadKey]['GamNames']:
                    stateConf += ' '+gamName+' '
                stateConf += '}\n'
                stateConf += '\t\t\t\t\t}\n'
            stateConf += '\t\t\t\t}\n'
            stateConf += '\t\t\t}\n'
        return stateConf

    def generateConfiguration(self):
        try:
            appName = self.getNode('NAME').data()
        except:
            raise Exception('Missing application name for '+self.getPath())
        try:
            firstStateName = self.getNode('STATE_1:NAME').data()
        except:
            raise Exception('Missing first state name for '+self.getPath())
        outConfig = '''
<TYPE_LIST>
+MDS_EVENTS = {
    Class = MDSEventManager
    StackSize = 1048576
    CPUs = 0x1
    Name = <APP_NAME>
}
/* +WebRoot = {
    Class = HttpObjectBrowser
    Root = "."
    +ObjectBrowse = {
        Class = HttpObjectBrowser
        Root = "/"
    }
    +ResourcesHtml = {
        Class = HttpDirectoryResource
        BaseDir = "/opt/MARTe2/MARTe2/Resources/HTTP/"
    } 
}
+WebServer = {
    Class = HttpService
    Port = 8085
    WebRoot = WebRoot
    Timeout = 0
    ListenMaxConnections = 255
    AcceptTimeout = 1000
    MaxNumberOfThreads = 8
    MinNumberOfThreads = 1
} */
<INTERFACE_LIST>    
+StateMachine = {
    Class = StateMachine
    +INITIAL = {
        Class = ReferenceContainer    
        +START = {
            Class = StateMachineEvent
            NextState = "IDLE"
            NextStateError = "IDLE"
            Timeout = 0
            +StartHttpServer = {
                Class = Message
                Destination = "WebServer"
                Function = "Start"
            }            
            +ChangeToStateIdleMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = PrepareNextState
                +Parameters = {
                    Class = ConfigurationDatabase
                    param1 = Idle
                }
            }
            +StartNextStateExecutionMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = StartNextStateExecution
            }
        }
    }
    +IDLE = {
        Class = ReferenceContainer
        +GOTORUN = {
            Class = StateMachineEvent
            NextState = "RUN"
            NextStateError = "IDLE"
            Timeout = 0 
            +ChangeToRunMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = PrepareNextState
                +Parameters = {
                   Class = ConfigurationDatabase
                    param1 = <FIRST_STATE>
                }
            }
            +StopCurrentStateExecutionMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = StopCurrentStateExecution
            }
            +StartNextStateExecutionMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = StartNextStateExecution
            }
        }
    }
    +RUN = {
        Class = ReferenceContainer
        +GOTOIDLE = {
            Class = StateMachineEvent
             NextState = "IDLE"
            NextStateError = "IDLE"
            Timeout = 0         
            +ChangeToIdleMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = PrepareNextState
                +Parameters = {
                    Class = ConfigurationDatabase
                    param1 = Idle
                }
            }
            +StopCurrentStateExecutionMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = StopCurrentStateExecution
            }
            +StartNextStateExecutionMsg = {
                Class = Message
                Destination = <APP_NAME>
                Function = StartNextStateExecution
            }
        }   
    }
}   
$<APP_NAME> = {
    Class = RealTimeApplication
    +Functions = {
        Class = ReferenceContainer
        +IDLE_MDSPLUS = {
          Class = IOGAM
          InputSignals = {
            Counter = {
              DataSource = IDLE_MDSPLUS_TIMER
              Type = uint32
              NumberOfElements = 1
            }
            Time = {
              DataSource = IDLE_MDSPLUS_TIMER
              Type = uint32
              NumberOfElements = 1
              Frequency = 10
            }
          }
          OutputSignals = {
            Counter = {
              DataSource = IDLE_MDSPLUS_DDB
              Type = uint32
            }
            Time = {
              DataSource = IDLE_MDSPLUS_DDB
              Type = uint32
              NumberOfElements = 1
            }
          }
        }
<GAM_LIST>
    }
    +Data = {
      Class = ReferenceContainer
      +IDLE_MDSPLUS_TIMER = {
        Class = LinuxTimer
        Signals = {
          Counter = {
            Type = uint32
          }
          Time = {
            Type = uint32
          }
        }
      }
      +IDLE_MDSPLUS_DDB = {
        Class = GAMDataSource
      }
      +Timings = {
        Class = TimingDataSource
      }
<DATASOURCE_LIST>
    }
    +States = {
      Class = ReferenceContainer
      +Idle = {
        Class = RealTimeState
        +Threads = {
          Class = ReferenceContainer
          +Thread1 = {
            Class = RealTimeThread
            Functions = {IDLE_MDSPLUS}
          }
        }
      }
<STATE_LIST>
    }

    +Scheduler = {
        Class = GAMScheduler
        TimingDataSource = Timings
    }
}
'''    
        config = self.getMarte2ConfigInfo()  
        outConfig = outConfig.replace('<APP_NAME>', appName)
        outConfig = outConfig.replace('<TYPE_LIST>', self.expandTypes(config['TypesDict']))
        outConfig = outConfig.replace('<INTERFACE_LIST>', self.expandInterfaces(config['Interfaces']))
        outConfig = outConfig.replace('<GAM_LIST>', self.expandGams(config['Gams']))
        outConfig = outConfig.replace('<DATASOURCE_LIST>', self.expandDataSources(config['DataSources']))
        outConfig = outConfig.replace('<STATE_LIST>', self.expandStates(config['StateDict']))
        outConfig = outConfig.replace('<FIRST_STATE>', firstStateName)
    
        return outConfig
    def check(self):
        try:
           self.generateConfiguration() 
        except Exception as e:
            return str(e)
        return 'Configuration OK'


    def buildConfiguration(self):
        config = self.generateConfiguration()
        #print(config)
        name = self.getNode('NAME').data()
        f = open('/tmp/'+name+'_marte_configuration.cfg', 'w')
        f.write(config)
        f.close()


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
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))

    def gotoidle(self):
        marteName = self.getNode('name').data()
        eventString1 = 'StateMachine:GOTOIDLE'
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))

    def doState(self, state):
        marteName = self.getNode('name').data()
        stateName = getattr(self, 'state_%d_name' % (state)).data()
        eventString1 = marteName+':StopCurrentStateExecution:XX'
        eventString2 = marteName+':'+'PrepareNextState:'+stateName
        eventString3 = marteName+':StartNextStateExecution:XX'
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))
        time.sleep(.1)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString2.encode(), dtype=np.uint8))
        time.sleep(.1)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
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
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString1.encode(), dtype=np.uint8))
        time.sleep(0.1)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString2.encode(), dtype=np.uint8))
        time.sleep(0.1)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(
            eventString3.encode(), dtype=np.uint8))

    def stopMarte(self):
        marteName = self.getNode('name').data()
        self.suspendMarte()
        time.sleep(2)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(b'EXIT', dtype=np.uint8))
        time.sleep(2)
        MDSplus.Event.seteventRaw(marteName, np.frombuffer(b'EXIT', dtype=np.uint8))
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

