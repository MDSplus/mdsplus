from MDSplus.compound import Range
from MDSplus.event import Event
from MDSplus.mdsarray import Float32Array, Float64Array
from MDSplus.mdsdata import Data
from MDSplus.mdsExceptions import DevCOMM_ERROR, DevBAD_PARAMETER, PyUNHANDLED_EXCEPTION, MDSplusERROR
from MDSplus.mdsscalar import Float32, Float64, Uint64
from MDSplus.tree import Tree, Device
import MDSplus as mds
from sentea.interrogator.api.http import Client
from threading import Thread
import os
import sys
import numpy as np
import errno
import time

class DM8125(Device):
    parts = [
        {'path': 'ABS_TIME', 'type': 'numeric', 'value':0},
        {'path': 'FIBREMASK', 'type': 'text', 'value':'11111111'},
        {'path': ':INIT', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER', 'INIT', 50), Method(None, 'init', head))"},
        {'path': ':IP', 'type': 'text', 'value': '127.0.0.1'},
        {'path': 'SAMPLE_RATE', 'type': 'numeric', 'value':10},
        {'path': ':START_STORE', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER', 'STORE', 50), Method(None, 'start_store', head))"},
        {'path': ':STOP_STORE', 'type': 'action',
         'valueExpr': "Action(Dispatch('SERVER', 'STORE', 50), Method(None, 'stop_store', head))"},
        {'path': 'THRESHOLD', 'type': 'numeric', 'value':8000}
        ]
    
    for fibre in range(8): # Example FIBRE1:TSTAMP_EPOCH
        fibre_path = 'FIBRE'+str(fibre+1)
        parts.append({'path':fibre_path, 'type':'structure'})
        parts.append({'path':fibre_path+':FIBRE_ID', 'type': 'text', 'value': ''})
        parts.append({'path':fibre_path+':SENSORMASK', 'type': 'text', 'value': '1111111111'})
        for sensor in range(10): # Example FIBRE1.SENSOR1:WAVELENGTH
            sensor_path = fibre_path+'.SENSOR'+str(sensor+1)
            parts.append({'path':sensor_path, 'type':'structure'})
            parts.append({'path':sensor_path+':C0', 'type':'numeric', 'value':0})
            parts.append({'path':sensor_path+':C1', 'type':'numeric', 'value':0})
            parts.append({'path':sensor_path+':C2', 'type':'numeric', 'value':0})
            parts.append({'path':sensor_path+':C3', 'type':'numeric', 'value':0})
            parts.append({'path':sensor_path+':INIT_WAVLN', 'type':'numeric'})
            parts.append({'path':sensor_path+':SENSOR_ID', 'type': 'text', 'value': ''})
            parts.append({'path':sensor_path+':TEMP', 'type':'signal'})
            parts.append({'path':sensor_path+':WAVELENGTH', 'type':'signal'})

    for fibre in range(8): # Example FIBRE1:TSTAMP_EPOCH
        fibre_path = 'FIBRE'+str(fibre+1)
        for sensor in range(10): # Example FIBRE1.SENSOR1:WAVELENGTH
            sensor_path = fibre_path+'.SENSOR'+str(sensor+1)
            parts.append({'path':sensor_path+':C4', 'type':'numeric', 'value':0})


    STOP_FLAG = False
    ABS_TIME = 0

    # These are constants
    ENABLED = 0
    INDEX = 1
    MAX_FIBRES = 8
    MAX_SENSORS = 10

    def thread_alive(self, thread):
        if getattr(thread, "is_alive", None):
            alive = thread.is_alive()
        elif getattr(thread, "isAlive", None):
            alive = thread.isAlive()
        else:
            print("Python version error")
            emsg = 'ERROR: Python version error'
            Data.execute('DevLogErr($1)', emsg)
            raise MDSplusERROR
        return alive

    ### ------------------------------------------------- Get Client --------------------------------------------------------------###
    def get_client(self):
        IPaddress = self.getNode(':IP').data()
        print("IP Address:", IPaddress)
        if IPaddress == '127.0.0.1':
            print("Please enter IP address into Tree")
            raise DevCOMM_ERROR 
        else:
            try:
                client = Client(IPaddress)
            except Exception as e:
                emsg = 'Cannot connect to deivce: %s'%(str(e))
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevCOMM_ERROR

        return client
    
    ### ------------------------------------------------- Initialise Data Structures --------------------------------------------------------------###
    def initialise_data_structures(self, data_length):

        self.c0 = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.float64)
        self.c1 = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.float64)
        self.c2 = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.float64)
        self.c3 = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.float64)
        self.c4 = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.float64)

        self.time = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS, data_length], dtype=np.float64)
        self.wave = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS, data_length], dtype=np.float64)
        self.temp = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS, data_length], dtype=np.float64)
        
        self.data_index = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS,], dtype=np.int8)
        

        for fibre in range(DM8125.MAX_FIBRES):
            node_root_path = "FIBRE"+str(fibre+1) # Example FIBRE1.SENSOR1:C0
            for sensor in range (DM8125.MAX_SENSORS):
                 
                sensor_node_path = node_root_path+".SENSOR"+str(sensor+1)
                
                node = sensor_node_path+":C0"
                self.c0[fibre][sensor] = self.getNode(node).data()
                node = sensor_node_path+":C1"
                self.c1[fibre][sensor] = self.getNode(node).data()
                node = sensor_node_path+":C2"
                self.c2[fibre][sensor] = self.getNode(node).data()
                node = sensor_node_path+":C3"
                self.c3[fibre][sensor] = self.getNode(node).data()
                node = sensor_node_path+":C4"
                self.c4[fibre][sensor] = self.getNode(node).data()
                
   

    ### -------------------------------------------------Store Data----------------------------------------------------------------------------- ###
    def store_data(self, fibre_number, sensor_number):
        
        fibre_name = "FIBRE"+str(fibre_number+1)
        sensor_name = ".SENSOR"+str(sensor_number+1)
        sensor_path = fibre_name + sensor_name
        node = getattr(self, sensor_path+":TEMP")
        node.makeSegment(
            self.time[fibre_number][sensor_number][0],  # segment start time
            self.time[fibre_number][sensor_number][self.data_index[fibre_number][sensor_number]-1],  # segment end time (same, single event)
            self.time[fibre_number][sensor_number][:self.data_index[fibre_number][sensor_number]],  # dimension: actual event time(s)
            self.temp[fibre_number][sensor_number][:self.data_index[fibre_number][sensor_number]] # value: list of temperatures
        )

        node = getattr(self, sensor_path+":WAVELENGTH")
        node.makeSegment(
            self.time[fibre_number][sensor_number][0],  # segment start time
            self.time[fibre_number][sensor_number][self.data_index[fibre_number][sensor_number]-1],  # segment end time (same, single event)
            self.time[fibre_number][sensor_number][:self.data_index[fibre_number][sensor_number]],  # dimension: actual event time(s)
            self.wave[fibre_number][sensor_number][:self.data_index[fibre_number][sensor_number]] # value: list of Wave lengths
        )

        return

