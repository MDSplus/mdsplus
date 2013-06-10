# -*- coding: iso-8859-1 -*-
from MDSplus import *
import time

class RFX_PROTECTIONS(Device):
    print 'RFX_PROTECTIONS'
    """RFX Machine Protections configuration"""
    parts=[{'path':':COMMENT', 'type':'text'},
      {'path':':MAX_BR_HOR_W', 'type':'numeric'},
      {'path':':MAX_BR_HOR_F', 'type':'numeric'},
      {'path':':MAX_BR_VER_W', 'type':'numeric'},
      {'path':':MAX_BR_VER_F', 'type':'numeric'},
      {'path':':MAX_BR_TIM_W', 'type':'numeric'},
      {'path':':MAX_BR_TIM_F', 'type':'numeric'},
      {'path':':MAX_PR_CURR', 'type':'numeric'},
      {'path':':MAX_PR_I2T', 'type':'numeric'},
      {'path':':MAX_TOR_I2T', 'type':'numeric'}]
    