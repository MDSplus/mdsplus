from MDSplus import *

class DIO4(Device):
    """INCAA DIO4 Timing Module"""
    parts=[{'path':':BOARD_ID', 'type':'numeric', 'value':0},
      {'path':':SW_MODE', 'type':'text', 'value':'LOCAL'},
      {'path':':IP_ADDR', 'type':'text'},
      {'path':':COMMENT', 'type':'text'},
      {'path':':CLOCK_SOURCE', 'type':'text', 'value':'INTERNAL'},
      {'path':':REC_START_EV', 'type':'text'},
      {'path':':REC_EVENTS', 'type':'text'},
      {'path':':REC_TIMES', 'type':'numeric'},
      {'path':':SYNCH', 'type':'text', 'value':'NO'},
      {'path':':SYNCH_EVENT', 'type':'text'}]
    print 'DIO4'	
    for i in range(8):
      parts.append({'path':'.CHANNEL_%d'%(i+1), 'type':'structure'})
      parts.append({'path':'.CHANNEL_%d:FUNCTION'%(i+1), 'type':'text', 'value':'CLOCK'})
      parts.append({'path':'.CHANNEL_%d:TRIG_MODE'%(i+1), 'type':'text', 'value':'SOFTWARE'})
      parts.append({'path':'.CHANNEL_%d:EVENT'%(i+1), 'type':'text'})
      parts.append({'path':'.CHANNEL_%d:CYCLIC'%(i+1), 'type':'text', 'value':'NO'})
      parts.append({'path':'.CHANNEL_%d:DELAY'%(i+1), 'type':'numeric', 'value':0.})
      parts.append({'path':'.CHANNEL_%d:DURATION'%(i+1), 'type':'numeric', 'value':1.})
      parts.append({'path':'.CHANNEL_%d:FREQ_1'%(i+1), 'type':'numeric', 'value':1000})
      parts.append({'path':'.CHANNEL_%d:FREQ_2'%(i+1), 'type':'numeric', 'value':1000})
      parts.append({'path':'.CHANNEL_%d:INIT_LEV_1'%(i+1), 'type':'text', 'value':'LOW'})
      parts.append({'path':'.CHANNEL_%d:INIT_LEV_2'%(i+1), 'type':'text', 'value':'LOW'})
      parts.append({'path':'.CHANNEL_%d:DUTY_CYCLE'%(i+1), 'type':'numeric', 'value':50})
      parts.append({'path':'.CHANNEL_%d:TRIGGER'%(i+1), 'type':'numeric', 'value':0})
      parts.append({'path':'.CHANNEL_%d:CLOCK'%(i+1), 'type':'numeric'})
      parts.append({'path':'.CHANNEL_%d:TRIGGER_1'%(i+1), 'type':'numeric'})
      parts.append({'path':'.CHANNEL_%d:TRIGGER_2'%(i+1), 'type':'numeric'})
      parts.append({'path':'.CHANNEL_%d:OUT_EV_1'%(i+1), 'type':'text'})
      parts.append({'path':'.CHANNEL_%d:OUT_EV_2'%(i+1), 'type':'text'})
      parts.append({'path':'.CHANNEL_%d:TERM_1'%(i+1), 'type':'text', 'value':'ON'})
      parts.append({'path':'.CHANNEL_%d:TERM_2'%(i+1), 'type':'text', 'value':'ON'})
      parts.append({'path':'.CHANNEL_%d:EDGE_1'%(i+1), 'type':'text', 'value':'RISING'})
      parts.append({'path':'.CHANNEL_%d:EDGE_2'%(i+1), 'type':'text', 'value':'RISING'})
      parts.append({'path':'.CHANNEL_%d:COMMENT'%(i+1), 'type':'text'})
    del i
    parts.append({'path':':INIT_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('CPCI_SERVER','INIT',50,None),Method(None,'init',head))",
	'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('CPCI_SERVER','STORE',50,None),Method(None,'store',head))",
	'options':('no_write_shot',)})
    
    
    def init(self,arg):
      try:
      	boardId = self.board_id.data()
      except: 
        Data.execute('DevLogErr($1,$2)', getNid(), 'Invalid Board ID specification')
        return 0
	
      
 #Software Mode
      try:
      	swMode = self.sw_mode.data()
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Software Mode'])
	return 0
      if swMode == 'REMOTE':
        try:
	  ipAddr = self.ip_addr.data()
        except: 
      	  Data.excute('DevLogErr', [getNid(), 'Invalid Remote IP Address'])
	  return 0
 
 #Clock Source      
      clockSourceDict = {'INTERNAL':0, 'HIGHWAY':1}
      try:
      	clockSource = clockSourceDict[self.clock_source.data()]
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Clock Source'])
	return 0

