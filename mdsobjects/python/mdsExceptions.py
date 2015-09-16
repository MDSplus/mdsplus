
_severity=["W", "S", "E", "I", "F", "?", "?", "?"]


class ServerException(Exception):
      pass

class DEVException(Exception):
      pass

class RETICONException(Exception):
      pass

class J221Exception(Exception):
      pass

class TIMINGException(Exception):
      pass

class B2408Exception(Exception):
      pass

class FERAException(Exception):
      pass

class HM650Exception(Exception):
      pass

class HV4032Exception(Exception):
      pass

class HV1440Exception(Exception):
      pass

class JOERGERException(Exception):
      pass

class U_OF_MException(Exception):
      pass

class IDLException(Exception):
      pass

class B5910AException(Exception):
      pass

class J412Exception(Exception):
      pass

class TR16Exception(Exception):
      pass

class A14Exception(Exception):
      pass

class L6810Exception(Exception):
      pass

class J_DACException(Exception):
      pass

class INCAAException(Exception):
      pass

class L8212Exception(Exception):
      pass

class MPBException(Exception):
      pass

class L8828Exception(Exception):
      pass

class L8818Exception(Exception):
      pass

class J_TR612Exception(Exception):
      pass

class L8206Exception(Exception):
      pass

class H912Exception(Exception):
      pass

class H908Exception(Exception):
      pass

class DSP2904Exception(Exception):
      pass

class PYException(Exception):
      pass

class DT196BException(Exception):
      pass

class ACQException(Exception):
      pass

class LibException(Exception):
      pass

class StrException(Exception):
      pass

class SsException(Exception):
      pass

class CamException(Exception):
      pass

class TclException(Exception):
      pass

class MdsdclException(Exception):
      pass

class TreeException(Exception):
      pass

class TdiException(Exception):
      pass

class ServerNOT_DISPATCHED(ServerException):
    status=0xfe18008
    facility="Server"
    message="action not dispatched, depended on failed action"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-NOT_DISPATCHED, action not dispatched, depended on failed action" % self.severity

class ServerINVALID_DEPENDENCY(ServerException):
    status=0xfe18010
    facility="Server"
    message="action dependency cannot be evaluated"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-INVALID_DEPENDENCY, action dependency cannot be evaluated" % self.severity

class ServerCANT_HAPPEN(ServerException):
    status=0xfe18018
    facility="Server"
    message="action contains circular dependency or depends on action which was not dispatched"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-CANT_HAPPEN, action contains circular dependency or depends on action which was not dispatched" % self.severity

class ServerINVSHOT(ServerException):
    status=0xfe18020
    facility="Server"
    message="invalid shot number, cannot dispatch actions in model"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-INVSHOT, invalid shot number, cannot dispatch actions in model" % self.severity

class ServerABORT(ServerException):
    status=0xfe18030
    facility="Server"
    message="Server action was aborted"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-ABORT, Server action was aborted" % self.severity

class ServerPATH_DOWN(ServerException):
    status=0xfe18040
    facility="Server"
    message="Path to server lost"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SERVER-%s-PATH_DOWN, Path to server lost" % self.severity

class DEVBAD_ENDIDX(DEVException):
    status=0x277c8008
    facility="DEV"
    message="unable to read end index for channel"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_ENDIDX, unable to read end index for channel" % self.severity

class DEVBAD_FILTER(DEVException):
    status=0x277c8010
    facility="DEV"
    message="illegal filter selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_FILTER, illegal filter selected" % self.severity

class DEVBAD_FREQ(DEVException):
    status=0x277c8018
    facility="DEV"
    message="illegal digitization frequency selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_FREQ, illegal digitization frequency selected" % self.severity

class DEVBAD_GAIN(DEVException):
    status=0x277c8020
    facility="DEV"
    message="illegal gain selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_GAIN, illegal gain selected" % self.severity

class DEVBAD_HEADER(DEVException):
    status=0x277c8028
    facility="DEV"
    message="unable to read header selection"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_HEADER, unable to read header selection" % self.severity

class DEVBAD_HEADER_IDX(DEVException):
    status=0x277c8030
    facility="DEV"
    message="unknown header configuration index"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_HEADER_IDX, unknown header configuration index" % self.severity

class DEVBAD_MEMORIES(DEVException):
    status=0x277c8038
    facility="DEV"
    message="unable to read number of memory modules"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_MEMORIES, unable to read number of memory modules" % self.severity

class DEVBAD_MODE(DEVException):
    status=0x277c8040
    facility="DEV"
    message="illegal mode selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_MODE, illegal mode selected" % self.severity

class DEVBAD_NAME(DEVException):
    status=0x277c8048
    facility="DEV"
    message="unable to read module name"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_NAME, unable to read module name" % self.severity

class DEVBAD_OFFSET(DEVException):
    status=0x277c8050
    facility="DEV"
    message="illegal offset selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_OFFSET, illegal offset selected" % self.severity

class DEVBAD_STARTIDX(DEVException):
    status=0x277c8058
    facility="DEV"
    message="unable to read start index for channel"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_STARTIDX, unable to read start index for channel" % self.severity

class DEVNOT_TRIGGERED(DEVException):
    status=0x277c8060
    facility="DEV"
    message="device was not triggered,  check wires and triggering device"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-NOT_TRIGGERED, device was not triggered,  check wires and triggering device" % self.severity

class DEVFREQ_TO_HIGH(DEVException):
    status=0x277c8068
    facility="DEV"
    message="the frequency is set to high for the requested number of channels"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-FREQ_TO_HIGH, the frequency is set to high for the requested number of channels" % self.severity

class DEVINVALID_NOC(DEVException):
    status=0x277c8070
    facility="DEV"
    message="the NOC (number of channels) requested is greater than the physical number of channels"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-INVALID_NOC, the NOC (number of channels) requested is greater than the physical number of channels" % self.severity

class DEVRANGE_MISMATCH(DEVException):
    status=0x277c8078
    facility="DEV"
    message="the range specified on the menu doesn't match the range setting on the device"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-RANGE_MISMATCH, the range specified on the menu doesn't match the range setting on the device" % self.severity

class DEVCAMACERR(DEVException):
    status=0x277c8080
    facility="DEV"
    message="Error doing CAMAC IO"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAMACERR, Error doing CAMAC IO" % self.severity

class DEVBAD_VERBS(DEVException):
    status=0x277c8088
    facility="DEV"
    message="Error reading interpreter list (:VERBS)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_VERBS, Error reading interpreter list (:VERBS)" % self.severity

class DEVBAD_COMMANDS(DEVException):
    status=0x277c8090
    facility="DEV"
    message="Error reading command list"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-BAD_COMMANDS, Error reading command list" % self.severity

class DEVCAM_ADNR(DEVException):
    status=0x277c8098
    facility="DEV"
    message="CAMAC: Address not recognized (2160)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_ADNR, CAMAC: Address not recognized (2160)" % self.severity

class DEVCAM_ERR(DEVException):
    status=0x277c80a0
    facility="DEV"
    message="CAMAC: Error reported by crate controler"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_ERR, CAMAC: Error reported by crate controler" % self.severity

class DEVCAM_LOSYNC(DEVException):
    status=0x277c80a8
    facility="DEV"
    message="CAMAC: Lost Syncronization error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_LOSYNC, CAMAC: Lost Syncronization error" % self.severity

class DEVCAM_LPE(DEVException):
    status=0x277c80b0
    facility="DEV"
    message="CAMAC: Longitudinal Parity error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_LPE, CAMAC: Longitudinal Parity error" % self.severity

class DEVCAM_TMO(DEVException):
    status=0x277c80b8
    facility="DEV"
    message="CAMAC: Highway time out error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_TMO, CAMAC: Highway time out error" % self.severity

class DEVCAM_TPE(DEVException):
    status=0x277c80c0
    facility="DEV"
    message="CAMAC: Transverse Parity error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_TPE, CAMAC: Transverse Parity error" % self.severity

class DEVCAM_STE(DEVException):
    status=0x277c80c8
    facility="DEV"
    message="CAMAC: Serial Transmission error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_STE, CAMAC: Serial Transmission error" % self.severity

class DEVCAM_DERR(DEVException):
    status=0x277c80d0
    facility="DEV"
    message="CAMAC: Delayed error from SCC"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_DERR, CAMAC: Delayed error from SCC" % self.severity

class DEVCAM_SQ(DEVException):
    status=0x277c80d8
    facility="DEV"
    message="CAMAC: I/O completion with Q = 1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_SQ, CAMAC: I/O completion with Q = 1" % self.severity

