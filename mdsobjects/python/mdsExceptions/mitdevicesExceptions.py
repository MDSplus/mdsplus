
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     mitdevices_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class DevException(MDSplusException):
  fac="Dev"


class DevBAD_ENDIDX(DevException):
  status=662470666
  message="unable to read end index for channel"
  msgnam="BAD_ENDIDX"



class DevBAD_FILTER(DevException):
  status=662470674
  message="illegal filter selected"
  msgnam="BAD_FILTER"



class DevBAD_FREQ(DevException):
  status=662470682
  message="illegal digitization frequency selected"
  msgnam="BAD_FREQ"



class DevBAD_GAIN(DevException):
  status=662470690
  message="illegal gain selected"
  msgnam="BAD_GAIN"



class DevBAD_HEADER(DevException):
  status=662470698
  message="unable to read header selection"
  msgnam="BAD_HEADER"



class DevBAD_HEADER_IDX(DevException):
  status=662470706
  message="unknown header configuration index"
  msgnam="BAD_HEADER_IDX"



class DevBAD_MEMORIES(DevException):
  status=662470714
  message="unable to read number of memory modules"
  msgnam="BAD_MEMORIES"



class DevBAD_MODE(DevException):
  status=662470722
  message="illegal mode selected"
  msgnam="BAD_MODE"



class DevBAD_NAME(DevException):
  status=662470730
  message="unable to read module name"
  msgnam="BAD_NAME"



class DevBAD_OFFSET(DevException):
  status=662470738
  message="illegal offset selected"
  msgnam="BAD_OFFSET"



class DevBAD_STARTIDX(DevException):
  status=662470746
  message="unable to read start index for channel"
  msgnam="BAD_STARTIDX"



class DevNOT_TRIGGERED(DevException):
  status=662470754
  message="device was not triggered,  check wires and triggering device"
  msgnam="NOT_TRIGGERED"



class DevFREQ_TO_HIGH(DevException):
  status=662470762
  message="the frequency is set to high for the requested number of channels"
  msgnam="FREQ_TO_HIGH"



class DevINVALID_NOC(DevException):
  status=662470770
  message="the NOC (number of channels) requested is greater than the physical number of channels"
  msgnam="INVALID_NOC"



class DevRANGE_MISMATCH(DevException):
  status=662470778
  message="the range specified on the menu doesn't match the range setting on the device"
  msgnam="RANGE_MISMATCH"



class DevCAMACERR(DevException):
  status=662470786
  message="Error doing CAMAC IO"
  msgnam="CAMACERR"



class DevBAD_VERBS(DevException):
  status=662470794
  message="Error reading interpreter list (:VERBS)"
  msgnam="BAD_VERBS"



class DevBAD_COMMANDS(DevException):
  status=662470802
  message="Error reading command list"
  msgnam="BAD_COMMANDS"



class DevCAM_ADNR(DevException):
  status=662470810
  message="CAMAC: Address not recognized (2160)"
  msgnam="CAM_ADNR"



class DevCAM_ERR(DevException):
  status=662470818
  message="CAMAC: Error reported by crate controler"
  msgnam="CAM_ERR"



class DevCAM_LOSYNC(DevException):
  status=662470826
  message="CAMAC: Lost Syncronization error"
  msgnam="CAM_LOSYNC"



class DevCAM_LPE(DevException):
  status=662470834
  message="CAMAC: Longitudinal Parity error"
  msgnam="CAM_LPE"



class DevCAM_TMO(DevException):
  status=662470842
  message="CAMAC: Highway time out error"
  msgnam="CAM_TMO"



class DevCAM_TPE(DevException):
  status=662470850
  message="CAMAC: Transverse Parity error"
  msgnam="CAM_TPE"



class DevCAM_STE(DevException):
  status=662470858
  message="CAMAC: Serial Transmission error"
  msgnam="CAM_STE"



class DevCAM_DERR(DevException):
  status=662470866
  message="CAMAC: Delayed error from SCC"
  msgnam="CAM_DERR"



class DevCAM_SQ(DevException):
  status=662470874
  message="CAMAC: I/O completion with Q = 1"
  msgnam="CAM_SQ"



class DevCAM_NOSQ(DevException):
  status=662470882
  message="CAMAC: I/O completion with Q = 0"
  msgnam="CAM_NOSQ"



