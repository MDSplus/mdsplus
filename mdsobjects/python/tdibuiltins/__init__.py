from sys import version_info as pyver
if pyver<(2,5):
    def _mimport(name, level=1):
        return __import__(name, globals())
else:
    def _mimport(name, level=1):
        return __import__(name, globals(), level=level)

def loadBuiltins(module):
    for item in dict(module.__dict__).values():
        try:
            if issubclass(item,Builtin):
                globals()[item.name]=item
        except:
            pass

_builtin=_mimport('builtin')
Builtin=_builtin.Builtin

loadBuiltins(_mimport('constants'))
loadBuiltins(_mimport('mathops'))
loadBuiltins(_mimport('builtins_other'))
Builtin.loadBuiltins(globals())