class DEVCAM_NOSQ(DEVException):
    status=0x277c80e0
    facility="DEV"
    message="CAMAC: I/O completion with Q = 0"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_NOSQ, CAMAC: I/O completion with Q = 0" % self.severity

class DEVCAM_SX(DEVException):
    status=0x277c80e8
    facility="DEV"
    message="CAMAC: I/O completion with X = 1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_SX, CAMAC: I/O completion with X = 1" % self.severity

class DEVCAM_NOSX(DEVException):
    status=0x277c80f0
    facility="DEV"
    message="CAMAC: I/O completion with X = 0"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-CAM_NOSX, CAMAC: I/O completion with X = 0" % self.severity

class DEVINV_SETUP(DEVException):
    status=0x277c80f8
    facility="DEV"
    message="device was not properly set up"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DEV-%s-INV_SETUP, device was not properly set up" % self.severity

class RETICONNORMAL(RETICONException):
    status=0x277c8198
    facility="RETICON"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-NORMAL, successful completion" % self.severity

class RETICONBAD_FRAMES(RETICONException):
    status=0x277c81a0
    facility="RETICON"
    message="frame count must be less than or equal to 2048"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-BAD_FRAMES, frame count must be less than or equal to 2048" % self.severity

class RETICONBAD_FRAME_SELECT(RETICONException):
    status=0x277c81a8
    facility="RETICON"
    message="frame interval must be 1,2,4,8,16,32 or 64"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-BAD_FRAME_SELECT, frame interval must be 1,2,4,8,16,32 or 64" % self.severity

class RETICONBAD_NUM_STATES(RETICONException):
    status=0x277c81b0
    facility="RETICON"
    message="number of states must be between 1 and 4"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-BAD_NUM_STATES, number of states must be between 1 and 4" % self.severity

class RETICONBAD_PERIOD(RETICONException):
    status=0x277c81b8
    facility="RETICON"
    message="period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-BAD_PERIOD, period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec" % self.severity

class RETICONBAD_PIXEL_SELECT(RETICONException):
    status=0x277c81c0
    facility="RETICON"
    message="pixel selection must be an array of 256 boolean values"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-BAD_PIXEL_SELECT, pixel selection must be an array of 256 boolean values" % self.severity

class RETICONDATA_CORRUPTED(RETICONException):
    status=0x277c81c8
    facility="RETICON"
    message="data in memory is corrupted or framing error detected, no data stored"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-DATA_CORRUPTED, data in memory is corrupted or framing error detected, no data stored" % self.severity

class RETICONTOO_MANY_FRAMES(RETICONException):
    status=0x277c81d0
    facility="RETICON"
    message="over 8192 frame start indicators in data read from memory"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%RETICON-%s-TOO_MANY_FRAMES, over 8192 frame start indicators in data read from memory" % self.severity

class J221NORMAL(J221Exception):
    status=0x277c8328
    facility="J221"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J221-%s-NORMAL, successful completion" % self.severity

class J221INVALID_DATA(J221Exception):
    status=0x277c8330
    facility="J221"
    message="ignoring invalid data in channel !SL"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J221-%s-INVALID_DATA, ignoring invalid data in channel !SL" % self.severity

class J221NO_DATA(J221Exception):
    status=0x277c8338
    facility="J221"
    message="no valid data was found for any channel"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J221-%s-NO_DATA, no valid data was found for any channel" % self.severity

class TIMINGINVCLKFRQ(TIMINGException):
    status=0x277c84b8
    facility="TIMING"
    message="Invalid clock frequency"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-INVCLKFRQ, Invalid clock frequency" % self.severity

class TIMINGINVDELDUR(TIMINGException):
    status=0x277c84c0
    facility="TIMING"
    message="Invalid pulse delay or duration, must be less than 655 seconds"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-INVDELDUR, Invalid pulse delay or duration, must be less than 655 seconds" % self.severity

class TIMINGINVOUTCTR(TIMINGException):
    status=0x277c84c8
    facility="TIMING"
    message="Invalid output mode selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-INVOUTCTR, Invalid output mode selected" % self.severity

class TIMINGINVPSEUDODEV(TIMINGException):
    status=0x277c84d0
    facility="TIMING"
    message="Invalid pseudo device attached to this decoder channel"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-INVPSEUDODEV, Invalid pseudo device attached to this decoder channel" % self.severity

class TIMINGINVTRGMOD(TIMINGException):
    status=0x277c84d8
    facility="TIMING"
    message="Invalid trigger mode selected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-INVTRGMOD, Invalid trigger mode selected" % self.severity

class TIMINGNOPSEUDODEV(TIMINGException):
    status=0x277c84e0
    facility="TIMING"
    message="No Pseudo device attached to this channel ... disabling"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-NOPSEUDODEV, No Pseudo device attached to this channel ... disabling" % self.severity

class TIMINGTOO_MANY_EVENTS(TIMINGException):
    status=0x277c84e8
    facility="TIMING"
    message="More than 16 events used by this decoder"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TIMING-%s-TOO_MANY_EVENTS, More than 16 events used by this decoder" % self.severity

class B2408NORMAL(B2408Exception):
    status=0x277c8648
    facility="B2408"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B2408-%s-NORMAL, successful completion" % self.severity

class B2408OVERFLOW(B2408Exception):
    status=0x277c8650
    facility="B2408"
    message="Triggers received after overflow"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B2408-%s-OVERFLOW, Triggers received after overflow" % self.severity

class B2408TRIG_LIM(B2408Exception):
    status=0x277c8658
    facility="B2408"
    message="Trigger limit possibly exceeded"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B2408-%s-TRIG_LIM, Trigger limit possibly exceeded" % self.severity

class FERANORMAL(FERAException):
    status=0x277c87d8
    facility="FERA"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-NORMAL, successful completion" % self.severity

class FERADIGNOTSTRARRAY(FERAException):
    status=0x277c87e0
    facility="FERA"
    message="The digitizer names must be an array of strings"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-DIGNOTSTRARRAY, The digitizer names must be an array of strings" % self.severity

class FERANODIG(FERAException):
    status=0x277c87e8
    facility="FERA"
    message="The digitizer names must be specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-NODIG, The digitizer names must be specified" % self.severity

class FERAMEMNOTSTRARRAY(FERAException):
    status=0x277c87f0
    facility="FERA"
    message="The memory names must be an array of strings"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-MEMNOTSTRARRAY, The memory names must be an array of strings" % self.severity

class FERANOMEM(FERAException):
    status=0x277c87f8
    facility="FERA"
    message="The memory names must be specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-NOMEM, The memory names must be specified" % self.severity

class FERAPHASE_LOST(FERAException):
    status=0x277c8800
    facility="FERA"
    message="Data phase lost No FERA data stored"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-PHASE_LOST, Data phase lost No FERA data stored" % self.severity

class FERACONFUSED(FERAException):
    status=0x277c8808
    facility="FERA"
    message="Fera Data inconsitant.  Data for this point zered."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-CONFUSED, Fera Data inconsitant.  Data for this point zered." % self.severity

class FERAOVER_RUN(FERAException):
    status=0x277c8810
    facility="FERA"
    message="Possible FERA memory overrun, too many triggers."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-OVER_RUN, Possible FERA memory overrun, too many triggers." % self.severity

class FERAOVERFLOW(FERAException):
    status=0x277c8818
    facility="FERA"
    message="Possible FERA data saturated.  Data point zeroed"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%FERA-%s-OVERFLOW, Possible FERA data saturated.  Data point zeroed" % self.severity

class HM650NORMAL(HM650Exception):
    status=0x277c8968
    facility="HM650"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HM650-%s-NORMAL, successful completion" % self.severity

class HM650DLYCHNG(HM650Exception):
    status=0x277c8970
    facility="HM650"
    message="HM650 requested delay can not be processed by hardware."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HM650-%s-DLYCHNG, HM650 requested delay can not be processed by hardware." % self.severity

class HV4032NORMAL(HV4032Exception):
    status=0x277c8af8
    facility="HV4032"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV4032-%s-NORMAL, successful completion" % self.severity

class HV4032WRONG_POD_TYPE(HV4032Exception):
    status=0x277c8b00
    facility="HV4032"
    message="HV40321A n and p can only be used with the HV4032 device"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV4032-%s-WRONG_POD_TYPE, HV40321A n and p can only be used with the HV4032 device" % self.severity

class HV1440NORMAL(HV1440Exception):
    status=0x277c8c88
    facility="HV1440"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-NORMAL, successful completion" % self.severity

