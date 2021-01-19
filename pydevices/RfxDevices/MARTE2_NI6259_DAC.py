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


@MC.BUILDER('NI6259::NI6259DAC', MC.MARTE2_COMPONENT.MODE_OUTPUT)
class MARTE2_NI6259_DAC(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'DAC0_0', 'type': 'float32', 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                              {'name': 'OutputPolarity', 'type': 'string', 'value': 'Bipolar'},
                                                                              {'name': 'Trigger', 'type': 'int', 'value': 1}]},
        {'name': 'DAC0_1', 'type': 'float32', 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                              {'name': 'OutputPolarity', 'type': 'string', 'value': 'Bipolar'}]},
        {'name': 'DAC0_2', 'type': 'float32', 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                              {'name': 'OutputPolarity', 'type': 'string', 'value': 'Bipolar'}]},
        {'name': 'DAC0_3', 'type': 'float32', 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                              {'name': 'OutputPolarity', 'type': 'string', 'value': 'Bipolar'}]}]
    parameters = [{'name': 'DeviceName', 'type': 'string', 'value': '/dev/pxi6259'},
                  {'name': 'BoardId', 'type': 'int32', 'value': 0},
                  {'name': 'ClockUpdateSource', 'type': 'string', 'value': 'UI_TC'},
                  {'name': 'ClockUpdatePolarity',
                      'type': 'string', 'value': 'RISING_EDGE'},
                  {'name': 'ClockUpdateDivisor', 'type': 'int32', 'value': 10}]
    parts = []
