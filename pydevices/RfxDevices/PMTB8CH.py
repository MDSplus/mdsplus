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

from MDSplus import mdsExceptions, Device, Data, version


class PMTB8CH(Device):

     parts = [
          {'path': ':COMMENT', 'type': 'text', 'value': 'Photomultiplier Board 8 CH'},
          {'path': ':IP', 'type': 'text', 'value': '192.168.111.56'},
          {'path': ':PORT', 'type': 'numeric', 'value': 5600}
     ]


     for i in range(8):
          parts.append({'path': '.CH_%02d'%(i + 1), 'type': 'structure'})
          parts.append({'path': '.CH_%02d.RD'%(i + 1), 'type': 'structure'})
          parts.append({'path': '.CH_%02d.RD:ID'%(i + 1), 'type': 'numeric', 'value': -1})
          parts.append({'path': '.CH_%02d.RD:GAIN'%(i + 1), 'type': 'numeric', 'value': 1E10})
          parts.append({'path': '.CH_%02d.RD:REM'%(i + 1), 'type': 'text', 'value': 'UNDEF'})

          parts.append({'path': '.CH_%02d.WR'%(i + 1), 'type': 'structure'})
          parts.append({'path': '.CH_%02d.WR:GAIN'%(i + 1), 'type': 'numeric', 'value': 0})
          parts.append({'path': '.CH_%02d.WR:NOISE_FLTR'%(i + 1), 'type': 'text', 'value': ''})
          parts.append({'path': '.CH_%02d.WR:OPT_FLTR'%(i + 1), 'type': 'text', 'value': ''})


     parts.append({'path': ':INIT_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('SERVER', 'INIT', 50, None), Method(None, 'INIT', head))", 'options': ('no_write_shot',)})






     def limit(self, num, minimum=0, maximum=4095):

          return max(min(num, maximum), minimum)



     def send_message(self, ip, port, message, ntry = 1, delay = 0.1, backoff = 2):

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

                    print traceback.format_exc()

                    if not ntry:

                         raise

                    time.sleep(delay)

                    delay *= backoff

               finally:

                    if sock is not None:

                         sock.close()







     def recvall(self, sock, size, timeout = 2):

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



     def recv_message(self, ip, port, ntry = 1 , delay = 0.1, backoff = 2):
          
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

                    data = self.recvall(sock, 56)

                    break

               except socket.error as e:

                    print traceback.format_exc()

                    if not ntry:

                         raise

                    time.sleep(delay)

                    delay *= backoff

               finally:

                    if sock is not None:

                         sock.close()


          return data




     def deserialize_msg(self, msg):

          q = 2.045 / 4096

          rem_dict = {0: 'NO', 1: 'YES'} 

          assert re.match('^(\#[0-7][01]([0-9]{4})){8}$', msg)

          line = msg.split('#')[1:]
          
          data = []

          for i, e in enumerate(msg.split('#')[1:]):
               gain = q * int(e[2:])

               g = float('{:.3f}'.format(q * int(e[2:])))

               data.append({'GAIN' : g, 'REM' : rem_dict[int(e[1])], 'ID': int(e[0]), 'REM RAW': int(e[1]), 'GAIN RAW': '{0:04d}'.format(int(e[2:])), 'GAIN RAW BIN': '{0:012b}'.format(int(e[2:]))})

          return data









     def INIT(self):

          try:
               ip = self.__getattr__('IP').data()
          except:
               Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP')   

               raise DevBAD_PARAMETER

          try:                
               port = self.__getattr__('PORT').data()
          except:
               Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid PORT')   

               raise DevBAD_PARAMETER
          


          print "\nFIRST READ"

          msg = self.recv_message(ip, port)

          print "RAW: " + msg

          data = self.deserialize_msg(msg)

          for i, item in enumerate(data):
               print('CH%d : '%(i + 1) + 'ID: ' + str(item['ID']) + ' - REM RAW: ' + str(item['REM RAW']) + ' - REM: ' + str(item['REM']) + ' - GAIN RAW: ' + str(item['GAIN RAW']) + ' - GAIN RAW BIN: ' + str(item['GAIN RAW BIN']) + ' - GAIN: ' + str(item['GAIN']))


          for i in range(8):
               try:
                    self.__getattr__('.CH_%02d.RD:ID'%(i + 1)).putData(data[i]['ID'])
                    self.__getattr__('.CH_%02d.RD:REM'%(i + 1)).putData(data[i]['REM'])
               except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write read parameters on channel %d'%(i+1))

                    raise mdsExceptions.TclFAILED_ESSENTIAL


          cmd = ''

          q = 2.045 / 4096

          print "\nWRITE"

          for i in range(8):
               cmd = cmd + '#' + str(data[i]['ID'])

               if data[i]['REM RAW']:
                    try:
                         gain = self.__getattr__('.CH_%02d.WR:GAIN'%(i + 1)).data()
               
                         print('CH%d : '%(i + 1) + 'ID: ' + str(data[i]['ID']) + ' - GAIN: ' + str(gain))

                    except:
                         Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid GAIN to write on channel %d'%(i+1))  

                         raise DevBAD_PARAMETER

                    cmd = cmd + '{0:04d}'.format(self.limit(int(gain / q)))
               else:
                    cmd = cmd + + str(data[i]['GAIN RAW'])


          print "RAW CMD: " + cmd

          self.send_message(ip, port, cmd)

          print "\nWAIT"

	  time.sleep(2)

          print "\nSECOND READ"

          msg = self.recv_message(ip, port)

          print "RAW: " + msg

          data = self.deserialize_msg(msg)

          for i, item in enumerate(data):
               print('CH%d : '%(i + 1) + 'ID: ' + str(item['ID']) + ' - REM RAW: ' + str(item['REM RAW']) + ' - REM: ' + str(item['REM']) + ' - GAIN RAW: ' + str(item['GAIN RAW']) + ' - GAIN RAW BIN: ' + str(item['GAIN RAW BIN']) + ' - GAIN: ' + str(item['GAIN']))

          for i in range(8):
               gain = float('{:.3f}'.format(q * float(data[i]['GAIN RAW'])))

               try:
                    self.__getattr__('.CH_%02d.RD:GAIN'%(i + 1)).putData(gain)
               except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot write gain on channel %d'%(i+1))

                    raise mdsExceptions.TclFAILED_ESSENTIAL


          print "\nCHECK GAINS"

          gains = []

          for i in range(8):
               try:
                    gain = self.__getattr__('.CH_%02d.WR:GAIN'%(i + 1)).data()

                    gains.append('{0:04d}'.format(self.limit(int(gain / q))))
               except:
                    Data.execute('DevLogErr($1, $2)', self.nid, 'Cannot read gain from channel %d'%(i+1))  

                    raise mdsExceptions.TclFAILED_ESSENTIAL



          for i in range(8):
               if data[i]['REM RAW']:
                    if data[i]['GAIN RAW'] != gains[i]:
                         Data.execute('DevLogErr($1, $2)', self.nid, 'Bad gain on channel %d of the board'%(i+1))

                         raise mdsExceptions.TclFAILED_ESSENTIAL

          print "\nGAINS OK"

          return 1




     def READ(self):

          try:
               ip = self.__getattr__('IP').data()
          except:
               Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid IP')   

               raise DevBAD_PARAMETER

          try:                
               port = self.__getattr__('PORT').data()
          except:
               Data.execute('DevLogErr($1, $2)', self.nid, 'Invalid PORT')   

               raise DevBAD_PARAMETER


          msg = self.recv_message(ip, port)

          print "RAW: " + msg

          data = self.deserialize_msg(msg)

          for i, item in enumerate(data):
               print('CH%d : '%(i + 1) + 'ID: ' + str(item['ID']) + ' - REM RAW: ' + str(item['REM RAW']) + ' - REM: ' + str(item['REM']) + ' - GAIN RAW: ' + str(item['GAIN RAW']) + ' - GAIN RAW BIN: ' + str(item['GAIN RAW BIN']) + ' - GAIN: ' + str(item['GAIN']))

          return 1
