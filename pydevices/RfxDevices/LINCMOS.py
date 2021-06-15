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

from MDSplus import mdsExceptions, Device, Data
import socket  # to connect to labview PC


class LINCMOS(Device):
    """LINCMOS NEW"""
    parts = [  # offset nid
        {'path': ':IP_ADDR', 'type': 'text'},  # 1

        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':INT_TIME', 'type': 'numeric', 'value': 350E-3},
        {'path': ':N_FRAMES', 'type': 'numeric', 'value': 20},
        {'path': ':FRAMES', 'type': 'signal', 'valueExpr': 'Data.compile("make_signal(transpose($1),,dim_of($1), [1..2048])", head.frames_raw)', 'options': (
            'no_write_model', 'no_compress_on_put')},
        {'path': ':FRAMES_RAW', 'type': 'signal', 'options': (
            'no_write_model', 'no_compress_on_put')},
        {'path': ':FRAMES_BG', 'type': 'signal', 'options': (
            'no_write_model', 'no_compress_on_put')},
        {'path': ':FRAME0_TIME', 'type': 'numeric', 'value': 0}]

    parts.append({'path': ':INIT_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CAMERA_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CAMERA_SERVER','INIT',50,None),Method(None,'startAcquisition',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CAMERA_SERVER','STORE',50,None),Method(None,'stopAcquisition',head))",
                  'options': ('no_write_shot',)})

    def init(self):
        print("Init spectra: reading data from traverser...")

        # Get IP Address
        try:
            ipAddr = self.ip_addr.data()
            ip, port = ipAddr.split(":")
            port = int(port)
            print ('Server Address = ', ip)
            print ('Server Port = ', str(port))
        except:
            ip = "localhost"
            port = 7500
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'WARNING: localhost mode. Put a valid IP address.')

        # Get Integration Time
        try:
            intTime = str(self.int_time.data())
            print ('Integration Time = ', intTime)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'Invalid integration time parameter value')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get Num. Frames
        try:
            nFrames = str(self.n_frames.data())
            print ('Number of Frames = ', nFrames)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'Invalid Number of Frames parameter value')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Get Shot Number
        try:
            shotNum = str(self.getTree().shot)
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'Cannot Retrieve Shot Number')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        msg = "LIN_CMOS#" + shotNum + "#" + \
            nFrames.zfill(4) + "#" + intTime + "\r\n"
        print ("Message to Send: " + msg)

        # Connect to server
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((ip, port))
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'ERROR: Cannot connect to server')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        # Send msg
        try:
            s.send(msg)
            # responses: LIN_CMOS#ACQ_ACK, LIN_CMOS#ACQ_ERR, LIN_CMOS#ACQ_END
            data = s.recv(16)
            print (data)
            if data == "LIN_CMOS#ACQ_ERR":
                Data.execute('DevLogErr($1,$2)', self.nid,
                             'ERROR: LINCMOS acquisition error')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            s.close()
        except:
            Data.execute('DevLogErr($1,$2)', self.nid,
                         'ERROR: TCP send message fail')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        print('Init LINCMOS end.')
        return 1
