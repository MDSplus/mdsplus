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


@MC.BUILDER('BaslerCamera', MC.MARTE2_COMPONENT.MODE_INPUT)
class MARTE2_BASLERCAMERA_A(MC.MARTE2_COMPONENT):
    outputs = [
        {'name': 'Frame', 'type': 'uint8', 'dimensions': -1, 'parameters': []},
        {'name': 'Timestamp', 'type': 'uint64', 'dimensions': 0, 'parameters': []},
    ]
    parameters = [
        {'name': 'IpAddress', 'type': 'string', 'value': "224.0.23.159"},
        {'name': 'FPS', 'type': 'float32', 'value': 5.0},
        {'name': 'Width', 'type': 'int64', 'value': 800},
        {'name': 'Height', 'type': 'int64', 'value': 600},
        {'name': 'OffsetX', 'type': 'int64', 'value': 0},
        {'name': 'OffsetY', 'type': 'int64', 'value': 0},
        {'name': 'RawGain', 'type': 'uint32', 'value': 136},
        {'name': 'ExposureTime', 'type': 'float64', 'value': 21110.0}, # microseconds
    ]
    parts = []

    def prepareMarteInfo(self):
        #self.OUTPUTS.FRAME.DIMENSIONS.putData(Data.compile(
        #    '[build_path("\\' + self.getFullPath() + '.parameters:par_3:value) * build_path("\\' + self.getFullPath() + '.parameters:par_4:value)]'
        #))

        # self.timebase.putData(Data.compile(
        #     ' 0 : * : 1.0D / (build_path("\\' + self.getFullPath() + '.parameters:par_2:value))'
        # ))

        # TODO: set Frame dimensions to Data.compile([Width * Height])
        pass
