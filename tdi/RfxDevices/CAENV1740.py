class CAENV1740(object):
    """CAEN V1740 64 Channels 12 Bit 65MS/S Digitizer"""
    def __init__(self,node):
      print "SONO IO!!!!!!"
      self.node=node
      self.N_HEAD = 0
      self.N_COMMENT = 1
      self.N_BOARD_ID = 2
      self.N_VME_ADDRESS = 3
      self.N_TRIG_MODE = 4
      self.N_TRIG_SOFT = 5
      self.N_TRIG_EXT = 6
      self.N_TRIG_SOURCE = 7
      self.N_TRIG_INT_TIM = 8
      self.N_CLOCK_MODE = 9
      self.N_CLOCK_DIV = 10
      self.N_CLOCK_SOURCE = 11
      self.N_NUM_SEGMENTS = 12
      self.N_USE_TIME = 13
      self.N_PTS = 14
      self.N_START_IDX = 15
      self.N_END_IDX = 16
      self.N_START_TIME = 17
      self.N_END_TIME = 18
      self.N_CONT_SAMPLES = 19
      self.K_NODES_PER_CHANNEL = 6
      self.N_CHANNEL_0= 20
      self.N_CHAN_STATE = 1
      self.N_CHAN_TRIG_STATE = 2
      self.N_CHAN_TRIG_THRESH_LEV = 3
      self.N_CHAN_TRIG_THRESH = 4
      self.N_CHAN_OFFSET = 5
      self.N_CHAN_DATA = 6
      self.cvV1718 = 0L                    # CAEN V1718 USB-VME bridge    
      self.cvV2718 = 1L                    # V2718 PCI-VME bridge with optical link       
      self.cvA2818 = 2L                    # PCI board with optical link                  
      self.cvA2719 = 3L                    # Optical link piggy-back                      
      self.cvA32_S_DATA = 0x0D             # A32 supervisory data access                  */
      self.cvD32 = 0x04		    # D32
      self.cvD64 = 0x08
      self.N_DATA_0= 68
      return

    def init(self,arg):
      from MDSplus import Tree, TreeNode, Int16Array, Float64Array, Int32, Int64, Float32, Float64, Signal, Data, Dimension, Window, Range
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time
      caenLib = CDLL("libCAENVME.so")
      handle = c_long(0)
      status = caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
      if status != 0:
	print 'Error initializing CAENVME' 
	return 0

      try:
	baseNid = self.node.getNid()
 
   	boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
    	print 'BOARD ID: ', boardId
    	vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
    	print 'VME ADDRESS: ', vmeAddress
  #Module Reset
    	data = c_int(0)
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0xEF24), byref(data), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error resetting V1740 Device' 
	    caenLib.CAENVME_End(handle)
	    return 0
	
    #give some time
    	time.sleep(0.01)

