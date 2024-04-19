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


@MC.BUILDER('PiecewiseLinearGAM', MC.MARTE2_COMPONENT.MODE_GAM)
class MARTE2_RAMP(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'Time', 'type': 'uint32', 'dimensions': 0, 'parameters': []},
        {'name': 'Enable', 'type': 'uint8', 'dimensions': 0, 'parameters': []},
        {'name': 'SoftLandingTrigger', 'type': 'uint8', 'dimensions': 0, 'parameters': []}]
    outputs = [
        {'name': 'ramp', 'type': 'float64', 'dimensions':0, 'parameters': []}]
    parameters = [
        {'name': 'InitialValue', 'type': 'float64'}, 
        {'name': 'SetPoint', 'type': 'float64'}, 
        {'name': 'Vertices', 'type': 'float64'}, 
        {'name': 'Slopes', 'type': 'float64'}, 
        {'name': 'SetPointScaling', 'type': 'float64'}, 
        {'name': 'SlopeScaling', 'type': 'float64'}, 
        {'name': 'TimeScaling', 'type': 'float64'}, 
        {'name': 'SoftLandingSlope', 'type': 'float64'}
    ]

    parts = []

    def prepareMarteInfo(self):
        pass