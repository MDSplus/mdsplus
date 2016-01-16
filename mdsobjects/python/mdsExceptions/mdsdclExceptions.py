
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     mdsdcl_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class MdsdclException(MDSplusException):
  fac="Mdsdcl"


class MdsdclSUCCESS(MdsdclException):
  status=134348809
  message="Normal successful completion"
  msgnam="SUCCESS"



class MdsdclEXIT(MdsdclException):
  status=134348817
  message="Normal exit"
  msgnam="EXIT"



class MdsdclERROR(MdsdclException):
  status=134348824
  message="Unsuccessful execution of command"
  msgnam="ERROR"



class MdsdclNORMAL(MdsdclException):
  status=134349609
  message="Normal successful completion"
  msgnam="NORMAL"



class MdsdclPRESENT(MdsdclException):
  status=134349617
  message="Entity is present"
  msgnam="PRESENT"



class MdsdclIVVERB(MdsdclException):
  status=134349626
  message="No such command"
  msgnam="IVVERB"



class MdsdclABSENT(MdsdclException):
  status=134349632
  message="Entity is absent"
  msgnam="ABSENT"



class MdsdclNEGATED(MdsdclException):
  status=134349640
  message="Entity is present but negated"
  msgnam="NEGATED"



class MdsdclNOTNEGATABLE(MdsdclException):
  status=134349650
  message="Entity cannot be negated"
  msgnam="NOTNEGATABLE"



class MdsdclIVQUAL(MdsdclException):
  status=134349658
  message="Invalid qualifier"
  msgnam="IVQUAL"



class MdsdclPROMPT_MORE(MdsdclException):
  status=134349666
  message="More input required for command"
  msgnam="PROMPT_MORE"



class MdsdclTOO_MANY_PRMS(MdsdclException):
  status=134349674
  message="Too many parameters specified"
  msgnam="TOO_MANY_PRMS"



class MdsdclTOO_MANY_VALS(MdsdclException):
  status=134349682
  message="Too many values"
  msgnam="TOO_MANY_VALS"



class MdsdclMISSING_VALUE(MdsdclException):
  status=134349690
  message="Qualifier value needed"
  msgnam="MISSING_VALUE"

