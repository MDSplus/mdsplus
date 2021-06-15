#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import time
import socket
import traceback
import logging
import errno
import os
import re
import inspect
import copy
import subprocess

from MDSplus import mdsExceptions, Device, Data, version


class EPFE(Device):

    parts = [
        {'path': ':COMMENT', 'type': 'text',
            'value': 'Electrostatic probes front end control'},
        {'path': ':IP', 'type': 'text', 'value': '192.168.62.55'},
        {'path': ':PORT', 'type': 'numeric', 'value': 1893},
        {'path': ':SYS_BIAS', 'type': 'text', 'value': 'OFF'},
        {'path': ':WD_TIMEOUT', 'type': 'numeric', 'value': 15}
    ]

    for i in range(8):

        parts.append({'path': '.CH_%02d' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.CH_%02d:SOURCE' % (
            i + 1), 'type': 'text', 'value': 'INT'})
        parts.append({'path': '.CH_%02d:IRANGE' % (
            i + 1), 'type': 'text', 'value': 'LOW'})
        parts.append({'path': '.CH_%02d:BIAS' % (
            i + 1), 'type': 'text', 'value': 'OFF'})
    del i

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('SERVER', 'INIT', 50, None), Method(None, 'INIT', head))", 'options': ('no_write_shot',)})
    parts.append({'path': ':STORE_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('SERVER', 'STORE', 50, None), Method(None, 'STORE', head))", 'options': ('no_write_shot',)})

    def send_cmd(self, ip, port, cmd):

        cmd = 'echo %s | nc %s %d' % (cmd, ip, port)

        out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)

        stdout, stderr = out.communicate()

        return stdout.split(':')[-1].strip()

    def INIT(self):

        ip = self.__getattr__('IP').data()

        port = self.__getattr__('PORT').data()

        sys_bias = self.__getattr__('SYS_BIAS').data()

        cmd = 'SYS:BIAS:%s' % (sys_bias)

        cmd_status = self.send_cmd(ip, port, cmd)

        print ('Sent command: %s' % (cmd))

        print ('Received status: %s' % (cmd_status))

        wd_timeout = self.__getattr__('WD_TIMEOUT').data()

        cmd = 'SYS:WD_TIMEOUT:%s' % (wd_timeout)

        cmd_status = self.send_cmd(ip, port, cmd)

        print ('Sent command: %s' % (cmd))

        print ('Received status: %s' % (cmd_status))

        for i in range(8):

            if self.__getattr__('CH_%02d' % (i + 1)).isOn():

                ch_source = self.__getattr__('CH_%02d:SOURCE' % (i + 1)).data()

                cmd = 'CH%d:SOUrce:%s' % (i + 1, ch_source)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

                ch_irange = self.__getattr__('CH_%02d:IRANGE' % (i + 1)).data()

                cmd = 'CH%d:IRANge:%s' % (i + 1, ch_irange)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

                ch_bias = self.__getattr__('CH_%02d:BIAS' % (i + 1)).data()

                cmd = 'CH%d:BIAS:%s' % (i + 1, ch_bias)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

        return 1

    def STORE(self):

        ip = self.__getattr__('IP').data()

        port = self.__getattr__('PORT').data()

        cmd = 'SYS:BIAS:?'

        cmd_status = self.send_cmd(ip, port, cmd)

        print ('Sent command: %s' % (cmd))

        print ('Received status: %s' % (cmd_status))

        if ('???' != cmd_status) and (cmd_status == self.__getattr__('SYS_BIAS').data()):

            self.__getattr__('SYS_BIAS').putData(cmd_status)

        cmd = 'SYS:WD_TIMEOUT:?'

        cmd_status = self.send_cmd(ip, port, cmd)

        print ('Sent command: %s' % (cmd))

        print ('Received status: %s' % (cmd_status))

        if ('???' != cmd_status) and (cmd_status == self.__getattr__('WD_TIMEOUT').data()):

            self.__getattr__('WD_TIMEOUT').putData(cmd_status)

        for i in range(8):

            if self.__getattr__('CH_%02d' % (i + 1)).isOn():

                cmd = 'CH%d:SOUrce:?' % (i + 1)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:SOURCE' % (i + 1)).data()):

                    self.__getattr__('CH_%02d:SOURCE' %
                                     (i + 1)).putData(cmd_status)

                cmd = 'CH%d:IRANge:?' % (i + 1)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:IRANGE' % (i + 1)).data()):

                    self.__getattr__('CH_%02d:IRANGE' %
                                     (i + 1)).putData(cmd_status)

                cmd = 'CH%d:BIAS:?' % (i + 1)

                cmd_status = self.send_cmd(ip, port, cmd)

                print ('Sent command: %s' % (cmd))

                print ('Received status: %s' % (cmd_status))

                if ('???' != cmd_status) and (cmd_status == self.__getattr__('CH_%02d:BIAS' % (i + 1)).data()):

                    self.__getattr__('CH_%02d:BIAS' %
                                     (i + 1)).putData(cmd_status)

        return 1
