"""
MDSplus
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL

"""
import sys as _sys
try:
    if '__package__' not in globals() or __package__ is None or len(__package__)==0:
        def _mimport(name):
            return __import__(name,globals())
    else:
        def _mimport(name):
            return __import__(name,globals(),{},[],1)

    _mimport('_loadglobals').load(globals())
except Exception:
    print('Error importing MDSplus package: %s' % (_sys.exc_info()[1],))

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
      print("Error removing %s: %s" % (packagedir,_sys.exc_info()[1]))
