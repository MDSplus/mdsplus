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

class SPIDER_SETUP(Device):
     """SPIDER Experiment Setup"""
     parts=[{'path':':COMMENT', 'type':'text'},

      {'path':'.ISEPS', 'type':'structure'},
      {'path':'.ISEPS:CAEN_FREQ', 'type':'numeric', 'value':2},
      {'path':'.ISEPS:CAEN_START', 'type':'numeric', 'value':0},
      {'path':'.ISEPS:CAEN_DURAT', 'type':'numeric', 'value':30},
      {'path':'.ISEPS:NI6259_FREQ', 'type':'numeric', 'value':10000},
      {'path':'.ISEPS:NI6259_START', 'type':'numeric', 'value':0},
      {'path':'.ISEPS:NI6259_DURAT', 'type':'numeric', 'value':60},
      {'path':'.ISEPS:NI6368_FREQ', 'type':'numeric', 'value':10000},
      {'path':'.ISEPS:NI6368_START', 'type':'numeric', 'value':0},
      {'path':'.ISEPS:NI6368_DURAT', 'type':'numeric', 'value':60},
      {'path':'.ISEPS:BREAK_DEAD', 'type':'numeric', 'value':10},
      {'path':'.ISEPS:BREAK_REC', 'type':'numeric', 'value':0}]

     for i in range(0,8):
           parts.append({'path':'.ISEPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.ISEPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.ISEPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_start)'})
           parts.append({'path':'.ISEPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_stop)'})
           parts.append({'path':'.ISEPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.ISEPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.ISEPS.WAVE_REC', 'type':'structure'})
     parts.append({'path':'.ISEPS.WAVE_REC:WAVE', 'type':'signal'})
     parts.append({'path':'.ISEPS.WAVE_REC:MIN_X', 'type':'numeric', 'value': 0})
     parts.append({'path':'.ISEPS.WAVE_REC:MAX_X', 'type':'numeric', 'value': 1})
     parts.append({'path':'.ISEPS.WAVE_REC:MIN_Y', 'type':'numeric'})
     parts.append({'path':'.ISEPS.WAVE_REC:MAX_Y', 'type':'numeric'})


     parts.append({'path':'.GVS', 'type':'structure'})
     parts.append({'path':'.GVS:TRIG_SOURCE', 'type':'numeric'})

     for i in range(0,8):
           parts.append({'path':'.GVS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.GVS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.GVS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_start)'})
           parts.append({'path':'.GVS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_stop)'})
           parts.append({'path':'.GVS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.GVS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.SPIDER', 'type':'structure'})
     parts.append({'path':'.SPIDER:T_START_SP', 'type':'numeric', 'value':0})
     parts.append({'path':'.SPIDER:RT_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.SPIDER:RT_STOP', 'type':'numeric', 'value':1})
     parts.append({'path':'.SPIDER:WREF_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.SPIDER:WREF_STOP', 'type':'numeric', 'value':0})

     parts.append({'path':'.ISEPS:EG_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.ISEPS:EG_MOD_FREQ', 'type':'numeric', 'value':1})
     parts.append({'path':'.ISEPS:EG_MOD_DC', 'type':'numeric', 'value':50})

     parts.append({'path':'.ISEPS:RF_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.ISEPS:RF_MOD_FREQ', 'type':'numeric','value':1})
     parts.append({'path':'.ISEPS:RF_MOD_DC', 'type':'numeric','value':50})

     parts.append({'path':'.ISEPS:FIL_STATE', 'type':'text', 'value':"OFF"})
     parts.append({'path':'.ISEPS:FIL_ON_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.ISEPS:FIL_ON_STOP', 'type':'numeric', 'value':0})
     parts.append({'path':'.SPIDER:RT_FREQ', 'type':'numeric', 'value':1000})
   
     parts.append({'path':'.ISEPS:PG_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.ISEPS:BI_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.ISEPS:BI_CTRL', 'type':'text', 'value':"VOLTAGE"})
     parts.append({'path':'.ISEPS:BP_STATE', 'type':'text', 'value':"DISABLED"})
     parts.append({'path':'.ISEPS:BP_CTRL', 'type':'text', 'value':"VOLTAGE"})

     parts.append({'path':'.AGPS', 'type':'structure'})
     parts.append({'path':'.AGPS:TRIG_SOURCE', 'type':'numeric'})

     parts.append({'path':'.AGPS:CAEN_FREQ', 'type':'numeric', 'value':2})
     parts.append({'path':'.AGPS:CAEN_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.AGPS:CAEN_DURAT', 'type':'numeric', 'value':30})
     parts.append({'path':'.AGPS:NI6259_FREQ', 'type':'numeric', 'value':10000})
     parts.append({'path':'.AGPS:NI6259_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.AGPS:NI6259_DURAT', 'type':'numeric', 'value':60})
     parts.append({'path':'.AGPS:NI6368_FREQ', 'type':'numeric', 'value':10000})
     parts.append({'path':'.AGPS:NI6368_START', 'type':'numeric', 'value':0})
     parts.append({'path':'.AGPS:NI6368_DURAT', 'type':'numeric', 'value':60})
     parts.append({'path':'.AGPS:BREAK_DEAD', 'type':'numeric', 'value':10})
     parts.append({'path':'.AGPS:BREAK_REC', 'type':'numeric', 'value':0})

     parts.append({'path':'.AGPS:AG_MOD_STATE', 'type':'text', 'value':'DISABLED'})
     parts.append({'path':'.AGPS:AG_MOD_FREQ', 'type':'numeric','value':1})
     parts.append({'path':'.AGPS:AG_MOD_DC', 'type':'numeric','value':50})

     for i in range(0,8):
           parts.append({'path':'.AGPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.AGPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.AGPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_start)'})
           parts.append({'path':'.AGPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_stop)'})
           parts.append({'path':'.AGPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.AGPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})

     parts.append({'path':'.AGPS.WAVE_REC', 'type':'structure'})
     parts.append({'path':'.AGPS.WAVE_REC:WAVE', 'type':'signal'})
     parts.append({'path':'.AGPS.WAVE_REC:MIN_X', 'type':'numeric', 'value': 0})
     parts.append({'path':'.AGPS.WAVE_REC:MAX_X', 'type':'numeric', 'value': 1})
     parts.append({'path':'.AGPS.WAVE_REC:MIN_Y', 'type':'numeric'})
     parts.append({'path':'.AGPS.WAVE_REC:MAX_Y', 'type':'numeric'})

     parts.append({'path':'.GVS:VC3001_TURNS', 'type':'numeric', 'value':0})
     parts.append({'path':'.GVS:VC3002_TURNS', 'type':'numeric', 'value':0})

     parts.append({'path':'.SPIDER:TIME_OF_INTE', 'type':'numeric'})


     for i in range(8,20):
           parts.append({'path':'.ISEPS.WAVE_%d'%(i+1), 'type':'structure'})
           parts.append({'path':'.ISEPS.WAVE_%d:WAVE'%(i+1), 'type':'signal'})
           parts.append({'path':'.ISEPS.WAVE_%d:MIN_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_start)'})
           parts.append({'path':'.ISEPS.WAVE_%d:MAX_X'%(i+1), 'type':'numeric', 'valueExpr': 'Data.compile("$1", head.spider_wref_stop)'})
           parts.append({'path':'.ISEPS.WAVE_%d:MIN_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.ISEPS.WAVE_%d:MAX_Y'%(i+1), 'type':'numeric'})
           parts.append({'path':'.ISEPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'ISESP wave %d'%(i+1)})

# Add LABEL field to all WAVE signals
     for i in range(0,8):
           parts.append({'path':'.ISEPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'ISESP wave %d'%(i+1)})
           parts.append({'path':'.GVS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'GVS wave %d'%(i+1)})
           parts.append({'path':'.AGPS.WAVE_%d:LABEL'%(i+1), 'type':'text', 'value':'AGPS wave %d'%(i+1)})
 


     parts.append({'path':'.GVS:PRESET_PRES', 'type':'numeric', 'value':0.2})
     parts.append({'path':'.GVS:TANK'       , 'type':'text'   , 'value':'3000'})
     parts.append({'path':'.GVS:GAS_TYPE'   , 'type':'text'   , 'value':'3090'})

     parts.append({'path':'.ISEPS:ISRF_TE_V'  , 'type':'numeric', 'value':10000})
     parts.append({'path':'.ISEPS:RF_REG_MODE', 'type':'text'   , 'value':'None'})
     parts.append({'path':'.ISEPS:FIL_V_REF'  , 'type':'numeric', 'value':54})
     parts.append({'path':'.ISEPS:CS1_I_REF'  , 'type':'numeric', 'value':0})
     parts.append({'path':'.ISEPS:CS2_I_REF'  , 'type':'numeric', 'value':0})

# Add SAFETY parameters. session and nominal limits in Plasma and Vaccum Pulses

     parts.append({'path':'.SAFETY_PAR', 'type':'structure'})
     parts.append({'path':'.SAFETY_PAR.PLASMA', 'type':'structure'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:AGPS_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:GVS_PRESMIN', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:GVS_PRESMAX', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISEG_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISRF_TE_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISSS_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISPG_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISRF1_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISRF2_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISRF3_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISRF4_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISBI_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISBP_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISBI_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.PLASMA:ISBP_CLIM', 'type':'Numeric'})

     parts.append({'path':'.SAFETY_PAR.VACUUM', 'type':'structure'})
     parts.append({'path':'.SAFETY_PAR.VACUUM:AGPS_VMAX', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.VACUUM:ISRF1_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.VACUUM:ISRF2_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.VACUUM:ISRF3_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.VACUUM:ISRF4_PLIM', 'type':'Numeric'})

     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA', 'type':'structure'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:AGPS_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:GVS_PRESMIN', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:GVS_PRESMAX', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISEG_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISRF_TE_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISSS_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISPG_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISRF1_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISRF2_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISRF3_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISRF4_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISBI_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISBP_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISBI_CLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_PLASMA:ISBP_CLIM', 'type':'Numeric'})

     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM', 'type':'structure'})
     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM:AGPS_VLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM:ISRF1_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM:ISRF2_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM:ISRF3_PLIM', 'type':'Numeric'})
     parts.append({'path':'.SAFETY_PAR.NOM_VACUUM:ISRF4_PLIM', 'type':'Numeric'})

     del(i)


