#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import time
import socket
import traceback
#import logging
#import errno
import os
import re
#import inspect
import copy

from MDSplus import mdsExceptions, Device, Data, version


class PLFE(Device):

    parts = [
        {'path': ':COMMENT', 'type': 'text',
            'value': 'Plasma light front end control'},
        {'path': ':IP', 'type': 'text', 'value': '192.168.62.56'},
        {'path': ':PORT', 'type': 'numeric', 'value': 2000}
    ]

    for i in range(6):

        parts.append({'path': '.BOARD_%02d' % (i + 1), 'type': 'structure'})

        parts.append(
            {'path': '.BOARD_%02d.CONFIG' % (i + 1), 'type': 'structure'})
        parts.append(
            {'path': '.BOARD_%02d.CONFIG.WR' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.BOARD_%02d.CONFIG.WR:STATUS' % (
            i + 1), 'type': 'numeric', 'value': 0})
        parts.append({'path': '.BOARD_%02d.CONFIG.WR:CONTROL' % (
            i + 1), 'type': 'numeric', 'value': 0})
        parts.append(
            {'path': '.BOARD_%02d.CONFIG.RD' % (i + 1), 'type': 'structure'})
        parts.append({'path': '.BOARD_%02d.CONFIG.RD:IDX' % (
            i + 1), 'type': 'numeric', 'value': -1})
        parts.append({'path': '.BOARD_%02d.CONFIG.RD:STATUS' % (
            i + 1), 'type': 'numeric', 'value': -1})
        parts.append({'path': '.BOARD_%02d.CONFIG.RD:STATUS_TXT' % (
            i + 1), 'type': 'text', 'value': 'UNDEF'})
        parts.append({'path': '.BOARD_%02d.CONFIG.RD:CONTROL' % (
            i + 1), 'type': 'numeric', 'value': -1})
        parts.append({'path': '.BOARD_%02d.CONFIG.RD:CONTROL_TXT' % (
            i + 1), 'type': 'text', 'value': 'UNDEF'})

        parts.append(
            {'path': '.BOARD_%02d.SETUP' % (i + 1), 'type': 'structure'})

        for j in range(2):

            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d' % (
                i + 1, j + 1), 'type': 'structure'})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.WR' % (
                i + 1, j + 1), 'type': 'structure'})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.WR:GAIN' % (
                i + 1, j + 1), 'type': 'numeric', 'value': 1})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.WR:FILTER' % (
                i + 1, j + 1), 'type': 'numeric', 'value': 0})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.WR:TRANS' % (
                i + 1, j + 1), 'type': 'numeric', 'value': 0})

            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD' % (
                i + 1, j + 1), 'type': 'structure'})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD:GAIN' % (
                i + 1, j + 1), 'type': 'numeric', 'value': 1E10})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD:FILTER' % (
                i + 1, j + 1), 'type': 'numeric', 'value': -1})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD:FILTER_TXT' % (
                i + 1, j + 1), 'type': 'text', 'value': 'UNDEF'})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD:TRANS' % (
                i + 1, j + 1), 'type': 'numeric', 'value': -1})
            parts.append({'path': '.BOARD_%02d.SETUP.CHANNEL_%02d.RD:TRANS_TXT' % (
                i + 1, j + 1), 'type': 'text', 'value': 'UNDEF'})

        del j
    del i

    parts.append({'path': ':ADC', 'type': 'numeric'})

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('SERVER', 'INIT', 50, None), Method(None, 'INIT', head))", 'options': ('no_write_shot',)})

    def serialize_msg(self, data):

        msg = ''

        for i, e in enumerate(data):

            msg = msg + '#' + str(e['idx']) + e['setup']

        return msg

    def send_message(self, ip, port, message, ntry=1, delay=0.1, backoff=2):

        while(ntry):

            time.sleep(0.5)

            ntry -= 1

            try:

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.connect((ip, port))

                time.sleep(0.1)

                sock.settimeout(1.0)

                sock.sendall('\x14')

                time.sleep(0.1)

                sock.sendall(message)

                break

            except socket.error as e:

                print (traceback.format_exc())

                if not ntry:

                    raise

                time.sleep(delay)

                delay *= backoff

            finally:

                if sock is not None:

                    sock.close()

    def recvall(self, sock, size, timeout=2):

        data = ''

        sock.settimeout(timeout)

        start_time = time.time()

        while(len(data) < size):

            packet = sock.recv(size - len(data))

            elapsed_time = int(time.time() - start_time)

            if int(timeout) <= elapsed_time:

                raise socket.timeout

            data += packet

        return data

    def recv_message(self, ip, port, ntry=1, delay=0.1, backoff=2):

        data = ''

        while(ntry):

            time.sleep(0.5)

            ntry -= 1

            try:

                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

                sock.connect((ip, port))

                time.sleep(0.1)

                sock.settimeout(1.0)

                sock.sendall('\x1E')

                time.sleep(0.1)

                data = self.recvall(sock, 36)

                break

            except socket.error as e:

                print (traceback.format_exc())

                if not ntry:

                    raise

                time.sleep(delay)

                delay *= backoff

            finally:

                if sock is not None:

                    sock.close()

        return data

    def copyBits(self, source, dest, mask):

        return (dest & ~mask) | (source & mask)

    def deserialize_msg(self, msg):
        assert re.match(
            '^(\#[0-5][01]([01][0-9][0-9]|2[0-4][0-9]|25[0-5])){6}$', msg)

        # assert re.match('^(\#[0-5][01]([01][0-9][0-9]|2[0-5][0-5])){6}$', msg)

        data = []

        for i, e in enumerate(msg.split('#')[1:]):

            data.append({'idx': int(e[0]), 'status': int(e[1]), 'setup': '{0:03d}'.format(
                int(e[2:])), 'bits': '{0:08b}'.format(int(e[2:]))})

        return data

    def INIT(self):

        ip = self.__getattr__('IP').data()

        port = self.__getattr__('PORT').data()

        msg = self.recv_message(ip, port)

        print ("= msg received ==========================================================================================")
        print (msg)
        print ("========================================================================================================\n")

        data = self.deserialize_msg(msg)

        print ("= msg received deserialized ============================================================================")
        print('\n'.join(map(str, data)))
        print ("========================================================================================================\n")

        cmd = ''

        for i, e in enumerate(msg.split('#')[1:]):
            cmd = cmd + '#' + e[0] + '{0:03d}'.format(~int(e[2:]) & 0xFF)

        print ("= msg to send (negate of initial msg) =================================================================")
        print (cmd)
        print ("========================================================================================================\n")

        self.send_message(ip, port, cmd)

        msg = self.recv_message(ip, port)

        print ("= msg received =========================================================================================")
        print (msg)
        print ("========================================================================================================\n")

        data2 = self.deserialize_msg(msg)

        print ("= msg received deserialized ============================================================================")
        print('\n'.join(map(str, data2)))
        print ("========================================================================================================\n")

        status_dict = {0: 'PRESENT', 1: 'ABSENT'}
        control_dict = {0: 'LOCAL', 1: 'REMOTE'}
        for i in range(6):
            self.__getattr__('.BOARD_%02d.CONFIG.RD:IDX' %
                             (i + 1)).putData(data[i]['idx'])
            self.__getattr__('.BOARD_%02d.CONFIG.RD:STATUS' %
                             (i + 1)).putData(data[i]['status'])
            self.__getattr__('.BOARD_%02d.CONFIG.RD:STATUS_TXT' %
                             (i + 1)).putData(status_dict[data[i]['status']])
            if data[i]['status']:
                self.__getattr__('.BOARD_%02d.CONFIG.RD:CONTROL' %
                                 (i + 1)).putData(-1)
                self.__getattr__('.BOARD_%02d.CONFIG.RD:CONTROL_TXT' %
                                 (i + 1)).putData('UNDEF')
            else:
                self.__getattr__('.BOARD_%02d.CONFIG.RD:CONTROL' % (
                    i + 1)).putData(int(data2[i]['setup']) == (~int(data[i]['setup']) & 0xFF))

                if int(data2[i]['setup']) == (~int(data[i]['setup']) & 0xFF):
                    self.__getattr__('.BOARD_%02d.CONFIG.RD:CONTROL_TXT' % (
                        i + 1)).putData('REMOTE')
                else:
                    self.__getattr__('.BOARD_%02d.CONFIG.RD:CONTROL_TXT' % (
                        i + 1)).putData('LOCAL')

