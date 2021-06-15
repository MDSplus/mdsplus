from MDSplus import mdsExceptions, Device, Data, Float32, Signal
from threading import Thread, Condition
import numpy as np
import socket
import time
import re


class LASER_QSMART(Device):
    """Quantel Q-smart 450/850 Pulsed Nd:YAG Laser"""
    parts = [{'path': ':IP_ADDRESS', 'type': 'text'},
             {'path': ':PORT', 'type': 'numeric', 'value': 100001},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':FL_VOLTAGE', 'type': 'numeric', 'value': 650},
             {'path': ':FL_PWIDTH', 'type': 'numeric', 'value': 170},
             {'path': ':FLQS_MODE', 'type': 'text', 'value': "FL-EXT QS-EXT"},
             {'path': ':FL_EXT_FREQ', 'type': 'numeric', 'value': 10},
             {'path': ':QS_PAR1', 'type': 'numeric', 'value': 0},
             {'path': ':QS_PAR2', 'type': 'numeric', 'value': 1},
             {'path': ':QS_PAR3', 'type': 'numeric', 'value': 1},
             {'path': ':DELAY_FLQS', 'type': 'numeric', 'value': 30},
             {'path': ':DELAY_QSQSO', 'type': 'numeric', 'value': 0},
             {'path': ':SSHOT', 'type': 'numeric'},
             {'path': ':USHOT', 'type': 'numeric'},
             {'path': ':CGTEMP', 'type': 'numeric'}]

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CRDS_SERVER','INIT',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':ARM_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CRDS_SERVER','PULSE_ON',50,None),Method(None,'arm',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CRDS_SERVER','PULSE_ON',50,None),Method(None,'stop',head))",
                  'options': ('no_write_shot',)})

    flqsModeDict = {'FL-INT QS-INT': 'II', 'FL-INT QS-EXT': 'IE',
                    'FL-EXT QS-INT': 'EI', 'FL-EXT QS-EXT': 'EE'}
    STATUS_OK = 0
    STATUS_ERR = -1
    STATUS_TMO = -2

    STATE0 = 0  # Boot Fault
    STATE1 = 1  # Warm up
    STATE2 = 2  # Laser ready for a RUN command
    STATE3 = 3  # Flashing - lamp disabled
    STATE4 = 4  # Flashing awaiting shutter to be opened
    STATE5 = 5  # Flashing - Pulse enabled
    STATE6 = 6  # Pulsed Laser ON /NLO Warm up
    STATE7 = 7  # Harmonic generator thermally stabilized
    STATE8 = 8  # NLO optimization
    STATE9 = 9  # APM ok : NLO ready

    stateToMsg = {'0': 'Boot Fault',
                  '1': 'Warm up',
                  '2': 'Laser ready for a RUN command',
                  '3': 'Flashing - lamp disabled',
                  '4': 'Flashing awaiting shutter to be opened',
                  '5': 'Flashing - Pulse enabled',
                  '6': 'Pulsed Laser ON /NLO Warm up',
                  '7': 'Harmonic generator thermally stabilized',
                  '8': 'NLO optimization',
                  '9': 'APM ok : NLO ready'}

    laserWorkers = {}

    def laserOpen(self, ip, port):
        try:
            # Create a TCP/IP socket
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(1)
            # Connect the socket to the port where the server is listening
            server_address = (ip, port)
            print('connecting to %s port %s' % (server_address))
            sock.connect(server_address)
            return sock, 'No error'
        except Exception as ex:
            return -1, str(ex)

    def laserClose(self, sock):
        sock.close()

    def sendCommand(self, sock, cmd, value=None):
        time.sleep(0.2)
        # Send data
        if value == None:
            sock.sendall(cmd+'\n')
        else:
            sock.sendall(cmd+' '+str(value)+'\n')

        # Look for the response
        try:
            answer = ''
            # while not answer.endswith('\n'):
            while True:
                answer += sock.recv(256)
        except socket.timeout as e:
            if len(answer) > 0:
                out = re.split('=|:|\n', answer)
                print ('Command %s %s  -> %s' % (cmd, value, answer[:-1]))
                if 'ERROR' in answer:
                    return -1, out[1].strip()
                elif 'OK' in answer:
                    if out[0].strip() == 'OK':
                        value = None
                    else:
                        value = out[1].strip()
                    return 0, value
        return -1, 'Invalid answer message format'

    def writeParam(self, sock, cmd, node, conv, msg):

        if node != None:
            try:
                if conv == None:
                    val = node.data()
                else:
                    val = conv[node.data()]
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve %s %s' % (msg, str(e)))
                raise mdsExceptions.DevBAD_PARAMETER
        else:
            val = None
        try:
            status, out = self.sendCommand(sock, cmd, val)
            if status == -1:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Error writing %s %s' % (msg, out))
                raise mdsExceptions.DevCOMM_ERROR
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Error writing %s %s' % (msg, str(e)))
            raise mdsExceptions.DevCOMM_ERROR

    def stateMonitor(self, sock, toState, numIter):
        for i in range(0, int(numIter)):
            time.sleep(0.2)
            status, value = self.sendCommand(sock, 'STATE')
            if status == -1:
                return self.STATUS_ERR, value
            if int(value) == toState:
                return self.STATUS_OK, value
        return self.STATUS_TMO, value


