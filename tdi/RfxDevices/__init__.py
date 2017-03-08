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

from MDSplus import Device as _debug
if _debug.debug: 
    from sys import stdout as _stdout
    def _debug(s,p=tuple()):
        _stdout.write(s % p)
else:
    def _debug(s,p=tuple()):
        pass
def _mimport(name):
    _debug('loading %-16s',(name+':'))
    try:
        try:
            module = __import__(name, globals(), fromlist=[name], level=1).__getattribute__(name)
        except:
            module = __import__(name, globals(), fromlist=[name]).__getattribute__(name)
        _debug(' successful\n')
        return module
    except:
        _debug(' failed!\n')

ACQIPPSETUP = _mimport('ACQIPPSETUP')
CAENDT5720 = _mimport('CAENDT5720')
CAENDT5724 = _mimport('CAENDT5724')
CAENV1740 = _mimport('CAENV1740')
CONTIPPSETUP = _mimport('CONTIPPSETUP')
CYGNET4K = _mimport('CYGNET4K')
# DIO
DIO4 = _mimport('DIO4')
DIO2_ENCDEC = _mimport('DIO2_ENCDEC')

FAKECAMERA = _mimport('FAKECAMERA')
FEMTO = _mimport('FEMTO')
FLIRSC65X = _mimport('FLIRSC65X')
MARTE_CONFIG = _mimport('MARTE_CONFIG')
# MARTE_COMMON
MARTE_COMMON = _mimport('MARTE_COMMON')
MARTE_DEVICE = _mimport('MARTE_DEVICE')
MARTE_RTSM = _mimport('MARTE_RTSM')
# MARTE_GENERIC
MARTE_GENERIC = _mimport('MARTE_GENERIC')
MARTE = _mimport('MARTE')
MARTE_BREAKDOWN = _mimport('MARTE_BREAKDOWN')
MARTE_DEQU = _mimport('MARTE_DEQU')
MARTE_EDA1 = _mimport('MARTE_EDA1')
MARTE_EDA1_OUT = _mimport('MARTE_EDA1_OUT')
MARTE_EDA3 = _mimport('MARTE_EDA3')
MARTE_EDA3_OUT = _mimport('MARTE_EDA3_OUT')
MARTE_MHD_AC_BC = _mimport('MARTE_MHD_AC_BC')
MARTE_MHD_BR = _mimport('MARTE_MHD_BR')
MARTE_MHD_BT = _mimport('MARTE_MHD_BT')
MARTE_MHD_CTRL = _mimport('MARTE_MHD_CTRL')
MARTE_MHD_I = _mimport('MARTE_MHD_I')
MARTE_NE = _mimport('MARTE_NE')
MARTE_WAVEGEN = _mimport('MARTE_WAVEGEN')
MARTE_XRAY = _mimport('MARTE_XRAY')

MILL3 = _mimport('MILL3')
NI6259AI = _mimport('NI6259AI')
NI6368AI = _mimport('NI6368AI')
NI6682 = _mimport('NI6682')
REDPYTADC = _mimport('REDPYTADC')
RFX_PROTECTIONS = _mimport('RFX_PROTECTIONS')
RFXVICONTROL = _mimport('RFXVICONTROL')
RFXWAVESETUP = _mimport('RFXWAVESETUP')
SIS3820 = _mimport('SIS3820')
SPIDER = _mimport('SPIDER')
SPIDER_SM = _mimport('SPIDER_SM')
ZELOS2150GV = _mimport('ZELOS2150GV')
print('RfxDevices loaded.')
