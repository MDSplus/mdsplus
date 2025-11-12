
import MDSplus 
from ctypes import CDLL, c_int, c_uint32
from time import sleep
try:
    from caen_libs import caendigitizer as dgtz
except:
    pass
    
class CAENV1742(MDSplus.Device):
    """CAEN V1742 32 Channels 12 Bit 5GS/s Digitizer"""
    parts = [
        {'path': ':COMMENT',     'type': 'text'},
        {'path': ':LINK_NUM',    'type': 'numeric',  'value': 0},
        {'path': ':NODE_NUM', 'type': 'numeric', 'value': 0},
        {'path': ':GROUP_1_ENA', 'type': 'text', 'value': 'YES'},
        {'path': ':GROUP_2_ENA', 'type': 'text', 'value': 'YES'},
        {'path': ':GROUP_3_ENA', 'type': 'text', 'value': 'YES'},
        {'path': ':GROUP_4_ENA', 'type': 'text', 'value': 'YES'},
        {'path': ':POST_TRIG', 'type': 'numeric', 'value': 50},
        {'path': ':ACQ_MODE', 'type': 'text', 'value': 'SOFTWARE'},
        {'path': ':FREQUENCY', 'type': 'text', 'value': '5GHz'},
        {'path': ':TRIG_POLAR', 'type': 'text', 'value': 'RISING EDGE'},
        {'path': ':FTRIG_MODE', 'type': 'text', 'value': 'DISABLED'},
        {'path': ':FTRIG_SIG', 'type': 'text', 'value': 'TTL'},
        {'path': ':TRIG_TIME', 'type': 'numeric'},
    ]
    for g in range(32):
        parts.extend([
            {'path': 'CHANNEL_%02d' % (g+1), 'type': 'structure'},
            {'path': 'CHANNEL_%02d:OFFSET' % (g+1), 'type': 'numeric', 'value': 50,  'options': ('no_write_shot',)},
            {'path': 'CHANNEL_%02d:RAW_DATA' % (g+1),  'type': 'signal', 'options': ('no_write_model',)},
        ])
    parts.extend([
        {'path': 'FAST_TRIG_0',  'type': 'signal', 'options': ('no_write_model',)},
        {'path': 'FAST_TRIG_1',  'type': 'signal', 'options': ('no_write_model',)},
    ])
    parts.extend([
        {'path': ':MAX_EVENTS', 'type': 'numeric','value':10},
        {'path': ':INIT_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAEN_SERVER','INIT' ,50,None),Method(None,'INIT' ,head))", 'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAEN_SERVER','STORE',50,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)},
    ])

    devices = {}

    def getDevice(self, linkNum, nodeNum):
        try:
            device = CAENV1742.devices[linkNum*1000+nodeNum]
        except:
            device = dgtz.Device.open(dgtz.ConnectionType.OPTICAL_LINK, linkNum, nodeNum, 0)
            CAENV1742.devices[linkNum*1000+nodeNum] = device
        return device


    def init(self):
        samplingSpeedDict = {'5GHz':dgtz.DRS4Frequency.F_5GHz, '2.5GHz':dgtz.DRS4Frequency.F_2_5GHz, 
            '1GHz': dgtz.DRS4Frequency.F_1GHz, '750MHz':dgtz.DRS4Frequency.F_750MHz}
        try:
            samplingSpeed = samplingSpeedDict[self.getNode('FREQUENCY').data()]
        except:
            print('Invalid frequency')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            nodeNum = self.getNode('NODE_NUM').data()
        except:
            print('Cannot read Node Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            linkNum = self.getNode('LINK_NUM').data()
        except:
            print('Cannot read Link Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        groupEnableMask = int(0)
        try:
            if self.getNode('GROUP_1_ENA').data() == 'YES':
                groupEnableMask |= 0x1
            if self.getNode('GROUP_2_ENA').data() == 'YES':
                groupEnableMask |= 0x2
            if self.getNode('GROUP_3_ENA').data() == 'YES':
                groupEnableMask |= 0x4
            if self.getNode('GROUP_4_ENA').data() == 'YES':
                groupEnableMask |= 0x8
        except:
            print('Invalid definition of group enabled')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            postTrigger = int(self.getNode('POST_TRIG').data())
        except:
            print("Cannot read post trigger pergentage")
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        if postTrigger < 0 or postTrigger > 100:
            print("Invalid post trigger pergentage")
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        acqModeDict = {'SOFTWARE': dgtz.AcqMode.SW_CONTROLLED, 'S_IN':dgtz.AcqMode.S_IN_CONTROLLED, 
            'FIRST_TRIGGER':dgtz.AcqMode.FIRST_TRG_CONTROLLED}   
        try:
            acqMode = acqModeDict[self.getNode('ACQ_MODE').data()]
        except:
            print('Invalid Acquisition mode')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        trigPolarityDict = {'RISING': dgtz.TriggerPolarity.ON_RISING_EDGE, 'FALLING':dgtz.TriggerPolarity.ON_FALLING_EDGE}
        try:
            trigPolarity = trigPolarityDict[self.getNode('TRIG_POLAR').data()]
        except:
            print('Invalid Trigger edge')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        fTrigModeDict = {'DISABLED':dgtz.TriggerMode.DISABLED, 'ENABLED':dgtz.TriggerMode.ACQ_ONLY, 
            'ENABLED&ACQUIRED':dgtz.TriggerMode.ACQ_ONLY}
        fTrigDigitizingDict =  {'DISABLED':dgtz.EnaDis.DISABLE, 'ENABLED':dgtz.EnaDis.DISABLE, 
            'ENABLED&ACQUIRED':dgtz.EnaDis.ENABLE}
        try:
            fTrigMode = fTrigModeDict[self.getNode('FTRIG_MODE').data()]
            fTrigDigitizing = fTrigDigitizingDict[self.getNode('FTRIG_MODE').data()]
        except:
            print('Invalid fats trigger mode')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
    
        fTrigDCOffsetDict = {'ECL':21920, 'NIM':32768, '0/-400mV':32768, '0/-200mV':32768,'BIPOLAR':32768, 'TTL':43008}
        fTrigThresholdDict = {'ECL':26214, 'NIM':20934, '0/-400mV':23574, '0/-200mV':24894,'BIPOLAR':26214, 'TTL':26214}
        try:
            fTrigDCOffset = fTrigDCOffsetDict[self.getNode('FTRIG_SIG').data()]
            fTrigeThreshold = fTrigThresholdDict[self.getNode('FTRIG_SIG').data()]
        except:
            print('Invalid Fats Trigger coupling')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        chanOffsets = []
        for chan in range(32):
            try:
                chanOffsetPerc = int(self.getNode('CHANNEL_%d:OFFSET' % (chan+1)).data())
            except:
                print('Cannot read offset for channel'+str(chan + 1))
                raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            if chanOffsetPerc < 0 or chanOffsetPerc > 100:
                printf('Invalid offset for channel'+str(chan + 1))
                raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
            chanOffsets.append(int(0xffff * chanOffsetPerc/100.))

        try:
            maxEvents = int(self.getNode('MAX_EVENTS').data())
        except:
            printf('Cannot get max number of events')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL


        try:
            device = self.getDevice(linkNum, nodeNum)
        except:
            print('Cannot open device')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        if device.get_info().model_name != 'VX1742B':
            print('Unexpected CAEN Model: '+device.get_info().name)
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
#stop previous acquisition if any   
        device.sw_stop_acquisition()        
        
        device.set_group_enable_mask(groupEnableMask)
        device.set_post_trigger_size(postTrigger)
        device.set_acquisition_mode(acqMode)
        device.set_trigger_polarity(0,trigPolarity)
        device.set_fast_trigger_mode(fTrigMode)
        device.set_fast_trigger_digitizing(fTrigDigitizing)
        device.set_group_fast_trigger_dc_offset(0,fTrigDCOffset)
        device.set_group_fast_trigger_dc_offset(2,fTrigDCOffset)
        device.set_group_fast_trigger_threshold(0,fTrigeThreshold)
        device.set_group_fast_trigger_threshold(2,fTrigeThreshold)
        device.set_drs4_sampling_frequency(samplingSpeed) 
        device.set_max_num_events_blt(maxEvents)

#enable extended group trigger time tag
        val = device.read_register(c_uint32(0x8000))
        newVal = val | (1<<20)
        device.write_register(c_uint32(0x8000), c_uint32(newVal))

        for chan in range(32):
            device.set_channel_dc_offset(chan, chanOffsets[chan])

        device.load_drs4_correction_data(samplingSpeed)
        device.enable_drs4_correction()
        try:
            device.free_readout_buffer()
        except:
            pass #already freed
        device.malloc_readout_buffer() 

        try:
            device.allocate_event()
        except:
            pass #already allocated   

        if acqMode == dgtz.AcqMode.SW_CONTROLLED:
            device.sw_start_acquisition()


    def trigger(self):
        try:
            nodeNum = self.getNode('NODE_NUM').data()
        except:
            print('Cannot read Node Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            linkNum = self.getNode('LINK_NUM').data()
        except:
            print('Cannot read Link Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            device = self.getDevice(linkNum, nodeNum)
        except:
            print('Cannot open device')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        device.send_sw_trigger()

    def store(self):
        try:
            nodeNum = self.getNode('NODE_NUM').data()
        except:
            print('Cannot read Node Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            linkNum = self.getNode('LINK_NUM').data()
        except:
            print('Cannot read Link Number')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            device = self.getDevice(linkNum, nodeNum)
        except:
            print('Cannot open device')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        device.read_data(dgtz.ReadMode.SLAVE_TERMINATED_READOUT_MBLT) 
        numEvents = device.get_num_events()
        print('%d Events acquired'%(numEvents))

        postTriggerPerc = device.get_post_trigger_size()
        preTriggerSamples = int(1024 * (100 - postTriggerPerc)/100.)

        samplingPeriodDict = {dgtz.DRS4Frequency.F_5GHz:200E-12, dgtz.DRS4Frequency.F_2_5GHz:400E-12, 
            dgtz.DRS4Frequency.F_1GHz:1E-9, dgtz.DRS4Frequency.F_750MHz:1.333333333333333E-9}
        samplingPeriod = samplingPeriodDict[device.get_drs4_sampling_frequency()]
        for currChan in range(32):
            self.getNode('CHANNEL_%d.RAW_DATA'%(currChan+1)).deleteData()
        self.getNode('FAST_TRIG_0').deleteData()
        self.getNode('FAST_TRIG_1').deleteData()
        fastTriggerAcquired = (self.getNode('FTRIG_MODE').data() == 'ENABLED&ACQUIRED')
        firstTriggerTime = self.getNode('TRIG_TIME').data()

        for currEvent in range(numEvents):
            evt_info, buffer = device.get_event_info(currEvent)
            evt=device.decode_event(buffer)
            for group in range(4):
                if evt.data_group[group] != None:
                    if group == 0 or group == 1:
                        triggerTime = evt.data_group[0].trigger_time_tag 
                        triggerTime += (evt.data_group[1].trigger_time_tag << 30)
                    else:
                        triggerTime = evt.data_group[2].trigger_time_tag 
                        triggerTime += (evt.data_group[3].trigger_time_tag << 30)

                    triggerTime *= 8.5E-9
                    #print("TriggerTime: "+str(triggerTime))
                    firstSampleTime = firstTriggerTime + triggerTime - preTriggerSamples * samplingPeriod 
                    if evt.data_group[group] != None:
                        for currChan in range(8):
                            currNode = self.getNode('CHANNEL_%d.RAW_DATA'%(group * 8 + currChan+1))
                            segStart = MDSplus.Float64(firstSampleTime)
                            segEnd = MDSplus.Float64(firstSampleTime + 1024 * samplingPeriod)
                            segDim = MDSplus.Range(segStart, segEnd, MDSplus.Float64(samplingPeriod))
                            segData = MDSplus.Float32Array(evt.data_group[group].data_channel[currChan])
                            currNode.makeSegment(segStart, segEnd, segDim, segData)
                        if group == 0 and fastTriggerAcquired:
                            segData =  MDSplus.Float32Array(evt.data_group[group].data_channel[8])
                            self.getNode('FAST_TRIG_0').makeSegment(segStart, segEnd, segDim, segData)
                        if group == 2 and fastTriggerAcquired:
                            segData =  MDSplus.Float32Array(evt.data_group[group].data_channel[8])
                            self.getNode('FAST_TRIG_1').makeSegment(segStart, segEnd, segDim, segData)
                        
        device.sw_stop_acquisition()