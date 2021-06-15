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

from MDSplus import version, mdsExceptions, Device, Data, Float32, Int64, String, Uint16
from threading import Thread
from ctypes import cdll, memmove, byref, c_int, c_char, c_void_p, c_byte, cast
from ctypes import c_char_p, c_long, c_longlong, c_ushort, POINTER, c_double
from ctypes import sizeof, CFUNCTYPE, resize, addressof
import math
import numpy as np
if version.ispy2:
    import Queue as queue
else:
    import queue


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


try:
    from PI_CAMStruct import *  # TODO: avoid module level import and avoid *
except ImportError:
    class PicamAdvanced():
        IntegerValueChangedCallback = CFUNCTYPE(c_int, c_void_p, c_int, c_int)
        IsRelevantChangedCallback = CFUNCTYPE(c_int, c_void_p, c_int, c_int)
        LargeIntegerValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_longlong)
        FloatingPointValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_double)
        PulseValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_longlong)
        ModulationsValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_longlong)
        CollectionConstraintChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, POINTER(PicamCollectionConstraint))
        AcquisitionUpdatedCallback = CFUNCTYPE(c_int, c_void_p, POINTER(
            PicamAvailableData), POINTER(PicamAcquisitionStatus))
        OnlineReadoutRateCalculationChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_double)
        ReadoutStrideChangedCallback = CFUNCTYPE(c_int, c_void_p, c_int, c_int)
        ParameterLargeIntegerValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, c_longlong)
        ParameterRoisValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, POINTER(PicamRois))
        ParameterPulseValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, POINTER(PicamPulse))
        ParameterModulationsValueChangedCallback = CFUNCTYPE(
            c_int, c_void_p, c_int, POINTER(PicamModulations))


class CameraInstanceValues():
    def __init__(self):
        self.camera = c_void_p(0)
        self.calculatedBufferSize = 0
        self.readoutStride = 0
        self.frameStride = 0
        self.framesPerReadout = 0
        self.framesSize = 0
        self.dataBuffer = None


class PI_CAM(Device):
    """Princeton Instrument CCD camera"""
    parts = [{'path': ':MODEL_NAME', 'type': 'text'},
             {'path': ':SERIAL_NUM', 'type': 'numeric'},
             {'path': ':COMMENT', 'type': 'text'},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 100},
             {'path': ':CLOCK_START', 'type': 'numeric'},
             {'path': ':CLOCK_DURAT', 'type': 'numeric'},
             {'path': ':CLOCK_SOURCE', 'type': 'numeric'},
             {'path': ':EXP_TIME', 'type': 'numeric', 'value': 100},
             {'path': ':TEMPERTURE', 'type': 'numeric', 'value': -70},
             {'path': ':SHUTTER_MODE', 'type': 'text', 'value': 'NORMAL'},
             {'path': ':READOUT_TIME', 'type': 'numeric'},
             {'path': ':CLEANS', 'type': 'numeric'},
             {'path': ':SKIP', 'type': 'numeric'},
             {'path': ':CHIP_OR', 'type': 'text', 'value': 'NORMAL'},
             {'path': ':ADC', 'type': 'text', 'value': 'HIGH_SPEED'},
             {'path': '.SPECTROMETER', 'type': 'structure'},
             {'path': '.SPECTROMETER:GRATING', 'type': 'numeric', 'value': 1},
             {'path': '.SPECTROMETER:LAMBDA_0', 'type': 'numeric'},
             {'path': '.SPECTROMETER:SLIT', 'type': 'numeric'}]

    for i in range(0, 24):
        parts.append({'path': '.SPECTRUM_%02d' % (i+1), 'type': 'structure'})
        parts.append({'path': '.SPECTRUM_%02d:NAME' % (i+1), 'type': 'text'})
        parts.append(
            {'path': '.SPECTRUM_%02d:ROI' % (i+1), 'type': 'structure'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:X' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:WIDTH' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:X_BIN' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:Y' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:HEIGHT' % (i+1), 'type': 'numeric'})
        parts.append(
            {'path': '.SPECTRUM_%02d.ROI:Y_BIN' % (i+1), 'type': 'numeric'})
        parts.append({'path': '.SPECTRUM_%02d:DATA' % (
            i+1), 'type': 'signal', 'options': ('no_write_model', 'no_compress_on_put')})
    del(i)

    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PC_SERVER','READY',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':START_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PC_SERVER','READY',60,None),Method(None,'start_store',head))",
                  'options': ('no_write_shot',)})
    parts.append({'path': ':STOP_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('PC_SERVER','POST_PULSE_CHECK',50,None),Method(None,'stop_store',head))",
                  'options': ('no_write_shot',)})

    parts.append({'path': ':NUM_FRAMES', 'type': 'numeric', 'value': 1})
    parts.append({'path': ':CTRL_MODE', 'type': 'text', 'value': 'LOCAL'})
    parts.append({'path': ':CTRL_SERVER', 'type': 'text'})

    parts.append({'path': '.SENSOR', 'type': 'structure'})
    parts.append({'path': '.SENSOR.INFORMATION', 'type': 'structure'})
    parts.append({'path': '.SENSOR.INFORMATION:TYPE', 'type': 'text'})
    parts.append({'path': '.SENSOR.INFORMATION:CCD_CHARACT', 'type': 'text'})

    parts.append({'path': '.SENSOR.INFORMATION.PIXEL', 'type': 'structure'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.PIXEL:GAP_HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.PIXEL:GAP_WIDTH', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.PIXEL:HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.PIXEL:WIDTH', 'type': 'numeric'})

    parts.append({'path': '.SENSOR.INFORMATION.ACTIVE', 'type': 'structure'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:BOTTOM_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:LEFT_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:RIGHT_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:TOP_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:WIDTH', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.ACTIVE:SEC_HEIGHT', 'type': 'numeric'})

    parts.append({'path': '.SENSOR.INFORMATION.MASKED', 'type': 'structure'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.MASKED:BOTTOM_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.MASKED:TOP_M', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.MASKED:HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.INFORMATION.MASKED:SEC_HEIGHT', 'type': 'numeric'})

    parts.append({'path': '.SENSOR.LAYOUT', 'type': 'structure'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE', 'type': 'structure'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:BOTTOM_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:LEFT_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:RIGHT_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:TOP_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:WIDTH', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.ACTIVE:HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.LAYOUT.ACTIVE:SEC_HEIGHT', 'type': 'numeric'})

    parts.append({'path': '.SENSOR.LAYOUT.MASKED', 'type': 'structure'})
    parts.append({'path': '.SENSOR.LAYOUT.MASKED:BOTTOM_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.MASKED:TOP_M', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.LAYOUT.MASKED:HEIGHT', 'type': 'numeric'})
    parts.append(
        {'path': '.SENSOR.LAYOUT.MASKED:SEC_HEIGHT', 'type': 'numeric'})

    parts.append({'path': '.SENSOR.CLEAN', 'type': 'structure'})
    parts.append({'path': '.SENSOR.CLEAN.CYCLE_COUNT', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.CLEAN.CYCLE_H', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.CLEAN.SEC_FIN_H', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.CLEAN.SEC_FIN_HC', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.CLEAN.SER_REG', 'type': 'numeric'})
    parts.append({'path': '.SENSOR.CLEAN.UNTIL_TRIG', 'type': 'numeric'})

    picamlib = None
    cameras = {}
    workers = {}

    camera = c_void_p(0)
    calculatedBufferSize = 0
    readoutStride = 0
    frameStride = 0
    framesPerReadout = 0
    frameSize = 0
    dataBuffer = None

    callBacks = {}
    dataQueue = None

    picamParameter = PicamParameter()
    instanceValues = {}

    CONNECTION = 1
    RESTORE_CONNECTION = 2

    ERROR = -1
    SUCCESS = 1

    shutterModeDict = {'NORMAL': PicamShutterTimingMode.Normal,
                       'OPEN': PicamShutterTimingMode.AlwaysOpen,  'CLOSE': PicamShutterTimingMode.AlwaysClosed}
    chipOrientationModeDict = {'NORMAL': PicamOrientationMask.Normal,
                               'FLIPPED_H': PicamOrientationMask.FlippedHorizontally, 'FLIPPED_V': PicamOrientationMask.FlippedVertically}
    adcQualityDict = {'LOW_NOISE': PicamAdcQuality.LowNoise, 'HIGH_SPEED': PicamAdcQuality.HighSpeed,
                      'HIGH_CAPACITY': PicamAdcQuality.HighCapacity, 'ELECTRON_MULTIPLIED': PicamAdcQuality.ElectronMultiplied}

    def getEnumString(self, _type,  value):
        string = c_char_p(0)
        PI_CAM.picamlib.Picam_GetEnumerationString(_type, value, byref(string))
        msg = string.value  # to check memory management
        PI_CAM.picamlib.Picam_DestroyString(string)
        return msg

    def getErrorSting(self, error):
        if(error == PicamError.NoError):
            return "Succeeded"
        else:
            return self.getEnumString(PicamEnumeratedType.Error, error)


# saveInfo and restoreInfo allow to handle open file descriptors
    def saveInfo(self):
        global calculatedBufferSize
        global readoutStride
        global frameStride
        global framesPerReadout
        global frameSize
        global dataBuffer

        PI_CAM.cameras[self.nid] = self.camera
        camVarInst = CameraInstanceValues()
        camVarInst.calculatedBufferSize = calculatedBufferSize
        camVarInst.readoutStride = readoutStride
        camVarInst.frameStride = frameStride
        camVarInst.framesPerReadout = framesPerReadout
        camVarInst.frameSize = frameSize
        camVarInst.dataBuffer = dataBuffer

        PI_CAM.instanceValues[self.nid] = camVarInst

        print ("saveInfo ", camVarInst.calculatedBufferSize, camVarInst.readoutStride, camVarInst.frameStride,
               camVarInst.framesPerReadout, camVarInst.dataBuffer, camVarInst.frameSize)

    def restoreInfo(self):
        global calculatedBufferSize
        global readoutStride
        global frameStride
        global framesPerReadout
        global frameSize
        global dataBuffer

        inited = c_long(0)

        if PI_CAM.picamlib is None:
            # CDLL("C:\Program Files\Princeton Instruments\PICam\Runtime\Picam.dll")
            PI_CAM.picamlib = cdll.picam
            error = PicamError.NoError

            error = PI_CAM.picamlib.Picam_InitializeLibrary()
            if error != PicamError.NoError:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot initialize PiCam library ')
                return mdsExceptions.TclFAILED_ESSENTIAL

        if not self.nid in PI_CAM.cameras.keys():
            # Camera not connected
            error = PI_CAM.picamlib.Picam_IsLibraryInitialized(byref(inited))
            if error == PicamError.NoError and inited:
                camID = PicamCameraID()
                isDemo = False
                modelName = ""
                try:
                    modelValue = self.model_name.data()
                    try:
                        modelName, openDemo = modelValue.split('::')
                        if openDemo == 'demo':
                            isDemo = True
                            print ("WARNING : Activate Demo camera ",
                                   modelName, serialNumber)
                    except:
                        modelName = modelValue
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Missing CCD model Id : Try to open first connected camera')
                    modelName = ""

                try:
                    serialNumber = (self.serial_num.data())
                except:
                    Data.execute('DevLogErr($1,$2)', self.getNid(),
                                 'Missing CCD serial number')
                    serialNumber = c_char_p('0000000000')

                if not isDemo:
                    if modelName == "":
                        error = PI_CAM.picamlib.Picam_OpenFirstCamera(
                            byref(self.camera))
                        if error == PicamError.NoError:
                            error = PI_CAM.picamlib.Picam_GetCameraID(
                                self.camera, byref(camID))
                        if error == PicamError.NoError:
                            error = PI_CAM.picamlib.Picam_CloseCamera(
                                self.camera)
                        if error == PicamError.NoError:
                            name = c_char_p(0)
                            error = PI_CAM.picamlib.Picam_GetEnumerationString(
                                PicamEnumeratedType.Model, camID.model, byref(name))
                            self.model_name.putData(String(name.value))
                            self.serial_num.putData(
                                Int64(int(camID.serial_number)))

# if error == PicamError.NoError :
##                                error = PI_CAM.picamlib.PicamAdvanced_OpenCameraDevice( byref(camID), byref(self.camera) );
##                                Data.execute('DevLogErr($1,$2)', self.getNid(), 'PicamAdvaced_OpenCameraDevice '+ self.getErrorSting(error) )
                    else:
                        numCamsAvailable = c_int(0)
                        camIDs = c_void_p(0)
                        PI_CAM.picamlib.Picam_GetAvailableCameraIDs(
                            byref(camIDs), byref(numCamsAvailable))
                        print (">>>>>>>>>>>> numCamsAvailable ",
                               numCamsAvailable, camIDs)
                        if numCamsAvailable != 0:
                            camIDs = cast(camIDs, POINTER(
                                (PicamCameraID) * int(numCamsAvailable.value))).contents
                            cameraFound = False
                            for i in range(numCamsAvailable.value):
                                name = c_char_p(0)
                                error = PI_CAM.picamlib.Picam_GetEnumerationString(
                                    PicamEnumeratedType.Model, camIDs[i].model, byref(name))
                                print (">>>>>>>>>>>>", name.value,  int(
                                    camIDs[i].serial_number), serialNumber)
                                if name.value == modelName and int(camIDs[i].serial_number) == serialNumber:
                                    camID = camIDs[i]
                                    cameraFound = True
                                    break
                                PI_CAM.picamlib.Picam_DestroyString(name)
                            if not cameraFound:
                                Data.execute('DevLogErr($1,$2)', self.getNid(
                                ), 'Camera model %s serial number %s not found ' % (modelName, serialNumber))
                                return mdsExceptions.TclFAILED_ESSENTIAL

                        #PI_CAM.picamlib.Picam_DestroyCameraIDs( camIDs );
                else:
                    # Demo camera
                    model_array = c_void_p(0)
                    model_count = c_int(0)
                    error = PI_CAM.picamlib.Picam_GetAvailableDemoCameraModels(
                        byref(model_array), byref(model_count))
                    if error != PicamError.NoError:
                        print("Picam not aivalable demo camera \n")
                    else:
                        print("Picam ivalable demo camera %d\n" %
                              (model_count.value))

                    model_array = cast(model_array, POINTER(
                        c_int * int(model_count.value))).contents

                    model_name = c_char_p(0)
                    for i in range(model_count.value):
                        error = PI_CAM.picamlib.Picam_GetEnumerationString(
                            PicamEnumeratedType.Model, model_array[i], byref(model_name))
                        if model_name.value == modelName:
                            break
                        PI_CAM.picamlib.Picam_DestroyString(model_name)

                    print (i,  model_count.value)
                    if i < model_count.value-1:
                        print("[%d] Picam aivalable Demo camera model %d\n" %
                              (i, model_array[i]))
                        print("Picam aivalable Demo camera model %s\n" %
                              (model_name.value))
                        error = PI_CAM.picamlib.Picam_ConnectDemoCamera(
                            model_array[i], serialNumber, byref(camID))
                    else:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'Open demo camera failed, camera not found ')
                        return mdsExceptions.TclFAILED_ESSENTIAL

# if error == PicamError.NoError :
##                        error = PI_CAM.picamlib.Picam_OpenCamera( byref(camID), byref(self.camera) )

                if error == PicamError.NoError:
                    error = PI_CAM.picamlib.PicamAdvanced_OpenCameraDevice(
                        byref(camID), byref(self.camera))
                    if error == PicamError.NoError:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'PicamAdvaced_OpenCameraDevice ' + self.getErrorSting(error))

                if error == PicamError.NoError:
                    string = c_char_p(0)
                    error = PI_CAM.picamlib.Picam_GetEnumerationString(
                        PicamEnumeratedType.Model, camID.model, byref(string))

                if error == PicamError.NoError:
                    print("%s" % (string.value))
                    print("(SN:%s) [%s]\n" %
                          (camID.serial_number, camID.sensor_name))
                    PI_CAM.picamlib.Picam_DestroyString(string)
                else:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Open camera failed ' + self.getErrorSting(error))
                    return mdsExceptions.TclFAILED_ESSENTIAL

                calculatedBufferSize = 0
                readoutStride = 0
                frameStride = 0
                framesPerReadout = 0
                frameSize = 0
                dataBuffer = None

                self.registerCameraCallbacks()
                self.initializeCalculatedBufferSize()

            else:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'PiCam library initialization failed ' + self.getErrorSting(error))
                return mdsExceptions.TclFAILED_ESSENTIAL
            return PI_CAM.CONNECTION
        else:
            self.camera = PI_CAM.cameras[self.nid]
            camVarInst = PI_CAM.instanceValues[self.nid]
            calculatedBufferSize = camVarInst.calculatedBufferSize
            readoutStride = camVarInst.readoutStride
            frameStride = camVarInst.frameStride
            framesPerReadout = camVarInst.framesPerReadout
            frameSize = camVarInst.frameSize
            dataBuffer = camVarInst.dataBuffer

            print ("retoreInfo ", calculatedBufferSize, readoutStride,
                   frameStride, framesPerReadout, dataBuffer, frameSize)

# else:
# PI_CAM.picamlib.Picam_UninitializeLibrary();
##                PI_CAM.picamlib = None
##                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Uninitialized PI_CAM library camera not found '+ self.getErrorSting(error) )
# return mdsExceptions.TclFAILED_ESSENTIAL

        return PI_CAM.RESTORE_CONNECTION

    def closeInfo(self):
        try:
            print ("Close Info")
            if self.nid in PI_CAM.cameras.keys():
                self.camera = PI_CAM.cameras[self.nid]
                del(PI_CAM.cameras[self.nid])
                del(PI_CAM.instanceValues[self.nid])
                #error = PI_CAM.picamlib.PicamAdvanced_CloseCamera( self.camera )
                print ("Close Camera")
                isRunning = c_int(0)
                error = PI_CAM.picamlib.Picam_IsAcquisitionRunning(
                    self.camera, byref(isRunning))
                print ("Camera is running", isRunning.value)

                error = PI_CAM.picamlib.PicamAdvanced_CloseCameraDevice(
                    self.camera)
                if error != PicamError.NoError:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'Cannot close PI_CAM Camera ' + self.getErrorSting(error))
                else:
                    print ("Camera Closed")
                PI_CAM.picamlib.Picam_UninitializeLibrary()
                PI_CAM.picamlib = None
        except Exception as e:
            Data.execute('DevLogErr($1,$2)', self.getNid(),
                         'Cannot close PI_CAM Camera ' + str(e))
        return


