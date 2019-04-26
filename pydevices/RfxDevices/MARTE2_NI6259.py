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

from MDSplus import *
from MARTE2_COMPONENT import *
class MARTE2_NI6259(MARTE2_COMPONENT):
    MARTE2_COMPONENT.outputs = [
      {'name': 'Counter', 'type':'uint32', 'dimensions': 0, 'parameters':[]},
      {'name': 'Time', 'type':'uint32', 'dimensions': 0, 'parameters':[]},
      {'name': 'ADC0_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':0},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC1_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC2_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC3_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC4_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC5_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC6_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]},
      {'name': 'ADC7_0', 'type': 'float32', 'dimensions': 0, 'parameters':[{'name':'InputRange', 'type':'float32', 'value':10},
												    {'name':'ChannelId', 'type': 'int', 'value':1},
												    {'name':'InputPolarity', 'type':'string', 'value': 'Bipolar'},
												    {'name': 'InputMode', 'type':'string','value':'RSE'}]}]
    MARTE2_COMPONENT.parameters = [{'name':'SamplingFrequency', 'type': 'int32', 'value':1000000},
				   {'name':'DeviceName', 'type': 'string', 'value':'/dev/pxi6259'},
				   {'name':'BoardId', 'type': 'int32', 'value':0},
				   {'name': 'DelayDivisor', 'type': 'int32', 'value':3},
				   {'name': 'ClockSampleSource', 'type': 'string', 'value': 'SI_TC'},
				   {'name': 'ClockSamplePolarity', 'type': 'string', 'value': 'ACTIVE_HIGH_OR_RISING_EDGE'},
				   {'name': 'ClockConvertSource', 'type': 'string', 'value': 'SI2TC'},
				   {'name': 'ClockConvertPolarity', 'type': 'string', 'value': 'RISING_EDGE'},
				   {'name': 'CPUs', 'type':'int32', 'value': 0xf}]

    parts = []
    MARTE2_COMPONENT.buildGam(parts, 'NI6259', MARTE2_COMPONENT.MODE_SYNCH_INPUT, 'build_range(0, 1000000, 1./build_path(".parameters:parameter_1:value"))')

    def prepareMarteInfo(self):
      print('0:1000000 : 8. / build_path("\\'+self.getFullPath()+'.parameters:parameter_1:value")' )
      self.timebase.putData(Data.compile('0:1000000 : 8./ build_path("\\'+self.getFullPath()+'.parameters:parameter_1:value")'))