###--------------------------------------------------------process_samples-----------------------------------------------------------------###
    def process_samples(self, data, data_length):
        ### Process samples
   
        if DM8125.ABS_TIME == 0:
            timestamp_epoch = data[0].timestamp_epoch # Epoch time in seconds
            timestamp_nanoseconds = data[0].timestamp_ns # Additional nano seconds
            DM8125.ABS_TIME = (timestamp_epoch) + (timestamp_nanoseconds * 1e-9)
            node_path = ":ABS_TIME"
            getattr(self, node_path).putData(DM8125.ABS_TIME)

        for sample in data:
            
            peak_wavelengths = sample.peak_wls
            fibre_number = sample.fiber_id - 1
            timestamp_epoch = sample.timestamp_epoch # Epoch time in seconds
            timestamp_nanoseconds = sample.timestamp_ns # Additional nano seconds
            sample_timestamp = (timestamp_epoch) + (timestamp_nanoseconds * 1e-9)
            machine_time = DM8125.ABS_TIME
            relative_time = Float64(sample_timestamp - machine_time)
            #relative_time = Float64(sample_timestamp)


            if len(peak_wavelengths) > np.sum(DM8125.SENSOR_ENABLED[fibre_number]):
                print("###ERROR###: Fibre ",fibre_number+1," Sensor data mismatch. Check Sentea settings against MDSplus tree, check spectrum in Sentea peakviewer")
                emsg = 'Sensor failed enable mismatch. Check Sentea settings against MDSplus tree'

            if len(peak_wavelengths) < np.sum(DM8125.SENSOR_ENABLED[fibre_number]):
                print("###ERROR###: Fibre ",fibre_number+1," Some sensor data missing. Check Sentea settings against MDSplus tree, check spectrum in Sentea peakviewer")
                emsg = 'Sensor failed enable mismatch. Check Sentea settings against MDSplus tree'

            if len(peak_wavelengths) == 0:
                print("###ERROR###: Fibre ",fibre_number+1," All sensor data missing. Check Sentea settings against MDSplus tree")
                emsg = 'Sensor failed enable mismatch. Check Sentea settings against MDSplus tree'
            
            if len(peak_wavelengths) != np.sum(DM8125.SENSOR_ENABLED[fibre_number]):
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg)
                raise DevBAD_PARAMETER


            for sensor_number in range(DM8125.MAX_SENSORS):
                ## Configuration error checking
                init_wavelength = DM8125.INIT_WAVELENGTH[fibre_number][sensor_number] 

                ## Add blank data for deselected sensor
                if DM8125.SENSOR_ENABLED[fibre_number][sensor_number] == 0:
                    peak_wavelengths.insert(sensor_number, 0)
                    continue
                
                #wavelength_difference = (peak_wavelengths[sensor_number] - init_wavelength)/init_wavelength

                measured_wavelength = peak_wavelengths[sensor_number]

                temperature = 0

                if self.c4[fibre_number][sensor_number] != 0:
                    temperature += self.c4[fibre_number][sensor_number] * pow(measured_wavelength, 4)
                    
                if self.c3[fibre_number][sensor_number] != 0:
                    temperature = temperature + self.c3[fibre_number][sensor_number] * pow(measured_wavelength, 3)

                if self.c2[fibre_number][sensor_number] != 0:
                    temperature = temperature + self.c2[fibre_number][sensor_number] * pow(measured_wavelength, 2)

                if self.c1[fibre_number][sensor_number] != 0:
                    temperature = temperature + self.c1[fibre_number][sensor_number] * pow(measured_wavelength, 1)

                if self.c0[fibre_number][sensor_number] != 0:
                    temperature = temperature + self.c0[fibre_number][sensor_number]

                self.time[fibre_number][sensor_number][self.data_index[fibre_number][sensor_number]] = relative_time
                self.temp[fibre_number][sensor_number][self.data_index[fibre_number][sensor_number]] = temperature
                self.wave[fibre_number][sensor_number][self.data_index[fibre_number][sensor_number]] = peak_wavelengths[sensor_number]
                self.data_index[fibre_number][sensor_number] += 1

                if self.data_index[fibre_number][sensor_number] == data_length:
                    self.store_data(fibre_number, sensor_number)
                    self.data_index[fibre_number][sensor_number] = 0

        return
    
    ### -----------------------------------------------------------Read Store Data--------------------------------------------------------------###
    def pulse_read_store_data(self, client):
        data_length = 10

        self.initialise_data_structures(data_length)
        
        while True:

            data = client.capture.get_data(limit=10000) ### GETS SAMPLES
            stop_command = DM8125.STOP_FLAG

            if len(data.samples) != 0: ### IF THRE ARE NO SAMPLES SKIP THIS LOOP
                self.process_samples(data.samples, data_length)
            else:
                time.sleep(1)

            if stop_command == True:
                break
 
        time.sleep(1)
        data = client.capture.get_data(limit=100) ### GETS SAMPLES
        client.capture.stop()
        self.process_samples(data.samples, data_length)
        for fibre_number in range(DM8125.MAX_FIBRES):
                    for sensor_number in range(DM8125.MAX_SENSORS):
                        if self.data_index[fibre_number][sensor_number] != 0:
                            self.store_data(fibre_number, sensor_number)
                            self.data_index[fibre_number][sensor_number] = 0

        print("\n###Data collected###")
        DM8125.STOP_FLAG = False
        
        return
    
    ### --------------------------------------------------DM8125 Init-----------------------------------------------------------------------------###
    def init(self):

        print("Python version: ", sys.version)

        print("\n\n---------------DM8125 Init------------------------")


        # Connect to device
        client = self.get_client()

        # Check capture status and stop capture if currently running
        capture_running = client.capture.get_status()
        print("Running = ", capture_running.active)
        if capture_running.active == True:
            print("Stopping")
            client.capture.stop() 

        print("###Capture status checked###")

        # Update capture settings.
        # Mode: "ref" = 250uS period (Slow and accurate), "fast" = 42uS period (fast but less accurate)
        # fibre_selection: fibre bit mask. (1 fibre = 1, 2 fibres = 3, 3 fibres = 7, 4 fibres = 15, 5 fibres = 31, 6 fibres = 63, 7 fibres = 127, 8 fibres = 255)
        
        DM8125.FIBREMASK = int(self.getNode(':FIBREMASK').data()[::-1], 2)
        print("FIBRE MASK")
        print(self.getNode(':FIBREMASK').data()[::-1])
        DM8125.SENSORMASK = np.zeros(DM8125.MAX_FIBRES, dtype=np.int16)
        DM8125.SENSOR_ENABLED = np.zeros([DM8125.MAX_FIBRES, DM8125.MAX_SENSORS], dtype=np.int8)


        for fibre in range(DM8125.MAX_FIBRES):
            sensor_path = "FIBRE"+str(fibre+1)
            node = getattr(self, sensor_path+":SENSORMASK")
            DM8125.SENSORMASK[fibre] = int(self.getNode(node).data()[::-1], 2)
            

            for sensor in range(DM8125.MAX_SENSORS):
                DM8125.SENSOR_ENABLED[fibre][sensor] = (DM8125.SENSORMASK[fibre] >> sensor) & 1

        
        print(DM8125.SENSOR_ENABLED)
        # sample_rate: Sample rate in Hz
        sample_rate = int(self.getNode(':SAMPLE_RATE').data())
        
        client.capture.update_settings(mode="ref", fiber_selection=DM8125.FIBREMASK, sample_rate=sample_rate)
        
        print("###Fibres and sample rate set###")
        
        # Get capture settings.
        print(client.capture.get_settings())

        DM8125.INIT_WAVELENGTH = np.zeros([DM8125.MAX_FIBRES,DM8125.MAX_SENSORS], dtype=np.float64)
        
        init_wavelengths = client.peak_detection.get_tracking_settings()
 
        DM8125.ABS_TIME = 0

        fibre_number = 0

        print(init_wavelengths.init_traces)

        for init_wavelengths in init_wavelengths.init_traces:

            node_root_path = "FIBRE"+str(fibre_number+1) # Example FIBRE1:PEAK_WLS
            
            sensor_number = 0
            for init_wavelength in init_wavelengths:
                while (DM8125.SENSOR_ENABLED[fibre_number][sensor_number] == 0) or (sensor_number > 10):
                    sensor_number += 1


                sensor_node_path = ".SENSOR"+str(sensor_number+1)
                node_path = node_root_path+sensor_node_path+":INIT_WAVLN"
                getattr(self, node_path).putData(init_wavelength)

                DM8125.INIT_WAVELENGTH[fibre_number][sensor_number] = init_wavelength
                sensor_number += 1

            if np.count_nonzero(DM8125.SENSOR_ENABLED[fibre_number]) != np.count_nonzero(DM8125.INIT_WAVELENGTH[fibre_number]):
                print("###ERROR###: Fibre ", fibre_number+1, " Sensor selection mismatch, the number of inital wavelengths recived from Sentea differs from what is set it MDSplus")
                emsg = 'Sensor selection mismatch'
                Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
                raise DevBAD_PARAMETER

            fibre_number += 1

        print("###Initial wavelengths written to tree###")

        ## This code will fill the device with some numbers to help verify the netbeans interface
        """ 
        for fibre in range(8):
            fibre_path = ":FIBRE"+str(fibre+1)
            node_path = fibre_path + ":FIBRE_ID"
            getattr(self, node_path).putData(str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1))
            node_path = fibre_path + ":SENSORMASK"
            getattr(self, node_path).putData(str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1)+str(fibre+1))
            for sensor in range(10):
                sensor_path = fibre_path + ".SENSOR"+str(sensor+1)
                node_path = sensor_path + ":SENSOR_ID"
                getattr(self, node_path).putData(str(fibre+1)+str(fibre+1)+str(sensor+1)+str(sensor+1))
                for coef in range(5):
                    node_path = sensor_path + ":C" + str(coef)
                    getattr(self, node_path).putData((coef)*10000+(fibre+1)*100+(sensor+1))

        """


    ### --------------------------------------------------DM8125 Start Store-----------------------------------------------------------------------###
    def start_store(self):
        DM8125.START_TIME = time.time()
        print("Getting the time took ", time.time() - DM8125.START_TIME, " seconds")

        print("\n\n-------------DM8125 Start Store-------------------")

        client = self.get_client()
        
        # Start capture.
        client.capture.start()
        
        print("###Capture Started###")
        
        DM8125.STOP_FLAG = False

        DM8125.store_thread = Thread(target=self.pulse_read_store_data, args=(client, ), daemon=True)
        DM8125.store_thread.start()
        print("###Data collection thread started###")

    ### --------------------------------------------------DM8125 Stop Store------------------------------------------------------------------------###
    def stop_store(self):
        print("\n\n-------------DM8125 Stop Store--------------------")
        client = self.get_client()

        DM8125.STOP_FLAG = True

        if DM8125.store_thread and self.thread_alive(DM8125.store_thread):
            print("Join Thread")
            DM8125.store_thread.join(timeout=5)

        print("###Capture stopped###")

        # Get power settings.
        print(client.capture.get_power_settings())
        print("###Power Settings Collected###")

        # Get spectrometer background correction update settings.
        print(client.capture.get_background_settings())
        print("###Background setting collected###")

