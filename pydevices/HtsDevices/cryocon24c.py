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
        {'path': ':SHOT_TIMES', 'type': 'numeric', 'options': ('write_shot')},
        {'path': ':TRIG_STR', 'type': 'text', 'options': ('nowrite_shot'), 'valueExpr': "EXT_FUNCTION(None,'ctime',head.SHOT_TIMES)"},
        {'path': ':INIT_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))",'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action', 'valueExpr': "Action(Dispatch('S','STORE',50,None),Method(None,'STOP',head))",'options': ('no_write_shot',)},
        {'path': ':RUNNING', 'type': 'numeric', 'options': ('no_write_model')},
    ]

    for c in range(ord('A'), ord('E')):
        parts.append({'path': ':INPUT_%c' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:SERIAL_NO' %(c,), 'type': 'TEXT', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%c:CALIBRATION' %(c,), 'type': 'TEXT', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:TEMPERATURE' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:RESISTENCE' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%c:OUTPUT_POWER' %(c,), 'type': 'signal', 'options': ('no_write_model', 'write_once',)})
        
    del c
    debug = None

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

    # RATE in Hz. Base rate of the stream.
    # TREND_RATE in Hz. 
    parts.append({'path': ':NODE', 'type': 'text', 'value': '192.168.0.254', 'options': ('no_write_shot')})
    parts.append({'path': ':STATUS_CMDS', 'type': 'text', 'value': MDSplus.makeArray(['*IDN?','SYSTem:HWRev?','SYSTem:FWREV?','SYSTem:AMBient?']),'options': ('no_write_shot',)})
    parts.append({'path': ':STATUS_OUT', 'type': 'any','options': ('write_shot', 'write_once', 'no_write_model')})
    parts.append({'path': ':RATE', 'type': 'numeric', 'value': 1,'options': ('no_write_shot')})        
    parts.append({'path': ':TREND_RATE', 'type': 'numeric', 'value': 1,'options': ('no_write_shot')})


    def init(self):
        '''start the stream
           socket
        '''
        # self.stream()
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
        # event_name = self.data_event.data()

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
        index    = 1
        caltable = []
        strCalcur= ''

        answer    = []

        # set up arrays of data and nodes to use in the loop
        seg_length = int(self.seg_length.data())
        times = np.zeros(seg_length)

        temps   = []
        resists = []
        outpower= []
        r_chans = []
        t_chans = []
        p_chans = []

        start_time = time.time()
        previous_time = 0

        # trend_tree = MDSplus.Tree('cryocon24c', 0, 'NORMAL')
        # shot = trend_tree.getNode('cryo24_shot')
        # shotRate = shot.shot_rate.data()

        query_cmd = []
        for i in range(ord('a'), ord('e')):
            chan = self.__getattr__('input_%c' % (chr(i),))
            if chan.on:
                temps.append(np.zeros(seg_length))
                resists.append(np.zeros(seg_length))
                outpower.append(np.zeros(seg_length))
                t_chans.append(self.__getattr__('input_%c_temperature' % (chr(i))))
                r_chans.append(self.__getattr__('input_%c_resistence' % (chr(i))))
                p_chans.append(self.__getattr__('input_%c_output_power' % (chr(i))))
                query_cmd.append('INP %c:TEMP?;SENP?;OUTP?' % (chr(i),))

        # Run until the STOP function is externally triggerd
        while self.running.on:
            shotTimes = []
            for sample in range(seg_length):
                previous_time = time.time()
                times[sample] = previous_time - start_time     

                # Cleaning cryocon buffer? The following seems to work with 
                # the answer from Cryocon is NAK. By asking ';' the answers
                # are back to normal:
                # ansQuery = self.queryCommand(s, ';')

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
                    ansQuery = self.queryCommand(s, query_cmd[i])
                    # print(ansQuery)
                    answer = ansQuery.split(';')

                    # Temperature reading
                    try:
                        # print("Parsing temperature /%s/" % answer[0])
                        temps[i][sample] = float(answer[0])
                        is_digit = True
                    except ValueError:
                        is_digit= False
                        if self.debugging():
                            print("Could not parse temperature /%s/" % answer[0])
                    if not is_digit:
                        temps[i][sample] = -9999.0

                    # Resistence reading
                    try:
                        # print("Parsing resistance /%s/" % ansQuery)                            
                        resists[i][sample] = float(answer[1])
                        is_digit=True                                
                    except ValueError:
                        is_digit=False
                        if self.debugging():
                            print("Could not parse resist /%s/" % answer)
                    if not is_digit:
                        resists[i][sample] = -9999.0
                        
                    # Output Power reading: Queries the output power of the selected control loop.
                    # This is a numeric field that is a percent of full scale.
                    try:
                        # print("Parsing output power /%s/" % ansQuery)
                        if 'NAK' not in ansQuery:
                            outpower[i][sample] = float(answer[2])
                        else:
                            outpower[i][sample] = float(-9999.0)
                    except ValueError:
                        if self.debugging():
                            print("Could not parse output power /%s/" % answer)
                                
                try:
                    dt = 1./float(self.rate.data())
                except:
                    dt = 1
                time.sleep(dt)

                if self.rate.data() != self.trend_rate.data():
                    shotTimes.append(float(times[sample]))
                    print('Shot Sample-Time ', times[sample])
                
            if sample != seg_length-1:
                for i in range(len(temps)):
                    times = times[0:sample]
                    temps[i] = temps[i][0:sample]
                    resists[i] = resists[i][0:sample]
                    outpower[i] = outpower[i][0:sample]
                    
            for i in range(len(temps)):            
                t_chans[i].makeSegment(times[0], times[-1], times, temps[i])
                r_chans[i].makeSegment(times[0], times[-1], times, resists[i])
                p_chans[i].makeSegment(times[0], times[-1], times, outpower[i])
                
                # MDSplus.Event.setevent(event_name)
            # if len(shotTimes) != 0:
            #     tree = MDSplus.Tree('cryocon24c', 0, 'NORMAL')
            #     shot_node = tree.getNode('cryo24_shot')
            #     shot_temp_node = shot_node.__getattr__('input_a_temperature')
            #     shot_temp = shot_node.getNode(shot_temp_node)
            #     shotsig = Signal(makeArray(shotTimes), None, makeArray(shotTimes))
            #     shot_temp.record = shotsig

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
    # SHOT_RATE in Hz. Choosen long trend rate.     
    parts.append({'path': ':T1', 'type': 'numeric', 'value': 0.,'options': ('no_write_shot')})
    parts.append({'path': ':T2', 'type': 'numeric', 'value': 0,'options': ('no_write_shot')})
    parts.append({'path': ':SHOT_RATE', 'type': 'numeric', 'value': 100,'options': ('no_write_shot')})
    parts.append({'path': ':SHOT_NUM', 'type': 'numeric', 'value': 0,'options': ('no_write_shot')})

    def init(self):
        self.t1.record = time.time()
        trend_tree = MDSplus.Tree('cryocon24c', 0, 'NORMAL')
        trend = trend_tree.getNode('cryo24_trend')
        trend.rate.record = self.shot_rate.data()
        
    def stop(self):
        times   = []
        temps   = []
        resists = []
        outpower= []
        
        self.t2.record = time.time()
        delta_shot= self.t2.data() - self.t1.data()

        trend_tree = MDSplus.Tree('cryocon24c', 0, 'NORMAL')
        trend = trend_tree.getNode('cryo24_trend')
        trend.rate.record = trend.trend_rate.data()


        trend_temp_node = trend.__getattr__('input_a_temperature')
        trend_temp = trend_tree.getNode(trend_temp_node)
        temps = trend_temp.data()
        times = trend_temp.dim_of().data()

        trend_resis_node = trend.__getattr__('input_a_resistence')
        trend_resis = trend_tree.getNode(trend_resis_node)
        resists = trend_resis.data()

        trend_outpower_node = trend.__getattr__('input_a_output_power')
        trend_outpower = trend_tree.getNode(trend_outpower_node)
        outpower = trend_outpower.data() 

        print(temps)
        print(times)
        print(resists)
        print(outpower)

        print('Writing data into shot node')
        # for i in np.arange(0, delta_shot, 1./float(self.shot_rate.data())):

        #     self.temperature.record = trend_temp.data()    
        #     self.resistence.record = trend_resis.data() 
        #     self.output_power.record = trend_outpower.data()


def main():

    # create tree model
    tree_model = MDSplus.Tree('cryocon24c', -1, 'NEW')
    print('Creating Device CRYO24_TREND')
    CRYOCON24C_TREND.Add(tree_model, 'CRYO24_TREND')
    node = tree_model.getNode('\CRYOCON24C::TOP:CRYO24_TREND:NODE')
    node.putData('198.125.182.159')
    print('Writing Tree for device ' + '198.125.182.159')
    tree_model.write()

    print('Creating Device CRYO24_SHOT')
    CRYOCON24C_SHOT.Add(tree_model, 'CRYO24_SHOT')
    tree_model.write()

    print('Closing Tree')
    tree_model.close()

    #Executing the experiment:
    print('Running INIT from cryocon24c using Python calls')
    tree = MDSplus.Tree('cryocon24c', -1)
    tree.setCurrent('cryocon24c', tree.getCurrent('cryocon24c') + 1)
    tree.createPulse(0)
    tree = MDSplus.Tree('cryocon24c', 0)
    dev_trend = tree.getNode('cryo24_trend')
    dev_trend.init()
    time.sleep(20)

    dev_shot = tree.getNode('cryo24_shot')
    dev_shot.init()

    time.sleep(20)
    dev_shot.stop()

    time.sleep(20)
    dev_trend.stop()
    return 1

if __name__ == '__main__':
    main()