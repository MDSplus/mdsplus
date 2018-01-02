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

import MDSplus
def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
acqfmc=_mimport('acqfmc')

class ACQ480(acqfmc.ACQFMC):
    """
    D-tAcq ACQ480FMC 8 channel transient recorder
    http://www.d-tacq.com/modproducts.shtml
    """

    parts = []
    parts.extend(acqfmc.ACQFMC.parts_head)
    parts.extend(acqfmc.ACQFMC.parts_tail)
    parts.append({'path': ':CLOCK_FREQ',  'type': 'numeric', 'value': 10000000, 'options': ('no_write_shot',)})
    parts.append({'path': ':ACTIVE_CHAN', 'type': 'numeric', 'value': 8,        'options': ('no_write_shot',)})
    for i in range(8) :
        parts.append({'path': ':INPUT_%02d'          % (i + 1,), 'type': 'signal',  'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%02d:STARTIDX' % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%02d:ENDIDX'   % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%02d:INC'      % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})

    _mod_cmds  = ['MANUFACTURER', 'PART_NUM', 'SERIAL']
    _arm_delay = 5

    def init(self):
        start = time.time()
        print(('Beginning initialization via unified command interface -- %s.' % ('system contoller and module' if self.sys_ctrl.data() == 1 else 'module only')))

        config = self._get_config()
        (mbclk, clkdiv) = self._zclk(4e6, 50e6, config['clock_freq'])

        sys_cmds = [
          'set_abort',
          'SIG:FP:TRG=INPUT',
          'SIG:SRC:TRG:0=EXT',
          'SIG:ZCLK_SRC=INT33M',
          'SYS:CLK:FPMUX=ZCLK',
          'SIG:CLK_MB:SET=%d' % mbclk,
          'SIG:SRC:CLK:1=EXT',
          'transient PRE=%d POST=%d OSAM=0 SOFT_TRIGGER=0' % (config['pre_trig'] * 1000, config['post_trig'] * 1000),
        ]
        mod_cmds = [
          'trg=%d,%d,%d' % (1, int(config['trig_src'][-1]),  config['trig_edge']  != 'FALLING'),
          'clk=%d,%d,%d' % (1, int(config['clock_src'][-1]), config['clock_edge'] != 'FALLING'),
          'clkdiv=%d'    % clkdiv,
        ]

        log = self._do_init(sys_cmds, mod_cmds)
        print(('Initialization via unified command interface succeeded in %0.1f seconds.' % (time.time() - start)))

        samples = (config['pre_trig'] + config['post_trig']) * 1000 * config['active_chan']
        print(('Estimated memory required: %0.1f Mbytes (out of %d total)' % (samples * 2 / 1048576., config['daq_mem'])))

        self.uut_log.record = log

    def store(self):
        start = time.time()
        print('Beginning upload via channel sockets.')

        (pre_trig, post_trig, offset) = self._pre_store()

        for n in range(int(self._uutcmd(self._get_board_site(), 'NCHAN'))) :
            gain = float(self._uutcmd(self._get_board_site(), 'ACQ480:GAIN:%02d' % (n + 1,)).split(' ')[1]) # dB
            volts = 2.5 / 10**(gain/20.) # 2.5V at 0 dB
            if self._debugging() : print(('channel %d range = %0.2fV' % (n + 1, volts)))
            self._store_channel(n + 1, offset, pre_trig, post_trig, self.clock, [0 - volts, volts])

        print(('Upload via channel sockets returned in %0.1f seconds.' % (time.time() - start)))

    INIT  = init
    STORE = store
