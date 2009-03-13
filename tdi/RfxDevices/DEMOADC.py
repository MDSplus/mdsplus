class DEMOADC(object):
    """A Demo 4 Channel, 16 but digfitizer"""
#Python instance constructor
    def __init__(self,node):
#The class is intialized passing the reference to the root node of the device instance, and it is recorded in the instance
        self.node=node
#definition of offsets of the various nodes composing the device subtree, just to make the code clearer
   	self.N_HEAD = 0
    	self.N_NAME = 1
	self.N_COMMENT = 2
	self.N_CLOCK_FREQ = 3
	self.N_TRIG_SOURCE = 4
	self.N_PTS = 5
	self.N_CHANNEL_0 = 6
#number of nodes for each channel
	self.K_NODES_PER_CHANNEL = 4
#offsets of channel subtree components with respect to the channel root
	self.N_CHAN_START_IDX = 1
	self.N_CHAN_END_IDX = 2
	self.N_CHAN_DATA = 3
        return


######################################INIT#################################################################
#init method, called to configure the ADC device. It will read configuration from the corresponding subtree
#and it will download configuration to the device
    def init(self,arg):
#Need to import some classes from MDSplus package
      	from MDSplus import TreeNode

#The device will be configured via a shared library (libDemoAdc in the MDSplus distribution) defining the following routines:
# initialize(char *name, int clockFreq, int postTriggerSamples)
# where clockFreq can have the following values:
#  - 1 -> clock freq. = 1KHz
#  - 2 -> clock freq. = 5KHz
#  - 3 -> clock freq. = 10KHz
#  - 4 -> clock freq. = 50 KHz
#  - 5 -> clock freq. = 100KHz
# trigger(char *name)
# acquire(char *name, short *c1, short *c2 short *c3, short *c4)
# the routine acquire returns 4  simulated sinusoidal waveform signals at the following frequencies:
# Channel 1: 10Hz
# Channel 2: 50 Hz
# Channel 3: 100 Hz
# Channel 4: 200Hz
#
# The name argument passed to all device routines is not used and simulates the device identifier
# used in real devices
#
#Since we need to link against a shaed library from python, we need ctypes package.
        from ctypes import CDLL, c_int, c_char_p
	try:
      	    deviceLib = CDLL("libDemoAdcShr.so")
	except:
	    print 'Cannot link to device library'
	    return 0

#deviceLib is the ctype DLL object which allows to call library routines

	#the node field is a TreeNode object. We get its integer nid (node identifier) via method getNid()
	baseNid = self.node.getNid()
	#we will add offsets to baseNid to identify nodes containing configuration data.

#in the following, we'll get data items in two steps (on the same line):
#1) instantiate a TreeNode object, passing the integer nid to the constructor
#2) read and evaluate (in the case the content is an expression) its content via TreeNode.data() method 
#all data access operation will be but in a try block in order to check for missing or wrong configuration data
	try:
   	    name = TreeNode(baseNid + self.N_NAME).data()
#we expect to get a string in name
	except:
	    print 'Missing Name in device'
	    return 0


#read the clock frequency and convert to clock mode. We use a dictionary for the conversion, and assume 
	clockDict = {1000:1, 5000:2, 10000:3, 50000:4, 100000:5}
	try:
	    clockFreq = TreeNode(baseNid + self.N_CLOCK_FREQ).data()
	    clockMode = clockDict[clockFreq]
	except:
	    print 'Missing or invalid clock frequency'
	    return 0

#read Post Trigger Samples and check for consistency
	try:
	    pts = TreeNode(baseNid + self.N_PTS).data()
	except:
	    print 'Missing or invalid Post Trigger Samples'
	    return 0

#all required configuation collected. Call external routine initialize passing the right parameters
#we use ctypes functions to convert python variable to appropriate C types to be passed to the external routine
	deviceLib.initialize(c_char_p(name), c_int(clockMode), c_int(pts))
#return success
	return 1


##################################STORE################################################
#store method, called to get samples from the ADC and to store waveforms in the tree
    def store(self,arg):
#import required symbols from MDSSplus and ctypes packages
      	from MDSplus import Tree, TreeNode, Int16Array, Float64Array, Int32, Int64, Float32, Float64, Signal, Data, Dimension, Window, Range
      	from ctypes import CDLL, c_char_p, c_short, byref

#instantiate library object
    	try:
      	    deviceLib = CDLL("libDemoAdcShr.so")
	except:
	    print 'Cannot link to device library'
	    return 0

#the node field is a TreeNode object. We get its integer nid (node identifier) via method getNid()
	baseNid = self.node.getNid()
#we will add offsets to baseNid to identify nodes containing configuration data.

#get name
	try:
   	    name = TreeNode(baseNid + self.N_NAME).data()
#we expect to get a string in name
	except:
	    print 'Missing Name in device'
	    return 0


#instantiate four short arrays with 65536 samples each. They will be passed to the acquire() external routine
	DataArray = c_short * 65536
	rawChan = []
	rawChan.append(DataArray())
	rawChan.append(DataArray())
	rawChan.append(DataArray())
	rawChan.append(DataArray())

	status = deviceLib.acquire(c_char_p(name), byref(rawChan[0]), byref(rawChan[1]), byref(rawChan[2]), byref(rawChan[3]))
	if status == -1:
	    print 'Acquisition Failed'
	    return 0