class DevCAM_SX(DevException):
  status=662470890
  message="CAMAC: I/O completion with X = 1"
  msgnam="CAM_SX"



class DevCAM_NOSX(DevException):
  status=662470898
  message="CAMAC: I/O completion with X = 0"
  msgnam="CAM_NOSX"



class DevINV_SETUP(DevException):
  status=662470906
  message="device was not properly set up"
  msgnam="INV_SETUP"



class DevPYDEVICE_NOT_FOUND(DevException):
  status=662470914
  message="Python device class not found."
  msgnam="PYDEVICE_NOT_FOUND"



class DevPYDEVICE_NOT_FOUND(DevException):
  status=662470922
  message="Device support not found."
  msgnam="PYDEVICE_NOT_FOUND"



class DevIO_STUCK(DevException):
  status=662470930
  message="I/O to Device is stuck. Check network connection and board status."
  msgnam="IO_STUCK"



class DevUNKOWN_STATE(DevException):
  status=662470938
  message="Device returned unrecognized state string"
  msgnam="UNKOWN_STATE"



class DevWRONG_TREE(DevException):
  status=662470946
  message="Attempt to digitizerinto different tree than it was armed with"
  msgnam="WRONG_TREE"



class DevWRONG_PATH(DevException):
  status=662470954
  message="Attempt to store digitizer into different path than it was armed with"
  msgnam="WRONG_PATH"



class DevWRONG_SHOT(DevException):
  status=662470962
  message="Attempt to store digitizer into different shot than it was armed with"
  msgnam="WRONG_SHOT"



class DevOFFLINE(DevException):
  status=662470970
  message="Device is not on line.  Check network connection"
  msgnam="OFFLINE"



class DevTRIGGERED_NOT_STORED(DevException):
  status=662470978
  message="Device was triggered but not stored."
  msgnam="TRIGGERED_NOT_STORED"



class DevNO_NAME_SPECIFIED(DevException):
  status=662470986
  message="Device name must be specifed - pleas fill it in."
  msgnam="NO_NAME_SPECIFIED"



class DevBAD_ACTIVE_CHAN(DevException):
  status=662470994
  message="Active channels either not available or invalid"
  msgnam="BAD_ACTIVE_CHAN"



class DevBAD_TRIG_SRC(DevException):
  status=662471002
  message="Trigger source either not available or invalid"
  msgnam="BAD_TRIG_SRC"



class DevBAD_CLOCK_SRC(DevException):
  status=662471010
  message="Clock source either not available or invalid"
  msgnam="BAD_CLOCK_SRC"



class DevBAD_PRE_TRIG(DevException):
  status=662471018
  message="Pre trigger samples either not available or invalid"
  msgnam="BAD_PRE_TRIG"



class DevBAD_POST_TRIG(DevException):
  status=662471026
  message="Clock source either not available or invalid"
  msgnam="BAD_POST_TRIG"



class DevBAD_CLOCK_FREQ(DevException):
  status=662471034
  message="Clock frequency either not available or invalid"
  msgnam="BAD_CLOCK_FREQ"



class DevTRIGGER_FAILED(DevException):
  status=662471042
  message="Device trigger method failed"
  msgnam="TRIGGER_FAILED"



class DevERROR_READING_CHANNEL(DevException):
  status=662471050
  message="Error reading data for channel from device"
  msgnam="ERROR_READING_CHANNEL"



class DevERROR_DOING_INIT(DevException):
  status=662471058
  message="Error sending ARM command to device"
  msgnam="ERROR_DOING_INIT"



class ReticonException(MDSplusException):
  fac="Reticon"


class ReticonNORMAL(ReticonException):
  status=662471065
  message="successful completion"
  msgnam="NORMAL"



class ReticonBAD_FRAMES(ReticonException):
  status=662471074
  message="frame count must be less than or equal to 2048"
  msgnam="BAD_FRAMES"



class ReticonBAD_FRAME_SELECT(ReticonException):
  status=662471082
  message="frame interval must be 1,2,4,8,16,32 or 64"
  msgnam="BAD_FRAME_SELECT"



class ReticonBAD_NUM_STATES(ReticonException):
  status=662471090
  message="number of states must be between 1 and 4"
  msgnam="BAD_NUM_STATES"



class ReticonBAD_PERIOD(ReticonException):
  status=662471098
  message="period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec"
  msgnam="BAD_PERIOD"



