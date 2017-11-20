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

from serial import Serial
from time import sleep
from MDSplus import Device, mdsExceptions

def qc_connect (port) :
    ser = Serial(port, 115200, timeout=1)
    sleep(0.5)
    if not ser.isOpen() :
        raise mdsExceptions.DevCOMM_ERROR('Serial port not open.')
    return ser

def qc_talk (ser, msg) :
    ser.write(msg + '\r\n')
    reply = ser.readline()
    ser.read(1) # clear out extra \x00 character
    return reply.strip('\r\n')

class QC9200 (Device) :
    """Limited control for the Quantum Composers Sapphire 9200 Series Pulse Generator."""
    parts=[{'path': ':PORT',        'type': 'TEXT',    'options':('no_write_shot',), 'value': 'COM3'}, # likely /dev/ttyACM0 on Linux
           {'path': ':TRIG_LEVEL',  'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 2.5},
           {'path': ':TRIG_EDGE',   'type': 'TEXT',    'options':('no_write_shot',), 'value': 'RISING'}, # or FALLING
           {'path': ':PERIOD',      'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 0.1},      # seconds
           {'path': ':N_BURST',     'type': 'NUMERIC', 'options':('no_write_shot',), 'value': 5},
           {'path': ':ACT_INIT',    'type': 'ACTION',  'options':('no_write_shot',), 'valueExpr': "Action(Dispatch('DAQ_SERVER', 'INIT',   1, None), Method(None, 'init',   head))"},
           {'path': ':ACT_DEINIT',  'type': 'ACTION',  'options':('no_write_shot',), 'valueExpr': "Action(Dispatch('DAQ_SERVER', 'DEINIT', 1, None), Method(None, 'disarm', head))"},
           {'path': ':INIT_CONFIG', 'type': 'TEXT',    'options':('no_write_model','write_once')}, # includes "plus" status, *IDN?, list of channels
           {'path': ':ACT_TEST',    'type': 'ACTION',  'options':('no_write_shot','disabled'), 'valueExpr': "Action(Dispatch('DAQ_SERVER', 'START', 1, None), Method(None, 'trigger', head))"},
          ]
    for ch in ('A', 'B', 'C', 'D') :
        parts.append({'path': '.CHANNEL_%s'          % ch, 'type': 'STRUCTURE','options':('no_write_shot',)})
        parts.append({'path': '.CHANNEL_%s:DELAY'    % ch, 'type': 'NUMERIC',  'options':('no_write_shot',), 'value': 0.0})      # seconds
        parts.append({'path': '.CHANNEL_%s:WIDTH'    % ch, 'type': 'NUMERIC',  'options':('no_write_shot',), 'value': 10e-3})    # seconds
        parts.append({'path': '.CHANNEL_%s:LEVEL'    % ch, 'type': 'NUMERIC',  'options':('no_write_shot',), 'value': 5.0})      # Volts
        parts.append({'path': '.CHANNEL_%s:POLARITY' % ch, 'type': 'TEXT',     'options':('no_write_shot',), 'value': 'NORMAL'}) # or INVERTED

    def init (self, arg) :

        #### read config nodes ####

        try :
            port       = self.port.data()
            trig_level = float(self.trig_level.data())
            trig_edge  = self.trig_edge.data().upper()
            period     = float(self.period.data())
            n_burst    = int(self.n_burst.data())

            if trig_level < 0.2 or trig_level > 15.0 or not (trig_edge == 'RISING' or trig_edge == 'FALLING') : raise mdsExceptions.DevBAD_PARAMETER
            if period <= 0.0 or n_burst < 1                                                                   : raise mdsExceptions.DevBAD_PARAMETER

            enable   = []
            delay    = []
            width    = []
            level    = []
            polarity = []

            for ch in ('A', 'B', 'C', 'D') :
                enable.append(     self.getNode('.CHANNEL_%s'          % ch).on)
                delay.append(float(self.getNode('.CHANNEL_%s:DELAY'    % ch).data()))
                width.append(float(self.getNode('.CHANNEL_%s:WIDTH'    % ch).data()))
                level.append(float(self.getNode('.CHANNEL_%s:LEVEL'    % ch).data()))
                polarity.append(   self.getNode('.CHANNEL_%s:POLARITY' % ch).data().upper())

            for n in range(4) :
                if delay[n] < 0.0          or  delay[n] > 1e3            : raise mdsExceptions.DevBAD_PARAMETER
                if width[n] < 10e-9        or  width[n] > 1e3            : raise mdsExceptions.DevBAD_PARAMETER
                if level[n] < 3.3          or  level[n] > 5.0            : raise mdsExceptions.DevBAD_PARAMETER
                if polarity[n] != 'NORMAL' and polarity[n] != 'INVERTED' : raise mdsExceptions.DevBAD_PARAMETER

        except :
            print('One or more configs is missing or invalid.')
            return 0

        #### pre-program ####

        ser = qc_connect(port)

        ans = qc_talk(ser, '*IDN?')
        config = 'Identification: ' + ans + '\n' # first line of INIT_CONFIG

        ans = qc_talk(ser, ':SYST:VERS?')
        config += 'SCPI version: ' + ans + '\n'

        ans = qc_talk(ser, ':SYST:COMM:BLUE:EN?')
        config += 'Bluetooth: ' + ('No' if '?2' in ans else 'Yes') + '\n'

        ans = qc_talk(ser, ':PULS0:COUN:STAT?')
        config += 'Plus features: ' + ('No' if '?2' in ans else 'Yes') + '\n'

        ans = qc_talk(ser, ':INST:CAT?')
        config += 'Channels: ' + ans + '\n'
        N_chan = len(ans.split(',')) - 1 # system timer is also a channel

        ans = qc_talk(ser, '*RCL 0')
        config += 'Load defaults: ' + ('OK' if 'ok' in ans else 'Failed') + '\n'

        #### program user settings ####

        ok =        'ok' in qc_talk(ser, ':PULS0:MOD BURS') # first OK check
        ok = ok and 'ok' in qc_talk(ser, ':PULS0:PER %.9f'     % period)
        ok = ok and 'ok' in qc_talk(ser, ':PULS0:BCO %d'       % n_burst)
        ok = ok and 'ok' in qc_talk(ser, ':PULS0:EXT:MOD TRIG')
        ok = ok and 'ok' in qc_talk(ser, ':PULS0:EXT:LEV %.3f' % trig_level)
        ok = ok and 'ok' in qc_talk(ser, ':PULS0:EXT:EDG %s'   % trig_edge)

        for n in range(N_chan) :
            ok = ok and 'ok' in qc_talk(ser, ':PULS%d:STAT %d'        % (n + 1, enable[n]))
            ok = ok and 'ok' in qc_talk(ser, ':PULS%d:DEL %.9f'       % (n + 1, delay[n]))
            ok = ok and 'ok' in qc_talk(ser, ':PULS%d:WIDT %.9f'      % (n + 1, width[n]))
            ok = ok and 'ok' in qc_talk(ser, ':PULS%d:OUTP:AMPL %.3f' % (n + 1, level[n]))
            ok = ok and 'ok' in qc_talk(ser, ':PULS%d:POL %s'         % (n + 1, polarity[n]))

        config += 'Program settings: ' + ('OK' if ok else 'Failed') + '\n'

        #### read other settings ####

        config += 'Other settings:\n'

        msg = ':SYST:AUT?'
        config += '  ' + msg + ' ' + qc_talk(ser, msg) + '\n'

        for key in ('PCO', 'OCO', 'EXT:POL') :
            msg = ':PULS0:%s?' % key
            config += '  ' + msg + ' ' + qc_talk(ser, msg) + '\n'

        for n in range(N_chan) :
            for key in ('SYNC', 'MUX', 'CMOD', 'BCO', 'PCO', 'OCO', 'WCO', 'CGAT') :
                msg = ':PULS%d:%s?' % (n + 1, key)
                config += '  ' + msg + ' ' + qc_talk(ser, msg) + '\n'

        #### arm system ####

        if ok :
            ans = qc_talk(ser, ':PULS0:STAT 1')
            ok = 'ok' in ans
            config += 'Armed: ' + 'OK' if ok else 'Failed'

        ser.close()
        self.init_config.putData(config)
        return 1 if ok else 0

    def trigger (self, arg) :
        ser = qc_connect(self.port.data())
        ans = qc_talk(ser, '*TRG')
        ser.close()
        return 1 if 'ok' in ans else 0

    def disarm (self, arg) :
        ser = qc_connect(self.port.data())
        ans = qc_talk(ser, ':PULS0:STAT 0')
        ser.close()
        return 1 if 'ok' in ans else 0
