from MDSplus import TreeNode, Signal, Data, Dimension, Window, Range,Device
from MDSplus import mdsExceptions, Int32, Int16Array, Float64Array, Float64
from ctypes import CDLL, c_int, c_short, c_long, byref, Structure
from time import sleep

class CAENV1740(object):
    """CAEN V1740 64 Channels 12 Bit 65MS/S Digitizer"""
    caenLib = None
    partNames = ['COMMENT','BOARD_ID','VME_ADDRESS','TRIG_MODE','TRIG_SOFT','TRIG_EXT','TRIG_SOURCE','TRIG_INT_TIM','CLOCK_MODE','CLOCK_DIV','CLOCK_SOURCE',
                 'NUM_SEGMENTS','USE_TIME','PTS','START_IDX','END_IDX','START_TIME','END_TIME','CONT_SAMPLES']
    parts =     [{'path':'.'+partName, 'type':'any'} for partName in partNames]
    def Add(self,tree,path):
        print('Add is not implemented for this device')
    def __init__(self,node):
      super(CAENV1740,self).__init__(node)
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
      self.cvV1718 = 0                     # CAEN V1718 USB-VME bridge
      self.cvV2718 = 1                     # V2718 PCI-VME bridge with optical link
      self.cvA2818 = 2                     # PCI board with optical link
      self.cvA2719 = 3                     # Optical link piggy-back
      self.cvA32_S_DATA = 0x0D             # A32 supervisory data access
      self.cvD32 = 0x04                    # D32
      self.cvD64 = 0x08
      self.N_DATA_0= 68
      return

    def init(self):
      if CAENV1740.caenLib is None:
          CAENV1740.caenLib = CDLL("libCAENVME.so")
      handle = c_long(0)
      status = CAENV1740.caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
      if status != 0:
          print('Error initializing CAENVME')
      raise mdsExceptions.TclFAILED_ESSENTIAL

      try:
          baseNid = self.node.getNid()
          boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
          print('BOARD ID: ', boardId)
          vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
          print('VME ADDRESS: ', vmeAddress)
#Module Reset
          data = c_int(0)
          status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0xEF24), byref(data), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
              print('Error resetting V1740 Device')
              CAENV1740.caenLib.CAENVME_End(handle)
              raise mdsExceptions.TclFAILED_ESSENTIAL

#give some time
          sleep(0.01)

#number of segments
          segmentDict = {1:0, 2:1, 4:2, 8:3, 16:4, 32:5, 64:6, 128:7, 256:8, 512:9, 1024:10}
          nSegments=TreeNode(baseNid+self.N_NUM_SEGMENTS).data()
          segmentCode = segmentDict[nSegments]
          status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x800c), byref(c_int(segmentCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
              print('Error writing number of segments')
              CAENV1740.caenLib.CAENVME_End(handle)
              raise mdsExceptions.TclFAILED_ESSENTIAL
#Global Group Configuration
          trigModeDict = {'OVER THRESHOLD':0, 'UNDER THRESHOLD':1}
          trigMode = TreeNode(baseNid + self.N_TRIG_MODE).data()
          trigModeCode = trigModeDict[trigMode]
          conf = trigModeCode << 6
          conf = conf | 0x00000010;
          status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8000), byref(c_int(conf)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
              print('Error writing group configuration')
              CAENV1740.caenLib.CAENVME_End(handle)
              raise mdsExceptions.TclFAILED_ESSENTIAL

#Group configurations
          trigEnableCode = 0
          chanEnableCode = 0
          enabledDict = {'ENABLED':1, 'DISABLED':0}
          for group in range(0,8):
#threshold level
#          threshold = TreeNode(baseNid+self.N_CHANNEL_0 + group * K_NODES_PER_CHANNEL + self.N_CHAN_TRIG_THRESH_LEV).data()
#             status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x1080 + group * 0x100), byref(c_int(threshold)), c_int#(self.cvA32_S_DATA), c_int(self.cvD32))
#              if status != 0:
#              print 'writing threshold level'
#              CAENV1740.caenLib.CAENVME_End(handle)
#              raise mdsExceptions.TclFAILED_ESSENTIAL
#offset
              offset = TreeNode(baseNid+self.N_CHANNEL_0 + group * self.K_NODES_PER_CHANNEL + self.N_CHAN_OFFSET).data()
              if(offset > 1):
                  offset = 1.
              if(offset < -1):
                  offset = -1
              offset = (offset / 1.) * 32767
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x1098 + group * 0x100), byref(c_int(int(offset + 0x08000))), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
              if status != 0:
                  print('Error writing DAC offset')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL
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
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x810C), byref(c_int(trigEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
              if status != 0:
                  print('Error writing trigger configuration')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8120), byref(c_int(chanEnableCode)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
              if status != 0:
                  print('Error writing channel enabling')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL

#Front Panel trigger out setting set TRIG/CLK to TTL
              data = 1
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x811C), byref(c_int(data)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

              try:
                  trigSource = TreeNode(baseNid + self.N_TRIG_SOURCE).data()

#if trigger is expressed as an array, consider only the first element
                  print('Trigger source: ', trigSource)
                  if len(TreeNode(baseNid + self.N_TRIG_SOURCE).getShape()) > 0:
                      trigSource = trigSource[0]
              except:
                  print('Cannot resolve Trigger source')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL

#Clock source
              clockMode = TreeNode(baseNid + self.N_CLOCK_MODE).data()
              if clockMode == 'EXTERNAL':
                  try:
                      clockSource = TreeNode(baseNid + self.N_CLOCK_SOURCE).getData()
                      if Device.debug: print('Clock source: ', clockSource)
                  except:
                      print('Cannot resolve Clock source')
                      CAENV1740.caenLib.CAENVME_End(handle)
                      raise mdsExceptions.TclFAILED_ESSENTIAL
              else:
                  clockSource = Range(None, None, Float64(1/62.5E6))
                  TreeNode(baseNid + self.N_CLOCK_SOURCE).putData(clockSource)

#Post Trigger Samples
              try:
                  pts = TreeNode(baseNid + self.N_PTS).data()
              except:
                  print('Cannot resolve PTS samples')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL
              segmentSize = 196608/nSegments
              if pts > segmentSize:
                  print('PTS Larger than segmentSize')
                  CAENV1740.caenLib.CAENVME_End(handle)
                  raise mdsExceptions.TclFAILED_ESSENTIAL
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8114), byref(c_int(pts)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))

