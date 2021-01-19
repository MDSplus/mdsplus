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
from ctypes import c_int, c_char, c_void_p, c_byte, c_longlong, c_double, Structure, POINTER


class PicamEnumeratedType:
    #-------------------------------------------------------------------------*/
    #- Function Return Error Codes -------------------------------------------*/
    #-------------------------------------------------------------------------*/
    Error = c_int(1)
    #-------------------------------------------------------------------------*/
    #- General String Handling -----------------------------------------------*/
    #-------------------------------------------------------------------------*/
    EnumeratedType = c_int(29)
    #-------------------------------------------------------------------------*/
    #- Camera Identification -------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    Model = c_int(2)
    ComputerInterface = c_int(3)
    #-------------------------------------------------------------------------*/
    #- Camera Plug 'n Play Discovery -----------------------------------------*/
    #-------------------------------------------------------------------------*/
    DiscoveryAction = c_int(26)
    #-------------------------------------------------------------------------*/
    #- Camera Access ---------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    HandleType = c_int(27)
    #-------------------------------------------------------------------------*/
    #- Camera Parameters -----------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    ValueType = c_int(4)
    ConstraintType = c_int(5)
    Parameter = c_int(6)
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Values - Enumerated Types ----------------------------*/
    #-------------------------------------------------------------------------*/
    AdcAnalogGain = c_int(7)
    AdcQuality = c_int(8)
    CcdCharacteristicsMask = c_int(9)
    GateTrackingMask = c_int(36)
    GatingMode = c_int(34)
    GatingSpeed = c_int(38)
    EMIccdGainControlMode = c_int(42)
    IntensifierOptionsMask = c_int(35)
    IntensifierStatus = c_int(33)
    ModulationTrackingMask = c_int(41)
    OrientationMask = c_int(10)
    OutputSignal = c_int(11)
    PhosphorType = c_int(39)
    PhotocathodeSensitivity = c_int(40)
    PhotonDetectionMode = c_int(43)
    PixelFormat = c_int(12)
    ReadoutControlMode = c_int(13)
    SensorTemperatureStatus = c_int(14)
    SensorType = c_int(15)
    ShutterTimingMode = c_int(16)
    TimeStampsMask = c_int(17)
    TriggerCoupling = c_int(30)
    TriggerDetermination = c_int(18)
    TriggerResponse = c_int(19)
    TriggerSource = c_int(31)
    TriggerTermination = c_int(32)
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Information - Value Access ---------------------------*/
    #-------------------------------------------------------------------------*/
    ValueAccess = c_int(20)
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Information - Dynamics -------------------------------*/
    #-------------------------------------------------------------------------*/
    DynamicsMask = c_int(28)
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Constraints - Enumerated Types -----------------------*/
    #-------------------------------------------------------------------------*/
    ConstraintScope = c_int(21)
    ConstraintSeverity = c_int(22)
    ConstraintCategory = c_int(23)
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Constraints - Regions Of Interest --------------------*/
    #-------------------------------------------------------------------------*/
    RoisConstraintRulesMask = c_int(24)
    #-------------------------------------------------------------------------*/
    #- Acquisition Control ---------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    AcquisitionErrorsMask = c_int(25)
    #-------------------------------------------------------------------------*/
    #- Acquisition Notification ----------------------------------------------*/
    #-------------------------------------------------------------------------*/
    AcquisitionState = c_int(37)
    AcquisitionStateErrorsMask = c_int(44)
    #-------------------------------------------------------------------------*/
    #- PicamEnumeratedType; #- (45) */


