def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

from os.path import dirname as _dirname, basename as _basename
import glob as _g


class MDSplusException(Exception):
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __init__(self,status=None):
    if isinstance(status,int):
      self.status=status
    if not hasattr(self,'status'):
      self.status=-2
      self.msgnam='Unknown'
      self.message='Unknown exception'
      self.fac='MDSplus'
    if isinstance(status,str):
      self.message=status
    self.severity=self.severities[self.status & 7]

  def __str__(self):
    return "%%%s-%s-%s, %s" % (self.fac.upper(),
                               self.severity,
                               self.msgnam,
                               self.message)

for _m in _g.glob(_dirname(__file__)+"/*.py"):
  if '__init__' not in _m:
    _m=_basename(_m)[0:-3]
    _m=_mimport(_m)
    for _key in _m.__dict__:
      globals()[_key]=_m.__dict__[_key]

_statusDict=dict()
exception,code = None,None
for exception in globals().values():
  if hasattr(exception,'status'):
    code = exception.status & -8
    if not code==0: _statusDict[code]=exception
del(exception,code)

class MDSplusError(MDSplusException):
  fac="MDSplus"
  severity="E"
  msgnam="Error"
  message="Failure to complete operation"
  status=-8|2  # serverity E
  def __init__(*args): pass

class MDSplusSuccess(MDSplusException):
  fac="MDSplus"
  severity="S"
  msgnam="Success"
  message="Successful execution"
  status=1
  def __init__(*args): pass

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
  status = int(status)
  if (status & -8) in _statusDict:
    return _statusDict[status & -8](status)
  elif status == MDSplusError.status:
    return MDSplusError()
  elif status == MDSplusSuccess.status:
    return MDSplusSuccess()
  else:
    return MDSplusUnknown(status)
