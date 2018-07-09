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


########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify one of the
# "xxxx_messages.xml files (or add a new one)
# and then in mdsshr do:
#     python gen_devices.py
########################################################

class MDSplusException(Exception):
  fac="MDSplus"
  statusDict={}
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __new__(cls,*argv):
      if len(argv)==0 or cls is not MDSplusException:
          return super(MDSplusException,cls).__new__(cls,*argv)
      status = int(argv[0])
      code   = status & -8
      if code in cls.statusDict:
          cls = cls.statusDict[code]
      else:
          cls = MDSplusUnknown
      return cls.__new__(cls,*argv)
  def __init__(self,status=None):
    if isinstance(status,int):
      self.status=status
    if not hasattr(self,'status'):
      self.status=PyUNHANDLED_EXCEPTION.status
      self.msgnam='Unknown'
      self.message='Unknown exception'
      self.fac='MDSplus'
    if isinstance(status,str):
      self.message = status
    self.severity=self.severities[self.status & 7]
    super(Exception,self).__init__(self.message)

  def __str__(self):
    return "%%%s-%s-%s, %s" % (self.fac.upper(),
                               self.severity,
                               self.msgnam,
                               self.message)

class MDSplusUnknown(MDSplusException):
  fac="MDSplus"
  msgnam="Unknown"
  def __init__(self,status):
    self.status=status
    self.severity=self.severities[self.status & 7]
    self.message="Operation returned unknown status value: %s" % str(status)
  def __repr__(self):
    return 'MDSplusUnknown(%s)'%(str(self.status),)

def statusToException(status):
    return MDSplusException(status)

def checkStatus(status,ignore=tuple()):
    if (status & 1)==0:
        exception = MDSplusException(status)
        if isinstance(exception, ignore):
            print(exception.message)
        else:
            raise MDSplusException(status)

########################### generated from mitdevices_messages.xml ########################



class DevException(MDSplusException):
  fac="Dev"


class DevBAD_ENDIDX(DevException):
  status=662470666
  message="unable to read end index for channel"
  msgnam="BAD_ENDIDX"

MDSplusException.statusDict[662470664] = DevBAD_ENDIDX


class DevBAD_FILTER(DevException):
  status=662470674
  message="illegal filter selected"
  msgnam="BAD_FILTER"

MDSplusException.statusDict[662470672] = DevBAD_FILTER


class DevBAD_FREQ(DevException):
  status=662470682
  message="illegal digitization frequency selected"
  msgnam="BAD_FREQ"

MDSplusException.statusDict[662470680] = DevBAD_FREQ


class DevBAD_GAIN(DevException):
  status=662470690
  message="illegal gain selected"
  msgnam="BAD_GAIN"

MDSplusException.statusDict[662470688] = DevBAD_GAIN


class DevBAD_HEADER(DevException):
  status=662470698
  message="unable to read header selection"
  msgnam="BAD_HEADER"

MDSplusException.statusDict[662470696] = DevBAD_HEADER


class DevBAD_HEADER_IDX(DevException):
  status=662470706
  message="unknown header configuration index"
  msgnam="BAD_HEADER_IDX"

MDSplusException.statusDict[662470704] = DevBAD_HEADER_IDX


class DevBAD_MEMORIES(DevException):
  status=662470714
  message="unable to read number of memory modules"
  msgnam="BAD_MEMORIES"

MDSplusException.statusDict[662470712] = DevBAD_MEMORIES


class DevBAD_MODE(DevException):
  status=662470722
  message="illegal mode selected"
  msgnam="BAD_MODE"

MDSplusException.statusDict[662470720] = DevBAD_MODE


class DevBAD_NAME(DevException):
  status=662470730
  message="unable to read module name"
  msgnam="BAD_NAME"

MDSplusException.statusDict[662470728] = DevBAD_NAME


class DevBAD_OFFSET(DevException):
  status=662470738
  message="illegal offset selected"
  msgnam="BAD_OFFSET"

MDSplusException.statusDict[662470736] = DevBAD_OFFSET


class DevBAD_STARTIDX(DevException):
  status=662470746
  message="unable to read start index for channel"
  msgnam="BAD_STARTIDX"

MDSplusException.statusDict[662470744] = DevBAD_STARTIDX


class DevNOT_TRIGGERED(DevException):
  status=662470754
  message="device was not triggered,  check wires and triggering device"
  msgnam="NOT_TRIGGERED"

MDSplusException.statusDict[662470752] = DevNOT_TRIGGERED


class DevFREQ_TO_HIGH(DevException):
  status=662470762
  message="the frequency is set to high for the requested number of channels"
  msgnam="FREQ_TO_HIGH"

MDSplusException.statusDict[662470760] = DevFREQ_TO_HIGH


class DevINVALID_NOC(DevException):
  status=662470770
  message="the NOC (number of channels) requested is greater than the physical number of channels"
  msgnam="INVALID_NOC"

MDSplusException.statusDict[662470768] = DevINVALID_NOC


class DevRANGE_MISMATCH(DevException):
  status=662470778
  message="the range specified on the menu doesn't match the range setting on the device"
  msgnam="RANGE_MISMATCH"

MDSplusException.statusDict[662470776] = DevRANGE_MISMATCH


class DevCAMACERR(DevException):
  status=662470786
  message="Error doing CAMAC IO"
  msgnam="CAMACERR"

MDSplusException.statusDict[662470784] = DevCAMACERR


class DevBAD_VERBS(DevException):
  status=662470794
  message="Error reading interpreter list (:VERBS)"
  msgnam="BAD_VERBS"

MDSplusException.statusDict[662470792] = DevBAD_VERBS


class DevBAD_COMMANDS(DevException):
  status=662470802
  message="Error reading command list"
  msgnam="BAD_COMMANDS"

MDSplusException.statusDict[662470800] = DevBAD_COMMANDS


class DevCAM_ADNR(DevException):
  status=662470810
  message="CAMAC: Address not recognized (2160)"
  msgnam="CAM_ADNR"

MDSplusException.statusDict[662470808] = DevCAM_ADNR


class DevCAM_ERR(DevException):
  status=662470818
  message="CAMAC: Error reported by crate controler"
  msgnam="CAM_ERR"

MDSplusException.statusDict[662470816] = DevCAM_ERR


class DevCAM_LOSYNC(DevException):
  status=662470826
  message="CAMAC: Lost Syncronization error"
  msgnam="CAM_LOSYNC"

MDSplusException.statusDict[662470824] = DevCAM_LOSYNC


class DevCAM_LPE(DevException):
  status=662470834
  message="CAMAC: Longitudinal Parity error"
  msgnam="CAM_LPE"

MDSplusException.statusDict[662470832] = DevCAM_LPE


class DevCAM_TMO(DevException):
  status=662470842
  message="CAMAC: Highway time out error"
  msgnam="CAM_TMO"

MDSplusException.statusDict[662470840] = DevCAM_TMO


class DevCAM_TPE(DevException):
  status=662470850
  message="CAMAC: Transverse Parity error"
  msgnam="CAM_TPE"

MDSplusException.statusDict[662470848] = DevCAM_TPE


class DevCAM_STE(DevException):
  status=662470858
  message="CAMAC: Serial Transmission error"
  msgnam="CAM_STE"

MDSplusException.statusDict[662470856] = DevCAM_STE


class DevCAM_DERR(DevException):
  status=662470866
  message="CAMAC: Delayed error from SCC"
  msgnam="CAM_DERR"

MDSplusException.statusDict[662470864] = DevCAM_DERR


class DevCAM_SQ(DevException):
  status=662470874
  message="CAMAC: I/O completion with Q = 1"
  msgnam="CAM_SQ"

MDSplusException.statusDict[662470872] = DevCAM_SQ


class DevCAM_NOSQ(DevException):
  status=662470882
  message="CAMAC: I/O completion with Q = 0"
  msgnam="CAM_NOSQ"

MDSplusException.statusDict[662470880] = DevCAM_NOSQ


class DevCAM_SX(DevException):
  status=662470890
  message="CAMAC: I/O completion with X = 1"
  msgnam="CAM_SX"

MDSplusException.statusDict[662470888] = DevCAM_SX


class DevCAM_NOSX(DevException):
  status=662470898
  message="CAMAC: I/O completion with X = 0"
  msgnam="CAM_NOSX"

MDSplusException.statusDict[662470896] = DevCAM_NOSX


class DevINV_SETUP(DevException):
  status=662470906
  message="device was not properly set up"
  msgnam="INV_SETUP"

MDSplusException.statusDict[662470904] = DevINV_SETUP


class DevPYDEVICE_NOT_FOUND(DevException):
  status=662470914
  message="Python device class not found."
  msgnam="PYDEVICE_NOT_FOUND"

MDSplusException.statusDict[662470912] = DevPYDEVICE_NOT_FOUND


class DevPY_INTERFACE_LIBRARY_NOT_FOUND(DevException):
  status=662470922
  message="The needed device hardware interface library could not be loaded."
  msgnam="PY_INTERFACE_LIBRARY_NOT_FOUND"

MDSplusException.statusDict[662470920] = DevPY_INTERFACE_LIBRARY_NOT_FOUND


class DevIO_STUCK(DevException):
  status=662470930
  message="I/O to Device is stuck. Check network connection and board status."
  msgnam="IO_STUCK"

MDSplusException.statusDict[662470928] = DevIO_STUCK


class DevUNKOWN_STATE(DevException):
  status=662470938
  message="Device returned unrecognized state string"
  msgnam="UNKOWN_STATE"

MDSplusException.statusDict[662470936] = DevUNKOWN_STATE


class DevWRONG_TREE(DevException):
  status=662470946
  message="Attempt to digitizerinto different tree than it was armed with"
  msgnam="WRONG_TREE"

MDSplusException.statusDict[662470944] = DevWRONG_TREE


class DevWRONG_PATH(DevException):
  status=662470954
  message="Attempt to store digitizer into different path than it was armed with"
  msgnam="WRONG_PATH"

MDSplusException.statusDict[662470952] = DevWRONG_PATH


class DevWRONG_SHOT(DevException):
  status=662470962
  message="Attempt to store digitizer into different shot than it was armed with"
  msgnam="WRONG_SHOT"

MDSplusException.statusDict[662470960] = DevWRONG_SHOT


