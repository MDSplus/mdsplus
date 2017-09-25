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

import MDSplus
import ctypes

def pointer(x):
    """Returns a ctypes pointer"""
    ptr = ctypes.pointer(x)
    return ptr

def kill(proc_pid):
    import psutil

    process = psutil.Process(proc_pid)
    print "Process is ", process
    for proc in process.children(recursive=True):
        proc.kill()
    process.kill()


class PICAM(MDSplus.Device):
    """
    Device class to support Princeton Instruments cameras with the PiCam library.

    methods:
      init
      store
    """

    parts = [
        {'path':':COMMENT','type':'text'},
        {'path':':SERIAL_NO','type':'text','options':('no_write_shot',)},
        {'path':':EXPOSURE','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':NUM_FRAMES','type':'numeric','value':30,'options':('no_write_shot',)},
        {'path':':ROIS','type':'numeric','options':('no_write_shot',)},
        {'path':':TIMEOUT','type':'numeric','value':100000,'options':('no_write_shot',)},
        {'path':':TRG_RESPONSE','type':'text', 'value':'StartOnSingleTrigger', 'options':('no_write_shot',)},

        {'path':':MODEL','type':'text','options':('no_write_model','write_once',)},
        {'path':':SENSOR','type':'text','options':('no_write_model','write_once',)},
        {'path':':LIB_VERSION','type':'text','options':('no_write_model','write_once',)},
        {'path':':SENSOR_TEMP','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':READOUT_TIME','type':'numeric','options':('no_write_model','write_once',)},

        {'path':':FRAMES','type':'numeric','options':('no_write_model','write_once',)},

        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
         'options':('no_write_shot',)}]

    cameras = []

    class camera_proc():
        def __init__(self, camera):
            self.camera = camera
            self.subproc = None

    def init(self):
        """
        Init method for the raspberry pi camera device.

        Start by deleting any running subrocesses that may be left
        over from previous inits, note the subprocess is stored in a
        class variable (ONLY one of these per server !)

        Read all of the settings and create a script to take the data.

        Note:  This device supports at most 1 camera per server.  Which is
        OK since the raspberry pis only have one camera port.
        """

        import os
        import subprocess

        self.debugging = os.getenv('DEBUG_DEVICES')
        camera = str(self.serial_no.record)

        c_rec = None
        for c in PICAM.cameras:
            if c.camera == camera :
                try:
                    if self.debugging:
                        print "PICAM killing ", c.subproc, c.subproc.pid
                    kill(c.subproc.pid)
                except Exception, e:
                    if self.debugging:
                        print "PICAM kill exception", e
                    pass
                c_rec = c
        if c_rec is None:
            c = PICAM.camera_proc(camera)
            PICAM.cameras.append(c)
        if not c.camera == camera:
            c = PICAM.camera_proc(camera)
            PICAM.cameras.append(c)

        tree = self.local_tree
        shot = self.tree.shot
        path = self.local_path
        c.subproc = subprocess.Popen('mdstcl 2>&1', stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
        c.subproc.stdin.write('set tree %s /shot = %d\n'%(tree, shot,))
        c.subproc.stdin.write('do/meth %s acquire\n'%(path,))
        c.subproc.stdin.write('exit\n')
        c.subproc.stdin.flush()
        return 1
    INIT=init

    def acquire(self):
        import os
        import ctypes as ctypes
        import numpy as np
        from MDSplus.mdsExceptions import DevCOMM_ERROR
        from MDSplus.mdsExceptions import DevBAD_PARAMETER
        from MDSplus.mdsExceptions import DevPY_INTERFACE_LIBRARY_NOT_FOUND

        try:
            import PythonForPicam as pfp
        except:
            raise DevPY_INTERFACE_LIBRARY_NOT_FOUND("Picam error importing PythonForPicam")

        self.debugging = os.getenv('DEBUG_DEVICES')
        exposure = float(self.exposure)
        exposure = pfp.piflt(exposure)
        num_frames = int(self.num_frames)
        timeout = int(self.timeout)
        serial_no = str(self.serial_no.record)
        try:
            if self.debugging:
                print "PICAM about to try to read the ROIS"
            rois = self.rois.data()
            if self.debugging:
                print "PICAM got the rois ", rois
        except Exception,e:
            if self.debugging:
                print "PICAM could not read the ROIS"
            rois = None
        
        print "Acquire  - debugging is ", self.debugging

        # initialize the library
        pfp.Picam_InitializeLibrary()

        # get library version information
        major = pfp.piint()
        minor = pfp.piint()
        distribution = pfp.piint()
        release = pfp.piint()
        pfp.Picam_GetVersion(pointer(major),pointer(minor),pointer(distribution),pointer(release))
        self.lib_version.record = 'Picam Version %d.%d.%d Released %d' % (major.value,minor.value,distribution.value,release.value,)

        available = ctypes.POINTER(ctypes.c_int)()
        availableCount = pfp.piint();

        status = pfp.Picam_GetAvailableCameraIDs(ctypes.byref(available), ctypes.byref(availableCount))
        camera = pfp.PicamHandle()
        cameras_type = pfp.PicamCameraID*availableCount.value
        cameras_pointer = ctypes.POINTER(cameras_type)
        cameras = ctypes.cast(available, cameras_pointer)
        found = False
        for c in cameras.contents:
            if self.debugging:
                print "checking ",c.serial_number
            if c.serial_number == serial_no:
                status = pfp.Picam_OpenCamera(pointer(c),ctypes.addressof(camera))
                if not status == "PicamError_None":
                    raise DevCOMM_ERROR("PiCam - could not open camera serial no %d - %s"% (serial_no,status,))
                found = True
        if not found:
            raise DevBAD_PARAMETER("PiCam - Could not find camera %d"%serial_no)
 
#        Picam_OpenCamera(ctypes.addressof(camera))
        PicamID = pfp.PicamCameraID()  

        pfp.Picam_GetCameraID(camera,  pointer(PicamID))
        self.model.record = str(PicamID.model)
        self.sensor.record = str(PicamID.sensor_name)

        trigger_response = str(self.trg_response.record)
        if trigger_response == 'NoResponse':
            trigger_resp = pfp.PicamTriggerResponse_NoResponse
        elif trigger_response == 'ReadoutPerTrigger':
            trigger_resp = pfp.PicamTriggerResponse_ReadoutPerTrigger
        elif trigger_response == 'ShiftPerTrigger':
            trigger_resp = pfp.PicamTriggerResponse_ShiftPerTrigger
        elif trigger_response == 'ExposeDuringTriggerPulse':
            trigger_resp = pfp.PicamTriggerResponse_ExposeDuringTriggerPulse
        elif trigger_response == 'StartOnSingleTrigger':
            trigger_resp = pfp.PicamTriggerResponse_StartOnSingleTrigger
        else:
            raise DevBAD_PARAMETER("PiCam - TRG_RESPONSE must be one of ('NoResponse','ReadoutPerTrigger','ShiftPerTrigger','ExposeDuringTriggerPulse', 'StartOnSingleTrigger')")

        if self.debugging:
            print "Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerResponse,",trigger_resp,")"
        pfp.Picam_SetParameterIntegerValue( camera, pfp.PicamParameter_TriggerResponse, trigger_resp )
        pfp.Picam_SetParameterIntegerValue( camera, pfp.PicamParameter_TriggerDetermination, pfp.PicamTriggerDetermination_PositivePolarity )
        pfp.Picam_SetParameterIntegerValue( camera, pfp.PicamParameter_OutputSignal, pfp.PicamOutputSignal_Exposing )

        # set the exposure
        if self.debugging:
            print "Picam_SetParameterFloatingPointValue( camera, PicamParameter_ExposureTime, ",exposure,")"
        pfp.Picam_SetParameterFloatingPointValue( camera, pfp.PicamParameter_ExposureTime, exposure )

        failCount = pfp.piint()
        paramsFailed = pfp.piint()
        if self.debugging:
            print "Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount))"
        pfp.Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount))
        if self.debugging:
            print "failcount is ", failCount
        pfp.Picam_DestroyParameters(pointer(paramsFailed))

        width = pfp.piint(0)
        pfp.Picam_GetParameterIntegerValue( camera, ctypes.c_int(pfp.PicamParameter_SensorActiveWidth), ctypes.byref(width) );
        width = width.value

        # if there are rois set the rois
        if rois is not None:
            if self.debugging:
                print "PICAM have rois"
            shape = rois.shape
            if shape[1] == 6 :
                if self.debugging:
                    print "PICAM  it is  nx6"
                Rois = pfp.PicamRois(shape[0])
                for i in range(shape[0]):
                    Rois.roi_array[i].x = rois[i,0]
                    Rois.roi_array[i].width = rois[i,1]
                    Rois.roi_array[i].x_binning = rois[i,2]
                    Rois.roi_array[i].y = rois[i,3]
                    Rois.roi_array[i].height = rois[i,4]
                    Rois.roi_array[i].y_binning = rois[i,5]
                    width = rois[i,1]
                if self.debugging:
                    print "PICAM The Rois are: ", Rois
                status = pfp.Picam_SetParameterRoisValue(camera, pfp.PicamParameter_Rois, pointer(Rois)) 
                if not status == "PicamError_None":
                    raise DevCOMM_ERROR("PiCam - error setting ROI- %s"% status)
                failCount = pfp.piint()
                paramsFailed = pfp.piint()
                status = pfp.Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount)) 
                if not status == "PicamError_None":
                    raise DevCOMM_ERROR("PiCam - error committing ROI Parameter Change %s" % status)
                if not failCount.value == 0:
                    raise DevCOMM_ERROR("PiCam - ROI commit failure count  > 0", failCount)
                pfp.Picam_DestroyParameters(pointer(paramsFailed))
            else:
                raise DevBAD_PARAMETER("PiCAM Rois must be 6xN array")

        errors = pfp.PicamAcquisitionErrorsMask()
        readout_count = pfp.pi64s(num_frames)
        readout_time_out = pfp.piint(-1)
        available = pfp.PicamAvailableData(0, 0)

        if self.debugging:
            print "about to call Picam_Acquire"
        status = pfp.Picam_Acquire(camera, readout_count, readout_time_out, ctypes.byref(available), ctypes.byref(errors)) 
        if not status == "PicamError_None":
            print "Picam_Acquire returned ",status
            raise DevCOMM_ERROR("PiCam - non zero return from Picam_Acquire - %s" % status)

        if self.debugging:
            print "back from aquire"

        temperature = pfp.piflt(0.0)
        status = pfp.Picam_GetParameterFloatingPointValue( camera, ctypes.c_int(pfp.PicamParameter_SensorTemperatureReading), ctypes.byref(temperature) )
        if status == "PicamError_None" :
            self.sensor_temp.record = temperature.value
            if self.debugging :
                print "PICAM read back sensor temperature ", temperature
        else:
            print "PICAM could not read back sensor temperature ", status

        readout_time = pfp.piflt(0.0)
        status = pfp.Picam_GetParameterFloatingPointValue( camera, ctypes.c_int(pfp.PicamParameter_ReadoutTimeCalculation), ctypes.byref(readout_time) )
        if status == "PicamError_None" :
            self.readout_time.record = readout_time.value
            if self.debugging :
                print "PICAM read back ReadoutTimeCalculation ", readout_time
        else:
            print "PICAM could not read back readout time ", status

        readoutstride = pfp.piint(0)
        status = pfp.Picam_GetParameterIntegerValue( camera, ctypes.c_int(pfp.PicamParameter_ReadoutStride), ctypes.byref(readoutstride) )
        if self.debugging:
            print "Picam_GetParameterIntegerValue( camera, ctypes.c_int(PicamParameter_ReadoutStride),",readoutstride," )", status
            
        if  not status == "PicamError_None" :
            raise DevCOMM_ERROR("PiCam - could not read readout stride - %s"% status)
            
        sz = readout_count.value*readoutstride.value/2
        if self.debugging:
            print "sz is ",sz, " num_frames is ", num_frames, "readout_count is ", readout_count, " readoutstride is ", readoutstride

        DataArrayType = pfp.pi16u*sz

        """ Create pointer type for the above array type """
        DataArrayPointerType = ctypes.POINTER(pfp.pi16u*sz)

        if self.debugging:
            print "PICAM - cast the read data into the pointer type"
        """ Create an instance of the pointer type, and point it to initial readout contents (memory address?) """
        DataPointer = ctypes.cast(available.initial_readout,DataArrayPointerType)

        if self.debugging:
            print "PICAM now deference the pointer"

        """ Create a separate array with readout contents """
        data = DataPointer.contents
        if self.debugging:
            print "PICAM - now make an np.empty of shorts (%d)"%sz
        ans = np.empty(sz,np.short)
        if self.debugging:
            print "PICAM - fill it in "
        ans[:] = data
        if self.debugging:
            print "PICAM reshape the data to be (%d, %d, %d)"%(num_frames, readoutstride.value/2/width, width)
        ans = ans.reshape((num_frames, readoutstride.value/2/width, width))
        self.frames.record = ans

        if self.debugging:
            print "un initialize the library"
        pfp.Picam_UninitializeLibrary()

        return 1

    ACQUIRE=acquire