# ////////////////////////////////////////////////////////////////////////////////
# // IsRelevantChanged
# // - called when the relevance of a parameter changes due to another parameter
# //   change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def IsRelevantChanged(self, camera,  parameter,  relevant):
        # - generate log message
        print ("---- IsRelevantChanged ----")
        if relevant:
            print (self.getEnumString(PicamEnumeratedType.Parameter,
                                      parameter), " relevance changed to true")
        else:
            print (self.getEnumString(PicamEnumeratedType.Parameter,
                                      parameter), " relevance changed to false")
        print ("---------------------------")
        return PicamError.NoError

# ////////////////////////////////////////////////////////////////////////////////
# // ValueAccessChanged
# // - called when the value access of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ValueAccessChanged(self, camera, parameter, access):
        # - generate log message
        print ("----- ValueAccessChanged access ", access)
        #print  self.getEnumString( PicamEnumeratedType.Parameter, parameter ), " value access changed to ", self.GetEnumString( PicamEnumeratedType.ValueAccess, access )
        print ("---- Parameter   " +
               self.getEnumString(PicamEnumeratedType.Parameter, parameter))
        print ("---- ValueAccess " +
               self.getEnumString(PicamEnumeratedType.ValueAccess, access))
        print ("---------------------------")
        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // RegisterParameterCallbacks
# // - initializes callbacks for any parameter changes from the model of the open
# //   camera
# ////////////////////////////////////////////////////////////////////////////////
    def registerParameterCallbacks(self):
        # - get the camera model
        picamModel = c_void_p(0)
        error = PI_CAM.picamlib.PicamAdvanced_GetCameraModel(
            self.camera, byref(picamModel))
        if error != PicamError.NoError:
            print ("Failed to get camera model.", self.getErrorSting(error))
            return

        # - register with each parameter
        parameters = c_void_p(0)
        count = c_long()
        error = PI_CAM.picamlib.Picam_GetParameters(
            picamModel, byref(parameters), byref(count))
        if error != PicamError.NoError:
            print ("Failed to get camera parameters.",
                   self.getErrorSting(error))
            return

        parameters = cast(parameters, POINTER(c_int))

# ParameterIntegerValueChanged
# ParameterLargeIntegerValueChanged
# ParameterFloatingPointValueChanged
# ParameterRoisValueChanged
# ParameterPulseValueChanged
# ParameterModulationsValueChanged

        IsRelevantChangedCallback = PicamAdvanced.IntegerValueChangedCallback(
            self.IsRelevantChanged)
        PI_CAM.callBacks["IsRelevantChanged"] = IsRelevantChangedCallback

        ValueAccessChangedCallback = PicamAdvanced.IntegerValueChangedCallback(
            self.ValueAccessChanged)
        PI_CAM.callBacks["ValueAccessChanged"] = ValueAccessChangedCallback

        IntegerValueChangedCallback = PicamAdvanced.IntegerValueChangedCallback(
            self.ParameterIntegerValueChanged)
        PI_CAM.callBacks["ParameterIntegerValueChanged"] = IntegerValueChangedCallback

        FloatingPointValueChangedCallback = PicamAdvanced.FloatingPointValueChangedCallback(
            self.ParameterFloatingPointValueChanged)
        PI_CAM.callBacks["ParameterFloatingPointValueChanged"] = FloatingPointValueChangedCallback

        CollectionConstraintChangedCallback = PicamAdvanced.CollectionConstraintChangedCallback(
            self.CollectionConstraintChanged)
        PI_CAM.callBacks["CollectionConstraintChanged"] = CollectionConstraintChangedCallback

        ParameterLargeIntegerValueChangedCallback = PicamAdvanced.ParameterLargeIntegerValueChangedCallback(
            self.ParameterLargeIntegerValueChanged)
        PI_CAM.callBacks["ParameterLargeIntegerValueChanged"] = ParameterLargeIntegerValueChangedCallback

        ParameterRoisValueChangedCallback = PicamAdvanced.ParameterRoisValueChangedCallback(
            self.ParameterRoisValueChanged)
        PI_CAM.callBacks["ParameterRoisValueChanged"] = ParameterRoisValueChangedCallback

        ParameterPulseValueChangedCallback = PicamAdvanced.ParameterPulseValueChangedCallback(
            self.ParameterPulseValueChanged)
        PI_CAM.callBacks["ParameterPulseValueChanged"] = ParameterPulseValueChangedCallback

        ParameterModulationsValueChangedCallback = PicamAdvanced.ParameterModulationsValueChangedCallback(
            self.ParameterModulationsValueChanged)
        PI_CAM.callBacks["ParameterModulationsValueChanged"] = ParameterModulationsValueChangedCallback

        for i in range(count.value):

            # - register for relevance changes
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForIsRelevantChanged(
                picamModel, c_int(parameters[i]), PI_CAM.callBacks["IsRelevantChanged"])
            if error != PicamError.NoError:
                print ("Failed to register for relevance changes.",
                       self.getErrorSting(error))
                return