#number of segments
    	segmentDict = {1:0, 2:1, 4:2, 8:3, 16:4, 32:5, 64:6, 128:7, 256:8, 512:9, 1024:10}
    	nSegments=TreeNode(baseNid+self.N_NUM_SEGMENTS).data()
    	segmentCode = segmentDict[nSegments]
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x800c), byref(c_int(segmentCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error writing number of segments' 
	    caenLib.CAENVME_End(handle)
	    return 0
#Global Group Configuration
    	trigModeDict = {'OVER THRESHOLD':0, 'UNDER THRESHOLD':1}
    	trigMode = TreeNode(baseNid + self.N_TRIG_MODE).data()
    	trigModeCode = trigModeDict[trigMode]
    	conf = trigModeCode << 6
    	conf = conf | 0x00000010;
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8000), byref(c_int(conf)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error writing group configuration' 
	    caenLib.CAENVME_End(handle)
	    return 0

#Group configurations
    	trigEnableCode = 0L
    	chanEnableCode = 0L
    	enabledDict = {'ENABLED':1, 'DISABLED':0}
    	for group in range(0,8):
#threshold level
#	threshold = TreeNode(baseNid+self.N_CHANNEL_0 + group * K_NODES_PER_CHANNEL + self.N_CHAN_TRIG_THRESH_LEV).data()
#   	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x1080 + group * 0x100), byref(c_int(threshold)), c_int#(self.cvA32_S_DATA), c_int(self.cvD32))
#    	if status != 0:
#	    print 'writing threshold level'
#	    caenLib.CAENVME_End(handle)
#	    return 0
#offset
	    offset = TreeNode(baseNid+self.N_CHANNEL_0 + group * self.K_NODES_PER_CHANNEL + self.N_CHAN_OFFSET).data()
	    if(offset > 1):
	    	offset = 1.
	    if(offset < -1):
	    	offset = -1
	    offset = (offset / 1.) * 32767
   	    status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x1098 + group * 0x100), byref(c_int(int(offset + 0x08000))), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	    if status != 0:
	    	print 'Error writing DAC offset'
	    	caenLib.CAENVME_End(handle)
	    	return 0
#states
	    state = TreeNode(baseNid+self.N_CHANNEL_0 + group * self.K_NODES_PER_CHANNEL + self.N_CHAN_STATE).data()
	    chanEnableCode = chanEnableCode | (enabledDict[state] << group)
	    trigState = TreeNode(baseNid+self.N_CHANNEL_0 + group * self.K_NODES_PER_CHANNEL + self.N_CHAN_TRIG_STATE).data()
	    trigEnableCode = trigEnableCode | (enabledDict[trigState] << group)
#endfor group in range(0,8)

    	trigExt = TreeNode(baseNid + self.N_TRIG_EXT).data()
    	trigEnableCode = trigEnableCode | (enabledDict[trigExt] << 30)
    	trigSoft = TreeNode(baseNid + self.N_TRIG_SOFT).data()
    	trigEnableCode = trigEnableCode | (enabledDict[trigSoft] << 31)
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error writing trigger configuration'
	    caenLib.CAENVME_End(handle)
	    return 0
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8120), byref(c_int(chanEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error writing channel enabling'
	    caenLib.CAENVME_End(handle)
	    return 0

#Front Panel trigger out setting set TRIG/CLK to TTL
    	data = 1
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x811C), byref(c_int(data)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

    	try:
    	    trigSource = TreeNode(baseNid + self.N_TRIG_SOURCE).data()
	
#if trigger is expressed as an array, consider only the first element
    	    print 'Trigger source: ', trigSource
	    if len(TreeNode(baseNid + self.N_TRIG_SOURCE).getShape()) > 0:
	    	trigSource = trigSource[0]
    	except:
	    print 'Cannot resolve Trigger source'
	    caenLib.CAENVME_End(handle)
	    return 0

#Clock source	
    	clockMode = TreeNode(baseNid + self.N_CLOCK_MODE).data()
    	if clockMode == 'EXTERNAL':
	    try:
   	    	clockSource = TreeNode(baseNid + self.N_CLOCK_SOURCE).getData()
    	    	print 'Clock source: ', clockSource
    	    except:
	    	print 'Cannot resolve Clock source'
	    	caenLib.CAENVME_End(handle)
	    	return 0
    	else:
	    
	    clockSource = Range(None, None, Float64(1/62.5E6))
    	    TreeNode(baseNid + self.N_CLOCK_SOURCE).putData(clockSource)

#Post Trigger Samples
    	try:
	    pts = TreeNode(baseNid + self.N_PTS).data()
    	except:
	    print 'Cannot resolve PTS samples'
	    caenLib.CAENVME_End(handle)
	    return 0
   	segmentSize = 196608/nSegments
    	if pts > segmentSize:
	    print 'PTS Larger than segmentSize'
	    caenLib.CAENVME_End(handle)
	    return 0
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8114), byref(c_int(pts)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

#Time management
    	useTime=TreeNode(baseNid+self.N_USE_TIME).data()
    	if useTime == 'YES':
	    try:
    	    	startTime = TreeNode(baseNid+self.N_START_TIME).data()
	    	endTime = TreeNode(baseNid+self.N_END_TIME).data()
	    except:
	    	print 'Cannot Read Start or End time'
	    	caenLib.CAENVME_End(handle)
	    	return 0
	    if endTime > 0:
	    	endIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), endTime + trigSource)
	    else:
	    	endIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + endTime), clockSource), trigSource)
   	    TreeNode(baseNid + self.N_END_IDX).putData(Int32(endIdx))

	    if startTime > 0:
	    	startIdx = Data.execute('x_to_i($1, $2)', Dimension(Window(0, None, trigSource), clockSource), startTime + trigSource)
	    else:
	    	startIdx = -Data.execute('x_to_i($1,$2)', Dimension(Window(0, None, trigSource + startTime), clockSource), trigSource)
   	    TreeNode(baseNid + self.N_START_IDX).putData(Int32(startIdx))

