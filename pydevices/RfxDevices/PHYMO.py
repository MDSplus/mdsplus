from MDSplus import Device, Data, Range, Uint64, Event, Float64, Float64Array, Tree
from MDSplus.mdsExceptions import DevCOMM_ERROR, DevBAD_PARAMETER, PyUNHANDLED_EXCEPTION
import time
import socket




class PHYMO(Device):
    '''PhyLogic PhyMotion stepping motor and positional encoder module'''
    #copying the device attributes from the phytango driver, assuiming movement unit will always be in mm


    #controller parameters
    parts = [
        {'path':':IP','type': 'text','value': '192.168.62.131'}
        ,{'path':':PORT','type': 'numeric','value': 22222}
    ]

    parts.append({'path':':AXIS', 'type':'numeric', 'value': 1}) #Axis will most likely remain as one, but some modules in the Phymotion lineup have mulitple axes
    parts.append({'path':':MODULE', 'type':'numeric', 'value': 1})
    parts.append({'path':':VELOCITY','type':'numeric','value': 400})
    parts.append({'path':':ACC','type':'numeric','value': 4000})
    parts.append({'path':':PARK_POS','type':'numeric','value': 100})
    parts.append({'path':':TARGET','type':'numeric','value': 0})
    parts.append({'path':':POSITION','type':'signal'})
        
    
    parts.extend([
        {'path': ':INITIALISE', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','INITIALISE',50,None),Method(None,'initialise',head))",
         'options': ('no_write_shot',)},
        {'path': ':MOVE_TO', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','MOVE_TO',50,None),Method(None,'move_to',head))",
         'options': ('no_write_shot',)},
        {'path': ':PARK', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','PARK',50,None),Method(None,'park',head))",
         'options': ('no_write_shot',)},
        {'path': ':STOP', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','STOP',50,None),Method(None,'STOP',head))",
         'options': ('no_write_shot',)},
        {'path': ':RESET', 'type': 'action',
         'valueExpr': "Action(Dispatch('PXI_SERVER','RESET',50,None),Method(None,'RESET',head))",
         'options': ('no_write_shot',)}
        ])
    
    probeAxis = None
    _STOP = None

    def initialise(self):
        
        # def __init__(self, targetPosition = '100', ip= '192.168.62.131', port = 22222, module = '2', axis = '1', parkingPosition = '100', speed = '400', acceleration = '4000', threadScaling = '-0.0000310059', encoderScaling = '0.00005', movingThreshold = 0.01, accuracy = 0.01, maxRepositions = 5 )
        target = self.target.data()
        ip = self.ip.data()
        port = self.port.data()
        axis = self.axis.data()
        module = self.module.data()
        velocity = self.velocity.data()
        acceleration = self.acc.data()
        parkPos = self.park_pos.data()
        PHYMO._STOP = False
        PHYMO.probeAxis = PhyMotionDriver(target, ip, port, module, axis, parkPos, velocity, acceleration) #populate with data from parts

        return()


    def move_to(self):
        target = self.target.data()
        movementData = PHYMO.probeAxis.moveTo(target, self)
            

    def park(self):
        parkPos = self.park_pos.data()
        movementData = PHYMO.probeAxis.moveTo(parkPos, self) #moves the probe to the parking position
        currentPos = PHYMO.probeAxis._readwrite(21) #obtains the current encoder position
        PHYMO.probeAxis._readwrite(20, True, currentPos) #sets the current counter position to match the encoder position
        return()

    def stop(self): #figure this out with Tim
        PHYMO.probeAxis._sendMessage("S")
        PHYMO._STOP = True
        return()
    
    def reset(self): #Also figure out with Tim
        self._STOP = False
        return()