#          gain_dict = {1000: 3, 100: 1, 10: 2, 1: 0}
        gain_dict = {1000: 3, 100: 2, 10: 1, 1: 0}
        out = []

        for i in range(6):
            out.append({'idx': self.__getattr__(
                '.BOARD_%02d.CONFIG.RD:IDX' % (i + 1)).data()})

            #print out

            if data[i]['status']:  # anche se locale devo farlo
                out[i]['setup'] = data[i]['setup']
                out[i]['bits'] = data[i]['bits']
            else:
                w = (gain_dict[self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.WR:GAIN' % (i + 1)).data()] << 0) | \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.WR:FILTER' % (i + 1)).data() << 4) | \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.WR:TRANS' % (i + 1)).data() << 5) | \
                    (gain_dict[self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.WR:GAIN' % (i + 1)).data()] << 2) | \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.WR:FILTER' % (i + 1)).data() << 6) | \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.WR:TRANS' %
                                      (i + 1)).data() << 7)

                #print "bb ", w

                w = 0
                w = w + \
                    (gain_dict[self.__getattr__(
                        '.BOARD_%02d.SETUP.CHANNEL_01.WR:GAIN' % (i + 1)).data()] << 0)
                w = w + \
                    (gain_dict[self.__getattr__(
                        '.BOARD_%02d.SETUP.CHANNEL_02.WR:GAIN' % (i + 1)).data()] << 2)
                w = w + \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.WR:FILTER' %
                                      (i + 1)).data() << 4)
                w = w + \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.WR:TRANS' %
                                      (i + 1)).data() << 5)
                w = w + \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.WR:FILTER' %
                                      (i + 1)).data() << 6)
                w = w + \
                    (self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.WR:TRANS' %
                                      (i + 1)).data() << 7)

                #print w

                if not self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01' % (i + 1)).isOn():
                    #print w
                    w = self.copyBits(int(data[i]['setup']), w, 0B00110011)
                    #print w
                if not self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02' % (i + 1)).isOn():
                    #print "aa ", w
                    w = self.copyBits(int(data[i]['setup']), w, 0B11001100)
                    #print w

                out[i]['setup'] = '{0:03d}'.format(w)
                out[i]['bits'] = '{0:08b}'.format(w)

        print ("= msg to send deserialized (derived from jTraverser) ===================================================")
        print('\n'.join(map(str, out)))
        print ("========================================================================================================\n")

        msg = self.serialize_msg(out)

        print ("= msg to send ==========================================================================================")
        print (msg)
        print ("========================================================================================================\n")

        self.send_message(ip, port, msg)

        msg = self.recv_message(ip, port)

        print ("= msg received =========================================================================================")
        print (msg)
        print ("========================================================================================================\n")

        data2 = self.deserialize_msg(msg)

        print ("= msg received deserialized ============================================================================")
        print('\n'.join(map(str, data2)))
        print ("========================================================================================================\n")

        filter_dict = {0: 'OFF', 1: 'ON'}
        trans_dict = {0: 'OFF', 1: 'ON'}
