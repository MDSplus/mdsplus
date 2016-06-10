def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

_data=_mimport('mdsdata')
_ver=_mimport('version')

class Ident(_data.Data):
    """Reference to MDSplus Ken Variable"""
    def __init__(self,name):
        self.name=_ver.tostr(name)
    def __str__(self):
        return self.name
