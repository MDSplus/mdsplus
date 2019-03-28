import MDSplus
import threading
import socket
import time
import datetime
import numpy as np
import copy

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
        init - start daq loop
        stop - stop daq loop
        trend - store one trend sample for each channel

     debugging() - is debugging enabled.
                   Controlled by environment variable DEBUG_DEVICES

     """

    parts = [
        {'path': ':COMMENT', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':SEG_LENGTH', 'type': 'numeric', 'value': 5, 'options': ('no_write_shot')},
        {'path': ':INIT_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))",'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('S','STOP',50,None),Method(None,'STOP',head))",'options': ('no_write_shot',)},
        {'path': ':RUNNING', 'type': 'numeric', 'options': ('no_write_model')},
    ]

    for c in range(ord('A'), ord('E')):
        parts.append({'path': ':INPUT_%c' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:SERIAL_NO' %(c,), 'type': 'TEXT', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%c:CALIBRATION' %(c,), 'type': 'TEXT', 'options': ('no_write_model', 'write_once',)})
        #parts.append({'path': ':INPUT_%c:TEMPERATURE' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:RESISTENCE' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:OUTPUT_POWER' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        
    del c
    debug = None

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)


class CRYOCON24C_TREND(CRYOCON24C):
    '''
    streamming "trend" class for cryocon 24c

    Store one sample for each of the channels that is on using
    putRow.  The timestamp will be time since the unix EPOCH in msec
    '''
    parts = copy.copy(CRYOCON24C.parts)
    for c in range(ord('A'), ord('E')):
        parts.append({'path': ':INPUT_%c:PROPOR_GAIN' %(c,), 'type': 'NUMERIC', 'options': ('no_write_model')})
        parts.append({'path': ':INPUT_%c:INTEGR_GAIN' %(c,), 'type': 'NUMERIC', 'options': ('no_write_model')})
        parts.append({'path': ':INPUT_%c:DERIVA_GAIN' %(c,), 'type': 'NUMERIC', 'options': ('no_write_model')})

    parts.append({'path': ':DATA_EVENT', 'type': 'text', 'value': 'CRYOCON24C_TREND', 'options': ('no_write_shot')})
    parts.append({'path': ':NODE', 'type': 'text', 'value': '192.168.0.254', 'options': ('no_write_shot')})
    # *IDN = The identification string of the device
    # SYSTEM:HWREV = The hardware revision number
    # SYSTEM:FWREV = The firmware revision number
    # SYSTEM:AMBIENT = 
    parts.append({'path': ':STATUS_CMDS', 'type': 'text', 'value': MDSplus.makeArray(['*IDN?','SYSTem:HWRev?','SYSTem:FWREV?','SYSTem:AMBient?']),'options': ('no_write_shot')})
    parts.append({'path': ':STATUS_OUT', 'type': 'any','options': ('write_shot', 'write_once', 'no_write_model')})
    # RATE in Hz. Base rate of the stream.
    # TREND_RATE in Hz. 
    parts.append({'path': ':RATE', 'type': 'numeric', 'value': 1,'options': ('no_write_shot')})        
    parts.append({'path': ':TREND_RATE', 'type': 'numeric', 'value': 1,'options': ('no_write_shot')})

    def sendCommand(self,s,cmd):
        s.send(cmd + "\r\n")

    def recvResponse(self,s):
        msg = ""
        while True:
            c = s.recv(1)
            if c == "\r":
                continue
            if c == "\n":
                break
            msg += c
        return msg

    def queryCommand(self,s,cmd):
        self.sendCommand(s,cmd)
        return self.recvResponse(s)

    def init(self):
        '''start the stream
           socket
        '''
        thread = self.Worker(self)
        thread.start()
        return 1
    INIT = init

    class Worker(threading.Thread):
        """An async worker should be a proper class
           This ensures that the methods remian in memory
           It should at least take one argument: teh device node
        """
        def __init__(self,dev):
            print(MDSplus.__file__)
            print(MDSplus.__version__)
            super(CRYOCON24C_TREND.Worker,self).__init__(name=dev.path)
            # make a thread safe copy of the device node with a non-global context
            self.dev = MDSplus.TreeNode.copy(dev)

        def run(self):
            self.dev.run()

    def run(self):
        # start it trending
        self.running.on=True
        self.rate.record = self.trend_rate.data()

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((str(self.node.data()), 5000))

        # open the instrument
        if self.debugging():
            print("about to open cryocon device %s" % str(self.node.data()))

        event_name = self.data_event.data()

        # read and save the status commands
        status_out={}

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

        for i in range(ord('a'), ord('e')):
            # Proportional gain, or P term for PID control.
            # This is a numeric field that is a percent of full scale.
            pgain_chan = self.__getattr__('input_%c_propor_gain' % (chr(i)))
            query_cmd = 'LOOP %c:PGA?' % (chr(i),)

            ansQuery = self.queryCommand(s, query_cmd)

            try:
                print("Parsing Proportional Gain /%s/" % ansQuery)
                pgain = float(ansQuery)
            except:
                if self.debugging():
                    print("Could not Proportional Gain /%s/" % ansQuery)
                pgain = 0.0

            pgain_chan.record = MDSplus.Float32(pgain)

            # Integrator gain term, in Seconds, for PID control.
            # This is a numeric field that is a percent of full scale.
            igain_chan = self.__getattr__('input_%c_integr_gain' % (chr(i)))
            query_cmd = 'LOOP %c:IGA?' % (chr(i),)

            ansQuery = self.queryCommand(s, query_cmd)

            try:
                print("Parsing Integral Gain /%s/" % ansQuery)
                igain = float(ansQuery)
            except:
                if self.debugging():
                    print("Could not Integral Gain /%s/" % ansQuery)
                igain = 0.0

            igain_chan.record = MDSplus.Float32(igain)

            # Derivative gain term, in inverse-Seconds, for PID control.
            # This is a numeric field that is a percent of full scale.
            dgain_chan = self.__getattr__('input_%c_deriva_gain' % (chr(i)))
            query_cmd = 'LOOP %c:DGA?' % (chr(i),)

            ansQuery = self.queryCommand(s, query_cmd)

            try:
                print("Parsing Derivative Gain /%s/" % ansQuery)
                dgain = float(ansQuery)
            except:
                if self.debugging():
                    print("Could not Derivative Gain /%s/" % ansQuery)
                dgain = 0.0

            dgain_chan.record = MDSplus.Float32(dgain)

        # For the storage of calibration curves
        # index    = 1
        # caltable = []
        # strCalcur= ''

        # set up arrays of data and nodes to use in the loop
        seg_length = int(self.seg_length.data())
        times = np.zeros(seg_length)

        answerTemp = []
        answerOutp = []
        temps   = []
        resists = []
        outpower= []
        r_chans = []
        t_chans = []
        p_chans = []

        start_time = time.time()
        previous_time = 0

        query_cmd_temp = []
        query_cmd_outp = []
        for i in range(ord('a'), ord('e')):
            chan = self.__getattr__('input_%c' % (chr(i),))
            if chan.on:
                temps.append(np.zeros(seg_length))
                resists.append(np.zeros(seg_length))
                outpower.append(np.zeros(seg_length))
                t_chans.append(self.__getattr__('input_%c' % (chr(i))))
                r_chans.append(self.__getattr__('input_%c_resistence' % (chr(i))))
                p_chans.append(self.__getattr__('input_%c_output_power' % (chr(i))))
                query_cmd_temp.append('INP %c:TEMP?;SENP?' % (chr(i),))
                query_cmd_outp.append('LOOP %c:OUTP?' % (chr(i),))
        
        # Run until the STOP function is externally triggerd
        while self.running.on:
            for sample in range(seg_length):
                previous_time = time.time()
                times[sample] = previous_time - start_time
                timestamp = time.time()

                # Calibrartion Curve storage
                # cal = self.__getattr__('input_%c_calibration' % (chr(i),))
                # self.sendCommand(s, 'CALCUR %d?' % (index))
                # while ';' not in strCalcur:
                #     ans = self.recvResponse(s)
                #     print('Answer: ', ans)
                #     caltable.append(ans)
                # ans = 1 #Disable for now
                # cal.record = ans

                for i in range(len(temps)):
                    ansQueryTemp = self.queryCommand(s, query_cmd_temp[i])
                    ansQueryOutp = self.queryCommand(s, query_cmd_outp[i])
                    answerTemp = ansQueryTemp.split(';')
                    answerOutp = ansQueryOutp

                    # Temperature reading
                    try:
                        temps[i][sample] = float(answerTemp[0])
                        is_digit = True
                    except ValueError:
                        is_digit= False
                        if self.debugging():
                            print("Could not parse temperature /%s/" % answerTemp[0])
                    if not is_digit:
                        temps[i][sample] = -9999.0
                    
                    t_chans[i].putRow(1000, MDSplus.Float32(temps[i][sample]), MDSplus.Int64(timestamp*1000.))

                    # Resistence reading
                    try:
                        resists[i][sample] = float(answerTemp[1])
                        is_digit=True                                
                    except ValueError:
                        is_digit=False
                        if self.debugging():
                            print("Could not parse resist /%s/" % answerTemp)
                    if not is_digit:
                        resists[i][sample] = -9999.0
                    
                    r_chans[i].putRow(1000, MDSplus.Float32(resists[i][sample]), MDSplus.Int64(timestamp*1000.))

                    # Output Power reading: Queries the output power of the selected control loop.
                    # This is a numeric field that is a percent of full scale.
                    try:
                        # print("Parsing output power /%s/" % ansQuery)
                        if 'NAK' not in ansQueryOutp:
                            outpower[i][sample] = float(answerOutp[2])
                        else:
                            outpower[i][sample] = float(-9999.0)
                    except ValueError:
                        if self.debugging():
                            print("Could not parse output power /%s/" % answerOutp)
                    
                    p_chans[i].putRow(1000, MDSplus.Float32(outpower[i][sample]), MDSplus.Int64(timestamp*1000.))
                                
                MDSplus.Event.setevent(event_name)

                try:
                    dt = 1./float(self.rate.data())
                except:
                    dt = 1
                time.sleep(dt)
               
                if self.debugging():
                    if self.rate.data() != self.trend_rate.data():
                        print('Shot Sample-Time at SHOT rate ---->', times[sample], MDSplus.Int64(time.time()*1000.))
                    else:
                        print('Shot Sample-Time at trend rate ', times[sample], MDSplus.Int64(time.time()*1000.))

        s.close()
    RUN=run

    def stop(self):
        '''
        stop method for the cryocon 24c
        set the stop flag by turning off the 'running' node
        '''
        self.running.on = False
        return 1
    STOP = stop

class CRYOCON24C_SHOT(CRYOCON24C):
    parts = copy.copy(CRYOCON24C.parts)
    parts.append({'path':':DATA_EVENT','type':'text', 'value': 'CRYOCON24C_DATA','options':('no_write_shot')})
    parts.append({'path': ':T1', 'type': 'numeric', 'options': ('no_write_shot'), 'help': 'The time in seconds that the shot began taking data'})
    parts.append({'path': ':T2', 'type': 'numeric', 'value': 0,'options': ('write_shot')})
    # SHOT_RATE in Hz. 
    parts.append({'path': ':SHOT_RATE', 'type': 'numeric', 'value': 5,'options': ('no_write_shot')})
    parts.append({'path': ':TREND_TREE', 'type': 'text', 'options': ('no_write_shot')})
    parts.append({'path': ':TREND_DEVICE', 'type': 'text', 'options': ('no_write_shot')})
    parts.append({'path': ':TREND_SHOT', 'type': 'numeric', 'value': 0, 'options': ('write_shot'), 'help': 'The record of the shot number of the trend this data came from'})

    def getTrendTree(self):
        tree_name    = self.trend_tree.data()
        shot_number  = self.trend_shot.data()
        trend_tree   = MDSplus.Tree(tree_name, shot_number, 'NORMAL')
        return trend_tree

    def init(self):
        trend_tree = self.getTrendTree()
        trend_dev = trend_tree.getNode(self.trend_device.data())
        trend_dev.rate.record = self.shot_rate.data()
    INIT=init

    def stop(self):
        event_name = self.shot_event.data()
        self.t2.record = MDSplus.Int64(time.time()*1000.)
        trend_tree = self.getTrendTree()
        trend_dev = trend_tree.getNode(self.trend_device.data())

        # Resetting the original rate:
        trend_dev.rate.record = trend_dev.trend_rate.data()

        # Getting T1 from TREND:
        t1 = MDSplus.Int64(self.t1.data()*1000.)

        #Saving TREND shot number information into the tree:
        self.trend_shot.record = trend_tree.getCurrent(self.trend_tree.data())
        
        #Set Time Context
        trend_tree.setTimeContext(t1, self.t2.data())
        
        print('Writing data into shot node')
        for i in range(ord('a'), ord('e')):
            trend_temp     = trend_dev.__getattr__('input_%c'% (chr(i)))
            trend_resis    = trend_dev.__getattr__('input_%c_resistence'%  (chr(i)))
            trend_outpower = trend_dev.__getattr__('input_%c_output_power'% (chr(i)))
            
            times    = trend_temp.dim_of().data()
            temps    = trend_temp.data()
            resists  = trend_resis.data()
            outpower = trend_outpower.data()

            # Might be able to improve
            start_time = times[0]
            for j in range(len(times)):
                times[j] -= start_time
                times[j] = float(times[j]) / 1000.
            
            shot_temp     = self.__getattr__('input_%c' % (chr(i)))
            shot_resis    = self.__getattr__('input_%c_resistence' % (chr(i)))
            shot_outpower = self.__getattr__('input_%c_output_power' % (chr(i)))

            shot_temp.record     = MDSplus.Signal(temps, None, times)
            shot_resis.record    = MDSplus.Signal(temps, None, times)
            shot_outpower.record = MDSplus.Signal(temps, None, times)
        
        MDSplus.Event.setevent(event_name)

    STOP=stop