class DevOFFLINE(DevException):
  status=662470970
  message="Device is not on line.  Check network connection"
  msgnam="OFFLINE"

MDSplusException.statusDict[662470968] = DevOFFLINE


class DevTRIGGERED_NOT_STORED(DevException):
  status=662470978
  message="Device was triggered but not stored."
  msgnam="TRIGGERED_NOT_STORED"

MDSplusException.statusDict[662470976] = DevTRIGGERED_NOT_STORED


class DevNO_NAME_SPECIFIED(DevException):
  status=662470986
  message="Device name must be specifed - pleas fill it in."
  msgnam="NO_NAME_SPECIFIED"

MDSplusException.statusDict[662470984] = DevNO_NAME_SPECIFIED


class DevBAD_ACTIVE_CHAN(DevException):
  status=662470994
  message="Active channels either not available or invalid"
  msgnam="BAD_ACTIVE_CHAN"

MDSplusException.statusDict[662470992] = DevBAD_ACTIVE_CHAN


class DevBAD_TRIG_SRC(DevException):
  status=662471002
  message="Trigger source either not available or invalid"
  msgnam="BAD_TRIG_SRC"

MDSplusException.statusDict[662471000] = DevBAD_TRIG_SRC


class DevBAD_CLOCK_SRC(DevException):
  status=662471010
  message="Clock source either not available or invalid"
  msgnam="BAD_CLOCK_SRC"

MDSplusException.statusDict[662471008] = DevBAD_CLOCK_SRC


class DevBAD_PRE_TRIG(DevException):
  status=662471018
  message="Pre trigger samples either not available or invalid"
  msgnam="BAD_PRE_TRIG"

MDSplusException.statusDict[662471016] = DevBAD_PRE_TRIG


class DevBAD_POST_TRIG(DevException):
  status=662471026
  message="Post trigger samples either not available or invalid"
  msgnam="BAD_POST_TRIG"

MDSplusException.statusDict[662471024] = DevBAD_POST_TRIG


class DevBAD_CLOCK_FREQ(DevException):
  status=662471034
  message="Clock frequency either not available or invalid"
  msgnam="BAD_CLOCK_FREQ"

MDSplusException.statusDict[662471032] = DevBAD_CLOCK_FREQ


class DevTRIGGER_FAILED(DevException):
  status=662471042
  message="Device trigger method failed"
  msgnam="TRIGGER_FAILED"

MDSplusException.statusDict[662471040] = DevTRIGGER_FAILED


class DevERROR_READING_CHANNEL(DevException):
  status=662471050
  message="Error reading data for channel from device"
  msgnam="ERROR_READING_CHANNEL"

MDSplusException.statusDict[662471048] = DevERROR_READING_CHANNEL


class DevERROR_DOING_INIT(DevException):
  status=662471058
  message="Error sending ARM command to device"
  msgnam="ERROR_DOING_INIT"

MDSplusException.statusDict[662471056] = DevERROR_DOING_INIT


class ReticonException(MDSplusException):
  fac="Reticon"


class ReticonNORMAL(ReticonException):
  status=662471065
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662471064] = ReticonNORMAL


class ReticonBAD_FRAMES(ReticonException):
  status=662471074
  message="frame count must be less than or equal to 2048"
  msgnam="BAD_FRAMES"

MDSplusException.statusDict[662471072] = ReticonBAD_FRAMES


class ReticonBAD_FRAME_SELECT(ReticonException):
  status=662471082
  message="frame interval must be 1,2,4,8,16,32 or 64"
  msgnam="BAD_FRAME_SELECT"

MDSplusException.statusDict[662471080] = ReticonBAD_FRAME_SELECT


class ReticonBAD_NUM_STATES(ReticonException):
  status=662471090
  message="number of states must be between 1 and 4"
  msgnam="BAD_NUM_STATES"

MDSplusException.statusDict[662471088] = ReticonBAD_NUM_STATES


class ReticonBAD_PERIOD(ReticonException):
  status=662471098
  message="period must be .5,1,2,4,8,16,32,64,128,256,512,1024,2048,4096 or 8192 msec"
  msgnam="BAD_PERIOD"

MDSplusException.statusDict[662471096] = ReticonBAD_PERIOD


class ReticonBAD_PIXEL_SELECT(ReticonException):
  status=662471106
  message="pixel selection must be an array of 256 boolean values"
  msgnam="BAD_PIXEL_SELECT"

MDSplusException.statusDict[662471104] = ReticonBAD_PIXEL_SELECT


class ReticonDATA_CORRUPTED(ReticonException):
  status=662471114
  message="data in memory is corrupted or framing error detected, no data stored"
  msgnam="DATA_CORRUPTED"

MDSplusException.statusDict[662471112] = ReticonDATA_CORRUPTED


class ReticonTOO_MANY_FRAMES(ReticonException):
  status=662471122
  message="over 8192 frame start indicators in data read from memory"
  msgnam="TOO_MANY_FRAMES"

MDSplusException.statusDict[662471120] = ReticonTOO_MANY_FRAMES


class J221Exception(MDSplusException):
  fac="J221"


class J221NORMAL(J221Exception):
  status=662471465
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662471464] = J221NORMAL


class J221INVALID_DATA(J221Exception):
  status=662471476
  message="ignoring invalid data in channel !SL"
  msgnam="INVALID_DATA"

MDSplusException.statusDict[662471472] = J221INVALID_DATA


class J221NO_DATA(J221Exception):
  status=662471482
  message="no valid data was found for any channel"
  msgnam="NO_DATA"

MDSplusException.statusDict[662471480] = J221NO_DATA


class TimingException(MDSplusException):
  fac="Timing"


class TimingINVCLKFRQ(TimingException):
  status=662471866
  message="Invalid clock frequency"
  msgnam="INVCLKFRQ"

MDSplusException.statusDict[662471864] = TimingINVCLKFRQ


class TimingINVDELDUR(TimingException):
  status=662471874
  message="Invalid pulse delay or duration, must be less than 655 seconds"
  msgnam="INVDELDUR"

MDSplusException.statusDict[662471872] = TimingINVDELDUR


class TimingINVOUTCTR(TimingException):
  status=662471882
  message="Invalid output mode selected"
  msgnam="INVOUTCTR"

MDSplusException.statusDict[662471880] = TimingINVOUTCTR


class TimingINVPSEUDODEV(TimingException):
  status=662471890
  message="Invalid pseudo device attached to this decoder channel"
  msgnam="INVPSEUDODEV"

MDSplusException.statusDict[662471888] = TimingINVPSEUDODEV


class TimingINVTRGMOD(TimingException):
  status=662471898
  message="Invalid trigger mode selected"
  msgnam="INVTRGMOD"

MDSplusException.statusDict[662471896] = TimingINVTRGMOD


class TimingNOPSEUDODEV(TimingException):
  status=662471907
  message="No Pseudo device attached to this channel ... disabling"
  msgnam="NOPSEUDODEV"

MDSplusException.statusDict[662471904] = TimingNOPSEUDODEV


class TimingTOO_MANY_EVENTS(TimingException):
  status=662471914
  message="More than 16 events used by this decoder"
  msgnam="TOO_MANY_EVENTS"

MDSplusException.statusDict[662471912] = TimingTOO_MANY_EVENTS


class B2408Exception(MDSplusException):
  fac="B2408"


class B2408NORMAL(B2408Exception):
  status=662472265
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662472264] = B2408NORMAL


class B2408OVERFLOW(B2408Exception):
  status=662472275
  message="Triggers received after overflow"
  msgnam="OVERFLOW"

MDSplusException.statusDict[662472272] = B2408OVERFLOW


class B2408TRIG_LIM(B2408Exception):
  status=662472284
  message="Trigger limit possibly exceeded"
  msgnam="TRIG_LIM"

MDSplusException.statusDict[662472280] = B2408TRIG_LIM


class FeraException(MDSplusException):
  fac="Fera"


class FeraNORMAL(FeraException):
  status=662472665
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662472664] = FeraNORMAL


class FeraDIGNOTSTRARRAY(FeraException):
  status=662472674
  message="The digitizer names must be an array of strings"
  msgnam="DIGNOTSTRARRAY"

MDSplusException.statusDict[662472672] = FeraDIGNOTSTRARRAY


class FeraNODIG(FeraException):
  status=662472682
  message="The digitizer names must be specified"
  msgnam="NODIG"

MDSplusException.statusDict[662472680] = FeraNODIG


class FeraMEMNOTSTRARRAY(FeraException):
  status=662472690
  message="The memory names must be an array of strings"
  msgnam="MEMNOTSTRARRAY"

MDSplusException.statusDict[662472688] = FeraMEMNOTSTRARRAY


class FeraNOMEM(FeraException):
  status=662472698
  message="The memory names must be specified"
  msgnam="NOMEM"

MDSplusException.statusDict[662472696] = FeraNOMEM


class FeraPHASE_LOST(FeraException):
  status=662472706
  message="Data phase lost No FERA data stored"
  msgnam="PHASE_LOST"

MDSplusException.statusDict[662472704] = FeraPHASE_LOST


class FeraCONFUSED(FeraException):
  status=662472716
  message="Fera Data inconsitant.  Data for this point zered."
  msgnam="CONFUSED"

MDSplusException.statusDict[662472712] = FeraCONFUSED


class FeraOVER_RUN(FeraException):
  status=662472724
  message="Possible FERA memory overrun, too many triggers."
  msgnam="OVER_RUN"

MDSplusException.statusDict[662472720] = FeraOVER_RUN


class FeraOVERFLOW(FeraException):
  status=662472731
  message="Possible FERA data saturated.  Data point zeroed"
  msgnam="OVERFLOW"

MDSplusException.statusDict[662472728] = FeraOVERFLOW


class Hm650Exception(MDSplusException):
  fac="Hm650"


class Hm650NORMAL(Hm650Exception):
  status=662473065
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662473064] = Hm650NORMAL


class Hm650DLYCHNG(Hm650Exception):
  status=662473076
  message="HM650 requested delay can not be processed by hardware."
  msgnam="DLYCHNG"

MDSplusException.statusDict[662473072] = Hm650DLYCHNG


class Hv4032Exception(MDSplusException):
  fac="Hv4032"


class Hv4032NORMAL(Hv4032Exception):
  status=662473465
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662473464] = Hv4032NORMAL


