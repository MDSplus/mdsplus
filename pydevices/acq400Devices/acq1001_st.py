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


import acq400_base


try:
    acq400_hapi = __import__('acq400_hapi', globals(), level=1)
except:
    acq400_hapi = __import__('acq400_hapi', globals())

INPFMT = ':INPUT_%3.3d'

class _ACQ1001_ST(acq400_base._ACQ400_ST_BASE):
    """
    D-Tacq ACQ2106 stream support.

    """
    pass


def assemble(cls):
    cls.parts = list(_ACQ1001_ST.base_parts) + list(_ACQ1001_ST.st_base_parts)
    
    for ch in range(1, cls.nchan+1):
        cls.parts.append({'path':INPFMT%(ch,), 'type':'signal','options':('no_write_model','write_once',),
                          'valueExpr':'head.setChanScale(%d)' %(ch,)})
        cls.parts.append({'path':INPFMT%(ch,)+':DECIMATE', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':COEFFICIENT','type':'NUMERIC', 'value':1, 'options':('no_write_shot')})
        cls.parts.append({'path':INPFMT%(ch,)+':OFFSET', 'type':'NUMERIC', 'value':1, 'options':('no_write_shot')})


chan_combos = [8, 16, 24, 32, 40, 48, 64]
class_ch_dict = {}

for channel_count in chan_combos:
    name_str = "ACQ1001_ST_{}".format(str(channel_count))
    class_ch_dict[name_str] = type(name_str, (_ACQ1001_ST,), {"nchan": channel_count})
    assemble(class_ch_dict[name_str])

for key,val in class_ch_dict.items():
        exec("{} = {}".format(key, "val"))

