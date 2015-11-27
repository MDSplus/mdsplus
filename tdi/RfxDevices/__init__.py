"""
RfxDevices
==========
@authors: Gabriele Manduchi (IGI Padova)
@copyright: 2008
@license: GNU GPL
=======
"""
def _mimport(name):
    try:
        return __import__(name, globals(), fromlist=[name], level=1).__getattribute__(name)
    except:
        return __import__(name, globals(), fromlist=[name]).__getattribute__(name)

ACQIPPSETUP = _mimport('ACQIPPSETUP')
CAENDT5720 = _mimport('CAENDT5720')
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
