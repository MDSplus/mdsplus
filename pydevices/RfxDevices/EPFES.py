

#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import time
import datetime
import socket
import traceback
import logging
import errno
import os
import re
import inspect
import copy
import threading
import string

from MDSplus import *

from threading import Thread
from threading import Lock

from MDSplus.mdsExceptions import DevBAD_PARAMETER


class EPFES(Device):

    parts = [
        {'path': ':COMMENT', 'type': 'text',
            'value': 'Electrostatic probes front end slave'},
        {'path': ':IP', 'type': 'text', 'value': '192.168.62.55'},
        {'path': ':PORT', 'type': 'numeric', 'value': 1893},
        {'path': ':EVT', 'type': 'text', 'value': 'EP_EVT'},
        {'path': ':T_ON', 'type': 'numeric', 'value': 1.0},
        {'path': ':T_OFF', 'type': 'numeric', 'value': 1.0},
        {'path': ':N_CYCL', 'type': 'numeric', 'value': 1},
        {'path': ':WD_TMT', 'type': 'numeric', 'value': 60.0}
    ]

    for i in range(8):

        parts.append({'path': '.CH_%02d' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.CH_%02d:SOURCE' % (
            i + 1), 'type': 'text', 'value': 'INT'})
        parts.append({'path': '.CH_%02d:IRANGE' % (
            i + 1), 'type': 'text', 'value': 'LOW'})
    del i

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('ELBP_PROBE_SERVER', 'INIT', 50, None), Method(None, 'init', head))", 'options': ('no_write_shot',)})
    parts.append({'path': ':STORE_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('ELBP_PROBE_SERVER', 'POST_PULSE_CHECK', 50, None), Method(None, 'store', head))", 'options': ('no_write_shot',)})

    workers = {}

    class MyEvent(Event):

        class Job(threading.Thread):

            def __init__(self, device):

                threading.Thread.__init__(self, name=event)

                self.doSequenceEvent = threading.Event()
                self.stopSequenceEvent = threading.Event()
                self.exitEvent = threading.Event()
                self.programEvent = threading.Event()
                self.newEvent = threading.Event()

                self.offEvent = threading.Event()
                self.onEvent = threading.Event()

                self.device = device

                self.nCycles = 0
                self.tON = 0
                self.tOFF = 0

                self.local = 0

            def doAllOn(self):

                try:
                    ip = self.device.getNode('IP').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid IP')

                    raise DevBAD_PARAMETER

                try:
                    port = self.device.getNode('PORT').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid PORT')

                    raise DevBAD_PARAMETER

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.settimeout(2.0)

                sock.connect((ip, port))

                cmd = "SYS:BIAS:ON\r\n"

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doAllOn() - " + cmd.rstrip("\r\n"))

                sock.sendall(cmd)

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - Job - doAllOn() - " + sock.recv(50).rstrip("\r\n"))

                sock.close()

            def doAllOff(self):

                try:
                    ip = self.device.getNode('IP').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid IP')

                    raise DevBAD_PARAMETER

                try:
                    port = self.device.getNode('PORT').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid PORT')

                    raise DevBAD_PARAMETER

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.settimeout(2.0)

                sock.connect((ip, port))

                cmd = "SYS:BIAS:OFF\r\n"

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doAllOff() - " + cmd.rstrip("\r\n"))

                sock.sendall(cmd)

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - Job - doAllOff() - " + sock.recv(50).rstrip("\r\n"))

                sock.close()

            def doProgram(self):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram()")

                try:
                    ip = self.device.getNode('IP').data()
                except Exception as ex:
                    Data.execute(
                        'DevLogErr($1, $2)', self.device.getNid(), 'Invalid IP %s' % (str(ex)))

                    raise DevBAD_PARAMETER

                try:
                    port = self.device.getNode('PORT').data()
                except Exception as ex:
                    Data.execute('DevLogErr($1, $2)', self.device.getNid(
                    ), 'Invalid PORT %s' % (str(ex)))

                    raise DevBAD_PARAMETER

                try:
                    self.tOn = self.device.getNode('T_ON').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid T_ON')

                    raise DevBAD_PARAMETER

                try:
                    self.tOff = self.device.getNode('T_OFF').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid T_OFF')

                    raise DevBAD_PARAMETER

                try:
                    self.nCycles = self.device.getNode('N_CYCL').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid N_CYCL')

                    raise DevBAD_PARAMETER

                try:
                    wdTimeout = self.device.getNode('WD_TMT').data()
                except:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Invalid WD_TMT')

                    raise DevBAD_PARAMETER

                ch = []
                i = 0
                for i in range(8):

                    ch.append(self.device.getNode(
                        'CH_%02d:IRANGE' % (i + 1)).data())

                    ch.append(self.device.getNode(
                        'CH_%02d:SOURCE' % (i + 1)).data())

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.settimeout(2.0)

                sock.connect((ip, port))

                cmd = "SYS:WD_TIMEOUT:?\r\n"

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram() - " + cmd.rstrip("\r\n"))

                sock.sendall(cmd)

                rData = sock.recv(50)

                sock.close()

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram() - " + rData.rstrip("\r\n"))

                if "Loc" in rData:

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                           " - Job - doProgram() - Module in local: skipped")

                    self.local = 1

                    return 1

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.settimeout(2.0)

                sock.connect((ip, port))

                cmd = "SYS:WD_TIMEOUT:%s\r\n" % (wdTimeout)

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram() - " + cmd.rstrip("\r\n"))

                sock.sendall(cmd)

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - Job - doProgram() - " + sock.recv(50).rstrip("\r\n"))

                sock.close()

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.settimeout(2.0)

                sock.connect((ip, port))

                cmd = "SYS:WD_TIMEOUT:?\r\n"

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram() - " + cmd.rstrip("\r\n"))

                sock.sendall(cmd)

                rData = sock.recv(50)

                sock.close()

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - doProgram() - " + rData.rstrip("\r\n"))

                rData = float(rData.split(':')[-1].strip())

                if ('???' != rData) and (str(rData) == str(float(self.device.getNode('WD_TMT').data()))):

                    pass

                else:
                    Data.execute('DevLogErr($1, $2)',
                                 self.device.getNid(), 'Command failed ' + cmd)

                    raise mdsExceptions.TclFAILED_ESSENTIAL

                if self.exitEvent.isSet() or self.stopSequenceEvent.isSet():

                    return

                i = 0

                j = 0

                for i in range(8):

                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                    sock.settimeout(2.0)

                    sock.connect((ip, port))

                    cmd = "CH%d:IRANge:%s\r\n" % (i + 1, string.upper(ch[j]))

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - doProgram() - " + cmd.rstrip("\r\n"))

                    sock.sendall(cmd)

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                           " - Job - doProgram() - " + sock.recv(50).rstrip("\r\n"))

                    sock.close()

                    if self.exitEvent.isSet() or self.stopSequenceEvent.isSet():

                        break

                    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                    sock.connect((ip, port))

                    cmd = "CH%d:SOUrce:%s\r\n" % (
                        i + 1, string.upper(ch[j + 1]))

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - doProgram() - " + cmd.rstrip("\r\n"))

                    sock.sendall(cmd)

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                           " - Job - doProgram() - " + sock.recv(50).rstrip("\r\n"))

                    sock.close()

                    j += 2

                    if self.exitEvent.isSet() or self.stopSequenceEvent.isSet():

                        break

            def program(self):

                if not self.programEvent.isSet():

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - program() - Executing")

                    self.programEvent.set()
                    self.newEvent.set()

                else:

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - program() - Already in execution")

            def myStart(self):

                if self.local:

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - myStart() - Module in local: skipped")

                    return

                if not self.doSequenceEvent.isSet():

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - myStart() - Executing")

                    self.doSequenceEvent.set()
                    self.newEvent.set()

                else:

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - myStart() - Already in execution")

            def stop(self):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - stop()")

                self.stopSequenceEvent.set()
                self.onEvent.set()
                self.offEvent.set()
                self.newEvent.set()

            def myExit(self):

                self.local = 0

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                       self.device.getPath() + " - Job - myExit()")

                self.exitEvent.set()
                self.onEvent.set()
                self.offEvent.set()
                self.newEvent.set()

            def run(self):

                while True:

                    now = datetime.datetime.now()

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - run() - Waiting for event...")

                    self.newEvent.wait()

                    now = datetime.datetime.now()

                    n = int(self.nCycles)

                    if self.doSequenceEvent.isSet():

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                               " - Job - run() - doSequenceEvent - Cycles to do: " + str(n))

                    elif self.exitEvent.isSet():

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                               self.device.getPath() + " - Job - run() - exitEvent")

                    elif self.stopSequenceEvent.isSet():

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                               self.device.getPath() + " - Job - run() - stopSequenceEvent")

                    elif self.programEvent.isSet():

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                               self.device.getPath() + " - Job - run() - programEvent")
                    else:
                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                               self.device.getPath() + " - Job - run() - Warning: unknown event")

                    nToDo = n

                    if nToDo == 0:

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                               self.device.getPath() + " - Job - run() - Warning: cycles to do is 0")

                    i = 1

                    while n > 0 and self.doSequenceEvent.isSet() and not self.exitEvent.isSet() and not self.stopSequenceEvent.isSet():

                        now = datetime.datetime.now()

                        print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                               " - Job - run() - Cycle " + str(i) + " of " + str(nToDo))

                        i += 1

                        self.doAllOn()

                        self.offEvent.wait(self.tOn)

                        self.doAllOff()

                        n -= 1

                        self.onEvent.wait(self.tOff)

                    print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                           self.device.getPath() + " - Job - run() - Done")

                    if self.programEvent.isSet() and not self.exitEvent.isSet():

                        self.doProgram()

                    self.newEvent.clear()
                    self.onEvent.clear()
                    self.offEvent.clear()
                    self.doSequenceEvent.clear()
                    self.stopSequenceEvent.clear()
                    self.programEvent.clear()

                    if self.exitEvent.isSet():
                        self.exitEvent.clear()
                        break

        def __init__(self, event, device):

            Event.__init__(self, event)

            self.device = device

            self.nid = device.nid

            self.job = self.Job(device)

            self.job.start()

        def run(self):

            evtName = self.getName()

            evtData = string.lower(Data.getString(self.getData()))

            data = evtData.split(" ")

            if "exit_%s" % (str(self.nid)) == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.myExit()
                self.job.join()
                self.cancel()
            elif "exit" == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.myExit()
                self.job.join()
                self.cancel()
            elif "start_%s" % (str(self.nid)) == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.myStart()
            elif "start" == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.myStart()
            elif "stop_%s" % (str(self.nid)) == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.stop()
            elif "stop" == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.stop()
            elif "init_%s" % (str(self.nid)) == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.program()
            elif "init" == string.lower(data[0]):

                now = datetime.datetime.now()

                print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " + self.device.getPath() +
                       " - MyEvent - run() - Arrived event " + string.lower(data[0]))

                self.job.program()

    def init(self):

        try:
            evtName = self.getNode('EVT').data()
        except:
            Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid EVT')

            raise DevBAD_PARAMETER

        if self.nid in EPFES.workers.keys():

            param = "exit_" + str(self.nid)

            Event.setevent(EPFES.workers[self.nid].getName(), param)

            EPFES.workers[self.nid].join()

            del EPFES.workers[self.nid]

        evt = self.MyEvent(evtName, self.copy())

        EPFES.workers[self.nid] = evt

        param = "init_" + str(self.nid)

        Event.setevent(evtName, param)

        return 1

    def start(self):

        if self.nid in EPFES.workers.keys():

            param = "start_" + str(self.nid)

            Event.setevent(EPFES.workers[self.nid].getName(), param)

        else:

            now = datetime.datetime.now()

            print (now.strftime("%Y-%m-%d %H:%M:%S") + " - " +
                   self.getPath() + " - EPFES - start() - Init not done")

        return 1

    def stop(self):

        if self.nid in EPFES.workers.keys():

            param = "stop_" + str(self.nid)

            Event.setevent(EPFES.workers[self.nid].getName(), param)

        else:

            try:
                evtName = self.getNode('EVT').data()
            except:
                Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid EVT')

                raise DevBAD_PARAMETER

            param = "stop_" + str(self.nid)

            Event.setevent(evtName, param)

        return 1

    def store(self):

        if self.nid in EPFES.workers.keys():

            param = "exit_" + str(self.nid)

            Event.setevent(EPFES.workers[self.nid].getName(), param)

            EPFES.workers[self.nid].join()

            del EPFES.workers[self.nid]

        else:

            try:
                evtName = self.getNode('EVT').data()
            except:
                Data.execute('DevLogErr($1, $2)', self.getNid(), 'Invalid EVT')

                raise DevBAD_PARAMETER

            param = "exit_" + str(self.nid)

            Event.setevent(evtName, param)

        return 1
