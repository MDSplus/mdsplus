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


@MC.BUILDER('StreamIn', MC.MARTE2_COMPONENT.MODE_INPUT)
class MARTE2_STREAM_IN_A(MC.MARTE2_COMPONENT):
    outputs = []
    for i in range(128):
        outputs.append(
            {'name': 'Out'+format(i+1, '03d'), 'type': 'float32', 'dimensions': -1, 'parameters': [
                {'name': 'Channel', 'type': 'string', 'value': 'CH'+format(i+1, '03d')}]})
    parameters = [
        {'name': 'NumberOfBuffers', 'type': 'int32', 'value': 100},
        {'name': 'CpuMask', 'type': 'int32', 'value': 127},
        {'name': 'StackSize', 'type': 'int32', 'value': 1000000},
        {'name': 'SynchronizingIdx', 'type': 'int32', 'value': -1}
    ]
    parts = []

    def prepareMarteInfo(self):
        pass
