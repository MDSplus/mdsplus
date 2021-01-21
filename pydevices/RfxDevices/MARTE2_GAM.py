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


@MC.BUILDER('TestGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_GAM(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'Input1', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input2', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input3', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input4', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input5', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input6', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input7', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input8', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input9', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input10', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input11', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input12', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input13', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input14', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input15', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Input16', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
    ]
    outputs = [
        {'name': 'Output1', 'type': 'int32', 'dimensions': 0, 'parameters': {}},
        {'name': 'Output2', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output3', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output4', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output5', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output6', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output7', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output8', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output9', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output10', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output11', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output12', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output13', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output14', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output15', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
        {'name': 'Output16', 'type': 'int32', 'dimensions': -1, 'parameters': {}},
    ]
    parameters = [
        {'name': 'Parameter1', 'type': 'int32'},
        {'name': 'Parameter2', 'type': 'int32'},
        {'name': 'Parameter3', 'type': 'int32'},
        {'name': 'Parameter4', 'type': 'int32'},
        {'name': 'Parameter5', 'type': 'int32'},
        {'name': 'Parameter6', 'type': 'int32'},
        {'name': 'Parameter7', 'type': 'int32'},
        {'name': 'Parameter8', 'type': 'int32'},
        {'name': 'Parameter9', 'type': 'int32'},
        {'name': 'Parameter10', 'type': 'int32'},
        {'name': 'Parameter11', 'type': 'int32'},
        {'name': 'Parameter12', 'type': 'int32'},
        {'name': 'Parameter13', 'type': 'int32'},
        {'name': 'Parameter14', 'type': 'int32'},
        {'name': 'Parameter15', 'type': 'int32'},
        {'name': 'Parameter16', 'type': 'int32'},
    ]
    parts = []