class PhyMotionDriver():
   
    def __init__(self, targetPosition = '100', ip= '192.168.62.131', port = 22222, module = '2', axis = '1', parkingPosition = '100', speed = '400', acceleration = '4000', threadScaling = '-0.0000310059', encoderScaling = '0.00005', movingThreshold = 0.01, accuracy = 0.01, maxRepositions = 5 ):
        """Creates a single axis of a PhyMotion controller

        Args:
            targetPosition (str, optional): Sets the position that the controller will navigate to if .move is called with no arguments.
            ip (str, optional): IP address of the controller. Defaults to '192.168.62.131'.
            port (int, optional): Network port of the controller. Defaults to 22222.
            module (str, optional): Module number of device to be controlled. Defaults to '2'.
            axis (str, optional): Axis on module, will be 1 unless module has multiple axes. Defaults to '1'.
            parkingPosition (str, optional): Sets zero position of the axis. Defaults to '100'.
            speed (str, optional): Frequency of motor, max 400, may need reducing if motor skips too much. Defaults to '400'.
            acceleration (str, optional): Acceleration ramp rate for step frequency, may need reducing if motor skips at start of movement. Defaults to '4000'.
            threadScaling (str, optional): Scaling factor to convert from motor steps to mm, can be measured or calculated using information in P03 section of manual. Defaults to '-0.0000310059'.
            encoderScaling (str, optional): Scaling factor for encoder, given by manufacturer or calculated from P39 in manual. Defaults to '0.00005'.
            movingThreshold (float, optional): Driver decides it has finished moving when measured distance is less than this in mm. Defaults to 0.01.
            accuracy (float, optional): Accuracy of final positioning in mm. Defaults to 0.01.
            maxRepositions (int, optional): Maximum number of iterations to obtain fine positioning at end of movement. Defaults to 5.
            
        Raises:
            MDSplus.mdsExceptions.DevCOMM_ERROR: Error raised on communications failure
        """
        print("Initialising")
        self.targetPosition = targetPosition
        self.ip = ip
        self.port = port
        self.module = module
        self.axis = axis
        self.parkingPosition = parkingPosition
        self.speed = speed
        self.acceleration = acceleration
        self.threadScaling = threadScaling
        self.encoderScaling = encoderScaling
        self.movingThreshold = movingThreshold
        self.accuracy = accuracy
        self.maxRepositions = maxRepositions
        self.__STX = chr(2)  # Start of text
        self.__ACK = chr(6)  # Command ok
        self.__NACK = chr(0x15)  # command failed
        self.__ETX = chr(3)  # end of text
        self.con = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._waitForWindow()
        try:
            self.con.connect((self.ip, self.port))
            print('Connected to ' + str(ip))
        except self.con.timeout:
            print('Failed connecting to ' + str(self.port))
            raise MDSplus.mdsExceptions.DevCOMM_ERROR()
        self.con.close()
        self.position = self._readwrite(21)
        self._readwrite(20, True, self.position)
        self._readwrite('01', True, '2')
        self._readwrite('02', True, '2')
        self._readwrite('03', True, self.threadScaling)
        self._readwrite('12', True, str(float(self.parkingPosition) - 1))
        self._readwrite('39', True, self.encoderScaling)
        return


    def _readwrite(self, parameter, write = False, value = 0):
        if write:
            cmd = "P" + str(parameter) + 'S' + value
        else:
            cmd = "P" + str(parameter) + 'R'
        return(self._sendMessage(cmd))
    
    def _sendMessage(self, message, waitforreply = True):
        '''
        Adds flow control data to the message, waits for an appopriate broadcast window slot, transmits to the controller+module defined at initialisation then returns data from the controller with flow control stripped
        '''
        cmd = self.__STX + "0" + str(self.module) + "." + str(self.axis) + message + ":XX" + self.__ETX
        self._waitForWindow()
        try:
            self.con = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.con.connect((self.ip, self.port))
            self.con.send(cmd.encode("utf-8"))
            res = self.con.recv(1024).decode("utf-8")
        except self.con.timeout:
            print('Failed connecting to ' + str(self.ip))
            self.con.close()
            raise MDSplus.mdsExceptions.DevCOMM_ERROR
        self.con.close()
        if self.__ACK in res:
            return (
                str(
                res.lstrip(self.__STX)
                .lstrip(self.__ACK)
                .rstrip(self.__ETX)[:-4]
                )
                )
        else:
            return str(self.__NACK)

    def moveTo(self, targetLoc, dev):
        """ Moves the probe to the position given by argument targetLoc
        This function uses a state machine to control the motor, it will intially use the absolute move command before refining the position with relative move commands until the desired accuracy is reached or it times out

        Args:
            targetLoc (float): Target position given in mm

        Returns:
            positionHistory (list[list[float]]): The position history of the probe recorded approximately every second with pairs of time and position

        Raises:
            MDSplus.mdsExceptions.DevBAD_MODE: Raised if the case structure somehow fails and enters an undefined state
        """        
        self.position = self.pastPosition = self._sendMessage("P021R")
        self.state = 'absMove'
        self.nextState = 'absMove'
        self.targetPosition = targetLoc
        repositions = 0 #number to be incremented on each fine repositioning, code will terminate when this exceeds maximum attempts value
        
        time.sleep(1-(time.time()%1)) #waits until next multiple of 1 whole second
        while(True): #look at using a pointer to do this so it can be stopped by the MDSplus object
            if(self.state == 'absMove'):
                #send move command and move to approximating state, initialises list of positions to return
                self.positionHistory = [[time.time(), float(self.position)]]
                cmd = "A+" + str(self.targetPosition)
                self._sendMessage(cmd)
                self.nextState = 'approximating'

            elif(self.state == 'approximating'):
                #monitor position and wait for it to finish, then go to rel<=Move
                if (abs(float(self.position) - float(self.pastPosition))<self.movingThreshold): #checks if movement has stopped by comparing current and previous positions, advances to next state if stopped
                    self.nextState = 'relMove'

            elif(self.state == 'relMove'):
                #calculate offset required and send relative move command, then go to honing state
                #print("Srt relmove " + str(time.time()))
                offset = float(self.targetPosition) - float(self.position)
                if (offset < 0):
                    self._sendMessage("-" + str(abs(offset)))
                else:
                    self._sendMessage("+" + str(abs(offset)))
                self.nextState = 'honing'

            elif(self.state == 'honing'):
                #monitor position and wait to see if it stopped moving yet, if accurate enough move to closeout state, otherwise return to relmove, if this has happened too many times then fail out
                if (abs(float(self.position) - float(self.pastPosition))<float(self.movingThreshold)): #checks device has stopped moving
                    if (abs(float(self.position) - float(self.targetPosition))<self.accuracy):    #checks position is accurate enough
                        self.nextState = 'closeOut'
                    else:
                        self.nextState = 'relMove'
                        repositions = repositions + 1
                        if repositions > self.maxRepositions:
                            self.nextState = 'closeOut'

            elif(self.state == 'closeOut'):
                self._sendMessage("S")
                self.position = self._sendMessage("P021R")
                self.positionHistory.append([time.time(), float(self.position)])
                return(self.positionHistory)
                #double check position is accurate enough, if so then send stop command to relieve active hold of motor and exit out of method

            else:
                print("Undefined state entered")
                raise MDSplus.mdsExceptions.DevBAD_MODE()
            time.sleep(float(0.5-(time.time()%0.5))) #waits until next multiple of 1/2 second
            self.state = self.nextState
            self.positionHistory.append([time.time(), float(self.position)])
            dev.position.putRow(1, float(self.position), time.time())
            self.pastPosition = self.position
            self.position = self._sendMessage("P021R")
            #print(str(time.time()) + " " + str(self.position) + " " + str(self._sendMessage("P019R")) + " " + self.state)
            '''
            if(float(self._readwrite("54")) > 100): #checks temperature of motor is not above threshold and sends stop command to controller, unsure what exception to raise so just returning current data
                self._sendMessage("S")
                print("Motor temperature too high, aborting run")
                return(self.positionHistory, False)
            if(float(self._readwrite("19"))>10): #checks if the encoder and counter diverge by too much, implying the assembly has slipped too much, should probably raise an exception
                self._sendMessage("S")
                print("Motor and gear assembly has slipped too far, aborting run")
                return(self.positionHistory, False)
            '''
    def _waitForWindow(self):
        startTime = (float(self.module) - 1) * 0.01
        endTime = startTime+0.008
        currTime = time.time()%0.1
        if (startTime < currTime):
            time.sleep(max(0.1, startTime - currTime))
            return(startTime - currTime)
        elif (currTime > endTime):
            time.sleep(max(0.1, (0.01- currTime) + startTime))
            return((0.1- currTime) + startTime)
        else:
            return(0.0)

              
