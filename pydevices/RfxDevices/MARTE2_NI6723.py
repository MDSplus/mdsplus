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

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('NI6723', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_NI6723(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'DAC0_0', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_1', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_2', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_3', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_4', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_5', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_6', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_7', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_8', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_9', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_10', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_11', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_12', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_13', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_14', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_15', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_16', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_17', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_18', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_19', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_20', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_21', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_22', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_23', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_24', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_25', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_26', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_27', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_28', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_29', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_30', 'type': 'int16', 'dimensions': 0, 'parameters': []},
        {'name': 'DAC0_31', 'type': 'int16', 'dimensions': 0, 'parameters': []}]
    parameters = [{'name': 'Address', 'type': 'string'}]
    parts = []