class HV1440WRONG_POD_TYPE(HV1440Exception):
    status=0x277c8c90
    facility="HV1440"
    message="HV1443 can only be used with the HV1440 device"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-WRONG_POD_TYPE, HV1443 can only be used with the HV1440 device" % self.severity

class HV1440BAD_FRAME(HV1440Exception):
    status=0x277c8c98
    facility="HV1440"
    message="HV1440 could not read the frame"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-BAD_FRAME, HV1440 could not read the frame" % self.severity

class HV1440BAD_RANGE(HV1440Exception):
    status=0x277c8ca0
    facility="HV1440"
    message="HV1440 could not read the range"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-BAD_RANGE, HV1440 could not read the range" % self.severity

class HV1440OUTRNG(HV1440Exception):
    status=0x277c8ca8
    facility="HV1440"
    message="HV1440 out of range"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-OUTRNG, HV1440 out of range" % self.severity

class HV1440STUCK(HV1440Exception):
    status=0x277c8cb0
    facility="HV1440"
    message="HV1440 not responding with Q"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%HV1440-%s-STUCK, HV1440 not responding with Q" % self.severity

class JOERGERBAD_PRE_TRIGGER(JOERGERException):
    status=0x277c8e18
    facility="JOERGER"
    message="bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%JOERGER-%s-BAD_PRE_TRIGGER, bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7" % self.severity

class JOERGERBAD_ACT_MEMORY(JOERGERException):
    status=0x277c8e20
    facility="JOERGER"
    message="bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%JOERGER-%s-BAD_ACT_MEMORY, bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8" % self.severity

class JOERGERBAD_GAIN(JOERGERException):
    status=0x277c8e28
    facility="JOERGER"
    message="bad gain specified, specify a value of 1,2,4 or 8"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%JOERGER-%s-BAD_GAIN, bad gain specified, specify a value of 1,2,4 or 8" % self.severity

class U_OF_MBAD_WAVE_LENGTH(U_OF_MException):
    status=0x277c8fa8
    facility="U_OF_M"
    message="bad wave length specified, specify value between 0 and 13000"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_WAVE_LENGTH, bad wave length specified, specify value between 0 and 13000" % self.severity

class U_OF_MBAD_SLIT_WIDTH(U_OF_MException):
    status=0x277c8fb0
    facility="U_OF_M"
    message="bad slit width specified, specify value between 0 and 500"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_SLIT_WIDTH, bad slit width specified, specify value between 0 and 500" % self.severity

class U_OF_MBAD_NUM_SPECTRA(U_OF_MException):
    status=0x277c8fb8
    facility="U_OF_M"
    message="bad number of spectra specified, specify value between 1 and 100"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_NUM_SPECTRA, bad number of spectra specified, specify value between 1 and 100" % self.severity

class U_OF_MBAD_GRATING(U_OF_MException):
    status=0x277c8fc0
    facility="U_OF_M"
    message="bad grating type specified, specify value between 1 and 5"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_GRATING, bad grating type specified, specify value between 1 and 5" % self.severity

class U_OF_MBAD_EXPOSURE(U_OF_MException):
    status=0x277c8fc8
    facility="U_OF_M"
    message="bad exposure time specified, specify value between 30 and 3000"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_EXPOSURE, bad exposure time specified, specify value between 30 and 3000" % self.severity

class U_OF_MBAD_FILTER(U_OF_MException):
    status=0x277c8fd0
    facility="U_OF_M"
    message="bad neutral density filter specified, specify value between 0 and 5"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-BAD_FILTER, bad neutral density filter specified, specify value between 0 and 5" % self.severity

class U_OF_MGO_FILE_ERROR(U_OF_MException):
    status=0x277c8fd8
    facility="U_OF_M"
    message="error creating new go file"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-GO_FILE_ERROR, error creating new go file" % self.severity

class U_OF_MDATA_FILE_ERROR(U_OF_MException):
    status=0x277c8fe0
    facility="U_OF_M"
    message="error opening datafile"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%U_OF_M-%s-DATA_FILE_ERROR, error opening datafile" % self.severity

class IDLNORMAL(IDLException):
    status=0x277c9138
    facility="IDL"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%IDL-%s-NORMAL, successful completion" % self.severity

class IDLERROR(IDLException):
    status=0x277c9140
    facility="IDL"
    message="IDL returned a non zero error code"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%IDL-%s-ERROR, IDL returned a non zero error code" % self.severity

class B5910ABAD_CHAN(B5910AException):
    status=0x277c92c8
    facility="B5910A"
    message="error evaluating data for channel !SL"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B5910A-%s-BAD_CHAN, error evaluating data for channel !SL" % self.severity

class B5910ABAD_CLOCK(B5910AException):
    status=0x277c92d0
    facility="B5910A"
    message="invalid internal clock range specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B5910A-%s-BAD_CLOCK, invalid internal clock range specified" % self.severity

class B5910ABAD_ITERATIONS(B5910AException):
    status=0x277c92d8
    facility="B5910A"
    message="invalid number of iterations specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B5910A-%s-BAD_ITERATIONS, invalid number of iterations specified" % self.severity

class B5910ABAD_NOC(B5910AException):
    status=0x277c92e0
    facility="B5910A"
    message="invalid number of active channels specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B5910A-%s-BAD_NOC, invalid number of active channels specified" % self.severity

class B5910ABAD_SAMPS(B5910AException):
    status=0x277c92e8
    facility="B5910A"
    message="number of samples specificed invalid"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%B5910A-%s-BAD_SAMPS, number of samples specificed invalid" % self.severity

class J412NOT_SORTED(J412Exception):
    status=0x277c9458
    facility="J412"
    message="times specified for J412 module were not sorted"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J412-%s-NOT_SORTED, times specified for J412 module were not sorted" % self.severity

class J412NO_DATA(J412Exception):
    status=0x277c9460
    facility="J412"
    message="there were no times specifed for J412 module"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J412-%s-NO_DATA, there were no times specifed for J412 module" % self.severity

class J412BADCYCLES(J412Exception):
    status=0x277c9468
    facility="J412"
    message="The number of cycles must be 1 .. 255"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J412-%s-BADCYCLES, The number of cycles must be 1 .. 255" % self.severity

class TR16NORMAL(TR16Exception):
    status=0x277c95e8
    facility="TR16"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-NORMAL, successful completion" % self.severity

class TR16BAD_MEMSIZE(TR16Exception):
    status=0x277c95f0
    facility="TR16"
    message="Memory size must be in 128K, 256K, 512k, 1024K"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_MEMSIZE, Memory size must be in 128K, 256K, 512k, 1024K" % self.severity

class TR16BAD_ACTIVEMEM(TR16Exception):
    status=0x277c95f8
    facility="TR16"
    message="Active Mem must be power of 2 8K to 1024K"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_ACTIVEMEM, Active Mem must be power of 2 8K to 1024K" % self.severity

class TR16BAD_ACTIVECHAN(TR16Exception):
    status=0x277c9600
    facility="TR16"
    message="Active channels must be in 1,2,4,8,16"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_ACTIVECHAN, Active channels must be in 1,2,4,8,16" % self.severity

class TR16BAD_PTS(TR16Exception):
    status=0x277c9608
    facility="TR16"
    message="PTS must be power of 2 32 to 1024K"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_PTS, PTS must be power of 2 32 to 1024K" % self.severity

class TR16BAD_FREQUENCY(TR16Exception):
    status=0x277c9610
    facility="TR16"
    message="Invalid clock frequency"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_FREQUENCY, Invalid clock frequency" % self.severity

class TR16BAD_MASTER(TR16Exception):
    status=0x277c9618
    facility="TR16"
    message="Master must be 0 or 1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_MASTER, Master must be 0 or 1" % self.severity

class TR16BAD_GAIN(TR16Exception):
    status=0x277c9620
    facility="TR16"
    message="Gain must be 1, 2, 4, or 8"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TR16-%s-BAD_GAIN, Gain must be 1, 2, 4, or 8" % self.severity

class A14NORMAL(A14Exception):
    status=0x277c9778
    facility="A14"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-NORMAL, successful completion" % self.severity

class A14BAD_CLK_DIVIDE(A14Exception):
    status=0x277c9780
    facility="A14"
    message="Clock divide must be one of 1,2,4,10,20,40, or 100"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_CLK_DIVIDE, Clock divide must be one of 1,2,4,10,20,40, or 100" % self.severity

class A14BAD_MODE(A14Exception):
    status=0x277c9788
    facility="A14"
    message="Mode must be in the range of 0 to 4"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_MODE, Mode must be in the range of 0 to 4" % self.severity