#at this point the raw signals are contained in rawChan1-4. We must now:
#1) reduce the dimension of the stored array using the start idx and end idx parameters for each channel, which define
#   the number of samples around the trigger which need to be stored in the pulse file (for this purpose the value of 
#   post trigger samples is also required)
#2) build the appropriate timing information
#3) Put all together in a Signal object
#4) store the Signal object in the tree

#read PostTriggerSamples
	try:
	    pts = TreeNode(baseNid + self.N_PTS).data()
	except:
	    print 'Missing or invalid Post Trigger Samples'
	    return 0
#for each channel we read start idx and end idx
	startIdx = []
	endIdx = []
	try :
	    for chan in range(0,4):
		currStartIdx = TreeNode(baseNid + self.N_CHANNEL_0 + chan * self.K_NODES_PER_CHANNEL + self.N_CHAN_START_IDX).data()
		currEndIdx = TreeNode(baseNid + self.N_CHANNEL_0 + chan * self.K_NODES_PER_CHANNEL + self.N_CHAN_END_IDX).data()
		startIdx.append(currStartIdx)
		endIdx.append(currEndIdx)
	except:
	    print 'Cannot read start idx or end idx'
	    return 0
#1)Build reduced arrays based on start idx and end idx for each channel
#recall that a transient recorder stores acquired data in a circular buffer and stops after acquiring 
#PTS samples after the trigger. This means that the sample corresponding to the trigger is at offset PTS samples
#before the end of the acquired sample array.

#the total number of samples returned by routine acquire()
	totSamples = 65536

#we read the time associated with the trigger. It is specified in the TRIG_SOURCE field of the device tree structure.
#it will be required in order to associate the correct time with each acquired sample
	try:
	    trigTime = TreeNode(baseNid + self.N_TRIG_SOURCE).data()
	except:
	    print 'Missing or invalid Post Trigger Samples'
	    return 0
#we need clock frequency as well
	try:
	    clockFreq = TreeNode(baseNid + self.N_CLOCK_FREQ).data()
	    clockPeriod = 1./clockFreq
	except:
	    print 'Missing or invalid clock frequency'
	    return 0
	

#the following steps are performed for each acquired channel 
	reducedRawChans = []
	for chan in range(0,4):
	    actStartIdx = totSamples - pts + startIdx[chan]  #first index of the part of interest of the sample array
	    actEndIdx = totSamples - pts  + endIdx[chan]   #last index of the part of interest of the sample array
#make sure we do not exceed original array limits
	    if actStartIdx < 0:
		actStartIdx = 0
	    if actEndIdx > totSamples:
		actEndIdx = totSamples - 1
#build reshaped array
	    reducedRawChan = rawChan[chan][actStartIdx:actEndIdx] 
	    

#2)Build timing information. For this purpose we use a  MDSplus "Dimension" object which contains two fields:
# "Window" and "Axis". Window object defines the start and end index of the associated data array and the time which is
# associated with the sample at index 0. Several possible combination of start and end indexes are possible (the can also be
#negative numbers). We adopt here the following convention: consider index 0 as the index of the sample corresponding
#to the trigger, and therefore associated with the trigger time. From the way we have built the reduced raw sample array, 
#it turns out that the start idx and end idx defined
#in the Window object are the same of the start and end indexes defined in the device configuration.
#
#The "Range" object describes a (possibly multispeed or busrt) clock. Its fields specify the clock period, the start and end time 
#for that clock frequency. In our case we need to describe a continuous single speed clock, so there is no need to 
#specify start and end times(it is a continuous, single speed clock).
#
#build the Dimension object in a single call
	    dim = Dimension(Window(startIdx[chan], endIdx[chan], trigTime), Range(None, None, clockPeriod))


#3) Put all togenther in a "Signal" object. MDSplus Signal objects define three fields: samples, raw samples, dimension
#   raw samples are contained in reducedRawChan. The computation required to convert the raw 16 bit sample into a +-10V
#   value is: sample = 10.*rawSample/32768. We may compute a new float array containing such data and store it together
#   with the raw sample (in the case we would like to reain also raw data. There is however a better way to do it 
#   by storing only the required information, i.e. the raw(16 bit) samples and the definition of the expression which
#   converts raw data into actual voltage levels. Therefore, the first field of the Signal object will contain only the
#   definition of an expression, which refers to the raw samples (the second field) of the same Signal object.
#   The MDSplus syntax for this conversion is:  10.*$VALUE/32768.
#   We shall use Data method compile() to build the MDSplus internal representation of this expression, and the stick it
#   as the first field of the Signal object
  	    convExpr = Data.compile("10.* $VALUE/32768.")

#use MDSplus Int16Array object to vest the short array reducedRawChan into the appropriate MDSplus type
	    rawMdsData = Int16Array(reducedRawChan)

#every MDSplus data type can have units associated with it
	    rawMdsData.setUnits("Count")
	    convExpr.setUnits("Volt")

#build the signal object
	    signal = Signal(convExpr, Int16Array(reducedRawChan), dim)

#write the signal in the tree 
    	    try:
    		TreeNode(baseNid + self.N_CHANNEL_0 + chan * self.K_NODES_PER_CHANNEL + self.N_CHAN_DATA).putData(signal)
            except:
    		print 'Cannot write Signal in the tree' 
 		return 0

#endfor chan in range(0,4):

#return success (odd numbers in MDSplus)
	return 1