class ReticonBAD_PIXEL_SELECT(ReticonException):
  status=662471106
  message="pixel selection must be an array of 256 boolean values"
  msgnam="BAD_PIXEL_SELECT"



class ReticonDATA_CORRUPTED(ReticonException):
  status=662471114
  message="data in memory is corrupted or framing error detected, no data stored"
  msgnam="DATA_CORRUPTED"



class ReticonTOO_MANY_FRAMES(ReticonException):
  status=662471122
  message="over 8192 frame start indicators in data read from memory"
  msgnam="TOO_MANY_FRAMES"



class J221Exception(MDSplusException):
  fac="J221"


class J221NORMAL(J221Exception):
  status=662471465
  message="successful completion"
  msgnam="NORMAL"



class J221INVALID_DATA(J221Exception):
  status=662471476
  message="ignoring invalid data in channel !SL"
  msgnam="INVALID_DATA"



class J221NO_DATA(J221Exception):
  status=662471482
  message="no valid data was found for any channel"
  msgnam="NO_DATA"



class TimingException(MDSplusException):
  fac="Timing"


class TimingINVCLKFRQ(TimingException):
  status=662471866
  message="Invalid clock frequency"
  msgnam="INVCLKFRQ"



class TimingINVDELDUR(TimingException):
  status=662471874
  message="Invalid pulse delay or duration, must be less than 655 seconds"
  msgnam="INVDELDUR"



class TimingINVOUTCTR(TimingException):
  status=662471882
  message="Invalid output mode selected"
  msgnam="INVOUTCTR"



class TimingINVPSEUDODEV(TimingException):
  status=662471890
  message="Invalid pseudo device attached to this decoder channel"
  msgnam="INVPSEUDODEV"



class TimingINVTRGMOD(TimingException):
  status=662471898
  message="Invalid trigger mode selected"
  msgnam="INVTRGMOD"



class TimingNOPSEUDODEV(TimingException):
  status=662471907
  message="No Pseudo device attached to this channel ... disabling"
  msgnam="NOPSEUDODEV"



class TimingTOO_MANY_EVENTS(TimingException):
  status=662471914
  message="More than 16 events used by this decoder"
  msgnam="TOO_MANY_EVENTS"



class B2408Exception(MDSplusException):
  fac="B2408"


class B2408NORMAL(B2408Exception):
  status=662472265
  message="successful completion"
  msgnam="NORMAL"



class B2408OVERFLOW(B2408Exception):
  status=662472275
  message="Triggers received after overflow"
  msgnam="OVERFLOW"



class B2408TRIG_LIM(B2408Exception):
  status=662472284
  message="Trigger limit possibly exceeded"
  msgnam="TRIG_LIM"



class FeraException(MDSplusException):
  fac="Fera"


class FeraNORMAL(FeraException):
  status=662472665
  message="successful completion"
  msgnam="NORMAL"



class FeraDIGNOTSTRARRAY(FeraException):
  status=662472674
  message="The digitizer names must be an array of strings"
  msgnam="DIGNOTSTRARRAY"



class FeraNODIG(FeraException):
  status=662472682
  message="The digitizer names must be specified"
  msgnam="NODIG"



class FeraMEMNOTSTRARRAY(FeraException):
  status=662472690
  message="The memory names must be an array of strings"
  msgnam="MEMNOTSTRARRAY"



class FeraNOMEM(FeraException):
  status=662472698
  message="The memory names must be specified"
  msgnam="NOMEM"



class FeraPHASE_LOST(FeraException):
  status=662472706
  message="Data phase lost No FERA data stored"
  msgnam="PHASE_LOST"



class FeraCONFUSED(FeraException):
  status=662472716
  message="Fera Data inconsitant.  Data for this point zered."
  msgnam="CONFUSED"



class FeraOVER_RUN(FeraException):
  status=662472724
  message="Possible FERA memory overrun, too many triggers."
  msgnam="OVER_RUN"



class FeraOVERFLOW(FeraException):
  status=662472731
  message="Possible FERA data saturated.  Data point zeroed"
  msgnam="OVERFLOW"



class Hm650Exception(MDSplusException):
  fac="Hm650"


class Hm650NORMAL(Hm650Exception):
  status=662473065
  message="successful completion"
  msgnam="NORMAL"



