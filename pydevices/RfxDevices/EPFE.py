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

import subprocess

from MDSplus import Device


class EPFE(Device):
    parts = [
         {'path': ':COMMENT', 'type': 'text', 'value': 'Electrostatic probes front end control'},
         {'path': ':IP', 'type': 'text', 'value': '192.168.62.55'},
         {'path': ':PORT', 'type': 'numeric', 'value': 1893},
         {'path': ':SYS_BIAS', 'type': 'text', 'value': 'OFF'},
         {'path': ':WD_TIMEOUT', 'type': 'numeric', 'value': 15}
    ]
    for i in range(8):
        parts.extend([
        {'path': '.CH_%02d'%(i + 1), 'type': 'structure'},
        {'path': '.CH_%02d:SOURCE'%(i + 1), 'type': 'text', 'value': 'INT'},
        {'path': '.CH_%02d:IRANGE'%(i + 1), 'type': 'text', 'value': 'LOW'},
        {'path': '.CH_%02d:BIAS'%(i + 1), 'type': 'text', 'value': 'OFF'},
        ])
    del(i)

    parts.append({'path': ':INIT_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('SERVER', 'INIT', 50, None), Method(None, 'INIT', head))", 'options': ('no_write_shot',)})
    parts.append({'path': ':STORE_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('SERVER', 'STORE', 50, None), Method(None, 'STORE', head))", 'options': ('no_write_shot',)})

    def send_cmd(self, ip, port, cmd, debuglevel=1):
        self.dprint(debuglevel, 'Send command: %s', cmd)
        cmd = 'echo %s | nc %s %d'%(cmd, ip, port)
        out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
        stdout, stderr = out.communicate()
        cmd_status = stdout.split(':')[-1].strip()
        self.dprint(debuglevel, 'Received status: %s'%(cmd_status))
        return cmd_status

    def INIT(self):
        ip = self.__getattr__('IP').data()
        port = self.__getattr__('PORT').data()
        sys_bias = self.__getattr__('SYS_BIAS').data()
        cmd = 'SYS:BIAS:%s'%(sys_bias,)
        self.send_cmd(ip, port, cmd)
        wd_timeout = self.__getattr__('WD_TIMEOUT').data()
        cmd = 'SYS:WD_TIMEOUT:%s'%(wd_timeout,)
        self.send_cmd(ip, port, cmd)
        for i in range(8):
            if self.__getattr__('CH_%02d'%(i + 1)).isOn():
                ch_source = self.__getattr__('CH_%02d:SOURCE'%(i + 1)).data()
                cmd = 'CH%d:SOUrce:%s'%(i + 1, ch_source)
                self.send_cmd(ip, port, cmd)
                ch_irange = self.__getattr__('CH_%02d:IRANGE'%(i + 1)).data()
                cmd = 'CH%d:IRANge:%s'%(i + 1, ch_irange)
                self.send_cmd(ip, port, cmd)
                ch_bias = self.__getattr__('CH_%02d:BIAS'%(i + 1)).data()
                cmd = 'CH%d:BIAS:%s'%(i + 1, ch_bias)
                self.send_cmd(ip, port, cmd)

    def STORE(self):
        ip = self.__getattr__('IP').data()
        port = self.__getattr__('PORT').data()
        cmd = 'SYS:BIAS:?'
        cmd_status = self.send_cmd(ip, port, cmd)
        if ('???' != cmd_status) and (cmd_status == self.__getattr__('SYS_BIAS').data()):
            self.__getattr__('SYS_BIAS').putData(cmd_status)
        cmd = 'SYS:WD_TIMEOUT:?'
        cmd_status = self.send_cmd(ip, port, cmd)
        if ('???' != cmd_status) and (cmd_status == self.__getattr__('WD_TIMEOUT').data()):
            self.__getattr__('WD_TIMEOUT').putData(cmd_status)
        for i in range(8):
            if self.__getattr__('CH_%02d'%(i + 1)).isOn():
                cmd = 'CH%d:SOUrce:?'%(i + 1)
                cmd_status = self.send_cmd(ip, port, cmd)
                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:SOURCE'%(i + 1)).data()):
                    self.__getattr__('CH_%02d:SOURCE'%(i + 1)).putData(cmd_status)
                cmd = 'CH%d:IRANge:?'%(i + 1)
                cmd_status = self.send_cmd(ip, port, cmd)
                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:IRANGE'%(i + 1)).data()):
                    self.__getattr__('CH_%02d:IRANGE'%(i + 1)).putData(cmd_status)
                cmd = 'CH%d:BIAS:?'%(i + 1)
                cmd_status = self.send_cmd(ip, port, cmd)
                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:BIAS'%(i + 1)).data()):
                    self.__getattr__('CH_%02d:BIAS'%(i + 1)).putData(cmd_status)
