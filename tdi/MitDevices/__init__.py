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

def _mimport(filename,name=None,local=locals()):
    from MDSplus import Device
    Device._mimport(globals(),local,filename,name)

_mimport('Dt200WriteMaster')
_mimport('acq132','ACQ132')
_mimport('acq196','ACQ196')
_mimport('acq196ao','ACQ196AO')
_mimport('acq200','ACQ200')
_mimport('acq216','ACQ216')
_mimport('acq216_ftp','ACQ216_FTP')

_mimport('a3248','A3248')
_mimport('cp7452','CP7452')

_mimport('dtao32','DTAO32')
_mimport('dtdo32','DTDO32')

_mimport('dt100','DT100')
_mimport('dt132','DT132')
_mimport('dt216b','DT216B')
_mimport('dt196','DT196')
_mimport('dt196a','DT196A')
_mimport('dt196b','DT196B')
_mimport('dt216','DT216')
_mimport('dt216a','DT216A')

_mimport('raspicam','RASPICAM')
_mimport('picam','PICAM')

_mimport('timeSignature',['timeSignature','timeSignatureAnalyze'])