class Hv4032WRONG_POD_TYPE(Hv4032Exception):
  status=662473474
  message="HV40321A n and p can only be used with the HV4032 device"
  msgnam="WRONG_POD_TYPE"

MDSplusException.statusDict[662473472] = Hv4032WRONG_POD_TYPE


class Hv1440Exception(MDSplusException):
  fac="Hv1440"


class Hv1440NORMAL(Hv1440Exception):
  status=662473865
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662473864] = Hv1440NORMAL


class Hv1440WRONG_POD_TYPE(Hv1440Exception):
  status=662473874
  message="HV1443 can only be used with the HV1440 device"
  msgnam="WRONG_POD_TYPE"

MDSplusException.statusDict[662473872] = Hv1440WRONG_POD_TYPE


class Hv1440BAD_FRAME(Hv1440Exception):
  status=662473882
  message="HV1440 could not read the frame"
  msgnam="BAD_FRAME"

MDSplusException.statusDict[662473880] = Hv1440BAD_FRAME


class Hv1440BAD_RANGE(Hv1440Exception):
  status=662473890
  message="HV1440 could not read the range"
  msgnam="BAD_RANGE"

MDSplusException.statusDict[662473888] = Hv1440BAD_RANGE


class Hv1440OUTRNG(Hv1440Exception):
  status=662473898
  message="HV1440 out of range"
  msgnam="OUTRNG"

MDSplusException.statusDict[662473896] = Hv1440OUTRNG


class Hv1440STUCK(Hv1440Exception):
  status=662473906
  message="HV1440 not responding with Q"
  msgnam="STUCK"

MDSplusException.statusDict[662473904] = Hv1440STUCK


class JoergerException(MDSplusException):
  fac="Joerger"


class JoergerBAD_PRE_TRIGGER(JoergerException):
  status=662474266
  message="bad pretrigger specified, specify a value of 0,1,2,3,4,5,6 or 7"
  msgnam="BAD_PRE_TRIGGER"

MDSplusException.statusDict[662474264] = JoergerBAD_PRE_TRIGGER


class JoergerBAD_ACT_MEMORY(JoergerException):
  status=662474274
  message="bad active memory specified, specify a value of 1,2,3,4,5,6,7 or 8"
  msgnam="BAD_ACT_MEMORY"

MDSplusException.statusDict[662474272] = JoergerBAD_ACT_MEMORY


class JoergerBAD_GAIN(JoergerException):
  status=662474282
  message="bad gain specified, specify a value of 1,2,4 or 8"
  msgnam="BAD_GAIN"

MDSplusException.statusDict[662474280] = JoergerBAD_GAIN


class U_of_mException(MDSplusException):
  fac="U_of_m"


class U_of_mBAD_WAVE_LENGTH(U_of_mException):
  status=662474666
  message="bad wave length specified, specify value between 0 and 13000"
  msgnam="BAD_WAVE_LENGTH"

MDSplusException.statusDict[662474664] = U_of_mBAD_WAVE_LENGTH


class U_of_mBAD_SLIT_WIDTH(U_of_mException):
  status=662474674
  message="bad slit width specified, specify value between 0 and 500"
  msgnam="BAD_SLIT_WIDTH"

MDSplusException.statusDict[662474672] = U_of_mBAD_SLIT_WIDTH


class U_of_mBAD_NUM_SPECTRA(U_of_mException):
  status=662474682
  message="bad number of spectra specified, specify value between 1 and 100"
  msgnam="BAD_NUM_SPECTRA"

MDSplusException.statusDict[662474680] = U_of_mBAD_NUM_SPECTRA


class U_of_mBAD_GRATING(U_of_mException):
  status=662474690
  message="bad grating type specified, specify value between 1 and 5"
  msgnam="BAD_GRATING"

MDSplusException.statusDict[662474688] = U_of_mBAD_GRATING


class U_of_mBAD_EXPOSURE(U_of_mException):
  status=662474698
  message="bad exposure time specified, specify value between 30 and 3000"
  msgnam="BAD_EXPOSURE"

MDSplusException.statusDict[662474696] = U_of_mBAD_EXPOSURE


class U_of_mBAD_FILTER(U_of_mException):
  status=662474706
  message="bad neutral density filter specified, specify value between 0 and 5"
  msgnam="BAD_FILTER"

MDSplusException.statusDict[662474704] = U_of_mBAD_FILTER


class U_of_mGO_FILE_ERROR(U_of_mException):
  status=662474714
  message="error creating new go file"
  msgnam="GO_FILE_ERROR"

MDSplusException.statusDict[662474712] = U_of_mGO_FILE_ERROR


class U_of_mDATA_FILE_ERROR(U_of_mException):
  status=662474722
  message="error opening datafile"
  msgnam="DATA_FILE_ERROR"

MDSplusException.statusDict[662474720] = U_of_mDATA_FILE_ERROR


class IdlException(MDSplusException):
  fac="Idl"


class IdlNORMAL(IdlException):
  status=662475065
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662475064] = IdlNORMAL


class IdlERROR(IdlException):
  status=662475074
  message="IDL returned a non zero error code"
  msgnam="ERROR"

MDSplusException.statusDict[662475072] = IdlERROR


class B5910aException(MDSplusException):
  fac="B5910a"


class B5910aBAD_CHAN(B5910aException):
  status=662475466
  message="error evaluating data for channel !SL"
  msgnam="BAD_CHAN"

MDSplusException.statusDict[662475464] = B5910aBAD_CHAN


class B5910aBAD_CLOCK(B5910aException):
  status=662475474
  message="invalid internal clock range specified"
  msgnam="BAD_CLOCK"

MDSplusException.statusDict[662475472] = B5910aBAD_CLOCK


class B5910aBAD_ITERATIONS(B5910aException):
  status=662475482
  message="invalid number of iterations specified"
  msgnam="BAD_ITERATIONS"

MDSplusException.statusDict[662475480] = B5910aBAD_ITERATIONS


class B5910aBAD_NOC(B5910aException):
  status=662475490
  message="invalid number of active channels specified"
  msgnam="BAD_NOC"

MDSplusException.statusDict[662475488] = B5910aBAD_NOC


class B5910aBAD_SAMPS(B5910aException):
  status=662475498
  message="number of samples specificed invalid"
  msgnam="BAD_SAMPS"

MDSplusException.statusDict[662475496] = B5910aBAD_SAMPS


class J412Exception(MDSplusException):
  fac="J412"


class J412NOT_SORTED(J412Exception):
  status=662475866
  message="times specified for J412 module were not sorted"
  msgnam="NOT_SORTED"

MDSplusException.statusDict[662475864] = J412NOT_SORTED


class J412NO_DATA(J412Exception):
  status=662475874
  message="there were no times specifed for J412 module"
  msgnam="NO_DATA"

MDSplusException.statusDict[662475872] = J412NO_DATA


class J412BADCYCLES(J412Exception):
  status=662475882
  message="The number of cycles must be 1 .. 255"
  msgnam="BADCYCLES"

MDSplusException.statusDict[662475880] = J412BADCYCLES


class Tr16Exception(MDSplusException):
  fac="Tr16"


class Tr16NORMAL(Tr16Exception):
  status=662476265
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662476264] = Tr16NORMAL


class Tr16BAD_MEMSIZE(Tr16Exception):
  status=662476274
  message="Memory size must be in 128K, 256K, 512k, 1024K"
  msgnam="BAD_MEMSIZE"

MDSplusException.statusDict[662476272] = Tr16BAD_MEMSIZE


class Tr16BAD_ACTIVEMEM(Tr16Exception):
  status=662476282
  message="Active Mem must be power of 2 8K to 1024K"
  msgnam="BAD_ACTIVEMEM"

MDSplusException.statusDict[662476280] = Tr16BAD_ACTIVEMEM


class Tr16BAD_ACTIVECHAN(Tr16Exception):
  status=662476290
  message="Active channels must be in 1,2,4,8,16"
  msgnam="BAD_ACTIVECHAN"

MDSplusException.statusDict[662476288] = Tr16BAD_ACTIVECHAN


class Tr16BAD_PTS(Tr16Exception):
  status=662476298
  message="PTS must be power of 2 32 to 1024K"
  msgnam="BAD_PTS"

MDSplusException.statusDict[662476296] = Tr16BAD_PTS


class Tr16BAD_FREQUENCY(Tr16Exception):
  status=662476306
  message="Invalid clock frequency"
  msgnam="BAD_FREQUENCY"

MDSplusException.statusDict[662476304] = Tr16BAD_FREQUENCY


class Tr16BAD_MASTER(Tr16Exception):
  status=662476314
  message="Master must be 0 or 1"
  msgnam="BAD_MASTER"

MDSplusException.statusDict[662476312] = Tr16BAD_MASTER


class Tr16BAD_GAIN(Tr16Exception):
  status=662476322
  message="Gain must be 1, 2, 4, or 8"
  msgnam="BAD_GAIN"

MDSplusException.statusDict[662476320] = Tr16BAD_GAIN


class A14Exception(MDSplusException):
  fac="A14"


class A14NORMAL(A14Exception):
  status=662476665
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662476664] = A14NORMAL


class A14BAD_CLK_DIVIDE(A14Exception):
  status=662476674
  message="Clock divide must be one of 1,2,4,10,20,40, or 100"
  msgnam="BAD_CLK_DIVIDE"

MDSplusException.statusDict[662476672] = A14BAD_CLK_DIVIDE


class A14BAD_MODE(A14Exception):
  status=662476682
  message="Mode must be in the range of 0 to 4"
  msgnam="BAD_MODE"

MDSplusException.statusDict[662476680] = A14BAD_MODE


class A14BAD_CLK_POLARITY(A14Exception):
  status=662476690
  message="Clock polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_CLK_POLARITY"

MDSplusException.statusDict[662476688] = A14BAD_CLK_POLARITY


class A14BAD_STR_POLARITY(A14Exception):
  status=662476698
  message="Start polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_STR_POLARITY"

MDSplusException.statusDict[662476696] = A14BAD_STR_POLARITY


class A14BAD_STP_POLARITY(A14Exception):
  status=662476706
  message="Stop polarity must be either 0 (rising) or 1 (falling)"
  msgnam="BAD_STP_POLARITY"

