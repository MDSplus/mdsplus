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
from acq2106_423st import Acq2106_423st
class ACQ2106_423_2ST(Acq2106_423st):
    """
    D-Tacq ACQ2106 with ACQ423 Digitizers (up to 6)  real time streaming support.

    32 Channels * number of slots
    Minimum 2Khz Operation
    24 bits == +-10V

    3 trigger modes:
      Automatic - starts recording on arm
      Soft - starts recording on trigger method (reboot / configuration required to switch )
      Hard - starts recording on hardware trigger input

    Software sample decimation

    Settable segment length in number of samples

    debugging() - is debugging enabled.  Controlled by environment variable DEBUG_DEVICES

    """
    from copy import copy
    parts=copy(Acq2106_423st.carrier_parts)
    sites=2
    for i in range(sites*32):
       parts.append({'path':':INPUT_%3.3d'%(i+1,),
                     'type':'signal',
                     'options':('no_write_model','write_once',),
                     'valueExpr':'head.setChanScale(%d)' %(i+1,)})
       parts.append({'path':':INPUT_%3.3d:DECIMATE'%(i+1,),
                     'type':'NUMERIC', 'valueExpr':'head.def_decimate', 
                     'options':('no_write_shot')})
       parts.append({'path':':INPUT_%3.3d:COEFFICIENT'%(i+1,),
                     'type':'NUMERIC', 
                     'value':1, 
                     'options':('no_write_shot')})
       parts.append({'path':':INPUT_%3.3d:OFFSET'%(i+1,),
                     'type':'NUMERIC', 
                     'value':1, 
                     'options':('no_write_shot')})
