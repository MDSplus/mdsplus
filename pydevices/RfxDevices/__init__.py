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

_mimport('ACQIPPSETUP')
_mimport('CAENDT5720')
_mimport('CAENDT5724')
_mimport('CAENV1740')
_mimport('CONTIPPSETUP')
_mimport('CYGNET4K')
# DIO
_mimport('DIO4')
_mimport('DIO2_ENCDEC')

_mimport('FAKECAMERA')
_mimport('FEMTO')
_mimport('FLIRSC65X')
_mimport('MARTE_CONFIG')
# MARTE_COMMON
_mimport('MARTE_COMMON')
_mimport('MARTE_DEVICE')
_mimport('MARTE_RTSM')
_mimport('MARTE_DUTY_CYCLE')
# MARTE_GENERIC
_mimport('MARTE_GENERIC')
_mimport('MARTE')
_mimport('MARTE_BREAKDOWN')
_mimport('MARTE_DEQU')
_mimport('MARTE_EDA1')
_mimport('MARTE_EDA1_OUT')
_mimport('MARTE_EDA3')
_mimport('MARTE_EDA3_OUT')
_mimport('MARTE_MHD_AC_BC')
_mimport('MARTE_MHD_BR')
_mimport('MARTE_MHD_BT')
_mimport('MARTE_MHD_CTRL')
_mimport('MARTE_MHD_I')
_mimport('MARTE_NE')
_mimport('MARTE_WAVEGEN')
_mimport('MARTE_XRAY')

_mimport('MILL3')
_mimport('NI6259AI')
_mimport('NI6368AI')
_mimport('NI6682')
_mimport('REDPYTADC')
_mimport('RFX_PROTECTIONS')
_mimport('RFXVICONTROL')
_mimport('RFXWAVESETUP')
_mimport('SIS3820')
_mimport('SPIDER')
_mimport('SPIDER_SM')
_mimport('ZELOS2150GV')
#print('RfxDevices loaded.')