class A14BAD_CLK_POLARITY(A14Exception):
    status=0x277c9790
    facility="A14"
    message="Clock polarity must be either 0 (rising) or 1 (falling)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_CLK_POLARITY, Clock polarity must be either 0 (rising) or 1 (falling)" % self.severity

class A14BAD_STR_POLARITY(A14Exception):
    status=0x277c9798
    facility="A14"
    message="Start polarity must be either 0 (rising) or 1 (falling)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_STR_POLARITY, Start polarity must be either 0 (rising) or 1 (falling)" % self.severity

class A14BAD_STP_POLARITY(A14Exception):
    status=0x277c97a0
    facility="A14"
    message="Stop polarity must be either 0 (rising) or 1 (falling)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_STP_POLARITY, Stop polarity must be either 0 (rising) or 1 (falling)" % self.severity

class A14BAD_GATED(A14Exception):
    status=0x277c97a8
    facility="A14"
    message="Gated clock must be either 0 (not gated) or 1 (gated)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%A14-%s-BAD_GATED, Gated clock must be either 0 (not gated) or 1 (gated)" % self.severity

class L6810NORMAL(L6810Exception):
    status=0x277c9908
    facility="L6810"
    message="successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-NORMAL, successful completion" % self.severity

class L6810BAD_ACTIVECHAN(L6810Exception):
    status=0x277c9910
    facility="L6810"
    message="Active chans must be 1, 2, or 4"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_ACTIVECHAN, Active chans must be 1, 2, or 4" % self.severity

class L6810BAD_ACTIVEMEM(L6810Exception):
    status=0x277c9918
    facility="L6810"
    message="Active memory must be power of 2 LE 8192"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_ACTIVEMEM, Active memory must be power of 2 LE 8192" % self.severity

class L6810BAD_FREQUENCY(L6810Exception):
    status=0x277c9920
    facility="L6810"
    message="Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_FREQUENCY, Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]" % self.severity

class L6810BAD_FULL_SCALE(L6810Exception):
    status=0x277c9928
    facility="L6810"
    message="Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_FULL_SCALE, Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]" % self.severity

class L6810BAD_MEMORIES(L6810Exception):
    status=0x277c9930
    facility="L6810"
    message="Memories must 1 .. 16"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_MEMORIES, Memories must 1 .. 16" % self.severity

class L6810BAD_COUPLING(L6810Exception):
    status=0x277c9938
    facility="L6810"
    message="Channel source / coupling must be one of 0 .. 7"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_COUPLING, Channel source / coupling must be one of 0 .. 7" % self.severity

class L6810BAD_OFFSET(L6810Exception):
    status=0x277c9940
    facility="L6810"
    message="Offset must be between 0 and 255"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_OFFSET, Offset must be between 0 and 255" % self.severity

class L6810BAD_SEGMENTS(L6810Exception):
    status=0x277c9948
    facility="L6810"
    message="Number of segments must be 1 .. 1024"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_SEGMENTS, Number of segments must be 1 .. 1024" % self.severity

class L6810BAD_TRIG_DELAY(L6810Exception):
    status=0x277c9950
    facility="L6810"
    message="Trigger delay must be between -8 and 247 in units of 8ths of segment size"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L6810-%s-BAD_TRIG_DELAY, Trigger delay must be between -8 and 247 in units of 8ths of segment size" % self.severity

class J_DACOUTRNG(J_DACException):
    status=0x277c9a98
    facility="J_DAC"
    message="Joerger DAC Channels out of range.  Bad chans code !XW"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J_DAC-%s-OUTRNG, Joerger DAC Channels out of range.  Bad chans code !XW" % self.severity

class INCAABAD_ACTIVE_CHANS(INCAAException):
    status=0x277c9c28
    facility="INCAA"
    message="bad active channels selection"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%INCAA-%s-BAD_ACTIVE_CHANS, bad active channels selection" % self.severity

class INCAABAD_MASTER(INCAAException):
    status=0x277c9c30
    facility="INCAA"
    message="bad master selection, must be 0 or 1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%INCAA-%s-BAD_MASTER, bad master selection, must be 0 or 1" % self.severity

class INCAABAD_EXT_1MHZ(INCAAException):
    status=0x277c9c38
    facility="INCAA"
    message="bad ext 1mhz selection, must be 0 or 1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%INCAA-%s-BAD_EXT_1MHZ, bad ext 1mhz selection, must be 0 or 1" % self.severity

class INCAABAD_PTSC(INCAAException):
    status=0x277c9c40
    facility="INCAA"
    message="bad PTSC setting"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%INCAA-%s-BAD_PTSC, bad PTSC setting" % self.severity

class L8212BAD_HEADER(L8212Exception):
    status=0x277c9db8
    facility="L8212"
    message="Invalid header jumper information (e.g. 49414944432)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-BAD_HEADER, Invalid header jumper information (e.g. 49414944432)" % self.severity

class L8212BAD_MEMORIES(L8212Exception):
    status=0x277c9dc0
    facility="L8212"
    message="Invalid number of memories, must be 1 .. 16"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-BAD_MEMORIES, Invalid number of memories, must be 1 .. 16" % self.severity

class L8212BAD_NOC(L8212Exception):
    status=0x277c9dc8
    facility="L8212"
    message="Invalid number of active channels"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-BAD_NOC, Invalid number of active channels" % self.severity

class L8212BAD_OFFSET(L8212Exception):
    status=0x277c9dd0
    facility="L8212"
    message="Invalid offset must be one of (0, -2048, -4096)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-BAD_OFFSET, Invalid offset must be one of (0, -2048, -4096)" % self.severity

class L8212BAD_PTS(L8212Exception):
    status=0x277c9dd8
    facility="L8212"
    message="Invalid pts code, must be 0 .. 7"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-BAD_PTS, Invalid pts code, must be 0 .. 7" % self.severity

class L8212FREQ_TO_HIGH(L8212Exception):
    status=0x277c9de0
    facility="L8212"
    message="Frequency to high for selected number of channels"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-FREQ_TO_HIGH, Frequency to high for selected number of channels" % self.severity

class L8212INVALID_NOC(L8212Exception):
    status=0x277c9de8
    facility="L8212"
    message="Invalid number of active channels"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8212-%s-INVALID_NOC, Invalid number of active channels" % self.severity

class MPBBADTIME(MPBException):
    status=0x277c9f48
    facility="MPB"
    message="Could not read time"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MPB-%s-BADTIME, Could not read time" % self.severity

class MPBBADFREQ(MPBException):
    status=0x277c9f50
    facility="MPB"
    message="Could not read frequency"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MPB-%s-BADFREQ, Could not read frequency" % self.severity

class L8828BAD_OFFSET(L8828Exception):
    status=0x277ca0d8
    facility="L8828"
    message="Offset for L8828 must be between 0 and 255"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8828-%s-BAD_OFFSET, Offset for L8828 must be between 0 and 255" % self.severity

class L8828BAD_PRETRIG(L8828Exception):
    status=0x277ca0e0
    facility="L8828"
    message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8828-%s-BAD_PRETRIG, Pre trigger samples for L8828 must be betwwen 0 and 7 eighths" % self.severity

class L8828BAD_ACTIVEMEM(L8828Exception):
    status=0x277ca0e8
    facility="L8828"
    message="ACTIVEMEM must be beteen 16K and 2M"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8828-%s-BAD_ACTIVEMEM, ACTIVEMEM must be beteen 16K and 2M" % self.severity

class L8828BAD_CLOCK(L8828Exception):
    status=0x277ca0f0
    facility="L8828"
    message="Invalid clock frequency specified."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8828-%s-BAD_CLOCK, Invalid clock frequency specified." % self.severity

class L8818BAD_OFFSET(L8818Exception):
    status=0x277ca268
    facility="L8818"
    message="Offset for L8828 must be between 0 and 255"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8818-%s-BAD_OFFSET, Offset for L8828 must be between 0 and 255" % self.severity

class L8818BAD_PRETRIG(L8818Exception):
    status=0x277ca270
    facility="L8818"
    message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8818-%s-BAD_PRETRIG, Pre trigger samples for L8828 must be betwwen 0 and 7 eighths" % self.severity

class L8818BAD_ACTIVEMEM(L8818Exception):
    status=0x277ca278
    facility="L8818"
    message="ACTIVEMEM must be beteen 16K and 2M"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8818-%s-BAD_ACTIVEMEM, ACTIVEMEM must be beteen 16K and 2M" % self.severity

class L8818BAD_CLOCK(L8818Exception):
    status=0x277ca280
    facility="L8818"
    message="Invalid clock frequency specified."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8818-%s-BAD_CLOCK, Invalid clock frequency specified." % self.severity