#-----------------------------------------------------------------------------*/
#- Camera Identification -----------------------------------------------------*/
#-----------------------------------------------------------------------------*/
class PicamModel:
    #-------------------------------------------------------------------------*/
    #- PI-MTE Series (1419) --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PIMteSeries = c_int(1400)
    #- PI-MTE 1024 Series ----------------------------------------------------*/
    PIMte1024Series = c_int(1401)
    PIMte1024F = c_int(1402)
    PIMte1024B = c_int(1403)
    PIMte1024BR = c_int(1405)
    PIMte1024BUV = c_int(1404)
    #- PI-MTE 1024FT Series --------------------------------------------------*/
    PIMte1024FTSeries = c_int(1406)
    PIMte1024FT = c_int(1407)
    PIMte1024BFT = c_int(1408)
    #- PI-MTE 1300 Series ----------------------------------------------------*/
    PIMte1300Series = c_int(1412)
    PIMte1300B = c_int(1413)
    PIMte1300R = c_int(1414)
    PIMte1300BR = c_int(1415)
    #- PI-MTE 2048 Series ----------------------------------------------------*/
    PIMte2048Series = c_int(1416)
    PIMte2048B = c_int(1417)
    PIMte2048BR = c_int(1418)
    #- PI-MTE 2K Series ------------------------------------------------------*/
    PIMte2KSeries = c_int(1409)
    PIMte2KB = c_int(1410)
    PIMte2KBUV = c_int(1411)
    #-------------------------------------------------------------------------*/
    #- PIXIS Series (76) -----------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PixisSeries = c_int(0)
    #- PIXIS 100 Series ------------------------------------------------------*/
    Pixis100Series = c_int(1)
    Pixis100F = c_int(2)
    Pixis100B = c_int(6)
    Pixis100R = c_int(3)
    Pixis100C = c_int(4)
    Pixis100BR = c_int(5)
    Pixis100BExcelon = c_int(54)
    Pixis100BRExcelon = c_int(55)
    PixisXO100B = c_int(7)
    PixisXO100BR = c_int(8)
    PixisXB100B = c_int(68)
    PixisXB100BR = c_int(69)
    #- PIXIS 256 Series ------------------------------------------------------*/
    Pixis256Series = c_int(26)
    Pixis256F = c_int(27)
    Pixis256B = c_int(29)
    Pixis256E = c_int(28)
    Pixis256BR = c_int(30)
    PixisXB256BR = c_int(31)
    #- PIXIS 400 Series ------------------------------------------------------*/
    Pixis400Series = c_int(37)
    Pixis400F = c_int(38)
    Pixis400B = c_int(40)
    Pixis400R = c_int(39)
    Pixis400BR = c_int(41)
    Pixis400BExcelon = c_int(56)
    Pixis400BRExcelon = c_int(57)
    PixisXO400B = c_int(42)
    PixisXB400BR = c_int(70)
    #- PIXIS 512 Series ------------------------------------------------------*/
    Pixis512Series = c_int(43)
    Pixis512F = c_int(44)
    Pixis512B = c_int(45)
    Pixis512BUV = c_int(46)
    Pixis512BExcelon = c_int(58)
    PixisXO512F = c_int(49)
    PixisXO512B = c_int(50)
    PixisXF512F = c_int(48)
    PixisXF512B = c_int(47)
    #- PIXIS 1024 Series -----------------------------------------------------*/
    Pixis1024Series = c_int(9)
    Pixis1024F = c_int(10)
    Pixis1024B = c_int(11)
    Pixis1024BR = c_int(13)
    Pixis1024BUV = c_int(12)
    Pixis1024BExcelon = c_int(59)
    Pixis1024BRExcelon = c_int(60)
    PixisXO1024F = c_int(16)
    PixisXO1024B = c_int(14)
    PixisXO1024BR = c_int(15)
    PixisXF1024F = c_int(17)
    PixisXF1024B = c_int(18)
    PixisXB1024BR = c_int(71)
    #- PIXIS 1300 Series -----------------------------------------------------*/
    Pixis1300Series = c_int(51)
    Pixis1300F = c_int(52)
    Pixis1300F_2 = c_int(75)
    Pixis1300B = c_int(53)
    Pixis1300BR = c_int(73)
    Pixis1300BExcelon = c_int(61)
    Pixis1300BRExcelon = c_int(62)
    PixisXO1300B = c_int(65)
    PixisXF1300B = c_int(66)
    PixisXB1300R = c_int(72)
    #- PIXIS 2048 Series -----------------------------------------------------*/
    Pixis2048Series = c_int(20)
    Pixis2048F = c_int(21)
    Pixis2048B = c_int(22)
    Pixis2048BR = c_int(67)
    Pixis2048BExcelon = c_int(63)
    Pixis2048BRExcelon = c_int(74)
    PixisXO2048B = c_int(23)
    PixisXF2048F = c_int(25)
    PixisXF2048B = c_int(24)
    #- PIXIS 2K Series -------------------------------------------------------*/
    Pixis2KSeries = c_int(32)
    Pixis2KF = c_int(33)
    Pixis2KB = c_int(34)
    Pixis2KBUV = c_int(36)
    Pixis2KBExcelon = c_int(64)
    PixisXO2KB = c_int(35)
    #-------------------------------------------------------------------------*/
    #- Quad-RO Series (104) --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    QuadroSeries = c_int(100)
    Quadro4096 = c_int(101)
    Quadro4096_2 = c_int(103)
    Quadro4320 = c_int(102)
    #-------------------------------------------------------------------------*/
    #- ProEM Series (214) ----------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    ProEMSeries = c_int(200)
    #- ProEM 512 Series ------------------------------------------------------*/
    ProEM512Series = c_int(203)
    ProEM512B = c_int(201)
    ProEM512BK = c_int(205)
    ProEM512BExcelon = c_int(204)
    ProEM512BKExcelon = c_int(206)
    #- ProEM 1024 Series -----------------------------------------------------*/
    ProEM1024Series = c_int(207)
    ProEM1024B = c_int(202)
    ProEM1024BExcelon = c_int(208)
    #- ProEM 1600 Series -----------------------------------------------------*/
    ProEM1600Series = c_int(209)
    ProEM1600xx2B = c_int(212)
    ProEM1600xx2BExcelon = c_int(210)
    ProEM1600xx4B = c_int(213)
    ProEM1600xx4BExcelon = c_int(211)
    #-------------------------------------------------------------------------*/
    #- ProEM+ Series (614) ---------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    ProEMPlusSeries = c_int(600)
    #- ProEM+ 512 Series -----------------------------------------------------*/
    ProEMPlus512Series = c_int(603)
    ProEMPlus512B = c_int(601)
    ProEMPlus512BK = c_int(605)
    ProEMPlus512BExcelon = c_int(604)
    ProEMPlus512BKExcelon = c_int(606)
    #- ProEM+ 1024 Series ----------------------------------------------------*/
    ProEMPlus1024Series = c_int(607)
    ProEMPlus1024B = c_int(602)
    ProEMPlus1024BExcelon = c_int(608)
    #- ProEM+ 1600 Series ----------------------------------------------------*/
    ProEMPlus1600Series = c_int(609)
    ProEMPlus1600xx2B = c_int(612)
    ProEMPlus1600xx2BExcelon = c_int(610)
    ProEMPlus1600xx4B = c_int(613)
    ProEMPlus1600xx4BExcelon = c_int(611)
    #-------------------------------------------------------------------------*/
    #- ProEM-HS Series (1209) ------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    ProEMHSSeries = c_int(1200)
    #- ProEM-HS 512 Series ---------------------------------------------------*/
    ProEMHS512Series = c_int(1201)
    ProEMHS512B = c_int(1202)
    ProEMHS512BK = c_int(1207)
    ProEMHS512BExcelon = c_int(1203)
    ProEMHS512BKExcelon = c_int(1208)
    #- ProEM-HS 1024 Series --------------------------------------------------*/
    ProEMHS1024Series = c_int(1204)
    ProEMHS1024B = c_int(1205)
    ProEMHS1024BExcelon = c_int(1206)
    #-------------------------------------------------------------------------*/
    #- PI-MAX3 Series (303) --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PIMax3Series = c_int(300)
    PIMax31024I = c_int(301)
    PIMax31024x256 = c_int(302)
    #-------------------------------------------------------------------------*/
    #- PI-MAX4 Series (721) --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PIMax4Series = c_int(700)
    #- PI-MAX4 1024i Series --------------------------------------------------*/
    PIMax41024ISeries = c_int(703)
    PIMax41024I = c_int(701)
    PIMax41024IRF = c_int(704)
    #- PI-MAX4 1024f Series --------------------------------------------------*/
    PIMax41024FSeries = c_int(710)
    PIMax41024F = c_int(711)
    PIMax41024FRF = c_int(712)
    #- PI-MAX4 1024x256 Series -----------------------------------------------*/
    PIMax41024x256Series = c_int(705)
    PIMax41024x256 = c_int(702)
    PIMax41024x256RF = c_int(706)
    #- PI-MAX4 2048 Series ---------------------------------------------------*/
    PIMax42048Series = c_int(716)
    PIMax42048F = c_int(717)
    PIMax42048B = c_int(718)
    PIMax42048FRF = c_int(719)
    PIMax42048BRF = c_int(720)
    #- PI-MAX4 512EM Series --------------------------------------------------*/
    PIMax4512EMSeries = c_int(708)
    PIMax4512EM = c_int(707)
    PIMax4512BEM = c_int(709)
    #- PI-MAX4 1024EM Series -------------------------------------------------*/
    PIMax41024EMSeries = c_int(713)
    PIMax41024EM = c_int(715)
    PIMax41024BEM = c_int(714)
    #-------------------------------------------------------------------------*/
    #- PyLoN Series (439) ----------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PylonSeries = c_int(400)
    #- PyLoN 100 Series ------------------------------------------------------*/
    Pylon100Series = c_int(418)
    Pylon100F = c_int(404)
    Pylon100B = c_int(401)
    Pylon100BR = c_int(407)
    Pylon100BExcelon = c_int(425)
    Pylon100BRExcelon = c_int(426)
    #- PyLoN 256 Series ------------------------------------------------------*/
    Pylon256Series = c_int(419)
    Pylon256F = c_int(409)
    Pylon256B = c_int(410)
    Pylon256E = c_int(411)
    Pylon256BR = c_int(412)
    #- PyLoN 400 Series ------------------------------------------------------*/
    Pylon400Series = c_int(420)
    Pylon400F = c_int(405)
    Pylon400B = c_int(402)
    Pylon400BR = c_int(408)
    Pylon400BExcelon = c_int(427)
    Pylon400BRExcelon = c_int(428)
    #- PyLoN 1024 Series -----------------------------------------------------*/
    Pylon1024Series = c_int(421)
    Pylon1024B = c_int(417)
    Pylon1024BExcelon = c_int(429)
    #- PyLoN 1300 Series -----------------------------------------------------*/
    Pylon1300Series = c_int(422)
    Pylon1300F = c_int(406)
    Pylon1300B = c_int(403)
    Pylon1300R = c_int(438)
    Pylon1300BR = c_int(432)
    Pylon1300BExcelon = c_int(430)
    Pylon1300BRExcelon = c_int(433)
    #- PyLoN 2048 Series -----------------------------------------------------*/
    Pylon2048Series = c_int(423)
    Pylon2048F = c_int(415)
    Pylon2048B = c_int(434)
    Pylon2048BR = c_int(416)
    Pylon2048BExcelon = c_int(435)
    Pylon2048BRExcelon = c_int(436)
    #- PyLoN 2K Series -------------------------------------------------------*/
    Pylon2KSeries = c_int(424)
    Pylon2KF = c_int(413)
    Pylon2KB = c_int(414)
    Pylon2KBUV = c_int(437)
    Pylon2KBExcelon = c_int(431)
    #-------------------------------------------------------------------------*/
    #- PyLoN-IR Series (904) -------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PylonirSeries = c_int(900)
    #- PyLoN-IR 1024 Series --------------------------------------------------*/
    Pylonir1024Series = c_int(901)
    Pylonir102422 = c_int(902)
    Pylonir102417 = c_int(903)
    #-------------------------------------------------------------------------*/
    #- PIoNIR Series (502) ---------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    PionirSeries = c_int(500)
    Pionir640 = c_int(501)
    #-------------------------------------------------------------------------*/
    #- NIRvana Series (802) --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    NirvanaSeries = c_int(800)
    Nirvana640 = c_int(801)
    #-------------------------------------------------------------------------*/
    #- NIRvana ST Series (1302) ----------------------------------------------*/
    #-------------------------------------------------------------------------*/
    NirvanaSTSeries = c_int(1300)
    NirvanaST640 = c_int(1301)
    #-------------------------------------------------------------------------*/
    #- NIRvana-LN Series (1102) ----------------------------------------------*/
    #-------------------------------------------------------------------------*/
    NirvanaLNSeries = c_int(1100)
    NirvanaLN640 = c_int(1101)
    #-------------------------------------------------------------------------*/
