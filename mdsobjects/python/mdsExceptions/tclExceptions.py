
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     tcl_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

MDSplusException=_mimport('__init__',1).MDSplusException



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
