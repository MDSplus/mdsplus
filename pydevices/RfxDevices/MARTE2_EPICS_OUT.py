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


@MC.BUILDER('EPICSCA::EPICSCAOutput', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_EPICS_OUT(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'PV_1', 'type': 'uint32', 'dimensions': 0, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_1'}]},
        {'name': 'PV_2', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_2'}]},
        {'name': 'PV_3', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_3'}]},
        {'name': 'PV_4', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_4'}]},
        {'name': 'PV_5', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_5'}]},
        {'name': 'PV_6', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_6'}]},
        {'name': 'PV_7', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_7'}]},
        {'name': 'PV_8', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_8'}]},
        {'name': 'PV_9', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_9'}]},
        {'name': 'PV_10', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_10'}]},
        {'name': 'PV_11', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_11'}]},
        {'name': 'PV_12', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_12'}]},
        {'name': 'PV_13', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_13'}]},
        {'name': 'PV_14', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_14'}]},
        {'name': 'PV_15', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_15'}]},
        {'name': 'PV_16', 'type': 'uint32', 'dimensions': -1, 'parameters': [
            {'name': 'PVName', 'type': 'string', 'value': 'PV_16'}]},
    ]
    parameters = [
        {'name': 'StackSize', 'type': 'int32', 'value': 1048576},
        {'name': 'CPUs', 'type': 'int32', 'value': 0xFF},
        {'name': 'IgnoreBufferOverrun', 'type': 'int32', 'value': 1},
        {'name': 'NumberOfBuffers', 'type': 'int32', 'value': 10}]
    parts = []
