import MDSplus
import threading
import socket
import time
import datetime
import numpy as np

if MDSplus.version.ispy2:
    def tostr(x):
        return x

    def tobytes(x):
        return x
else:
    def tostr(x):
        return b if isinstance(b, str) else b.decode('utf-8')

    def tobytes(x):
        return s if isinstance(s, bytes) else s.encode('utf-8')


class CRYOCON24C(MDSplus.Device):
    """
     4 channel Cryocon temperature monitor

     Each channel (A-D):
       channel_[a-d] - measured resistence
                      (for trend timestamp record, for daq segmented record)
       serial_no - serial number of the sensor
       calibration - 2d array of calibration values read from device
       temperature - measured temperature
                    (for trend timestamp record, for daq segmented record)

     Methods:
        init  - start daq loop
        stop  - stop daq loop
        trend - store one trend sample for each channel

     debugging() - is debugging enabled.
                   Controlled by environment variable DEBUG_DEVICES

     """

    inputs = "abcd"
    loops = range(1, 4+1)

    parts = [
        {'path': ':COMMENT', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':INIT_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))", 'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action',
            'valueExpr': "Action(Dispatch('S','STOP',50,None),Method(None,'STORE',head))", 'options': ('no_write_shot',)},
        {'path': ':RUNNING', 'type': 'numeric', 'options': ('no_write_model')},
    ]
    for i in inputs.upper():
        parts.extend([
            {'path': ':INPUT_%c' % (i,), 'type': 'signal', 'options': (
                'no_write_model', 'write_once',)},
            {'path': ':INPUT_%c:SERIAL_NO' % (
                i,), 'type': 'TEXT', 'options': ('no_write_shot')},
            {'path': ':INPUT_%c:CALIBRATION' % (i,), 'type': 'TEXT', 'options': (
                'no_write_model', 'write_once',)},
            {'path': ':INPUT_%c:RESISTENCE' % (i,), 'type': 'signal', 'options': (
                'no_write_model', 'write_once',)},
        ])
    for i in loops:
        parts.extend([
            {'path': ':LOOP_%d' % (i,), 'type': 'signal', 'options': (
                'no_write_model', 'write_once',)},
        ])
    del(i)
    debug = None

    def debugging(self):
        import os
        if self.debug == None:
            self.debug = os.getenv("DEBUG_DEVICES")
        return(self.debug)


