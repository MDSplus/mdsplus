if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

def loadBuiltins(module):
    for item in dict(module.__dict__).values():
        try:
            if issubclass(item,Builtin):
                globals()[item.name]=item
        except:
            pass

_builtin=_mimport('builtin',1)
Builtin=_builtin.Builtin

loadBuiltins(_mimport('constants',1))
loadBuiltins(_mimport('mathops',1))
loadBuiltins(_mimport('builtins_other',1))
Builtin.loadBuiltins(globals())
