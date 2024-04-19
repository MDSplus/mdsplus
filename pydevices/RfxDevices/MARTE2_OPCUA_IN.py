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


@MC.BUILDER('OPCUADataSource::OPCUADSInput', MC.MARTE2_COMPONENT.MODE_INPUT)
class MARTE2_OPCUA_IN(MC.MARTE2_COMPONENT):
    outputs = []
    for idx in range(128):
        outputs += [{'name': 'Out'+str(idx+1), 'type': 'int32', 'dimensions': -1, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]}]
    parameters = [
        {'name': 'Address', 'type': 'string', 'value': ''},
        {'name': 'Authentication', 'type': 'string', 'value': 'None'},
        {'name': 'ReadMode', 'type': 'string', 'value': 'Read'},
        {'name': 'SamplingTime', 'type': 'int32', 'value': 300},
        {'name': 'Synchronise', 'type': 'string', 'value': 'yes'},
        {'name': 'CpuMask', 'type': 'int32', 'value': 255},
        {'name': 'StackSize', 'type': 'int32', 'value': 10000000},
        {'name': 'Traverse', 'type': 'int32', 'value': 0},
        {'name': 'RootNamespaceIndex', 'type': 'int32', 'value': 1},
        {'name': 'RootIdentifierValue', 'type': 'string', 'value': 'DataBlocksGlobal'},
        {'name': 'RootIdentifierType', 'type': 'string'}]
    parts = []

    def prepareMarteInfo(self):
        self.timebase.putData(Data.compile(
            ' * : * : (build_path("\\'+self.getFullPath()+'.parameters:par_4:value"))'))
