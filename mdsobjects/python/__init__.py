"""
MDSplus
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL

"""
def _mimport(name, level=1):
    try:
        if not __package__:
            return __import__(name, globals())
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
try:
    @property
    def gub(self):
        return 42
    @gub.setter
    def gub(self,value):
        self._gub=value
    del gub
except:
    raise Exception("Python version 2.6 or higher is now required to use the MDSplus python package.")


try:
    _mvers=_mimport('_version')
    __version__=_mvers.version
    __doc__=__doc__+"Version: %s\nBranch: %s\nCommit: %s\nRelease tag: %s\n" % (_mvers.version,
                                                                              _mvers.branch,
                                                                              _mvers.commit,
                                                                              _mvers.release_tag)
    __doc__=__doc__+"Release: %s\n" % _mvers.release_date
    branch=_mvers.branch
    commit=_mvers.commit
    release_tag=_mvers.release_tag
    del _mvers
except:
    __version__='Unknown'


def load(gbls=globals()):
    def loadmod_full(name,gbls):
        mod=_mimport(name)
        for key in mod.__dict__:
            if not key.startswith('_'):
                gbls[key]=mod.__dict__[key]
    for mod in ('mdsdata','mdsscalar','mdsarray','compound','descriptor',
                'apd','event','tree','scope','_mdsshr',
                'connection','mdsdcl','mdsExceptions'):
        loadmod_full(mod,gbls)
    return gbls

if __name__==__package__:
    load()
    def getPyLib():
        from ctypes.util import find_library
        import sys
        libname = ('python%d%d' if sys.platform.startswith('win') else 'python%d.%d')%sys.version_info[0:2]
        return find_library(libname)
    if not "PyLib" in globals():
        globals()['PyLib'] = getPyLib()
        if   PyLib:setenv("PyLib",PyLib)
        else:PyLib=getenv("PyLib")
