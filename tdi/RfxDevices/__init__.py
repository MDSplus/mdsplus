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

<<<<<<< HEAD
ACQIPPSETUP = _mimport('ACQIPPSETUP')
CAENDT5720 = _mimport('CAENDT5720')
CAENV1740 = _mimport('CAENV1740')
CONTIPPSETUP = _mimport('CONTIPPSETUP')
CYGNET4K = _mimport('CYGNET4K')
# DIO
DIO4 = _mimport('DIO4')
DIO2_ENCDEC = _mimport('DIO2_ENCDEC')
=======
from CAENV1740 import CAENV1740
from CAENDT5720 import CAENDT5720
from CAENDT5724 import CAENDT5724
from DIO4 import DIO4
from SIS3820 import SIS3820
from FEMTO import FEMTO
from MARTE_GENERIC import MARTE_GENERIC
from MARTE import MARTE
from MARTE_MHD_CTRL import MARTE_MHD_CTRL
from MARTE_MHD_BT import MARTE_MHD_BT
from MARTE_MHD_BR import MARTE_MHD_BR
from MARTE_MHD_I import MARTE_MHD_I
from MARTE_MHD_AC_BC import MARTE_MHD_AC_BC
from MARTE_EDA1 import MARTE_EDA1
from MARTE_EDA3 import MARTE_EDA3
from MARTE_EDA1_OUT import MARTE_EDA1_OUT
from MARTE_EDA3_OUT import MARTE_EDA3_OUT
from MARTE_DEQU import MARTE_DEQU
from MARTE_CONFIG import MARTE_CONFIG
from MARTE_WAVEGEN import MARTE_WAVEGEN
from MARTE_BREAKDOWN import MARTE_BREAKDOWN
from MARTE_RTSM import MARTE_RTSM
from SPIDER_SM import SPIDER_SM
from SPIDER import SPIDER
from ZELOS2150GV import ZELOS2150GV
from FAKECAMERA import FAKECAMERA
from FLIRSC65X import FLIRSC65X
from NI6259AI import NI6259AI
from RFXWAVESETUP import RFXWAVESETUP
from NI6368AI import NI6368AI
from RFX_PROTECTIONS import RFX_PROTECTIONS
from ACQIPPSETUP import ACQIPPSETUP
from CONTIPPSETUP import CONTIPPSETUP
#from MILL3 import MILL3
from DIO2_ENCDEC import DIO2_ENCDEC
from MARTE_COMMON import MARTE_COMMON
from MARTE_DEVICE import MARTE_DEVICE
from MARTE_XRAY import MARTE_XRAY
from MARTE_NE import MARTE_NE
from CYGNET4K import CYGNET4K
from REDPYTADC import REDPYTADC
from RFXVICONTROL import RFXVICONTROL
from MARTE_DUTY_CYCLE import MARTE_DUTY_CYCLE
>>>>>>> fc8b338905a379dbe20ffdc2303641271f6ee218

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
