
class ServerNOT_DISPATCHED(Exception):
    msgnum=0xfe18008
    def __str__(self):
        return "action not dispatched, depended on failed action"

class ServerINVALID_DEPENDENCY(Exception):
    msgnum=0xfe18010
    def __str__(self):
        return "action dependency cannot be evaluated"

class ServerCANT_HAPPEN(Exception):
    msgnum=0xfe18018
    def __str__(self):
        return "action contains circular dependency or depends on action which was not dispatched"

class ServerINVSHOT(Exception):
    msgnum=0xfe18020
    def __str__(self):
        return "invalid shot number, cannot dispatch actions in model"

class ServerABORT(Exception):
    msgnum=0xfe18030
    def __str__(self):
        return "Server action was aborted"

class ServerPATH_DOWN(Exception):
    msgnum=0xfe18040
    def __str__(self):
        return "Path to server lost"

class DEVBAD_ENDIDX(Exception):
    msgnum=0x277c8008
    def __str__(self):
        return "unable to read end index for channel"

class DEVBAD_FILTER(Exception):
    msgnum=0x277c8010
    def __str__(self):
        return "illegal filter selected"

class DEVBAD_FREQ(Exception):
    msgnum=0x277c8018
    def __str__(self):
        return "illegal digitization frequency selected"

class DEVBAD_GAIN(Exception):
    msgnum=0x277c8020
    def __str__(self):
        return "illegal gain selected"

class DEVBAD_HEADER(Exception):
    msgnum=0x277c8028
    def __str__(self):
        return "unable to read header selection"

class DEVBAD_HEADER_IDX(Exception):
    msgnum=0x277c8030
    def __str__(self):
        return "unknown header configuration index"

class DEVBAD_MEMORIES(Exception):
    msgnum=0x277c8038
    def __str__(self):
        return "unable to read number of memory modules"

class DEVBAD_MODE(Exception):
    msgnum=0x277c8040
    def __str__(self):
        return "illegal mode selected"

class DEVBAD_NAME(Exception):
    msgnum=0x277c8048
    def __str__(self):
        return "unable to read module name"

class DEVBAD_OFFSET(Exception):
    msgnum=0x277c8050
    def __str__(self):
        return "illegal offset selected"

class DEVBAD_STARTIDX(Exception):
    msgnum=0x277c8058
    def __str__(self):
        return "unable to read start index for channel"

class DEVNOT_TRIGGERED(Exception):
    msgnum=0x277c8060
    def __str__(self):
        return "device was not triggered,  check wires and triggering device"

class DEVFREQ_TO_HIGH(Exception):
    msgnum=0x277c8068
    def __str__(self):
        return "the frequency is set to high for the requested number of channels"

class DEVINVALID_NOC(Exception):
    msgnum=0x277c8070
    def __str__(self):
        return "the NOC (number of channels) requested is greater than the physical number of channels"

class DEVRANGE_MISMATCH(Exception):
    msgnum=0x277c8078
    def __str__(self):
        return "the range specified on the menu doesn't match the range setting on the device"

class DEVCAMACERR(Exception):
    msgnum=0x277c8080
    def __str__(self):
        return "Error doing CAMAC IO"

class DEVBAD_VERBS(Exception):
    msgnum=0x277c8088
    def __str__(self):
        return "Error reading interpreter list (:VERBS)"

class DEVBAD_COMMANDS(Exception):
    msgnum=0x277c8090
    def __str__(self):
        return "Error reading command list"

class DEVCAM_ADNR(Exception):
    msgnum=0x277c8098
    def __str__(self):
        return "CAMAC: Address not recognized (2160)"

class DEVCAM_ERR(Exception):
    msgnum=0x277c80a0
    def __str__(self):
        return "CAMAC: Error reported by crate controler"

class DEVCAM_LOSYNC(Exception):
    msgnum=0x277c80a8
    def __str__(self):
        return "CAMAC: Lost Syncronization error"

class DEVCAM_LPE(Exception):
    msgnum=0x277c80b0
    def __str__(self):
        return "CAMAC: Longitudinal Parity error"

class DEVCAM_TMO(Exception):
    msgnum=0x277c80b8
    def __str__(self):
        return "CAMAC: Highway time out error"

class DEVCAM_TPE(Exception):
    msgnum=0x277c80c0
    def __str__(self):
        return "CAMAC: Transverse Parity error"

class DEVCAM_STE(Exception):
    msgnum=0x277c80c8
    def __str__(self):
        return "CAMAC: Serial Transmission error"

class DEVCAM_DERR(Exception):
    msgnum=0x277c80d0
    def __str__(self):
        return "CAMAC: Delayed error from SCC"

class DEVCAM_SQ(Exception):
    msgnum=0x277c80d8
    def __str__(self):
        return "CAMAC: I/O completion with Q = 1"

class DEVCAM_NOSQ(Exception):
    msgnum=0x277c80e0
    def __str__(self):
        return "CAMAC: I/O completion with Q = 0"

class DEVCAM_SX(Exception):
    msgnum=0x277c80e8
    def __str__(self):
        return "CAMAC: I/O completion with X = 1"

class DEVCAM_NOSX(Exception):
    msgnum=0x277c80f0
    def __str__(self):
        return "CAMAC: I/O completion with X = 0"

class DEVINV_SETUP(Exception):
    msgnum=0x277c80f8
    def __str__(self):
        return "device was not properly set up"

class RETICONNORMAL(Exception):
    msgnum=0x277c8198
    def __str__(self):
        return "successful completion"

class RETICONBAD_FRAMES(Exception):
    msgnum=0x277c81a0
    def __str__(self):
        return "frame count must be less than or equal to 2048"

class RETICONBAD_FRAME_SELECT(Exception):
    msgnum=0x277c81a8
    def __str__(self):
        return "frame interval must be 1,2,4,8,16,32 or 64"

class RETICONBAD_NUM_STATES(Exception):
    msgnum=0x277c81b0
    def __str__(self):
        return "number of states must be between 1 and 4"

class RETICONBAD_PERIOD(Exception):
    msgnum=0x277c81b8
    def __str__(self):
        return "period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec"

class RETICONBAD_PIXEL_SELECT(Exception):
    msgnum=0x277c81c0
    def __str__(self):
        return "pixel selection must be an array of 256 boolean values"

class RETICONDATA_CORRUPTED(Exception):
    msgnum=0x277c81c8
    def __str__(self):
        return "data in memory is corrupted or framing error detected, no data stored"

class RETICONTOO_MANY_FRAMES(Exception):
    msgnum=0x277c81d0
    def __str__(self):
        return "over 8192 frame start indicators in data read from memory"

class J221NORMAL(Exception):
    msgnum=0x277c8328
    def __str__(self):
        return "successful completion"

class J221INVALID_DATA(Exception):
    msgnum=0x277c8330
    def __str__(self):
        return "ignoring invalid data in channel !SL"

class J221NO_DATA(Exception):
    msgnum=0x277c8338
    def __str__(self):
        return "no valid data was found for any channel"

class TIMINGINVCLKFRQ(Exception):
    msgnum=0x277c84b8
    def __str__(self):
        return "Invalid clock frequency"

class TIMINGINVDELDUR(Exception):
    msgnum=0x277c84c0
    def __str__(self):
        return "Invalid pulse delay or duration, must be less than 655 seconds"

class TIMINGINVOUTCTR(Exception):
    msgnum=0x277c84c8
    def __str__(self):
        return "Invalid output mode selected"

class TIMINGINVPSEUDODEV(Exception):
    msgnum=0x277c84d0
    def __str__(self):
        return "Invalid pseudo device attached to this decoder channel"

class TIMINGINVTRGMOD(Exception):
    msgnum=0x277c84d8
    def __str__(self):
        return "Invalid trigger mode selected"