#- PicamModel;


class PicamError:
    #-------------------------------------------------------------------------*/
    #- Success ---------------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    NoError = 0
    #-------------------------------------------------------------------------*/
    #- General Errors --------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    UnexpectedError = 4
    UnexpectedNullPointer = 3
    InvalidPointer = 35
    InvalidCount = 39
    InvalidOperation = 42
    OperationCanceled = 43
    #-------------------------------------------------------------------------*/
    #- Library Initialization Errors -----------------------------------------*/
    #-------------------------------------------------------------------------*/
    LibraryNotInitialized = 1
    LibraryAlreadyInitialized = 5
    #-------------------------------------------------------------------------*/
    #- General String Handling Errors ----------------------------------------*/
    #-------------------------------------------------------------------------*/
    InvalidEnumeratedType = 16
    EnumerationValueNotDefined = 17
    #-------------------------------------------------------------------------*/
    #- Plug 'n Play Discovery Errors -----------------------------------------*/
    #-------------------------------------------------------------------------*/
    NotDiscoveringCameras = 18
    AlreadyDiscoveringCameras = 19
    #-------------------------------------------------------------------------*/
    #- Camera Access Errors --------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    NoCamerasAvailable = 34
    CameraAlreadyOpened = 7
    InvalidCameraID = 8
    InvalidHandle = 9
    DeviceCommunicationFailed = 15
    DeviceDisconnected = 23
    DeviceOpenElsewhere = 24
    #-------------------------------------------------------------------------*/
    #- Demo Errors -----------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    InvalidDemoModel = 6
    InvalidDemoSerialNumber = 21
    DemoAlreadyConnected = 22
    DemoNotSupported = 40
    #-------------------------------------------------------------------------*/
    #- Camera Parameter Access Errors ----------------------------------------*/
    #-------------------------------------------------------------------------*/
    ParameterHasInvalidValueType = 11
    ParameterHasInvalidConstraintType = 13
    ParameterDoesNotExist = 12
    ParameterValueIsReadOnly = 10
    InvalidParameterValue = 2
    InvalidConstraintCategory = 38
    ParameterValueIsIrrelevant = 14
    ParameterIsNotOnlineable = 25
    ParameterIsNotReadable = 26
    #-------------------------------------------------------------------------*/
    #- Camera Data Acquisition Errors ----------------------------------------*/
    #-------------------------------------------------------------------------*/
    InvalidParameterValues = 28
    ParametersNotCommitted = 29
    InvalidAcquisitionBuffer = 30
    InvalidReadoutCount = 36
    InvalidReadoutTimeOut = 37
    InsufficientMemory = 31
    AcquisitionInProgress = 20
    AcquisitionNotInProgress = 27
    TimeOutOccurred = 32
    AcquisitionUpdatedHandlerRegistered = 33
    InvalidAcquisitionState = 44
    NondestructiveReadoutEnabled = 41
    #-------------------------------------------------------------------------*/
    #- End class PicamError;  (45) */