class Hm650DLYCHNG(Hm650Exception):
  status=662473076
  message="HM650 requested delay can not be processed by hardware."
  msgnam="DLYCHNG"



class Hv4032Exception(MDSplusException):
  fac="Hv4032"


class Hv4032NORMAL(Hv4032Exception):
  status=662473465
  message="successful completion"
  msgnam="NORMAL"



class Hv4032WRONG_POD_TYPE(Hv4032Exception):
  status=662473474
  message="HV40321A n and p can only be used with the HV4032 device"
  msgnam="WRONG_POD_TYPE"



class Hv1440Exception(MDSplusException):
  fac="Hv1440"


class Hv1440NORMAL(Hv1440Exception):
  status=662473865
  message="successful completion"
  msgnam="NORMAL"



class Hv1440WRONG_POD_TYPE(Hv1440Exception):
  status=662473874
  message="HV1443 can only be used with the HV1440 device"
  msgnam="WRONG_POD_TYPE"



class Hv1440BAD_FRAME(Hv1440Exception):
  status=662473882
  message="HV1440 could not read the frame"
  msgnam="BAD_FRAME"



class Hv1440BAD_RANGE(Hv1440Exception):
  status=662473890
  message="HV1440 could not read the range"
  msgnam="BAD_RANGE"



class Hv1440OUTRNG(Hv1440Exception):
  status=662473898
  message="HV1440 out of range"
  msgnam="OUTRNG"



class Hv1440STUCK(Hv1440Exception):
  status=662473906
  message="HV1440 not responding with Q"
  msgnam="STUCK"



class JoergerException(MDSplusException):
  fac="Joerger"


class JoergerBAD_PRE_TRIGGER(JoergerException):
  status=662474266
  message="bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7"
  msgnam="BAD_PRE_TRIGGER"



class JoergerBAD_ACT_MEMORY(JoergerException):
  status=662474274
  message="bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8"
  msgnam="BAD_ACT_MEMORY"



class JoergerBAD_GAIN(JoergerException):
  status=662474282
  message="bad gain specified, specify a value of 1,2,4 or 8"
  msgnam="BAD_GAIN"



class U_of_mException(MDSplusException):
  fac="U_of_m"


class U_of_mBAD_WAVE_LENGTH(U_of_mException):
  status=662474666
  message="bad wave length specified, specify value between 0 and 13000"
  msgnam="BAD_WAVE_LENGTH"



class U_of_mBAD_SLIT_WIDTH(U_of_mException):
  status=662474674
  message="bad slit width specified, specify value between 0 and 500"
  msgnam="BAD_SLIT_WIDTH"



class U_of_mBAD_NUM_SPECTRA(U_of_mException):
  status=662474682
  message="bad number of spectra specified, specify value between 1 and 100"
  msgnam="BAD_NUM_SPECTRA"



class U_of_mBAD_GRATING(U_of_mException):
  status=662474690
  message="bad grating type specified, specify value between 1 and 5"
  msgnam="BAD_GRATING"



class U_of_mBAD_EXPOSURE(U_of_mException):
  status=662474698
  message="bad exposure time specified, specify value between 30 and 3000"
  msgnam="BAD_EXPOSURE"



class U_of_mBAD_FILTER(U_of_mException):
  status=662474706
  message="bad neutral density filter specified, specify value between 0 and 5"
  msgnam="BAD_FILTER"



class U_of_mGO_FILE_ERROR(U_of_mException):
  status=662474714
  message="error creating new go file"
  msgnam="GO_FILE_ERROR"



class U_of_mDATA_FILE_ERROR(U_of_mException):
  status=662474722
  message="error opening datafile"
  msgnam="DATA_FILE_ERROR"



class IdlException(MDSplusException):
  fac="Idl"


class IdlNORMAL(IdlException):
  status=662475065
  message="successful completion"
  msgnam="NORMAL"



class IdlERROR(IdlException):
  status=662475074
  message="IDL returned a non zero error code"
  msgnam="ERROR"



class B5910aException(MDSplusException):
  fac="B5910a"


class B5910aBAD_CHAN(B5910aException):
  status=662475466
  message="error evaluating data for channel !SL"
  msgnam="BAD_CHAN"



class B5910aBAD_CLOCK(B5910aException):
  status=662475474
  message="invalid internal clock range specified"
  msgnam="BAD_CLOCK"