##            print "Param : ", self.getEnumString( PicamEnumeratedType.Parameter, parameters[i] )

            # - register for value access changes
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForValueAccessChanged(
                picamModel, c_int(parameters[i]), PI_CAM.callBacks["ValueAccessChanged"])
            if error != PicamError.NoError:
                print ("Failed to register for access changes.",
                       self.getErrorSting(error))
                return

            # - register for value changes
            if not self.registerParameterValueChangedCallback(picamModel, parameters[i]):
                continue

            # - register for constraint changes
            if not self.registerConstraintChangedCallback(picamModel, parameters[i]):
                continue

        PI_CAM.picamlib.Picam_DestroyParameters(parameters)


# ////////////////////////////////////////////////////////////////////////////////
# // ParameterLargeIntegerValueChanged
# // - called when a large integer parameter changes due to another parameter
# //   change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterLargeIntegerValueChanged(self, camera, parameter, value):
        #/ - generate log message
        message = self.getEnumString(
            PicamEnumeratedType.Parameter, parameter) + " value changed to " + str(value)
        print (message)

        return PicamError.NoError

# ////////////////////////////////////////////////////////////////////////////////
# // ParameterRoisValueChanged
# // - called when a rois parameter changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterRoisValueChanged(self, camera, parameter, value):

        try:
            region = cast(value, POINTER(PicamRois)).contents
        except:
            return PicamError.NoError

        #/ - generate log message
        print (self.getEnumString(PicamEnumeratedType.Parameter, parameter))
        print ("region.roi_count = ", region.roi_count)

        for i in range(region.roi_count):
            print ("x         = ", region.roi_array[i].x)
            print ("width     = ", region.roi_array[i].width)
            print ("x_binning = ", region.roi_array[i].x_binning)

            print ("y         = ", region.roi_array[i].y)
            print ("height    = ", region.roi_array[i].height)
            print ("y_binning = ", region.roi_array[i].y_binning)

        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // ParameterPulseValueChanged
# // - called when a pulse parameter changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterPulseValueChanged(self, camera, parameter, value):

        try:
            value = cast(value, POINTER(PicamPulse)).contents
        except:
            return PicamError.NoError

        #/ - generate log message
        message = self.getEnumString(PicamEnumeratedType.Parameter, parameter) + \
            " value changed to " + \
            "delayed to " + str(value.delay) + \
            " of width " + str(value.width)
        print(message)
        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // ParameterModulationsValueChanged
# // - called when a modulations parameter changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterModulationsValueChanged(self, camera, parameter, value):

        try:
            value = cast(value, POINTER(PicamModulations)).contents
        except:
            return PicamError.NoError

        #/ - generate log message
        message = self.getEnumString(PicamEnumeratedType_Parameter, parameter) + \
            " value changed to:\r\n"

        for m in range(value.modulation_count):
            message += "\tcos(" + \
                str(value.modulation_array[m].frequency) + \
                "t + " + \
                str(value.modulation_array[m].phase) + \
                "pi/180) lasting " + \
                str(value.modulation_array[m].duration) + \
                " with output signal cos(" + \
                str(value.modulation_array[m].output_signal_frequency) + "t)"

            if m != (value.modulation_count-1):
                message += "\r\n"

        print(message)

        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // RegisterParameterValueChangedCallback
# // - initializes callback for any parameter value changes from the camera model
# ///////////////////////////////////////////////////////////////////////////////
    def registerParameterValueChangedCallback(self, model, parameter):

        #/ - get the value type
        valueType = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterValueType(
            model, parameter, byref(valueType))
        if error != PicamError.NoError:
            print ("Failed to get parameter value type..",
                   self.getErrorSting(error))
            return False

        valueType = valueType.value

        #/ - register for value changes
        if valueType == PicamValueType.Integer or valueType == PicamValueType.Boolean or valueType == PicamValueType.Enumeration:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForIntegerValueChanged(
                model, parameter, PI_CAM.callBacks["ParameterIntegerValueChanged"])
        elif valueType == PicamValueType.LargeInteger:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForLargeIntegerValueChanged(
                model, parameter,  PI_CAM.callBacks["ParameterLargeIntegerValueChanged"])
        elif valueType == PicamValueType.FloatingPoint:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForFloatingPointValueChanged(
                model, parameter, PI_CAM.callBacks["ParameterFloatingPointValueChanged"])
        elif valueType == PicamValueType.Rois:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForRoisValueChanged(
                model, parameter, PI_CAM.callBacks["ParameterRoisValueChanged"])
        elif valueType == PicamValueType.Pulse:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForPulseValueChanged(
                model, parameter, PI_CAM.callBacks["ParameterPulseValueChanged"])
        elif valueType == PicamValueType_Modulations:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForModulationsValueChanged(
                model, parameter, PI_CAM.callBacks["ParameterModulationsValueChanged"])
        else:
            print ("Unexpected value type.",  valueType)
            return False

        if error != PicamError.NoError:
            print ("Failed to register for value changes.",
                   valueType, self.getErrorSting(error))
            return False

        return True

# ////////////////////////////////////////////////////////////////////////////////
# // RegisterConstraintChangedCallback
# // - initializes callback for any parameter constraint changes from the camera
# //   model
# ////////////////////////////////////////////////////////////////////////////////
    def registerConstraintChangedCallback(self, model, parameter):
        #/ - get the constraint type
        constraintType = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterConstraintType(
            model, parameter, byref(constraintType))
        if error != PicamError.NoError:
            print ("Failed to get parameter constraint type.",
                   self.getErrorSting(error))
            return False

        constraintType = constraintType.value

# CollectionConstraintChanged
# RangeConstraintChanged
# RoisConstraintChanged
# PulseConstraintChanged
# ModulationsConstraintChanged

        #/ - register for constraint changes
        if constraintType == PicamConstraintType._None:
            error = PicamError.NoError
        elif constraintType == PicamConstraintType.Collection:
            error = PI_CAM.picamlib.PicamAdvanced_RegisterForDependentCollectionConstraintChanged(
                model, parameter, PI_CAM.callBacks["CollectionConstraintChanged"])
# elif constraintType == PicamConstraintType.Range:
##            error = PicamAdvanced_RegisterForDependentRangeConstraintChanged( model, parameter, RangeConstraintChanged )
# elif constraintType == PicamConstraintType.Rois:
##            error = PicamAdvanced_RegisterForDependentRoisConstraintChanged( model, parameter, RoisConstraintChanged )
# elif constraintType == PicamConstraintType.Pulse:
##            error = PicamAdvanced_RegisterForDependentPulseConstraintChanged( model, parameter, PulseConstraintChanged )
# elif constraintType == PicamConstraintType_Modulations:
##            error = PicamAdvanced_RegisterForDependentModulationsConstraintChanged( model, parameter, ModulationsConstraintChanged )
        else:
            print ("Unexpected constraint type. ", constraintType)
            return False

        if error != PicamError.NoError:
            print ("Failed to register for constraint changes. ",
                   self.getErrorSting(error))
            return False

        return True

# ////////////////////////////////////////////////////////////////////////////////
# // CollectionConstraintChanged
# // - called when a collection constraint of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def CollectionConstraintChanged(self, camera, parameter, constraint):

        #/ - get the value type
        valueType = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterValueType(
            camera, parameter, byref(valueType))
        if error != PicamError.NoError:
            print ("Failed to get parameter value type.",
                   self.getErrorSting(error))
            return PicamError.NoError

        constraint = cast((constraint), POINTER(
            PicamCollectionConstraint)).contents
        valueType = valueType.value

        #/ - generate log message
        message = self.getEnumString(
            PicamEnumeratedType.Parameter, parameter) + " collection constraint changed to:\r\n"
        if constraint.values_count == 0:
            message += "\t<empty set>"
        else:
            message += "\t" + str(constraint.values_count) + " Value(s):"
            for i in range(constraint.values_count):
                message += "\r\n\t\t"
                if valueType == PicamValueType.Integer:
                    message += str(int(constraint.values_array[i]))
                elif valueType == PicamValueType.Boolean:
                    if constraint.values_array[i]:
                        message += "true"
                    else:
                        message += "false"
                elif valueType == PicamValueType.Enumeration:
                    enumType = c_int(0)
                    error = PI_CAM.picamlib.Picam_GetParameterEnumeratedType(
                        camera, parameter, byref(enumType))
                    if error != PicamError.NoError:
                        print ("Failed to get enumerated type. ",
                               self.getErrorSting(error))
                        return PicamError.NoError
                    message += self.getEnumString(enumType,
                                                  int(constraint.values_array[i]))

                elif valueType == PicamValueType.LargeInteger:
                    message += str(int(constraint.values_array[i]))
                elif valueType == PicamValueType.FloatingPoint:
                    message += str(constraint.values_array[i])
                else:
                    message += str(constraint.values_array[i]) + \
                        " (unknown value type " + str(valueType) + ")"

        print (message)

        return PicamError.NoError

