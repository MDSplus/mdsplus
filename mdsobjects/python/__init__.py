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
import os,sys,numpy,ctypes,ctypes.util,hashlib # importing required packages
if sys.version_info < (2,6):
    raise Exception("Python version 2.6 or higher is now required to use the MDSplus python package.")
# importing libs for convenience and to early check if we have what we need in place
_ver = _mimport("version")
class libs:
    MdsShr = _ver.load_library('MdsShr')
    TreeShr= _ver.load_library('TreeShr')
    TdiShr = _ver.load_library('TdiShr')
    try:   Mdsdcl = version.load_library('Mdsdcl')
    except:Mdsdcl = None
    try:   MdsIpShr = version.load_library('MdsIpShr')
    except:MdsIpShr = None

# check version
if __name__=="MDSplus":
    version_check = os.getenv("MDSPLUS_VERSION_CHECK","on").lower()
    version_check = not (version_check == "0" or version_check == "off" or version_check == "no")
else:
    version_check = False
try:
    _version = _mimport('_version')
    __version__ = _version.version
    branch      = _version.branch
    commit      = _version.commit
    release_tag = _version.release_tag
    __doc__ = """%s
Version: %s\nBranch: %s\nCommit: %s\nRelease tag: %s
Release: %s
""" % (__doc__,__version__,branch,commit,release_tag,_version.release_date)
except:
    if version_check:
        sys.stderr.write("PYTHONPATH was set to: %s and unable to import version information\n" % os.environ['PYTHONPATH'])
    __version__='Unknown'

if version_check:
    def version_check():
        class MDSplusVersionInfo(ctypes.Structure):
            _fields_= [("MAJOR",ctypes.c_char_p),
                       ("MINOR",ctypes.c_char_p),
                       ("RELEASE",ctypes.c_char_p),
                       ("BRANCH",ctypes.c_char_p),
                       ("RELEASE_TAG",ctypes.c_char_p),
                       ("COMMIT",ctypes.c_char_p),
                       ("DATE",ctypes.c_char_p),
                       ("MDSVERSION",ctypes.c_char_p)]
        try:
            _info=ctypes.cast(libs.MdsShr.MDSplusVersion,ctypes.POINTER(MDSplusVersionInfo)).contents
            _ver = version.tostr(_info.MDSVERSION.decode())
        except:
            _ver = "Unknown"
        if _ver != __version__ or _ver == "Unknown":
            sys.stderr.write('''Warning:
  The MDSplus python module version (%s) does not match
  the version of the installed MDSplus libraries (%s).
  Upgrade the module using the mdsplus/mdsobjects/python directory of the
  MDSplus installation.
''' % (__version__, _ver ))
    version_check()
del version_check, _ver

def load_package(gbls={},version_check=False):
    def loadmod_full(name,gbls):
        mod=_mimport(name)
        for key in mod.__dict__:
            if not key.startswith('_'):
                gbls[key]=mod.__dict__[key]
    for name in ('os','sys','numpy','ctypes','libs','__version__'):
        gbls[name] = globals()[name]
    loadmod_full('version',gbls)
    loadmod_full('mdsdata',gbls)
    loadmod_full('mdsscalar',gbls)
    loadmod_full('mdsarray',gbls)
    loadmod_full('compound',gbls)
    loadmod_full('descriptor',gbls)
    loadmod_full('apd',gbls)
    loadmod_full('event',gbls)
    loadmod_full('tree',gbls)
    loadmod_full('scope',gbls)
    loadmod_full('_mdsshr',gbls)
    loadmod_full('mdsExceptions',gbls)
    loadmod_full('mdsdcl',gbls)
    if libs.MdsIpShr is not None:
        loadmod_full('connection',gbls)
    gbls["PyLib"] = os.getenv("PyLib")
    return gbls

if __name__==__package__:
    def PyLib():
        name = ('python%d%d' if sys.platform.startswith('win') else 'python%d.%d')%sys.version_info[0:2]
        try:    lib = ctypes.util.find_library(name)
        except:	lib = None
        if lib is None:
            lib = os.getenv("PyLib",None)
            if lib is not None:
                return lib
            lib = name
        libs.MdsShr.MdsPutEnv(version.tobytes("%s=%s"%("PyLib",lib)))
        return lib
    PyLib = PyLib()
    load_package(globals(),True)
    try:    _mimport("magic") # load ipython magic
    except: pass
    del load_package