class B5910aBAD_ITERATIONS(B5910aException):
  status=662475482
  message="invalid number of iterations specified"
  msgnam="BAD_ITERATIONS"



class B5910aBAD_NOC(B5910aException):
  status=662475490
  message="invalid number of active channels specified"
  msgnam="BAD_NOC"



class B5910aBAD_SAMPS(B5910aException):
  status=662475498
  message="number of samples specificed invalid"
  msgnam="BAD_SAMPS"



class J412Exception(MDSplusException):
  fac="J412"


class J412NOT_SORTED(J412Exception):
  status=662475866
  message="times specified for J412 module were not sorted"
  msgnam="NOT_SORTED"



class J412NO_DATA(J412Exception):
  status=662475874
  message="there were no times specifed for J412 module"
  msgnam="NO_DATA"



class J412BADCYCLES(J412Exception):
  status=662475882
  message="The number of cycles must be 1 .. 255"
  msgnam="BADCYCLES"



class Tr16Exception(MDSplusException):
  fac="Tr16"


class Tr16NORMAL(Tr16Exception):
  status=662476265
  message="successful completion"
  msgnam="NORMAL"



class Tr16BAD_MEMSIZE(Tr16Exception):
  status=662476274
  message="Memory size must be in 128K, 256K, 512k, 1024K"
  msgnam="BAD_MEMSIZE"



class Tr16BAD_ACTIVEMEM(Tr16Exception):
  status=662476282
  message="Active Mem must be power of 2 8K to 1024K"
  msgnam="BAD_ACTIVEMEM"



class Tr16BAD_ACTIVECHAN(Tr16Exception):
  status=662476290
  message="Active channels must be in 1,2,4,8,16"
  msgnam="BAD_ACTIVECHAN"



class Tr16BAD_PTS(Tr16Exception):
  status=662476298
  message="PTS must be power of 2 32 to 1024K"
  msgnam="BAD_PTS"



class Tr16BAD_FREQUENCY(Tr16Exception):
  status=662476306
  message="Invalid clock frequency"
  msgnam="BAD_FREQUENCY"



class Tr16BAD_MASTER(Tr16Exception):
  status=662476314
  message="Master must be 0 or 1"
  msgnam="BAD_MASTER"



class Tr16BAD_GAIN(Tr16Exception):
  status=662476322
  message="Gain must be 1, 2, 4, or 8"
  msgnam="BAD_GAIN"



class A14Exception(MDSplusException):
  fac="A14"


class A14NORMAL(A14Exception):
  status=662476665
  message="successful completion"
  msgnam="NORMAL"



class A14BAD_CLK_DIVIDE(A14Exception):
  status=662476674
  message="Clock divide must be one of 1,2,4,10,20,40, or 100"
  msgnam="BAD_CLK_DIVIDE"



class A14BAD_MODE(A14Exception):
  status=662476682
  message="Mode must be in the range of 0 to 4"
  msgnam="BAD_MODE"



class A14BAD_CLK_POLARITY(A14Exception):
  status=662476690
  message="Clock polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_CLK_POLARITY"



class A14BAD_STR_POLARITY(A14Exception):
  status=662476698
  message="Start polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_STR_POLARITY"



class A14BAD_STP_POLARITY(A14Exception):
  status=662476706
  message="Stop polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_STP_POLARITY"



class A14BAD_GATED(A14Exception):
  status=662476714
  message="Gated clock must be either 0 (not gated) or 1 (gated)"
  msgnam="BAD_GATED"



class L6810Exception(MDSplusException):
  fac="L6810"


class L6810NORMAL(L6810Exception):
  status=662477065
  message="successful completion"
  msgnam="NORMAL"



class L6810BAD_ACTIVECHAN(L6810Exception):
  status=662477074
  message="Active chans must be 1, 2, or 4"
  msgnam="BAD_ACTIVECHAN"



class L6810BAD_ACTIVEMEM(L6810Exception):
  status=662477082
  message="Active memory must be power of 2 LE 8192"
  msgnam="BAD_ACTIVEMEM"



class L6810BAD_FREQUENCY(L6810Exception):
  status=662477090
  message="Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]"
  msgnam="BAD_FREQUENCY"



class L6810BAD_FULL_SCALE(L6810Exception):
  status=662477098
  message="Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]"
  msgnam="BAD_FULL_SCALE"



