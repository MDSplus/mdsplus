
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     camshr_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class CamException(MDSplusException):
  fac="Cam"


class CamDONE_Q(CamException):
  status=134316041
  message="I/O completed with X=1, Q=1"
  msgnam="DONE_Q"



class CamDONE_NOQ(CamException):
  status=134316049
  message="I/O completed with X=1, Q=0"
  msgnam="DONE_NOQ"



class CamDONE_NOX(CamException):
  status=134320128
  message="I/O completed with X=0 - probable failure"
  msgnam="DONE_NOX"



class CamSERTRAERR(CamException):
  status=134322178
  message="serial transmission error on highway"
  msgnam="SERTRAERR"



class CamSCCFAIL(CamException):
  status=134322242
  message="serial crate controller failure"
  msgnam="SCCFAIL"



class CamOFFLINE(CamException):
  status=134322282
  message="crate is offline"
  msgnam="OFFLINE"

