"""
MDSplus
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL



"""
import sys as _sys,os as _os
try:

  if '__package__' not in globals() or __package__ is None or len(__package__)==0:
    def _mimport(name,level):
      return __import__(name,globals())
  else:
    def _mimport(name,level):
      return __import__(name,globals(),{},[],level)

  _mimport('_loadglobals',1).load(globals())

except:
  print("Error importing MDSplus package: %s" % (_sys.exc_info()[1]))


def remove():
  "Remove installed MDSplus package"
  _f=__file__.split(_os.sep)
  while len(_f) > 1 and 'egg' not in _f[-1]:
    _f=_f[:-1]
  packagedir=_os.sep.join(_f)
  if packagedir.endswith('.egg'):
    try:
      import shutil
      shutil.rmtree(packagedir)
    except:
      print("Error removing %s: %s" % (_pkgdir,sys.error_sys()[1]))
