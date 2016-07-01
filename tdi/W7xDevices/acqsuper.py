import os

import MDSplus

class ACQSUPER (MDSplus.Device) :
    """
    Superclass covering ALL D-tAcq device types (hypothetically).
    """

    parts_head = [
      {'path': ':COMMENT',    'type': 'text'},
      {'path': ':HOST',       'type': 'text',    'value': '192.168.0.2', 'options': ('no_write_shot',)}, # hostname or ip address
      {'path': ':DAQ_MEM',    'type': 'numeric', 'value': 512,           'options': ('no_write_shot',)}, # Mbytes
      {'path': ':PRE_TRIG',   'type': 'numeric', 'value': 0,             'options': ('no_write_shot',)}, # 1000s or 1024s
      {'path': ':POST_TRIG',  'type': 'numeric', 'value': 128,           'options': ('no_write_shot',)}, # 1000s or 1024s
      {'path': ':TRIG_EDGE',  'type': 'text',    'value': 'rising',      'options': ('no_write_shot',)},
      {'path': ':CLOCK_EDGE', 'type': 'text',    'value': 'falling',     'options': ('no_write_shot',)},
      {'path': ':CLOCK_DIV',  'type': 'numeric', 'value': 1,             'options': ('no_write_shot',)},
    ]

    _debug = None

    def _debugging (self) :
        if self._debug == None :
            self._debug = os.getenv('DEBUG_DEVICES')
        return self._debug

    def _get_board_addr (self) :
        addr = str(self.host.record)
        if addr == 'None' or len(addr) == 0 :
            print('error: board hostname/ip address record is empty')
            raise MDSplus.mdsExceptions.DevINV_SETUP
        return addr

    def _get_config (self) :
        config = {}
        try :
            key = 'daq_mem'
            config[key] = int(self.daq_mem.data())
            if config[key] < 0 : raise ValueError

            key = 'pre_trig'
            config[key] = int(self.pre_trig.data())
            if config[key] < 0 : raise ValueError

            key = 'post_trig'
            config[key] = int(self.post_trig.data())
            if config[key] < 0 : raise ValueError

            key = 'trig_edge'
            config[key] = str(self.trig_edge.data()).upper()
            if config[key] != 'RISING' and config[key] != 'FALLING' : raise ValueError

            key = 'clock_edge'
            config[key] = str(self.clock_edge.data()).upper()
            if config[key] != 'RISING' and config[key] != 'FALLING' : raise ValueError

            key = 'clock_div'
            config[key] = int(self.clock_div.data())
            if config[key] < 1 : raise ValueError

            key = 'trig_src'
            config[key] = self.trig_src.record.getOriginalPartName().getString()[1:]

            key = 'clock_src'
            config[key] = self.clock_src.record.getOriginalPartName().getString()[1:]

            key = 'clock_freq'
            config[key] = float(self.clock_freq.data())
            if config[key] < 40 : raise ValueError

            key = 'active_chan'
            config[key] = int(self.active_chan.data())

        except :
            print('error: config "%s" is missing or invalid' % key)
            raise MDSplus.mdsExceptions.DevINV_SETUP

        return config