class L6810BAD_MEMORIES(L6810Exception):
  status=662477106
  message="Memories must 1 .. 16"
  msgnam="BAD_MEMORIES"



class L6810BAD_COUPLING(L6810Exception):
  status=662477114
  message="Channel source / coupling must be one of 0 .. 7"
  msgnam="BAD_COUPLING"



class L6810BAD_OFFSET(L6810Exception):
  status=662477122
  message="Offset must be between 0 and 255"
  msgnam="BAD_OFFSET"



class L6810BAD_SEGMENTS(L6810Exception):
  status=662477130
  message="Number of segments must be 1 .. 1024"
  msgnam="BAD_SEGMENTS"



class L6810BAD_TRIG_DELAY(L6810Exception):
  status=662477138
  message="Trigger delay must be between -8 and 247 in units of 8ths of segment size"
  msgnam="BAD_TRIG_DELAY"



class J_dacException(MDSplusException):
  fac="J_dac"


class J_dacOUTRNG(J_dacException):
  status=662477466
  message="Joerger DAC Channels out of range.  Bad chans code !XW"
  msgnam="OUTRNG"



class IncaaException(MDSplusException):
  fac="Incaa"


class IncaaBAD_ACTIVE_CHANS(IncaaException):
  status=662477866
  message="bad active channels selection"
  msgnam="BAD_ACTIVE_CHANS"



class IncaaBAD_MASTER(IncaaException):
  status=662477874
  message="bad master selection, must be 0 or 1"
  msgnam="BAD_MASTER"



class IncaaBAD_EXT_1MHZ(IncaaException):
  status=662477882
  message="bad ext 1mhz selection, must be 0 or 1"
  msgnam="BAD_EXT_1MHZ"



class IncaaBAD_PTSC(IncaaException):
  status=662477890
  message="bad PTSC setting"
  msgnam="BAD_PTSC"



class L8212Exception(MDSplusException):
  fac="L8212"


class L8212BAD_HEADER(L8212Exception):
  status=662478266
  message="Invalid header jumper information (e.g. 49414944432)"
  msgnam="BAD_HEADER"



class L8212BAD_MEMORIES(L8212Exception):
  status=662478274
  message="Invalid number of memories, must be 1 .. 16"
  msgnam="BAD_MEMORIES"



class L8212BAD_NOC(L8212Exception):
  status=662478282
  message="Invalid number of active channels"
  msgnam="BAD_NOC"



class L8212BAD_OFFSET(L8212Exception):
  status=662478290
  message="Invalid offset must be one of (0, -2048, -4096)"
  msgnam="BAD_OFFSET"



class L8212BAD_PTS(L8212Exception):
  status=662478298
  message="Invalid pts code, must be 0 .. 7"
  msgnam="BAD_PTS"



class L8212FREQ_TO_HIGH(L8212Exception):
  status=662478306
  message="Frequency to high for selected number of channels"
  msgnam="FREQ_TO_HIGH"



class L8212INVALID_NOC(L8212Exception):
  status=662478314
  message="Invalid number of active channels"
  msgnam="INVALID_NOC"



class MpbException(MDSplusException):
  fac="Mpb"


class MpbBADTIME(MpbException):
  status=662478666
  message="Could not read time"
  msgnam="BADTIME"



class MpbBADFREQ(MpbException):
  status=662478674
  message="Could not read frequency"
  msgnam="BADFREQ"



class L8828Exception(MDSplusException):
  fac="L8828"


class L8828BAD_OFFSET(L8828Exception):
  status=662479066
  message="Offset for L8828 must be between 0 and 255"
  msgnam="BAD_OFFSET"



class L8828BAD_PRETRIG(L8828Exception):
  status=662479074
  message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"
  msgnam="BAD_PRETRIG"



class L8828BAD_ACTIVEMEM(L8828Exception):
  status=662479082
  message="ACTIVEMEM must be beteen 16K and 2M"
  msgnam="BAD_ACTIVEMEM"



class L8828BAD_CLOCK(L8828Exception):
  status=662479090
  message="Invalid clock frequency specified."
  msgnam="BAD_CLOCK"



class L8818Exception(MDSplusException):
  fac="L8818"


class L8818BAD_OFFSET(L8818Exception):
  status=662479466
  message="Offset for L8828 must be between 0 and 255"
  msgnam="BAD_OFFSET"



class L8818BAD_PRETRIG(L8818Exception):
  status=662479474
  message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"
  msgnam="BAD_PRETRIG"



