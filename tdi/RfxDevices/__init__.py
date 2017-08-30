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
_mimport('BASLERACA')
_mimport('CAENDT5720')
_mimport('CAENDT5724')
_mimport('CAENV1740')
_mimport('CONTIPPSETUP')
_mimport('CYGNET4K')

_mimport('SPIDER_TIMING')
_mimport('CRIO_FAU')
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