class J_TR612BAD_ACTMEM(J_TR612Exception):
    status=0x277ca2b8
    facility="J_TR612"
    message="ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J_TR612-%s-BAD_ACTMEM, ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all" % self.severity

class J_TR612BAD_PRETRIG(J_TR612Exception):
    status=0x277ca2c0
    facility="J_TR612"
    message="PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J_TR612-%s-BAD_PRETRIG, PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples" % self.severity

class J_TR612BAD_MODE(J_TR612Exception):
    status=0x277ca2c8
    facility="J_TR612"
    message="MODE value out of range, must be 0 (for normal) or 1 (for burst mode)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J_TR612-%s-BAD_MODE, MODE value out of range, must be 0 (for normal) or 1 (for burst mode)" % self.severity

class J_TR612BAD_FREQUENCY(J_TR612Exception):
    status=0x277ca2d0
    facility="J_TR612"
    message="FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%J_TR612-%s-BAD_FREQUENCY, FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external" % self.severity

class L8206NODATA(L8206Exception):
    status=0x277ca3f8
    facility="L8206"
    message="no data has been written to memory"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%L8206-%s-NODATA, no data has been written to memory" % self.severity

class H912BAD_CLOCK(H912Exception):
    status=0x277ca448
    facility="H912"
    message="Bad value specified in INT_CLOCK node, use Setup device to correct"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H912-%s-BAD_CLOCK, Bad value specified in INT_CLOCK node, use Setup device to correct" % self.severity

class H912BAD_BLOCKS(H912Exception):
    status=0x277ca450
    facility="H912"
    message="Bad value specified in BLOCKS node, use Setup device to correct"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H912-%s-BAD_BLOCKS, Bad value specified in BLOCKS node, use Setup device to correct" % self.severity

class H912BAD_PTS(H912Exception):
    status=0x277ca458
    facility="H912"
    message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H912-%s-BAD_PTS, Bad value specfiied in PTS node, must be an integer value between 1 and 131071" % self.severity

class H908BAD_CLOCK(H908Exception):
    status=0x277ca498
    facility="H908"
    message="Bad value specified in INT_CLOCK node, use Setup device to correct"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H908-%s-BAD_CLOCK, Bad value specified in INT_CLOCK node, use Setup device to correct" % self.severity

class H908BAD_ACTIVE_CHANS(H908Exception):
    status=0x277ca4a0
    facility="H908"
    message="Bad value specified in ACTIVE_CHANS node, use Setup device to correct"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H908-%s-BAD_ACTIVE_CHANS, Bad value specified in ACTIVE_CHANS node, use Setup device to correct" % self.severity

class H908BAD_PTS(H908Exception):
    status=0x277ca4a8
    facility="H908"
    message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%H908-%s-BAD_PTS, Bad value specfiied in PTS node, must be an integer value between 1 and 131071" % self.severity

class DSP2904CHANNEL_READ_ERROR(DSP2904Exception):
    status=0x277ca4e8
    facility="DSP2904"
    message="Error reading channel"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DSP2904-%s-CHANNEL_READ_ERROR, Error reading channel" % self.severity

class PYUNHANDLED_EXCEPTION(PYException):
    status=0x277ca538
    facility="PY"
    message="Python device raised and exception, see log files for more details"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%PY-%s-UNHANDLED_EXCEPTION, Python device raised and exception, see log files for more details" % self.severity

class DT196BNO_SAMPLES(DT196BException):
    status=0x277ca588
    facility="DT196B"
    message="Module did not acquire any samples"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%DT196B-%s-NO_SAMPLES, Module did not acquire any samples" % self.severity

class ACQINITIALIZATION_ERROR(ACQException):
    status=0x277ca590
    facility="ACQ"
    message="Error during module initialization"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%ACQ-%s-INITIALIZATION_ERROR, Error during module initialization" % self.severity

class ACQSETTINGS_NOT_LOADED(ACQException):
    status=0x277ca598
    facility="ACQ"
    message="settings not loaded"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%ACQ-%s-SETTINGS_NOT_LOADED, settings not loaded" % self.severity

class ACQWRONG_TREE(ACQException):
    status=0x277ca5a0
    facility="ACQ"
    message="Attempt to store ACQ module into different tree than it was armed with"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%ACQ-%s-WRONG_TREE, Attempt to store ACQ module into different tree than it was armed with" % self.severity

class ACQWRONG_PATH(ACQException):
    status=0x277ca5a8
    facility="ACQ"
    message="Attempt to store ACQ module into different path than it was armed with"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%ACQ-%s-WRONG_PATH, Attempt to store ACQ module into different path than it was armed with" % self.severity

class ACQWRONG_SHOT(ACQException):
    status=0x277ca5b0
    facility="ACQ"
    message="Attempt to store ACQ module into different shot than it was armed with"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%ACQ-%s-WRONG_SHOT, Attempt to store ACQ module into different shot than it was armed with" % self.severity

class LibINSVIRMEM(LibException):
    status=0x158210
    facility="Lib"
    message="Insufficient virtual memory"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-INSVIRMEM, Insufficient virtual memory" % self.severity

class LibINVARG(LibException):
    status=0x158230
    facility="Lib"
    message="Invalid argument"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-INVARG, Invalid argument" % self.severity

class LibINVSTRDES(LibException):
    status=0x158220
    facility="Lib"
    message="Invalid string descriptor"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-INVSTRDES, Invalid string descriptor" % self.severity

class LibKEYNOTFOU(LibException):
    status=0x1582f8
    facility="Lib"
    message="Key not found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-KEYNOTFOU, Key not found" % self.severity

class LibNOTFOU(LibException):
    status=0x158270
    facility="Lib"
    message="Entity not found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-NOTFOU, Entity not found" % self.severity

class LibQUEWASEMP(LibException):
    status=0x1582e8
    facility="Lib"
    message="Queue was empty"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-QUEWASEMP, Queue was empty" % self.severity

class LibSTRTRU(LibException):
    status=0x158010
    facility="Lib"
    message="String truncated"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%LIB-%s-STRTRU, String truncated" % self.severity

class StrMATCH(StrException):
    status=0x248418
    facility="Str"
    message="Strings match"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%STR-%s-MATCH, Strings match" % self.severity

class StrNOMATCH(StrException):
    status=0x248208
    facility="Str"
    message="Strings do not match"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%STR-%s-NOMATCH, Strings do not match" % self.severity

class StrNOELEM(StrException):
    status=0x248218
    facility="Str"
    message="Not enough delimited characters"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%STR-%s-NOELEM, Not enough delimited characters" % self.severity

class StrINVDELIM(StrException):
    status=0x248210
    facility="Str"
    message="Not enough delimited characters"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%STR-%s-INVDELIM, Not enough delimited characters" % self.severity

class StrSTRTOOLON(StrException):
    status=0x248070
    facility="Str"
    message="String too long"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%STR-%s-STRTOOLON, String too long" % self.severity

class SsINTOVF(SsException):
    status=0x478
    facility="Ss"
    message="Integer overflow"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%SS-%s-INTOVF, Integer overflow" % self.severity

class CamDONE_Q(CamException):
    status=0x8018008
    facility="Cam"
    message="I/O completed with X=1, Q=1"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-DONE_Q, I/O completed with X=1, Q=1" % self.severity

class CamDONE_NOQ(CamException):
    status=0x8018010
    facility="Cam"
    message="I/O completed with X=1, Q=0"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-DONE_NOQ, I/O completed with X=1, Q=0" % self.severity

class CamDONE_NOX(CamException):
    status=0x8019000
    facility="Cam"
    message="I/O completed with X=0 - probable failure"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-DONE_NOX, I/O completed with X=0 - probable failure" % self.severity

class CamSERTRAERR(CamException):
    status=0x8019800
    facility="Cam"
    message="serial transmission error on highway"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-SERTRAERR, serial transmission error on highway" % self.severity

class CamSCCFAIL(CamException):
    status=0x8019840
    facility="Cam"
    message="serial crate controller failure"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-SCCFAIL, serial crate controller failure" % self.severity

class CamOFFLINE(CamException):
    status=0x8019868
    facility="Cam"
    message="crate is offline"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%CAM-%s-OFFLINE, crate is offline" % self.severity

class TclNORMAL(TclException):
    status=0x2a0008
    facility="Tcl"
    message="Normal successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TCL-%s-NORMAL, Normal successful completion" % self.severity

class TclFAILED_ESSENTIAL(TclException):
    status=0x2a0010
    facility="Tcl"
    message="Essential action failed"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TCL-%s-FAILED_ESSENTIAL, Essential action failed" % self.severity

