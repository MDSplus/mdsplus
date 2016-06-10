
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     mdsshr_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class LibException(MDSplusException):
  fac="Lib"


class LibINSVIRMEM(LibException):
  status=1409556
  message="Insufficient virtual memory"
  msgnam="INSVIRMEM"



class LibINVARG(LibException):
  status=1409588
  message="Invalid argument"
  msgnam="INVARG"



class LibINVSTRDES(LibException):
  status=1409572
  message="Invalid string descriptor"
  msgnam="INVSTRDES"



class LibKEYNOTFOU(LibException):
  status=1409788
  message="Key not found"
  msgnam="KEYNOTFOU"



class LibNOTFOU(LibException):
  status=1409652
  message="Entity not found"
  msgnam="NOTFOU"



class LibQUEWASEMP(LibException):
  status=1409772
  message="Queue was empty"
  msgnam="QUEWASEMP"



class LibSTRTRU(LibException):
  status=1409041
  message="String truncated"
  msgnam="STRTRU"



class StrException(MDSplusException):
  fac="Str"


class StrMATCH(StrException):
  status=2393113
  message="Strings match"
  msgnam="MATCH"



class StrNOMATCH(StrException):
  status=2392584
  message="Strings do not match"
  msgnam="NOMATCH"



class StrNOELEM(StrException):
  status=2392600
  message="Not enough delimited characters"
  msgnam="NOELEM"



class StrINVDELIM(StrException):
  status=2392592
  message="Not enough delimited characters"
  msgnam="INVDELIM"



class StrSTRTOOLON(StrException):
  status=2392180
  message="String too long"
  msgnam="STRTOOLON"



class SsException(MDSplusException):
  fac="Ss"


class SsINTOVF(SsException):
  status=1148
  message="Integer overflow"
  msgnam="INTOVF"

