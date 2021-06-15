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
RfxDevices
==========
@authors: Gabriele Manduchi (IGI Padova)
@copyright: 2008
@license: GNU GPL
=======
"""
import os
import MDSplus


def _mimport(name, level=1):
    try:
        module = __import__(name, globals())
    except:
        module = __import__(name, globals(), level=level)
    for k, v in module.__dict__.items():
        if isinstance(v, type(MDSplus.Device)):
            if issubclass(v, MDSplus.Device):
                globals()[k] = v


try:
    _mvers = _mimport('_version')
    __version__ = _mvers.version
    __doc__ = __doc__+"Version: %s\nBranch: %s\nCommit: %s\nRelease tag: %s\n" % (_mvers.version,
                                                                                  _mvers.branch,
                                                                                  _mvers.commit,
                                                                                  _mvers.release_tag)
    __doc__ = __doc__+"Release: %s\n" % _mvers.release_date
    branch = _mvers.branch
    commit = _mvers.commit
    release_tag = _mvers.release_tag
    del _mvers
except:
    __version__ = 'Unknown'

for filename in os.listdir(os.path.dirname(__file__)):
    if not filename.startswith("_"):
        if filename.endswith('.py'):
            _mimport(filename[:-3])

#print('RfxDevices loaded.')