class MdsdclSUCCESS(MdsdclException):
    status=0x8020008
    facility="Mdsdcl"
    message="Normal successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-SUCCESS, Normal successful completion" % self.severity

class MdsdclEXIT(MdsdclException):
    status=0x8020010
    facility="Mdsdcl"
    message="Normal exit"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-EXIT, Normal exit" % self.severity

class MdsdclERROR(MdsdclException):
    status=0x8020018
    facility="Mdsdcl"
    message="Unsuccessful execution of command"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-ERROR, Unsuccessful execution of command" % self.severity

class MdsdclNORMAL(MdsdclException):
    status=0x8020328
    facility="Mdsdcl"
    message="Normal successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-NORMAL, Normal successful completion" % self.severity

class MdsdclPRESENT(MdsdclException):
    status=0x8020330
    facility="Mdsdcl"
    message="Entity is present"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-PRESENT, Entity is present" % self.severity

class MdsdclIVVERB(MdsdclException):
    status=0x8020338
    facility="Mdsdcl"
    message="No such command"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-IVVERB, No such command" % self.severity

class MdsdclABSENT(MdsdclException):
    status=0x8020340
    facility="Mdsdcl"
    message="Entity is absent"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-ABSENT, Entity is absent" % self.severity

class MdsdclNEGATED(MdsdclException):
    status=0x8020348
    facility="Mdsdcl"
    message="Entity is present but negated"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-NEGATED, Entity is present but negated" % self.severity

class MdsdclNOTNEGATABLE(MdsdclException):
    status=0x8020350
    facility="Mdsdcl"
    message="Entity cannot be negated"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-NOTNEGATABLE, Entity cannot be negated" % self.severity

class MdsdclIVQUAL(MdsdclException):
    status=0x8020358
    facility="Mdsdcl"
    message="Invalid qualifier"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-IVQUAL, Invalid qualifier" % self.severity

class MdsdclPROMPT_MORE(MdsdclException):
    status=0x8020360
    facility="Mdsdcl"
    message="More input required for command"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-PROMPT_MORE, More input required for command" % self.severity

class MdsdclTOO_MANY_PRMS(MdsdclException):
    status=0x8020368
    facility="Mdsdcl"
    message="Too many parameters specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-TOO_MANY_PRMS, Too many parameters specified" % self.severity

class MdsdclTOO_MANY_VALS(MdsdclException):
    status=0x8020370
    facility="Mdsdcl"
    message="Too many values"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-TOO_MANY_VALS, Too many values" % self.severity

class MdsdclMISSING_VALUE(MdsdclException):
    status=0x8020378
    facility="Mdsdcl"
    message="Qualifier value needed"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%MDSDCL-%s-MISSING_VALUE, Qualifier value needed" % self.severity

class TreeALREADY_OFF(TreeException):
    status=0xfd18028
    facility="Tree"
    message="Node is already OFF"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ALREADY_OFF, Node is already OFF" % self.severity

class TreeALREADY_ON(TreeException):
    status=0xfd18030
    facility="Tree"
    message="Node is already ON"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ALREADY_ON, Node is already ON" % self.severity

class TreeALREADY_OPEN(TreeException):
    status=0xfd18038
    facility="Tree"
    message="Tree is already OPEN"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ALREADY_OPEN, Tree is already OPEN" % self.severity

class TreeALREADY_THERE(TreeException):
    status=0xfd18088
    facility="Tree"
    message="Node is already in the tree"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ALREADY_THERE, Node is already in the tree" % self.severity

class TreeBADRECORD(TreeException):
    status=0xfd180b8
    facility="Tree"
    message="Data corrupted: cannot read record"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-BADRECORD, Data corrupted: cannot read record" % self.severity

class TreeBOTH_OFF(TreeException):
    status=0xfd18098
    facility="Tree"
    message="Both this node and its parent are off"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-BOTH_OFF, Both this node and its parent are off" % self.severity

class TreeBUFFEROVF(TreeException):
    status=0xfd18110
    facility="Tree"
    message="Output buffer overflow"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-BUFFEROVF, Output buffer overflow" % self.severity

class TreeCONGLOMFULL(TreeException):
    status=0xfd18120
    facility="Tree"
    message="Current conglomerate is full"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-CONGLOMFULL, Current conglomerate is full" % self.severity

class TreeCONGLOM_NOT_FULL(TreeException):
    status=0xfd18128
    facility="Tree"
    message="Current conglomerate is not yet full"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-CONGLOM_NOT_FULL, Current conglomerate is not yet full" % self.severity

class TreeCONTINUING(TreeException):
    status=0xfd18960
    facility="Tree"
    message="Operation continuing: note following error"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-CONTINUING, Operation continuing: note following error" % self.severity

class TreeDUPTAG(TreeException):
    status=0xfd180c8
    facility="Tree"
    message="Tag name already in use"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-DUPTAG, Tag name already in use" % self.severity

class TreeEDITTING(TreeException):
    status=0xfd18190
    facility="Tree"
    message="Tree file open for edit: operation not permitted"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-EDITTING, Tree file open for edit: operation not permitted" % self.severity

class TreeILLEGAL_ITEM(TreeException):
    status=0xfd18108
    facility="Tree"
    message="Invalid item code or part number specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ILLEGAL_ITEM, Invalid item code or part number specified" % self.severity

class TreeILLPAGCNT(TreeException):
    status=0xfd180d0
    facility="Tree"
    message="Illegal page count, error mapping tree file"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ILLPAGCNT, Illegal page count, error mapping tree file" % self.severity

class TreeINVDFFCLASS(TreeException):
    status=0xfd18138
    facility="Tree"
    message="Invalid data fmt: only CLASS_S can have data in NCI"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVDFFCLASS, Invalid data fmt: only CLASS_S can have data in NCI" % self.severity

class TreeINVDTPUSG(TreeException):
    status=0xfd18188
    facility="Tree"
    message="Attempt to store datatype which conflicts with the designated usage of this node"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVDTPUSG, Attempt to store datatype which conflicts with the designated usage of this node" % self.severity

class TreeINVPATH(TreeException):
    status=0xfd18100
    facility="Tree"
    message="Invalid tree pathname specified"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVPATH, Invalid tree pathname specified" % self.severity

class TreeINVRECTYP(TreeException):
    status=0xfd18140
    facility="Tree"
    message="Record type invalid for requested operation"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVRECTYP, Record type invalid for requested operation" % self.severity

class TreeINVTREE(TreeException):
    status=0xfd180c0
    facility="Tree"
    message="Invalid tree identification structure"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVTREE, Invalid tree identification structure" % self.severity

class TreeMAXOPENEDIT(TreeException):
    status=0xfd180d8
    facility="Tree"
    message="Too many files open for edit"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-MAXOPENEDIT, Too many files open for edit" % self.severity

class TreeNEW(TreeException):
    status=0xfd18018
    facility="Tree"
    message="New tree created"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NEW, New tree created" % self.severity

class TreeNMN(TreeException):
    status=0xfd18060
    facility="Tree"
    message="No More Nodes"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NMN, No More Nodes" % self.severity

class TreeNMT(TreeException):
    status=0xfd18068
    facility="Tree"
    message="No More Tags"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NMT, No More Tags" % self.severity

class TreeNNF(TreeException):
    status=0xfd18070
    facility="Tree"
    message="Node Not Found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NNF, Node Not Found" % self.severity

class TreeNODATA(TreeException):
    status=0xfd180e0
    facility="Tree"
    message="No data available for this node"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NODATA, No data available for this node" % self.severity

class TreeNODNAMLEN(TreeException):
    status=0xfd18148
    facility="Tree"
    message="Node name too long (12 chars max)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NODNAMLEN, Node name too long (12 chars max)" % self.severity

class TreeNOEDIT(TreeException):
    status=0xfd180f0
    facility="Tree"
    message="Tree file is not open for edit"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOEDIT, Tree file is not open for edit" % self.severity

class TreeNOLOG(TreeException):
    status=0xfd181a8
    facility="Tree"
    message="Experiment pathname (xxx_path) not defined"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOLOG, Experiment pathname (xxx_path) not defined" % self.severity

class TreeNOMETHOD(TreeException):
    status=0xfd180b0
    facility="Tree"
    message="Method not available for this object"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOMETHOD, Method not available for this object" % self.severity

class TreeNOOVERWRITE(TreeException):
    status=0xfd18180
    facility="Tree"
    message="Write-once node: overwrite not permitted"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOOVERWRITE, Write-once node: overwrite not permitted" % self.severity

