"""
MitDevices
==========

@authors: Tom Fredian(MIT/USA), Josh Stillerman(MIT/USA)
@copyright: 2008
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

from Dt200WriteMaster import Dt200WriteMaster
from acq132     import ACQ132
from acq196     import ACQ196
from acq196ao   import ACQ196AO
from acq216     import ACQ216
from acq216_ftp import ACQ216_FTP
from cp7452     import CP7452
from dtao32     import DTAO32
from dtdo32     import DTDO32
from dt132      import DT132
from dt216b     import DT216B
from dt196      import DT196
from dt196a     import DT196A
from dt196b     import DT196B
from dt216      import DT216
from dt216a     import DT216A
from raspicam	import RASPICAM
from picam      import PICAM
from timeSignature import timeSignatureAnalyze, timeSignature
import acq200
import transport
import dt100
