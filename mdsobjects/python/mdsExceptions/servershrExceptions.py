
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     servershr_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class ServerException(MDSplusException):
  fac="Server"


class ServerNOT_DISPATCHED(ServerException):
  status=266436616
  message="action not dispatched, depended on failed action"
  msgnam="NOT_DISPATCHED"



class ServerINVALID_DEPENDENCY(ServerException):
  status=266436626
  message="action dependency cannot be evaluated"
  msgnam="INVALID_DEPENDENCY"



class ServerCANT_HAPPEN(ServerException):
  status=266436634
  message="action contains circular dependency or depends on action which was not dispatched"
  msgnam="CANT_HAPPEN"



class ServerINVSHOT(ServerException):
  status=266436642
  message="invalid shot number, cannot dispatch actions in model"
  msgnam="INVSHOT"



class ServerABORT(ServerException):
  status=266436658
  message="Server action was aborted"
  msgnam="ABORT"



class ServerPATH_DOWN(ServerException):
  status=266436674
  message="Path to server lost"
  msgnam="PATH_DOWN"



class ServerSOCKET_ADDR_ERROR(ServerException):
  status=266436682
  message="Cannot obtain ip address socket is bound to."
  msgnam="SOCKET_ADDR_ERROR"



class ServerINVALID_ACTION_OPERATION(ServerException):
  status=266436690
  message="None"
  msgnam="INVALID_ACTION_OPERATION"