# ////////////////////////////////////////////////////////////////////////////////
# // RangeConstraintChanged
# // - called when a range constraint of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
# def RangeConstraintChanged( self, camera, parameter,  PicamRangeConstraint* constraint )
##
# try :
##        constraint = cast(constraint, POINTER(PicamRangeConstraint)).contents
# except:
# return PicamError.NoError
##
##
# / - get the value type
##    valueType = c_int(0)
##    error = PI_CAM.picamlib.Picam_GetParameterValueType( camera, parameter, byref(valueType) );
# if error != PicamError.NoError :
##        print "Failed to get parameter value type.", self.getErrorSting(error)
# return PicamError.NoError
##
##    valueType = valueType.value
##
# / - generate log message
# message = self.getEnumString( PicamEnumeratedType_Parameter, parameter ) + \
##         " range constraint changed to:\r\n"
# if( constraint.empty_set )
##        message += "\t<empty set>";
# else
# {
# if valueType == PicamValueType.Integer:
# message += "\tMinimum: " + str(cast( constraint.minimum, c_int).value ) << L"\r\n";
# woss << L"\tMaximum: "
# << static_cast<piint>( constraint->maximum ) << L"\r\n";
# woss << L"\tIncrement: "
# << static_cast<piint>( constraint->increment );
# if( constraint->outlying_values_count )
# {
# woss << L"\r\n\tIncluding "
# << constraint->outlying_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << static_cast<piint>(
# constraint->outlying_values_array[i] );
# }
# }
# if( constraint->excluded_values_count )
# {
# woss << L"\r\n\tExcluding "
# << constraint->excluded_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << static_cast<piint>(
# constraint->excluded_values_array[i] );
# }
# }
# break;
# case PicamValueType_LargeInteger:
# woss << L"\tMinimum: "
# << static_cast<pi64s>( constraint->minimum ) << L"\r\n";
# woss << L"\tMaximum: "
# << static_cast<pi64s>( constraint->maximum ) << L"\r\n";
# woss << L"\tIncrement: "
# << static_cast<pi64s>( constraint->increment );
# if( constraint->outlying_values_count )
# {
# woss << L"\r\n\tIncluding "
# << constraint->outlying_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << static_cast<pi64s>(
# constraint->outlying_values_array[i] );
# }
# }
# if( constraint->excluded_values_count )
# {
# woss << L"\r\n\tExcluding "
# << constraint->excluded_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << static_cast<pi64s>(
# constraint->excluded_values_array[i] );
# }
# }
# break;
# case PicamValueType_FloatingPoint:
# woss << L"\tMinimum: " << constraint->minimum << L"\r\n";
# woss << L"\tMaximum: " << constraint->maximum << L"\r\n";
# woss << L"\tIncrement: " << constraint->increment;
# if( constraint->outlying_values_count )
# {
# woss << L"\r\n\tIncluding "
# << constraint->outlying_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << constraint->outlying_values_array[i];
# }
# }
# if( constraint->excluded_values_count )
# {
# woss << L"\r\n\tExcluding "
# << constraint->excluded_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << constraint->excluded_values_array[i];
# }
# }
# break;
# default:
# woss << L"\tMinimum: "
# << constraint->minimum
# << L" (unknown value type " << valueType << L")\r\n";
# woss << L"\tMaximum: "
# << constraint->maximum
# << L" (unknown value type " << valueType << L")\r\n";
# woss << L"\tIncrement: "
# << constraint->increment
# << L" (unknown value type " << valueType << L")";
# if( constraint->outlying_values_count )
# {
# woss << L"\r\n\tIncluding "
# << constraint->outlying_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << constraint->outlying_values_array[i]
# << L" (unknown value type " << valueType << L")";
# }
# }
# if( constraint->excluded_values_count )
# {
# woss << L"\r\n\tExcluding "
# << constraint->excluded_values_count << L" Value(s):";
# for( piint i = 0;
# i < constraint->excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t"
# << constraint->excluded_values_array[i]
# << L" (unknown value type " << valueType << L")";
# }
# }
# break;
# }
# }
##
##    LogEvent( woss.str() );
##
# return PicamError_None;
# }
##
# ////////////////////////////////////////////////////////////////////////////////
# // RoisConstraintChanged
# // - called when a rois constraint of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
# PicamError PIL_CALL RoisConstraintChanged(
# PicamHandle camera,
# PicamParameter parameter,
# const PicamRoisConstraint* constraint )
# {
##    UNREFERENCED_PARAMETER( camera );
##
# // - do nothing if parameters dialog is not open
# if( !parameters_ )
# return PicamError_None;
##
# // - generate log message
# std::wostringstream woss;
##    woss << GetEnumString( PicamEnumeratedType_Parameter, parameter )
# << L" rois constraint changed to:\r\n";
# if( constraint->empty_set )
# woss << L"\t<empty set>";
# else
# {
# // - generate maximum count message
# woss << L"\tMaximum Count: "
# << constraint->maximum_roi_count << L"\r\n";
##
# // - generate rois rules message
# woss << L"\tRules: "
# << GetEnumString(
# PicamEnumeratedType_RoisConstraintRulesMask,
# constraint->rules )
# << L"\r\n";
##
# // - generate x constraint message
# woss << L"\tX Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << static_cast<piint>( constraint->x_constraint.minimum )
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << static_cast<piint>( constraint->x_constraint.maximum )
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << static_cast<piint>( constraint->x_constraint.increment );
# if( constraint->x_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->x_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->x_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->x_constraint.outlying_values_array[i] );
# }
# }
# if( constraint->x_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->x_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->x_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->x_constraint.excluded_values_array[i] );
# }
# }
##
# // - generate y constraint message
# woss << L"\r\n\tY Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << static_cast<piint>( constraint->y_constraint.minimum )
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << static_cast<piint>( constraint->y_constraint.maximum )
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << static_cast<piint>( constraint->y_constraint.increment );
# if( constraint->y_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->y_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->y_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->y_constraint.outlying_values_array[i] );
# }
# }
# if( constraint->y_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->y_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->y_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->y_constraint.excluded_values_array[i] );
# }
# }
##
# // - generate width constraint message
# woss << L"\r\n\tWidth Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << static_cast<piint>( constraint->width_constraint.minimum )
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << static_cast<piint>( constraint->width_constraint.maximum )
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << static_cast<piint>( constraint->width_constraint.increment );
# if( constraint->width_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->width_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->width_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->width_constraint.outlying_values_array[i] );
# }
# }
# if( constraint->width_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->width_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->width_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->width_constraint.excluded_values_array[i] );
# }
# }
##
# // - generate height constraint message
# woss << L"\r\n\tHeight Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << static_cast<piint>( constraint->height_constraint.minimum )
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << static_cast<piint>( constraint->height_constraint.maximum )
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << static_cast<piint>( constraint->height_constraint.increment );
# if( constraint->height_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->height_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->height_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->height_constraint.outlying_values_array[i] );
# }
# }
# if( constraint->height_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->height_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->height_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << static_cast<piint>(
# constraint->height_constraint.excluded_values_array[i] );
# }
# }
##
# // - generate x-binning constraint message
# if( constraint->x_binning_limits_count )
# {
# woss << L"\r\n\tX-Binning Limitted to "
# << constraint->x_binning_limits_count << L" Value(s):";
# for( piint i = 0; i < constraint->x_binning_limits_count; ++i )
# woss << L"\r\n\t\t" << constraint->x_binning_limits_array[i];
# }
##
# // - generate y-binning constraint message
# if( constraint->y_binning_limits_count )
# {
# woss << L"\r\n\tY-Binning Limitted to "
# << constraint->y_binning_limits_count << L" Value(s):";
# for( piint i = 0; i < constraint->y_binning_limits_count; ++i )
# woss << L"\r\n\t\t" << constraint->y_binning_limits_array[i];
# }
# }
##
##    LogEvent( woss.str() );
##
# return PicamError_None;
# }
##
# ////////////////////////////////////////////////////////////////////////////////
# // PulseConstraintChanged
# // - called when a pulse constraint of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
# PicamError PIL_CALL PulseConstraintChanged(
# PicamHandle camera,
# PicamParameter parameter,
# const PicamPulseConstraint* constraint )
# {
##    UNREFERENCED_PARAMETER( camera );
##
# // - do nothing if parameters dialog is not open
# if( !parameters_ )
# return PicamError_None;
##
# // - generate log message
# std::wostringstream woss;
##    woss << GetEnumString( PicamEnumeratedType_Parameter, parameter )
# << L" pulse constraint changed to:\r\n";
# if( constraint->empty_set )
# woss << L"\t<empty set>";
# else
# {
# // - generate minimum duration message
# woss << L"\tMinimum Duration: "
# << constraint->minimum_duration << L"\r\n";
##
# // - generate maximum duration message
# woss << L"\tMaximum Duration: "
# << constraint->maximum_duration << L"\r\n";
##
# // - generate delay constraint message
# woss << L"\tDelay Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->delay_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->delay_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->delay_constraint.increment;
# if( constraint->delay_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->delay_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->delay_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->delay_constraint.outlying_values_array[i];
# }
# }
# if( constraint->delay_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->delay_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->delay_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->delay_constraint.excluded_values_array[i];
# }
# }
##
# // - generate width constraint message
# woss << L"\r\n\tWidth Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->width_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->width_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->width_constraint.increment;
# if( constraint->width_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->width_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->width_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->width_constraint.outlying_values_array[i];
# }
# }
# if( constraint->width_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->width_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->width_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->width_constraint.excluded_values_array[i];
# }
# }
# }
##
##    LogEvent( woss.str() );
##
# return PicamError_None;
# }
##
# ////////////////////////////////////////////////////////////////////////////////
# // ModulationsConstraintChanged
# // - called when a modulations constraint of a parameter changes due to another
# //   parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
# PicamError PIL_CALL ModulationsConstraintChanged(
# PicamHandle camera,
# PicamParameter parameter,
# const PicamModulationsConstraint* constraint )
# {
##    UNREFERENCED_PARAMETER( camera );
##
# // - do nothing if parameters dialog is not open
# if( !parameters_ )
# return PicamError_None;
##
# // - generate log message
# std::wostringstream woss;
##    woss << GetEnumString( PicamEnumeratedType_Parameter, parameter )
# << L" modulations constraint changed to:\r\n";
# if( constraint->empty_set )
# woss << L"\t<empty set>";
# else
# {
# // - generate maximum count message
# woss << L"\tMaximum Count: "
# << constraint->maximum_modulation_count << L"\r\n";
##
# // - generate duration constraint message
# woss << L"\tDuration Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->duration_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->duration_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->duration_constraint.increment;
# if( constraint->duration_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->duration_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->duration_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# duration_constraint.outlying_values_array[i];
# }
# }
# if( constraint->duration_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->duration_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->duration_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# duration_constraint.excluded_values_array[i];
# }
# }
##
# // - generate frequency constraint message
# woss << L"\r\n\tFrequency Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->frequency_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->frequency_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->frequency_constraint.increment;
# if( constraint->frequency_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->frequency_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->frequency_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# frequency_constraint.outlying_values_array[i];
# }
# }
# if( constraint->frequency_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->frequency_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->frequency_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# frequency_constraint.excluded_values_array[i];
# }
# }
##
# // - generate phase constraint message
# woss << L"\r\n\tPhase Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->phase_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->phase_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->phase_constraint.increment;
# if( constraint->phase_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->phase_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->phase_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->phase_constraint.outlying_values_array[i];
# }
# }
# if( constraint->phase_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->phase_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->phase_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->phase_constraint.excluded_values_array[i];
# }
# }
##
# // - generate output signal frequency constraint message
# woss << L"\r\n\tOutput Signal Frequency Constraint:\r\n";
# woss << L"\t\tMinimum: "
# << constraint->output_signal_frequency_constraint.minimum
# << L"\r\n";
# woss << L"\t\tMaximum: "
# << constraint->output_signal_frequency_constraint.maximum
# << L"\r\n";
# woss << L"\t\tIncrement: "
# << constraint->output_signal_frequency_constraint.increment;
# if( constraint->
# output_signal_frequency_constraint.outlying_values_count )
# {
# woss << L"\r\n\t\tIncluding "
# << constraint->
# output_signal_frequency_constraint.outlying_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->
# output_signal_frequency_constraint.outlying_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# output_signal_frequency_constraint.
# outlying_values_array[i];
# }
# }
# if( constraint->output_signal_frequency_constraint.excluded_values_count )
# {
# woss << L"\r\n\t\tExcluding "
# << constraint->
# output_signal_frequency_constraint.excluded_values_count
# << L" Value(s):";
# for( piint i = 0;
# i < constraint->
# output_signal_frequency_constraint.excluded_values_count;
# ++i )
# {
# woss << L"\r\n\t\t\t"
# << constraint->
# output_signal_frequency_constraint.
# excluded_values_array[i];
# }
# }
# }
##
##    LogEvent( woss.str() );
##
# return PicamError_None;
# }


