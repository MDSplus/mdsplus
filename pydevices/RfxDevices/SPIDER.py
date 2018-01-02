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

from MDSplus import mdsExceptions, Device

class SPIDER(Device):
    """MARTe configuration"""
    parts=[{'path':':COMMENT', 'type':'text'},
      {'path':':CAMERA_FREQ', 'type':'numeric', 'value':10},
      {'path':':CAMERA_START', 'type':'numeric', 'value':0},
      {'path':':CAMERA_DURAT', 'type':'numeric', 'value':60},
      {'path':':CAEN_FREQ', 'type':'numeric', 'value':2},
      {'path':':CAEN_START', 'type':'numeric', 'value':0},
      {'path':':CAEN_DURAT', 'type':'numeric', 'value':30},
      {'path':':NI_FREQ', 'type':'numeric', 'value':10000},
      {'path':':NI_START', 'type':'numeric', 'value':0},
      {'path':':NI_DURAT', 'type':'numeric', 'value':60},
      {'path':':NI6368_FREQ', 'type':'numeric', 'value':10000},
      {'path':':NI6368_START', 'type':'numeric', 'value':0},
      {'path':':NI6368_DURAT', 'type':'numeric', 'value':60},
      {'path':':BREAK_DEAD', 'type':'numeric', 'value':10},
      {'path':':BREAK_REC', 'type':'numeric', 'value':0},
      {'path':'.WAVE_1', 'type':'structure'},
      {'path':'.WAVE_1:WAVE', 'type':'signal'},
      {'path':'.WAVE_1:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_1:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_1:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_1:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_2', 'type':'structure'},
      {'path':'.WAVE_2:WAVE', 'type':'signal'},
      {'path':'.WAVE_2:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_2:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_2:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_2:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_3', 'type':'structure'},
      {'path':'.WAVE_3:WAVE', 'type':'signal'},
      {'path':'.WAVE_3:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_3:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_3:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_3:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_4', 'type':'structure'},
      {'path':'.WAVE_4:WAVE', 'type':'signal'},
      {'path':'.WAVE_4:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_4:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_4:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_4:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_5', 'type':'structure'},
      {'path':'.WAVE_5:WAVE', 'type':'signal'},
      {'path':'.WAVE_5:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_5:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_5:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_5:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_6', 'type':'structure'},
      {'path':'.WAVE_6:WAVE', 'type':'signal'},
      {'path':'.WAVE_6:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_6:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_6:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_6:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_7', 'type':'structure'},
      {'path':'.WAVE_7:WAVE', 'type':'signal'},
      {'path':'.WAVE_7:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_7:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_7:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_7:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_8', 'type':'structure'},
      {'path':'.WAVE_8:WAVE', 'type':'signal'},
      {'path':'.WAVE_8:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_8:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_8:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_8:MAX_Y', 'type':'numeric'},
      {'path':'.WAVE_REC', 'type':'structure'},
      {'path':'.WAVE_REC:WAVE', 'type':'signal'},
      {'path':'.WAVE_REC:MIN_X', 'type':'numeric'},
      {'path':'.WAVE_REC:MAX_X', 'type':'numeric'},
      {'path':'.WAVE_REC:MIN_Y', 'type':'numeric'},
      {'path':'.WAVE_REC:MAX_Y', 'type':'numeric'}]
