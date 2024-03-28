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


@MC.BUILDER(' OPCUADataSource::OPCUADSOutput', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_OPCUA_OUT(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'In1', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In2', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In3', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In4', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In5', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In6', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In7', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In8', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In9', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In10', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In11', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In12', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In13', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In14', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In15', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In16', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In17', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In18', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In19', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In20', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In21', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In22', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In23', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In24', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In25', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In26', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In27', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In28', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In29', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In30', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In31', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
        {'name': 'In32', 'type': 'int32', 'dimensions': 0, 'parameters': [
            {'name': 'NamespaceIndex', 'type':'int32', 'value': 0},
            {'name': 'Path', 'type':'string', 'value': ''} ]},
    ]
    
    parameters = [
        {'name': 'Address', 'type': 'string', 'value': ''},
        {'name': 'Authentication', 'type': 'string', 'value': 'None'},
        {'name': 'UserPawsswordFile', 'type': 'string', 'value': ''},
        {'name': 'Traverse', 'type': 'int32', 'value': 0},
        {'name': 'RootNamespaceIndex', 'type': 'int32', 'value': 1},
        {'name': 'RootIdentifierValue', 'type': 'string', 'value': 'DataBlocksGlobal'},
        {'name': 'RootIdentifierType', 'type': 'string'}]
    parts = []

    def prepareMarteInfo(self):
        pass
       
 