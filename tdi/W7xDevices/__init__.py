"""
W7xDevices
==========

@author: Brian Standley (Max Planck Institute for Plasma Physics)
@copyright: 2015
@license: GNU GPL
"""
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

def _mimport(filename,name=None,local=locals()):
    from MDSplus import Device
    Device._mimport(globals(),local,filename,name)

_mimport('acq425','ACQ425')
_mimport('acq480','ACQ480')
_mimport('bnc845','BNC845')
_mimport('qc9200','QC9200')