MDSplusException.statusDict[662476704] = A14BAD_STP_POLARITY


class A14BAD_GATED(A14Exception):
  status=662476714
  message="Gated clock must be either 0 (not gated) or 1 (gated)"
  msgnam="BAD_GATED"

MDSplusException.statusDict[662476712] = A14BAD_GATED


class L6810Exception(MDSplusException):
  fac="L6810"


class L6810NORMAL(L6810Exception):
  status=662477065
  message="successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[662477064] = L6810NORMAL


class L6810BAD_ACTIVECHAN(L6810Exception):
  status=662477074
  message="Active chans must be 1, 2, or 4"
  msgnam="BAD_ACTIVECHAN"

MDSplusException.statusDict[662477072] = L6810BAD_ACTIVECHAN


class L6810BAD_ACTIVEMEM(L6810Exception):
  status=662477082
  message="Active memory must be power of 2 LE 8192"
  msgnam="BAD_ACTIVEMEM"

MDSplusException.statusDict[662477080] = L6810BAD_ACTIVEMEM


class L6810BAD_FREQUENCY(L6810Exception):
  status=662477090
  message="Frequency must be in [0, .02, .05, .1, .2, .5, 1, 2, 5, 10, 20, 50, 100,  200, 500, 1000, 2000, 5000]"
  msgnam="BAD_FREQUENCY"

MDSplusException.statusDict[662477088] = L6810BAD_FREQUENCY


class L6810BAD_FULL_SCALE(L6810Exception):
  status=662477098
  message="Full Scale must be in [.4096, 1.024, 2.048, 4.096, 10.24, 25.6, 51.2, 102.4]"
  msgnam="BAD_FULL_SCALE"

MDSplusException.statusDict[662477096] = L6810BAD_FULL_SCALE


class L6810BAD_MEMORIES(L6810Exception):
  status=662477106
  message="Memories must 1 .. 16"
  msgnam="BAD_MEMORIES"

MDSplusException.statusDict[662477104] = L6810BAD_MEMORIES


class L6810BAD_COUPLING(L6810Exception):
  status=662477114
  message="Channel source / coupling must be one of 0 .. 7"
  msgnam="BAD_COUPLING"

MDSplusException.statusDict[662477112] = L6810BAD_COUPLING


class L6810BAD_OFFSET(L6810Exception):
  status=662477122
  message="Offset must be between 0 and 255"
  msgnam="BAD_OFFSET"

MDSplusException.statusDict[662477120] = L6810BAD_OFFSET


class L6810BAD_SEGMENTS(L6810Exception):
  status=662477130
  message="Number of segments must be 1 .. 1024"
  msgnam="BAD_SEGMENTS"

MDSplusException.statusDict[662477128] = L6810BAD_SEGMENTS


class L6810BAD_TRIG_DELAY(L6810Exception):
  status=662477138
  message="Trigger delay must be between -8 and 247 in units of 8ths of segment size"
  msgnam="BAD_TRIG_DELAY"

MDSplusException.statusDict[662477136] = L6810BAD_TRIG_DELAY


class J_dacException(MDSplusException):
  fac="J_dac"


class J_dacOUTRNG(J_dacException):
  status=662477466
  message="Joerger DAC Channels out of range.  Bad chans code !XW"
  msgnam="OUTRNG"

MDSplusException.statusDict[662477464] = J_dacOUTRNG


class IncaaException(MDSplusException):
  fac="Incaa"


class IncaaBAD_ACTIVE_CHANS(IncaaException):
  status=662477866
  message="bad active channels selection"
  msgnam="BAD_ACTIVE_CHANS"

MDSplusException.statusDict[662477864] = IncaaBAD_ACTIVE_CHANS


class IncaaBAD_MASTER(IncaaException):
  status=662477874
  message="bad master selection, must be 0 or 1"
  msgnam="BAD_MASTER"

MDSplusException.statusDict[662477872] = IncaaBAD_MASTER


class IncaaBAD_EXT_1MHZ(IncaaException):
  status=662477882
  message="bad ext 1mhz selection, must be 0 or 1"
  msgnam="BAD_EXT_1MHZ"

MDSplusException.statusDict[662477880] = IncaaBAD_EXT_1MHZ


class IncaaBAD_PTSC(IncaaException):
  status=662477890
  message="bad PTSC setting"
  msgnam="BAD_PTSC"

MDSplusException.statusDict[662477888] = IncaaBAD_PTSC


class L8212Exception(MDSplusException):
  fac="L8212"


class L8212BAD_HEADER(L8212Exception):
  status=662478266
  message="Invalid header jumper information (e.g. 49414944432)"
  msgnam="BAD_HEADER"

MDSplusException.statusDict[662478264] = L8212BAD_HEADER


class L8212BAD_MEMORIES(L8212Exception):
  status=662478274
  message="Invalid number of memories, must be 1 .. 16"
  msgnam="BAD_MEMORIES"

MDSplusException.statusDict[662478272] = L8212BAD_MEMORIES


class L8212BAD_NOC(L8212Exception):
  status=662478282
  message="Invalid number of active channels"
  msgnam="BAD_NOC"

MDSplusException.statusDict[662478280] = L8212BAD_NOC


class L8212BAD_OFFSET(L8212Exception):
  status=662478290
  message="Invalid offset must be one of (0, -2048, -4096)"
  msgnam="BAD_OFFSET"

MDSplusException.statusDict[662478288] = L8212BAD_OFFSET


class L8212BAD_PTS(L8212Exception):
  status=662478298
  message="Invalid pts code, must be 0 .. 7"
  msgnam="BAD_PTS"

MDSplusException.statusDict[662478296] = L8212BAD_PTS


class L8212FREQ_TO_HIGH(L8212Exception):
  status=662478306
  message="Frequency to high for selected number of channels"
  msgnam="FREQ_TO_HIGH"

MDSplusException.statusDict[662478304] = L8212FREQ_TO_HIGH


class L8212INVALID_NOC(L8212Exception):
  status=662478314
  message="Invalid number of active channels"
  msgnam="INVALID_NOC"

MDSplusException.statusDict[662478312] = L8212INVALID_NOC


class MpbException(MDSplusException):
  fac="Mpb"


class MpbBADTIME(MpbException):
  status=662478666
  message="Could not read time"
  msgnam="BADTIME"

MDSplusException.statusDict[662478664] = MpbBADTIME


class MpbBADFREQ(MpbException):
  status=662478674
  message="Could not read frequency"
  msgnam="BADFREQ"

MDSplusException.statusDict[662478672] = MpbBADFREQ


class L8828Exception(MDSplusException):
  fac="L8828"


class L8828BAD_OFFSET(L8828Exception):
  status=662479066
  message="Offset for L8828 must be between 0 and 255"
  msgnam="BAD_OFFSET"

MDSplusException.statusDict[662479064] = L8828BAD_OFFSET


class L8828BAD_PRETRIG(L8828Exception):
  status=662479074
  message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"
  msgnam="BAD_PRETRIG"

MDSplusException.statusDict[662479072] = L8828BAD_PRETRIG


class L8828BAD_ACTIVEMEM(L8828Exception):
  status=662479082
  message="ACTIVEMEM must be beteen 16K and 2M"
  msgnam="BAD_ACTIVEMEM"

MDSplusException.statusDict[662479080] = L8828BAD_ACTIVEMEM


class L8828BAD_CLOCK(L8828Exception):
  status=662479090
  message="Invalid clock frequency specified."
  msgnam="BAD_CLOCK"

MDSplusException.statusDict[662479088] = L8828BAD_CLOCK


class L8818Exception(MDSplusException):
  fac="L8818"


class L8818BAD_OFFSET(L8818Exception):
  status=662479466
  message="Offset for L8828 must be between 0 and 255"
  msgnam="BAD_OFFSET"

MDSplusException.statusDict[662479464] = L8818BAD_OFFSET


class L8818BAD_PRETRIG(L8818Exception):
  status=662479474
  message="Pre trigger samples for L8828 must be betwwen 0 and 7 eighths"
  msgnam="BAD_PRETRIG"

MDSplusException.statusDict[662479472] = L8818BAD_PRETRIG


class L8818BAD_ACTIVEMEM(L8818Exception):
  status=662479482
  message="ACTIVEMEM must be beteen 16K and 2M"
  msgnam="BAD_ACTIVEMEM"

MDSplusException.statusDict[662479480] = L8818BAD_ACTIVEMEM


class L8818BAD_CLOCK(L8818Exception):
  status=662479490
  message="Invalid clock frequency specified."
  msgnam="BAD_CLOCK"

MDSplusException.statusDict[662479488] = L8818BAD_CLOCK


class J_tr612Exception(MDSplusException):
  fac="J_tr612"


class J_tr612BAD_ACTMEM(J_tr612Exception):
  status=662479546
  message="ACTMEM value out of range, must be 0-7 where 0=1/8th and 7 = all"
  msgnam="BAD_ACTMEM"

MDSplusException.statusDict[662479544] = J_tr612BAD_ACTMEM


class J_tr612BAD_PRETRIG(J_tr612Exception):
  status=662479554
  message="PRETRIG value out of range, must be 0-7 where 0 = none and 7 = 7/8 pretrigger samples"
  msgnam="BAD_PRETRIG"

MDSplusException.statusDict[662479552] = J_tr612BAD_PRETRIG


class J_tr612BAD_MODE(J_tr612Exception):
  status=662479562
  message="MODE value out of range, must be 0 (for normal) or 1 (for burst mode)"
  msgnam="BAD_MODE"

MDSplusException.statusDict[662479560] = J_tr612BAD_MODE


class J_tr612BAD_FREQUENCY(J_tr612Exception):
  status=662479570
  message="FREQUENCY value out of range, must be 0-4 where 0=3MHz,1=2MHz,2=1MHz,3=100KHz,4=external"
  msgnam="BAD_FREQUENCY"

MDSplusException.statusDict[662479568] = J_tr612BAD_FREQUENCY


class L8206Exception(MDSplusException):
  fac="L8206"


class L8206NODATA(L8206Exception):
  status=662479868
  message="no data has been written to memory"
  msgnam="NODATA"

MDSplusException.statusDict[662479864] = L8206NODATA


class H912Exception(MDSplusException):
  fac="H912"


