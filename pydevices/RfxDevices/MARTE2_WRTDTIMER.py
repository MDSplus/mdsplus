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

from MDSplus import Data

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('WRTDTimer', MC.MARTE2_COMPONENT.MODE_SYNCH_INPUT)
class MARTE2_WRTDTIMER(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Counter', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Time', 'type': 'int64', 'dimensions': 0, 'parameters': []},
        {'name': 'OutTrigger', 'type': 'uint8', 'dimensions': 0, 'parameters': []},
        {'name': 'AbsoluteTime', 'type': 'uint64', 'dimensions': -1, 'parameters': []},
        
    ]
    parameters = [
        {'name': 'ClockName', 'type': 'string', 'value': 'CLOCK_REALTIME'},
        {'name': 'CpuMask', 'type': 'int32', 'value': 127},
        {'name': 'UdpPort', 'type': 'uint32', 'value': 5044},
        {'name': 'EventName', 'type': 'string', 'value': "*"},
        {'name': 'MulticastGroup', 'type': 'string', 'value': "224.0.23.159"},
        {'name': 'LeapSeconds', 'type': 'int32', 'value': 0},
        {'name': 'Delay', 'type': 'float64', 'value': 0},
        {'name': 'Phase', 'type': 'float64', 'value': 0},
        {'name': 'Period', 'type': 'float64', 'value': 1E-1},
    ]
    parts = []

    def prepareMarteInfo(self):
        self.timebase.putData(Data.compile(
            ' (build_path("\\'+self.getFullPath()+'.parameters:par_7:value"))+(build_path("\\'+self.getFullPath()+'.parameters:par_8:value")): * : (build_path("\\'+self.getFullPath()+'.parameters:par_9:value"))'))
