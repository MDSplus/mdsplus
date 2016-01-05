import time
import math
import pexpect
import socket
import array
import numpy

import MDSplus
import acqsuper

class UUT :
    def __init__ (self, addr, port, debug=None) :
        self.addr  = addr
        self.port  = port
        self.debug = debug
        self.child = pexpect.spawn('nc %s %s' % (addr, str(port)), timeout=5)

    def send (self, cmd, no_reply=False) :
        self.child.sendline(str(cmd))
        try :
            echo = self.child.readline().strip('\r\n')
            if self.debug : print('%s port %d echo: %s' % (self.addr, self.port, echo))
            reply = self.child.readline().strip('\r\n') if not no_reply else '(none expected)'
            if self.debug : print('%s port %d reply: %s' % (self.addr, self.port, reply))

        except pexpect.TIMEOUT :
            reply = 'error: UUT timeout'
            raise MDSplus.mdsExceptions.DevINV_SETUP

        return reply

class ACQFMC (acqsuper.ACQSUPER) :
    """
    Abstract class covering FMC/ELF D-tAcq device types.

    DONE:
      - PRE_TRIG, POST_TRIG (pre_trig needs debugging)
      - TRIG_EDGE, CLOCK_EDGE
      - TRIG_SRC (must be di0, taken from LEMO)
      - CLOCK_SRC (must be di1, taken from MCLK)
      - CLOCK_FREQ
      - SYS_CLOCK, INT_CLOCK, CLOCK

    TODO:
      - option to actually use module internal clock
      - custom routing for trigger and clock (SYNC/HDMI)
      - test multiple modules per carrier
      - trigger method
      - work over exceptions
      - DAQ_MEM
      - ACTIVE_CHAN
      - CLOCK_DIV
      - BOARD_STATUS from STATUS_PVS (needs EPICS)
    """

    parts_tail = [
      {'path': ':SYS_CLOCK',    'type': 'axis',                                                                                                   'options': ('no_write_model', 'write_once')}, # system internal clock
      {'path': ':INT_CLOCK',    'type': 'axis',                                                                                                   'options': ('no_write_model', 'write_once')}, # module internal clock
      {'path': ':CLOCK',        'type': 'axis',                                                                                                   'options': ('no_write_model', 'write_once')}, # includes all divisors
      {'path': ':DI0',          'type': 'axis',    'value':     0.,                                                                               'options': ('no_write_shot',)},
      {'path': ':DI1',          'type': 'axis',    'valueExpr': 'head.sys_clock',                                                                 'options': ('no_write_shot',)},
      {'path': ':TRIG_SRC',     'type': 'numeric', 'valueExpr': 'head.di0',                                                                       'options': ('no_write_shot',)},
      {'path': ':CLOCK_SRC',    'type': 'numeric', 'valueExpr': 'head.di1',                                                                       'options': ('no_write_shot',)},
      {'path': ':SYS_CTRL',     'type': 'numeric', 'value':     True,                                                                             'options': ('no_write_shot',)},
      {'path': ':SITE',         'type': 'numeric', 'value':     1,                                                                                'options': ('no_write_shot',)},
      {'path': ':STATUS_PVS',   'type': 'text',    'value':     MDSplus.makeArray(['SYS:Z:TEMP', 'SYS:0:TEMP', 'SYS:1:TEMP']),                    'options': ('no_write_shot',)},
      {'path': ':BOARD_STATUS', 'type': 'signal',                                                                                                 'options': ('no_write_model',)},
      {'path': ':UUT_LOG',      'type': 'text',                                                                                                   'options': ('no_write_model',)},
      {'path': ':ACT_INIT',     'type': 'action',  'valueExpr': 'Action(Dispatch("DAQ_SERVER", "INIT",  11, None), Method(None, "INIT",  head))', 'options': ('no_write_shot',)},
      {'path': ':ACT_ARM',      'type': 'action',  'valueExpr': 'Action(Dispatch("DAQ_SERVER", "INIT",  21, None), Method(None, "ARM",   head))', 'options': ('no_write_shot',)},
      {'path': ':ACT_STORE',    'type': 'action',  'valueExpr': 'Action(Dispatch("DAQ_SERVER", "STORE", 41, None), Method(None, "STORE", head))', 'options': ('no_write_shot',)},
    ]

    _state_table   = ['STOP', 'ARM', 'RUN_PRE', 'RUN_POST', 'POSTPROCESS']
    _sys_port      = 4220
    _data_port     = 53000
    _sys_cmds      = ['fpga_version', 'software_version']
    _mod_cmds      = ['MANUFACTURER', 'PART_NUM', 'SERIAL', 'adc_18b', 'data32']
    _arm_delay     = 0.5
    _store_retries = 5

    def syscmd (self, cmd) : print(self._uutcmd(0,                      cmd))
    def modcmd (self, cmd) : print(self._uutcmd(self._get_board_site(), cmd))

    def getstate (self) :
        nums = self._uutcmd(0, 'state').split(' ')
        state = int(nums[0])
        print('state = %d:%s, pre/post/elapsed = %s/%s/%s' % (state, self._state_table[state], nums[1], nums[2], nums[3]))

    def disarm (self) : self._uutcmd(0, 'set_abort')

    def arm (self) :
        uut_sys = UUT(self._get_board_addr(), self._sys_port,                          debug=self._debugging())
        uut_mod = UUT(self._get_board_addr(), self._sys_port + self._get_board_site(), debug=self._debugging())

        self._uutcmd(0, 'set_arm')
        time.sleep(self._arm_delay)
        uut_mod.send('shot=%d' % self.tree.shot, no_reply=True)

        state = int(self._uutcmd(0, 'state').split(' ')[0])
        if state != 1 and state != 2 :
            print('warning: not armed')
            raise MDSplus.mdsExceptions.AcqINITIALIZATION_ERROR

    def init (self) :
        start = time.time()
        print('Beginning initialization via unified command interface -- %s.' % ('system contoller and module' if self.sys_ctrl.data() == 1 else 'module only'))

        config = self._get_config()

        clkdiv = 10**(7 - int(math.log10(config['clock_freq'])))
        mbclk  = int(config['clock_freq'] * clkdiv)

        if mbclk > 33e6 :
            clkdiv /= 4
            mbclk = int(config['clock_freq'] * clkdiv)

        if self._debugging() : print('clkdiv = %d, mbclk = %d' % (clkdiv, mbclk))
        if clkdiv < 1 or mbclk < 4e6 or mbclk > 33e6 :
            print('error: could not find a valid mbclk/clkdiv pair')
            raise MDSplus.mdsExceptions.DevBAD_FREQ

        sys_cmds = [
          'set_abort',
          'SIG:FP:TRG=INPUT',
          'SIG:SRC:TRG:0=EXT',
          'SIG:ZCLK_SRC=INT33M',
          'SYS:CLK:FPMUX=ZCLK',
          'SIG:CLK_MB:SET=%d' % mbclk,
          'SIG:SRC:CLK:1=MCLK',
          'transient PRE=%d POST=%d OSAM=1 SOFT_TRIGGER=0' % (config['pre_trig'] * 1000, config['post_trig'] * 1000),
        ]
        mod_cmds = [
          'trg=%d,%d,%d' % (1, int(config['trig_src'][-1]),  config['trig_edge']  != 'FALLING'),
          'clk=%d,%d,%d' % (1, int(config['clock_src'][-1]), config['clock_edge'] != 'FALLING'),
          'clkdiv=%d'    % clkdiv,
        ]

        log = self._do_init(sys_cmds, mod_cmds)
        print('Initialization via unified command interface succeeded in %g seconds.' % (time.time() - start))

        samples = (config['pre_trig'] + config['post_trig']) * 1000 * config['active_chan']
        print('Estimated memory required: %0.1f Mbytes (out of %d total)' % (samples * 2 / 1048576, config['daq_mem']))

        self.uut_log.record = log
        return 1

    def store (self) :
        start = time.time()
        print('Beginning upload via channel sockets.')

        board_addr = self._get_board_addr()
        board_site = self._get_board_site()
        uut_sys = UUT(board_addr, self._sys_port,              debug=self._debugging())
        uut_mod = UUT(board_addr, self._sys_port + board_site, debug=self._debugging())

        # check if arming was performed in a different shot
        if self.tree.shot != int(uut_mod.send('shot')) :
            raise MDSplus.mdsExceptions.AcqWRONG_SHOT

        # check if device is still armed
        nums = uut_sys.send('state').split(' ')
        state = int(nums[0])
        if state == 1 or state == 2 :
            print('error: device was armed but not triggered')
            raise MDSplus.mdsExceptions.DevNOT_TRIGGERED

        # wait for STOP
        for i in range(self._store_retries) :
            if state == 0 : break
            time.sleep(1)
            nums = uut_sys.send('state').split(' ')
            state = int(nums[0])
        if state != 0 :
            print('warning: device not ready')

        # read samples
        pre_trig  = int(nums[1])
        post_trig = int(nums[2])
        if pre_trig + post_trig == 0 :
            print('error: device was not armed')
            raise MDSplus.mdsExceptions.AcqINITIALIZATION_ERROR

        # read and store clocks
        intclk = float(uut_mod.send('sysclkhz'))
        mbclk  = float(uut_sys.send('SIG:CLK_MB:FREQ').split(' ')[1])
        clk    = float(uut_sys.send('SIG:CLK_S%d:FREQ' % board_site).split(' ')[1])
        try :
            self.int_clock.record = MDSplus.Range(None, None, 1./intclk)
            self.sys_clock.record = MDSplus.Range(None, None, 1./mbclk)
            self.clock.record     = MDSplus.Range(None, None, 1./clk) # TODO: handle module internal clock
        except MDSplus.mdsExceptions.TreeNOOVERWRITE :
            print('warning: retrying store method')

        # compute channel offset due to lower-numbered sites
        offset = 0
        for i in range(1, board_site) :
            offset += int(uut_sys.send('get.site %d NCHAN' % i))
        if self._debugging() : print('channel offset = %d' % offset)

        # read gains and store data
        for n in range(int(uut_mod.send('NCHAN'))) :
            volts = float(uut_mod.send('GAIN:%02d' % (n + 1,)).split(' ')[1].strip('V'))
            if self._debugging() : print('channel %d range = %0.2fV' % (n + 1, volts))
            self._store_channel(n + 1, offset, pre_trig, post_trig, self.clock, [0 - volts, volts])

        print('Upload via channel sockets returned in %g seconds.' % (time.time() - start))
        return 1

    SYSCMD   = syscmd
    MODCMD   = modcmd
    GETSTATE = getstate
    DISARM   = disarm
    ARM      = arm
    INIT     = init
    STORE    = store

    def _uutcmd (self, site, cmd) :
        board_addr = self._get_board_addr()
        uut = UUT(board_addr, self._sys_port + site, self._debugging())
        return uut.send(cmd)

    def _get_board_site (self) :
        site = int(self.site.record)
        if site < 1 or site > 6 :
            print('error: site out of range, should be 1-6')
            raise MDSplus.mdsExceptions.DevINV_SETUP
        return site

    def _do_init (self, sys_cmds, mod_cmds) :

        board_addr = self._get_board_addr()
        board_site = self._get_board_site()
        uut_sys = UUT(board_addr, self._sys_port,              debug=self._debugging())
        uut_mod = UUT(board_addr, self._sys_port + board_site, debug=self._debugging())
        log = ''

        # system info (read-only) and config
        if self.sys_ctrl.data() == 1 :
            for cmd in self._sys_cmds :
                ans = uut_sys.send(cmd)
                log += 'site 0: "%s" -> %s\n' % (cmd, ans)
            for cmd in sys_cmds :
                ans = uut_sys.send(cmd)
                log += 'site 0: "%s" -> %s\n' % (cmd, ans)

        # module info (read-only) and config
        for cmd in self._mod_cmds :
            ans = uut_mod.send(cmd)
            log += 'site %d: "%s" -> %s\n' % (board_site, cmd, ans)
        for cmd in mod_cmds :
            ans = uut_mod.send(cmd, no_reply=True)
            log += 'site %d: "%s" -> %s\n' % (board_site, cmd, ans)

        return log

    def _store_channel (self, chan, chan_offset, pre_trig, post_trig, clock_node, vin) :
        if self._debugging() : print('store_channel(): working on channel %d (+%d)' % (chan, chan_offset))

        chan_node = self.__getattr__('input_%02d' % chan)
        if chan_node.on :
            if self._debugging() : print('store_channel(): channel is on')

            try    : start_idx = max(int(self.__getattr__('input_%02d_startidx' % chan)), 0 - pre_trig)
            except : start_idx = 0 - pre_trig

            try    : end_idx = min(int(self.__getattr__('input_%02d_endidx' % chan)), post_trig - 1)
            except : end_idx = post_trig - 1

            try    : inc = max(int(self.__getattr__('input_%02d_inc' % chan)), 1)
            except : inc = 1

            try :
                buf = self._read_raw_data(chan, chan_offset, pre_trig, start_idx, end_idx, inc)
                dim = MDSplus.Dimension(MDSplus.Window(start_idx/inc, end_idx/inc, self.trig_src), clock_node if inc == 1 else MDSplus.Range(None, None, clock_node.evaluate().getDelta() * inc))
                chan_node.record = MDSplus.Data.compile('BUILD_SIGNAL(BUILD_WITH_UNITS((($1 + ($2 - $1)*($VALUE - -32768)/(32767 - -32768))), "V"), BUILD_WITH_UNITS($3, "Counts"), $4)', vin[0], vin[1], buf, dim)

            except Exception as ex :
                print('error: error processing channel %d (+%d)\n%s' % (chan, chan_offset, str(ex)))

    def _read_raw_data (self, chan, chan_offset, pre_trig, start_idx, end_idx, inc) :
        if self._debugging() : print('read_raw_data(): called with chan=%d (+%d), pre_trig=%d, start_idx=%d, end_idx=%d, inc=%d' % (chan, chan_offset, pre_trig, start_idx, end_idx, inc))

        bytes_to_read = (end_idx + pre_trig + 1) * 2
        if self._debugging() : print('read_raw_data(): want %d bytes' % bytes_to_read)

        sock = socket.create_connection((self._get_board_addr(), self._data_port + chan_offset + chan), timeout=10)
        sock.settimeout(None)
        f = sock.makefile('r', 32768)
        buf = f.read(bytes_to_read)
        f.close()
        sock.close()

        bytes_actually_read = len(buf)
        if self._debugging() : print('read_raw_data(): read %d bytes' % len(buf))
        if bytes_actually_read != bytes_to_read : print('warning: read fewer bytes than expected for channel %d (+%d)' % (chan, chan_offset))

        raw = array.array('h')
        raw.fromstring(buf)
        ans = numpy.ndarray(buffer=raw, dtype=numpy.int16, offset=(pre_trig + start_idx)*2, strides=(inc*2), shape=((end_idx - start_idx + 1)/inc))

        return ans # catch all exceptions in _store_channel()
