"""
MDSplus
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL

"""
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

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

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



#try:
_mimport('_loadglobals').load(globals())
#except Exception as exc:
#    print('Error importing MDSplus package: %s' % (exc,))

TdiCompile=globals()['Data'].compile
TdiEvaluate=globals()['Data'].evaluate
TdiExecute=globals()['Data'].execute

def _remove():
    "Remove installed MDSplus package"
    import os

    def _findPackageDir():
        _f=__file__.split(os.sep)
        while len(_f) > 1 and _f[-1] != 'MDSplus':
            _f=_f[:-1]
        if _f[-1] != 'MDSplus':
            return None
        if 'egg' in _f[-2]:
            _f=_f[:-1]
        return os.sep.join(_f)

    packagedir=_findPackageDir()
    try:
        import shutil
        shutil.rmtree(packagedir)
    except Exception as exc:
        print("Error removing %s: %s" % (packagedir,exc))