class L8818BAD_ACTIVEMEM(L8818Exception):
  status=662479482
  message="ACTIVEMEM must be beteen 16K and 2M"
  msgnam="BAD_ACTIVEMEM"



class L8818BAD_CLOCK(L8818Exception):
  status=662479490
  message="Invalid clock frequency specified."
  msgnam="BAD_CLOCK"



class J_tr612Exception(MDSplusException):
  fac="J_tr612"


class J_tr612BAD_ACTMEM(J_tr612Exception):
  status=662479546
  message="ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all"
  msgnam="BAD_ACTMEM"



class J_tr612BAD_PRETRIG(J_tr612Exception):
  status=662479554
  message="PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples"
  msgnam="BAD_PRETRIG"



class J_tr612BAD_MODE(J_tr612Exception):
  status=662479562
  message="MODE value out of range, must be 0 (for normal) or 1 (for burst mode)"
  msgnam="BAD_MODE"



class J_tr612BAD_FREQUENCY(J_tr612Exception):
  status=662479570
  message="FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external"
  msgnam="BAD_FREQUENCY"



class L8206Exception(MDSplusException):
  fac="L8206"


class L8206NODATA(L8206Exception):
  status=662479868
  message="no data has been written to memory"
  msgnam="NODATA"



class H912Exception(MDSplusException):
  fac="H912"


class H912BAD_CLOCK(H912Exception):
  status=662479946
  message="Bad value specified in INT_CLOCK node, use Setup device to correct"
  msgnam="BAD_CLOCK"



class H912BAD_BLOCKS(H912Exception):
  status=662479954
  message="Bad value specified in BLOCKS node, use Setup device to correct"
  msgnam="BAD_BLOCKS"



class H912BAD_PTS(H912Exception):
  status=662479962
  message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"
  msgnam="BAD_PTS"



class H908Exception(MDSplusException):
  fac="H908"


class H908BAD_CLOCK(H908Exception):
  status=662480026
  message="Bad value specified in INT_CLOCK node, use Setup device to correct"
  msgnam="BAD_CLOCK"



class H908BAD_ACTIVE_CHANS(H908Exception):
  status=662480034
  message="Bad value specified in ACTIVE_CHANS node, use Setup device to correct"
  msgnam="BAD_ACTIVE_CHANS"



class H908BAD_PTS(H908Exception):
  status=662480042
  message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"
  msgnam="BAD_PTS"



class Dsp2904Exception(MDSplusException):
  fac="Dsp2904"


class Dsp2904CHANNEL_READ_ERROR(Dsp2904Exception):
  status=662480106
  message="Error reading channel"
  msgnam="CHANNEL_READ_ERROR"



class PyException(MDSplusException):
  fac="Py"


class PyUNHANDLED_EXCEPTION(PyException):
  status=662480186
  message="Python device raised and exception, see log files for more details"
  msgnam="UNHANDLED_EXCEPTION"



class Dt196bException(MDSplusException):
  fac="Dt196b"


class Dt196bNO_SAMPLES(Dt196bException):
  status=662480266
  message="Module did not acquire any samples"
  msgnam="NO_SAMPLES"



class DevCANNOT_LOAD_SETTINGS(DevException):
  status=662480290
  message="Error loading settings from XML"
  msgnam="CANNOT_LOAD_SETTINGS"



class DevCANNOT_GET_BOARD_STATE(DevException):
  status=662480298
  message="Cannot retrieve state of daq board"
  msgnam="CANNOT_GET_BOARD_STATE"



class DevACQCMD_FAILED(DevException):
  status=662480306
  message="Error executing acqcmd on daq board"
  msgnam="ACQCMD_FAILED"



class DevACQ2SH_FAILED(DevException):
  status=662480314
  message="Error executing acq2sh command on daq board"
  msgnam="ACQ2SH_FAILED"



class DevBAD_PARAMETER(DevException):
  status=662480322
  message="Invalid parameter specified for device"
  msgnam="BAD_PARAMETER"



class DevCOMM_ERROR(DevException):
  status=662480330
  message="Error communicating with device"
  msgnam="COMM_ERROR"



class DevCAMERA_NOT_FOUND(DevException):
  status=662480338
  message="Could not find specified camera on the network"
  msgnam="CAMERA_NOT_FOUND"

