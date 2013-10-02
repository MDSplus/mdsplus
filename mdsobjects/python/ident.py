if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_data=_mimport('mdsdata',1)

class Ident(_data.Data):
    """Reference to MDSplus Ken Variable"""
    def __init__(self,name):
        self.name=str(name)
    def __str__(self):
        return self.name

