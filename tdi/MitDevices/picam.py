import MDSplus
import ctypes
from PythonForPicam import *

def pointer(x):
    """Returns a ctypes pointer"""
    ptr = ctypes.pointer(x)
    return ptr


class PICAM(MDSplus.Device):
    """
    Device class to support Princeton Instruments cameras with the PiCam library.

    methods:
      init
      store
    """

    parts = [
        {'path':':COMMENT','type':'text'},
        {'path':':SERIAL_NO','type':'numeric','options':('no_write_shot',)},
        {'path':':EXPOSURE','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':NUM_FRAMES','type':'numeric','value':30,'options':('no_write_shot',)},
        {'path':':ROIS','type':'numeric','options':('no_write_shot',)},
        {'path':':TIMEOUT','type':'numeric','value':100000,'options':('no_write_shot',)},
        {'path':':TRG_RESPONSE','type':'text', 'value':'StartOnSingleTrigger', 'options':('no_write_shot',)},

        {'path':':MODEL','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':SENSOR','type':'text','options':('no_write_model','write_once',)},
        {'path':':LIB_VERSION','type':'text','options':('no_write_model','write_once',)},
        {'path':':TEMPERATURE','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':FRAMES','type':'numeric','options':('no_write_model','write_once',)},

        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
         'options':('no_write_shot',)},
        {'path':':STORE_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('DATA_SERVER','STORE',50,None),Method(None,'STORE',head))",
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

        camera = int(self.serial_no)

        c = None
        for c in PICAM.cameras:
            if c.camera == camera :
                try:
                    c.subproc.terminate()
                except:
                    pass
                c_rec = c
        if c is None:
            c = PICAM.camera_proc(camera)
            PICAM.cameras.append(c)
        if not c.camera == camera:
            c = PICAM.camera_proc(camera)
            PICAM.cameras.append(c)

        tree = self.local_tree
        shot = self.tree.shot
        path = self.local_path
        c.subproc = subprocess.Popen('mdstcl', stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        c.subproc.stdin.write('set tree %s /shot = %d\n'%(tree, shot,))
        c.subproc.stdin.write('do/meth %s acquire\n'%(path,))
        c.subproc.stdin.flush()
    INIT=init

    def acquire(self):

        import os
        import ctypes as ctypes
        import numpy as np
        from MDSplus.mdsExceptions import DevCOMM_ERROR
        from MDSplus.mdsExceptions import DevBAD_PARAMETER

        self.debugging = os.getenv('DEBUG_DEVICES')
        exposure = float(self.exposure)
        exposure = piflt(exposure)
        num_frames = int(self.num_frames)
        timeout = int(self.timeout)
        try:
            rois = self.rois.record.data()
        except:
            rois = None

        # initialize the library
        Picam_InitializeLibrary()

        # get library version information
        major = piint()
        minor = piint()
        distribution = piint()
        release = piint()
        Picam_GetVersion(pointer(major),pointer(minor),pointer(distribution),pointer(release))
        self.lib_version.record = 'Picam Version %d.%d.%d Released %d' % (major.value,minor.value,distribution.value,release.value,)
# find the camera
#
#        Picam_GetAvailableCameraIDs(
#           const PicamCameraID** id_array,
#           piint* id_count);
#        for camera in ans:
#           if it is the one open it
#
        camera = PicamHandle()
        Picam_OpenFirstCamera(ctypes.addressof(camera))
        PicamID = PicamCameraID()  

        Picam_GetCameraID(camera,  pointer(PicamID))
        self.model.record = PicamID.model
        self.sensor.record = PicamID.sensor_name

        trigger_response = str(self.trg_response.record)
        if trigger_response == 'NoResponse':
            trigger_resp = PicamTriggerResponse_NoResponse
        elif trigger_response == 'ReadoutPerTrigger':
            trigger_resp = PicamTriggerResponse_ReadoutPerTrigger
        elif trigger_response == 'ShiftPerTrigger':
            trigger_resp = PicamTriggerResponse_ShiftPerTrigger
        elif trigger_response == 'ExposeDuringTriggerPulse':
            trigger_resp = PicamTriggerResponse_ExposeDuringTriggerPulse
        elif trigger_response == 'StartOnSingleTrigger':
            trigger_resp = PicamTriggerResponse_StartOnSingleTrigger
        else:
            raise DevBAD_PARAMETER("PiCam - TRG_RESPONSE must be one of ('NoResponse','ReadoutPerTrigger','ShiftPerTrigger','ExposeDuringTriggerPulse', 'StartOnSingleTrigger')")

        if self.debugging:
            print "Picam_SetParameterIntegerValue(camera, PicamParameter_TriggerResponse,",trigger_resp,")"
	Picam_SetParameterIntegerValue( camera, PicamParameter_TriggerResponse, trigger_resp )
	Picam_SetParameterIntegerValue( camera, PicamParameter_TriggerDetermination, PicamTriggerDetermination_NegativePolarity )
        Picam_SetParameterIntegerValue( camera, PicamParameter_OutputSignal, PicamOutputSignal_EffectivelyExposing )

        # set the exposure
        if self.debugging:
            print "Picam_SetParameterFloatingPointValue( camera, PicamParameter_ExposureTime, ",exposure,")"
        Picam_SetParameterFloatingPointValue( camera, PicamParameter_ExposureTime, exposure )

        failCount = piint()
        paramsFailed = piint()
        if self.debugging:
            print "Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount))"
        Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount))
        if self.debugging:
            print "failcount is ", failCount
        Picam_DestroyParameters(pointer(paramsFailed))

        # if there are rois set the rois
        if rois is not None:
            shape = rois.shape
            if shape[1] == 6 :
                Rois = PicamRois(shape[0])
                for i in range(shape[0]):
                    Rois.roi_array[i].x = rois[i,0]
                    Rois.roi_array[i].width = rois[i,1]
                    Rois.roi_array[i].x_binning = rois[i,2]
                    Rois.roi_array[i].y = rois[i,3]
                    Rois.roi_array[i].height = rois[i,4]
                    Rois.roi_array[i].y_binning = rois[i,5]
                status = Picam_SetParameterRoisValue(camera, PicamParameter_Rois, pointer(Rois)) 
                if not status == "PicamError_None":
                    raise DevCOMM_ERROR("PiCam - error setting ROI- %s"% status)
                failCount = piint()
                paramsFailed = piint()
                status = Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount)) 
                if not status == "PicamError_None":
                    raise DevCOMM_ERROR("PiCam - error committing ROI Parameter Change %s" % status)
                if not failCount.value == 0:
                    raise DevCOMM_ERROR("PiCam - ROI commit failure count  > 0", failCount)
                Picam_DestroyParameters(pointer(paramsFailed))
            else:
                raise DevBAD_PARAMETER("PiCAM Rois must be 6xN array")

        errors = PicamAcquisitionErrorsMask()
        readout_count = pi64s(num_frames)
        readout_time_out = piint(-1)
        available = PicamAvailableData(0, 0)

        if self.debugging:
            print "about to call Picam_Acquire"
        status = Picam_Acquire(camera, readout_count, readout_time_out, ctypes.byref(available), ctypes.byref(errors)) 
        if not status == "PicamError_None":
            print "Picam_Acquire returned ",status
            raise DevCOMM_ERROR("PiCam - non zero return from Picam_Acquire - %s" % status)

        if self.debugging:
            print "back from aquire"

        readoutstride = piint(0)
        status = Picam_GetParameterIntegerValue( camera, ctypes.c_int(PicamParameter_ReadoutStride), ctypes.byref(readoutstride) )
        if self.debugging:
            print "Picam_GetParameterIntegerValue( camera, ctypes.c_int(PicamParameter_ReadoutStride),",readoutstride," )", status
            
        if  not status == "PicamError_None" :
            raise DevCOMM_ERROR("PiCam - could not read readout stride - %s"% status)

        sz = num_frames*readoutstride.value/2
        DataArrayType = pi16u*sz

        """ Create pointer type for the above array type """
        DataArrayPointerType = ctypes.POINTER(pi16u*sz)

        """ Create an instance of the pointer type, and point it to initial readout contents (memory address?) """
        DataPointer = ctypes.cast(available.initial_readout,DataArrayPointerType)


        """ Create a separate array with readout contents """
        data = DataPointer.contents
        ans = np.empty(sz,np.short)
        ans[:] = data
        ans = ans.reshape((num_frames, 512, sz/512/num_frames))
        self.frames.record = ans
        return 1

    ACQUIRE=acquire