#Time management
              useTime=TreeNode(baseNid+self.N_USE_TIME).data()
              if useTime == 'YES':
                  try:
                      startTime = TreeNode(baseNid+self.N_START_TIME).data()
                      endTime = TreeNode(baseNid+self.N_END_TIME).data()
                  except:
                      print('Cannot Read Start or End time')
                      CAENV1740.caenLib.CAENVME_End(handle)
                      raise mdsExceptions.TclFAILED_ESSENTIAL
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
              status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8100), byref(c_int(4)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
              CAENV1740.caenLib.CAENVME_End(handle)
              return
      except:
          print('Generic Error')
          CAENV1740.caenLib.CAENVME_End(handle)
          raise mdsExceptions.TclFAILED_ESSENTIAL

################################TRIGGER###################################

    def trigger(self):
      if CAENV1740.caenLib is None:
        CAENV1740.caenLib = CDLL("libCAENVME.so")
      handle = c_long(0)
      status = CAENV1740.caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
      if status != 0:
          print('Error initializing CAENVME')
          raise mdsExceptions.TclFAILED_ESSENTIAL
      try:
          baseNid = self.node.getNid()
          boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
          if Device.debug: print('BOARD ID: ', boardId)
          vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
          if Device.debug: print('VME ADDRESS: ', vmeAddress)
  #Module Reset
          status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8108), byref(c_int(0)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
          if status != 0:
              print('Error resetting V1740 Device')
              CAENV1740.caenLib.CAENVME_End(handle)
              raise mdsExceptions.TclFAILED_ESSENTIAL
          CAENV1740.caenLib.CAENVME_End(handle)
          return
      except:
          print('Generic Error')
          CAENV1740.caenLib.CAENVME_End(handle)
          raise mdsExceptions.TclFAILED_ESSENTIAL

####################################STORE###################################

    def store(self):
        if CAENV1740.caenLib is None:
          CAENV1740.caenLib = CDLL("libCAENVME.so")
        handle = c_long(0)
        status = CAENV1740.caenLib.CAENVME_Init(c_int(self.cvV2718), c_int(0), c_int(0), byref(handle))
        if status != 0:
            print('Error initializing CAENVME')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:  # except line 508
            baseNid = self.node.getNid()
            boardId = TreeNode(baseNid + self.N_BOARD_ID).data()
            if Device.debug: print('BOARD ID: ', boardId)
            vmeAddress = TreeNode(baseNid + self.N_VME_ADDRESS).data()
            if Device.debug: print('VME ADDRESS: ', vmeAddress)
            try:
                clock = TreeNode(baseNid + self.N_CLOCK_SOURCE).evaluate()
                dt = clock.getDelta().data()
            except:
                print('Error evaluating clock source')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                trig = TreeNode(baseNid + self.N_TRIG_SOURCE).data()
            except:
                print('Error evaluating trigger source')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                startIdx = TreeNode(baseNid + self.N_START_IDX).data()
                endIdx = TreeNode(baseNid + self.N_END_IDX).data()
            except:
                print('Error evaluating start or end idx')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                pts = TreeNode(baseNid + self.N_PTS).data()
            except:
                print('Error evaluating Post Trigger Samples')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL


        # Stop device
            status = CAENV1740.caenLib.CAENVME_WriteCycle(handle, c_int(vmeAddress + 0x8100), byref(c_int(0)), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
            if status != 0:
                print('Error stopping device')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL
        #need to wait a while
            sleep(0.1)


        # Read number of buffers */
            actSegments = c_int(0)
            status = CAENV1740.caenLib.CAENVME_ReadCycle(handle, c_int(vmeAddress + 0x812C), byref(actSegments), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
            if status != 0:
                print('Error reading number of acquired segments')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL

            if Device.debug: print('Acquired segments: ', actSegments.value)
            if actSegments.value == 0:
                CAENV1740.caenLib.CAENVME_End(handle)
                return


        #Compute Segment Size
            try:
                nSegments = TreeNode(baseNid + self.N_NUM_SEGMENTS).data()
                segmentSamples = 196608/nSegments
                print('Segment samples: ', segmentSamples)
            except:
                print('Error reading max number of segments')
                CAENV1740.caenLib.CAENVME_End(handle)
                raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get Active groups
            groupMask = c_int(0)
            status = CAENV1740.caenLib.CAENVME_ReadCycle(handle, c_int(vmeAddress + 0x8120), byref(groupMask), c_int(self.cvA32_S_DATA), c_int(self.cvD32))
            nActGroups = 0
            groupMask = groupMask.value
            for group in range(0,8):
                if (groupMask & (1 << group)) != 0:
                    nActGroups = nActGroups + 1
            if nActGroups == 0:
                if Device.debug: print('No active groups')
                CAENV1740.caenLib.CAENVME_End(handle)
                return
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
                status = CAENV1740.caenLib.CAENVME_FIFOBLTReadCycle(handle, c_int(vmeAddress), byref(segment), c_int(segmentSize),
                c_int(self.cvA32_S_DATA), c_int(self.cvD64), byref(retLen))
                if status != 0:
                    print('Error reading data segment')
                    CAENV1740.caenLib.CAENVME_End(handle)
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                actSize = 4 * (segment.eventSize & 0x0fffffff)
                if actSize != segmentSize:
                    print('Expected event size different from expected size')
                    CAENV1740.caenLib.CAENVME_End(handle)
                    raise mdsExceptions.TclFAILED_ESSENTIAL
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

            print('DIM: ', dim)
            dim.setUnits("s");
            chan0Nid = baseNid + self.N_CHANNEL_0
            data0Nid = baseNid + self.N_DATA_0
            for group in range(0,8):
                if groupMask & (1 << group):
                    try:
                        offset = TreeNode(chan0Nid + group * self.N_CHAN_DATA + self.N_CHAN_OFFSET).data()
                    except:
                        print('Error evaluating group offset')
                        CAENV1740.caenLib.CAENVME_End(handle)
                        raise mdsExceptions.TclFAILED_ESSENTIAL
                    for chan in range(0,8):
                        raw = Int16Array(channels[group * 8 + chan])
                        raw.setUnits("counts")
                        data = Data.compile("2*($VALUE - 2048)/4096.+$1", offset);
                        data.setUnits("Volts")
                        signal = Signal(data, raw, dim)
                        try:
                            TreeNode(data0Nid + group * 8 + chan).putData(signal)
                        except:
                            print('Cannot write Signal in the tree')
                            CAENV1740.caenLib.CAENVME_End(handle)
                            raise mdsExceptions.TclFAILED_ESSENTIAL
                     #endfor
                 #endif
            #endfor

            CAENV1740.caenLib.CAENVME_End(handle)
            return
        except:  # try line 258
            print('Generic Error')
            CAENV1740.caenLib.CAENVME_End(handle)
            raise mdsExceptions.TclFAILED_ESSENTIAL