class TIMINGNOPSEUDODEV(Exception):
    msgnum=0x277c84e0
    def __str__(self):
        return "No Pseudo device attached to this channel ... disabling"

class TIMINGTOO_MANY_EVENTS(Exception):
    msgnum=0x277c84e8
    def __str__(self):
        return "More than 16 events used by this decoder"

class B2408NORMAL(Exception):
    msgnum=0x277c8648
    def __str__(self):
        return "successful completion"

class B2408OVERFLOW(Exception):
    msgnum=0x277c8650
    def __str__(self):
        return "Triggers received after overflow"

class B2408TRIG_LIM(Exception):
    msgnum=0x277c8658
    def __str__(self):
        return "Trigger limit possibly exceeded"

class FERANORMAL(Exception):
    msgnum=0x277c87d8
    def __str__(self):
        return "successful completion"

class FERADIGNOTSTRARRAY(Exception):
    msgnum=0x277c87e0
    def __str__(self):
        return "The digitizer names must be an array of strings"

class FERANODIG(Exception):
    msgnum=0x277c87e8
    def __str__(self):
        return "The digitizer names must be specified"

class FERAMEMNOTSTRARRAY(Exception):
    msgnum=0x277c87f0
    def __str__(self):
        return "The memory names must be an array of strings"

class FERANOMEM(Exception):
    msgnum=0x277c87f8
    def __str__(self):
        return "The memory names must be specified"

class FERAPHASE_LOST(Exception):
    msgnum=0x277c8800
    def __str__(self):
        return "Data phase lost No FERA data stored"

class FERACONFUSED(Exception):
    msgnum=0x277c8808
    def __str__(self):
        return "Fera Data inconsitant.  Data for this point zered."

class FERAOVER_RUN(Exception):
    msgnum=0x277c8810
    def __str__(self):
        return "Possible FERA memory overrun, too many triggers."

class FERAOVERFLOW(Exception):
    msgnum=0x277c8818
    def __str__(self):
        return "Possible FERA data saturated.  Data point zeroed"

class HM650NORMAL(Exception):
    msgnum=0x277c8968
    def __str__(self):
        return "successful completion"

class HM650DLYCHNG(Exception):
    msgnum=0x277c8970
    def __str__(self):
        return "HM650 requested delay can not be processed by hardware."

class HV4032NORMAL(Exception):
    msgnum=0x277c8af8
    def __str__(self):
        return "successful completion"

class HV4032WRONG_POD_TYPE(Exception):
    msgnum=0x277c8b00
    def __str__(self):
        return "HV40321A n and p can only be used with the HV4032 device"

class HV1440NORMAL(Exception):
    msgnum=0x277c8c88
    def __str__(self):
        return "successful completion"

class HV1440WRONG_POD_TYPE(Exception):
    msgnum=0x277c8c90
    def __str__(self):
        return "HV1443 can only be used with the HV1440 device"

class HV1440BAD_FRAME(Exception):
    msgnum=0x277c8c98
    def __str__(self):
        return "HV1440 could not read the frame"

class HV1440BAD_RANGE(Exception):
    msgnum=0x277c8ca0
    def __str__(self):
        return "HV1440 could not read the range"

class HV1440OUTRNG(Exception):
    msgnum=0x277c8ca8
    def __str__(self):
        return "HV1440 out of range"

class HV1440STUCK(Exception):
    msgnum=0x277c8cb0
    def __str__(self):
        return "HV1440 not responding with Q"

class JOERGERBAD_PRE_TRIGGER(Exception):
    msgnum=0x277c8e18
    def __str__(self):
        return "bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7"

class JOERGERBAD_ACT_MEMORY(Exception):
    msgnum=0x277c8e20
    def __str__(self):
        return "bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8"

class JOERGERBAD_GAIN(Exception):
    msgnum=0x277c8e28
    def __str__(self):
        return "bad gain specified, specify a value of 1,2,4 or 8"

class U_OF_MBAD_WAVE_LENGTH(Exception):
    msgnum=0x277c8fa8
    def __str__(self):
        return "bad wave length specified, specify value between 0 and 13000"

class U_OF_MBAD_SLIT_WIDTH(Exception):
    msgnum=0x277c8fb0
    def __str__(self):
        return "bad slit width specified, specify value between 0 and 500"

class U_OF_MBAD_NUM_SPECTRA(Exception):
    msgnum=0x277c8fb8
    def __str__(self):
        return "bad number of spectra specified, specify value between 1 and 100"

class U_OF_MBAD_GRATING(Exception):
    msgnum=0x277c8fc0
    def __str__(self):
        return "bad grating type specified, specify value between 1 and 5"

class U_OF_MBAD_EXPOSURE(Exception):
    msgnum=0x277c8fc8
    def __str__(self):
        return "bad exposure time specified, specify value between 30 and 3000"

class U_OF_MBAD_FILTER(Exception):
    msgnum=0x277c8fd0
    def __str__(self):
        return "bad neutral density filter specified, specify value between 0 and 5"

class U_OF_MGO_FILE_ERROR(Exception):
    msgnum=0x277c8fd8
    def __str__(self):
        return "error creating new go file"

class U_OF_MDATA_FILE_ERROR(Exception):
    msgnum=0x277c8fe0
    def __str__(self):
        return "error opening datafile"

class IDLNORMAL(Exception):
    msgnum=0x277c9138
    def __str__(self):
        return "successful completion"

class IDLERROR(Exception):
    msgnum=0x277c9140
    def __str__(self):
        return "IDL returned a non zero error code"

class B5910ABAD_CHAN(Exception):
    msgnum=0x277c92c8
    def __str__(self):
        return "error evaluating data for channel !SL"

class B5910ABAD_CLOCK(Exception):
    msgnum=0x277c92d0
    def __str__(self):
        return "invalid internal clock range specified"

class B5910ABAD_ITERATIONS(Exception):
    msgnum=0x277c92d8
    def __str__(self):
        return "invalid number of iterations specified"

class B5910ABAD_NOC(Exception):
    msgnum=0x277c92e0
    def __str__(self):
        return "invalid number of active channels specified"

class B5910ABAD_SAMPS(Exception):
    msgnum=0x277c92e8
    def __str__(self):
        return "number of samples specificed invalid"

class J412NOT_SORTED(Exception):
    msgnum=0x277c9458
    def __str__(self):
        return "times specified for J412 module were not sorted"

class J412NO_DATA(Exception):
    msgnum=0x277c9460
    def __str__(self):
        return "there were no times specifed for J412 module"

class J412BADCYCLES(Exception):
    msgnum=0x277c9468
    def __str__(self):
        return "The number of cycles must be 1 .. 255"

class TR16NORMAL(Exception):
    msgnum=0x277c95e8
    def __str__(self):
        return "successful completion"

class TR16BAD_MEMSIZE(Exception):
    msgnum=0x277c95f0
    def __str__(self):
        return "Memory size must be in 128K, 256K, 512k, 1024K"

class TR16BAD_ACTIVEMEM(Exception):
    msgnum=0x277c95f8
    def __str__(self):
        return "Active Mem must be power of 2 8K to 1024K"

class TR16BAD_ACTIVECHAN(Exception):
    msgnum=0x277c9600
    def __str__(self):
        return "Active channels must be in 1,2,4,8,16"

class TR16BAD_PTS(Exception):
    msgnum=0x277c9608
    def __str__(self):
        return "PTS must be power of 2 32 to 1024K"

class TR16BAD_FREQUENCY(Exception):
    msgnum=0x277c9610
    def __str__(self):
        return "Invalid clock frequency"

class TR16BAD_MASTER(Exception):
    msgnum=0x277c9618
    def __str__(self):
        return "Master must be 0 or 1"

