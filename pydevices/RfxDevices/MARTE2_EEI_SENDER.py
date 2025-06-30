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




from MDSplus import Data, Int32Array

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('EEIOut', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_EEI_SENDER(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},        
        {'name': 'Ref_1', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_2', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_3', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_4', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_5', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_6', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_7', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_8', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_9', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_10', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_11', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_12', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_13', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_14', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_15', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_16', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_17', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_18', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_19', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_20', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_21', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_22', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_23', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_24', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_25', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_26', 'type': 'float32', 'dimensions': 0, 'parameters': []},
        {'name': 'Ref_27', 'type': 'float32', 'dimensions': 0, 'parameters': []},
]
    parameters = [
        {'name': 'Ip', 'type': 'string', 'value': 'localhost'},
        {'name': 'Port', 'type': 'int32', 'value': 44488},
        {'name': 'CircuitId', 'type': 'int32', 'value': 1}]
    parts = []

    def prepareMarteInfo(self):
        pass
    