#Recorder Event
      try:
      	recStartEv = Data.execute('TimingDecodeEvent($1)', self.rec_start_ev)
      except: 
      	recStartEv = 0
	  
#Synch events	  
      try:
      	synch = self.synch.data()
	if synch == 'YES':
	  synchEvent = self.synch_event.data()
          synchEvSize =Data.execute('size($1)', synchEvent)
	  if synchEvSize == 1:
	    synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent))
	  else:
	    for i in range(synchEvSize):
	      synchEvents.append(Data.execute('TimingDecodeEvent($1)', synchEvent[i]))
	else:
	  synchEvents = [-1]
      except:       
      	Data.excute('DevLogErr', [getNid(), 'Invalid Synch Event(s)'])
	return 0

#HW Init
      if swMode == 'REMOTE':
        status = Data.execute('MdsConnect("'+ ipAddr + '")')
	if status > 0:
	  status = Data.execute('MdsValue("DIO4HWInit(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, clockSource, recStartEv, makeArray(synchEvents))
	  if status == 0:
	    Data.execute('MdsDisconnect()')
      	    Data.excute('DevLogErr', [getNid(), 'Cannot execute remote HW initialization. See CPCI console for details'])
	    return 0
	else:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot connect to remote CPCI system'])
	  return 0
      else:
        status = Data.execute("DIO4HWInit(0, $1, $2, $3, $4)", boardId, clockSource, recStartEv, makeArray(synchEvents))
	if status == 0:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot execute HW initialization'])
	  return 0

      trigModeDict = {'EVENT':0, 'RISING EDGE':1, 'FALLING EDGE':2, 'SOFTWARE':3}
		
#Channels Setup
      channelMask = 0
      huge = Data.execute('HUGE(0)')
      for c in range(8):
        print 'CHANNEL ' + str(c)
        if getattr(self, 'channel_%d'%(c+1)).isOn():
	  print 'IS ON ' + str(c)
  	  channelMask = channelMask | (1 << c)
	  function = getattr(self, 'channel_%d_function'%(c+1)).data()
	  if function == 'CLOCK': #Clock Generation
	    try :
	      frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
	      dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
	    except:
     	      Data.excute('DevLogErr', [getNid(), 'Invalid clock parameters for channel %d'%(c+1)])
	      return 0
            if swMode == 'REMOTE':
	      status = Data.execute('MdsValue("DIO4HWSetClockChan(0, $1, $2, $3, $4)", $1,$2,$3,$4)', boardId, c, frequency, dutyCycle)
	      if status == 0:
	        Data.execute('MdsDisconnect()')
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute remote HW clock setup. See CPCI console for details'])
	        return 0
            else:
              status = Data.execute("DIO4HWSetClockChan(0, $1, $2, $3, $4)", boardId, c, frequency, dutyCycle)
	      if status == 0:
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute HW clock setup'])
	        return 0
	  elif function == 'PULSE':   #Trigger Generation
	    print function
	    try:
	      trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
	      print trigMode
	      trigModeCode = trigModeDict[trigMode]
	      if trigMode == 'EVENT':
	        try:
	          event = getattr(self,'channel_%d_event'%(c+1)).data() 
                  eventSize = Data.execute('size($1)', event)
		  eventCodes = []
		  print event
		  print eventSize

	          if eventSize == 1:
	            eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
		    eventTime = Data.execute('TimingGetEventTime($1)', event)
		    print eventCodes
		    print eventTime

		    if eventTime == huge or eventTime == -huge:
		      eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		    else:
		      setattr(self,'channel_%d_trigger'%(c+1), eventTime)
	          else:
	            for i in range(eventSize):
	              eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
	            trigger = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		except:
      	          Data.excute('DevLogErr', [getNid(), 'Cannot resolve event(s) for channel %d'%(c+1)])
	          return 0
	      else: #no events
	        eventCodes = [-1]
		
	      print 'CACCA!!!!!!!!'
	      cyclicDict = {'NO':0, 'YES':1}
	      levelDict = {'LOW':0, 'HIGH':1}
	      cyclic = cyclicDict[getattr(self,'channel_%d_cyclic'%(c+1)).data()]
	      print cyclic
	      initLev1 = levelDict[getattr(self,'channel_%d_init_lev_1'%(c+1)).data()]
	      print initLev1		  
	      initLev2 = levelDict[getattr(self,'channel_%d_init_lev_2'%(c+1)).data()]
	      print initLev2
	      duration = getattr(self,'channel_%d_duration'%(c+1)).data()
	      print duration
	      delay = getattr(self,'channel_%d_delay'%(c+1)).data()
	      print delay
	    except:
     	      Data.excute('DevLogErr', [getNid(), 'Invalid trigger parameters for channel %d'%(c+1)])
	      return 0
	      
            if swMode == 'REMOTE':
	      status = Data.execute('MdsValue("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)", $1,$2,$3,$4,$5,$6,$7,$8,$9)', boardId, c, trigModeCode, 
	      		cyclic, initLev1, initLev2, delay, duration, makeArray(eventCodes))
	      if status == 0:
	        Data.execute('MdsDisconnect()')
      	        Data.execute('DevLogErr($1, $2)', getNid(), 'Cannot execute remote HW pulse setup. See CPCI console for details')
	        return 0
            else:
	      print 'before'
	      eventCodesA = makeArray(eventCodes)
	      print 'after'
	      print eventCodesA
              status = Data.execute("DIO4HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)",boardId, c, trigModeCode, 
	      		cyclic, initLev1, initLev2, delay, duration, eventCodesA)
	      if status == 0:
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute HW pulse setup'])
	        return 0
	    setattr(self, 'channel_%d_trig_1'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"
	    	+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()))
	    setattr(self, 'channel_%d_trig_2'%(c+1), Data.compile(getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"
	    	+ getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()))

	  elif function == 'GCLOCK':   #Gated clock generation
	    try:
	      trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
	      trigModeCode = trigModeDict[trigMode]
	      if trigMode == 'EVENT':
	        try:
	          event = getattr(self,'channel_%d_event'%(c+1)).data() 
                  eventSize = Data.execute('size($1)', event)
		  eventCodes = []
	          if eventSize == 1:
	            eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
		    eventTime = Data.execute('TimingGetEventTime($1)', event)
		    if eventTime == huge or eventTime == -huge:
		      eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		    else:
		      setattr(self,'channel_%d_trigger'%(c+1), eventTime)
	          else:
	            for i in range(eventSize):
	              eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
	            trigger = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		except:
      	          Data.excute('DevLogErr', [getNid(), 'Cannot resolve event(s) for channel %d'%(c+1)])
	          return 0
	      else: #no events
	        eventCodes = [-1]
	      frequency = getattr(self,'channel_%d_freq_1'%(c+1)).data()
	      duration = getattr(self,'channel_%d_duration'%(c+1)).data()
	      delay = getattr(self,'channel_%d_delay'%(c+1)).data()
	      dutyCycle = getattr(self,'channel_%d_duty_cycle'%(c+1)).data()
	      print 'FATTO !!!!'
	    except:
     	      Data.excute('DevLogErr', [getNid(), 'Invalid Gated Clock parameters for channel %d'%(c+1)])
	      return 0
            if swMode == 'REMOTE':
	      status = Data.execute('MdsValue("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)", $1,$2,$3,$4,$5,$6,$7, $8)', boardId, c, trigModeCode, 
	      		frequency, delay, duration, makeArray(eventCodes), dutyCycle)
	      if status == 0:
	        Data.execute('MdsDisconnect()')
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute remote HW GClock setup. See CPCI console for details'])
	        return 0
            else:
	      print 'BEFORE'
              status = Data.execute("DIO4HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)",boardId, c, trigModeCode, 
	      		frequency, delay, duration, makeArray(eventCodes), dutyCycle)
	      if status == 0:
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute HW GClock setup'])
	        return 0
	    print 'FATTO !!!!'
	    trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
	    trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
	    frequency = long((1. / frequency) / 1E-7 + 0.5) * 1E-7
	    period = 1./frequency
	    setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE('+trig1+','+trig2+','+str(period)+')'))

	  elif function == 'DCLOCK': #Dual Speed clock generator
	    try:
	      trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
	      trigModeCode = trigModeDict[trigMode]
	      if trigMode == 'EVENT':
	        try:
	          event = getattr(self,'channel_%d_event'%(c+1)).data() 
                  eventSize = Data.execute('size($1)', event)
		  eventCodes = []
	          if eventSize == 1:
	            eventCodes.append(Data.execute('TimingDecodeEvent($1)', event))
		    eventTime = Data.execute('TimingGetEventTime($1)', event)
		    if eventTime == huge or eventTime == -huge:
		      eventTime = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		    else:
		      setattr(self,'channel_%d_trigger'%(c+1), eventTime)
	          else:
	            for i in range(eventSize):
	              eventCodes.append(Data.execute('TimingDecodeEvent($1)', event[i]))
	            trigger = getattr(self,'channel_%d_trigger'%(c+1)).data() 
		except:
      	          Data.excute('DevLogErr', [getNid(), 'Cannot resolve event(s) for channel %d'%(c+1)])
	          return 0
	      else: #no events
	        eventCodes = [-1]
	      freq1 = getattr(self,'channel_%d_freq_1'%(c+1)).data()
	      freq2 = getattr(self,'channel_%d_freq_2'%(c+1)).data()
	      duration = getattr(self,'channel_%d_duration'%(c+1)).data()
	      delay = getattr(self,'channel_%d_delay'%(c+1)).data()
	    except:
     	      Data.excute('DevLogErr', [getNid(), 'Invalid Dual Speed Clock parameters for channel %d'%(c+1)])
	      return 0
            if swMode == 'REMOTE':
	      status = Data.execute('MdsValue("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)", $1,$2,$3,$4,$5,$6,$7,%8)', boardId, c, trigModeCode, 
	      		freq1, freq2, delay, duration, makeArray(eventCodes))
	      if status == 0:
	        Data.execute('MdsDisconnect()')
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute remote HW DClock setup. See CPCI console for details'])
	        return 0
            else:
              status = Data.execute("DIO4HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)",boardId, c, trigModeCode, 
	      		freq1, freq2, delay, duration, makeArray(eventCodes))
	      if status == 0:
      	        Data.excute('DevLogErr', [getNid(), 'Cannot execute HW DClock setup'])
	        return 0
	    trig1 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()
	    trig2 = getattr(self,'channel_%d_trigger'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_delay'%(c+1)).getFullPath()+"+"+getattr(self,'channel_%d_duration'%(c+1)).getFullPath()
            period1 = int((1. / freq1) / 1E-7 + 0.5) * 1E-7
            period2 = int((1. / freq2) / 1E-7 + 0.5) * 1E-7
	    setattr(self, 'channel_%d_clock'%(c+1), Data.compile('BUILD_RANGE([-1E6,'+trig1+','+trig2+'],['+trig1+','+trig2+',1E6],['+str(period1)+','+str(period2)+','+str(period1)+'])'))
          elif function == 'ENCODER':

	  
            outEvents = []
	    rising = []
	    terminations = []
	    if getattr(self, 'channel_%d_term_1'%(c+1)).data() == 'ON':
	      term1 = 1
	    else:
	      term1 = 0
	    if getattr(self, 'channel_%d_term_2'%(c+1)).data() == 'ON':
	      term2 = 1
	    else:
	      term2 = 0
	    if getattr(self, 'channel_%d_edge_1'%(c+1)).data() == 'RISING':
	      rising1 = 1
	    else:
	      rising1 = 0	      
	    if getattr(self, 'channel_%d_edge_2'%(c+1)).data() == 'RISING':
	      rising2 = 1
	    else:
	      rising2 = 0
	      
	    print term1
	    print term2
	    print rising1
	    print rising2  
	    
	    try:
              outEvent1 = Data.execute('TimingDecodeEvent($1)', getattr(self, 'channel_%d_out_ev_1'%(c+1)).data())
	    except:
	      outEvent1 = -1;
	    try:
              outEvent2 = Data.execute('TimingDecodeEvent($1)', getattr(self, 'channel_%d_out_ev_2'%(c+1)).data())    
 	    except:
	      outEvent2 = -1

	    print outEvent1
	    print outEvent2

	    if outEvent1 != -1 or outEvent2 != -1:
              if swMode == 'REMOTE':
	        status = Data.execute('MdsValue("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5, $6, $7, $8)", $1,$2,$3, $4, $5, $6, $7, $8)', boardId, c, outEvent1, outEvent2, 
			term1, term2, rising1, rising2) 
	        if status == 0:
	          Data.execute('MdsDisconnect()')
      	          Data.excute('DevLogErr', [getNid(), 'Cannot execute remote HW Event Generation setup. See CPCI console for details'])
	          return 0
              else:
	        print 'ENCODER  2!!!!!'
                status = Data.execute("DIO4HWSetEventGenChan(0, $1, $2, $3, $4, $5, $6, $7, $8)",boardId, c, outEvent1, outEvent2, term1, term2, rising1, rising2) 
	        if status == 0:
      	          Data.excute('DevLogErr', [getNid(), 'Cannot execute HW Event Generation setup'])
	          return 0
          else:
      	    Data.excute('DevLogErr', [getNid(), 'Invalid function for channel %d'%(c+1)])
	    return 0
#End channel settings                  		   
      print 'CANALI SETTATI'
      print 'channelMask: ' + str(channelMask)
      print synchEvents    
      print swMode    
      if swMode == 'REMOTE':
        status = Data.execute('MdsValue("DIO4HWStartChan(0, $1, $2, $3)", $1,$2,$3)', boardId, channelMask, makeArray(synchEvents)) 
	Data.execute('MdsDisconnect()')
	if status == 0:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot start DIO4 device. See CPCI console for details'])
	  return 0
      else:
        print 'FACCIO START'
        status = Data.execute("DIO4HWStartChan(0, $1, $2, $3)",boardId, channelMask, makeArray(synchEvents))
	print 'START FATTO: status: ' + str(status) 
	if status == 0:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot start DIO4 Device'])
	  return 0
      print 'END INIT'
      return 1
  	      		  
#####################################End INIT method ##############################



    def store(self, arg):
      try:
      	boardId = self.board_id.data()
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Board ID specification'])
	return 0
	
      
 #Software Mode
      try:
      	swMode = self.sw_mode.data()
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Software Mode'])
	return 0
      if swMode == 'REMOTE':
        try:
	  ipAddr = self.ip_addr.data()
        except: 
      	  Data.excute('DevLogErr', [getNid(), 'Invalid Remote IP Address'])
	  return 0
 
      if swMode == 'REMOTE':
        status = Data.execute('MdsConnect("'+ ipAddr + '")')
	if status == 0:
	  Data.execute('MdsDisconnect()')
      	  Data.excute('DevLogErr', [getNid(), 'Cannot Connect to CPCI crate'])
	  return 0
	recEvents = Data.execute('MdsValue("DIO4HWIGetRecEvents(0, $1)", $1)', boardId)
	recTimes =  Data.execute('MdsValue("_DIO4_rec_times")');
      else:
        recEvents = Data.execute("DIO4HWGetRecEvents(0, $1)", boardId)
	recTimes = Data.execute('_DIO4_rec_times')
      self.rec_events = makeArray(recEvents)
      try:
        recStartEvTime = Data.execute('TimingGetEventTime($1)', self.rec_start_ev)
      except:
        recStartEvTime = 0
      self.rec_times = makeArray(recTimes + recStartEvTime)
      for c in range(8):
        if getattr(self, 'channel_%d'%(c+1)).isOn():
	  function = getattr(self, 'channel_%d_function'%(c+1)).data()
          if function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK':
            if swMode == 'REMOTE':
              phaseCounts = Data.execute('MdsValue("DIO4HWGetPhaseCount(0, $1, $2)", $1,$2)', boardId, c)
            else:
              phaseCounts = Data.execute("DIO4HWGetPhaseCount(0, $1, $2)",boardId, c)
            trigPath = getattr(self, 'channel_%d.trigger'%(c+1)).getFullPath()
            setattr(self, 'channel_%d.trig_1'%(c+1), Data.compile(trigPath+'+$1', phaseCounts[0]))
            setattr(self, 'channel_%d.trig_2'%(c+1), Data.compile(trigPath+'+$1', phaseCounts[1]))
      return 1
    
####################################################End Store method #######################################

 
    def trigger(self, arg):
      try:
      	boardId = self.board_id.data()
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Board ID specification'])
	return 0
	
      print 'TRIGGER'
 #Software Mode
      try:
      	swMode = self.sw_mode.data()
      except: 
      	Data.excute('DevLogErr', [getNid(), 'Invalid Software Mode'])
	return 0
      if swMode == 'REMOTE':
        try:
	  ipAddr = self.ip_addr.data()
        except: 
      	  Data.excute('DevLogErr', [getNid(), 'Invalid Remote IP Address'])
	  return 0
 
      if swMode == 'REMOTE':
        status = Data.execute('MdsConnect("'+ ipAddr + '")')
	if status == 0:
	  Data.execute('MdsDisconnect()')
      	  Data.excute('DevLogErr', [getNid(), 'Cannot Connect to CPCI crate'])
	  return 0
      channelMask = 0
      for c in range(8):
        if getattr(self, 'channel_%d'%(c+1)).isOn():
	  function = getattr(self, 'channel_%d_function'%(c+1)).data()
	  trigMode = getattr(self,'channel_%d_trig_mode'%(c+1)).data() 
          if (function == 'PULSE' or function == 'DCLOCK' or function == 'GCLOCK') and trigMode == 'SOFTWARE':
	    channelMask = channelMask | (1 << c)
	    
      if swMode == 'REMOTE':
        status = Data.execute('MdsValue("DIO4HWTrigger(0, $1, $2)", $1,$2)', boardId, channelMask) 
	Data.execute('MdsDisconnect()')
	if status == 0:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot start DIO4 device. See CPCI console for details'])
	  return 0
      else:
        print 'FACCIO TRIGGER' + str(c)
        status = Data.execute("DIO4HWTrigger(0, $1, $2)",boardId, channelMask)
	print 'TRIGGER FATTO: status: ' + str(status) 
	if status == 0:
      	  Data.excute('DevLogErr', [getNid(), 'Cannot start DIO4 Device'])
	  return 0
      return 1
       
          
  