class TR16BAD_GAIN(Exception):
    msgnum=0x277c9620
    def __str__(self):
        return "Gain must be 1, 2, 4, or 8"

class A14NORMAL(Exception):
    msgnum=0x277c9778
    def __str__(self):
        return "successful completion"

class A14BAD_CLK_DIVIDE(Exception):
    msgnum=0x277c9780
    def __str__(self):
        return "Clock divide must be one of 1,2,4,10,20,40, or 100"

class A14BAD_MODE(Exception):
    msgnum=0x277c9788
    def __str__(self):
        return "Mode must be in the range of 0 to 4"

class A14BAD_CLK_POLARITY(Exception):
    msgnum=0x277c9790
    def __str__(self):
        return "Clock polarity must be either 0 (rising) or 1 (falling)"

class A14BAD_STR_POLARITY(Exception):
    msgnum=0x277c9798
    def __str__(self):
        return "Start polarity must be either 0 (rising) or 1 (falling)"

class A14BAD_STP_POLARITY(Exception):
    msgnum=0x277c97a0
    def __str__(self):
        return "Stop polarity must be either 0 (rising) or 1 (falling)"

class A14BAD_GATED(Exception):
    msgnum=0x277c97a8
    def __str__(self):
        return "Gated clock must be either 0 (not gated) or 1 (gated)"

class L6810NORMAL(Exception):
    msgnum=0x277c9908
    def __str__(self):
        return "successful completion"

class L6810BAD_ACTIVECHAN(Exception):
    msgnum=0x277c9910
    def __str__(self):
        return "Active chans must be 1, 2, or 4"

class L6810BAD_ACTIVEMEM(Exception):
    msgnum=0x277c9918
    def __str__(self):
        return "Active memory must be power of 2 LE 8192"

class L6810BAD_FREQUENCY(Exception):
    msgnum=0x277c9920
    def __str__(self):
        return "Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]"

class L6810BAD_FULL_SCALE(Exception):
    msgnum=0x277c9928
    def __str__(self):
        return "Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]"

class L6810BAD_MEMORIES(Exception):
    msgnum=0x277c9930
    def __str__(self):
        return "Memories must 1 .. 16"

class L6810BAD_COUPLING(Exception):
    msgnum=0x277c9938
    def __str__(self):
        return "Channel source / coupling must be one of 0 .. 7"

class L6810BAD_OFFSET(Exception):
    msgnum=0x277c9940
    def __str__(self):
        return "Offset must be between 0 and 255"

class L6810BAD_SEGMENTS(Exception):
    msgnum=0x277c9948
    def __str__(self):
        return "Number of segments must be 1 .. 1024"

class L6810BAD_TRIG_DELAY(Exception):
    msgnum=0x277c9950
    def __str__(self):
        return "Trigger delay must be between -8 and 247 in units of 8ths of segment size"

class J_DACOUTRNG(Exception):
    msgnum=0x277c9a98
    def __str__(self):
        return "Joerger DAC Channels out of range.  Bad chans code !XW"

class INCAABAD_ACTIVE_CHANS(Exception):
    msgnum=0x277c9c28
    def __str__(self):
        return "bad active channels selection"

class INCAABAD_MASTER(Exception):
    msgnum=0x277c9c30
    def __str__(self):
        return "bad master selection, must be 0 or 1"

class INCAABAD_EXT_1MHZ(Exception):
    msgnum=0x277c9c38
    def __str__(self):
        return "bad ext 1mhz selection, must be 0 or 1"

class INCAABAD_PTSC(Exception):
    msgnum=0x277c9c40
    def __str__(self):
        return "bad PTSC setting"

class L8212BAD_HEADER(Exception):
    msgnum=0x277c9db8
    def __str__(self):
        return "Invalid header jumper information (e.g. 49414944432)"

class L8212BAD_MEMORIES(Exception):
    msgnum=0x277c9dc0
    def __str__(self):
        return "Invalid number of memories, must be 1 .. 16"

class L8212BAD_NOC(Exception):
    msgnum=0x277c9dc8
    def __str__(self):
        return "Invalid number of active channels"

class L8212BAD_OFFSET(Exception):
    msgnum=0x277c9dd0
    def __str__(self):
        return "Invalid offset must be one of (0, -2048, -4096)"

class L8212BAD_PTS(Exception):
    msgnum=0x277c9dd8
    def __str__(self):
        return "Invalid pts code, must be 0 .. 7"

class L8212FREQ_TO_HIGH(Exception):
    msgnum=0x277c9de0
    def __str__(self):
        return "Frequency to high for selected number of channels"

class L8212INVALID_NOC(Exception):
    msgnum=0x277c9de8
    def __str__(self):
        return "Invalid number of active channels"

class MPBBADTIME(Exception):
    msgnum=0x277c9f48
    def __str__(self):
        return "Could not read time"

class MPBBADFREQ(Exception):
    msgnum=0x277c9f50
    def __str__(self):
        return "Could not read frequency"

class L8828BAD_OFFSET(Exception):
    msgnum=0x277ca0d8
    def __str__(self):
        return "Offset for L8828 must be between 0 and 255"

class L8828BAD_PRETRIG(Exception):
    msgnum=0x277ca0e0
    def __str__(self):
        return "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"

class L8828BAD_ACTIVEMEM(Exception):
    msgnum=0x277ca0e8
    def __str__(self):
        return "ACTIVEMEM must be beteen 16K and 2M"

class L8828BAD_CLOCK(Exception):
    msgnum=0x277ca0f0
    def __str__(self):
        return "Invalid clock frequency specified."

class L8818BAD_OFFSET(Exception):
    msgnum=0x277ca268
    def __str__(self):
        return "Offset for L8828 must be between 0 and 255"

class L8818BAD_PRETRIG(Exception):
    msgnum=0x277ca270
    def __str__(self):
        return "Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"

class L8818BAD_ACTIVEMEM(Exception):
    msgnum=0x277ca278
    def __str__(self):
        return "ACTIVEMEM must be beteen 16K and 2M"

class L8818BAD_CLOCK(Exception):
    msgnum=0x277ca280
    def __str__(self):
        return "Invalid clock frequency specified."

class J_TR612BAD_ACTMEM(Exception):
    msgnum=0x277ca2b8
    def __str__(self):
        return "ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all"

class J_TR612BAD_PRETRIG(Exception):
    msgnum=0x277ca2c0
    def __str__(self):
        return "PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples"

class J_TR612BAD_MODE(Exception):
    msgnum=0x277ca2c8
    def __str__(self):
        return "MODE value out of range, must be 0 (for normal) or 1 (for burst mode)"

class J_TR612BAD_FREQUENCY(Exception):
    msgnum=0x277ca2d0
    def __str__(self):
        return "FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external"

class L8206NODATA(Exception):
    msgnum=0x277ca3f8
    def __str__(self):
        return "no data has been written to memory"

class H912BAD_CLOCK(Exception):
    msgnum=0x277ca448
    def __str__(self):
        return "Bad value specified in INT_CLOCK node, use Setup device to correct"

class H912BAD_BLOCKS(Exception):
    msgnum=0x277ca450
    def __str__(self):
        return "Bad value specified in BLOCKS node, use Setup device to correct"

class H912BAD_PTS(Exception):
    msgnum=0x277ca458
    def __str__(self):
        return "Bad value specfiied in PTS node, must be an integer value between 1 and 131071"

class H908BAD_CLOCK(Exception):
    msgnum=0x277ca498
    def __str__(self):
        return "Bad value specified in INT_CLOCK node, use Setup device to correct"

class H908BAD_ACTIVE_CHANS(Exception):
    msgnum=0x277ca4a0
    def __str__(self):
        return "Bad value specified in ACTIVE_CHANS node, use Setup device to correct"

