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

from MDSplus import Data, TreeNode, TreePath

MC = __import__('MARTE2_COMPONENT', globals())


@MC.BUILDER('MultiSinGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_SIN(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'In', 'type': 'uint32', 'dimensions': 0, 'parameters': []}]
    outputs = [
        {'name': 'Out', 'type': 'uint16',
            'dimensions': Data.compile('[4000]'), 'parameters': []}]
    parameters = [{'name': 'SinAmpl', 'type': 'float32', 'value': 1024},
        {'name': 'SinFreq', 'type': 'float32', 'value': 500},
        {'name': 'SinPhase', 'type': 'float32', 'value': 0},
        {'name': 'SinOff', 'type': 'float32', 'value': 2047},
        {'name': 'CycleTime', 'type': 'float32', 'value': 0.004}]
    parts = []

    def prepareMarteInfo(self):
        pass