class CRYOCON24C_TREND(CRYOCON24C):
    '''
    streamming "trend" class for cryocon 24c

    Store one sample for each of the channels that is on using
    putRow.  The timestamp will be time since the unix EPOCH in msec
    '''
    parts = list(CRYOCON24C.parts)

    for i in CRYOCON24C.loops:
        parts.extend([
            {'path': ':LOOP_%d:PROPOR_GAIN' % (
                i,), 'type': 'NUMERIC', 'options': ('no_write_model')},
            {'path': ':LOOP_%d:INTEGR_GAIN' % (
                i,), 'type': 'NUMERIC', 'options': ('no_write_model')},
            {'path': ':LOOP_%d:DERIVA_GAIN' % (
                i,), 'type': 'NUMERIC', 'options': ('no_write_model')},
            {'path': ':LOOP_%d:SETPOINT' % (
                i,), 'type': 'NUMERIC', 'options': ('no_write_shot')},
        ])
    del(i)
    parts.extend([
        {'path': ':DATA_EVENT', 'type': 'text',
            'value': 'CRYOCON24C_TREND', 'options': ('no_write_shot')},
        {'path': ':NODE', 'type': 'text', 'value': '192.168.0.254',
            'options': ('no_write_shot')},
        # *IDN = The identification string of the device
        # SYSTEM:HWREV = The hardware revision number
        # SYSTEM:FWREV = The firmware revision number
        # SYSTEM:AMBIENT =
        {'path': ':STATUS_CMDS', 'type': 'text', 'value': MDSplus.makeArray(
            ['*IDN?', 'SYSTem:HWRev?', 'SYSTem:FWREV?', 'SYSTem:AMBient?']), 'options': ('no_write_shot')},
        {'path': ':STATUS_OUT', 'type': 'any', 'options': (
            'write_shot', 'write_once', 'no_write_model')},
    ])

    def sendCommand(self, s, cmd):
        s.send(tobytes(cmd + "\r\n"))

    def recvResponse(self, s):
        msg = ""
        while True:
            c = tostr(s.recv(1))
            if c == "\r":
                continue
            if c == "\n":
                break
            msg += c
        return msg

    def queryCommand(self, s, cmd):
        self.sendCommand(s, cmd)
        return self.recvResponse(s)

    def init(self):
        '''
        init method for cryocon 24c
        '''
        self.trend()
        return 1
    INIT = init

    def trend(self):
        # start it trending
        self.running.on = True

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((str(self.node.data()), 5000))

        # open the instrument
        if self.debugging():
            print("about to open cryocon device %s" % str(self.node.data()))

        event_name = self.data_event.data()

        try:
            self.status_out.getData()
        except:
            if self.debugging():
                print("Status_Out tree node is empty. Lets write the data now.")
            # read and save the status commands
            status_out = {}

            for cmd in self.status_cmds:
                if self.debugging():
                    print('about to send %s' % cmd)

                # status_out[str(cmd)] = instrument.query(str(cmd))
                status_out[str(cmd)] = self.queryCommand(s, str(cmd))

                if self.debugging():
                    print('  got back %s' % status_out[str(cmd)])

            self.status_out.record = status_out

        # Control Loop PID values Numeric Entry The Pgain, Igain and Dgain lines correspond to the Proportional,
        # Integral and Derivative coefficients of the control loop. Pman is the output power that will be applied
        # to the load if the manual control mode is selected.
        # Values for the Proportional, or P, gain term range from zero to 1000. This is a unit- less gain term
        # that is applied to the control loop. Gain is scaled to reflect the actual heater range and the load resistance.
        # Integrator gain values range from zero to 10,000. The units of this term are Seconds. A value of zero
        # turns the integration function off.
        # Derivative gain values have units of inverse Seconds and may have values from zero to 1000. A value of
        # zero turns the Derivative control function off.

        for i in self.loops:
            # Proportional gain, or P term for PID control.
            # This is a numeric field that is a percent of full scale.
            pgain_chan = self.__getattr__('loop_%d_propor_gain' % (i,))
            pgain_query_cmd = 'LOOP %d:PGA?' % (i,)

            # Integrator gain term, in Seconds, for PID control.
            # This is a numeric field that is a percent of full scale.
            igain_chan = self.__getattr__('loop_%d_integr_gain' % (i,))
            igain_query_cmd = 'LOOP %d:IGA?' % (i,)

            # Derivative gain term, in inverse-Seconds, for PID control.
            # This is a numeric field that is a percent of full scale.
            dgain_chan = self.__getattr__('loop_%d_deriva_gain' % (i,))
            dgain_query_cmd = 'LOOP %d:DGA?' % (i,)

            spoint_chan = self.__getattr__('loop_%d_setpoint' % (i,))
            spoint_query_cmd = 'LOOP %d:SETP?' % (i,)

            ansQuery = self.queryCommand(s, pgain_query_cmd)

            try:
                pgain_chan.getData()
                igain_chan.getData()
                dgain_chan.getData()
            except:
                try:
                    print("Parsing Proportional Gain /%s/" % ansQuery)
                    pgain = float(ansQuery)
                except:
                    if self.debugging():
                        print("Could not parse Proportional Gain /%s/" %
                              ansQuery)
                    pgain = 0.0

                pgain_chan.record = MDSplus.Float32(pgain)

                ansQuery = self.queryCommand(s, igain_query_cmd)

                try:
                    print("Parsing Integral Gain /%s/" % ansQuery)
                    igain = float(ansQuery)
                except:
                    if self.debugging():
                        print("Could not parse Integral Gain /%s/" % ansQuery)
                    igain = 0.0

                igain_chan.record = MDSplus.Float32(igain)

                ansQuery = self.queryCommand(s, dgain_query_cmd)

                try:
                    print("Parsing Derivative Gain /%s/" % ansQuery)
                    dgain = float(ansQuery)
                except:
                    if self.debugging():
                        print("Could not parse Derivative Gain /%s/" % ansQuery)
                    dgain = 0.0

                dgain_chan.record = MDSplus.Float32(dgain)

                ansQuery = self.queryCommand(s, spoint_query_cmd)

                try:
                    print("Parsing Setpoint /%s/" % ansQuery)
                    spoint = float(ansQuery)
                except:
                    if self.debugging():
                        print("Could not parse Setpoint /%s/" % ansQuery)
                    spoint = 0.0

                spoint_chan.record = MDSplus.Float32(spoint)

        for i in self.inputs:
            t_chans = self.__getattr__('input_%c' % (i,))
            r_chans = self.__getattr__('input_%c_resistence' % (i,))
            if t_chans.on:
                query_cmd_temp = 'INP %c:TEMP?;SENP?' % (i,)
                ansQuery = self.queryCommand(s, query_cmd_temp)

                t_time = time.time()

                # Temperature reading
                try:
                    temps = float(ansQuery.split(';')[0])
                    is_digit = True
                except ValueError:
                    is_digit = False
                    if self.debugging():
                        print("Could not parse temperature /%s/" %
                              ansQuery.split(';')[0])
                if not is_digit:
                    temps = -9999.0

                t_chans.putRow(1000, MDSplus.Float32(temps),
                               MDSplus.Int64(t_time*1000.))

                # Resistence reading
                try:
                    resists = float(ansQuery.split(';')[1])
                    is_digit = True
                except ValueError:
                    is_digit = False
                    if self.debugging():
                        print("Could not parse resist /%s/" %
                              ansQuery.split(';')[1])
                if not is_digit:
                    resists = -9999.0

                r_chans.putRow(1000, MDSplus.Float32(resists),
                               MDSplus.Int64(t_time*1000.))

        for i in self.loops:
            p_chans = self.__getattr__('loop_%d' % (i,))
            query_cmd_outp = 'LOOP %d:HTRRead?' % (i,)
            ansQuery = self.queryCommand(s, query_cmd_outp)
            # remove the '%' at the end of the number
            answerOutp = ansQuery[:-1]

            if p_chans.on:
                t_time = time.time()
                # Output Power reading: Queries the output power of the selected control loop.
                # This is a numeric field that is a percent of full scale.
                try:
                    print("Parsing output power /%s/" % ansQuery)
                    if 'NAK' not in ansQuery:
                        outpower = float(answerOutp)
                    else:
                        outpower = float(-9999.0)
                except ValueError:
                    if self.debugging():
                        print("Could not parse output power /%s/" % ansQuery)

                p_chans.putRow(1000, MDSplus.Float32(outpower),
                               MDSplus.Int64(t_time*1000.))

        MDSplus.Event.setevent(event_name)
        s.close()
    TREND = trend

    def stop(self):
        '''
        stop method for the cryocon 24c
        set the stop flag by turning off the 'running' node
        '''
        self.running.on = False
        return 1
    STOP = stop