class PicamConstraintType:
    _None = 1
    Range = 2
    Collection = 3
    Rois = 4
    Pulse = 5
    Modulations = 6
    #- PicamConstraintType; #- (7) */


class PicamValueType:
    #-------------------------------------------------------------------------*/
    #- Integral Types --------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    Integer = 1
    Boolean = 3
    Enumeration = 4
    #-------------------------------------------------------------------------*/
    #- Large Integral Type ---------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    LargeInteger = 6
    #-------------------------------------------------------------------------*/
    #- Floating Point Type ---------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    FloatingPoint = 2
    #-------------------------------------------------------------------------*/
    #- Regions of Interest Type ----------------------------------------------*/
    #-------------------------------------------------------------------------*/
    Rois = 5
    #-------------------------------------------------------------------------*/
    #- Pulse Type ------------------------------------------------------------*/
    #-------------------------------------------------------------------------*/
    Pulse = 7
    #-------------------------------------------------------------------------*/
    #- Custom Intensifier Modulation Sequence Type ---------------------------*/
    #-------------------------------------------------------------------------*/
    Modulations = 8
    #-------------------------------------------------------------------------*/
    #- PicamValueType;  (9) */

    # define PI_V('v,c,n) (((PicamConstraintType_##c)<<24)+((PicamValueType_##v)<<16)+(n))
    #ExposureTime                      = v.FloatingPoint<<24 + c.Range<<16 + 23
    #ExposureTime                      = self.PIcamValueType.FloatingPoint<<24 + PicamConstraintType.Range<<16 + 23


