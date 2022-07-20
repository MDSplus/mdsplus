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


@MC.BUILDER('UDP::UDPSender', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_UDP_SENDER(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'TIMEBASE', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'IN_0', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_1', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_2', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_3', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_4', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_5', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_6', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_7', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_8', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_9', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_10', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_11', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_12', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_13', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_14', 'type': 'uint32', 'dimensions': -1, 'parameters': []},
        {'name': 'IN_15', 'type': 'uint32', 'dimensions': -1, 'parameters': []}]
    parameters = [
        {'name': 'Address', 'type': 'string', 'value': 'localhost'},
        {'name': 'Port', 'type': 'int32', 'value': 44488},
        {'name': 'ExecutionMode', 'type': 'string', 'value': 'RealTimeThread'}]
    parts = []

    def prepareMarteInfo(self):
        self.inputs_timebase_value.putData(self.timebase)
      
    
    
