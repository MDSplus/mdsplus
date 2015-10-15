from sys import version_info as pyver
if pyver<(2,5):
    def _mimport(name, level=1):
        return __import__(name, globals())
else:
    def _mimport(name, level=1):
        return __import__(name, globals(), level=level)

_data=_mimport('mdsdata')
_ver=_mimport('version')

class Ident(_data.Data):
    """Reference to MDSplus Ken Variable"""
    def __init__(self,name):
        self.name=_ver.tostr(name)
    def __str__(self):
        return self.name
