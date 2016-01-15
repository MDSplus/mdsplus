# -*- coding: iso-8859-1 -*-
from MDSplus import mdsExceptions, Device

class RFX_PROTECTIONS(Device):
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
      {'path':':MAX_TOR_I2T', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T', 'type':'structure'},
      {'path':'.FCOIL_R_I2T:I2T_R1', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R2', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R3', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R4', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R5', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R6', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R7', 'type':'numeric'},
      {'path':'.FCOIL_R_I2T:I2T_R8', 'type':'numeric'}]
