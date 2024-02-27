
import  MDSplus
import RfxDevices
import subprocess
import numpy as np
import time
import traceback
import os

MC = __import__('MARTE2_COMPONENT', globals())


class MARTE2_SUPERVISOR(Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [
        {'path': ':NAME', 'type': 'text'}, 
        {'path': ':COMMENT','type': 'text'}, 
        {'path': ':IP_ADDRESS','type': 'text'}, 
        {'path': ':NUM_STATES', 'type': 'numeric'},
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
                      ':NUM_THREADS', 'type': 'numeric'})
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
                          '.THREAD_'+str(threadIdx+1)+':TIMEBASE_MOD', 'type': 'string', 'value': 'EXTERNAL'})
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
        t = self.getTree()
        try:
            gams = threadNode.getNode('GAMS').getData()
        except:
            return []

        return self.convertGamNodes()

    def convertGamNodes(self, gams):
        gamNodes = []
        if isinstance(gams, VECTOR):
            for i in range(gams.getNumDescs()):
                currGamNode = gams.getDescAt(i)
                gamNodes.append(currGamNode)
        else:
            for gam1 in gams.data():
                if isinstance(gam1, str):
                    gam = gam1
                else:
                    gam = str(gam1, 'utf_8')
                currGamNode = t.getNode(gam)
                gamNids.append(currGamNode)
        #Check
        for currGamNode in gamNodes:
            if not isinstance(currGamNode, RfxDevices.MARTE2_COMPONENT):
                raise Exception('Declared node is not a MARTE2_COMPONENT: '+ currGamNode(getPath()))
            gamMode = currGamNode.getPath('MODE').data()
            if not (gamMode == MODE_GAM or gamMode == MODE_INPUT or gamMode == MODE_SYNC_INPUT or gamMode == MODE_OUTPUT):
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
        if isinstance(interfaces, VECTOR):
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
            gamMode = currInterface.getPath('MODE').data()
            if not (gamMode == MODE_INTERFACE):
                raise Exception('Declared MARTE2 device can only be Interface: '+ currInterface.getPath())
        return interfaceNodes

   #return the list (TreeNodes) of associated MARTE2 supervisor
     def getSupervisorNodes(self):
        t = self.getTree()
        try:
            supervisors = self.getNode('SUPERVISOR').getData()
        except:
            return []

        supervisorNodes = []
        if isinstance(supervisors, VECTOR):
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
            if not isinstance(currSupervisor, RfxDevices.MARTE2_SUPERVISOR):
                raise Exception('Declared node is not a MARTE2_SUPERVISOR: '+ currsupervisor(getPath()))
        return supervisorNodes

 

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
        supervisors = self.getSupervisors()
        supervisors.append(self)
        for supervisorNode in supervisors:
            try: 
                supervisorIp = supervisorNode.getNode('IP_ADDRESS').data()
            except:
                raise Exception('IP ADDRESS not defined foir supervisor '+ supervisorNode.getPath())
            
            threadNames = []
            for threadIdx in range(MARTE2_SUPERVISOR.MAX_THREADS):
                threadDevices = supervisorNode.getGamNodes(stateIdx, threadIdx)
                if len(threadDevices) == 0: #if no components defined for this thread
                    continue
                try:
                    threadName = self.getNode('STATE_%d_THREAD_%d_NAME').data()
                except:
                    raise Exception('Missing NAME for thread '+str(threadIdx)+' in state '+str(stateIdx))
                if threadName in ThreadNames:
                    raise Excpetion('Duplicated thread name: '+threadName)
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

        threadMap['DeviceInfo'] = deviceInfo
            
        for threadIdx in range(MARTE2_SUPERVFSOR.MAX_THREADS):
            threadName = self.getNode('STATE_%d_THREAD_%d_NAME').data()
            threadDevices = supervisorNode.getGamNodes(stateIdx, threadIdx)
            if len(threadDevices) == 0:
                continue
            try:
                timebaseDef = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DEF', (stateIdx+1, threadIdx+1)).getData()
            except:
                raise Exception('Missing Thread timebase for thread '+threadName+ ' in supervisor '+self.getPath())
            if isinstance(timebaseDef, MDSplus.Range):
                threadInfo[threadName] = {
                    'SyncThreadName': None,
                    'SyncThreadSupervisor': None,
                    'SyncDiv': None,
                }
            elif isinstance(timebaseDef, MDSplus.TreeNode):
                if(timebaseDef.getNid() == threadDevices[0].getNid()): #Thread synchronized by SyncInput
                    if timebaseDef.getNode('MODE').data() != MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
                        raise Exception('Only SynchronizedInput devices can synchronize a thread in supervisor '+self.getPath())
                    threadInfo[threadName] = {
                        'SyncThreadName': None,
                        'SyncThreadSupervisor': None,
                        'SyncDiv': None,
                    }
                else:
                    supervisorNode = timebasebaseDef.getParent().getParent().getParent()
                    if not isinstance(supervisorNode, RfxDevices.MARTE2_SUPERVISOR):
                        throw Exception('Wrong timebase definition for thread '+threadName+' supervisor '+sef.getPath())
                    try:
                        syncThreadName = timebaseDef.getParent().getNode('NAME').data()
                    except:
                        raise Exception('Missing Thread name in supervisor '+supervisorNode.getPath())
                    syncThreadSupervisor = supervisorNode.getNid()
                    try:
                        syncDiv = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DIV', (stateIdx+1, threadIdx+1)).getData()
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
            if gamNodes[0].getNode('MODE') !=  MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
               raise Exception('Cannot get synchronizing device in external synchronization '+self.getPath())
            try:
                retType =  gamNodes[0].getNode('OUTPUTS.TIME:TYPE').data()
            except:
                raise Exception('Cannot get time type in external synchronization '+self.getPath())
            try:
                period =  gamNodes[0].getNode('TIMEBASE').getDescAt(2).data()
            except:
                raise Exception('Cannot get period external synchronization '+self.getPath())
            return retType, period

        if timebaseMode == 'EXTERNAL':
            try :
                timebaseRef = timebaseDefNode.getData()
            except:
                raise Exception('Cannot get referenced timebase in derived synchronization '+self.getPath())
            syncSupervisor = timebaseRef.getParent().getParent().getParent()
            if not isinstance(syncSupervisor, RfxDevices.MARTE2_SUPERVISOR):
                raise Exception('Wrongly referenced timebase in derived synchronization '+self.getPath())

            return syncSupervisor.getSynchonizationTimeTypePeriod(timebaseRef)
        
        raise Exception('Invalid timebase mode '+ timebaseMode+ ' for 'self.getPath())


    #return the list of GAMs and DataSurces required to handle sychronization for the specified thread
    #returns a Dictionary with fields 'TimerDDB', 'TimerType', 'TimerPeriod', 'DataSources', 'Gams' 
    def getSynchronizationInfo(self, stateIdx, threadIdx):
        retInfo = {}
        retDataSources = []
        retGams = []
        try:
            timebaseMode = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_MOD', (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing Timebase Mode for supervisor '+self.getPath())
        try:
            threadName = self.getNode('STATE_%d_THREAD_%d_NAME', (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing thread name for for supervisor '+self.getPath())

        if timebaseMode == 'INTERNAL':
            try:
                frequency = self.getNode('TIMEBASE_DEF').data()
            except:
                raise Exception('Missing period definition  for Internal timebase mode in thread '+threadName+' supervisor '.self.getPath)
            retDataSources.append( {
                'Name': threadName+'_'+str(self.getNid())+'_Timer',
                'Class': 'LinuxTimer',
                'Signals': [{'Name':'Counter', 'Type': 'uint32'}, {'Name':'Time', 'Type': 'uint32'}]
            })
            retDataSources.append( {
                'Name': threadName+'_'+str(self.getNid())+'_TimerDDB',
                'Class': 'GAMDataSource'
            })
            retGams.append( {
                'Name': threadName+'_'+str(self.getNid())+'_TimerIOGAM'
                'Inputs': [{
                    'Name': 'Counter',
                    'Type': 'uint32',
                    'DataSource': threadName+'_Timer'
                },
                {
                    'Name': 'Time',
                    'Type': 'uint32',
                    'DataSource': threadName+'_Timer'
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
            syncDevices = self.getGamNodes(stateIdx, threadIdx)[0]
            if len(syncDevices) == 0:
                raise Exception('Cannot retrieve the synchronizing device for thread '+threadName+' supervisor '+self.getPath()) 
            syncDevice = syncDevices[0]
            gamMode = syncDevice.getPath('MODE').data()    
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

        if timebaseMode == 'DERIVED':
            try:
                refTimebaseNode =  self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DEF', (stateIdx+1, threadIdx+1))
                refTimebaseDef = refTimebaseNode.getData()
            except:
                raise Exception('Cannot retrieve thread timebase for thread '+threadName+' supervisor '+self.getPath())
            if not isinstance(refTimebaseDef, MDSplus.TreeNode):
                raise Exception('Invalid timebase reference for thread '+threadName+' supervisor '+self.getPath())
            if not isinstance(refTimebaseDef.getParent().getParent().getParent(), RfxDevices.MARTE2_SUPERVISOR):
                raise Exception('Invalid timebase reference for thread '+threadName+' supervisor '+self.getPath())
            refSupervisor = refTimebaseDef.getParent().getParent().getParent()
            try:
                syncDiv = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DIV', (stateIdx+1, threadIdx+1))
            except:
                raise Exception('Invalid timebase div for thread '+threadName+' supervisor '+self.getPath())

            try:
                refThreadName = refTimebaseDef.getParent().getNode('NAME').data()
            except:
                raise Exception('Cannot retrieve the name of the synchronizing thread r thread '+threadName+' supervisor '+self.getPath())
            timerType, timerPeriod = self.getSynchonizationTimeTypePeriod(timebaseDefNode)
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
                        'DataSource': refThreadName+'_TimerSync'
                        'Samples': syncDiv
                    }] 
                    'Outputs': [{
                        'Name': 'Time',
                        'Type': timerType
                        'DataSource':  threadName+'_TimerDDB'
                     }] 
                })
                retInfo['TimerBBD'] = threadName+'_TimerDDB'
                retInfo['TimerType'] = timerType
                retInfo['TimerPeriod'] = timerPeriod / syncDiv
                retInfo['DataSources'] = retDataSources
                retInfo['Gams'] = retGams
                retrun retInfo
            else: #Synchronized on a thread from another supervisor
                try: 
                    port = timebaseDefNode.getParent().getNode('TIME_PORT').data()
                except:
                    raise Exception('Cannot get port number for derived timebase in '+self.getPath())
                
                retDataSources.append({
                    'Name': threadName + '_SYNC_IN',
                    'Class': 'RTNIn',
                    'Id' : refTimebaseNode.getNid(),
                    'Port': port,
                    'IsSync': 'yes'
                    'Signals':[{
                        'Name': 'Time',
                        'Type': timerType
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
                        'DataSource': threadName+'_SYNC_IN'
                        'Samples': syncDiv
                    }] 
                    'Outputs': [{
                        'Name': 'Time',
                        'Type': timerType
                        'DataSource':  threadName+'_TimerDDB'
                     }] 
                })
                retInfo['TimerBBD'] = threadName+'_TimerDDB'
                retInfo['TimerType'] = timerType
                retInfo['TimerPeriod'] = timerPeriod / syncDiv
                retInfo['DataSources'] = retDataSources
                retInfo['Gams'] = retGams
                return retInfo
        raise Exception('Invalid timebase mode for '+self.getpath())

        #check if this timebase def node is referenced by the timebaseRefNode of another thread of another supervisor
    def getReferencingSupervisorsInfo(self, timebaseDefNode, stateIdx):
        retIps = []
        retPorts = []
        extSupervisors = self.getSupervisorNodes()
        for extSupervisor in extSupervisors:
            try:
                supervisorIp = extSupervisor.getNode('IP_ADDRESS').data()
            except:
                raise Exception('Missing IP Address for superisor '+ extSupervisor.gatPath())
            for threadIdx in range(MARTE2_SUPERVISOR.MAX_THREADS):
                if len(extSupervisor.getGamNodes(stateIdx, threadIdx)) > 0:
                    try:
                        extTimebaseMode = extSupervisor.getNode('STATE_%d_THREAD_%d_TIMEBASE_MOD', (stateIdx+1, threadIdx+1)).data()
                    except:
                        raise Exception('Invalid timebase mode for supervisor '+ extSupervisor.getPath())
                    if extTimebaseMode == 'DERIVED':
                        try:
                            extTimebaseRef =  extSupervisor.getNode('STATE_%d_THREAD_%d_TIMEBASE_DEF', (stateIdx+1, threadIdx+1)).getData()
                        except:
                            raise Exception('Missing external reference for drived timebase mode in '+ extSupervisor.getPath())
                        if isinstance(extTimebaseRef, MDSplus.TreeNode) and extTimebaseRef.getNid() == timebaseDefNode.getNid():
                            try:
                                threadPort = extSupervisor.getNode('STATE_%d_THREAD_%d_TIME_PORT', (stateIdx+1, threadIdx+1)).data()
                            except:
                                raise Exception('Missing port for drived timebase mode in '+ extSupervisor.getPath())
                            retIps.append(supervisorIp)
                            retPorts.append(threadPort)
        return retIps, retPorts

    #Convert the passed lits into a MARTe array
    def toMarteArray(list):
        marteArr = '{'
        for l in list:
            marteArr += ' '+str(l)+' '
        marteArr += '}'
        return marteArr

    #Check if this thread is referenced by another thread of the same supervisor
    def isReferencedByAnotherThreadSameSupervisor(self, timebaseDefNode, stateIdx):
        for threadIdx in range(MARTE2_SUPERVISOR.MAX_THREADS):
            if len(self.getGamNodes(stateIdx, threadIdx)) > 0:
                try:
                    extTimebaseMode = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_MOD', (stateIdx+1, threadIdx+1)).data()
                except:
                    raise Exception('Invalid timebase mode for supervisor '+ self.getPath())
                if extTimebaseMode == 'DERIVED':
                    try:
                        extTimebaseRef =  self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DEF', (stateIdx+1, threadIdx+1)).getData()
                    except:
                        raise Exception('Missing external reference for drived timebase mode in '+self.getPath())
                    if isinstance(extTimebaseRef, MDSplus.TreeNode) and extTimebaseRef.getNid() == timebaseDefNode.getNid():
                        return True
        return False

 

    #return DataSource and Gam Dict lists (possibly empty) to be added after synchornization has ben establisher
    #before any device of the thread if INTERNAL or DERIVED or after the first SyncInput device for EXTERNAL
    def getPostSynchronizationInfo(self, stateIdx, threadIdx, timerType, TimerDDB):
        retGams = []
        retDataSources = []
        try:
            threadName = self.getNode('STATE_%d_THREAD_%d_NAME', (stateIdx+1, threadIdx+1)).data()
        except:
            raise Exception('Missing thread name for for supervisor '+self.getPath())
        timebaseDefNode = self.getNode('STATE_%d_THREAD_%d_TIMEBASE_DEF', (stateIdx+1, threadIdx+1)).getData()
        if self.isReferencedByAnotherThreadSameSupervisor(timebaseDefNode, stateIdx):
            retDataSources.append({
                'Name': threadName+'_TimerSync',
                'Class': 'RealTimeThreadSynchronization',
            })
            retGams.append({
                'Name': threadName+'TimerSync_IOGAM',
                'Class': 'IOGAM',
                'Inputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': timerDDB
                }]
                'Outputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': threadName+'_TimerSync'
                }]
            })
        ips, ports = self.getReferencingSupervisorsInfo(timebaseDefNode, stateIdx):
        if len(ips) > 0:
            retDataSources.append({
                'Name': threadName+'_RTN_OUT',
                'Class': 'RTNOut',
                'Signals':[{
                    'Name':'Time',
                    'Type': timerType,
                    'Ips' : self.toMarteArray(ips)
                    'Ports': self.toMarteArray(ports) 
                    'Id': timebaseDefNode.getNid()
                }]
            })
            retGams.append({
                'Name': threadName+'RTN_OUT_IOGAM',
                'Class': 'IOGAM',
                'Inputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': timerDDB
                }]
                'Outputs': [{
                    'Name': 'Time',
                    'Type': timerType,
                    'DataSource': threadName+'RTN_OUT'
                }]
            })
        return retDataSources, retGams

    #return the list of DataSources and GAMs and the types dict corresponding (in a dictionary) to the given thread in the given state        
    def getThreadInfo(self, threadMap, typesDict, stateIdx, threadIdx):
        deviceNodes = getGamNodes(stateIdx, threadIdx)
        if len(deviceNodes) == 0:
            return [],[]
        retSyncInfo = self.getSynchronizationInfo(stateIdx, threadIdx)
        dataSources = retSyncInfo['DataSources']
        gams = retSyncInfo['Gams']
        if gams[0].getNode('MODE').data ==  MARTE2_SUPERVISOR.MODE_SYNC_INPUT:
            currDataSources, gurrGams = deviceNodes[0].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
            dataSources.append(currDataSources)
            gams.append(currGams)
            postSyncDataSources, postSyncGams = self.getPostSynchronizationInfo(stateIdx, threadIdx, 
                retSyncInfo['TimerType'], retSyncInfo['TimerDDB']):
            dataSources.append(currDataSources)
            gams.append(currGams)
            for deviceIdx in range(1, len(deviceNodes)):
                currDataSources, gurrGams = deviceNodes[deviceIdx].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                    retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
                dataSources.append(currDataSources)
                gams.append(currGams)
        else: #thread not synchronized by synch input device
            postSyncDataSources, postSyncGams = self.getPostSynchronizationInfo(stateIdx, threadIdx, 
                retSyncInfo['TimerType'], retSyncInfo['TimerDDB']):
            dataSources.append(currDataSources)
            gams.append(currGams)
            for deviceIdx in range(0, len(deviceNodes)):
                currDataSources, gurrGams = deviceNodes[deviceIdx].generateMarteConfiguration(threadMap, retSyncInfo['TimerDDB'], 
                    retSyncInfo['TimerType'], retSyncInfo['TimerPeriod'], typesDict)  
                dataSources.append(currDataSources)
                gams.append(currGams)

        return dataSources, gams

    def getStateInfo(self, typesDict, stateIdx):
        threadMap = self.getThreadMap(stateIdx)
        dataSources = []
        gams = []
        threadGamsDics = {}
        for threadIdx in range(MARTE2_SUPERVISOR.MAX_THREADS):
            try:
                currThreadName = self.getNode('STATE_%d_THREAD_%d_NAME', (stateIdx+1, threadIdx+1)).data()
            except:
                raise Exception("Missing name ofr thread "+str(threadIdx+1)+'  state: '+str(stateIdx+1))

            currDataSources, currGams = self.getThreadInfo(threadMap, typesDict, stateIdx, threadIdx)
            if len(currGams) == 0:
                continue
            gamNames = []
            for currGam in gams:
                gamNames.append(currGam['Name'])
            gams.append(currGams)
            dataSources.append(curDataSources)

        return {
            'DataSources': dataSources,
            'Gams': gams
            'ThreadGams': gamNames
        }

    





                retInfo['TimerBBD'] = threadName+'_TimerDDB'
                retInfo['TimerType'] = timerType
                retInfo['TimerPeriod'] = timerPeriod / syncDiv
                retInfo['DataSources'] = retDataSources
                retInfo['Gams'] = retGams










$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
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
