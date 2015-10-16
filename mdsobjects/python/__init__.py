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
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

#try:
_mimport('_loadglobals').load(globals())
#except Exception:
#    import sys
#    print('Error importing MDSplus package: %s' % (sys.exc_info()[1],))

def remove():
    import os
    "Remove installed MDSplus package"
    _f=__file__.split(os.sep)
    while len(_f) > 1 and 'egg' not in _f[-1]:
        _f=_f[:-1]
    packagedir = os.sep.join(_f)
    if packagedir.endswith('.egg'):
        try:
            import shutil
            shutil.rmtree(packagedir)
        except Exception:
            import sys
            print("Error removing %s: %s" % (packagedir,sys.exc_info()[1]))
