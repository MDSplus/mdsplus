
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     tcl_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class TclException(MDSplusException):
  fac="Tcl"


class TclNORMAL(TclException):
  status=2752521
  message="Normal successful completion"
  msgnam="NORMAL"



class TclFAILED_ESSENTIAL(TclException):
  status=2752528
  message="Essential action failed"
  msgnam="FAILED_ESSENTIAL"

