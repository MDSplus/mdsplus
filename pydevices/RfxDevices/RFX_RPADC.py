
from MDSplus import mdsExceptions, Device, Data, Event
from threading import Thread
from ctypes import CDLL, c_int, c_double, c_char_p


class RFX_RPADC(Device):
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':DECIMATION', 'type': 'numeric', 'value': 125000},
             {'path': ':BUF_SIZE', 'type': 'numeric', 'value': 100000},
             {'path': ':SEG_SIZE', 'type': 'numeric', 'value': 100000},
             {'path': ':TRIGGER', 'type': 'numeric'},
             {'path': ':DURATION', 'type': 'numeric'},  # if <= 0 streaming
             # STREAMING or EVENT_STREAMING or TRIGGER_STREAMING or TRIGGER_SINGLE
             {'path': ':MODE', 'type': 'text', 'value': 'STREAMING'},
             # event generation in respect of EV_LEVEL: UPPER or LOWER
             {'path': ':EV_MODE', 'type': 'text', 'value': 'UPPER'},
             # channel (A or B) used for event detection
             {'path': ':EV_CHANNEL', 'type': 'text', 'value': 'A'},
             {'path': ':EV_LEVEL', 'type': 'numeric'},  # event generation level
             {'path': ':EV_SAMPLES', 'type': 'numeric',
              'value': 2},  # event validation samples
             {'path': ':PRE_SAMPLES', 'type': 'numeric',
              'value': 100},  # pre trigger samples
             {'path': ':POST_SAMPLES', 'type': 'numeric',
              'value': 100},  # post trigger samples
             {'path': ':CLOCK_MODE', 'type': 'text',
                 'value': 'INTERNAL'},  # Clock mode
             {'path': ':EXT_CLOCK', 'type': 'numeric'},  # Ext. Clock
             # if clock_mode == TRIG_EVENT or clock_mode == EXT_EVENT
             {'path': ':TRIG_EVENT', 'type': 'numeric'},
             {'path': ':RANGE_A', 'type': 'numeric', 'value': 1.},  # 1/20 Vpp
             {'path': ':RANGE_B', 'type': 'numeric', 'value': 1.},
             {'path': ':GAIN_A', 'type': 'numeric', 'value': 1.},
             {'path': ':GAIN_B', 'type': 'numeric', 'value': 1.},
             {'path': ':OFFSET_A', 'type': 'numeric', 'value': 0.},
             {'path': ':OFFSET_B', 'type': 'numeric', 'value': 0.},
             {'path': ':EVENT_CODE', 'type': 'numeric', 'value': 0.},
             {'path': ':RAW_A', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':RAW_B', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')},
             {'path': ':CHAN_A', 'type': 'signal'},
             {'path': ':CHAN_B', 'type': 'signal'},
             {'path': ':MDS_TRIG_EV', 'type': 'text'},
             {'path': ':INIT_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
              'options': ('no_write_shot',)},
             {'path': ':START_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
              'options': ('no_write_shot',)},
             {'path': ':STOP_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
              'options': ('no_write_shot',)},
             {'path': ':START_TIME', 'type': 'numeric', 'value': 0},
             {'path': ':ABS_TRIGGER', 'type': 'numeric', 'value': 0},
             {'path': ':DEAD_TIME', 'type': 'numeric', 'value': 1E-3},
             {'path': ':HW_OFFS_A', 'type': 'numeric', 'value': 0},
             {'path': ':HW_OFFS_B', 'type': 'numeric', 'value': 0},
             {'path': ':TRIG_RECV', 'type': 'signal', 'options': (
                 'no_write_model', 'no_compress_on_put')}
             ]

    class TriggerEvent(Event):
        def __init__(self, evName, device):
            Event.__init__(self, evName)
            self.device = device

        def run(self):
            self.device.do_trigger()

    class Configuration:
        def configure(self, lib, fd, name, shot, chanANid, chanBNid, trigRecvNid, triggerNid, startTimeNid, preSamples,
                      postSamples, segmentSamples, frequency, frequency1, single, absTriggerTimeFromFPGA, absTriggerNid):
            self.lib = lib
            self.fd = fd
            self.name = name
            self.shot = shot
            self.chanANid = chanANid
            self.chanBNid = chanBNid
            self.trigRecvNid = trigRecvNid
            self.triggerNid = triggerNid
            self.startTimeNid = startTimeNid
            self.preSamples = preSamples
            self.postSamples = postSamples
            self.segmentSamples = segmentSamples
            self.frequency = frequency
            self.frequency1 = frequency1
            self.single = single
            self.absTriggerTimeFromFPGA = absTriggerTimeFromFPGA
            self.absTriggerNid = absTriggerNid

    class AsynchStore(Thread):
        def configure(self, conf):
            self.lib = conf.lib
            self.fd = conf.fd
            self.name = conf.name
            self.shot = conf.shot
            self.chanANid = conf.chanANid
            self.chanBNid = conf.chanBNid
            self.trigRecvNid = conf.trigRecvNid
            self.triggerNid = conf.triggerNid
            self.startTimeNid = conf.startTimeNid
            self.preSamples = conf.preSamples
            self.postSamples = conf.postSamples
            self.segmentSamples = conf.segmentSamples
            self.frequency = conf.frequency
            self.frequency1 = conf.frequency1
            self.single = conf.single
            self.absTriggerTimeFromFPGA = conf.absTriggerTimeFromFPGA
            self.absTriggerNid = conf.absTriggerNid

        def run(self):
            try:
               self.lib.rpadcStream(
                    c_int(self.fd), c_char_p(self.name), c_int(self.shot), c_int(self.chanANid), c_int(self.chanBNid),c_int(self.trigRecvNid),
                    c_int(self.triggerNid), c_int(self.preSamples), c_int(self.postSamples),
                    c_int(self.segmentSamples), c_double(self.frequency), c_double(self.frequency1), c_int(self.single), c_int(self.absTriggerTimeFromFPGA), c_int(self.absTriggerNid))
            except ValueError as e:
                print(e)
                raise mdsExceptions.TclFAILED_ESSENTIAL

            print('THREAD TERMINATED')

        def stop(self):
            self.stopFlag = c_int(1)

    conf = Configuration()
    lib = None
    fd = 0

    def init(self):
        print('================= RPADC Init ===============')
        if self.lib is None:
            self.lib = CDLL("libredpitaya.so")
            print('library loaded')
        try:
            if (self.ev_channel.data() == 'A'):
                trigFromChanA = 1
            else:
                trigFromChanA = 0
            print('MODE')
            modeDict = {'STREAMING': 0, 'EVENT_STREAMING': 1,
                        'EVENT_SINGLE': 2, 'TRIGGER_STREAMING': 3, 'TRIGGER_SINGLE': 4}
            try:
                mode = modeDict[self.mode.data()]
            except:
                print('Invalid mode: ' + self.mode.data())
                raise mdsExceptions.TclFAILED_ESSENTIAL
            if mode == 2 or mode == 4:  # force segment composted of a single block
                isSingle = 1
            else:
                isSingle = 0
            clockModeDict = {'INTERNAL': 0, 'TRIG_EXTERNAL': 1, 'TRIG_SYNC': 2, 
                             'EXTERNAL': 3, 'SYNC': 4}
            print('CLOCK MODE XXX')
            print(self.clock_mode.data())
            print('ccc')
            try:
                clockMode = clockModeDict[self.clock_mode.data()]
                print(clockMode)
            except:
                print('Invalid clock mode: ' + self.clock_mode.data())
                raise mdsExceptions.TclFAILED_ESSENTIAL
            if clockMode == 4 or clockMode == 2:
                absTriggerTimeFromFPGA = 1
            else:
                absTriggerTimeFromFPGA = 0
            print(self.mode.data)
            if self.mode.data() == 'STREAMING':
                preSamples = 0
                postSamples = 0
            else:
                preSamples = self.pre_samples.data()
                postSamples = self.post_samples.data()
            print('EV_MODE')
            if (self.ev_mode.data() == 'UPPER'):
                trigAboveThreshold = 1
            else:
                trigAboveThreshold = 0
            print('EVENT')
            try:
                evLevel = self.ev_level.data()
                evSamples = self.ev_samples.data()
            except:
                print('Cannot get evLevel/evSamples')
            try:
                decimation = self.decimation.data()
                frequency = 125E6/decimation
                frequency1 = frequency
            except:
                print('Cannot get decimation')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            print('CLOCK')
            if self.clock_mode.data() != 'INTERNAL':
                try:
                    if self.clock_mode.data() == 'TRIG_SYNC':
                        frequency1 = 1E6
                        frequency = 125E6/decimation
                    elif self.clock_mode.data() == 'SYNC':
                        frequency = 1E6 / decimation
                        frequency1 = 1E6
                    elif self.clock_mode.data() == 'TRIG_EXTERNAL':
                        period = Data.execute(
                            'slope_of($)', self.ext_clock)
                        frequency1 = 1./period
                        frequency = 125E6 / decimation
                    else:  #EXTERNAL
                        period = Data.execute(
                            'slope_of($)', self.ext_clock)
                        frequency1 = 1./period
                        frequency = frequency1 / decimation
                except:
                    print('Cannot resolve external clock')
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            segSize = self.seg_size.data()
            try:
                deadTime = self.dead_time.data()
                deadTime = int(deadTime * frequency)
            except:
                print('Cannot resolve dead time')
                raise mdsExceptions.TclFAILED_ESSENTIAL
                
            try:
            	offsa = int(self.hw_offs_a)
            except:
                print('Cannot  read hw_offs_a')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
            	offsb = int(self.hw_offs_b)
            except:
                print('Cannot  read hw_offs_b')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            	
            print('opening device')
            self.fd = self.lib.rpadcInit(c_int(mode), c_int(clockMode), c_int(preSamples), c_int(postSamples), c_int(trigFromChanA),
                                         c_int(trigAboveThreshold), c_int(evLevel), c_int(evSamples), c_int(decimation-1), c_int(deadTime), c_int(offsa), c_int(offsb))
            if self.fd < 0:
                print("Error opening device")
                raise mdsExceptions.TclFAILED_ESSENTIAL
            print('device opened')
            self.conf.configure(self.lib, self.fd, self.getTree().name, self.getTree().shot, self.raw_a.getNid(), self.raw_b.getNid(), self.trig_recv.getNid(),
                self.trigger.getNid(), self.start_time.getNid(), preSamples, postSamples, segSize, frequency,
                frequency1, isSingle, absTriggerTimeFromFPGA, self.abs_trigger.getNid())
            print('configured')
            aChan = self.getTree().tdiCompile(
                '($1*$2/8192.)*$3 + $4', self.raw_a, self.range_a, self.gain_a, self.offset_a)
            print('compilato')
            print(aChan)
            self.chan_a.putData(self.getTree().tdiCompile(
                '($1*$2/8192.)*$3 + $4', self.raw_a, self.range_a, self.gain_a, self.offset_a))
            print('scritto1')
            self.chan_b.putData(self.getTree().tdiCompile(
                '($1*$2/8192.)*$3 + $4', self.raw_b, self.range_b, self.gain_b, self.offset_b))
            print('scritto')
        except Exception as e:
            print(str(e))
            raise mdsExceptions.TclFAILED_ESSENTIAL

    def start_store(self):
        try:
            worker = self.AsynchStore()
            worker.configure(self.conf)
            worker.daemon = True
            worker.start()
            # if MDSplus event defined, connect it to SW trigger
            try:
                evName = self.mds_trig_ev.data()
                RFX_RPADC.triggerEv = RFX_RPADC.TriggerEvent(evName, self)
            except:
                pass
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL


    def stop_store(self):
        try:
            self.conf.lib.rpadcStop(self.conf.fd)
            try:
                RFX_RPADC.triggerEv.cancel()
            except:
                pass
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL

    def do_trigger(self):
        import time
        print('TRIGGER')
        try:
            # self.conf.lib.rpadcTrigger(self.conf.fd, self.getTree().name, self.getTree().shot, self.trigger.getNid())
            self.conf.lib.rpadcTrigger(self.conf.fd)
            self.start_time = int(round(time.time() * 1000))
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