class PicamParameter:
    #c = PicamConstraintType()
    #v = self.PIcamValueType()
    def PI_V(self, v, c, n):
        return c_int((getattr(PicamConstraintType, c) << 24) + (getattr(PicamValueType, v) << 16) + (n))

    def __init__(self):
        #--------------------------------------------------------------------------------------*/
        #- Shutter Timing ---------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.ExposureTime = self.PI_V('FloatingPoint', 'Range',        23)
        self.ShutterTimingMode = self.PI_V('Enumeration',   'Collection',   24)
        self.ShutterOpeningDelay = self.PI_V(
            'FloatingPoint', 'Range',        46)
        self.ShutterClosingDelay = self.PI_V(
            'FloatingPoint', 'Range',        25)
        self.ShutterDelayResolution = self.PI_V(
            'FloatingPoint', 'Collection',   47)
        #--------------------------------------------------------------------------------------*/
        #- Intensifier ------------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.EnableIntensifier = self.PI_V('Boolean',       'Collection',   86)
        self.IntensifierStatus = self.PI_V('Enumeration',   '_None',        87)
        self.IntensifierGain = self.PI_V('Integer',       'Range',        88)
        self.EMIccdGainControlMode = self.PI_V(
            'Enumeration',   'Collection',  123)
        self.EMIccdGain = self.PI_V('Integer',       'Range',       124)
        self.PhosphorDecayDelay = self.PI_V(
            'FloatingPoint', 'Range',        89)
        self.PhosphorDecayDelayResolution = self.PI_V(
            'FloatingPoint', 'Collection',   90)
        self.GatingMode = self.PI_V('Enumeration',   'Collection',   93)
        self.RepetitiveGate = self.PI_V('Pulse',         'Pulse',        94)
        self.SequentialStartingGate = self.PI_V(
            'Pulse',         'Pulse',        95)
        self.SequentialEndingGate = self.PI_V(
            'Pulse',         'Pulse',        96)
        self.SequentialGateStepCount = self.PI_V(
            'LargeInteger',  'Range',        97)
        self.SequentialGateStepIterations = self.PI_V(
            'LargeInteger',  'Range',        98)
        self.DifStartingGate = self.PI_V('Pulse',         'Pulse',       102)
        self.DifEndingGate = self.PI_V('Pulse',         'Pulse',       103)
        self.BracketGating = self.PI_V('Boolean',       'Collection',  100)
        self.IntensifierOptions = self.PI_V(
            'Enumeration',   '_None',        101)
        self.EnableModulation = self.PI_V('Boolean',       'Collection',  111)
        self.ModulationDuration = self.PI_V(
            'FloatingPoint', 'Range',       118)
        self.ModulationFrequency = self.PI_V(
            'FloatingPoint', 'Range',       112)
        self.RepetitiveModulationPhase = self.PI_V(
            'FloatingPoint', 'Range',       113)
        self.SequentialStartingModulationPhase = self.PI_V(
            'FloatingPoint', 'Range',       114)
        self.SequentialEndingModulationPhase = self.PI_V(
            'FloatingPoint', 'Range',       115)
        self.CustomModulationSequence = self.PI_V(
            'Modulations',   'Modulations', 119)
        self.PhotocathodeSensitivity = self.PI_V(
            'Enumeration',   '_None',        107)
        self.GatingSpeed = self.PI_V('Enumeration',   '_None',        108)
        self.PhosphorType = self.PI_V('Enumeration',   '_None',        109)
        self.IntensifierDiameter = self.PI_V(
            'FloatingPoint', '_None',        110)
        #--------------------------------------------------------------------------------------*/
        #- Analog to Digital Conversion -------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.AdcSpeed = self.PI_V('FloatingPoint', 'Collection',   33)
        self.AdcBitDepth = self.PI_V('Integer',       'Collection',   34)
        self.AdcAnalogGain = self.PI_V('Enumeration',   'Collection',   35)
        self.AdcQuality = self.PI_V('Enumeration',   'Collection',   36)
        self.AdcEMGain = self.PI_V('Integer',       'Range',        53)
        self.CorrectPixelBias = self.PI_V('Boolean',       'Collection',  106)
        #--------------------------------------------------------------------------------------*/
        #- Hardware I/O -----------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.TriggerSource = self.PI_V('Enumeration',   'Collection',   79)
        self.TriggerResponse = self.PI_V('Enumeration',   'Collection',   30)
        self.TriggerDetermination = self.PI_V(
            'Enumeration',   'Collection',   31)
        self.TriggerFrequency = self.PI_V('FloatingPoint', 'Range',        80)
        self.TriggerTermination = self.PI_V(
            'Enumeration',   'Collection',   81)
        self.TriggerCoupling = self.PI_V('Enumeration',   'Collection',   82)
        self.TriggerThreshold = self.PI_V('FloatingPoint', 'Range',        83)
        self.OutputSignal = self.PI_V('Enumeration',   'Collection',   32)
        self.InvertOutputSignal = self.PI_V(
            'Boolean',       'Collection',   52)
        self.AuxOutput = self.PI_V('Pulse',         'Pulse',        91)
        self.EnableSyncMaster = self.PI_V('Boolean',       'Collection',   84)
        self.SyncMaster2Delay = self.PI_V('FloatingPoint', 'Range',        85)
        self.EnableModulationOutputSignal = self.PI_V(
            'Boolean',       'Collection',  116)
        self.ModulationOutputSignalFrequency = self.PI_V(
            'FloatingPoint', 'Range',       117)
        self.ModulationOutputSignalAmplitude = self.PI_V(
            'FloatingPoint', 'Range',       120)
        self.AnticipateTrigger = self.PI_V('Boolean',       'Collection',  131)
        self.DelayFromPreTrigger = self.PI_V(
            'FloatingPoint', 'Range',       132)
        #--------------------------------------------------------------------------------------*/
        #- Readout Control --------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.ReadoutControlMode = self.PI_V(
            'Enumeration',   'Collection',   26)
        self.ReadoutTimeCalculation = self.PI_V(
            'FloatingPoint', '_None',         27)
        self.ReadoutPortCount = self.PI_V('Integer',       'Collection',   28)
        self.ReadoutOrientation = self.PI_V(
            'Enumeration',   '_None',         54)
        self.KineticsWindowHeight = self.PI_V(
            'Integer',       'Range',        56)
        self.VerticalShiftRate = self.PI_V('FloatingPoint', 'Collection',   13)
        self.Accumulations = self.PI_V('LargeInteger',  'Range',        92)
        self.EnableNondestructiveReadout = self.PI_V(
            'Boolean',       'Collection',  128)
        self.NondestructiveReadoutPeriod = self.PI_V(
            'FloatingPoint', 'Range',       129)
        #--------------------------------------------------------------------------------------*/
        #- Data Acquisition -------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.Rois = self.PI_V('Rois',          'Rois',         37)
        self.NormalizeOrientation = self.PI_V(
            'Boolean',       'Collection',   39)
        self.DisableDataFormatting = self.PI_V(
            'Boolean',       'Collection',   55)
        self.ReadoutCount = self.PI_V('LargeInteger',  'Range',        40)
        self.ExactReadoutCountMaximum = self.PI_V(
            'LargeInteger',  '_None',         77)
        self.PhotonDetectionMode = self.PI_V(
            'Enumeration',   'Collection',  125)
        self.PhotonDetectionThreshold = self.PI_V(
            'FloatingPoint', 'Range',       126)
        self.PixelFormat = self.PI_V('Enumeration',   'Collection',   41)
        self.FrameSize = self.PI_V('Integer',       '_None',         42)
        self.FrameStride = self.PI_V('Integer',       '_None',         43)
        self.FramesPerReadout = self.PI_V('Integer',       '_None',         44)
        self.ReadoutStride = self.PI_V('Integer',       '_None',         45)
        self.PixelBitDepth = self.PI_V('Integer',       '_None',         48)
        self.ReadoutRateCalculation = self.PI_V(
            'FloatingPoint', '_None',         50)
        self.OnlineReadoutRateCalculation = self.PI_V(
            'FloatingPoint', '_None',         99)
        self.FrameRateCalculation = self.PI_V(
            'FloatingPoint', '_None',         51)
        self.Orientation = self.PI_V('Enumeration',   '_None',         38)
        self.TimeStamps = self.PI_V('Enumeration',   'Collection',   68)
        self.TimeStampResolution = self.PI_V(
            'LargeInteger',  'Collection',   69)
        self.TimeStampBitDepth = self.PI_V('Integer',       'Collection',   70)
        self.TrackFrames = self.PI_V('Boolean',       'Collection',   71)
        self.FrameTrackingBitDepth = self.PI_V(
            'Integer',       'Collection',   72)
        self.GateTracking = self.PI_V('Enumeration',   'Collection',  104)
        self.GateTrackingBitDepth = self.PI_V(
            'Integer',       'Collection',  105)
        self.ModulationTracking = self.PI_V(
            'Enumeration',   'Collection',  121)
        self.ModulationTrackingBitDepth = self.PI_V(
            'Integer',       'Collection',  122)
        #--------------------------------------------------------------------------------------*/
        #- Sensor Information -----------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.SensorType = self.PI_V('Enumeration',   '_None',         57)
        self.CcdCharacteristics = self.PI_V(
            'Enumeration',   '_None',         58)
        self.SensorActiveWidth = self.PI_V(
            'Integer',       '_None',         59)
        self.SensorActiveHeight = self.PI_V(
            'Integer',       '_None',         60)
        self.SensorActiveLeftMargin = self.PI_V(
            'Integer',       '_None',         61)
        self.SensorActiveTopMargin = self.PI_V(
            'Integer',       '_None',         62)
        self.SensorActiveRightMargin = self.PI_V(
            'Integer',       '_None',         63)
        self.SensorActiveBottomMargin = self.PI_V(
            'Integer',       '_None',         64)
        self.SensorMaskedHeight = self.PI_V(
            'Integer',       '_None',         65)
        self.SensorMaskedTopMargin = self.PI_V(
            'Integer',       '_None',         66)
        self.SensorMaskedBottomMargin = self.PI_V(
            'Integer',       '_None',         67)
        self.SensorSecondaryMaskedHeight = self.PI_V(
            'Integer',       '_None',         49)
        self.SensorSecondaryActiveHeight = self.PI_V(
            'Integer',       '_None',         74)
        self.PixelWidth = self.PI_V('FloatingPoint', '_None',          9)
        self.PixelHeight = self.PI_V('FloatingPoint', '_None',         10)
        self.PixelGapWidth = self.PI_V('FloatingPoint', '_None',         11)
        self.PixelGapHeight = self.PI_V('FloatingPoint', '_None',         12)
        #--------------------------------------------------------------------------------------*/
        #- Sensor Layout ----------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.ActiveWidth = self.PI_V('Integer',       'Range',         1)
        self.ActiveHeight = self.PI_V('Integer',       'Range',         2)
        self.ActiveLeftMargin = self.PI_V('Integer',       'Range',         3)
        self.ActiveTopMargin = self.PI_V('Integer',       'Range',         4)
        self.ActiveRightMargin = self.PI_V('Integer',       'Range',         5)
        self.ActiveBottomMargin = self.PI_V(
            'Integer',       'Range',         6)
        self.MaskedHeight = self.PI_V('Integer',       'Range',         7)
        self.MaskedTopMargin = self.PI_V('Integer',       'Range',         8)
        self.MaskedBottomMargin = self.PI_V(
            'Integer',       'Range',        73)
        self.SecondaryMaskedHeight = self.PI_V(
            'Integer',       'Range',        75)
        self.SecondaryActiveHeight = self.PI_V(
            'Integer',       'Range',        76)
        #--------------------------------------------------------------------------------------*/
        #- Sensor Cleaning --------------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.CleanSectionFinalHeight = self.PI_V(
            'Integer',       'Range',        17)
        self.CleanSectionFinalHeightCount = self.PI_V(
            'Integer',       'Range',        18)
        self.CleanSerialRegister = self.PI_V(
            'Boolean',       'Collection',   19)
        self.CleanCycleCount = self.PI_V('Integer',       'Range',        20)
        self.CleanCycleHeight = self.PI_V('Integer',       'Range',        21)
        self.CleanBeforeExposure = self.PI_V(
            'Boolean',       'Collection',   78)
        self.CleanUntilTrigger = self.PI_V('Boolean',       'Collection',   22)
        self.StopCleaningOnPreTrigger = self.PI_V(
            'Boolean',       'Collection',  130)
        #--------------------------------------------------------------------------------------*/
        #- Sensor Temperature -----------------------------------------------------------------*/
        #--------------------------------------------------------------------------------------*/
        self.SensorTemperatureSetPoint = self.PI_V(
            'FloatingPoint', 'Range',        14)
        self.SensorTemperatureReading = self.PI_V(
            'FloatingPoint', '_None',         15)
        self.SensorTemperatureStatus = self.PI_V(
            'Enumeration',   '_None',         16)
        self.DisableCoolingFan = self.PI_V('Boolean',       'Collection',   29)
        self.EnableSensorWindowHeater = self.PI_V(
            'Boolean',       'Collection',  127)
        #--------------------------------------------------------------------------------------*/
        #- PicamParameter;  (133) */

