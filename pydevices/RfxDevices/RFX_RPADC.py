
from MDSplus import *
from threading import Thread
from ctypes import *


class RFX_RPADC(Device):
    parts=[{'path':':COMMENT', 'type':'text'},
        {'path':':DECIMATION', 'type':'numeric', 'value':125000},
        {'path':':BUF_SIZE', 'type':'numeric', 'value':100000},
        {'path':':SEG_SIZE', 'type':'numeric', 'value':100000},
        {'path':':TRIGGER', 'type':'numeric'},
        {'path':':DURATION', 'type':'numeric'},				#if <= 0 streaming
        {'path':':MODE', 'type':'text','value':'STREAMING'}, 		#STREAMING or EVENT_STREAMING or TRIGGER_STREAMING or TRIGGER_SINGLE
        {'path':':EV_MODE', 'type':'text','value':'UPPER'}, 		#event generation in respect of EV_LEVEL: UPPER or LOWER
        {'path':':EV_CHANNEL', 'type':'text','value':'A'}, 		#channel (A or B) used for event detection
        {'path':':EV_LEVEL', 'type':'numeric'},  			#event generation level
        {'path':':EV_SAMPLES', 'type':'numeric', 'value': 2},  		#event validation samples
        {'path':':PRE_SAMPLES', 'type':'numeric', 'value': 100},  	#pre trigger samples
        {'path':':POST_SAMPLES', 'type':'numeric', 'value': 100},  	#post trigger samples
        {'path':':CLOCK_MODE', 'type':'text', 'value': 'INTERNAL'},  	#Clock mode
        {'path':':EXT_CLOCK', 'type':'numeric'},  			#Ext. Clock
        {'path':':TRIG_EVENT', 'type':'numeric'	},  			#if clock_mode == TRIG_EVENT or clock_mode == EXT_EVENT
        {'path':':CHAN_A', 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  },
        {'path':':CHAN_B', 'type':'signal', 'options':('no_write_model', 'no_compress_on_put')  },
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
         'options':('no_write_shot',)},
        {'path':':START_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('PXI_SERVER','INIT',50,None),Method(None,'start_store',head))",
         'options':('no_write_shot',)},
        {'path':':STOP_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('PXI_SERVER','FINISH_SHOT',50,None),Method(None,'stop_store',head))",
         'options':('no_write_shot',)},
        {'path':':START_TIME', 'type':'numeric', 'value':0}]


    class Configuration:
      def configure(self, lib, fd, name, shot, chanANid, chanBNid, triggerNid, startTimeNid, preSamples, postSamples, segmentSamples, frequency, single):
          self.lib = lib
          self.fd = fd
          self.name = name
          self.shot = shot
          self.chanANid = chanANid
          self.chanBNid = chanBNid
          self.triggerNid = triggerNid
          self.startTimeNid = startTimeNid
          self.preSamples = preSamples
          self.postSamples = postSamples
          self.segmentSamples = segmentSamples
          self.frequency = frequency
          self.single = single

    class AsynchStore(Thread):
      def configure(self, conf):
          self.lib = conf.lib
          self.fd = conf.fd
          self.name = conf.name
          self.shot = conf.shot
          self.chanANid = conf.chanANid
          self.chanBNid = conf.chanBNid
          self.triggerNid = conf.triggerNid
          self.startTimeNid = conf.startTimeNid
          self.preSamples = conf.preSamples
          self.postSamples = conf.postSamples
          self.segmentSamples = conf.segmentSamples
          self.frequency = conf.frequency
          self.single = conf.single


      def run(self):
          print('START THREAD', self.name, self.shot)
          try:
             self.lib.rpadcStream(c_int(self.fd), c_char_p(self.name), c_int(self.shot), c_int(self.chanANid), c_int(self.chanBNid),
                                  c_int(self.triggerNid), c_int(self.preSamples), c_int(self.postSamples),
                                  c_int(self.segmentSamples), c_double(self.frequency), c_int(self.single))
          except ValueError:
               print(ValueError)
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
            modeDict = {'STREAMING': 0, 'EVENT_STREAMING':1, 'EVENT_SINGLE':2, 'TRIGGER_STREAMING':3, 'TRIGGER_SINGLE':4}
            try:
                mode = modeDict[self.mode.data()]
            except:
                print('Invalid mode: ' + self.mode.data())
                return 0
            print('letto mode')
            if mode == 2 or mode == 4 or mode == 3:  #force segment composted of a single block
                isSingle = 1
            else:
                isSingle = 0
            clockModeDict = {'INTERNAL':0,'TRIG_EXTERNAL':1, 'EXTERNAL':2, 'TRIG_EVENT':3, 'EXT_EVENT':4}
            try:
                clockMode = clockModeDict[self.clock_mode.data()]
            except:
                print('Invalid clock mode: ' + self.clock_mode.data())
                return 0
            print('letto clock')
            if self.mode.data() == 'STREAMING':
                preSamples = 0
                postSamples = 0
            else:
                preSamples = self.pre_samples.data()
                postSamples = self.post_samples.data()
            if (self.ev_mode.data() == 'UPPER'):
                trigAboveThreshold = 1
            else:
                trigAboveThreshold = 0
            evLevel = self.ev_level.data()
            evSamples = self.ev_samples.data()
            print('leggo clock_mode')
            if self.clock_mode.data() == 'INTERNAL':
                try:
                    decimation = self.decimation.data()
                    frequency = 125E6/decimation;
                except:
                    print('Cannot get decimation')
                    return 0
            else:
                try:
                    period = Data.execute('slope_of($)', self.ext_clock)
                    frequency = 1./period
                except:
                    print('Cannot resolve external clock')
                    return 0
            segSize = self.seg_size.data()
            print('opening device')
            self.fd = self.lib.rpadcInit(c_int(mode), c_int(clockMode), c_int(preSamples), c_int(postSamples), c_int(trigFromChanA),
                                         c_int(trigAboveThreshold), c_int(evLevel), c_int(evSamples), c_int(decimation))
            if self.fd < 0:
                print("Error opening device")
                return
            print('device opened')
            self.conf.configure(self.lib, self.fd, self.getTree().name, self.getTree().shot, self.chan_a.getNid(), self.chan_b.getNid(),
                                self.trigger.getNid(), self.start_time.getNid(), preSamples, postSamples, segSize, frequency, isSingle)
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return -1

    def start_store(self):
        try:
           worker = self.AsynchStore()
           worker.configure(self.conf)
           worker.daemon = True
           worker.start()
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return -1

    def stop_store(self):
        try:
            self.conf.lib.rpadcStop(self.conf.fd)
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return -1

    def do_trigger(self):
        import time
        print('TRIGGER')
        try:
            # self.conf.lib.rpadcTrigger(self.conf.fd, self.getTree().name, self.getTree().shot, self.trigger.getNid())
            self.conf.lib.rpadcTrigger(self.conf.fd)
            self.start_time = int(round(time.time() * 1000))
        except:
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return -1









