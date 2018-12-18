# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

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
    import os
    print("PYTHONPATH was set to: %s and unable to import version information" % os.environ['PYTHONPATH'])
    __version__='Unknown'

import sys,ctypes as _C
class MDSplusVersionInfo(_C.Structure):
    _fields_= [("MAJOR",_C.c_char_p),
               ("MINOR",_C.c_char_p),
               ("RELEASE",_C.c_char_p),
               ("BRANCH",_C.c_char_p),
               ("RELEASE_TAG",_C.c_char_p),
               ("COMMIT",_C.c_char_p),
               ("DATE",_C.c_char_p),
               ("MDSVERSION",_C.c_char_p)]
_ver=_mimport('version')
mdsshr=_ver.load_library('MdsShr')
del _ver
_info=_C.cast(mdsshr.MDSplusVersion,_C.POINTER(MDSplusVersionInfo)).contents
del mdsshr
if _info.MDSVERSION != __version__:
    sys.stderr.write("""Warning:
  The MDSplus python module version (%s) does not match
  the version of the installed MDSplus libraries (%s).
  Upgrade the module using the mdsplus/mdsobjects/python directory of the
  MDSplus installation.
""" % (__version__, _info.MDSVERSION ))

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
        try:   return find_library(libname)
        except:return None
    if not "PyLib" in globals():
        PyLib = getPyLib()
        if   PyLib:globals()['setenv']("PyLib",PyLib)
        else:PyLib=globals()['getenv']("PyLib")