class H912BAD_CLOCK(H912Exception):
  status=662479946
  message="Bad value specified in INT_CLOCK node, use Setup device to correct"
  msgnam="BAD_CLOCK"

MDSplusException.statusDict[662479944] = H912BAD_CLOCK


class H912BAD_BLOCKS(H912Exception):
  status=662479954
  message="Bad value specified in BLOCKS node, use Setup device to correct"
  msgnam="BAD_BLOCKS"

MDSplusException.statusDict[662479952] = H912BAD_BLOCKS


class H912BAD_PTS(H912Exception):
  status=662479962
  message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"
  msgnam="BAD_PTS"

MDSplusException.statusDict[662479960] = H912BAD_PTS


class H908Exception(MDSplusException):
  fac="H908"


class H908BAD_CLOCK(H908Exception):
  status=662480026
  message="Bad value specified in INT_CLOCK node, use Setup device to correct"
  msgnam="BAD_CLOCK"

MDSplusException.statusDict[662480024] = H908BAD_CLOCK


class H908BAD_ACTIVE_CHANS(H908Exception):
  status=662480034
  message="Bad value specified in ACTIVE_CHANS node, use Setup device to correct"
  msgnam="BAD_ACTIVE_CHANS"

MDSplusException.statusDict[662480032] = H908BAD_ACTIVE_CHANS


class H908BAD_PTS(H908Exception):
  status=662480042
  message="Bad value specfiied in PTS node, must be an integer value between 1 and 131071"
  msgnam="BAD_PTS"

MDSplusException.statusDict[662480040] = H908BAD_PTS


class Dsp2904Exception(MDSplusException):
  fac="Dsp2904"


class Dsp2904CHANNEL_READ_ERROR(Dsp2904Exception):
  status=662480106
  message="Error reading channel"
  msgnam="CHANNEL_READ_ERROR"

MDSplusException.statusDict[662480104] = Dsp2904CHANNEL_READ_ERROR


class PyException(MDSplusException):
  fac="Py"


class PyUNHANDLED_EXCEPTION(PyException):
  status=662480186
  message="Python device raised an exception, see log files for more details"
  msgnam="UNHANDLED_EXCEPTION"

MDSplusException.statusDict[662480184] = PyUNHANDLED_EXCEPTION


class Dt196bException(MDSplusException):
  fac="Dt196b"


class Dt196bNO_SAMPLES(Dt196bException):
  status=662480266
  message="Module did not acquire any samples"
  msgnam="NO_SAMPLES"

MDSplusException.statusDict[662480264] = Dt196bNO_SAMPLES


class DevCANNOT_LOAD_SETTINGS(DevException):
  status=662480290
  message="Error loading settings from XML"
  msgnam="CANNOT_LOAD_SETTINGS"

MDSplusException.statusDict[662480288] = DevCANNOT_LOAD_SETTINGS


class DevCANNOT_GET_BOARD_STATE(DevException):
  status=662480298
  message="Cannot retrieve state of daq board"
  msgnam="CANNOT_GET_BOARD_STATE"

MDSplusException.statusDict[662480296] = DevCANNOT_GET_BOARD_STATE


class DevACQCMD_FAILED(DevException):
  status=662480306
  message="Error executing acqcmd on daq board"
  msgnam="ACQCMD_FAILED"

MDSplusException.statusDict[662480304] = DevACQCMD_FAILED


class DevACQ2SH_FAILED(DevException):
  status=662480314
  message="Error executing acq2sh command on daq board"
  msgnam="ACQ2SH_FAILED"

MDSplusException.statusDict[662480312] = DevACQ2SH_FAILED


class DevBAD_PARAMETER(DevException):
  status=662480322
  message="Invalid parameter specified for device"
  msgnam="BAD_PARAMETER"

MDSplusException.statusDict[662480320] = DevBAD_PARAMETER


class DevCOMM_ERROR(DevException):
  status=662480330
  message="Error communicating with device"
  msgnam="COMM_ERROR"

MDSplusException.statusDict[662480328] = DevCOMM_ERROR


class DevCAMERA_NOT_FOUND(DevException):
  status=662480338
  message="Could not find specified camera on the network"
  msgnam="CAMERA_NOT_FOUND"

MDSplusException.statusDict[662480336] = DevCAMERA_NOT_FOUND


class DevNOT_A_PYDEVICE(DevException):
  status=662480346
  message="Device is not a python device."
  msgnam="NOT_A_PYDEVICE"

MDSplusException.statusDict[662480344] = DevNOT_A_PYDEVICE

########################### generated from treeshr_messages.xml ########################



class TreeException(MDSplusException):
  fac="Tree"


class TreeALREADY_OFF(TreeException):
  status=265388075
  message="Node is already OFF"
  msgnam="ALREADY_OFF"

MDSplusException.statusDict[265388072] = TreeALREADY_OFF


class TreeALREADY_ON(TreeException):
  status=265388083
  message="Node is already ON"
  msgnam="ALREADY_ON"

MDSplusException.statusDict[265388080] = TreeALREADY_ON


class TreeALREADY_OPEN(TreeException):
  status=265388091
  message="Tree is already OPEN"
  msgnam="ALREADY_OPEN"

MDSplusException.statusDict[265388088] = TreeALREADY_OPEN


class TreeALREADY_THERE(TreeException):
  status=265388168
  message="Node is already in the tree"
  msgnam="ALREADY_THERE"

MDSplusException.statusDict[265388168] = TreeALREADY_THERE


class TreeBADRECORD(TreeException):
  status=265388218
  message="Data corrupted: cannot read record"
  msgnam="BADRECORD"

MDSplusException.statusDict[265388216] = TreeBADRECORD


class TreeBOTH_OFF(TreeException):
  status=265388184
  message="Both this node and its parent are off"
  msgnam="BOTH_OFF"

MDSplusException.statusDict[265388184] = TreeBOTH_OFF


class TreeBUFFEROVF(TreeException):
  status=265388306
  message="Output buffer overflow"
  msgnam="BUFFEROVF"

MDSplusException.statusDict[265388304] = TreeBUFFEROVF


class TreeCONGLOMFULL(TreeException):
  status=265388322
  message="Current conglomerate is full"
  msgnam="CONGLOMFULL"

MDSplusException.statusDict[265388320] = TreeCONGLOMFULL


class TreeCONGLOM_NOT_FULL(TreeException):
  status=265388330
  message="Current conglomerate is not yet full"
  msgnam="CONGLOM_NOT_FULL"

MDSplusException.statusDict[265388328] = TreeCONGLOM_NOT_FULL


class TreeCONTINUING(TreeException):
  status=265390435
  message="Operation continuing: note following error"
  msgnam="CONTINUING"

MDSplusException.statusDict[265390432] = TreeCONTINUING


class TreeDUPTAG(TreeException):
  status=265388234
  message="Tag name already in use"
  msgnam="DUPTAG"

MDSplusException.statusDict[265388232] = TreeDUPTAG


class TreeEDITTING(TreeException):
  status=265388434
  message="Tree file open for edit: operation not permitted"
  msgnam="EDITTING"

MDSplusException.statusDict[265388432] = TreeEDITTING


class TreeILLEGAL_ITEM(TreeException):
  status=265388298
  message="Invalid item code or part number specified"
  msgnam="ILLEGAL_ITEM"

MDSplusException.statusDict[265388296] = TreeILLEGAL_ITEM


class TreeILLPAGCNT(TreeException):
  status=265388242
  message="Illegal page count, error mapping tree file"
  msgnam="ILLPAGCNT"

MDSplusException.statusDict[265388240] = TreeILLPAGCNT


class TreeINVDFFCLASS(TreeException):
  status=265388346
  message="Invalid data fmt: only CLASS_S can have data in NCI"
  msgnam="INVDFFCLASS"

MDSplusException.statusDict[265388344] = TreeINVDFFCLASS


class TreeINVDTPUSG(TreeException):
  status=265388426
  message="Attempt to store datatype which conflicts with the designated usage of this node"
  msgnam="INVDTPUSG"

MDSplusException.statusDict[265388424] = TreeINVDTPUSG


class TreeINVPATH(TreeException):
  status=265388290
  message="Invalid tree pathname specified"
  msgnam="INVPATH"

MDSplusException.statusDict[265388288] = TreeINVPATH


class TreeINVRECTYP(TreeException):
  status=265388354
  message="Record type invalid for requested operation"
  msgnam="INVRECTYP"

MDSplusException.statusDict[265388352] = TreeINVRECTYP


class TreeINVTREE(TreeException):
  status=265388226
  message="Invalid tree identification structure"
  msgnam="INVTREE"

MDSplusException.statusDict[265388224] = TreeINVTREE


class TreeMAXOPENEDIT(TreeException):
  status=265388250
  message="Too many files open for edit"
  msgnam="MAXOPENEDIT"

MDSplusException.statusDict[265388248] = TreeMAXOPENEDIT


class TreeNEW(TreeException):
  status=265388059
  message="New tree created"
  msgnam="NEW"

MDSplusException.statusDict[265388056] = TreeNEW


class TreeNMN(TreeException):
  status=265388128
  message="No More Nodes"
  msgnam="NMN"

MDSplusException.statusDict[265388128] = TreeNMN


class TreeNMT(TreeException):
  status=265388136
  message="No More Tags"
  msgnam="NMT"

MDSplusException.statusDict[265388136] = TreeNMT


class TreeNNF(TreeException):
  status=265388144
  message="Node Not Found"
  msgnam="NNF"

MDSplusException.statusDict[265388144] = TreeNNF


class TreeNODATA(TreeException):
  status=265388258
  message="No data available for this node"
  msgnam="NODATA"

MDSplusException.statusDict[265388256] = TreeNODATA


class TreeNODNAMLEN(TreeException):
  status=265388362
  message="Node name too long (12 chars max)"
  msgnam="NODNAMLEN"

MDSplusException.statusDict[265388360] = TreeNODNAMLEN


class TreeNOEDIT(TreeException):
  status=265388274
  message="Tree file is not open for edit"
  msgnam="NOEDIT"

MDSplusException.statusDict[265388272] = TreeNOEDIT


class TreeNOLOG(TreeException):
  status=265388458
  message="Experiment pathname (xxx_path) not defined"
  msgnam="NOLOG"

MDSplusException.statusDict[265388456] = TreeNOLOG


