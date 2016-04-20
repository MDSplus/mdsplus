import MDSplus
from PythonForPicam import *

class PICAM(MDSplus.Device):
    """
    Device class to support Princeton Instruments cameras with the PiCam library.

    methods:
      init
      store
    """

    parts = [
        {'path':':COMMENT','type':'text'},
        {'path':':EXPOSURE','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':NUM_FRAMES','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':ROIS','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':TIMEOUT','type':'numeric','value':100000,'options':('no_write_shot',)},
        {'path':':CAMERA_NO','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':TRG_RESPONSE','type':'text', 'value':'StartOnSingleTrigger', 'options':('no_write_shot',)},

        {'path':':MODEL','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':SENSOR','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':SERIAL_NO','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':LIB_VERSION','type':'numeric','options':('no_write_model','write_once',)},
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
        def __init__(self, camera, proc):
            self.camera = camera
            self.proc = proc

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

        camera = int(self.camera)
        for c in PICAM.cameras:
            if c.camera == camera :
                try:
                    c.subproc.terminate()
                except:
                    pass
                break
        if not c.camera == camera:
            PICCAM.cameras.append(PICAM.camera_proc(camera, None))

        tree = self.local_tree
        shot = self.tree.shot
        path = self.local_path
        c.subproc = subprocess.Popen('picam %s %d %s'%(tree, shot, path,),
                                      shell=True)
    INIT=init

    def acquire(self):

        import os
        import ctypes as ctypes
        import numpy as np
        from MDSplus.mdsExceptions import DevCOMM_ERROR
        from MDSplus.mdsExceptions import DevBAD_PARAMETER

        self.debugging = os.getenv('DEBUG_DEVICES')
        exposure = int(self.exposure)
        num_frames = int(self.num_frames)
        timeout = int(self.timeout)
        try:
            rois = self.rois.record
        except:
            rois = None
        camera_no = int(self.camera_no)
        picamDll = str(self.picamdll.record)
        picam = load(picamDll)

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


        Picam_GetCameraID(camera,  pointer(PicamID))
        self.model.record = PicamID.model
        self.sensor.record = PicamID.sensor_name
        self.serial_no.record = PicamID.serial_number

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

	Picam_SetParameterIntegerValue( camera, PicamParameter_TriggerResponse, PicamTriggerResponse_ReadoutPerTrigger );
	Picam_SetParameterIntegerValue( camera, PicamParameter_TriggerDetermination, PicamTriggerDetermination_NegativePolarity );
        # set the exposure
        Picam_SetParameterFloatingPointValue( camera, PicamParameter_ExposureTime, exposure )

        failCount = piint()
        paramsFailed = piint()
        Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount))
        Picam_DestroyParameters(pointer(paramsFailed))

        # if there are rois set the rois
        if not rois == None:
            shape = rois.shape
            if shape[1] == 6 :
                Rois = PicamRois(shape[0])
                for i in range(shape[0]):
                    Rois[i].x = rois[i,0]
                    Rois[i].width = rois[i,1]
                    Rois[i].x_binning = rois[i,2]
                    Rois[i].y = rois[i,3]
                    Rois[i].height = rois[i,4]
                    Rois[i].y_binning = rois[i,5]
                if Picam_SetParameterRoisValue(camera, PicamParameter_Rois, pointer(Rois)):
                    raise DevCOMM_ERROR("PiCam - error setting ROI")
                failCount = piint()
                paramsFailed = piint()
                if Picam_CommitParameters(camera, pointer(paramsFailed), ctypes.byref(failCount)):
                    raise DevCOMM_ERROR("PiCam - error committing ROI Parameter Change")
                if failCount > 0:
                    raise DevCOMM_ERROR("PiCam - ROI commit failure count > 0")
                Picam_DestroyParameters(pointer(paramsFailed))
            else:
                raise DevBAD_PARAMETER("PiCAM Rois must be 6xN array")

        errors = PicamAcquisitionErrorsMask()
        readout_count = pi64s(1)
        readout_time_out = piint(timeout)
        available = PicamAvailableData(0, 0)

        if Picam_Acquire(camera, readout_count, readout_time_out, ctypes.byref(available), ctypes.byref(errors)):
            raise DevCOMM_ERROR("PiCam - non zero return from Picam_Acquire")

        readoutstride = piint(0);
        if Picam_GetParameterIntegerValue( camera, ctypes.c_int(PicamParameter_ReadoutStride), ctypes.byref(readoutstride) ) :
            raise DevCOMM_ERROR("PiCam - could not read readout stride")

        sz = readoutstride.value/2
        DataArrayType = pi16u*sz

        """ Create pointer type for the above array type """
        DataArrayPointerType = ctypes.POINTER(pi16u*sz)

        """ Create an instance of the pointer type, and point it to initial readout contents (memory address?) """
        DataPointer = ctypes.cast(available.initial_readout,DataArrayPointerType)


        """ Create a separate array with readout contents """
        data = DataPointer.contents
        ans = np.empty(sz,np.short)
        ans[:] = data
        ans = ans.reshape((512, sz/512))
        self.frames.record = ans
        return 1

    ACQUIRE=acquire