class TreeNORMAL(TreeException):
    status=0xfd18008
    facility="Tree"
    message="Normal successful completion"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NORMAL, Normal successful completion" % self.severity

class TreeNOTALLSUBS(TreeException):
    status=0xfd18020
    facility="Tree"
    message="Main tree opened but not all subtrees found/or connected"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOTALLSUBS, Main tree opened but not all subtrees found/or connected" % self.severity

class TreeNOTCHILDLESS(TreeException):
    status=0xfd180f8
    facility="Tree"
    message="Node must be childless to become subtree reference"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOTCHILDLESS, Node must be childless to become subtree reference" % self.severity

class TreeNOT_IN_LIST(TreeException):
    status=0xfd181c0
    facility="Tree"
    message="Tree being opened was not in the list"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOT_IN_LIST, Tree being opened was not in the list" % self.severity

class TreeNOTMEMBERLESS(TreeException):
    status=0xfd18170
    facility="Tree"
    message="Subtree reference can not have members"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOTMEMBERLESS, Subtree reference can not have members" % self.severity

class TreeNOTOPEN(TreeException):
    status=0xfd180e8
    facility="Tree"
    message="No tree file currently open"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOTOPEN, No tree file currently open" % self.severity

class TreeNOTSON(TreeException):
    status=0xfd18178
    facility="Tree"
    message="Subtree reference cannot be a member"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOTSON, Subtree reference cannot be a member" % self.severity

class TreeNOT_CONGLOM(TreeException):
    status=0xfd18160
    facility="Tree"
    message="Head node of conglomerate does not contain a DTYPE_CONGLOM record"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOT_CONGLOM, Head node of conglomerate does not contain a DTYPE_CONGLOM record" % self.severity

class TreeNOT_OPEN(TreeException):
    status=0xfd180a8
    facility="Tree"
    message="Tree not currently open"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOT_OPEN, Tree not currently open" % self.severity

class TreeNOWRITEMODEL(TreeException):
    status=0xfd18198
    facility="Tree"
    message="Data for this node can not be written into the MODEL file"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOWRITEMODEL, Data for this node can not be written into the MODEL file" % self.severity

class TreeNOWRITESHOT(TreeException):
    status=0xfd181a0
    facility="Tree"
    message="Data for this node can not be written into the SHOT file"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOWRITESHOT, Data for this node can not be written into the SHOT file" % self.severity

class TreeNO_CONTEXT(TreeException):
    status=0xfd18040
    facility="Tree"
    message="There is no active search to end"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NO_CONTEXT, There is no active search to end" % self.severity

class TreeOFF(TreeException):
    status=0xfd180a0
    facility="Tree"
    message="Node is OFF"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-OFF, Node is OFF" % self.severity

class TreeON(TreeException):
    status=0xfd18048
    facility="Tree"
    message="Node is ON"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-ON, Node is ON" % self.severity

class TreeOPEN(TreeException):
    status=0xfd18050
    facility="Tree"
    message="Tree is OPEN (no edit)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-OPEN, Tree is OPEN (no edit)" % self.severity

class TreeOPEN_EDIT(TreeException):
    status=0xfd18058
    facility="Tree"
    message="Tree is OPEN for edit"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-OPEN_EDIT, Tree is OPEN for edit" % self.severity

class TreePARENT_OFF(TreeException):
    status=0xfd18090
    facility="Tree"
    message="Parent of this node is OFF"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-PARENT_OFF, Parent of this node is OFF" % self.severity

class TreeREADERR(TreeException):
    status=0xfd181b8
    facility="Tree"
    message="Error reading record for node"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-READERR, Error reading record for node" % self.severity

class TreeREADONLY(TreeException):
    status=0xfd181b0
    facility="Tree"
    message="Tree was opened with readonly access"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-READONLY, Tree was opened with readonly access" % self.severity

class TreeRESOLVED(TreeException):
    status=0xfd18010
    facility="Tree"
    message="Indirect reference successfully resolved"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-RESOLVED, Indirect reference successfully resolved" % self.severity

class TreeSUCCESS(TreeException):
    status=0xfd18640
    facility="Tree"
    message="Operation successful"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-SUCCESS, Operation successful" % self.severity

class TreeTAGNAMLEN(TreeException):
    status=0xfd18150
    facility="Tree"
    message="Tagname too long (max 24 chars)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-TAGNAMLEN, Tagname too long (max 24 chars)" % self.severity

class TreeTNF(TreeException):
    status=0xfd18078
    facility="Tree"
    message="Tag Not Found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-TNF, Tag Not Found" % self.severity

class TreeTREENF(TreeException):
    status=0xfd18080
    facility="Tree"
    message="Tree Not Found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-TREENF, Tree Not Found" % self.severity

class TreeUNRESOLVED(TreeException):
    status=0xfd18130
    facility="Tree"
    message="Not an indirect node reference: No action taken"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-UNRESOLVED, Not an indirect node reference: No action taken" % self.severity

class TreeUNSPRTCLASS(TreeException):
    status=0xfd18118
    facility="Tree"
    message="Unsupported descriptor class"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-UNSPRTCLASS, Unsupported descriptor class" % self.severity

class TreeUNSUPARRDTYPE(TreeException):
    status=0xfd18168
    facility="Tree"
    message="Complex data types not supported as members of arrays"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-UNSUPARRDTYPE, Complex data types not supported as members of arrays" % self.severity

class TreeWRITEFIRST(TreeException):
    status=0xfd18158
    facility="Tree"
    message="Tree has been modified:  write or quit first"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-WRITEFIRST, Tree has been modified:  write or quit first" % self.severity

class TreeFAILURE(TreeException):
    status=0xfd18fa0
    facility="Tree"
    message="Operation NOT successful"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-FAILURE, Operation NOT successful" % self.severity

class TreeLOCK_FAILURE(TreeException):
    status=0xfd18fb0
    facility="Tree"
    message="Error locking file, perhaps NFSLOCKING not enabled on this system"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-LOCK_FAILURE, Error locking file, perhaps NFSLOCKING not enabled on this system" % self.severity

class TreeFILE_NOT_FOUND(TreeException):
    status=0xfd18fa8
    facility="Tree"
    message="File or Directory Not Found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-FILE_NOT_FOUND, File or Directory Not Found" % self.severity

class TreeCANCEL(TreeException):
    status=0xfd18c80
    facility="Tree"
    message="User canceled operation"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-CANCEL, User canceled operation" % self.severity

class TreeNOSEGMENTS(TreeException):
    status=0xfd18fb8
    facility="Tree"
    message="No segments exist in this node"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOSEGMENTS, No segments exist in this node" % self.severity

class TreeINVDTYPE(TreeException):
    status=0xfd18fc0
    facility="Tree"
    message="Invalid datatype for data segment"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVDTYPE, Invalid datatype for data segment" % self.severity

class TreeINVSHAPE(TreeException):
    status=0xfd18fc8
    facility="Tree"
    message="Invalid shape for this data segment"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVSHAPE, Invalid shape for this data segment" % self.severity

class TreeINVSHOT(TreeException):
    status=0xfd18fd8
    facility="Tree"
    message="Invalid shot number - must be -1 (model), 0 (current), or Positive"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVSHOT, Invalid shot number - must be -1 (model), 0 (current), or Positive" % self.severity

class TreeINVTAG(TreeException):
    status=0xfd18fe8
    facility="Tree"
    message="Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-INVTAG, Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores" % self.severity

class TreeNOPATH(TreeException):
    status=0xfd18ff0
    facility="Tree"
    message="No 'treename'_path environment variable defined. Cannot locate tree files."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOPATH, No 'treename'_path environment variable defined. Cannot locate tree files." % self.severity

class TreeTREEFILEREADERR(TreeException):
    status=0xfd18ff8
    facility="Tree"
    message="Error reading in tree file contents."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-TREEFILEREADERR, Error reading in tree file contents." % self.severity

class TreeMEMERR(TreeException):
    status=0xfd19000
    facility="Tree"
    message="Memory allocation error."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-MEMERR, Memory allocation error." % self.severity

class TreeNOCURRENT(TreeException):
    status=0xfd19008
    facility="Tree"
    message="No current shot number set for this tree."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOCURRENT, No current shot number set for this tree." % self.severity

class TreeFOPENW(TreeException):
    status=0xfd19010
    facility="Tree"
    message="Error opening file for read-write."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-FOPENW, Error opening file for read-write." % self.severity

class TreeFOPENR(TreeException):
    status=0xfd19018
    facility="Tree"
    message="Error opening file read-only."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-FOPENR, Error opening file read-only." % self.severity