# ////////////////////////////////////////////////////////////////////////////////
# // ReadoutStrideChanged
# // - called when the readout stride changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def readoutStrideChanged(self, camera,  parameter,  value):

        onlineReadoutRate = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            camera, self.picamParameter.OnlineReadoutRateCalculation, byref(onlineReadoutRate))
        if error != PicamError.NoError:
            print ("Failed to get online readout rate.",
                   self.getErrorSting(error))
            return PicamError.NoError

        self.calculateBufferSize(value, onlineReadoutRate.value)

        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // ParameterIntegerValueChanged
# // - called when an integer parameter changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterIntegerValueChanged(self, camera, parameter, value):
        #/ - get the value type
        valueType = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterValueType(
            camera, parameter, byref(valueType))
        if error != PicamError.NoError:
            print ("Failed to get parameter value type. ",
                   self.getErrorSting(error))
            return PicamError.NoError

        valueType = valueType.value

        #/ - generate log message
        message = self.getEnumString(
            PicamEnumeratedType.Parameter, parameter) + " value changed to "
        if valueType == PicamValueType.Integer:
            message += str(value)
        elif valueType == PicamValueType.Boolean:
            if value:
                message += "true"
            else:
                message += "false"
        elif valueType == PicamValueType.Enumeration:
            enumType = c_int(0)
            error = PI_CAM.picamlib.Picam_GetParameterEnumeratedType(
                camera, parameter, byref(enumType))
            if error != PicamError.NoError:
                print ("Failed to get enumerated type.",
                       self.getErrorSting(error))
                return PicamError.NoError
            message += self.getEnumString(enumType, value)
        else:
            message += 'unknown value type ' + \
                str(valueType) + self.getErrorSting(error)

        print(message)

        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // ParameterFloatingPointValueChanged
# // - called when a floating point parameter changes due to another parameter
# //   change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def ParameterFloatingPointValueChanged(self, camera, parameter, value):

        #/ - generate log message
        message = self.getEnumString(
            PicamEnumeratedType.Parameter, parameter) + " value changed to " + str(value)
        print (message)

        return PicamError.NoError


# ////////////////////////////////////////////////////////////////////////////////
# // ApplyExposureTime
# // - sets the exposure time on the selected camera
# ////////////////////////////////////////////////////////////////////////////////
    def ApplyExposureTime(self, exposure):

        #/ - determine if acquiring
        running = c_int(0)
        error = PI_CAM.picamlib.Picam_IsAcquisitionRunning(
            self.camera, byref(running))
        if error != PicamError.NoError:
            print ("Failed to determine if acquiring.",
                   self.getErrorSting(error))
            return

        #/ - set the exposure time appropriately
        model = c_void_p(0)
        error = PI_CAM.picamlib.PicamAdvanced_GetCameraModel(
            self.camera, byref(model))
        if error != PicamError.NoError:
            print ("Failed to get camera model.", self.getErrorSting(error))
            return
        if running.value:
            error = PI_CAM.picamlib.Picam_SetParameterFloatingPointValueOnline(
                model, self.picamParameter.ExposureTime, c_double(exposure))
            if error != PicamError.NoError:
                print ("Failed to set exposure time online.",
                       self.getErrorSting(error))
                return
        else:
            error = PI_CAM.picamlib.Picam_SetParameterFloatingPointValue(
                model, self.picamParameter.ExposureTime, c_double(exposure))
            if error != PicamError.NoError:
                print ("Failed to set exposure time.",
                       self.getErrorSting(error))
                return

# ////////////////////////////////////////////////////////////////////////////////
# // InitializeCalculatedBufferSize
# // - calculates the first buffer size for a camera just opened
# ////////////////////////////////////////////////////////////////////////////////
    def initializeCalculatedBufferSize(self):
        # - get the current readout rate
        # - note this accounts for rate increases in online scenarios
        onlineReadoutRate = c_double(0)
        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            self.camera, self.picamParameter.OnlineReadoutRateCalculation, byref(onlineReadoutRate))
        if error != PicamError.NoError:
            print ("Failed to get online readout rate.",
                   self.getErrorSting(error))

        #- get the current readout stride ( era device_ sostituito con camera )
        readoutStride = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ReadoutStride, byref(readoutStride))
        if error != PicamError.NoError:
            print ("Failed to get readout stride.", self.getErrorSting(error))

        #- calculate the buffer size
        self.calculateBufferSize(readoutStride.value, onlineReadoutRate.value)


# ////////////////////////////////////////////////////////////////////////////////
# // RegisterCameraCallbacks
# // - initializes callbacks from the open camera
# ////////////////////////////////////////////////////////////////////////////////
    def registerCameraCallbacks(self):

        print (" REGISTERED CALLBACK ")

        OnlineReadoutRateCalculationChangedCallback = PicamAdvanced.OnlineReadoutRateCalculationChangedCallback(
            self.onlineReadoutRateCalculationChanged)
        PI_CAM.callBacks['OnlineReadoutRateCalculationChanged'] = OnlineReadoutRateCalculationChangedCallback

        #/ - register online readout rate changed
        error = PI_CAM.picamlib.PicamAdvanced_RegisterForFloatingPointValueChanged(
            self.camera, self.picamParameter.OnlineReadoutRateCalculation, PI_CAM.callBacks['OnlineReadoutRateCalculationChanged'])
        if error != PicamError.NoError:
            print ("Failed to register for online readout rate changed.",
                   self.getErrorSting(error))

        ReadoutStrideChangedCallback = PicamAdvanced.ReadoutStrideChangedCallback(
            self.readoutStrideChanged)
        PI_CAM.callBacks['ReadoutStrideChanged'] = ReadoutStrideChangedCallback

        #/ - register readout stride changed
        error = PI_CAM.picamlib.PicamAdvanced_RegisterForIntegerValueChanged(
            self.camera, self.picamParameter.ReadoutStride, PI_CAM.callBacks['ReadoutStrideChanged'])
        if error != PicamError.NoError:
            print ("Failed to register for readout stride changed.",
                   self.getErrorSting(error))

        #/ - register parameter changed
        self.registerParameterCallbacks()

        #/ - register acquisition updated

        AcquisitionUpdatedCallback = PicamAdvanced.AcquisitionUpdatedCallback(
            self.acquisitionUpdated)
        PI_CAM.callBacks["AcquisitionUpdated"] = AcquisitionUpdatedCallback

        error = PI_CAM.picamlib.PicamAdvanced_RegisterForAcquisitionUpdated(
            self.camera, PI_CAM.callBacks['AcquisitionUpdated'])
        if error != PicamError.NoError:
            print ("Failed to register for acquisition updated.",
                   self.getErrorSting(error))


# ////////////////////////////////////////////////////////////////////////////////
# // ReadoutRateChanged
# // - called when the online readout rate changes due to another parameter change
# // - called on the same thread that changed the other parameter (in this case
# //   all SetParameter calls will be made on the main thread)
# ////////////////////////////////////////////////////////////////////////////////
    def onlineReadoutRateCalculationChanged(self, camera,  parameter, value):

        readoutStride = c_int(0)
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            camera, self.picamParameter.ReadoutStride, byref(readoutStride))
        if error != PicamError.NoError:
            print ("Failed to get readout stride.", self.getErrorSting(error))
            return PicamError.NoError

        self.calculateBufferSize(readoutStride.value, value)

        return PicamError.NoError

# ////////////////////////////////////////////////////////////////////////////////
# // CalculateBufferSize
# // - calculates a new circular buffer size given a readout stride and rate
# ////////////////////////////////////////////////////////////////////////////////
    def calculateBufferSize(self,  readoutStride,  onlineReadoutRate):
        global calculatedBufferSize

# // - calculate a circular buffer with the following simple rules:
# //   - contain at least 3 seconds worth of readout rate
# //   - contain at least 2 readouts
# // - note this takes into account changes that affect rate online (such as
# //   exposure) by assuming worst case (fastest rate)

        readouts = math.ceil(max(5. * onlineReadoutRate, 2.))
        calculatedBufferSize = readoutStride * readouts

# ////////////////////////////////////////////////////////////////////////////////
# // CacheFrameNavigation
# // - caches frame navigation information to extract frames from readouts
# // - takes the lock
# ////////////////////////////////////////////////////////////////////////////////
    def cacheFrameNavigation(self):

        global readoutStride
        global frameStride
        global framesPerReadout
        global frameSize

    # // - lock before accessing shared state
    # AutoLock al( lock_ );

        data = c_int(0)
        #/ - cache the readout stride device_
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ReadoutStride, byref(data))
        if error != PicamError.NoError:
            print ("Failed to get readout stride.", self.getErrorSting(error))
            return False
        readoutStride = data.value
        print ("readoutStride", readoutStride)

        data = c_int(0)
        #/ - cache the frame stride
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.FrameStride, byref(data))
        if error != PicamError.NoError:
            print ("Failed to get frame stride.", self.getErrorSting(error))
            return False
        frameStride = data.value
        print ("frameStride", frameStride)

        data = c_int(0)
        #/ - cache the frames per readout
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.FramesPerReadout, byref(data))
        if error != PicamError.NoError:
            print ("Failed to get frames per readout.",
                   self.getErrorSting(error))
            return False

        framesPerReadout = data.value
        print ("framesPerReadout", framesPerReadout)

        data = c_int(0)
        #/ - cache the frames size
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.FrameSize, byref(data))
        if error != PicamError.NoError:
            print ("Failed to get frames per readout.",
                   self.getErrorSting(error))
            return False

        frameSize = data.value
        print ("frameSize", frameSize)

        return True

# ////////////////////////////////////////////////////////////////////////////////
# // AcquisitionUpdated
# // - called when any of the following occur during acquisition:
# //   - a new readout arrives
# //   - acquisition completes due to total readouts acquired
# //   - acquisition completes due to a stop request
# //   - acquisition completes due to an acquisition error
# //   - an acquisition error occurs
# // - called on another thread
# // - all update callbacks are serialized
# // - takes the lock
# ////////////////////////////////////////////////////////////////////////////////
    def acquisitionUpdated(self, device, available, status):
        global readoutStride
        global frameStride
        global framesPerReadout
        global frameSize
        global dataQueue

        try:
            available = cast(available, POINTER(PicamAvailableData)).contents
            status = cast(status, POINTER(PicamAcquisitionStatus)).contents
        except:
            return PicamError.NoError

        if available.readout_count:

            #/ - copy the last available frame to the shared image buffer and notify
            # AutoLock al( lock_ );
            lastReadoutOffset = self.readoutStride * \
                (available.readout_count - 1)
            lastFrameOffset = self.frameStride * (self.framesPerReadout - 1)

            if dataQueue != None:
                pidata = cast((available.initial_readout),
                              POINTER(c_byte)).contents
                piDataAddr = addressof(pidata) + \
                    lastReadoutOffset + lastFrameOffset
                qDataVal = (c_byte * frameSize)()
                qDataAddr = addressof(qDataVal)
                memmove(qDataAddr, piDataAddr, frameSize)
                qData = PicamAvailableData()
                qData.initial_readout = cast((qDataVal), (c_void_p))
                qData.readout_count = available.readout_count
                dataQueue.put(qData)