class H908BAD_PTS(Exception):
    msgnum=0x277ca4a8
    def __str__(self):
        return "Bad value specfiied in PTS node, must be an integer value between 1 and 131071"

class DSP2904CHANNEL_READ_ERROR(Exception):
    msgnum=0x277ca4e8
    def __str__(self):
        return "Error reading channel"

class PYUNHANDLED_EXCEPTION(Exception):
    msgnum=0x277ca538
    def __str__(self):
        return "Python device raised and exception, see log files for more details"

class DT196BNO_SAMPLES(Exception):
    msgnum=0x277ca588
    def __str__(self):
        return "Module did not acquire any samples"

class ACQINITIALIZATION_ERROR(Exception):
    msgnum=0x277ca590
    def __str__(self):
        return "Error during module initialization"

class ACQSETTINGS_NOT_LOADED(Exception):
    msgnum=0x277ca598
    def __str__(self):
        return "settings not loaded"

class ACQWRONG_TREE(Exception):
    msgnum=0x277ca5a0
    def __str__(self):
        return "Attempt to store ACQ module into different tree than it was armed with"

class ACQWRONG_PATH(Exception):
    msgnum=0x277ca5a8
    def __str__(self):
        return "Attempt to store ACQ module into different path than it was armed with"

class ACQWRONG_SHOT(Exception):
    msgnum=0x277ca5b0
    def __str__(self):
        return "Attempt to store ACQ module into different shot than it was armed with"

class LibINSVIRMEM(Exception):
    msgnum=0x158210
    def __str__(self):
        return "Insufficient virtual memory"

class LibINVARG(Exception):
    msgnum=0x158230
    def __str__(self):
        return "Invalid argument"

class LibINVSTRDES(Exception):
    msgnum=0x158220
    def __str__(self):
        return "Invalid string descriptor"

class LibKEYNOTFOU(Exception):
    msgnum=0x1582f8
    def __str__(self):
        return "Key not found"

class LibNOTFOU(Exception):
    msgnum=0x158270
    def __str__(self):
        return "Entity not found"

class LibQUEWASEMP(Exception):
    msgnum=0x1582e8
    def __str__(self):
        return "Queue was empty"

class LibSTRTRU(Exception):
    msgnum=0x158010
    def __str__(self):
        return "String truncated"

class StrMATCH(Exception):
    msgnum=0x248418
    def __str__(self):
        return "Strings match"

class StrNOMATCH(Exception):
    msgnum=0x248208
    def __str__(self):
        return "Strings do not match"

class StrNOELEM(Exception):
    msgnum=0x248218
    def __str__(self):
        return "Not enough delimited characters"

class StrINVDELIM(Exception):
    msgnum=0x248210
    def __str__(self):
        return "Not enough delimited characters"

class StrSTRTOOLON(Exception):
    msgnum=0x248070
    def __str__(self):
        return "String too long"

class SsINTOVF(Exception):
    msgnum=0x478
    def __str__(self):
        return "Integer overflow"

class CamDONE_Q(Exception):
    msgnum=0x8018008
    def __str__(self):
        return "I/O completed with X=1, Q=1"

class CamDONE_NOQ(Exception):
    msgnum=0x8018010
    def __str__(self):
        return "I/O completed with X=1, Q=0"

class CamDONE_NOX(Exception):
    msgnum=0x8019000
    def __str__(self):
        return "I/O completed with X=0 - probable failure"

class CamSERTRAERR(Exception):
    msgnum=0x8019800
    def __str__(self):
        return "serial transmission error on highway"

class CamSCCFAIL(Exception):
    msgnum=0x8019840
    def __str__(self):
        return "serial crate controller failure"

class CamOFFLINE(Exception):
    msgnum=0x8019868
    def __str__(self):
        return "crate is offline"

class TclNORMAL(Exception):
    msgnum=0x2a0008
    def __str__(self):
        return "Normal successful completion"

class TclFAILED_ESSENTIAL(Exception):
    msgnum=0x2a0010
    def __str__(self):
        return "Essential action failed"

class MdsdclSUCCESS(Exception):
    msgnum=0x8020008
    def __str__(self):
        return "Normal successful completion"

class MdsdclEXIT(Exception):
    msgnum=0x8020010
    def __str__(self):
        return "Normal exit"

class MdsdclERROR(Exception):
    msgnum=0x8020018
    def __str__(self):
        return "Unsuccessful execution of command"

class MdsdclNORMAL(Exception):
    msgnum=0x8020328
    def __str__(self):
        return "Normal successful completion"

class MdsdclPRESENT(Exception):
    msgnum=0x8020330
    def __str__(self):
        return "Entity is present"

class MdsdclIVVERB(Exception):
    msgnum=0x8020338
    def __str__(self):
        return "No such command"

class MdsdclABSENT(Exception):
    msgnum=0x8020340
    def __str__(self):
        return "Entity is absent"

class MdsdclNEGATED(Exception):
    msgnum=0x8020348
    def __str__(self):
        return "Entity is present but negated"

class MdsdclNOTNEGATABLE(Exception):
    msgnum=0x8020350
    def __str__(self):
        return "Entity cannot be negated"

class MdsdclIVQUAL(Exception):
    msgnum=0x8020358
    def __str__(self):
        return "Invalid qualifier"

class MdsdclPROMPT_MORE(Exception):
    msgnum=0x8020360
    def __str__(self):
        return "More input required for command"

class MdsdclTOO_MANY_PRMS(Exception):
    msgnum=0x8020368
    def __str__(self):
        return "Too many parameters specified"

class MdsdclTOO_MANY_VALS(Exception):
    msgnum=0x8020370
    def __str__(self):
        return "Too many values"

class MdsdclMISSING_VALUE(Exception):
    msgnum=0x8020378
    def __str__(self):
        return "Qualifier value needed"

class TreeALREADY_OFF(Exception):
    msgnum=0xfd18028
    def __str__(self):
        return "Node is already OFF"

class TreeALREADY_ON(Exception):
    msgnum=0xfd18030
    def __str__(self):
        return "Node is already ON"

class TreeALREADY_OPEN(Exception):
    msgnum=0xfd18038
    def __str__(self):
        return "Tree is already OPEN"

class TreeALREADY_THERE(Exception):
    msgnum=0xfd18088
    def __str__(self):
        return "Node is already in the tree"

class TreeBADRECORD(Exception):
    msgnum=0xfd180b8
    def __str__(self):
        return "Data corrupted: cannot read record"

class TreeBOTH_OFF(Exception):
    msgnum=0xfd18098
    def __str__(self):
        return "Both this node and its parent are off"

class TreeBUFFEROVF(Exception):
    msgnum=0xfd18110
    def __str__(self):
        return "Output buffer overflow"

class TreeCONGLOMFULL(Exception):
    msgnum=0xfd18120
    def __str__(self):
        return "Current conglomerate is full"

class TreeCONGLOM_NOT_FULL(Exception):
    msgnum=0xfd18128
    def __str__(self):
        return "Current conglomerate is not yet full"

class TreeCONTINUING(Exception):
    msgnum=0xfd18960
    def __str__(self):
        return "Operation continuing: note following error"

class TreeDUPTAG(Exception):
    msgnum=0xfd180c8
    def __str__(self):
        return "Tag name already in use"

class TreeEDITTING(Exception):
    msgnum=0xfd18190
    def __str__(self):
        return "Tree file open for edit: operation not permitted"

class TreeILLEGAL_ITEM(Exception):
    msgnum=0xfd18108
    def __str__(self):
        return "Invalid item code or part number specified"

class TreeILLPAGCNT(Exception):
    msgnum=0xfd180d0
    def __str__(self):
        return "Illegal page count, error mapping tree file"

