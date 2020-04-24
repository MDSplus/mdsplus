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

from MDSplus import *
from MARTE2_COMPONENT import *
class MARTE2_STREAM_IN(MARTE2_COMPONENT):
    MARTE2_COMPONENT.outputs = [{'name': 'Out1', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'Channel', 'type':'string','value':'CH1'}]},
{'name': 'Out2', 'type': 'float32', 'dimensions': -1, 'parameters':[{'name':'Channel', 'type':'string','value':'CH2'}]},
{'name': 'Out3', 'type': 'float32', 'dimensions': -1, 'parameters':[{'name':'Channel', 'type':'string','value':'CH3'}]},
{'name': 'Out4', 'type': 'float32', 'dimensions': -1, 'parameters':[{'name':'Channel', 'type':'string','value':'CH4'}]},
{'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters':[]}]

    MARTE2_COMPONENT.parameters = [{'name':'NumberOfBuffers', 'type': 'int32', 'value':100}, 
                                   {'name':'CpuMask', 'type': 'int32', 'value': 127},
                                   {'name':'StackSize', 'type': 'int32', 'value': 1000000},
                                   {'name':'SynchronizingIdx', 'type': 'int32', 'value' : 0},
                                   {'name':'Period', 'type': 'float32', 'value' : 1E-2}]

    parts = []
    MARTE2_COMPONENT.buildGam(parts, 'StreamIn', MARTE2_COMPONENT.MODE_SYNCH_INPUT)

    def prepareMarteInfo(self):
        self.timebase.putData(Data.compile('0:1000000 : (build_path("\\'+self.getFullPath()+'.parameters:par_5:value"))'))