#          gain_dict = {3: 1000, 1: 100, 2: 10, 0: 1}
        gain_dict = {3: 1000, 2: 100, 1: 10, 0: 1}
        for i in range(6):
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.RD:FILTER' %
                             (i + 1)).putData(int(data2[i]['bits'][3]))
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.RD:FILTER_TXT' % (
                i + 1)).putData(filter_dict[int(data2[i]['bits'][3])])
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.RD:TRANS' %
                             (i + 1)).putData(int(data2[i]['bits'][2]))
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.RD:TRANS_TXT' % (
                i + 1)).putData(trans_dict[int(data2[i]['bits'][2])])
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.RD:FILTER' %
                             (i + 1)).putData(int(data2[i]['bits'][1]))
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.RD:FILTER_TXT' % (
                i + 1)).putData(filter_dict[int(data2[i]['bits'][1])])
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.RD:TRANS' %
                             (i + 1)).putData(int(data2[i]['bits'][0]))
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.RD:TRANS_TXT' % (
                i + 1)).putData(trans_dict[int(data2[i]['bits'][0])])

            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_01.RD:GAIN' % (
                i + 1)).putData(gain_dict[int(data2[i]['bits'][6:8], 2)])
            self.__getattr__('.BOARD_%02d.SETUP.CHANNEL_02.RD:GAIN' % (
                i + 1)).putData(gain_dict[int(data2[i]['bits'][4:6], 2)])

        return 1

    def READ(self):

        ip = self.__getattr__('IP').data()

        port = self.__getattr__('PORT').data()

        msg = self.recv_message(ip, port)

        print ("= msg received ==========================================================================================")
        print (msg)
        print ("========================================================================================================\n")

        cmd_final = ''

        for i, e in enumerate(msg.split('#')[1:]):
            cmd_final = cmd_final + '#' + e[0] + '{0:03d}'.format(int(e[2:]))

        print ("= msg to send after read completition (equal to initial msg) ===========================================")
        print (cmd_final)
        print ("========================================================================================================\n")

        data = self.deserialize_msg(msg)

        print ("= msg received deserialized ============================================================================")
        print('\n'.join(map(str, data)))
        print ("========================================================================================================\n")

        cmd = ''

        for i, e in enumerate(msg.split('#')[1:]):
            cmd = cmd + '#' + e[0] + '{0:03d}'.format(~int(e[2:]) & 0xFF)

        print ("= msg to send (negate of initial msg) =================================================================")
        print (cmd)
        print ("========================================================================================================\n")

        self.send_message(ip, port, cmd)

        msg = self.recv_message(ip, port)

        print ("= msg received =========================================================================================")
        print(msg)
        print ("========================================================================================================\n")

        data2 = self.deserialize_msg(msg)

        print ("= msg received deserialized ============================================================================")
        print('\n'.join(map(str, data2)))
        print ("========================================================================================================\n")

        status_dict = {0: 'PRESENT', 1: 'ABSENT'}
        control_dict = {0: 'LOCAL', 1: 'REMOTE'}
        filter_dict = {0: 'OFF', 1: 'ON'}
        trans_dict = {0: 'OFF', 1: 'ON'}