class CRYOCON24C_SHOT(CRYOCON24C):
    parts = CRYOCON24C.parts + [
        {'path': ':DATA_EVENT', 'type': 'text',
            'value': 'CRYOCON24C_DATA', 'options': ('no_write_shot')},
        {'path': ':T1', 'type': 'numeric', 'options': (
            'no_write_shot'), 'help': 'The time in seconds that the shot began taking data'},
        {'path': ':T2', 'type': 'numeric',
            'value': 0, 'options': ('write_shot')},

        {'path': ':TREND_TREE', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':TREND_DEVICE', 'type': 'text',
            'options': ('no_write_shot')},
        {'path': ':TREND_SHOT', 'type': 'numeric', 'value': 0, 'options': (
            'write_shot'), 'help': 'The record of the shot number of the trend this data came from'},
    ]

    def getTrendTree(self):
        tree_name = self.trend_tree.data()
        shot_number = self.trend_shot.data()
        trend_tree = MDSplus.Tree(tree_name, shot_number, 'NORMAL')
        return trend_tree

    def init(self):
        # Place holder
        pass
    INIT = init

    def store(self):
        event_name = self.data_event.data()
        if self.t2.rlength == 0:
            self.t2.record = MDSplus.Int64(time.time()*1000.)
        trend_tree = self.getTrendTree()
        trend_dev = trend_tree.getNode(self.trend_device.data())

        # Getting T1 from TREND:
        t1 = MDSplus.Int64(self.t1.data()*1000.)

        # Saving TREND shot number information into the tree:
        self.trend_shot.record = trend_tree.getCurrent(self.trend_tree.data())

        # Set Time Context
        trend_tree.setTimeContext(t1, self.t2.data())

        print('Writing data into shot node')
        for i in self.inputs:
            trend_temp = trend_dev.__getattr__('input_%c' % (i,))
            trend_resis = trend_dev.__getattr__('input_%c_resistence' % (i,))

            times = trend_temp.dim_of().data()
            temps = trend_temp.data()
            resists = trend_resis.data()

            # Might be able to improve
            start_time = times[0]
            for j in range(len(times)):
                times[j] -= start_time
                times[j] = float(times[j]) / 1000.

            shot_temp = self.__getattr__('input_%c' % (i,))
            shot_resis = self.__getattr__('input_%c_resistence' % (i,))

            shot_temp.record = MDSplus.Signal(temps, None, times)
            shot_resis.record = MDSplus.Signal(resists, None, times)

        for i in self.loops:
            trend_outpower = trend_dev.__getattr__('loop_%d' % (i,))
            times = trend_outpower.dim_of().data()
            outpower = trend_outpower.data()

            start_time = times[0]
            for j in range(len(times)):
                times[j] -= start_time
                times[j] = float(times[j]) / 1000.

            shot_outpower = self.__getattr__('loop_%d' % (i,))
            shot_outpower.record = MDSplus.Signal(outpower, None, times)

        MDSplus.Event.setevent(event_name)
    STORE = store