class TreeNOMETHOD(TreeException):
  status=265388208
  message="Method not available for this object"
  msgnam="NOMETHOD"

MDSplusException.statusDict[265388208] = TreeNOMETHOD


class TreeNOOVERWRITE(TreeException):
  status=265388418
  message="Write-once node: overwrite not permitted"
  msgnam="NOOVERWRITE"

MDSplusException.statusDict[265388416] = TreeNOOVERWRITE


class TreeNORMAL(TreeException):
  status=265388041
  message="Normal successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[265388040] = TreeNORMAL


class TreeNOTALLSUBS(TreeException):
  status=265388067
  message="Main tree opened but not all subtrees found/or connected"
  msgnam="NOTALLSUBS"

MDSplusException.statusDict[265388064] = TreeNOTALLSUBS


class TreeNOTCHILDLESS(TreeException):
  status=265388282
  message="Node must be childless to become subtree reference"
  msgnam="NOTCHILDLESS"

MDSplusException.statusDict[265388280] = TreeNOTCHILDLESS


class TreeNOT_IN_LIST(TreeException):
  status=265388482
  message="Tree being opened was not in the list"
  msgnam="NOT_IN_LIST"

MDSplusException.statusDict[265388480] = TreeNOT_IN_LIST


class TreeNOTMEMBERLESS(TreeException):
  status=265388402
  message="Subtree reference can not have members"
  msgnam="NOTMEMBERLESS"

MDSplusException.statusDict[265388400] = TreeNOTMEMBERLESS


class TreeNOTOPEN(TreeException):
  status=265388266
  message="No tree file currently open"
  msgnam="NOTOPEN"

MDSplusException.statusDict[265388264] = TreeNOTOPEN


class TreeNOTSON(TreeException):
  status=265388410
  message="Subtree reference cannot be a member"
  msgnam="NOTSON"

MDSplusException.statusDict[265388408] = TreeNOTSON


class TreeNOT_CONGLOM(TreeException):
  status=265388386
  message="Head node of conglomerate does not contain a DTYPE_CONGLOM record"
  msgnam="NOT_CONGLOM"

MDSplusException.statusDict[265388384] = TreeNOT_CONGLOM


class TreeNOT_OPEN(TreeException):
  status=265388200
  message="Tree not currently open"
  msgnam="NOT_OPEN"

MDSplusException.statusDict[265388200] = TreeNOT_OPEN


class TreeNOWRITEMODEL(TreeException):
  status=265388442
  message="Data for this node can not be written into the MODEL file"
  msgnam="NOWRITEMODEL"

MDSplusException.statusDict[265388440] = TreeNOWRITEMODEL


class TreeNOWRITESHOT(TreeException):
  status=265388450
  message="Data for this node can not be written into the SHOT file"
  msgnam="NOWRITESHOT"

MDSplusException.statusDict[265388448] = TreeNOWRITESHOT


class TreeNO_CONTEXT(TreeException):
  status=265388099
  message="There is no active search to end"
  msgnam="NO_CONTEXT"

MDSplusException.statusDict[265388096] = TreeNO_CONTEXT


class TreeOFF(TreeException):
  status=265388192
  message="Node is OFF"
  msgnam="OFF"

MDSplusException.statusDict[265388192] = TreeOFF


class TreeON(TreeException):
  status=265388107
  message="Node is ON"
  msgnam="ON"

MDSplusException.statusDict[265388104] = TreeON


class TreeOPEN(TreeException):
  status=265388115
  message="Tree is OPEN (no edit)"
  msgnam="OPEN"

MDSplusException.statusDict[265388112] = TreeOPEN


class TreeOPEN_EDIT(TreeException):
  status=265388123
  message="Tree is OPEN for edit"
  msgnam="OPEN_EDIT"

MDSplusException.statusDict[265388120] = TreeOPEN_EDIT


class TreePARENT_OFF(TreeException):
  status=265388176
  message="Parent of this node is OFF"
  msgnam="PARENT_OFF"

MDSplusException.statusDict[265388176] = TreePARENT_OFF


class TreeREADERR(TreeException):
  status=265388474
  message="Error reading record for node"
  msgnam="READERR"

MDSplusException.statusDict[265388472] = TreeREADERR


class TreeREADONLY(TreeException):
  status=265388466
  message="Tree was opened with readonly access"
  msgnam="READONLY"

MDSplusException.statusDict[265388464] = TreeREADONLY


class TreeRESOLVED(TreeException):
  status=265388049
  message="Indirect reference successfully resolved"
  msgnam="RESOLVED"

MDSplusException.statusDict[265388048] = TreeRESOLVED


class TreeSUCCESS(TreeException):
  status=265389633
  message="Operation successful"
  msgnam="SUCCESS"

MDSplusException.statusDict[265389632] = TreeSUCCESS


class TreeTAGNAMLEN(TreeException):
  status=265388370
  message="Tagname too long (max 24 chars)"
  msgnam="TAGNAMLEN"

MDSplusException.statusDict[265388368] = TreeTAGNAMLEN


class TreeTNF(TreeException):
  status=265388152
  message="Tag Not Found"
  msgnam="TNF"

MDSplusException.statusDict[265388152] = TreeTNF


class TreeTREENF(TreeException):
  status=265388160
  message="Tree Not Found"
  msgnam="TREENF"

MDSplusException.statusDict[265388160] = TreeTREENF


class TreeUNRESOLVED(TreeException):
  status=265388338
  message="Not an indirect node reference: No action taken"
  msgnam="UNRESOLVED"

MDSplusException.statusDict[265388336] = TreeUNRESOLVED


class TreeUNSPRTCLASS(TreeException):
  status=265388314
  message="Unsupported descriptor class"
  msgnam="UNSPRTCLASS"

MDSplusException.statusDict[265388312] = TreeUNSPRTCLASS


class TreeUNSUPARRDTYPE(TreeException):
  status=265388394
  message="Complex data types not supported as members of arrays"
  msgnam="UNSUPARRDTYPE"

MDSplusException.statusDict[265388392] = TreeUNSUPARRDTYPE


class TreeWRITEFIRST(TreeException):
  status=265388378
  message="Tree has been modified:  write or quit first"
  msgnam="WRITEFIRST"

MDSplusException.statusDict[265388376] = TreeWRITEFIRST


class TreeFAILURE(TreeException):
  status=265392034
  message="Operation NOT successful"
  msgnam="FAILURE"

MDSplusException.statusDict[265392032] = TreeFAILURE


class TreeLOCK_FAILURE(TreeException):
  status=265392050
  message="Error locking file, perhaps NFSLOCKING not enabled on this system"
  msgnam="LOCK_FAILURE"

MDSplusException.statusDict[265392048] = TreeLOCK_FAILURE


class TreeFILE_NOT_FOUND(TreeException):
  status=265392042
  message="File or Directory Not Found"
  msgnam="FILE_NOT_FOUND"

MDSplusException.statusDict[265392040] = TreeFILE_NOT_FOUND


class TreeCANCEL(TreeException):
  status=265391232
  message="User canceled operation"
  msgnam="CANCEL"

MDSplusException.statusDict[265391232] = TreeCANCEL


class TreeNOSEGMENTS(TreeException):
  status=265392058
  message="No segments exist in this node"
  msgnam="NOSEGMENTS"

MDSplusException.statusDict[265392056] = TreeNOSEGMENTS


class TreeINVDTYPE(TreeException):
  status=265392066
  message="Invalid datatype for data segment"
  msgnam="INVDTYPE"

MDSplusException.statusDict[265392064] = TreeINVDTYPE


class TreeINVSHAPE(TreeException):
  status=265392074
  message="Invalid shape for this data segment"
  msgnam="INVSHAPE"

MDSplusException.statusDict[265392072] = TreeINVSHAPE


class TreeINVSHOT(TreeException):
  status=265392090
  message="Invalid shot number - must be -1 (model), 0 (current), or Positive"
  msgnam="INVSHOT"

MDSplusException.statusDict[265392088] = TreeINVSHOT


class TreeINVTAG(TreeException):
  status=265392106
  message="Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores"
  msgnam="INVTAG"

MDSplusException.statusDict[265392104] = TreeINVTAG


class TreeNOPATH(TreeException):
  status=265392114
  message="No 'treename'_path environment variable defined. Cannot locate tree files."
  msgnam="NOPATH"

MDSplusException.statusDict[265392112] = TreeNOPATH


class TreeTREEFILEREADERR(TreeException):
  status=265392122
  message="Error reading in tree file contents."
  msgnam="TREEFILEREADERR"

MDSplusException.statusDict[265392120] = TreeTREEFILEREADERR


class TreeMEMERR(TreeException):
  status=265392130
  message="Memory allocation error."
  msgnam="MEMERR"

MDSplusException.statusDict[265392128] = TreeMEMERR


class TreeNOCURRENT(TreeException):
  status=265392138
  message="No current shot number set for this tree."
  msgnam="NOCURRENT"

MDSplusException.statusDict[265392136] = TreeNOCURRENT


class TreeFOPENW(TreeException):
  status=265392146
  message="Error opening file for read-write."
  msgnam="FOPENW"

MDSplusException.statusDict[265392144] = TreeFOPENW


class TreeFOPENR(TreeException):
  status=265392154
  message="Error opening file read-only."
  msgnam="FOPENR"

MDSplusException.statusDict[265392152] = TreeFOPENR


class TreeFCREATE(TreeException):
  status=265392162
  message="Error creating new file."
  msgnam="FCREATE"

MDSplusException.statusDict[265392160] = TreeFCREATE


class TreeCONNECTFAIL(TreeException):
  status=265392170
  message="Error connecting to remote server."
  msgnam="CONNECTFAIL"

MDSplusException.statusDict[265392168] = TreeCONNECTFAIL


class TreeNCIWRITE(TreeException):
  status=265392178
  message="Error writing node characterisitics to file."
  msgnam="NCIWRITE"

MDSplusException.statusDict[265392176] = TreeNCIWRITE


class TreeDELFAIL(TreeException):
  status=265392186
  message="Error deleting file."
  msgnam="DELFAIL"

MDSplusException.statusDict[265392184] = TreeDELFAIL


class TreeRENFAIL(TreeException):
  status=265392194
  message="Error renaming file."
  msgnam="RENFAIL"

MDSplusException.statusDict[265392192] = TreeRENFAIL


