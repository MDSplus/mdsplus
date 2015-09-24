from os.path import dirname as _dirname, basename as _basename
import glob as _g


class MDSplusException(Exception):
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __init__(self,status=None):
    if status is not None:
      self.status=status
    self.severity=self.severities[self.status & 7]

  def __str__(self):
    return "%%%s-%s-%s, %s" % (self.fac,
                               self.severity,
                               self.msgnam,
                               self.message)


_modules=_g.glob(_dirname(__file__)+"/*.py")

if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

for _m in _modules:
  if '__init__' not in _m:
    _m=_basename(_m)[0:-3]
    _m=_mimport(_m,1)
    for _key in _m.__dict__:
      globals()[_key]=_m.__dict__[_key]

_statusDict=dict()


_all=globals()
exception=None

for exception in _all:
  if hasattr(_all[exception],"status"):
    _statusDict[_all[exception].status & -8]=_all[exception]

def statusToException(status):
  if (status & -8) in _statusDict:
    return _statusDict[status & -8]