#-----------------------------------------------------------------------------*/
#- Acquisition Control -------------------------------------------------------*/
#-----------------------------------------------------------------------------*/


class PicamAvailableData(Structure):
    _fields_ = [('initial_readout', c_void_p),
                ('readout_count', c_longlong)]


#-----------------------------------------------------------------------------*/
class PicamAcquisitionErrorsMask:
    _None = 0x0,
    DataLost = 0x1,
    ConnectionLost = 0x2
#-PicamAcquisitionErrorsMask; # (0x4) */
#-----------------------------------------------------------------------------*/


class PicamCameraID(Structure):
    _fields_ = [('model', c_int),
                ('computer_interface', c_int),
                ('sensor_name', (c_char * 64)),
                ('serial_number', (c_char * 64))]


#-----------------------------------------------------------------------------*/
#- Camera Parameter Values - Regions of Interest -----------------------------*/
#-----------------------------------------------------------------------------*/
class PicamRoi(Structure):
    _fields_ = [('x', c_int),
                ('width', c_int),
                ('x_binning', c_int),
                ('y', c_int),
                ('height', c_int),
                ('y_binning', c_int)]
#- PicamRoi;
#-----------------------------------------------------------------------------*/


class PicamRois(Structure):
    _fields_ = [('roi_array', POINTER(PicamRoi)),
                ('roi_count', c_int)]