##            pidata = cast( (available.initial_readout), POINTER(c_ushort) )
# for i in range(3):
##                print pidata[i]

    # const pibyte* frame =
    # static_cast<const pibyte*>( available->initial_readout ) +
    ##            lastReadoutOffset + lastFrameOffset;
    # std::memcpy( &imageData_[0], frame, frameSize_ );
    # ++imageDataVersion_;
    # WakeConditionVariable( &imageDataAvailable_ );
            # al.Release();

            #/ - check for overrun after copying
            overran = c_int(0)
            error = PI_CAM.picamlib.PicamAdvanced_HasAcquisitionBufferOverrun(
                device, byref(overran))
            if(error != PicamError.NoError):
                print ("Failed check for buffer overrun. ", self.getEnumString)(
                    PicamEnumeratedType.Error, error)
            elif(overran):
                print ("Buffer overran.")

        #/ - note when acquisition has completed
        if not status.running:
            print ('acquisitionInactive')
    ##        SetEvent( acquisitionInactive_ );
    ##        PostMessage( main_, WM_ACQUISITION_STOPPED, 0, 0 );

        return PicamError.NoError

    def readSensorCleanParams(self):
        pass


# ////////////////////////////////////////////////////////////////////////////////
# // SetReadoutCount
# // - sets the readout count appropriate for preview or acquire
# ////////////////////////////////////////////////////////////////////////////////
    def setReadoutCount(self, readouts):
        error = PI_CAM.picamlib.Picam_SetParameterLargeIntegerValue(
            self.camera, self.picamParameter.ReadoutCount, c_int(readouts))
        if(error != PicamError.NoError):
            print ("Cannot set readout count. ", self.getEnumString(
                PicamEnumeratedType.Error, error))
            return False
        return True

# ////////////////////////////////////////////////////////////////////////////////
# // CommitParameters
# // - commits the camera model and shows results in the parameters dialog
# ////////////////////////////////////////////////////////////////////////////////
    def CommitParameters(self):

        #/ - get the camera model
        model = c_void_p(0)
        error = PI_CAM.picamlib.PicamAdvanced_GetCameraModel(
            self.camera, byref(model))
        if(error != PicamError.NoError):
            print ("Failed to get camera model. ", self.getEnumString(
                PicamEnumeratedType.Error, error))
            return False

        #/ - apply changes to the device
        #/ - any changes to the model will be handled through change callbacks
        error = PI_CAM.picamlib.PicamAdvanced_CommitParametersToCameraDevice(
            model)
        if(error != PicamError.NoError):
            print ("Failed to commit to camera device. ",
                   self.getEnumString(PicamEnumeratedType.Error, error))
            return False

        return True


# ////////////////////////////////////////////////////////////////////////////////
# // Start
# // - starts acquisition with the selected camera
# ////////////////////////////////////////////////////////////////////////////////
    def startAcquisition(self):
        global calculatedBufferSize
        global dataBuffer
        global dataQueue

        #/ - determine if parameters need to be committed
        committed = c_int(0)
        # error = Picam_AreParametersCommitted( device_, &committed );
        error = PI_CAM.picamlib.Picam_AreParametersCommitted(
            self.camera, byref(committed))
        if(error != PicamError.NoError):
            print ("Cannot determine if parameters need to be committed. ",
                   self.getEnumString(PicamEnumeratedType.Error, error))
            return

        #/ - commit parameters from the model to the device _device
        if(not committed.value):
            model = c_void_p(0)
            error = PI_CAM.picamlib.PicamAdvanced_GetCameraModel(
                self.camera, byref(model))
            if(error != PicamError.NoError):
                print ("Cannot get the camera model. ",
                       self.getEnumString(PicamEnumeratedType.Error, error))
                return

            error = PI_CAM.picamlib.PicamAdvanced_CommitParametersToCameraDevice(
                model)
            if(error != PicamError.NoError):
                print ("Failed to commit the camera model parameters. ",
                       self.getEnumString(PicamEnumeratedType.Error, error))
                return

        #/ - reallocate circular buffer if necessary _device
        if(calculatedBufferSize == 0):
            print ("Cannot start with a circular buffer of no length.")
            return

        if dataBuffer == None:
            dataBuffer = (c_ushort * int(calculatedBufferSize))()

        if(len(dataBuffer) != int(calculatedBufferSize)):
            resize(dataBuffer, sizeof(dataBuffer._type_)
                   * int(calculatedBufferSize))
            dataBuffer = (dataBuffer._type_ * int(calculatedBufferSize)
                          ).from_address(addressof(dataBuffer))

        #/ - get current circular buffer
        picamBuffer = PicamAcquisitionBuffer()
        error = PI_CAM.picamlib.PicamAdvanced_GetAcquisitionBuffer(
            self.camera, byref(picamBuffer))
        if(error != PicamError.NoError):
            print ("Failed to get circular buffer. ",
                   self.getEnumString(PicamEnumeratedType.Error, error))
            return

        #/ - update circular buffer if neccessary
        if(addressof(dataBuffer) != picamBuffer.memory or len(dataBuffer) != picamBuffer.memory_size):
            picamBuffer.memory = addressof(dataBuffer)
            picamBuffer.memory_size = len(dataBuffer)
            error = PI_CAM.picamlib.PicamAdvanced_SetAcquisitionBuffer(
                self.camera, byref(picamBuffer))
            if(error != PicamError.NoError):
                print ("Failed to set circular buffer. ",
                       self.getEnumString(PicamEnumeratedType.Error, error))
                return


# / - cache information used to extract frames during acquisition
##        if not self.cacheFrameNavigation(self) :orienta
# return;
##  ##
    # // - initialize image data and display
    # if( !InitializeImage() )
    # return;
    ##
    # // - mark acquisition active just before acquisition begins
    ##    ResetEvent( acquisitionInactive_ );

        #/ - start device_
        error = PI_CAM.picamlib.Picam_StartAcquisition(self.camera)
        if(error != PicamError.NoError):
            print ("Failed to start acquisition. ",
                   self.getEnumString(PicamEnumeratedType.Error, error))
            return
    # // - indicate acquisition has begun
    ##    acquiring_ = true;
    ##    SetCursor( acquiringCursor_ );

# ////////////////////////////////////////////////////////////////////////////////
# // Stop
# // - requests for an asynchronous acquisition to stop
# ////////////////////////////////////////////////////////////////////////////////
    def stopAcquisition(self):
        error = PI_CAM.picamlib.Picam_StopAcquisition(self.camera)
        if(error != PicamError.NoError):
            print ("Failed to stop acquisition. ", self.getEnumString(
                PicamEnumeratedType.Error, error))
            return

# Worker Management
    def saveWorker(self):
        PI_CAM.workers[self.nid] = self.worker

    def restoreWorker(self):
        if self.nid in PI_CAM.workers.keys():
            self.worker = PI_CAM.workers[self.nid]
            return True
        else:
            return False

# AsynchStore class
    class AsynchStore(Thread):

        def configure(self, device, nodes, dataQueue):
            self.device = device
            self.camera = self.device.camera
            self.stopAcq = False
            self.readFrame = 0
            self.readoutStride = c_int(0)
            self.frameSize = c_int(0)
            self.tsRes = c_longlong(0)
            self.tsBitDepth = c_int(0)
            self.frameBitDepth = c_int(0)
            self.nodes = nodes
            self.count = 0
            self.dataQueue = dataQueue
            self.triggered = False

            self.trigMode = self.device.clock_mode.data()
            if self.trigMode == 'EXTERNAL':
                self.colockSource = self.device.clock_source.data()
                print ("colockSource ", self.colockSource)
                self.numFrames = len(self.colockSource)
            else:
                self.numFrames = self.device.num_frames.data()

            self.orientationMode = self.device.chipOrientationModeDict[self.device.chip_or.data(
            )]

            # Get the region of interest */
            regionPtr = c_void_p()
            error = PI_CAM.picamlib.Picam_GetParameterRoisValue(
                self.camera, self.device.picamParameter.Rois, byref(regionPtr))
            if error != PicamError.NoError:
                Data.execute('DevLogErr($1,$2)', self.device.getNid(
                ), 'PI_CAM error reading rois ' + self.device.getErrorSting())
                return PI_CAM.ERROR
            self.region = cast(regionPtr, POINTER(PicamRois)).contents

            return PI_CAM.SUCCESS

        def saveRoi(self, dataRoi, node, currTime, width,  height):
            # try:
                # Start time and end time for the current segment are the same (1 frame is contained) */
            startTime = Float32(currTime)
            endTime = Float32(currTime)
            # Segment dimension is a 1D array with one element (the segment has one row) */
            oneDim = [1]
            # Data dimension is a 3D array where the last dimension is 1 */
            dim = Float32([currTime])
            # unlike MDSplus C uses row-first ordering, so the last MDSplus dimension becomes the first one in C */
            segmentDims = [1, height, width]

            if self.orientationMode == PicamOrientationMask.FlippedHorizontally:
                dataRoi = np.flip(np.reshape(
                    dataRoi, [height, width]), 1).ravel()
            elif self.orientationMode == PicamOrientationMask.FlippedVertically:
                dataRoi = np.flip(np.reshape(
                    dataRoi, [height, width]), 0).ravel()

            segment = Uint16(dataRoi)
            segment.resize(segmentDims)
            # Write the entire segment */
            node.makeSegment(startTime, endTime, dim, segment)
            # Free stuff */
            del (segment)
            del (startTime)
            del (endTime)
            del (dim)
# except Exception as e :
##                raise mdsExceptions.TclFAILED_ESSENTIAL

        def saveFrame(self, data):

            pidata = cast((data.initial_readout), POINTER(c_ushort))
            numFrame = data.readout_count

            for loop in range(numFrame):
                stIdx = self.readoutStride.value * loop
                metadataOffset = stIdx + self.frameSize.value
                arIdx = stIdx/sizeof(c_ushort)

                self.trigMode = self.device.clock_mode.data()
                if self.trigMode == 'EXTERNAL':
                    currTime = self.colockSource[self.count]
                else:
                    currTime = self.count
                print ("Curr Time ", currTime)
                self.count = self.count + 1
                roiIndex = 0
                for i in range(self.region.roi_count):
                    print ("Frame %d Roi %d " % (loop, i))
                    w = self.region.roi_array[i].width / \
                        self.region.roi_array[i].x_binning
                    h = self.region.roi_array[i].height / \
                        self.region.roi_array[i].y_binning
                    roiPixel = w * h
                    ff = pidata[arIdx + roiIndex: arIdx + roiIndex + roiPixel]
                    self.saveRoi(ff, self.nodes[i], currTime, w, h)
                    roiIndex += roiPixel

        def run(self):

            data = PicamAvailableData()
            PI_CAM.picamlib.Picam_GetParameterIntegerValue(
                self.camera, self.device.picamParameter.ReadoutStride, byref(self.readoutStride))
            PI_CAM.picamlib.Picam_GetParameterIntegerValue(
                self.camera, self.device.picamParameter.FrameSize, byref(self.frameSize))
            PI_CAM.picamlib.Picam_GetParameterLargeIntegerValue(
                self.camera, self.device.picamParameter.TimeStampResolution, byref(self.tsRes))
            PI_CAM.picamlib.Picam_GetParameterIntegerValue(
                self.camera, self.device.picamParameter.TimeStampBitDepth, byref(self.tsBitDepth))
            PI_CAM.picamlib.Picam_GetParameterIntegerValue(
                self.camera, self.device.picamParameter.FrameTrackingBitDepth, byref(self.frameBitDepth))

            errors = c_char('0')

            self.triggered = False
            while not self.stopReq:
                try:
                    print("Waiting for frames to be collected",
                          self.count, self.numFrames)
                    qData = self.dataQueue.get(True, 2)
                    if qData.initial_readout != None:
                        self.triggered = True
                        self.saveFrame(qData)
                        del qData
                except queue.Empty as e:
                    pass
                except Exception as e:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PI_CAM save spectra exception ' + str(e))
                    self.stopReq = True
                if self.count+1 >= self.numFrames:
                    self.stopReq = True
                    print ("END Thread")