# Support class for Laser shots
    class AsynchStart(Thread):

        def configure(self, device, ip_address, port, stopArm):

            self.device = device

            self.sock, msg = device.laserOpen(ip_address, port)
            if self.sock == -1:
                Data.execute('DevLogErr($1,$2)', self.device.getNid(),
                             'Cannot open device ' + msg)
                raise mdsExceptions.DevCOMM_ERROR

            self.stopArm = stopArm
            self.stopFlag = False

            return 1

        def run(self):

            try:

                status, currState = self.device.stateMonitor(
                    self.sock, self.device.STATE2, 3)
                if status == self.device.STATUS_OK:
                    self.device.writeParam(
                        self.sock, 'RUN', None, None, 'RUN Activates the Flash lamps')
                else:
                    print('WARNING: Laser not in Ready for RUN command state')

                if status == self.device.STATUS_OK:

                    ephocSec = time.time()

                    status, currState = self.device.stateMonitor(
                        self.sock, self.device.STATE5, 10)

                    # Waiting for QSwitch enable
                    #print('Waiting for 8sec for QSwitch laser trigger')
                    # time.sleep(2)

                    delay = (9 - (time.time() - ephocSec))
                    print('DELAY ', delay)
                    if delay > 0:
                        time.sleep(delay)

                    if status == self.device.STATUS_OK:
                        self.device.writeParam(
                            self.sock, 'QSW 1', None, None, 'QSW Starts the Q-Switch laser emission')
                    else:
                        print('WARNING: Laser not in Flashing state')

                    #status, currState = self.device.stateMonitor(self.sock, self.device.STATE3, 30)
                    # if  status != self.device.STATUS_OK :
                    #    print('WARNING: Laser not in ON state')

                while not self.stopFlag:

                    #print('LASER armed waiting for stop command')
                    # self.stopArm.acquire()
                    # self.stopArm.wait()
                    time.sleep(3.)
                    status, value = self.device.sendCommand(self.sock, 'STATE')

                print('Stop command Receved')

                if status == self.device.STATUS_OK:
                    self.device.writeParam(
                        self.sock, 'QSW 0', None, None, 'QSW Stops the Q-Switch laser emission')
                else:
                    print('WARNING: Laser not in Pulse ON state')

                if status == self.device.STATUS_OK:
                    time.sleep(2)

                    status, currState = self.device.stateMonitor(
                        self.sock, self.device.STATE5, 3)
                    if status == self.device.STATUS_OK:
                        self.device.writeParam(
                            self.sock, 'STOP', None, None, 'STOP Stops the flash lamps')
                    else:
                        print('WARNING: Laser not in Flashing state')

            except Exception as ex:
                print('LASER ARM thread exception ' + str(ex))

            self.device.laserClose(self.sock)
            print('LASER ARM thread exit!!')

            return 0

        def stop(self):
            self.stopFlag = True
            self.stopArm.acquire()
            self.stopArm.notify()
            self.stopArm.release()

