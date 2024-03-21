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

class MITICA_SETUP(Device):
     """MITICA Experiment Setup"""
     parts=[{'path':':COMMENT', 'type':'text'},

      {'path':'.MISEPS', 'type':'structure'},
      {'path':'.MISEPS:CAEN_FREQ', 'type':'numeric', 'value':2},
      {'path':'.MISEPS:CAEN_START', 'type':'numeric', 'value':0},
      {'path':'.MISEPS:CAEN_DURAT', 'type':'numeric', 'value':30},
      {'path':'.MISEPS:NI6259_FREQ', 'type':'numeric', 'value':10000},
      {'path':'.MISEPS:NI6259_START', 'type':'numeric', 'value':0},
      {'path':'.MISEPS:NI6259_DURAT', 'type':'numeric', 'value':60},
      {'path':'.MISEPS:NI6368_FREQ', 'type':'numeric', 'value':10000},
      {'path':'.MISEPS:NI6368_START', 'type':'numeric', 'value':0},
      {'path':'.MISEPS:NI6368_DURAT', 'type':'numeric', 'value':60},
      {'path':'.MISEPS:BREAK_DEAD', 'type':'numeric', 'value':10},
      {'path':'.MISEPS:BREAK_REC', 'type':'numeric', 'value':0}]

     for i in range(0,8):
           parts.append({'path':'.MISEPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.MISEPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.MISEPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.MISEPS.WAVE_REC', 'type':'structure'})
     parts.append({'path':'.MISEPS.WAVE_REC:WAVE', 'type':'signal'})
     parts.append({'path':'.MISEPS.WAVE_REC:MIN_X', 'type':'numeric'})
     parts.append({'path':'.MISEPS.WAVE_REC:MAX_X', 'type':'numeric'})
     parts.append({'path':'.MISEPS.WAVE_REC:MIN_Y', 'type':'numeric'})
     parts.append({'path':'.MISEPS.WAVE_REC:MAX_Y', 'type':'numeric'})


     parts.append({'path':'.MGVS', 'type':'structure'})
     parts.append({'path':'.MGVS:TRIG_SOURCE', 'type':'numeric'})

     for i in range(0,8):
           parts.append({'path':'.MGVS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.MGVS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.MGVS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.MITICA', 'type':'structure'})
     parts.append({'path':'.MITICA:T_START_SP', 'type':'numeric', 'value':0})
     parts.append({'path':'.MITICA:RT_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MITICA:RT_STOP', 'type':'numeric', 'value':1})
     parts.append({'path':'.MITICA:WREF_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MITICA:WREF_STOP', 'type':'numeric', 'value':0})

     parts.append({'path':'.MISEPS:EG_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.MISEPS:EG_MOD_FREQ', 'type':'numeric', 'value':1})
     parts.append({'path':'.MISEPS:EG_MOD_DC', 'type':'numeric', 'value':50})

     parts.append({'path':'.MISEPS:RF_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.MISEPS:RF_MOD_FREQ', 'type':'numeric','value':1})
     parts.append({'path':'.MISEPS:RF_MOD_DC', 'type':'numeric','value':50})

     parts.append({'path':'.MISEPS:FIL_STATE', 'type':'text', 'value':"OFF"})
     parts.append({'path':'.MISEPS:FIL_ON_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MISEPS:FIL_ON_STOP', 'type':'numeric', 'value':0})
     parts.append({'path':'.MITICA:RT_FREQ', 'type':'numeric', 'value':1000})
   
     parts.append({'path':'.MISEPS:PG_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.MISEPS:BI_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.MISEPS:BI_CTRL', 'type':'text', 'value':"VOLTAGE"})
     parts.append({'path':'.MISEPS:BP_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.MISEPS:BP_CTRL', 'type':'text', 'value':"VOLTAGE"})

     parts.append({'path':'.MAGPS', 'type':'structure'})
     parts.append({'path':'.MAGPS:TRIG_SOURCE', 'type':'numeric'})

     parts.append({'path':'.MAGPS:CAEN_FREQ', 'type':'numeric', 'value':2})
     parts.append({'path':'.MAGPS:CAEN_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MAGPS:CAEN_DURAT', 'type':'numeric', 'value':30})
     parts.append({'path':'.MAGPS:NI6259_FREQ', 'type':'numeric', 'value':10000})
     parts.append({'path':'.MAGPS:NI6259_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MAGPS:NI6259_DURAT', 'type':'numeric', 'value':60})
     parts.append({'path':'.MAGPS:NI6368_FREQ', 'type':'numeric', 'value':10000})
     parts.append({'path':'.MAGPS:NI6368_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.MAGPS:NI6368_DURAT', 'type':'numeric', 'value':60})
     parts.append({'path':'.MAGPS:BREAK_DEAD', 'type':'numeric', 'value':10})
     parts.append({'path':'.MAGPS:BREAK_REC', 'type':'numeric', 'value':0})

     parts.append({'path':'.MAGPS:AG_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.MAGPS:AG_MOD_FREQ', 'type':'numeric','value':1})
     parts.append({'path':'.MAGPS:AG_MOD_DC', 'type':'numeric','value':50})

     for i in range(0,8):
           parts.append({'path':'.MAGPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.MAGPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.MAGPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MAGPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MAGPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MAGPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.MAGPS.WAVE_REC', 'type':'structure'})
     parts.append({'path':'.MAGPS.WAVE_REC:WAVE', 'type':'signal'})
     parts.append({'path':'.MAGPS.WAVE_REC:MIN_X', 'type':'numeric'})
     parts.append({'path':'.MAGPS.WAVE_REC:MAX_X', 'type':'numeric'})
     parts.append({'path':'.MAGPS.WAVE_REC:MIN_Y', 'type':'numeric'})
     parts.append({'path':'.MAGPS.WAVE_REC:MAX_Y', 'type':'numeric'})

     parts.append({'path':'.MGVS:VC5001_TURNS', 'type':'numeric', 'value':0})
     parts.append({'path':'.MGVS:VC5002_TURNS', 'type':'numeric', 'value':0})

     parts.append({'path':'.MITICA:TIME_OF_INTE', 'type':'numeric'})


     for i in range(8,20):
           parts.append({'path':'.MISEPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.MISEPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.MISEPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MISEPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'ISESP wave %d'%(i+1)})

# Add LABEL field to all WAVE signals
     for i in range(0,8):
           parts.append({'path':'.MISEPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'ISESP wave %d'%(i+1)})
           parts.append({'path':'.MGVS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'MGVS wave %d'%(i+1)})
           parts.append({'path':'.MAGPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'MAGPS wave %d'%(i+1)})
 
     parts.append({'path':'.MGVS:GISB_PSP', 'type':'numeric', 'value':0.2})
     parts.append({'path':'.MGVS:GISB_TANK'       , 'type':'text'   , 'value':'3000'})
     parts.append({'path':'.MGVS:GISB_GAS_TYP'   , 'type':'text'   , 'value':'H2'})

     parts.append({'path':'.MISEPS:ISRF_TE_V'  , 'type':'numeric', 'value':10000})
     parts.append({'path':'.MISEPS:RF_REG_MODE', 'type':'text'   , 'value':'None'})
     parts.append({'path':'.MISEPS:FIL_V_REF'  , 'type':'numeric', 'value':54})
     parts.append({'path':'.MISEPS:CS1_I_REF'  , 'type':'numeric', 'value':0})
     parts.append({'path':'.MISEPS:CS2_I_REF'  , 'type':'numeric', 'value':0})

     parts.append({'path':'.MGVS:VC5003_TURNS', 'type':'numeric', 'value':0})
     parts.append({'path':'.MGVS:GISC_TANK'       , 'type':'text'   , 'value':'3000'})
     parts.append({'path':'.MGVS:GISC_GAS_TYP'   , 'type':'text'   , 'value':'H2'})
     parts.append({'path':'.MGVS:GISC_PSP', 'type':'numeric', 'value':0.2})


     for i in range(8,16):
           parts.append({'path':'.MGVS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.MGVS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.MGVS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.MGVS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'MGVS wave %d'%(i+1)})



     del(i)