##            NUM_FRAMES = 5
##            NO_TIMEOUT = 10000
##
# while not self.stopReq :
# try :
##                    print( "Waiting for %d frames to be collected\n" %(NUM_FRAMES) )
##                    error = PI_CAM.picamlib.Picam_Acquire( self.camera, NUM_FRAMES, NO_TIMEOUT, byref(data), byref(errors) )
# if error == PicamError.TimeOutOccurred :
# continue
# if error != PicamError.NoError :
##                        Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PI_CAM acquisition error '+ self.device.getErrorSting(error))
##                        self.stopReq = True
# continue
##
# print( "Center Three Points:\tFrame # %d\n"%( data.readout_count ) );
# self.saveFrame(data)
##
# except Exception as e :
##                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PI_CAM save spectra exception ' + str(e) )
##                    self.stopReq = True

            print ('ASYNCH WORKER TERMINATED')
            # self.device.closeInfo()
            return

        def isTriggered(self):
            return self.triggered

        def stop(self):
            qData = PicamAvailableData()
            self.dataQueue.put(qData)
            self.stopReq = True


# End Inner class AsynchStore

    def setRois(self, rois):

          #- If we require symmetrical regions return since we are not setup */
          #- for that with our regions */

        constraintPtr = c_void_p()

        error = PI_CAM.picamlib.Picam_GetParameterRoisConstraint(
            self.camera, self.picamParameter.Rois, PicamConstraintCategory.Required, byref(constraintPtr))
        if error != PicamError.NoError:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'PI_CAM Region Of Interest setting error ' + self.getErrorSting(error))
            return error

        constraint = cast(constraintPtr, POINTER(PicamRoisConstraint)).contents

##            print "scope ", constraint.scope
##            print "severity ", constraint.severity
##            print "empty_set ", constraint.empty_set
##            print "rules ", constraint.rules
##            print "maximum_roi_count ", constraint.maximum_roi_count
##            print "width_constraint.maximum ", constraint.width_constraint.maximum
##            print "height_constraint.maximum ", constraint.height_constraint.maximum

        # Clean up constraints after using them */
        PI_CAM.picamlib.Picam_DestroyRoisConstraints(constraintPtr)

        picamRois = PicamRois()
        picamRois.roi_count = c_int(len(rois))
        picamRois.roi_array = cast((PicamRoi * len(rois))(), POINTER(PicamRoi))

        for i in range(0, len(rois)):
            picamRois.roi_array[i].x = c_int(rois[i][0])
            picamRois.roi_array[i].width = c_int(rois[i][1])
            picamRois.roi_array[i].x_binning = c_int(rois[i][2])
            picamRois.roi_array[i].y = c_int(rois[i][3])
            picamRois.roi_array[i].height = c_int(rois[i][4])
            picamRois.roi_array[i].y_binning = c_int(rois[i][5])


##            print "picamRois.roi_count = ", picamRois.roi_count
# for i in range(picamRois.roi_count):
##                print "x = ", picamRois.roi_array[i].x
##                print "width = ",picamRois.roi_array[i].width
##                print "x_binning = ",picamRois.roi_array[i].x_binning
##                print "y = ",picamRois.roi_array[i].y
##                print "height = ",picamRois.roi_array[i].heightorienta
##                print "y_binning = ",picamRois.roi_array[i].y_binning

        #  Set the region of interest */
        error = PI_CAM.picamlib.Picam_SetParameterRoisValue(
            self.camera, self.picamParameter.Rois, byref(picamRois))

        #  Error check */
