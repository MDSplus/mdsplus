import MDSplus 
import numpy as np
from time import sleep
try:
    from caen_felib import lib, device, error
except:
    pass
    
data = {}
digs = {}
endpoints = {}

class CAEN2740(MDSplus.Device):
    """CAEN 2740 64 Channels 12 Bit 125M/s Digitizer"""
    parts = [
        {'path': ':COMMENT',     'type': 'text'},
        {'path': ':IP',    'type': 'text',  'value': '192.168.0.254'},
        {'path': ':PRE_TRIG', 'type': 'numeric', 'value': 50},
        {'path': ':POST_TRIG', 'type': 'numeric', 'value': 50},
        {'path': ':DECIMATION', 'type': 'numeric', 'value': 50},
        {'path': ':TRIG_TIME', 'type': 'numeric'},
    ]
    for g in range(64):
        parts.extend([
            {'path': 'CHANNEL_%02d' % (g+1), 'type': 'structure'},
            {'path': 'CHANNEL_%02d:OFFSET' % (g+1), 'type': 'numeric', 'value': 50,  'options': ('no_write_shot',)},
            {'path': 'CHANNEL_%02d:ENABLED' % (g+1), 'type': 'text', 'value': 'YES',  'options': ('no_write_shot',)},
            {'path': 'CHANNEL_%02d:DATA' % (g+1),  'type': 'signal', 'options': ('no_write_model',)},
        ])
    parts.extend([
        {'path': ':CLOCK_MODE', 'type': 'text', 'value':'INTERNAL'},
        {'path': ':INIT_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAEN_SERVER','INIT' ,50,None),Method(None,'INIT' ,head))", 'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('CAEN_SERVER','STORE',50,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)},
    ])

    def init(self):
        try:
            ip = self.getNode('IP').data()
        except:
            print('Cannot get device IP')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        DIG2_SCHEME = 'dig2'
        DIG2_AUTHORITY = ip
        DIG2_QUERY = ''
        DIG2_PATH = ''
        DIG2_URI = f'{DIG2_SCHEME}://{DIG2_AUTHORITY}/{DIG2_PATH}?{DIG2_QUERY}'
        try:
            digs[ip] = device.connect(DIG2_URI)
        except:
            print('Cannot connect to '+ip)   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL


        digs[ip].cmd.DISARMACQUISITION()
        try:
            clockMode = self.getNode('CLOCK_MODE').data()
        except:
            print('Cannot get Clock Mode')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            preSamples = self.getNode('PRE_TRIG').data()
        except:
            print('Cannot get Pre Trigger Samples')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        if preSamples < 0:
            print('Pre Trig samples must be nonegative')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            postSamples = self.getNode('POST_TRIG').data()
        except:
            print('Cannot get Post Trigger Samples')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        if preSamples < 0:
            print('Post Trig samples must be nonegative')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            decimation = self.getNode('DECIMATION').data()
        except:
            print('Cannot get Decimation')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        recLen = preSamples + postSamples
        try:
            digs[ip].par.RECORDLENGTHS.value = f'{recLen}'
        except:
            print('Cannot set Record Length to '+ str(reclen))  
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
 
        try:
            digs[ip].par.PRETRIGGERS.value = f'{preSamples}'
        except:
            print('Cannot set Pre Trigger samples to '+ str(preSamples))   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            digs[ip].par.DECIMATIONFACTOR.value = f'{decimation}'
        except:
            print('Cannot set decimation to '+ str(decimation))   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            digs[ip].par.ACQTRIGGERSOURCE.value = 'SWTRG|TRGIN'  # Enable software triggers
        except:
            print('Cannot set trigger source')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            if clockMode == 'INTERNAL':
                digs[ip].par.CLOCKSOURCE.value = 'Internal'
            else:
                digs[ip].par.CLOCKSOURCE.value = 'FPClkIn'
            digs[ip].par.ENCLOCKOUTFP.value = 'True'
        except:
            print('Cannot set clock mode')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        numChans = int(digs[ip].par.NUMCH.value)
        if numChans != 64:
            print('Unexpected numewr of channels: '+str(numChans))
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

    # Configure endpoint
        data_format = [
            {
                'name': 'EVENT_SIZE',
                'type': device.DataType.SIZE_T,
            },
            {
                'name': 'TIMESTAMP',
                'type': device.DataType.U64,
            },
            {
                'name': 'WAVEFORM',
                'type': device.DataType.U16,
                'dim': 2,
                'shape': [numChans, int(recLen)],
            },
            {
                'name': 'WAVEFORM_SIZE',
                'type': device.DataType.SIZE_T,
                'dim': 1,
                'shape': [numChans],
            },
        ]
        decoded_endpoint_path = 'scope'
        endpoints[ip] = digs[ip].endpoint[decoded_endpoint_path]
        data[ip] =  endpoints[ip].set_read_data_format(data_format)
        digs[ip].endpoint.par.ACTIVEENDPOINT.value = decoded_endpoint_path

        for chan in range(numChans):
            offset = self.getNode('CHANNEL_%02d:OFFSET' % (chan+1)).data()
            digs[ip].ch[chan].par.dcoffset.value = f'{offset}'

        # Start acquisition
        digs[ip].cmd.ARMACQUISITION()
        digs[ip].cmd.SWSTARTACQUISITION()

    def trigger(self):
        try:
            ip = self.getNode('IP').data()
        except:
            print('Cannot get device IP')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            digs[ip]
        except:
            print('Cannot retrieve dig: Module likely non initialized')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        digs[ip].cmd.SENDSWTRIGGER()

    def store(self):
        try:
            ip = self.getNode('IP').data()
        except:
            print('Cannot get device IP')   
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
        try:
            digs[ip]
            data[ip]
        except:
            print('Cannot retrieve dig and data: Module likely non initialized')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        # Get reference to data fields
        event_size = data[ip][0].value
        timestamp = data[ip][1].value
        waveform = data[ip][2].value
        waveform_size = data[ip][3].value
        try:
            endpoints[ip].read_data(100, data[ip])
        except error.Error as ex:
            print('Cannot read data')
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL

        try:
            triggerTime = self.getNode('TRIG_TIME').data()
        except:
            print('Warning: trigger time not defined, assumed 0')
            triggerTime = 0
        decimation = self.getNode('DECIMATION').data()
        delta = decimation / 125E6
        preSamples = self.getNode('PRE_TRIG').data()
        postSamples = self.getNode('POST_TRIG').data()

        inputRange = int(digs[ip].par.INPUTRANGE.value)
 
        dim = MDSplus.Dimension(MDSplus.Window(MDSplus.Int32(-preSamples), MDSplus.Int32(postSamples), MDSplus.Float64(triggerTime)), MDSplus.Range(
                None, None, MDSplus.Float64(delta)))
        convExpr = self.getTree().tdiCompile(str(inputRange)+'* $VALUE/32768. - '+str(inputRange/2))
        for chan in range(64):
            enabled = self.getNode('CHANNEL_%02d:ENABLED' % (chan+1)).data()
            if enabled == 'YES':
                dataNode = self.getNode('CHANNEL_%02d:DATA' % (chan+1))
                rawData = MDSplus.Uint16Array(waveform[chan])
                dataSignal = MDSplus.Signal(convExpr, rawData, dim)
                dataNode.putData(dataSignal)
            
        digs[ip].cmd.DISARMACQUISITION()