class TreeINVDFFCLASS(Exception):
    msgnum=0xfd18138
    def __str__(self):
        return "Invalid data fmt: only CLASS_S can have data in NCI"

class TreeINVDTPUSG(Exception):
    msgnum=0xfd18188
    def __str__(self):
        return "Attempt to store datatype which conflicts with the designated usage of this node"

class TreeINVPATH(Exception):
    msgnum=0xfd18100
    def __str__(self):
        return "Invalid tree pathname specified"

class TreeINVRECTYP(Exception):
    msgnum=0xfd18140
    def __str__(self):
        return "Record type invalid for requested operation"

class TreeINVTREE(Exception):
    msgnum=0xfd180c0
    def __str__(self):
        return "Invalid tree identification structure"

class TreeMAXOPENEDIT(Exception):
    msgnum=0xfd180d8
    def __str__(self):
        return "Too many files open for edit"

class TreeNEW(Exception):
    msgnum=0xfd18018
    def __str__(self):
        return "New tree created"

class TreeNMN(Exception):
    msgnum=0xfd18060
    def __str__(self):
        return "No More Nodes"

class TreeNMT(Exception):
    msgnum=0xfd18068
    def __str__(self):
        return "No More Tags"

class TreeNNF(Exception):
    msgnum=0xfd18070
    def __str__(self):
        return "Node Not Found"

class TreeNODATA(Exception):
    msgnum=0xfd180e0
    def __str__(self):
        return "No data available for this node"

class TreeNODNAMLEN(Exception):
    msgnum=0xfd18148
    def __str__(self):
        return "Node name too long (12 chars max)"

class TreeNOEDIT(Exception):
    msgnum=0xfd180f0
    def __str__(self):
        return "Tree file is not open for edit"

class TreeNOLOG(Exception):
    msgnum=0xfd181a8
    def __str__(self):
        return "Experiment pathname (xxx_path) not defined"

class TreeNOMETHOD(Exception):
    msgnum=0xfd180b0
    def __str__(self):
        return "Method not available for this object"

class TreeNOOVERWRITE(Exception):
    msgnum=0xfd18180
    def __str__(self):
        return "Write-once node: overwrite not permitted"

class TreeNORMAL(Exception):
    msgnum=0xfd18008
    def __str__(self):
        return "Normal successful completion"

class TreeNOTALLSUBS(Exception):
    msgnum=0xfd18020
    def __str__(self):
        return "Main tree opened but not all subtrees found/or connected"

class TreeNOTCHILDLESS(Exception):
    msgnum=0xfd180f8
    def __str__(self):
        return "Node must be childless to become subtree reference"

class TreeNOT_IN_LIST(Exception):
    msgnum=0xfd181c0
    def __str__(self):
        return "Tree being opened was not in the list"

class TreeNOTMEMBERLESS(Exception):
    msgnum=0xfd18170
    def __str__(self):
        return "Subtree reference can not have members"

class TreeNOTOPEN(Exception):
    msgnum=0xfd180e8
    def __str__(self):
        return "No tree file currently open"

class TreeNOTSON(Exception):
    msgnum=0xfd18178
    def __str__(self):
        return "Subtree reference cannot be a member"

class TreeNOT_CONGLOM(Exception):
    msgnum=0xfd18160
    def __str__(self):
        return "Head node of conglomerate does not contain a DTYPE_CONGLOM record"

class TreeNOT_OPEN(Exception):
    msgnum=0xfd180a8
    def __str__(self):
        return "Tree not currently open"

class TreeNOWRITEMODEL(Exception):
    msgnum=0xfd18198
    def __str__(self):
        return "Data for this node can not be written into the MODEL file"

class TreeNOWRITESHOT(Exception):
    msgnum=0xfd181a0
    def __str__(self):
        return "Data for this node can not be written into the SHOT file"

class TreeNO_CONTEXT(Exception):
    msgnum=0xfd18040
    def __str__(self):
        return "There is no active search to end"

class TreeOFF(Exception):
    msgnum=0xfd180a0
    def __str__(self):
        return "Node is OFF"

class TreeON(Exception):
    msgnum=0xfd18048
    def __str__(self):
        return "Node is ON"

class TreeOPEN(Exception):
    msgnum=0xfd18050
    def __str__(self):
        return "Tree is OPEN (no edit)"

class TreeOPEN_EDIT(Exception):
    msgnum=0xfd18058
    def __str__(self):
        return "Tree is OPEN for edit"

class TreePARENT_OFF(Exception):
    msgnum=0xfd18090
    def __str__(self):
        return "Parent of this node is OFF"

class TreeREADERR(Exception):
    msgnum=0xfd181b8
    def __str__(self):
        return "Error reading record for node"

class TreeREADONLY(Exception):
    msgnum=0xfd181b0
    def __str__(self):
        return "Tree was opened with readonly access"

class TreeRESOLVED(Exception):
    msgnum=0xfd18010
    def __str__(self):
        return "Indirect reference successfully resolved"

class TreeSUCCESS(Exception):
    msgnum=0xfd18640
    def __str__(self):
        return "Operation successful"

class TreeTAGNAMLEN(Exception):
    msgnum=0xfd18150
    def __str__(self):
        return "Tagname too long (max 24 chars)"

class TreeTNF(Exception):
    msgnum=0xfd18078
    def __str__(self):
        return "Tag Not Found"

class TreeTREENF(Exception):
    msgnum=0xfd18080
    def __str__(self):
        return "Tree Not Found"

class TreeUNRESOLVED(Exception):
    msgnum=0xfd18130
    def __str__(self):
        return "Not an indirect node reference: No action taken"

class TreeUNSPRTCLASS(Exception):
    msgnum=0xfd18118
    def __str__(self):
        return "Unsupported descriptor class"

class TreeUNSUPARRDTYPE(Exception):
    msgnum=0xfd18168
    def __str__(self):
        return "Complex data types not supported as members of arrays"

class TreeWRITEFIRST(Exception):
    msgnum=0xfd18158
    def __str__(self):
        return "Tree has been modified:  write or quit first"

class TreeFAILURE(Exception):
    msgnum=0xfd18fa0
    def __str__(self):
        return "Operation NOT successful"

class TreeLOCK_FAILURE(Exception):
    msgnum=0xfd18fb0
    def __str__(self):
        return "Error locking file, perhaps NFSLOCKING not enabled on this system"

class TreeFILE_NOT_FOUND(Exception):
    msgnum=0xfd18fa8
    def __str__(self):
        return "File or Directory Not Found"

class TreeCANCEL(Exception):
    msgnum=0xfd18c80
    def __str__(self):
        return "User canceled operation"

class TreeNOSEGMENTS(Exception):
    msgnum=0xfd18fb8
    def __str__(self):
        return "No segments exist in this node"

class TreeINVDTYPE(Exception):
    msgnum=0xfd18fc0
    def __str__(self):
        return "Invalid datatype for data segment"

class TreeINVSHAPE(Exception):
    msgnum=0xfd18fc8
    def __str__(self):
        return "Invalid shape for this data segment"

class TreeINVSHOT(Exception):
    msgnum=0xfd18fd8
    def __str__(self):
        return "Invalid shot number - must be -1 (model), 0 (current), or Positive"

class TreeINVTAG(Exception):
    msgnum=0xfd18fe8
    def __str__(self):
        return "Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores"

class TreeNOPATH(Exception):
    msgnum=0xfd18ff0
    def __str__(self):
        return "No 'treename'_path environment variable defined. Cannot locate tree files."

class TreeTREEFILEREADERR(Exception):
    msgnum=0xfd18ff8
    def __str__(self):
        return "Error reading in tree file contents."

class TreeMEMERR(Exception):
    msgnum=0xfd19000
    def __str__(self):
        return "Memory allocation error."

