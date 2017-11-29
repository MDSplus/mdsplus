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


import sys
import socket
import datetime

import MDSplus

def bnc_connect (host, port) :
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        try :
                remote_ip = socket.gethostbyname(host)
                sock.connect((remote_ip, port))
        except socket.gaierror :
                print(('Hostname %s could not be resolved.' % host))
                sys.exit(1)
        except socket.error :
                print('Socket error')
                sys.exit(1)

        return (sock, remote_ip)

def bnc_talk (sock, msg) :
        try :
                sock.sendall(msg + '\n')

                if '?' not in msg :
                        msg = ':syst:error?'
                        sock.sendall(msg + '\n')

        except socket.error :
                print(('Send %s failed.' % msg))
                sys.exit(1)

        reply = ''
        while True :
                reply += sock.recv(1024)
                if reply.endswith('\n') : break

        return reply.strip('\n')

def bnc_check (ans, all_ok) : return (True, all_ok) if 'No error' in ans else (False, False)

def bnc_report (ok) : return ' . . . OK' if ok else ' . . . Not OK'

class BNC845 (MDSplus.Device) :
        """Control functions for the BNC generator supplied by FZ Juelich, based on BNCCommunication.py by A. Kraemer-Flecken (a.kraemer-flecken@fz-juelich.de).  Can be tested with script sim_bncgen from the MDSplusW7X project."""
        parts=[{'path': ':HOST',         'type': 'TEXT',    'options':('no_write_shot',), 'value': '192.168.0.100'},
               {'path': ':PORT',         'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 18},
               {'path': ':TRIG_SRC',     'type': 'TEXT',    'options':('no_write_shot',), 'value': 'external'},
               {'path': ':TRIG_SLOPE',   'type': 'TEXT',    'options':('no_write_shot',), 'value': 'pos'},
               {'path': ':BLANKING',     'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 0}, # on/off
               {'path': ':POWER',        'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 10}, # dBm
               {'path': ':OFFSET',       'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 30}, # MHz
               {'path': ':SWEEPS',       'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 3},
               {'path': ':STEPS',        'type': 'NUMERIC', 'options':('no_write_shot',), 'value': MDSplus.Float64Array([[5000, 23.2], [10000, 23.4], [15000, 23.6], [20000, 23.8], [25000, 24.0], [30000, 24.2], [35000, 24.4], [40000, 24.6], [45000, 24.8]])}, # ms, GHz
               {'path': ':DEINI_ACTION', 'type': 'ACTION',  'options':('no_write_shot',), 'valueExpr': "Action(Dispatch('MAIN_SERVER', 'DEINIT', 1, None), Method(None, 'off',  head))"},
               {'path': ':INIT_ACTION',  'type': 'ACTION',  'options':('no_write_shot',), 'valueExpr': "Action(Dispatch('MAIN_SERVER', 'INIT',   1, None), Method(None, 'init', head))"},
               {'path': ':INIT_LOG',     'type': 'TEXT',    'options':('no_write_model','write_once')},
               {'path': ':INIT_CONFIG',  'type': 'TEXT',    'options':('no_write_model','write_once')}]

        def init (self, disable_rf=None) :

                #### read config nodes ####

                try :
                        host       = self.host.data()
                        port       = self.port.data()
                        trig_src   = self.trig_src.data()
                        trig_slope = self.trig_slope.data()
                        blanking   = self.blanking.data()
                        power      = self.power.data()
                        offset     = self.offset.data()
                        sweeps     = self.sweeps.data()
                        steps      = self.steps.data()
                except :
                        print('One or more configs is missing or invalid.')
                        return 0

                delay = 0.0 # constant, not confirgurable

                #### build buffers ####

                freq_buf  = ''
                power_buf = ''
                delay_buf = ''
                dwell_buf = ''

                if len(steps.shape) < 2 : steps = steps.reshape(1, 2)

                N = steps.shape[0]
                for i in range(N) :
                        dwell = (steps[i][0] - steps[i-1][0] if i > 0 else steps[i][0]) / 1e3
                        freq  = steps[i][1] * 1e9 / 2.0

                        freq_buf  += (', ' if i > 0 else '') + str(freq)
                        power_buf += (', ' if i > 0 else '') + str(power)
                        delay_buf += (', ' if i > 0 else '') + str(delay)
                        dwell_buf += (', ' if i > 0 else '') + str(dwell)

                #### program BNC ####

                log = str(datetime.datetime.utcnow()) + '\n' # first line of INIT_LOG

                (sock, remote_ip) = bnc_connect(host, port)
                log += 'Connected to %s (%s)\n' % (host, remote_ip)

                ans = bnc_talk(sock, '*TST?')
                if '0' in ans :
                        log += 'All tests passed sucessfully.\n'
                else :
                        log += 'One or more tests failed.'
                        sock.close()
                        self.init_log.putData(log)
                        return 0

                all_ok = True

                ans = bnc_talk(sock, '*IDN?')
                config = 'Identification: ' + ans + '\n' # first line of INIT_CONFIG

                ans = bnc_talk(sock, 'syst:vers?')
                config += 'SCPI version number: ' + ans + '\n'

                ans = bnc_talk(sock, 'freq:mode fix')
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'BNC sweep mode stopped: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, ':list:freq ' + freq_buf)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set frequencies: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, ':list:freq?')
                freq_list = ans.split(',')

                ans = bnc_talk(sock, ':list:power ' + power_buf)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set power: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, ':list:power?')
                power_list = ans.split(',')

                ans = bnc_talk(sock, ':list:delay ' + delay_buf)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set delay time: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, ':list:delay?')
                delay_list = ans.split(',')

                ans = bnc_talk(sock, ':list:dwell ' + dwell_buf)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set dwell time: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, ':list:dwell?')
                dwell_list = ans.split(',')

                if len(freq_list) == N and len(power_list) == N and len(delay_list) == N and len(dwell_list) == N :
                        config += 'Frequency/power/delay/duration settings:\n'
                        for i in range(N) :
                                config += '  Step %d:  %s [GHz]  %s [dBm]  %s [s]  %s [s]\n' % (i, freq_list[i], power_list[i], delay_list[i], dwell_list[i])

                ans = bnc_talk(sock, 'sour:list:count ' + str(sweeps))
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Repetition rate set: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'sour:list:count?')
                config += 'Repetition is set to: ' + ans + '\n'

                ans = bnc_talk(sock, 'sour2:coup on')
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Source 2 coupled to source 1: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'sour2:coup?')
                config += 'Source 2 coupled to source 1: ' + ('Yes' if '1' in ans else 'No') + '\n'

                ans = bnc_talk(sock, 'sour2:coup:offset %0.6fe6' % offset)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set source 2 offset: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'sour2:coup:offset?')
                config += 'Offset of source 2 is set to: ' + ans + '\n'

                ans = bnc_talk(sock, 'trigger:type normal')
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set trigger type: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'trigger:source ' + trig_src)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set trigger source: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'trigger:source?')
                config += 'Trigger is set to: ' + ans + '\n'

                ans = bnc_talk(sock, 'trigger:slope ' + trig_slope)
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set trigger slope: ' + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'trigger:slope?')
                config += 'Triggered slope: ' + ans + '\n'

                ans = bnc_talk(sock, 'output:blanking ' + ('on' if blanking != 0 else 'off'))
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'RF output blanking ' + ('on: ' if blanking != 0 else 'off: ') + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'output:blanking?')
                config += 'RF output blanking: ' + ('Yes' if '1' in ans else 'No') + '\n'

                ans = bnc_talk(sock, 'output:state ' + ('off' if disable_rf != None else 'on'))
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Output ' + ('disabled: ' if disable_rf != None else 'enabled: ') + ans + bnc_report(ok) + '\n'

                ans = bnc_talk(sock, 'freq:mode list')
                (ok, all_ok) = bnc_check(ans, all_ok)
                log += 'Set frequencies to list mode: ' + ans + bnc_report(ok)

                ans = bnc_talk(sock, 'freq:mode?')
                config += 'Frequency mode set to: ' + ans

                sock.close()

                self.init_log.putData(log)
                self.init_config.putData(config)

                return 1 if all_ok else 0

        def off (self, arg) :
                (sock, remote_ip) = bnc_connect(self.host.data(), self.port.data())

                ans = bnc_talk(sock, 'output:state off')
                (ok, all_ok) = bnc_check(ans, True)

                sock.close()
                return ok