# if error == PicamError.NoError:
##
##                paramsFailed = c_void_p(0)
##                failCount = c_int(0);
##
# Commit ROI to hardware */
##                error = PI_CAM.picamlib.Picam_CommitParameters(self.camera, byref(paramsFailed), byref(failCount));
# PI_CAM.picamlib.Picam_DestroyParameters(paramsFailed);
##
# if error == PicamError.NoError:
# orienta
# Get the region of interest */
##                    regionPtr = c_void_p()
##                    error = PI_CAM.picamlib.Picam_GetParameterRoisValue(self.camera, self.picamParameter.Rois, byref(regionPtr));
# if error != PicamError.NoError :
##                        print("Picam error Picam_GetParameterRoisValue %d\n" %(error))
##                    region = cast(regionPtr, POINTER(PicamRois)).contents
##
##                    print "region.roi_count = ", region.roi_count
# for i in range(region.roi_count):
##                        print "x = ", region.roi_array[i].x
##                        print "width = ",region.roi_array[i].width
##                        print "x_binning = ",region.roi_array[i].x_binning
##
##                        print "y = ",region.roi_array[i].y
##                        print "height = ",region.roi_array[i].height
##                        print "y_binning = ",region.roi_array[i].y_binning
# else:
# return error
# else:
##                Data.execute('DevLogErr($1,$2)', self.getNid(), 'PI_CAM Region Of Interest setting error '+ self.getErrorSting(error))
# return error

        return error

    # - Save sensor information parameters
    def saveSensorInformation(self):
        intVal = c_int(0)
        doubleVal = c_double(0)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.CcdCharacteristics, byref(intVal))
        if(error == PicamError.NoError):
            strVal = self.getEnumString(
                PicamEnumeratedType.CcdCharacteristicsMask, intVal)
            self.sensor_information_ccd_charact.putData(strVal)
            print ("Sensor CcdCharacteristics ", strVal)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorType, byref(intVal))
        if(error == PicamError.NoError):
            strVal = self.getEnumString(PicamEnumeratedType.SensorType, intVal)
            print ("Sensor SensorType            ", strVal)
            self.sensor_information_type.putData(strVal)

        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            self.camera, self.picamParameter.PixelGapHeight, byref(doubleVal))
        if(error == PicamError.NoError):
            self.sensor_information_pixel_gap_height.putData(doubleVal)
            print ("Sensor PixelGapHeight        ", doubleVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            self.camera, self.picamParameter.PixelGapWidth, byref(doubleVal))
        if(error == PicamError.NoError):
            self.sensor_information_pixel_gap_width.putData(doubleVal)
            print ("Sensor PixelGapWidth         ", doubleVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            self.camera, self.picamParameter.PixelHeight, byref(doubleVal))
        if(error == PicamError.NoError):
            self.sensor_information_pixel_height.putData(doubleVal)
            print ("Sensor PixelHeight           ", doubleVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterFloatingPointValue(
            self.camera, self.picamParameter.PixelWidth, byref(doubleVal))
        if(error == PicamError.NoError):
            self.sensor_information_pixel_width.putData(doubleVal)
            print ("Sensor PixelWidth            ", doubleVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveWidth, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_width.putData(intVal)
            print ("Sensor ActiveWidth           ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_height.putData(intVal)
            print ("Sensor ActiveHeight          ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveLeftMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_left_m.putData(intVal)
            print ("Sensor ActiveLeftMargin      ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveTopMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_top_m.putData(intVal)
            print ("Sensor ActiveTopMargin       ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveRightMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_right_m.putData(intVal)
            print ("Sensor ActiveRightMargin     ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorActiveBottomMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_bottom_m.putData(intVal)
            print ("Sensor ActiveBottomMargin    ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorSecondaryActiveHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_active_sec_height.putData(intVal)
            print ("Sensor SecondaryActiveHeight ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorMaskedHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_masked_height.putData(intVal)
            print ("Sensor MaskedHeight          ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorMaskedTopMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_masked_top_m.putData(intVal)
            print ("Sensor MaskedTopMargin       ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorMaskedBottomMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_masked_bottom_m.putData(intVal)
            print ("Sensor MaskedBottomMargin    ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SensorSecondaryMaskedHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_information_masked_sec_height.putData(intVal)
            print ("Sensor SecondaryMaskedHeight ", intVal.value)

    # - Save sensor layout parameters
    # - Values are read from device and saved in the pulse file
    # - This parameters can also be set in the camera
    def saveSensorLayout(self):
        intVal = c_int(0)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveWidth, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_width.putData(intVal)
            print ("Sensor Layout  ActiveWidth           ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_height.putData(intVal)
            print ("Sensor Layout  ActiveHeight          ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveLeftMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_left_m.putData(intVal)
            print ("Sensor Layout  ActiveLeftMargin      ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveTopMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_top_m.putData(intVal)
            print ("Sensor Layout  ActiveTopMargin       ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveRightMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_right_m.putData(intVal)
            print ("Sensor Layout  ActiveRightMargin     ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ActiveBottomMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_bottom_m.putData(intVal)
            print ("Sensor Layout  ActiveBottomMargin    ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SecondaryActiveHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_active_sec_height.putData(intVal)
            print ("Sensor Layout  SecondaryActiveHeight ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.MaskedHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_masked_height.putData(intVal)
            print ("Sensor Layout  MaskedHeight          ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.MaskedTopMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_masked_top_m.putData(intVal)
            print ("Sensor Layout  MaskedTopMargin       ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.MaskedBottomMargin, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_masked_bottom_m.putData(intVal)
            print ("Sensor Layout  MaskedBottomMargin    ", intVal.value)

        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.SecondaryMaskedHeight, byref(intVal))
        if(error == PicamError.NoError):
            self.sensor_layout_masked_sec_height.putData(intVal)
            print ("Sensor Layout  SecondaryMaskedHeight ", intVal.value)

    # - reads the temperature and temperature status directly from hardware
    def readSensorTemperature(self):

        # read temperature
        temperature = c_double(0)
        error = PI_CAM.picamlib.Picam_ReadParameterFloatingPointValue(
            self.camera, self.picamParameter.SensorTemperatureReading, byref(temperature))
        if(error == PicamError.NoError):
            print ("Sensor Temperature ", temperature.value, " degrees C")
        else:
            self.getErrorSting()

    # - read temperature status
        status = c_int(0)
        error = PI_CAM.picamlib.Picam_ReadParameterIntegerValue(
            self.camera, self.picamParameter.SensorTemperatureStatus, byref(status))
        if(error == PicamError.NoError):
            print ("Sensor status ", self.getEnumString)(
                PicamEnumeratedType.SensorTemperatureStatus, status)
        else:
            print ("Error : ", self.getErrorSting(error))
        return temperature.value

    # - set clean cycle count directly from hardware
    def setCleanCycleCount(self, cleanCycleCount):
        error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
            self.camera, self.picamParameter.CleanCycleCount, (cleanCycleCount))
        return error

    # - set shutter timing mode directly from hardware
    def setShutterMode(self, shutterMode):
        error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
            self.camera, self.picamParameter.ShutterTimingMode, (shutterMode))
        return error

    # - set adc quality  directly from hardware
    def setAdcQuality(self, adcQuality):
        error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
            self.camera, self.picamParameter.AdcQuality, (adcQuality))
        return error

    # - set readout orientation mode directly from hardware
    def setReadoutOrientationMode(self, orientationMode):
        orient = c_int(-1)
        print ("------ ORIENTATION ", orient)
        #error = PI_CAM.picamlib.Picam_SetParameterIntegerValue( self.camera, self.picamParameter.ReadoutOrientation, (orientationMode) )
        error = PI_CAM.picamlib.Picam_GetParameterIntegerValue(
            self.camera, self.picamParameter.ReadoutOrientation, byref(orient))
        print ("------ ORIENTATION ",
               self.getEnumString(PicamEnumeratedType.OrientationMask, orient))
        return error

    def setTriggerMode(self, trigMode):

        # PicamTriggerResponse_NoResponse
        # The camera acquires on its own and does not react to triggers.
        # PicamTriggerResponse_ExposeDuring
        # TriggerPulse
        # Each trigger begins an exposure that lasts for the
        # duration of the signal level (i.e., until the signal transitions back.)
        # PicamTriggerResponse_ReadoutPerTrigger
        # Each trigger leads to one readout.
        # NOTE: For cameras that can non-destructively readout, all non-destructive readouts
        # associated with the normal readout will occur on the same trigger as the normal readout.
        # PicamTriggerResponse_ShiftPerTrigger
        # Each trigger acquires another frame and reads out after the last frame is stored.
        # PicamTriggerResponse_StartOnSingleTrigger
        # The camera begins acquisition after a single trigger and continues without the need for further triggers.
        print ("SetTriggerMode ", trigMode)

        if trigMode == 'EXTERNAL':
            print (">>>>>>>>>>>>>>>> set EXTERNAL")
            error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
                self.camera, self.picamParameter.TriggerSource, PicamTriggerSource.External)
            if error == PicamError.NoError:
                error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
                    self.camera, self.picamParameter.TriggerDetermination, PicamTriggerDetermination.PositivePolarity)
            if error:
                error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
                    self.camera, self.picamParameter.TriggerDetermination, PicamTriggerDetermination.RisingEdge)
            if error:
                error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
                    self.camera, self.picamParameter.TriggerResponse, PicamTriggerResponse.ReadoutPerTrigger)
                #error = PI_CAM.picamlib.Picam_SetParameterIntegerValue( self.camera, self.picamParameter.TriggerResponse, PicamTriggerResponse.ShiftPerTrigger)
            # if error == PicamError.NoError:
            #    error = PI_CAM.picamlib.Picam_SetParameterIntegerValue( self.camera, self.picamParameter.TriggerDetermination, PicamTriggerDetermination.PositivePolarity)
        else:
            print (">>>>>>>>>>>>>>>> set INTERNAL")
            error = PI_CAM.picamlib.Picam_SetParameterIntegerValue(
                self.camera, self.picamParameter.TriggerResponse, PicamTriggerResponse.NoResponse)
        return error

    # PicamTriggerDetermination_PositivePolarity
    # The trigger is initially a signal's rising edge and then level-sensitive to a high signal for the rest of the acquisition.
    # PicamTriggerDetermination_NegativePolarity
    # The trigger is initially a signal's falling edge and then level-sensitive to a low signal for the rest of the acquisition.
    # PicamTriggerDetermination_RisingEdge
    # The trigger is a signal's rising edge.
    # PicamTriggerDetermination_FallingEdge
    # The trigger is a signal's falling edge.

        #error = picamlib.Picam_SetParameterIntegerValue( camera, picamParameter.TriggerDetermination, PicamTriggerDetermination.RisingEdge );

    # PicamTriggerCoupling_AC The components are AC-coupled.
    # PicamTriggerCoupling_DC The components are DC-coupled.

        #error = picamlib.Picam_SetParameterIntegerValue( camera, picamParameter.TriggerCoupling, PicamTriggerCoupling.DC );

    # PicamTriggerSource_External The trigger source is external to the camera.
    # PicamTriggerSource_Internal The trigger source is the camera.

        #error = picamlib.Picam_SetParameterIntegerValue( camera, picamParameter.TriggerSource, PicamTriggerSource.External );

    # PicamTriggerTermination_FiftyOhms The trigger terminates into 50 ohms.
    # PicamTriggerTermination_HighImpedance The trigger terminates into very high impedance.

        #error = picamlib.Picam_SetParameterIntegerValue( camera, picamParameter.TriggerTermination, PicamTriggerTermination.HighImpedance )

        return error


##########init############################################################################
    def init(self):

        print('================= PI_CAM Init ==================')
        if self.restoreInfo() == mdsExceptions.TclFAILED_ESSENTIAL:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:

            self.readSensorTemperature()

            self.saveSensorInformation()

            self.saveSensorLayout()

            rois = []
            for spec in range(0, 24):
                specNode = getattr(self, 'spectrum_%02d' % (spec+1))
                if specNode.isOn():
                    print ('Spectrum ', spec, ' ON')
                    getattr(self, 'spectrum_%02d_data' % (spec+1)).deleteData()
                    getattr(self, 'spectrum_%02d_data' %
                            (spec+1)).setCompressOnPut(False)
                    try:
                        x = getattr(self, 'spectrum_%02d_roi_x' %
                                    (spec+1)).data()
                        width = getattr(
                            self, 'spectrum_%02d_roi_width' % (spec+1)).data()
                        xbin = getattr(
                            self, 'spectrum_%02d_roi_x_bin' % (spec+1)).data()
                        y = getattr(self, 'spectrum_%02d_roi_y' %
                                    (spec+1)).data()
                        height = getattr(
                            self, 'spectrum_%02d_roi_height' % (spec+1)).data()
                        ybin = getattr(
                            self, 'spectrum_%02d_roi_y_bin' % (spec+1)).data()
                        rois.append([x, width, xbin, y, height, ybin])
                    except:
                        Data.execute('DevLogErr($1,$2)', self.getNid(
                        ), 'PI_CAM roi ' + str(spec+1) + ' values error, set roi off')
                        specNode.setOn(False)
                        continue

            print ("Rois ", rois)
            error = self.setRois(rois)
            if error != PicamError.NoError:
                del(rois)
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'PI_CAM Region Of Interest setting error ' + self.getErrorSting(error))
                raise mdsExceptions.TclFAILED_ESSENTIAL
            del(rois)

            try:
                expTime = self.exp_time.data()
                self.ApplyExposureTime(expTime)
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve expousure time value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                cleans = int(self.cleans.data())
                error = self.setCleanCycleCount(cleans)
                if error != PicamError.NoError:
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(
                    ), 'PI_CAM error writing  clean cycle count value ' + self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve clean cycle count value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                skip = int(self.skip.data())
                """
                error = self.setSkipCount(skip)
                if error != PicamError.NoError :
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PI_CAM error writing  skip count value '+ self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                """
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve skip count value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                adcQuality = self.adcQualityDict[self.adc.data()]
                """
                error = self.setAdcQuality(skip)
                if error != PicamError.NoError :
                    Data.execute('DevLogErr($1,$2)', self.device.getNid(), 'PI_CAM error writing  ADC quality value '+ self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
                """
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve ADC quality value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                shutterMode = self.shutterModeDict[self.shutter_mode.data()]
                error = self.setShutterMode(shutterMode)
                if error != PicamError.NoError:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'PI_CAM error setting  shutter mode value ' + self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve shutter mode value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                trigMode = self.clock_mode.data()
                error = self.setTriggerMode(trigMode)
                if error != PicamError.NoError:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'PI_CAM error setting clock mode value :: ' + self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve clock mode value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                orientationMode = self.chipOrientationModeDict[self.chip_or.data(
                )]
                error = self.setReadoutOrientationMode(orientationMode)
                if error != PicamError.NoError:
                    Data.execute('DevLogErr($1,$2)', self.getNid(
                    ), 'PI_CAM error readout orientation mode value ' + self.getErrorSting(error))
                    raise mdsExceptions.TclFAILED_ESSENTIAL
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(
                ), 'Cannot resolve readout orientation mode value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

       #/ - cache information used to extract frames during acquisition
            if not self.cacheFrameNavigation():
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Error reading camera acquisition information')
                raise mdsExceptions.TclFAILED_ESSENTIAL

            try:
                trigMode = self.clock_mode.data()
                if trigMode == 'EXTERNAL':
                    colockSource = self.clock_source.data()
                    nFrame = len(colockSource)
                else:
                    nFrame = self.num_frames.data()

                self.setReadoutCount(nFrame)  # 0 Continuous)
            except Exception as e:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'Cannot resolve number of frame value ' + str(e))
                raise mdsExceptions.TclFAILED_ESSENTIAL

            self.CommitParameters()
            self.saveInfo()
            print('===============================================')
        except Exception as e:
            print (e)
            self.saveInfo()
            self.closeInfo()
            raise mdsExceptions.TclFAILED_ESSENTIAL

        return 1


##########init############################################################################
    def start_store(self):
        global dataQueue

        print('\n================= PI_CAM Start Store ===============')
        if self.restoreInfo() == mdsExceptions.TclFAILED_ESSENTIAL:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        """
        if dataQueue != None :
            print 'Data queue counr ', dataQueue.qSize()
            with q.mutext:
                dataQueue.queue.clear()
        """

        dataQueue = queue.Queue()

        self.worker = self.AsynchStore()
        self.worker.daemon = True
        self.worker.stopReq = False

        nodes = []
        for spec in range(0, 24):
            if getattr(self, 'spectrum_%02d' % (spec+1)).isOn():
                nodes.append(getattr(self, 'spectrum_%02d_data' % (spec+1)))

        if self.worker.configure(self, nodes, dataQueue) == PI_CAM.SUCCESS:
            self.saveWorker()
            self.worker.start()
            print('===================================================\n')
            self.startAcquisition()
            self.saveInfo()
        else:
            raise mdsExceptions.TclFAILED_ESSENTIAL
        return 1

##########init############################################################################
    def stop_store(self):

        print('\n\n================= PI_CAM Stop Store ===============')
        if self.restoreInfo() == mdsExceptions.TclFAILED_ESSENTIAL:
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.stopAcquisition()

        print("PI_CAM stop_store")
        if self.restoreWorker():
            if self.worker.isTriggered() == False:
                Data.execute('DevLogErr($1,$2)', self.getNid(),
                             'CCD camera not triggered')
            if self.worker.isAlive():
                print("PI_CAM stop_worker")
                self.worker.stop()
                self.worker.join()
                print("PI_CAM worker stoped")

        shutterMode = self.shutterModeDict["CLOSE"]
        error = self.setShutterMode(shutterMode)
        if error != PicamError.NoError:
            Data.execute('DevLogErr($1,$2)', self.getNid(
            ), 'PI_CAM error setting  shutter mode value ' + self.getErrorSting(error))
            raise mdsExceptions.TclFAILED_ESSENTIAL

        self.closeInfo()
        print('===================================================\n')
        return 1