class TreeNOCURRENT(Exception):
    msgnum=0xfd19008
    def __str__(self):
        return "No current shot number set for this tree."

class TreeFOPENW(Exception):
    msgnum=0xfd19010
    def __str__(self):
        return "Error opening file for read-write."

class TreeFOPENR(Exception):
    msgnum=0xfd19018
    def __str__(self):
        return "Error opening file read-only."

class TreeFCREATE(Exception):
    msgnum=0xfd19020
    def __str__(self):
        return "Error creating new file."

class TreeCONNECTFAIL(Exception):
    msgnum=0xfd19028
    def __str__(self):
        return "Error connecting to remote server."

class TreeNCIWRITE(Exception):
    msgnum=0xfd19030
    def __str__(self):
        return "Error writing node characterisitics to file."

class TreeDELFAIL(Exception):
    msgnum=0xfd19038
    def __str__(self):
        return "Error deleting file."

class TreeRENFAIL(Exception):
    msgnum=0xfd19040
    def __str__(self):
        return "Error renaming file."

class TreeEMPTY(Exception):
    msgnum=0xfd19048
    def __str__(self):
        return "Empty string provided."

class TreePARSEERR(Exception):
    msgnum=0xfd19050
    def __str__(self):
        return "Invalid node search string."

class TreeNCIREAD(Exception):
    msgnum=0xfd19058
    def __str__(self):
        return "Error reading node characteristics from file."

class TreeNOVERSION(Exception):
    msgnum=0xfd19060
    def __str__(self):
        return "No version available."

class TreeDFREAD(Exception):
    msgnum=0xfd19068
    def __str__(self):
        return "Error reading from datafile."

class TdiBREAK(Exception):
    msgnum=0xfd38008
    def __str__(self):
        return "BREAK was not in DO FOR SWITCH or WHILE"

class TdiCASE(Exception):
    msgnum=0xfd38010
    def __str__(self):
        return "CASE was not in SWITCH statement"

class TdiCONTINUE(Exception):
    msgnum=0xfd38018
    def __str__(self):
        return "CONTINUE was not in DO FOR or WHILE"

class TdiEXTRANEOUS(Exception):
    msgnum=0xfd38020
    def __str__(self):
        return "Some characters were unused, bad number maybe"

class TdiRETURN(Exception):
    msgnum=0xfd38028
    def __str__(self):
        return "Extraneous RETURN statement, not from a FUN"

class TdiABORT(Exception):
    msgnum=0xfd38030
    def __str__(self):
        return "Program requested abort"

class TdiBAD_INDEX(Exception):
    msgnum=0xfd38038
    def __str__(self):
        return "Index or subscript is too small or too big"

class TdiBOMB(Exception):
    msgnum=0xfd38040
    def __str__(self):
        return "Bad punctuation, could not compile the text"

class TdiEXTRA_ARG(Exception):
    msgnum=0xfd38048
    def __str__(self):
        return "Too many arguments for function, watch commas"

class TdiGOTO(Exception):
    msgnum=0xfd38050
    def __str__(self):
        return "GOTO target label not found"

class TdiINVCLADSC(Exception):
    msgnum=0xfd38058
    def __str__(self):
        return "Storage class not valid, must be scalar or array"

class TdiINVCLADTY(Exception):
    msgnum=0xfd38060
    def __str__(self):
        return "Invalid mixture of storage class and data type"

class TdiINVDTYDSC(Exception):
    msgnum=0xfd38068
    def __str__(self):
        return "Storage data type is not valid"

class TdiINV_OPC(Exception):
    msgnum=0xfd38070
    def __str__(self):
        return "Invalid operator code in a function"

class TdiINV_SIZE(Exception):
    msgnum=0xfd38078
    def __str__(self):
        return "Number of elements does not match declaration"

class TdiMISMATCH(Exception):
    msgnum=0xfd38080
    def __str__(self):
        return "Shape of arguments does not match"

class TdiMISS_ARG(Exception):
    msgnum=0xfd38088
    def __str__(self):
        return "Missing argument is required for function"

class TdiNDIM_OVER(Exception):
    msgnum=0xfd38090
    def __str__(self):
        return "Number of dimensions is over the allowed 8"

class TdiNO_CMPLX(Exception):
    msgnum=0xfd38098
    def __str__(self):
        return "There are no complex forms of this function"

class TdiNO_OPC(Exception):
    msgnum=0xfd380a0
    def __str__(self):
        return "No support for this function, today"

class TdiNO_OUTPTR(Exception):
    msgnum=0xfd380a8
    def __str__(self):
        return "An output pointer is required"

class TdiNO_SELF_PTR(Exception):
    msgnum=0xfd380b0
    def __str__(self):
        return "No $VALUE is defined for signal or validation"

class TdiNOT_NUMBER(Exception):
    msgnum=0xfd380b8
    def __str__(self):
        return "Value is not a scalar number and must be"

class TdiNULL_PTR(Exception):
    msgnum=0xfd380c0
    def __str__(self):
        return "Null pointer where value needed"

class TdiRECURSIVE(Exception):
    msgnum=0xfd380c8
    def __str__(self):
        return "Overly recursive function, calls itself maybe"

class TdiSIG_DIM(Exception):
    msgnum=0xfd380d0
    def __str__(self):
        return "Signal dimension does not match data shape"

class TdiSYNTAX(Exception):
    msgnum=0xfd380d8
    def __str__(self):
        return "Bad punctuation or misspelled word or number"

class TdiTOO_BIG(Exception):
    msgnum=0xfd380e0
    def __str__(self):
        return "Conversion of number lost significant digits"

class TdiUNBALANCE(Exception):
    msgnum=0xfd380e8
    def __str__(self):
        return "Unbalanced () [] {} '' " " or /**/"

class TdiUNKNOWN_VAR(Exception):
    msgnum=0xfd380f0
    def __str__(self):
        return "Unknown/undefined variable name"

class TdiSTRTOOLON(Exception):
    msgnum=0xfd380f8
    def __str__(self):
        return "string is too long (greater than 65535)"

class TdiTIMEOUT(Exception):
    msgnum=0xfd38100
    def __str__(self):
        return "task did not complete in alotted time"