class TreeEMPTY(TreeException):
  status=265392200
  message="Empty string provided."
  msgnam="EMPTY"

MDSplusException.statusDict[265392200] = TreeEMPTY


class TreePARSEERR(TreeException):
  status=265392210
  message="Invalid node search string."
  msgnam="PARSEERR"

MDSplusException.statusDict[265392208] = TreePARSEERR


class TreeNCIREAD(TreeException):
  status=265392218
  message="Error reading node characteristics from file."
  msgnam="NCIREAD"

MDSplusException.statusDict[265392216] = TreeNCIREAD


class TreeNOVERSION(TreeException):
  status=265392226
  message="No version available."
  msgnam="NOVERSION"

MDSplusException.statusDict[265392224] = TreeNOVERSION


class TreeDFREAD(TreeException):
  status=265392234
  message="Error reading from datafile."
  msgnam="DFREAD"

MDSplusException.statusDict[265392232] = TreeDFREAD


class TreeCLOSEERR(TreeException):
  status=265392242
  message="Error closing temporary tree file."
  msgnam="CLOSEERR"

MDSplusException.statusDict[265392240] = TreeCLOSEERR


class TreeMOVEERROR(TreeException):
  status=265392250
  message="Error replacing original treefile with new one."
  msgnam="MOVEERROR"

MDSplusException.statusDict[265392248] = TreeMOVEERROR


class TreeOPENEDITERR(TreeException):
  status=265392258
  message="Error reopening new treefile for write access."
  msgnam="OPENEDITERR"

MDSplusException.statusDict[265392256] = TreeOPENEDITERR


class TreeREADONLY_TREE(TreeException):
  status=265392266
  message="Tree is marked as readonly. No write operations permitted."
  msgnam="READONLY_TREE"

MDSplusException.statusDict[265392264] = TreeREADONLY_TREE


class TreeWRITETREEERR(TreeException):
  status=265392274
  message="Error writing .tree file"
  msgnam="WRITETREEERR"

MDSplusException.statusDict[265392272] = TreeWRITETREEERR


class TreeNOWILD(TreeException):
  status=265392282
  message="No wildcard characters permitted in node specifier"
  msgnam="NOWILD"

MDSplusException.statusDict[265392280] = TreeNOWILD

########################### generated from mdsshr_messages.xml ########################



class LibException(MDSplusException):
  fac="Lib"


class LibINSVIRMEM(LibException):
  status=1409556
  message="Insufficient virtual memory"
  msgnam="INSVIRMEM"

MDSplusException.statusDict[1409552] = LibINSVIRMEM


class LibINVARG(LibException):
  status=1409588
  message="Invalid argument"
  msgnam="INVARG"

MDSplusException.statusDict[1409584] = LibINVARG


class LibINVSTRDES(LibException):
  status=1409572
  message="Invalid string descriptor"
  msgnam="INVSTRDES"

MDSplusException.statusDict[1409568] = LibINVSTRDES


class LibKEYNOTFOU(LibException):
  status=1409788
  message="Key not found"
  msgnam="KEYNOTFOU"

MDSplusException.statusDict[1409784] = LibKEYNOTFOU


class LibNOTFOU(LibException):
  status=1409652
  message="Entity not found"
  msgnam="NOTFOU"

MDSplusException.statusDict[1409648] = LibNOTFOU


class LibQUEWASEMP(LibException):
  status=1409772
  message="Queue was empty"
  msgnam="QUEWASEMP"

MDSplusException.statusDict[1409768] = LibQUEWASEMP


class LibSTRTRU(LibException):
  status=1409041
  message="String truncated"
  msgnam="STRTRU"

MDSplusException.statusDict[1409040] = LibSTRTRU


class StrException(MDSplusException):
  fac="Str"


class StrMATCH(StrException):
  status=2393113
  message="Strings match"
  msgnam="MATCH"

MDSplusException.statusDict[2393112] = StrMATCH


class StrNOMATCH(StrException):
  status=2392584
  message="Strings do not match"
  msgnam="NOMATCH"

MDSplusException.statusDict[2392584] = StrNOMATCH


class StrNOELEM(StrException):
  status=2392600
  message="Not enough delimited characters"
  msgnam="NOELEM"

MDSplusException.statusDict[2392600] = StrNOELEM


class StrINVDELIM(StrException):
  status=2392592
  message="Not enough delimited characters"
  msgnam="INVDELIM"

MDSplusException.statusDict[2392592] = StrINVDELIM


class StrSTRTOOLON(StrException):
  status=2392180
  message="String too long"
  msgnam="STRTOOLON"

MDSplusException.statusDict[2392176] = StrSTRTOOLON


class MDSplusWARNING(MDSplusException):
  status=65536
  message="Warning"
  msgnam="WARNING"

MDSplusException.statusDict[65536] = MDSplusWARNING


class MDSplusSUCCESS(MDSplusException):
  status=65545
  message="Success"
  msgnam="SUCCESS"

MDSplusException.statusDict[65544] = MDSplusSUCCESS


class MDSplusERROR(MDSplusException):
  status=65554
  message="Error"
  msgnam="ERROR"

MDSplusException.statusDict[65552] = MDSplusERROR


class MDSplusFATAL(MDSplusException):
  status=65572
  message="Fatal"
  msgnam="FATAL"

MDSplusException.statusDict[65568] = MDSplusFATAL


class SsException(MDSplusException):
  fac="Ss"


class SsSUCCESS(SsException):
  status=1
  message="Success"
  msgnam="SUCCESS"

MDSplusException.statusDict[0] = SsSUCCESS


class SsINTOVF(SsException):
  status=1148
  message="Integer overflow"
  msgnam="INTOVF"

MDSplusException.statusDict[1144] = SsINTOVF


class SsINTERNAL(SsException):
  status=-1
  message="This status is meant for internal use only, you should never have seen this message."
  msgnam="INTERNAL"

MDSplusException.statusDict[-8] = SsINTERNAL

########################### generated from tdishr_messages.xml ########################



class TdiException(MDSplusException):
  fac="Tdi"


class TdiBREAK(TdiException):
  status=265519112
  message="BREAK was not in DO FOR SWITCH or WHILE"
  msgnam="BREAK"

MDSplusException.statusDict[265519112] = TdiBREAK


class TdiCASE(TdiException):
  status=265519120
  message="CASE was not in SWITCH statement"
  msgnam="CASE"

MDSplusException.statusDict[265519120] = TdiCASE


class TdiCONTINUE(TdiException):
  status=265519128
  message="CONTINUE was not in DO FOR or WHILE"
  msgnam="CONTINUE"

MDSplusException.statusDict[265519128] = TdiCONTINUE


class TdiEXTRANEOUS(TdiException):
  status=265519136
  message="Some characters were unused, bad number maybe"
  msgnam="EXTRANEOUS"

MDSplusException.statusDict[265519136] = TdiEXTRANEOUS


class TdiRETURN(TdiException):
  status=265519144
  message="Extraneous RETURN statement, not from a FUN"
  msgnam="RETURN"

MDSplusException.statusDict[265519144] = TdiRETURN


class TdiABORT(TdiException):
  status=265519154
  message="Program requested abort"
  msgnam="ABORT"

MDSplusException.statusDict[265519152] = TdiABORT


class TdiBAD_INDEX(TdiException):
  status=265519162
  message="Index or subscript is too small or too big"
  msgnam="BAD_INDEX"

MDSplusException.statusDict[265519160] = TdiBAD_INDEX


class TdiBOMB(TdiException):
  status=265519170
  message="Bad punctuation, could not compile the text"
  msgnam="BOMB"

MDSplusException.statusDict[265519168] = TdiBOMB


class TdiEXTRA_ARG(TdiException):
  status=265519178
  message="Too many arguments for function, watch commas"
  msgnam="EXTRA_ARG"

MDSplusException.statusDict[265519176] = TdiEXTRA_ARG


class TdiGOTO(TdiException):
  status=265519186
  message="GOTO target label not found"
  msgnam="GOTO"

MDSplusException.statusDict[265519184] = TdiGOTO


class TdiINVCLADSC(TdiException):
  status=265519194
  message="Storage class not valid, must be scalar or array"
  msgnam="INVCLADSC"

MDSplusException.statusDict[265519192] = TdiINVCLADSC


class TdiINVCLADTY(TdiException):
  status=265519202
  message="Invalid mixture of storage class and data type"
  msgnam="INVCLADTY"

MDSplusException.statusDict[265519200] = TdiINVCLADTY


class TdiINVDTYDSC(TdiException):
  status=265519210
  message="Storage data type is not valid"
  msgnam="INVDTYDSC"

MDSplusException.statusDict[265519208] = TdiINVDTYDSC


class TdiINV_OPC(TdiException):
  status=265519218
  message="Invalid operator code in a function"
  msgnam="INV_OPC"

MDSplusException.statusDict[265519216] = TdiINV_OPC


class TdiINV_SIZE(TdiException):
  status=265519226
  message="Number of elements does not match declaration"
  msgnam="INV_SIZE"

MDSplusException.statusDict[265519224] = TdiINV_SIZE


class TdiMISMATCH(TdiException):
  status=265519234
  message="Shape of arguments does not match"
  msgnam="MISMATCH"

MDSplusException.statusDict[265519232] = TdiMISMATCH


class TdiMISS_ARG(TdiException):
  status=265519242
  message="Missing argument is required for function"
  msgnam="MISS_ARG"

MDSplusException.statusDict[265519240] = TdiMISS_ARG


class TdiNDIM_OVER(TdiException):
  status=265519250
  message="Number of dimensions is over the allowed 8"
  msgnam="NDIM_OVER"

MDSplusException.statusDict[265519248] = TdiNDIM_OVER


class TdiNO_CMPLX(TdiException):
  status=265519258
  message="There are no complex forms of this function"
  msgnam="NO_CMPLX"

MDSplusException.statusDict[265519256] = TdiNO_CMPLX


class TdiNO_OPC(TdiException):
  status=265519266
  message="No support for this function, today"
  msgnam="NO_OPC"

MDSplusException.statusDict[265519264] = TdiNO_OPC


class TdiNO_OUTPTR(TdiException):
  status=265519274
  message="An output pointer is required"
  msgnam="NO_OUTPTR"