# end class AsynchStore

###################### Worker Management ###########################
    def saveWorker(self):
        LASER_QSMART.laserWorkers[self.getNid()] = self.worker

    def restoreWorker(self):
        try:
            if self.getNid() in LASER_QSMART.laserWorkers.keys():
                self.worker = LASER_QSMART.laserWorkers[self.getNid()]
                return 1
            else:
                return -1
        except:
            print('Cannot restore worker!!')
            return -1

    def removeWorker(self):
        del LASER_QSMART.laserWorkers[self.getNid()]

####################################################################

    def init(self):

        if self.restoreWorker() == 1:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Laser is armed')
            self.stop()

        try:
            ip_address = self.ip_address.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP address ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            port = self.port.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP port ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        sock, msg = self.laserOpen(ip_address, port)
        if sock == -1:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open device ' + msg)
            raise mdsExceptions.DevCOMM_ERROR

        status, currState = self.stateMonitor(sock, self.STATE2, 15)
        if status != self.STATUS_OK:
            self.laserClose(sock)
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'Laser not reaches the ready to run state (2), current state %s' % (self.stateToMsg[currState]))
            raise mdsExceptions.DevIO_STUCK

        try:
            self.writeParam(sock, 'CAPVSET', self.fl_voltage,
                            None, 'Flashlamp voltage')
            self.writeParam(sock, 'LPW', self.fl_pwidth,
                            None, 'Flashlamp pulse width')
            self.writeParam(sock, 'TRIG', self.flqs_mode,
                            self.flqsModeDict, 'Flashlamp Qswitch mode')
            self.writeParam(sock, 'QSPAR1', self.qs_par1, None,
                            'QSPAR1 number of cycles for Burst/Scan mode')
            self.writeParam(sock, 'QSPAR2', self.qs_par2, None,
                            'QSPAR2 number of passive pulses per cycle for Burst/Scan mode')
            self.writeParam(sock, 'QSPAR3', self.qs_par3, None,
                            'QSPAR3 number of active Q-Switch pulses per cycle for Burst/Scan mode')
            self.writeParam(sock, 'QDLY', self.delay_flqs, None,
                            'DELAY_FLQS delay between the end of the flashlamp pulse and the beginning of the Q-S pulse')
            self.writeParam(sock, 'QDLYO', self.delay_qsqso, None,
                            'DELAY_QSQSO delay between the Q-Switch pulse and the Q-Switch Synch. Out')
            print ('Finish laser initialization')
        except Exception as ex:
            self.laserClose(sock)
            raise ex

        self.laserClose(sock)
        return 1

    def arm(self):

        if self.restoreWorker() == 1:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Laser is already armed')
            raise mdsExceptions.DevCOMM_ERROR

        try:
            ip_address = self.ip_address.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP address ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            port = self.port.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP port ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        self.worker = self.AsynchStart()
        self.worker.daemon = True

        stopArm = Condition()

        self.worker.configure(self, ip_address, port, stopArm)

        self.saveWorker()
        self.worker.start()

        return 1

    def stop(self):

        if self.restoreWorker() == -1:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Laser not in arm state')
            raise mdsExceptions.DevCOMM_ERROR

        self.worker.stop()
        self.worker.join()
        self.removeWorker()

        return 1

    def reset(self):

        if self.restoreWorker() == 1:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Laser is armed. Reset operation not allowed')
            raise mdsExceptions.DevCOMM_ERROR

        try:
            ip_address = self.ip_address.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP address ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        try:
            port = self.port.data()
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot resolve IP port ' + str(e))
            raise mdsExceptions.DevBAD_PARAMETER

        sock, msg = self.laserOpen(ip_address, port)
        if sock == -1:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot open device ' + msg)
            raise mdsExceptions.DevCOMM_ERROR

        try:
            self.writeParam(sock, 'RESET', None, None,
                            'Laser reset ==> switch ON/OFF')
            print ('Finish laser Reset')
        except Exception as ex:
            self.laserClose(sock)
            raise ex

        self.laserClose(sock)
        return 1