#          gain_dict = {3: 1000, 1: 100, 2: 10, 0: 1}
        gain_dict = {3: 1000, 2: 100, 1: 10, 0: 1}

        for i in range(6):
            print ('BOARD{0:d}'.format(i + 1))
            if data[i]['status']:
                print ('\tStatus:\t\tABSENT')
                print ('\tControl:\t--------')
                print ('\tCH1 gain:\t--------')
                print ('\tCH1 filter:\t--------')
                print ('\tCH1 trans:\t--------')
                print ('\tCH2 gain:\t--------')
                print ('\tCH2 filter:\t--------')
                print ('\tCH2 trans:\t--------')
            else:
                print ('\tStatus:\t\tPRESENT')
                if int(data2[i]['setup']) == (~int(data[i]['setup']) & 0xFF):
                    print ('\tControl:\tREMOTE')
                else:
                    print ('\tControl:\tLOCAL')

                print ('\tCH1 gain:\t{0:d}'.format(
                    gain_dict[int(data[i]['bits'][6:8], 2)]))
                print ('\tCH1 filter:\t{0}'.format(
                    filter_dict[int(data[i]['bits'][3])]))
                print ('\tCH1 trans:\t{0}'.format(
                    trans_dict[int(data[i]['bits'][2])]))
                print ('\tCH2 gain:\t{0:d}'.format(
                    gain_dict[int(data[i]['bits'][4:6], 2)]))
                print ('\tCH2 filter:\t{0}'.format(
                    filter_dict[int(data[i]['bits'][1])]))
                print ('\tCH2 trans:\t{0}'.format(
                    trans_dict[int(data[i]['bits'][0])]))

        self.send_message(ip, port, cmd_final)

        return 1