MDSplusException.statusDict[265519272] = TdiNO_OUTPTR


class TdiNO_SELF_PTR(TdiException):
  status=265519282
  message="No $VALUE is defined for signal or validation"
  msgnam="NO_SELF_PTR"

MDSplusException.statusDict[265519280] = TdiNO_SELF_PTR


class TdiNOT_NUMBER(TdiException):
  status=265519290
  message="Value is not a scalar number and must be"
  msgnam="NOT_NUMBER"

MDSplusException.statusDict[265519288] = TdiNOT_NUMBER


class TdiNULL_PTR(TdiException):
  status=265519298
  message="Null pointer where value needed"
  msgnam="NULL_PTR"

MDSplusException.statusDict[265519296] = TdiNULL_PTR


class TdiRECURSIVE(TdiException):
  status=265519306
  message="Overly recursive function, calls itself maybe"
  msgnam="RECURSIVE"

MDSplusException.statusDict[265519304] = TdiRECURSIVE


class TdiSIG_DIM(TdiException):
  status=265519314
  message="Signal dimension does not match data shape"
  msgnam="SIG_DIM"

MDSplusException.statusDict[265519312] = TdiSIG_DIM


class TdiSYNTAX(TdiException):
  status=265519322
  message="Bad punctuation or misspelled word or number"
  msgnam="SYNTAX"

MDSplusException.statusDict[265519320] = TdiSYNTAX


class TdiTOO_BIG(TdiException):
  status=265519330
  message="Conversion of number lost significant digits"
  msgnam="TOO_BIG"

MDSplusException.statusDict[265519328] = TdiTOO_BIG


class TdiUNBALANCE(TdiException):
  status=265519338
  message="Unbalanced () [] {} '' " " or /**/"
  msgnam="UNBALANCE"

MDSplusException.statusDict[265519336] = TdiUNBALANCE


class TdiUNKNOWN_VAR(TdiException):
  status=265519346
  message="Unknown/undefined variable name"
  msgnam="UNKNOWN_VAR"

MDSplusException.statusDict[265519344] = TdiUNKNOWN_VAR


class TdiSTRTOOLON(TdiException):
  status=265519356
  message="string is too long (greater than 65535)"
  msgnam="STRTOOLON"

MDSplusException.statusDict[265519352] = TdiSTRTOOLON


class TdiTIMEOUT(TdiException):
  status=265519364
  message="task did not complete in alotted time"
  msgnam="TIMEOUT"

MDSplusException.statusDict[265519360] = TdiTIMEOUT


class ApdException(MDSplusException):
  fac="Apd"


class ApdAPD_APPEND(ApdException):
  status=266141706
  message="First argument must be APD or *"
  msgnam="APD_APPEND"

MDSplusException.statusDict[266141704] = ApdAPD_APPEND


class ApdDICT_KEYVALPAIR(ApdException):
  status=266141714
  message="A Dictionary requires an even number of elements"
  msgnam="DICT_KEYVALPAIR"

MDSplusException.statusDict[266141712] = ApdDICT_KEYVALPAIR


class ApdDICT_KEYCLS(ApdException):
  status=266141722
  message="Keys must be scalar, i.e. CLASS_S"
  msgnam="DICT_KEYCLS"

MDSplusException.statusDict[266141720] = ApdDICT_KEYCLS

########################### generated from mdsdcl_messages.xml ########################



class MdsdclException(MDSplusException):
  fac="Mdsdcl"


class MdsdclSUCCESS(MdsdclException):
  status=134348809
  message="Normal successful completion"
  msgnam="SUCCESS"

MDSplusException.statusDict[134348808] = MdsdclSUCCESS


class MdsdclEXIT(MdsdclException):
  status=134348817
  message="Normal exit"
  msgnam="EXIT"

MDSplusException.statusDict[134348816] = MdsdclEXIT


class MdsdclERROR(MdsdclException):
  status=134348824
  message="Unsuccessful execution of command"
  msgnam="ERROR"

MDSplusException.statusDict[134348824] = MdsdclERROR


class MdsdclNORMAL(MdsdclException):
  status=134349609
  message="Normal successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[134349608] = MdsdclNORMAL


class MdsdclPRESENT(MdsdclException):
  status=134349617
  message="Entity is present"
  msgnam="PRESENT"

MDSplusException.statusDict[134349616] = MdsdclPRESENT


class MdsdclIVVERB(MdsdclException):
  status=134349626
  message="No such command"
  msgnam="IVVERB"

MDSplusException.statusDict[134349624] = MdsdclIVVERB


class MdsdclABSENT(MdsdclException):
  status=134349632
  message="Entity is absent"
  msgnam="ABSENT"

MDSplusException.statusDict[134349632] = MdsdclABSENT


class MdsdclNEGATED(MdsdclException):
  status=134349640
  message="Entity is present but negated"
  msgnam="NEGATED"

MDSplusException.statusDict[134349640] = MdsdclNEGATED


class MdsdclNOTNEGATABLE(MdsdclException):
  status=134349650
  message="Entity cannot be negated"
  msgnam="NOTNEGATABLE"

MDSplusException.statusDict[134349648] = MdsdclNOTNEGATABLE


class MdsdclIVQUAL(MdsdclException):
  status=134349658
  message="Invalid qualifier"
  msgnam="IVQUAL"

MDSplusException.statusDict[134349656] = MdsdclIVQUAL


class MdsdclPROMPT_MORE(MdsdclException):
  status=134349666
  message="More input required for command"
  msgnam="PROMPT_MORE"

MDSplusException.statusDict[134349664] = MdsdclPROMPT_MORE


class MdsdclTOO_MANY_PRMS(MdsdclException):
  status=134349674
  message="Too many parameters specified"
  msgnam="TOO_MANY_PRMS"

MDSplusException.statusDict[134349672] = MdsdclTOO_MANY_PRMS


class MdsdclTOO_MANY_VALS(MdsdclException):
  status=134349682
  message="Too many values"
  msgnam="TOO_MANY_VALS"

MDSplusException.statusDict[134349680] = MdsdclTOO_MANY_VALS


class MdsdclMISSING_VALUE(MdsdclException):
  status=134349690
  message="Qualifier value needed"
  msgnam="MISSING_VALUE"

MDSplusException.statusDict[134349688] = MdsdclMISSING_VALUE

########################### generated from servershr_messages.xml ########################



class ServerException(MDSplusException):
  fac="Server"


class ServerNOT_DISPATCHED(ServerException):
  status=266436616
  message="action not dispatched, depended on failed action"
  msgnam="NOT_DISPATCHED"

MDSplusException.statusDict[266436616] = ServerNOT_DISPATCHED


class ServerINVALID_DEPENDENCY(ServerException):
  status=266436626
  message="action dependency cannot be evaluated"
  msgnam="INVALID_DEPENDENCY"

MDSplusException.statusDict[266436624] = ServerINVALID_DEPENDENCY


class ServerCANT_HAPPEN(ServerException):
  status=266436634
  message="action contains circular dependency or depends on action which was not dispatched"
  msgnam="CANT_HAPPEN"

MDSplusException.statusDict[266436632] = ServerCANT_HAPPEN


class ServerINVSHOT(ServerException):
  status=266436642
  message="invalid shot number, cannot dispatch actions in model"
  msgnam="INVSHOT"

MDSplusException.statusDict[266436640] = ServerINVSHOT


class ServerABORT(ServerException):
  status=266436658
  message="Server action was aborted"
  msgnam="ABORT"

MDSplusException.statusDict[266436656] = ServerABORT


class ServerPATH_DOWN(ServerException):
  status=266436674
  message="Path to server lost"
  msgnam="PATH_DOWN"

MDSplusException.statusDict[266436672] = ServerPATH_DOWN


class ServerSOCKET_ADDR_ERROR(ServerException):
  status=266436682
  message="Cannot obtain ip address socket is bound to."
  msgnam="SOCKET_ADDR_ERROR"

MDSplusException.statusDict[266436680] = ServerSOCKET_ADDR_ERROR


class ServerINVALID_ACTION_OPERATION(ServerException):
  status=266436690
  message="None"
  msgnam="INVALID_ACTION_OPERATION"

MDSplusException.statusDict[266436688] = ServerINVALID_ACTION_OPERATION

########################### generated from camshr_messages.xml ########################



class CamException(MDSplusException):
  fac="Cam"


class CamDONE_Q(CamException):
  status=134316041
  message="I/O completed with X=1, Q=1"
  msgnam="DONE_Q"

MDSplusException.statusDict[134316040] = CamDONE_Q


class CamDONE_NOQ(CamException):
  status=134316049
  message="I/O completed with X=1, Q=0"
  msgnam="DONE_NOQ"

MDSplusException.statusDict[134316048] = CamDONE_NOQ


class CamDONE_NOX(CamException):
  status=134320128
  message="I/O completed with X=0 - probable failure"
  msgnam="DONE_NOX"

MDSplusException.statusDict[134320128] = CamDONE_NOX


class CamSERTRAERR(CamException):
  status=134322178
  message="serial transmission error on highway"
  msgnam="SERTRAERR"

MDSplusException.statusDict[134322176] = CamSERTRAERR


class CamSCCFAIL(CamException):
  status=134322242
  message="serial crate controller failure"
  msgnam="SCCFAIL"

MDSplusException.statusDict[134322240] = CamSCCFAIL


class CamOFFLINE(CamException):
  status=134322282
  message="crate is offline"
  msgnam="OFFLINE"

MDSplusException.statusDict[134322280] = CamOFFLINE

########################### generated from tcl_messages.xml ########################



class TclException(MDSplusException):
  fac="Tcl"


class TclNORMAL(TclException):
  status=2752521
  message="Normal successful completion"
  msgnam="NORMAL"

MDSplusException.statusDict[2752520] = TclNORMAL


class TclFAILED_ESSENTIAL(TclException):
  status=2752528
  message="Essential action failed"
  msgnam="FAILED_ESSENTIAL"

MDSplusException.statusDict[2752528] = TclFAILED_ESSENTIAL


class TclNO_DISPATCH_TABLE(TclException):
  status=2752536
  message="No dispatch table found. Forgot to do DISPATCH/BUILD?"
  msgnam="NO_DISPATCH_TABLE"

MDSplusException.statusDict[2752536] = TclNO_DISPATCH_TABLE
