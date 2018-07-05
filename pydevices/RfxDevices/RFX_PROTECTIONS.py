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