#- PicamRois;
#-----------------------------------------------------------------------------*/

#-----------------------------------------------------------------------------*/
#- Camera Parameter Constraints - Enumerated Types ---------------------------*/
#-----------------------------------------------------------------------------*/


class PicamConstraintScope:
    Independent = c_int(1)
    Dependent = c_int(2)
#- PicamConstraintScope; # (3) */
#-----------------------------------------------------------------------------*/


class PicamConstraintSeverity:
    Error = c_int(1)
    Warning = c_int(2)
#- PicamConstraintSeverity; # (3) */
#-----------------------------------------------------------------------------*/


class PicamConstraintCategory:
    Capable = c_int(1)
    Required = c_int(2)
    Recommended = c_int(3)
#-} PicamConstraintCategory; # (4) */
#-----------------------------------------------------------------------------*/
#- Camera Parameter Constraints - Range --------------------------------------*/
#-----------------------------------------------------------------------------*/


class PicamRangeConstraint(Structure):
    _fields_ = [('scope', c_int),
                ('severity', c_int),
                ('empty_set', c_int),
                ('minimum', c_double),
                ('maximum', c_double),
                ('increment', c_double),
                ('excluded_values_array', POINTER(c_double)),
                ('excluded_values_count', c_int),
                ('outlying_values_array', POINTER(c_double)),
                ('outlying_values_count', c_int)]
#-PicamRangeConstraint;
#-----------------------------------------------------------------------------*/
#- Camera Parameter Constraints - Regions Of Interest ------------------------*/
#-----------------------------------------------------------------------------*/


class PicamRoisConstraintRulesMask:
    _None = c_byte(0x00)
    XBinningAlignment = c_byte(0x01)
    YBinningAlignment = c_byte(0x02)
    HorizontalSymmetry = c_byte(0x04)
    VerticalSymmetry = c_byte(0x08)
    SymmetryBoundsBinning = c_byte(0x10)
#- PicamRoisConstraintRulesMask; # (0x20) */
#-----------------------------------------------------------------------------*/


class PicamRoisConstraint(Structure):
    _fields_ = [('scope', c_int),
                ('severity', c_int),
                ('empty_set', c_int),
                ('rules', c_byte),
                ('maximum_roi_count', c_int),
                ('x_constraint', PicamRangeConstraint),
                ('width_constraint', PicamRangeConstraint),
                ('x_binning_limits_array', POINTER(c_int)),
                ('x_binning_limits_count', c_int),
                ('y_constraint', PicamRangeConstraint),
                ('height_constraint', PicamRangeConstraint),
                ('y_binning_limits_array', POINTER(c_int)),
                ('y_binning_limits_count', c_int)]
#-PicamRoisConstraint;

#----------------------------------------------------------------------------*/
# Camera Parameter Constraints - Collection ---------------------------------*/
#----------------------------------------------------------------------------*/


class PicamCollectionConstraint(Structure):
    _fields_ = [('scope', c_int),
                ('severity', c_int),
                ('values_array', POINTER(c_double)),
                ('values_count', c_int)]
#- PicamCollectionConstraint;

#-----------------------------------------------------------------------------*/


class PicamAcquisitionStatus(Structure):
    _fields_ = [('running', c_int),
                ('errors', c_int),
                ('readout_rate', c_double)]
#- PicamAcquisitionStatus;

#*----------------------------------------------------------------------------*/
#* Acquisition Setup - Buffer ------------------------------------------------*/
#*----------------------------------------------------------------------------*/


class PicamAcquisitionBuffer(Structure):
    _fields_ = [('memory', c_void_p),
                ('memory_size', c_longlong)]
#- PicamAcquisitionBuffer;

#*----------------------------------------------------------------------------*/
#* Camera Parameter Values - Custom Intensifier Modulation Sequence ----------*/
#*----------------------------------------------------------------------------*/


class PicamModulation(Structure):
    _fields_ = [('duration', c_double),
                ('frequency', c_double),
                ('phase', c_double),
                ('output_signal_frequency', c_double)]
#- PicamModulation;
#*----------------------------------------------------------------------------*/


class PicamModulations(Structure):
    _fields_ = [('modulation_array', POINTER(PicamModulation)),
                ('modulation_count', c_int)]
#- PicamModulations;

#*----------------------------------------------------------------------------*/
#* Camera Parameter Values - Pulse -------------------------------------------*/
#*----------------------------------------------------------------------------*/


class PicamPulse(Structure):
    _fields_ = [('delay', c_double),
                ('delay', c_double)]
#- PicamPulse;

#-----------------------------------------------------------------------------*/
#- Camera Parameter Values - Enumerated Types --------------------------------*/
#-----------------------------------------------------------------------------*/


class PicamAdcAnalogGain:
    Low = c_int(1)
    Medium = c_int(2)
    High = c_int(3)
#-PicamAdcAnalogGain; # (4) */
#-----------------------------------------------------------------------------*/


class PicamAdcQuality:
    LowNoise = c_int(1)
    HighCapacity = c_int(2)
    HighSpeed = c_int(4)
    ElectronMultiplied = c_int(3)
#-PicamAdcQuality; # (5) */
#-----------------------------------------------------------------------------*/


class PicamCcdCharacteristicsMask:
    _None = c_int(0x000)
    BackIlluminated = c_int(0x001)
    DeepDepleted = c_int(0x002)
    OpenElectrode = c_int(0x004)
    UVEnhanced = c_int(0x008)
    ExcelonEnabled = c_int(0x010)
    SecondaryMask = c_int(0x020)
    Multiport = c_int(0x040)
    AdvancedInvertedMode = c_int(0x080)
    HighResistivity = c_int(0x100)