# Run device 
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8100), byref(c_int(4)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	caenLib.CAENVME_End(handle)
    	return 1
      except:
	print 'Generic Error' 
	caenLib.CAENVME_End(handle)
	return 0

################################TRIGGER###################################

    def trigger(self,arg):
      from MDSplus import Tree, TreeNode, Int16Array, Float64Array, Int32, Int64, Float32, Float64, Signal, Data, Dimension, Window, Range
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time
      caenLib = CDLL("libCAENVME.so")
      handle = c_long(0)
      status = caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
      if status != 0:
	print 'Error initializing CAENVME' 
	return 0
      try:
 	baseNid = self.node.getNid()
    	boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
    	print 'BOARD ID: ', boardId
    	vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
    	print 'VME ADDRESS: ', vmeAddress
  #Module Reset
    	data = c_int(0)
    	status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8108), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
    	if status != 0:
	    print 'Error resetting V1740 Device' 
	    caenLib.CAENVME_End(handle)
	    return 0

    	caenLib.CAENVME_End(handle)
    	return 1
      except:
	print 'Generic Error' 
	caenLib.CAENVME_End(handle)
	return 0

####################################STORE###################################

    def store(self,arg):
      from MDSplus import Tree, TreeNode, Int16Array, Float64Array, Int32, Int64, Float32, Float64, Signal, Data, Dimension, Window, Range
      from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
      import time
      caenLib = CDLL("libCAENVME.so")
      handle = c_long(0)
      status = caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
      if status != 0:
	print 'Error initializing CAENVME' 
	return 0
      try:
	baseNid = self.node.getNid()
    	boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
    	print 'BOARD ID: ', boardId
    	vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
    	print 'VME ADDRESS: ', vmeAddress
    	try:
	    clock = TreeNode(baseNid + self.N_CLOCK_SOURCE).evaluate()
	    dt = clock.getDelta().data()
    	except:
	    print 'Error evaluating clock source' 
	    caenLib.CAENVME_End(handle)
	    return 0
    	try:
	    trig = TreeNode(baseNid + self.N_TRIG_SOURCE).data()
    	except:
	    print 'Error evaluating trigger source' 
	    caenLib.CAENVME_End(handle)
	    return 0
    	try:
	    startIdx = TreeNode(baseNid + self.N_START_IDX).data()
	    endIdx = TreeNode(baseNid + self.N_END_IDX).data()
    	except:
	    print 'Error evaluating start or end idx' 
	    caenLib.CAENVME_End(handle)
	    return 0
        try:
    	    pts = TreeNode(baseNid + self.N_PTS).data()
        except:
    	    print 'Error evaluating Post Trigger Samples' 
    	    caenLib.CAENVME_End(handle)
    	    return 0
    
    
    # Stop device 
        status = caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8100), byref(c_int(0L)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
    	    print 'Error stopping device' 
    	    caenLib.CAENVME_End(handle)
    	    return 0
    #need to wait a while
        time.sleep(0.1)
    
    
    # Read number of buffers */
        actSegments = c_int(0)
        status = caenLib.CAENVME_ReadCycle(handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        if status != 0:
    	    print 'Error reading number of acquired segments' 
    	    caenLib.CAENVME_End(handle)
    	    return 0
    
        print 'Acquired segments: ', actSegments.value
	if actSegments.value == 0:
    	    caenLib.CAENVME_End(handle)
    	    return 1

    
    #Compute Segment Size
        try:
            nSegments = TreeNode(baseNid + self.N_NUM_SEGMENTS).data()
            segmentSamples = 196608/nSegments
    	    print 'Segment samples: ', segmentSamples
        except: 
    	    print 'Error reading max number of segments' 
    	    caenLib.CAENVME_End(handle)
    	    return 0
    	
    
    # Get Active groups
        groupMask = c_int(0)
        status = caenLib.CAENVME_ReadCycle(handle, c_int(vmeAddress + 0x8120), byref(groupMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
        nActGroups = 0
        groupMask = groupMask.value
        for group in range(0,8):
    	    if (groupMask & (1 << group)) != 0:
    	    	nActGroups = nActGroups + 1
            if nActGroups == 0:
    		print 'No active groups' 
    		caenLib.CAENVME_End(handle)
    		return 1
 
       	segmentSize = 16 + segmentSamples * nActGroups * 8 * 12 / 8
       	class V1740Data(Structure):
        	_fields_ = [("eventSize", c_int), ("boardGroup", c_int), ("counter", c_int), ("time", c_int), ("data", c_int * (segmentSamples * 64*12/(8*4)))]
    
        actSegments = actSegments.value
        currStartIdx = segmentSamples - pts + startIdx
        currEndIdx = segmentSamples - pts + endIdx
    
        DataArray = c_short * ((currEndIdx - currStartIdx + 1) * actSegments)
        triggers = []
        deltas = []
        channels = [] 
        for chan in range(0,64):
    	    channels.append([])
        for chan in range(0,64):
    	    channels[chan] = DataArray()
    
        c = []
        for i in range(0,64):
    	    c.append(0)
        for sample in range(0,actSegments):
    	    segment= V1740Data()
    	    retLen = c_int(0)
    	    status = caenLib.CAENVME_FIFOBLTReadCycle(handle, c_int(vmeAddress), byref(segment), c_int(segmentSize), 
    		c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))
            if status != 0:
    		print 'Error reading data segment' 
    	  	caenLib.CAENVME_End(handle)
    		return 0
      	    actSize = 4 * (segment.eventSize & 0x0fffffff)
    	    if actSize != segmentSize: 
    		print 'Expected event size different from expected size' 
    	  	caenLib.CAENVME_End(handle)
    		return 0
    	    counter = segment.time/2
    	    triggers.append(counter*dt)
    	    deltas.append(dt)
    	    sizeInInts = (segment.eventSize & 0x0fffffff) - 4;
    	    groupSize = sizeInInts/nActGroups;
    	    groupOffset = 0
            for group in range(0,8):
    	    	if (groupMask & (1 << group)) != 0:
    		    rpnt = 0
    		    sampleCount = 0;
    		    while rpnt < groupSize :
    		        if rpnt % 9 == 0:
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    			    	channels[group*8][c[group*8+0]] = segment.data[groupOffset+rpnt] & 0x00000FFF
    				c[group*8+0] = c[group*8+0]+1
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    			    	channels[group*8][c[group*8+0]] = (segment.data[groupOffset+rpnt] & 0x00FFF000) >> 12
    				c[group*8+0] = c[group*8+0]+1
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    			    	channels[group*8][c[group*8+0]] = ((segment.data[groupOffset+rpnt] & 0xFF000000) >> 24) | ((segment.data[groupOffset+rpnt+1] & 0x0000000F) << 8)
    				c[group*8+0] = c[group*8+0]+1
    		        if rpnt % 9 == 1:
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    			    	channels[group*8+1][c[group*8+1]] = (segment.data[groupOffset+rpnt] & 0x0000FFF0) >> 4
    				c[group*8+1] = c[group*8+1]+1
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    			    	channels[group*8+1][c[group*8+1]] = (segment.data[groupOffset+rpnt] & 0x0FFF0000) >> 16
    				c[group*8+1] = c[group*8+1]+1
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    			    	channels[group*8+1][c[group*8+1]] = ((segment.data[groupOffset+rpnt] & 0xF0000000) >> 28) | ((segment.data[groupOffset+rpnt+1] & 0x000000FF) << 4)
    				c[group*8+1] = c[group*8+1]+1
    		        if rpnt % 9 == 2:
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    			    	channels[group*8+2][c[group*8+2]] = (segment.data[groupOffset+rpnt] &   0x000FFF00) >> 8
    				c[group*8+2] = c[group*8+2]+1
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+2][c[group*8+2]] = (segment.data[groupOffset+rpnt] & 0xFFF00000) >> 20
    				c[group*8+2] = c[group*8+2]+1
    		        if rpnt % 9 == 3:
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+2][c[group*8+2]] = segment.data[groupOffset+rpnt] &  0x00000FFF
    				c[group*8+2] = c[group*8+2]+1
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    		    	    	channels[group*8+3][c[group*8+3]] = (segment.data[groupOffset+rpnt]  & 0x00FFF000) >> 12
    				c[group*8+3] = c[group*8+3]+1
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+3][c[group*8+3]] = ((segment.data[groupOffset+rpnt]  & 0xFF000000) >> 24) | ((segment.data[groupOffset+rpnt+1] & 0x0000000F) << 8)
    				c[group*8+3] = c[group*8+3]+1
    		        if rpnt % 9 == 4:
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+3][c[group*8+3]] = (segment.data[groupOffset+rpnt] & 0x0000FFF0) >> 4
    				c[group*8+3] = c[group*8+3]+1
   			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    		    	    	channels[group*8+4][c[group*8+4]] = (segment.data[groupOffset+rpnt] & 0x0FFF0000) >> 16
    				c[group*8+4] = c[group*8+4]+1
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+4][c[group*8+4]] = ((segment.data[groupOffset+rpnt] & 0xF0000000) >> 28) | ((segment.data[groupOffset+rpnt+1] & 0x000000FF) << 4)
    				c[group*8+4] = c[group*8+4]+1
    		        if rpnt % 9 == 5:
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+4][c[group*8+4]] = (segment.data[groupOffset+rpnt]  & 0x000FFF00) >> 8
    				c[group*8+4] = c[group*8+4]+1
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    		    	    	channels[group*8+5][c[group*8+5]] = (segment.data[groupOffset+rpnt] & 0xFFF00000) >> 20
    				c[group*8+5] = c[group*8+5]+1
    		        if rpnt % 9 == 6:
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+5][c[group*8+5]] = segment.data[groupOffset+rpnt]  & 0x00000FFF
    				c[group*8+5] = c[group*8+5]+1
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+5][c[group*8+5]] = (segment.data[groupOffset+rpnt] & 0x00FFF000) >> 12
    				c[group*8+5] = c[group*8+5]+1
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    		    	    	channels[group*8+6][c[group*8+6]] = ((segment.data[groupOffset+rpnt] & 0xFF000000) >> 24) | ((segment.data[groupOffset+rpnt+1] & 0x0000000F) << 8)
    				c[group*8+6] = c[group*8+6]+1
    		        if rpnt % 9 == 7:
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+6][c[group*8+6]] = (segment.data[groupOffset+rpnt] & 0x0000FFF0) >> 4
    				c[group*8+6] = c[group*8+6]+1
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+6][c[group*8+6]] = (segment.data[groupOffset+rpnt] & 0x0FFF0000) >> 16
    				c[group*8+6] = c[group*8+6]+1
    			    if sampleCount >= currStartIdx and sampleCount <= currEndIdx :
    		    	    	channels[group*8+7][c[group*8+7]] = ((segment.data[groupOffset+rpnt] & 0xF0000000) >> 28) | ((segment.data[groupOffset+rpnt+1] & 0x000000FF) << 4)
    				c[group*8+7] = c[group*8+7]+1
    		        if rpnt % 9 == 8:
    			    if sampleCount +1 >= currStartIdx and sampleCount + 1 <= currEndIdx :
    		    	    	channels[group*8+7][c[group*8+7]] = (segment.data[groupOffset+rpnt] & 0x000FFF00) >> 8
    				c[group*8+7] = c[group*8+7]+1
    			    if sampleCount +2 >= currStartIdx and sampleCount +2 <= currEndIdx :
    		    	    	channels[group*8+7][c[group*8+7]] = (segment.data[groupOffset+rpnt] & 0xFFF00000) >> 20
    				c[group*8+7] = c[group*8+7]+1
    		        if rpnt % 9 == 8:
    		    	    sampleCount = sampleCount + 3
    		        rpnt = rpnt + 1
    		    #endwhile
    		    groupOffset = groupOffset + groupSize
    	        #endif 
            #endfor group in range(0:8)
        #endfor samples in range(0, actSegments)
    
    	
        if len(TreeNode(baseNid + self.N_TRIG_SOURCE).getShape()) > 0:
            dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig[0]),Range(Float64Array(trig) + Float64(startIdx * dt),  Float64Array(trig) + Float64(endIdx * dt), Float64Array(deltas)))
        else:
            dim = Dimension(Window(startIdx,endIdx+(actSegments - 1) * (endIdx - startIdx), trig),Range(Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(startIdx * dt),  Float64Array(triggers) - Float64(triggers[0]) + Float64(trig) + Float64(endIdx * dt), Float64Array(deltas)))
    
        print 'DIM: ', dim
        dim.setUnits("s");
        chan0Nid = baseNid + self.N_CHANNEL_0
        data0Nid = baseNid + self.N_DATA_0
        for group in range(0,8):
    	    if groupMask & (1 << group):
        	try:
    	    	    offset = TreeNode(baseNid + self.N_CHANNEL_0 + group * self.N_CHAN_DATA + self.N_CHAN_OFFSET).data()
        	except:
    		    print 'Error evaluating group offset' 
    		    caenLib.CAENVME_End(handle)
    		    return 0
    	        for chan in range(0,8):
    		    raw = Int16Array(channels[group * 8 + chan])
    		    raw.setUnits("counts")
    		    data = Data.compile("2*($VALUE - 2048)/4096.+$1", offset);
    		    data.setUnits("Volts")
    		    signal = Signal(data, raw, dim)
    		    try:
    		    	TreeNode(baseNid + self.N_DATA_0 + group * 8 + chan).putData(signal)
        	    except:
    		    	print 'Cannot write Signal in the tree' 
    		    	caenLib.CAENVME_End(handle)
    		    	return 0
        	#endfor
    	    #endif
        #endfor
    
        caenLib.CAENVME_End(handle)
        return 1
      except:
	print 'Generic Error' 
	caenLib.CAENVME_End(handle)
	return 0

 
