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


@MC.BUILDER('AD20xxADC::AD20xxDAC', MC.MARTE2_COMPONENT.MODE_SYNCH_OUTPUT)
class MARTE2_AD20XXDAC(MC.MARTE2_COMPONENT):
    inputs = [
        {'name': 'DAC0_0', 'type': 'uint16', 'samples': 4000, 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 0},
                                                                                {'name': 'OutputRange', 'type': 'float32', 'value': 10}]},
        {'name': 'DAC0_1', 'type': 'uint16', 'samples': 4000, 'dimensions': 0, 'parameters': [{'name': 'ChannelId', 'type': 'int', 'value': 1},
                                                                                {'name': 'OutputRange', 'type': 'float32', 'value': 10}]},
    ]
    parameters = [{'name': 'BoardId', 'type': 'int32', 'value': 0},
                  {'name': 'ClockSource', 'type': 'string', 'value': 'InternalTiming'},
                  {'name': 'SSIMode','type': 'string', 'value': 'Slave'},
                  {'name': 'TriggerSource','type': 'string', 'value': 'ExternalEXTD'},
                  {'name': 'ExecutionMode','type': 'string', 'value': 'RealTimeThread'},
                  {'name': 'ClockDivisor', 'type': 'int32', 'value': 40},
                  {'name': 'SleepTime', 'type': 'int32', 'value': 0},
                  {'name': 'Model', 'type': 'int32', 'value': 2010},
                  ]
    parts = []
    
    def getCounterName(self):
        return 'Counter_da'
    def getTimeName(self):
        return 'Time_da'
        
        
