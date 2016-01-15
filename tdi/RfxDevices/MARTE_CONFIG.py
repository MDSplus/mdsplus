# -*- coding: iso-8859-1 -*-
from MDSplus import mdsExceptions, Device

class MARTE_CONFIG(Device):
    """MARTe configuration"""
    parts=[{'path':':COMMENT', 'type':'text'},
      {'path':':CONFIG', 'type':'numeric', 'value':0},
      {'path':':DEC_GAIN', 'type':'numeric', 'value':0},
      {'path':':COMP_RS_GAIN', 'type':'numeric', 'value':0},
      {'path':':KP', 'type':'numeric', 'value':0},
      {'path':':KI', 'type':'numeric', 'value':0},
      {'path':':MIN_IP_CURR', 'type':'numeric', 'value':0},
      {'path':':EQUI_NON_LIN', 'type':'numeric', 'value':0},
      {'path':':FFWD_ON', 'type':'numeric', 'value':0},
      {'path':':FDBK_ON', 'type':'numeric', 'value':0},
      {'path':':VOLT_CONTR', 'type':'numeric', 'value':0},
      {'path':':EQUIFLUX', 'type':'numeric', 'value':0},
      {'path':':STRAIN_AL', 'type':'numeric', 'value':0},
      {'path':':CHK_DISRUP', 'type':'numeric', 'value':0},
      {'path':':VSHIFT_CONTR', 'type':'numeric', 'value':0}]
