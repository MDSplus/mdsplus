# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import time
import math
import pexpect
import socket
import array
import numpy

import MDSplus
def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
acqsuper=_mimport('acqsuper')

class UUT:
    def __init__(self, addr, port, debug=None):
        self.addr  = addr
        self.port  = port
        self.debug = debug
        self.child = pexpect.spawn('nc %s %s' % (addr, str(port)), timeout=5)

    def send(self, cmd, no_reply=False):
        self.child.sendline(str(cmd))
        try:
            echo = self.child.readline().strip('\r\n')
            if self.debug : print(('%s port %d echo: %s' % (self.addr, self.port, echo)))
            reply = self.child.readline().strip('\r\n') if not no_reply else '(none expected)'
            if self.debug : print(('%s port %d reply: %s' % (self.addr, self.port, reply)))
        except pexpect.TIMEOUT as e:
            raise MDSplus.mdsExceptions.DevINV_SETUP(str(e))
        return reply

class ACQFMC(acqsuper.ACQSUPER):
    """
    Abstract class covering FMC/ELF D-tAcq device types

    DONE:
      - PRE_TRIG, POST_TRIG (pre_trig likely to be buggy)
      - TRIG_EDGE, CLOCK_EDGE
      - TRIG_SRC (must be di0, taken from LEMO)
      - CLOCK_SRC (must be di1, taken from MCLK)
      - CLOCK_FREQ
      - SYS_CLOCK, INT_CLOCK, CLOCK

    TODO:
      - option to actually use module internal clock (vs. carrier)
      - custom routing for trigger and clock (SYNC/HDMI)
      - test multiple modules per carrier
      - soft trigger method
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
    _store_retries = 5

    def syscmd(self, cmd): print((self._uutcmd(0,                      cmd)))
    def modcmd(self, cmd): print((self._uutcmd(self._get_board_site(), cmd)))

    def getstate(self):
        nums = self._uutcmd(0, 'state').split(' ')
        state = int(nums[0])
        print(('state = %d:%s, pre/post/elapsed = %s/%s/%s' % (state, self._state_table[state], nums[1], nums[2], nums[3])))

    def disarm(self): self._uutcmd(0, 'set_abort')

    def arm(self):
        uut_sys = UUT(self._get_board_addr(), self._sys_port,                          debug=self._debugging())
        uut_mod = UUT(self._get_board_addr(), self._sys_port + self._get_board_site(), debug=self._debugging())

        uut_mod.send('shot=%d' % (self.tree.shot - 1,), no_reply=True)
        uut_sys.send('set_arm')
        time.sleep(self._arm_delay)

        state = int(uut_sys.send('state').split(' ')[0])
        if state != 1 and state != 2 :
            print(('warning: not armed after %d seconds' % self._arm_delay))

    SYSCMD   = syscmd
    MODCMD   = modcmd
    GETSTATE = getstate
    DISARM   = disarm
    ARM      = arm

    def _uutcmd(self, site, cmd):
        board_addr = self._get_board_addr()
        uut = UUT(board_addr, self._sys_port + site, self._debugging())
        return uut.send(cmd)

    def _get_board_site(self):
        try:
            site = int(self.site.record)
            if site < 1 or site > 6 : raise ValueError
        except:
            raise MDSplus.mdsExceptions.DevINV_SETUP('site config missing or invalid, should be 1-6')
        return site

    def _zclk(self, mbclk_min, mbclk_max, freq):
        if freq > mbclk_max :
            raise MDSplus.mdsExceptions.DevFREQ_TO_HIGH()

        clkdiv = int(math.ceil(mbclk_min/freq))
        mbclk  = int(freq * clkdiv)

        if self._debugging() : print(('zclk(): mbclk = %d, clkdiv = %d' % (mbclk, clkdiv)))
        return (mbclk, clkdiv)

    def _do_init(self, sys_cmds, mod_cmds):
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

    def _pre_store(self):
        board_addr = self._get_board_addr()
        board_site = self._get_board_site()
        uut_sys = UUT(board_addr, self._sys_port,              debug=self._debugging())
        uut_mod = UUT(board_addr, self._sys_port + board_site, debug=self._debugging())

        # check if arming was performed in a different shot
        if self.tree.shot != int(uut_mod.send('shot')) :
            raise MDSplus.mdsExceptions.DevWRONG_SHOT

        # check if device is still armed
        nums = uut_sys.send('state').split(' ')
        state = int(nums[0])
        if state == 1 or state == 2 :
            raise MDSplus.mdsExceptions.DevNOT_TRIGGERED('device was armed but not triggered')

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
            raise MDSplus.mdsExceptions.DevNOT_TRIGGERED('device was not armed')

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
        if self._debugging() : print(('pre_store(): channel offset = %d' % offset))

        return (pre_trig, post_trig, offset)

    def _store_channel(self, chan, chan_offset, pre_trig, post_trig, clock_node, vin):
        if self._debugging() : print(('store_channel(): working on channel %d (+%d)' % (chan, chan_offset)))

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
                raise MDSplus.mdsExceptions.DevNOT_TRIGGERED('channel %d (+%d)\n%s' % (chan, chan_offset, str(ex)))

    def _read_raw_data(self, chan, chan_offset, pre_trig, start_idx, end_idx, inc):
        if self._debugging() : print(('read_raw_data(): called with chan=%d (+%d), pre_trig=%d, start_idx=%d, end_idx=%d, inc=%d' % (chan, chan_offset, pre_trig, start_idx, end_idx, inc)))

        bytes_to_read = (end_idx + pre_trig + 1) * 2
        if self._debugging() : print(('read_raw_data(): want %d bytes' % bytes_to_read))

        sock = socket.create_connection((self._get_board_addr(), self._data_port + chan_offset + chan), timeout=10)
        sock.settimeout(None)
        f = sock.makefile('r', 32768)
        buf = f.read(bytes_to_read)
        f.close()
        sock.close()

        bytes_actually_read = len(buf)
        if self._debugging() : print(('read_raw_data(): read %d bytes' % len(buf)))
        if bytes_actually_read != bytes_to_read : print(('warning: read fewer bytes than expected for channel %d (+%d)' % (chan, chan_offset)))

        raw = array.array('h')
        raw.fromstring(buf)
        ans = numpy.ndarray(buffer=raw, dtype=numpy.int16, offset=(pre_trig + start_idx)*2, strides=(inc*2), shape=((end_idx - start_idx + 1)/inc))

        return ans # catch all exceptions in _store_channel()
