import MDSplus
import threading
import socket
import time

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
        {'path': ':NODE', 'type': 'text', 'value': '192.168.0.254',
         'options': ('no_write_shot')},
        {'path': ':COMMENT', 'type': 'text', 'options': ('no_write_shot')},
        {'path': ':TREND_EVENT', 'type': 'text', 'value': 'CRYOCON_TREND',
         'options': ('no_write_shot')},
        {'path': ':DATA_EVENT', 'type': 'text', 'value': 'CRYOCCON_STREAM',
         'options': ('no_write_shot')},
        {'path': ':STATUS_CMDS', 'type': 'text',
         'value': MDSplus.makeArray(['*IDN?',
                                     'SYSTem:HWRev?',
                                     'SYSTem:FWREV?',
                                     'SYSTem:AMBient?']),
         'options': ('no_write_shot',)},
        {'path': ':STATUS_OUT',
         'type': 'any',
         'options': ('write_shot', 'write_once', 'no_write_model')},
        {'path': ':SEG_LENGTH',
         'type': 'numeric',
         'value': 5,
         'options': ('no_write_shot')},
        {'path': ':MAX_SEGMENTS',
         'type': 'numeric',
         'value': 200,
         'options': ('no_write_shot')},
        {'path': ':RATE',
         'type': 'numeric',
         'value': .25,
         'options': ('no_write_shot')},
        {'path': ':TRIG_TIME', 'type': 'numeric', 'options': ('write_shot')},
        {'path': ':TRIG_STR', 'type': 'text', 'options': ('nowrite_shot'),
         'valueExpr': "EXT_FUNCTION(None,'ctime',head.TRIG_TIME)"},
        {'path': ':RUNNING', 'type': 'numeric', 'options': ('no_write_model')},
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr':
             "Action(Dispatch('S','INIT',50,None),Method(None,'INIT',head))",
         'options': ('no_write_shot',)},
        {'path': ':STOP_ACTION', 'type': 'action',
         'valueExpr':
             "Action(Dispatch('S','STORE',50,None),Method(None,'STOP',head))",
         'options': ('no_write_shot',)},
    ]

    for c in range(ord('A'), ord('E')):
        parts.append({'path': ':INPUT_%c' % (c,),
                      'type': 'signal',
                      'options': ('no_write_model', 'write_once',)})

        parts.append({'path': ':INPUT_%c:SERIAL_NO' % (c,),
                      'type': 'TEXT', 'options': ('no_write_shot')})

        parts.append({'path': ':INPUT_%c:CALIBRATION' % (c,),
                      'type': 'TEXT',
                      'options': ('no_write_model', 'write_once',)})

        parts.append({'path': ':INPUT_%c:PROPOR_GAIN' % (c,),
                      'type': 'NUMERIC', 'options': ('no_write_model')})

        parts.append({'path': ':INPUT_%c:INTEGR_GAIN' % (c,),
                      'type': 'NUMERIC', 'options': ('no_write_model')})

        parts.append({'path': ':INPUT_%c:DERIVA_GAIN' % (c,),
                      'type': 'NUMERIC', 'options': ('no_write_model')})

        parts.append({'path': ':INPUT_%c:TEMPERATURE' % (c,),
                      'type': 'signal',
                      'options': ('no_write_model', 'write_once',)})

        parts.append({'path': ':INPUT_%c:RESISTENCE' % (c,),
                      'type': 'signal',
                      'options': ('no_write_model', 'write_once',)})

        parts.append({'path': ':INPUT_%c:OUTPUT_POWER' % (c,),
                      'type': 'signal',
                      'options': ('no_write_model', 'write_once',)})


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
    trend class for cryocon 24c

    Store one sample for each of the channels that is on using
    putRow.  The timestamp will be time since the unix EPOCH in msec
    '''

    def init(self):
        '''start the stream
           socket
        '''

        # start it trending
        self.running.on=True

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
            max_segments = self.max_segments.data()

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

                pgain_chan.putRow(1000,
                                  MDSplus.Float32(pgain),
                                  MDSplus.Int64(t_time * 1000.))

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

                igain_chan.putRow(1000,
                                  MDSplus.Float32(igain),
                                  MDSplus.Int64(t_time * 1000.))

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

                dgain_chan.putRow(1000,
                                  MDSplus.Float32(dgain),
                                  MDSplus.Int64(t_time * 1000.))

            # For the storage of calibration curves
            index    = 1
            caltable = []
            strCalcur= ''

            query_cmd = ''
            answer    = []

            segment = 0
            while self.running.on and segment < max_segments:
                for i in range(ord('a'), ord('e')):
                    chan = self.__getattr__('input_%c' % (chr(i),))
                    if chan.on:

                        # Calibrartion Curve storage
                        cal = self.__getattr__('input_%c_calibration' % (chr(i),))
                        self.sendCommand(s, 'CALCUR %d?' % (index))

                        while ';' not in strCalcur:
                            ans = self.recvResponse(s)
                            print('Answer: ', ans)
                            caltable.append(ans)

                        ans = 1 #Disable for now
                        cal.record = ans

                        # Temperature reading
                        temp_chan = self.__getattr__('input_%c_temperature' % (chr(i)))
                        query_cmd = 'INP %c:TEMP?;UNIT?' % (chr(i),)

                        ansQuery = self.queryCommand(s, query_cmd)
                        answer = ansQuery.split(';')
                        print(answer)

                        t_time = time.time()
                        try:
                            print("Parsing temperature /%s/" % ans[0])
                            temp = float(answer[0])
                        except:
                            if self.debugging():
                                print("Could not parse temperature /%s/" % answer[0])
                            temp = 0.0

                        temp_chan.putRow(1000,
                                      MDSplus.Float32(temp),
                                      MDSplus.Int64(t_time * 1000.))


                        # Resistence reading
                        resist_chan = self.__getattr__('input_%c_resistence' % (chr(i)))
                        query_cmd = 'INP %c:SENP?' % (chr(i),)

                        ansQuery = self.queryCommand(s, query_cmd)

                        try:
                            print("Parsing resistance /%s/" % ansQuery)
                            resist = float(ansQuery)
                        except:
                            if self.debugging():
                                print("Could not parse resist /%s/" % ansQuery)
                            resist = 0.0

                        resist_chan.putRow(1000,
                                    MDSplus.Float32(resist),
                                    MDSplus.Int64(t_time * 1000.))


                        # Output Power reading: Queries the output power of the selected control loop.
                        # This is a numeric field that is a percent of full scale.
                        outp_chan = self.__getattr__('input_%c_output_power' % (chr(i)))
                        query_cmd = 'LOOP %c:OUTP?' % (chr(i),)

                        ansQuery = self.queryCommand(s, query_cmd)

                        try:
                            print("Parsing output power /%s/" % ansQuery)
                            outpower = float(ansQuery)
                        except:
                            if self.debugging():
                                print("Could not parse output power /%s/" % ansQuery)
                            outpower = 0.0

                        outp_chan.putRow(1000,
                                    MDSplus.Float32(outpower),
                                    MDSplus.Int64(t_time * 1000.))


                s.close()

            MDSplus.Event.setevent(event_name)


    def stop(self):
        '''
        stop method for the cryocon 24c
        set the stop flag by turning off the 'running' node
        '''
        self.running.on = False
        return 1

    STOP = stop

class CRYOCON24C_SHOT(CRYOCON24C):
    def init(self):
        pass
    def stop(self):
        pass
    pass



def main():

    # create tree model
    tree_model = MDSplus.Tree('cryocon24c', -1, 'NEW')
    CRYOCON24C_TREND.Add(tree_model, 'CRYOCON24C')
    node = tree_model.getNode('\CRYOCON24C::TOP:CRYOCON24C:NODE')
    node.putData('198.125.182.159')
    print('Writing Tree for device ' + '198.125.182.159')
    tree_model.write()
    print('Closing Tree')
    tree_model.close()

    #Executing the experiment:
    print('Running INIT from cryocon24c using MDS-TCL')
    tree = MDSplus.Tree('cryocon24c', -1)
    tree.tcl('set tree cryocon24c/shot=-1')
    tree.tcl('set current cryocon24c/increment')
    tree.tcl('create pulse 0')
    tree.tcl('set tree cryocon24c/shot=0')
    tree.tcl('do /meth CRYOCON24C INIT')
    return 1

if __name__ == '__main__':
    main()