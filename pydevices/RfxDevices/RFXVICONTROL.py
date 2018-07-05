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

class RFXVICONTROL(Device):
   parts=[{'path':':COMMENT', 'type':'text'},
      {'path':':FILLING_TYPE', 'type':'numeric', 'value':3},
      {'path':':IMP_LEVEL', 'type':'numeric', 'value':100},
      {'path':':PRESSURE', 'type':'numeric', 'value':0},
      {'path':':CONTROL', 'type':'numeric', 'value':0},
      {'path':':CTRL_START', 'type':'numeric', 'value':0},
      {'path':':NE_SCALE', 'type':'numeric', 'value':0},
      {'path':':FILL_WAVE','type':'signal', 'options':'compress_on_put'},
      {'path':':FILL_MIN_X','type':'numeric', 'value':0},
      {'path':':FILL_MAX_X','type':'numeric', 'value':1},
      {'path':':FILL_MIN_Y','type':'numeric', 'value':0},
      {'path':':FILL_MAX_Y','type':'numeric', 'value':10},
      {'path':':PUFF_WAVE','type':'signal', 'options':'compress_on_put'},
      {'path':':PUFF_MIN_X','type':'numeric', 'value':0},
      {'path':':PUFF_MAX_X','type':'numeric', 'value':1},
      {'path':':PUFF_MIN_Y','type':'numeric', 'value':0},
      {'path':':PUFF_MAX_Y','type':'numeric', 'value':10},
      {'path':':CTRL_WAVE','type':'signal', 'options':'compress_on_put'},
      {'path':':CTRL_MIN_X','type':'numeric', 'value':0},
      {'path':':CTRL_MAX_X','type':'numeric', 'value':1},
      {'path':':CTRL_MIN_Y','type':'numeric', 'value':0},
      {'path':':CTRL_MAX_Y','type':'numeric', 'value':10}]
