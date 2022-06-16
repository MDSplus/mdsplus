
from MDSplus import mdsExceptions, Device, Data, Event
from threading import Thread
from ctypes import CDLL, c_int, c_double, c_char_p, byref


class RFX_RPDAC(Device):
    parts = [{'path': ':COMMENT', 'type': 'text'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 1000},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':TRIG_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':TRIG_TIME', 'type': 'numeric', 'value': 0},
             {'path': ':CHAN1_X', 'type': 'numeric'},
             {'path': ':CHAN1_Y', 'type': 'numeric'},
             {'path': ':CHAN2_X', 'type': 'numeric'},
             {'path': ':CHAN2_Y', 'type': 'numeric'},
             {'path': ':INIT_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('RP_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
              'options': ('no_write_shot',)},
             {'path': ':TRIG_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('RP_SERVER','INIT',60,None),Method(None,'trigger',head))",
              'options': ('no_write_shot',)},
             {'path': ':STOP_ACTION', 'type': 'action',
              'valueExpr': "Action(Dispatch('RP_SERVER','STORE',50,None),Method(None,'stop',head))",
              'options': ('no_write_shot',)}]

 
    def init(self):
        print('================= RPDAC Init ===============')
        try:
            self.lib = CDLL("libredpitaya.so")
            print('library loaded')
        except:
            print('Cannot load redpitaya.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            if (self.clock_mode.data().upper() == 'INTERNAL'):
                useExtClock = 0
            else:
                useExtClock = 1
        except:
            print('Cannot get clock_mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            if (self.trig_mode.data().upper() == 'INTERNAL'):
                useExtTrigger = 0
            else:
                useExtTrigger = 1
        except:
            print('Cannot get trigger mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
          
        try:
            freq = self.clock_freq.data()
        except:
            print('Cannot get clock frequency')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            trigTime = self.trig_time.data()
        except:
            print('Cannot get trigger time')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            x1 = self.chan1_x.data().astype('float64')
            x1 = x1 - trigTime
            numX1 = len(x1)
        except:
            print('Cannot read Chan 1 X array')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            y1 = self.chan1_y.data().astype('float64')
            numY1 = len(y1)
        except:
            print('Cannot read Chan 1 Y array')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if numX1 != numY1:
            print('Different size for Chan 1 arrays')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            x2 = self.chan2_x.data().astype('float64')
            x2 = x2 - trigTime
            numX2 = len(x2)
        except:
            print('Cannot read Chan 2 X array')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            y2 = self.chan2_y.data().astype('float64')
            numY2 = len(y2)
        except:
            print('Cannot read Chan 2 Y array')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if numX2 != numY2:
            print('Different size for Chan 2 arrays')
            raise mdsExceptions.TclFAILED_ESSENTIAL
         
        self.fd = self.lib.rpdacInit(c_int(useExtClock), c_int(useExtTrigger), 
                                     c_double(freq), 
                                     c_int(numX1), 
                                     (c_double * numX1)(*y1),
                                     (c_double * numX1)(*x1),
                                     c_int(numX2), 
                                     (c_double * numX2)(*y2),
                                     (c_double * numX2)(*x2))
 
        return
      
 
    def trigger(self):
        print('================= RPDAC Trigger ===============')
        try:
            self.lib = CDLL("libredpitaya.so")
            print('library loaded')
        except:
            print('Cannot load redpitaya.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.lib.rpdacTrigger();
        
    def stop(self):
        print('================= RPDAC Trigger ===============')
        try:
            self.lib = CDLL("libredpitaya.so")
            print('library loaded')
        except:
            print('Cannot load redpitaya.so')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        self.lib.rpdacStop();
        