#-PicamCcdCharacteristicsMask; # (0x200) */
#-----------------------------------------------------------------------------*/


class PicamEMIccdGainControlMode:
    Optimal = c_int(1)
    Manual = c_int(2)
#-PicamEMIccdGainControlMode; # (3) */
#-----------------------------------------------------------------------------*/


class PicamGateTrackingMask:
    _None = c_int(0x0)
    Delay = c_int(0x1)
    Width = c_int(0x2)
#-PicamGateTrackingMask; /* (0x4) */
#-----------------------------------------------------------------------------*/


class PicamGatingMode:
    Repetitive = c_int(1)
    Sequential = c_int(2)
    Custom = c_int(3)
#-PicamGatingMode; /* (4) */
#-----------------------------------------------------------------------------*/


class PicamGatingSpeed:
    Fast = c_int(1)
    Slow = c_int(2)
#-PicamGatingSpeed; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamIntensifierOptionsMask:
    _None = c_int(0x0)
    McpGating = c_int(0x1)
    SubNanosecondGating = c_int(0x2)
    Modulation = c_int(0x4)
#-PicamIntensifierOptionsMask; /* (0x8) */
#-----------------------------------------------------------------------------*/


class PicamIntensifierStatus:
    PicamIntensifierStatus_PoweredOff = c_int(1)
    PicamIntensifierStatus_PoweredOn = c_int(2)
#-PicamIntensifierStatus; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamModulationTrackingMask:
    _None = c_int(0x0)
    Duration = c_int(0x1)
    Frequency = c_int(0x2)
    Phase = c_int(0x4)
    OutputSignalFrequency = c_int(0x8)
#-PicamModulationTrackingMask; /* (0x10) */
#-----------------------------------------------------------------------------*/


class PicamOrientationMask:
    Normal = c_int(0x0)
    FlippedHorizontally = c_int(0x1)
    FlippedVertically = c_int(0x2)
#-PicamOrientationMask; /* (0x4) */
#-----------------------------------------------------------------------------*/


class PicamOutputSignal:
    NotReadingOut = c_int(1)
    ShutterOpen = c_int(2)
    Busy = c_int(3)
    AlwaysLow = c_int(4)
    AlwaysHigh = c_int(5)
    Acquiring = c_int(6)
    ShiftingUnderMask = c_int(7)
    Exposing = c_int(8)
    EffectivelyExposing = c_int(9)
    ReadingOut = c_int(10)
    WaitingForTrigger = c_int(11)
#-PicamOutputSignal; /* (12) */
#-----------------------------------------------------------------------------*/


class PicamPhosphorType:
    P43 = c_int(1)
    P46 = c_int(2)
#-PicamPhosphorType; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamPhotocathodeSensitivity:
    RedBlue = c_int(1)
    SuperRed = c_int(7)
    SuperBlue = c_int(2)
    UV = c_int(3)
    SolarBlind = c_int(10)
    Unigen2Filmless = c_int(4)
    InGaAsFilmless = c_int(9)
    HighQEFilmless = c_int(5)
    HighRedFilmless = c_int(8)
    HighBlueFilmless = c_int(6)
#-PicamPhotocathodeSensitivity; /* (11) */
#-----------------------------------------------------------------------------*/


class PicamPhotonDetectionMode:
    Disabled = c_int(1)
    Thresholding = c_int(2)
    Clipping = c_int(3)
#-PicamPhotonDetectionMode; /* (4) */
#-----------------------------------------------------------------------------*/


class PicamPixelFormat:
    Monochrome16Bit = c_int(1)
#-PicamPixelFormat; /* (2) */
#-----------------------------------------------------------------------------*/


class PicamReadoutControlMode:
    FullFrame = c_int(1)
    FrameTransfer = c_int(2)
    Interline = c_int(5)
    Kinetics = c_int(3)
    SpectraKinetics = c_int(4)
    Dif = c_int(6)
#-PicamReadoutControlMode; /* (7) */
#-----------------------------------------------------------------------------*/


class PicamSensorTemperatureStatus:
    Unlocked = c_int(1)
    Locked = c_int(2)
#-PicamSensorTemperatureStatus; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamSensorType:
    Ccd = c_int(1)
    InGaAs = c_int(2)
#-PicamSensorType; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamShutterTimingMode:
    Normal = c_int(1)
    AlwaysClosed = c_int(2)
    AlwaysOpen = c_int(3)
    OpenBeforeTrigger = c_int(4)
#-PicamShutterTimingMode; /* (5) */
#-----------------------------------------------------------------------------*/


class PicamTimeStampsMask:
    _None = c_int(0x0)
    ExposureStarted = c_int(0x1)
    ExposureEnded = c_int(0x2)
#-PicamTimeStampsMask; /* (0x4) */
#-----------------------------------------------------------------------------*/


class PicamTriggerCoupling:
    AC = c_int(1)
    DC = c_int(2)
#-PicamTriggerCoupling; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamTriggerDetermination:
    PositivePolarity = c_int(1)
    NegativePolarity = c_int(2)
    RisingEdge = c_int(3)
    FallingEdge = c_int(4)
#-PicamTriggerDetermination; /* (5) */
#-----------------------------------------------------------------------------*/


class PicamTriggerResponse:
    NoResponse = c_int(1)
    ReadoutPerTrigger = c_int(2)
    ShiftPerTrigger = c_int(3)
    ExposeDuringTriggerPulse = c_int(4)
    StartOnSingleTrigger = c_int(5)
#-PicamTriggerResponse; /* (6) */
#-----------------------------------------------------------------------------*/


class PicamTriggerSource:
    External = c_int(1)
    Internal = c_int(2)
#-PicamTriggerSource; /* (3) */
#-----------------------------------------------------------------------------*/


class PicamTriggerTermination:
    FiftyOhms = c_int(1)
    HighImpedance = c_int(2)
#-PicamTriggerTermination; /* (3) */