def MDSplusException(msgnum):
    edict={
0xfe18008:"ServerNOT_DISPATCHED",
0xfe18010:"ServerINVALID_DEPENDENCY",
0xfe18018:"ServerCANT_HAPPEN",
0xfe18020:"ServerINVSHOT",
0xfe18030:"ServerABORT",
0xfe18040:"ServerPATH_DOWN",
0x277c8008:"DEVBAD_ENDIDX",
0x277c8010:"DEVBAD_FILTER",
0x277c8018:"DEVBAD_FREQ",
0x277c8020:"DEVBAD_GAIN",
0x277c8028:"DEVBAD_HEADER",
0x277c8030:"DEVBAD_HEADER_IDX",
0x277c8038:"DEVBAD_MEMORIES",
0x277c8040:"DEVBAD_MODE",
0x277c8048:"DEVBAD_NAME",
0x277c8050:"DEVBAD_OFFSET",
0x277c8058:"DEVBAD_STARTIDX",
0x277c8060:"DEVNOT_TRIGGERED",
0x277c8068:"DEVFREQ_TO_HIGH",
0x277c8070:"DEVINVALID_NOC",
0x277c8078:"DEVRANGE_MISMATCH",
0x277c8080:"DEVCAMACERR",
0x277c8088:"DEVBAD_VERBS",
0x277c8090:"DEVBAD_COMMANDS",
0x277c8098:"DEVCAM_ADNR",
0x277c80a0:"DEVCAM_ERR",
0x277c80a8:"DEVCAM_LOSYNC",
0x277c80b0:"DEVCAM_LPE",
0x277c80b8:"DEVCAM_TMO",
0x277c80c0:"DEVCAM_TPE",
0x277c80c8:"DEVCAM_STE",
0x277c80d0:"DEVCAM_DERR",
0x277c80d8:"DEVCAM_SQ",
0x277c80e0:"DEVCAM_NOSQ",
0x277c80e8:"DEVCAM_SX",
0x277c80f0:"DEVCAM_NOSX",
0x277c80f8:"DEVINV_SETUP",
0x277c8198:"RETICONNORMAL",
0x277c81a0:"RETICONBAD_FRAMES",
0x277c81a8:"RETICONBAD_FRAME_SELECT",
0x277c81b0:"RETICONBAD_NUM_STATES",
0x277c81b8:"RETICONBAD_PERIOD",
0x277c81c0:"RETICONBAD_PIXEL_SELECT",
0x277c81c8:"RETICONDATA_CORRUPTED",
0x277c81d0:"RETICONTOO_MANY_FRAMES",
0x277c8328:"J221NORMAL",
0x277c8330:"J221INVALID_DATA",
0x277c8338:"J221NO_DATA",
0x277c84b8:"TIMINGINVCLKFRQ",
0x277c84c0:"TIMINGINVDELDUR",
0x277c84c8:"TIMINGINVOUTCTR",
0x277c84d0:"TIMINGINVPSEUDODEV",
0x277c84d8:"TIMINGINVTRGMOD",
0x277c84e0:"TIMINGNOPSEUDODEV",
0x277c84e8:"TIMINGTOO_MANY_EVENTS",
0x277c8648:"B2408NORMAL",
0x277c8650:"B2408OVERFLOW",
0x277c8658:"B2408TRIG_LIM",
0x277c87d8:"FERANORMAL",
0x277c87e0:"FERADIGNOTSTRARRAY",
0x277c87e8:"FERANODIG",
0x277c87f0:"FERAMEMNOTSTRARRAY",
0x277c87f8:"FERANOMEM",
0x277c8800:"FERAPHASE_LOST",
0x277c8808:"FERACONFUSED",
0x277c8810:"FERAOVER_RUN",
0x277c8818:"FERAOVERFLOW",
0x277c8968:"HM650NORMAL",
0x277c8970:"HM650DLYCHNG",
0x277c8af8:"HV4032NORMAL",
0x277c8b00:"HV4032WRONG_POD_TYPE",
0x277c8c88:"HV1440NORMAL",
0x277c8c90:"HV1440WRONG_POD_TYPE",
0x277c8c98:"HV1440BAD_FRAME",
0x277c8ca0:"HV1440BAD_RANGE",
0x277c8ca8:"HV1440OUTRNG",
0x277c8cb0:"HV1440STUCK",
0x277c8e18:"JOERGERBAD_PRE_TRIGGER",
0x277c8e20:"JOERGERBAD_ACT_MEMORY",
0x277c8e28:"JOERGERBAD_GAIN",
0x277c8fa8:"U_OF_MBAD_WAVE_LENGTH",
0x277c8fb0:"U_OF_MBAD_SLIT_WIDTH",
0x277c8fb8:"U_OF_MBAD_NUM_SPECTRA",
0x277c8fc0:"U_OF_MBAD_GRATING",
0x277c8fc8:"U_OF_MBAD_EXPOSURE",
0x277c8fd0:"U_OF_MBAD_FILTER",
0x277c8fd8:"U_OF_MGO_FILE_ERROR",
0x277c8fe0:"U_OF_MDATA_FILE_ERROR",
0x277c9138:"IDLNORMAL",
0x277c9140:"IDLERROR",
0x277c92c8:"B5910ABAD_CHAN",
0x277c92d0:"B5910ABAD_CLOCK",
0x277c92d8:"B5910ABAD_ITERATIONS",
0x277c92e0:"B5910ABAD_NOC",
0x277c92e8:"B5910ABAD_SAMPS",
0x277c9458:"J412NOT_SORTED",
0x277c9460:"J412NO_DATA",
0x277c9468:"J412BADCYCLES",
0x277c95e8:"TR16NORMAL",
0x277c95f0:"TR16BAD_MEMSIZE",
0x277c95f8:"TR16BAD_ACTIVEMEM",
0x277c9600:"TR16BAD_ACTIVECHAN",
0x277c9608:"TR16BAD_PTS",
0x277c9610:"TR16BAD_FREQUENCY",
0x277c9618:"TR16BAD_MASTER",
0x277c9620:"TR16BAD_GAIN",
0x277c9778:"A14NORMAL",
0x277c9780:"A14BAD_CLK_DIVIDE",
0x277c9788:"A14BAD_MODE",
0x277c9790:"A14BAD_CLK_POLARITY",
0x277c9798:"A14BAD_STR_POLARITY",
0x277c97a0:"A14BAD_STP_POLARITY",
0x277c97a8:"A14BAD_GATED",
0x277c9908:"L6810NORMAL",
0x277c9910:"L6810BAD_ACTIVECHAN",
0x277c9918:"L6810BAD_ACTIVEMEM",
0x277c9920:"L6810BAD_FREQUENCY",
0x277c9928:"L6810BAD_FULL_SCALE",
0x277c9930:"L6810BAD_MEMORIES",
0x277c9938:"L6810BAD_COUPLING",
0x277c9940:"L6810BAD_OFFSET",
0x277c9948:"L6810BAD_SEGMENTS",
0x277c9950:"L6810BAD_TRIG_DELAY",
0x277c9a98:"J_DACOUTRNG",
0x277c9c28:"INCAABAD_ACTIVE_CHANS",
0x277c9c30:"INCAABAD_MASTER",
0x277c9c38:"INCAABAD_EXT_1MHZ",
0x277c9c40:"INCAABAD_PTSC",
0x277c9db8:"L8212BAD_HEADER",
0x277c9dc0:"L8212BAD_MEMORIES",
0x277c9dc8:"L8212BAD_NOC",
0x277c9dd0:"L8212BAD_OFFSET",
0x277c9dd8:"L8212BAD_PTS",
0x277c9de0:"L8212FREQ_TO_HIGH",
0x277c9de8:"L8212INVALID_NOC",
0x277c9f48:"MPBBADTIME",
0x277c9f50:"MPBBADFREQ",
0x277ca0d8:"L8828BAD_OFFSET",
0x277ca0e0:"L8828BAD_PRETRIG",
0x277ca0e8:"L8828BAD_ACTIVEMEM",
0x277ca0f0:"L8828BAD_CLOCK",
0x277ca268:"L8818BAD_OFFSET",
0x277ca270:"L8818BAD_PRETRIG",
0x277ca278:"L8818BAD_ACTIVEMEM",
0x277ca280:"L8818BAD_CLOCK",
0x277ca2b8:"J_TR612BAD_ACTMEM",
0x277ca2c0:"J_TR612BAD_PRETRIG",
0x277ca2c8:"J_TR612BAD_MODE",
0x277ca2d0:"J_TR612BAD_FREQUENCY",
0x277ca3f8:"L8206NODATA",
0x277ca448:"H912BAD_CLOCK",
0x277ca450:"H912BAD_BLOCKS",
0x277ca458:"H912BAD_PTS",
0x277ca498:"H908BAD_CLOCK",
0x277ca4a0:"H908BAD_ACTIVE_CHANS",
0x277ca4a8:"H908BAD_PTS",
0x277ca4e8:"DSP2904CHANNEL_READ_ERROR",
0x277ca538:"PYUNHANDLED_EXCEPTION",
0x277ca588:"DT196BNO_SAMPLES",
0x277ca590:"ACQINITIALIZATION_ERROR",
0x277ca598:"ACQSETTINGS_NOT_LOADED",
0x277ca5a0:"ACQWRONG_TREE",
0x277ca5a8:"ACQWRONG_PATH",
0x277ca5b0:"ACQWRONG_SHOT",
0x158210:"LibINSVIRMEM",
0x158230:"LibINVARG",
0x158220:"LibINVSTRDES",
0x1582f8:"LibKEYNOTFOU",
0x158270:"LibNOTFOU",
0x1582e8:"LibQUEWASEMP",
0x158010:"LibSTRTRU",
0x248418:"StrMATCH",
0x248208:"StrNOMATCH",
0x248218:"StrNOELEM",
0x248210:"StrINVDELIM",
0x248070:"StrSTRTOOLON",
0x478:"SsINTOVF",
0x8018008:"CamDONE_Q",
0x8018010:"CamDONE_NOQ",
0x8019000:"CamDONE_NOX",
0x8019800:"CamSERTRAERR",
0x8019840:"CamSCCFAIL",
0x8019868:"CamOFFLINE",
0x2a0008:"TclNORMAL",
0x2a0010:"TclFAILED_ESSENTIAL",
0x8020008:"MdsdclSUCCESS",
0x8020010:"MdsdclEXIT",
0x8020018:"MdsdclERROR",
0x8020328:"MdsdclNORMAL",
0x8020330:"MdsdclPRESENT",
0x8020338:"MdsdclIVVERB",
0x8020340:"MdsdclABSENT",
0x8020348:"MdsdclNEGATED",
0x8020350:"MdsdclNOTNEGATABLE",
0x8020358:"MdsdclIVQUAL",
0x8020360:"MdsdclPROMPT_MORE",
0x8020368:"MdsdclTOO_MANY_PRMS",
0x8020370:"MdsdclTOO_MANY_VALS",
0x8020378:"MdsdclMISSING_VALUE",
0xfd18028:"TreeALREADY_OFF",
0xfd18030:"TreeALREADY_ON",
0xfd18038:"TreeALREADY_OPEN",
0xfd18088:"TreeALREADY_THERE",
0xfd180b8:"TreeBADRECORD",
0xfd18098:"TreeBOTH_OFF",
0xfd18110:"TreeBUFFEROVF",
0xfd18120:"TreeCONGLOMFULL",
0xfd18128:"TreeCONGLOM_NOT_FULL",
0xfd18960:"TreeCONTINUING",
0xfd180c8:"TreeDUPTAG",
0xfd18190:"TreeEDITTING",
0xfd18108:"TreeILLEGAL_ITEM",
0xfd180d0:"TreeILLPAGCNT",
0xfd18138:"TreeINVDFFCLASS",
0xfd18188:"TreeINVDTPUSG",
0xfd18100:"TreeINVPATH",
0xfd18140:"TreeINVRECTYP",
0xfd180c0:"TreeINVTREE",
0xfd180d8:"TreeMAXOPENEDIT",
0xfd18018:"TreeNEW",
0xfd18060:"TreeNMN",
0xfd18068:"TreeNMT",
0xfd18070:"TreeNNF",
0xfd180e0:"TreeNODATA",
0xfd18148:"TreeNODNAMLEN",
0xfd180f0:"TreeNOEDIT",
0xfd181a8:"TreeNOLOG",
0xfd180b0:"TreeNOMETHOD",
0xfd18180:"TreeNOOVERWRITE",
0xfd18008:"TreeNORMAL",
0xfd18020:"TreeNOTALLSUBS",
0xfd180f8:"TreeNOTCHILDLESS",
0xfd181c0:"TreeNOT_IN_LIST",
0xfd18170:"TreeNOTMEMBERLESS",
0xfd180e8:"TreeNOTOPEN",
0xfd18178:"TreeNOTSON",
0xfd18160:"TreeNOT_CONGLOM",
0xfd180a8:"TreeNOT_OPEN",
0xfd18198:"TreeNOWRITEMODEL",
0xfd181a0:"TreeNOWRITESHOT",
0xfd18040:"TreeNO_CONTEXT",
0xfd180a0:"TreeOFF",
0xfd18048:"TreeON",
0xfd18050:"TreeOPEN",
0xfd18058:"TreeOPEN_EDIT",
0xfd18090:"TreePARENT_OFF",
0xfd181b8:"TreeREADERR",
0xfd181b0:"TreeREADONLY",
0xfd18010:"TreeRESOLVED",
0xfd18640:"TreeSUCCESS",
0xfd18150:"TreeTAGNAMLEN",
0xfd18078:"TreeTNF",
0xfd18080:"TreeTREENF",
0xfd18130:"TreeUNRESOLVED",
0xfd18118:"TreeUNSPRTCLASS",
0xfd18168:"TreeUNSUPARRDTYPE",
0xfd18158:"TreeWRITEFIRST",
0xfd18fa0:"TreeFAILURE",
0xfd18fb0:"TreeLOCK_FAILURE",
0xfd18fa8:"TreeFILE_NOT_FOUND",
0xfd18c80:"TreeCANCEL",
0xfd18fb8:"TreeNOSEGMENTS",
0xfd18fc0:"TreeINVDTYPE",
0xfd18fc8:"TreeINVSHAPE",
0xfd18fd8:"TreeINVSHOT",
0xfd18fe8:"TreeINVTAG",
0xfd18ff0:"TreeNOPATH",
0xfd18ff8:"TreeTREEFILEREADERR",
0xfd19000:"TreeMEMERR",
0xfd19008:"TreeNOCURRENT",
0xfd19010:"TreeFOPENW",
0xfd19018:"TreeFOPENR",
0xfd19020:"TreeFCREATE",
0xfd19028:"TreeCONNECTFAIL",
0xfd19030:"TreeNCIWRITE",
0xfd19038:"TreeDELFAIL",
0xfd19040:"TreeRENFAIL",
0xfd19048:"TreeEMPTY",
0xfd19050:"TreePARSEERR",
0xfd19058:"TreeNCIREAD",
0xfd19060:"TreeNOVERSION",
0xfd19068:"TreeDFREAD",
0xfd38008:"TdiBREAK",
0xfd38010:"TdiCASE",
0xfd38018:"TdiCONTINUE",
0xfd38020:"TdiEXTRANEOUS",
0xfd38028:"TdiRETURN",
0xfd38030:"TdiABORT",
0xfd38038:"TdiBAD_INDEX",
0xfd38040:"TdiBOMB",
0xfd38048:"TdiEXTRA_ARG",
0xfd38050:"TdiGOTO",
0xfd38058:"TdiINVCLADSC",
0xfd38060:"TdiINVCLADTY",
0xfd38068:"TdiINVDTYDSC",
0xfd38070:"TdiINV_OPC",
0xfd38078:"TdiINV_SIZE",
0xfd38080:"TdiMISMATCH",
0xfd38088:"TdiMISS_ARG",
0xfd38090:"TdiNDIM_OVER",
0xfd38098:"TdiNO_CMPLX",
0xfd380a0:"TdiNO_OPC",
0xfd380a8:"TdiNO_OUTPTR",
0xfd380b0:"TdiNO_SELF_PTR",
0xfd380b8:"TdiNOT_NUMBER",
0xfd380c0:"TdiNULL_PTR",
0xfd380c8:"TdiRECURSIVE",
0xfd380d0:"TdiSIG_DIM",
0xfd380d8:"TdiSYNTAX",
0xfd380e0:"TdiTOO_BIG",
0xfd380e8:"TdiUNBALANCE",
0xfd380f0:"TdiUNKNOWN_VAR",
0xfd380f8:"TdiSTRTOOLON",
0xfd38100:"TdiTIMEOUT",
}
    try:
      return globals()[edict[msgnum]]()
    except:
      return Exception("Unknown status return - %s" % hex(msgnum))