class TreeFCREATE(TreeException):
    status=0xfd19020
    facility="Tree"
    message="Error creating new file."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-FCREATE, Error creating new file." % self.severity

class TreeCONNECTFAIL(TreeException):
    status=0xfd19028
    facility="Tree"
    message="Error connecting to remote server."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-CONNECTFAIL, Error connecting to remote server." % self.severity

class TreeNCIWRITE(TreeException):
    status=0xfd19030
    facility="Tree"
    message="Error writing node characterisitics to file."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NCIWRITE, Error writing node characterisitics to file." % self.severity

class TreeDELFAIL(TreeException):
    status=0xfd19038
    facility="Tree"
    message="Error deleting file."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-DELFAIL, Error deleting file." % self.severity

class TreeRENFAIL(TreeException):
    status=0xfd19040
    facility="Tree"
    message="Error renaming file."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-RENFAIL, Error renaming file." % self.severity

class TreeEMPTY(TreeException):
    status=0xfd19048
    facility="Tree"
    message="Empty string provided."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-EMPTY, Empty string provided." % self.severity

class TreePARSEERR(TreeException):
    status=0xfd19050
    facility="Tree"
    message="Invalid node search string."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-PARSEERR, Invalid node search string." % self.severity

class TreeNCIREAD(TreeException):
    status=0xfd19058
    facility="Tree"
    message="Error reading node characteristics from file."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NCIREAD, Error reading node characteristics from file." % self.severity

class TreeNOVERSION(TreeException):
    status=0xfd19060
    facility="Tree"
    message="No version available."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-NOVERSION, No version available." % self.severity

class TreeDFREAD(TreeException):
    status=0xfd19068
    facility="Tree"
    message="Error reading from datafile."

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TREE-%s-DFREAD, Error reading from datafile." % self.severity

class TdiBREAK(TdiException):
    status=0xfd38008
    facility="Tdi"
    message="BREAK was not in DO FOR SWITCH or WHILE"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-BREAK, BREAK was not in DO FOR SWITCH or WHILE" % self.severity

class TdiCASE(TdiException):
    status=0xfd38010
    facility="Tdi"
    message="CASE was not in SWITCH statement"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-CASE, CASE was not in SWITCH statement" % self.severity

class TdiCONTINUE(TdiException):
    status=0xfd38018
    facility="Tdi"
    message="CONTINUE was not in DO FOR or WHILE"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-CONTINUE, CONTINUE was not in DO FOR or WHILE" % self.severity

class TdiEXTRANEOUS(TdiException):
    status=0xfd38020
    facility="Tdi"
    message="Some characters were unused, bad number maybe"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-EXTRANEOUS, Some characters were unused, bad number maybe" % self.severity

class TdiRETURN(TdiException):
    status=0xfd38028
    facility="Tdi"
    message="Extraneous RETURN statement, not from a FUN"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-RETURN, Extraneous RETURN statement, not from a FUN" % self.severity

class TdiABORT(TdiException):
    status=0xfd38030
    facility="Tdi"
    message="Program requested abort"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-ABORT, Program requested abort" % self.severity

class TdiBAD_INDEX(TdiException):
    status=0xfd38038
    facility="Tdi"
    message="Index or subscript is too small or too big"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-BAD_INDEX, Index or subscript is too small or too big" % self.severity

class TdiBOMB(TdiException):
    status=0xfd38040
    facility="Tdi"
    message="Bad punctuation, could not compile the text"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-BOMB, Bad punctuation, could not compile the text" % self.severity

class TdiEXTRA_ARG(TdiException):
    status=0xfd38048
    facility="Tdi"
    message="Too many arguments for function, watch commas"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-EXTRA_ARG, Too many arguments for function, watch commas" % self.severity

class TdiGOTO(TdiException):
    status=0xfd38050
    facility="Tdi"
    message="GOTO target label not found"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-GOTO, GOTO target label not found" % self.severity

class TdiINVCLADSC(TdiException):
    status=0xfd38058
    facility="Tdi"
    message="Storage class not valid, must be scalar or array"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-INVCLADSC, Storage class not valid, must be scalar or array" % self.severity

class TdiINVCLADTY(TdiException):
    status=0xfd38060
    facility="Tdi"
    message="Invalid mixture of storage class and data type"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-INVCLADTY, Invalid mixture of storage class and data type" % self.severity

class TdiINVDTYDSC(TdiException):
    status=0xfd38068
    facility="Tdi"
    message="Storage data type is not valid"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-INVDTYDSC, Storage data type is not valid" % self.severity

class TdiINV_OPC(TdiException):
    status=0xfd38070
    facility="Tdi"
    message="Invalid operator code in a function"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-INV_OPC, Invalid operator code in a function" % self.severity

class TdiINV_SIZE(TdiException):
    status=0xfd38078
    facility="Tdi"
    message="Number of elements does not match declaration"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-INV_SIZE, Number of elements does not match declaration" % self.severity

class TdiMISMATCH(TdiException):
    status=0xfd38080
    facility="Tdi"
    message="Shape of arguments does not match"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-MISMATCH, Shape of arguments does not match" % self.severity

class TdiMISS_ARG(TdiException):
    status=0xfd38088
    facility="Tdi"
    message="Missing argument is required for function"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-MISS_ARG, Missing argument is required for function" % self.severity

class TdiNDIM_OVER(TdiException):
    status=0xfd38090
    facility="Tdi"
    message="Number of dimensions is over the allowed 8"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NDIM_OVER, Number of dimensions is over the allowed 8" % self.severity

class TdiNO_CMPLX(TdiException):
    status=0xfd38098
    facility="Tdi"
    message="There are no complex forms of this function"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NO_CMPLX, There are no complex forms of this function" % self.severity

class TdiNO_OPC(TdiException):
    status=0xfd380a0
    facility="Tdi"
    message="No support for this function, today"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NO_OPC, No support for this function, today" % self.severity

class TdiNO_OUTPTR(TdiException):
    status=0xfd380a8
    facility="Tdi"
    message="An output pointer is required"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NO_OUTPTR, An output pointer is required" % self.severity

class TdiNO_SELF_PTR(TdiException):
    status=0xfd380b0
    facility="Tdi"
    message="No $VALUE is defined for signal or validation"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NO_SELF_PTR, No $VALUE is defined for signal or validation" % self.severity

class TdiNOT_NUMBER(TdiException):
    status=0xfd380b8
    facility="Tdi"
    message="Value is not a scalar number and must be"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NOT_NUMBER, Value is not a scalar number and must be" % self.severity

class TdiNULL_PTR(TdiException):
    status=0xfd380c0
    facility="Tdi"
    message="Null pointer where value needed"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-NULL_PTR, Null pointer where value needed" % self.severity

class TdiRECURSIVE(TdiException):
    status=0xfd380c8
    facility="Tdi"
    message="Overly recursive function, calls itself maybe"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-RECURSIVE, Overly recursive function, calls itself maybe" % self.severity

class TdiSIG_DIM(TdiException):
    status=0xfd380d0
    facility="Tdi"
    message="Signal dimension does not match data shape"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-SIG_DIM, Signal dimension does not match data shape" % self.severity

class TdiSYNTAX(TdiException):
    status=0xfd380d8
    facility="Tdi"
    message="Bad punctuation or misspelled word or number"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-SYNTAX, Bad punctuation or misspelled word or number" % self.severity

class TdiTOO_BIG(TdiException):
    status=0xfd380e0
    facility="Tdi"
    message="Conversion of number lost significant digits"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-TOO_BIG, Conversion of number lost significant digits" % self.severity

class TdiUNBALANCE(TdiException):
    status=0xfd380e8
    facility="Tdi"
    message="Unbalanced () [] {} '' " " or /**/"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-UNBALANCE, Unbalanced () [] {} '' " " or /**/" % self.severity

class TdiUNKNOWN_VAR(TdiException):
    status=0xfd380f0
    facility="Tdi"
    message="Unknown/undefined variable name"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-UNKNOWN_VAR, Unknown/undefined variable name" % self.severity

class TdiSTRTOOLON(TdiException):
    status=0xfd380f8
    facility="Tdi"
    message="string is too long (greater than 65535)"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-STRTOOLON, string is too long (greater than 65535)" % self.severity

class TdiTIMEOUT(TdiException):
    status=0xfd38100
    facility="Tdi"
    message="task did not complete in alotted time"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%TDI-%s-TIMEOUT, task did not complete in alotted time" % self.severity


def statusToException(msgnum):
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
      return globals()[edict[msgnum & -8]](msgnum)
    except:
      return Exception("?-%s-?: Unknown status return - %s" % (_severity[msgnum & 7],hex(msgnum)))
