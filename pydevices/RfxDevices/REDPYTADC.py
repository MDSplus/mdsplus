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

from MDSplus import mdsExceptions, Device, Data, Float32, Float32Array, version
if version.ispy3:
    import http as httplib
else:
    import httplib
import json
from  time import sleep

class REDPYTADC(Device):
  parts=[{'path':':IP_ADDR', 'type':'text'},{'path':':COMMENT', 'type':'text'},
  {'path':':TRIG_SOURCE', 'type':'numeric', 'value':2},
  {'path':':TRIG_EDGE', 'type':'numeric', 'value':0},
  {'path':':FULL_SCALE_1', 'type':'numeric', 'value':1},
  {'path':':FULL_SCALE_2', 'type':'numeric', 'value':1},
  {'path':':TRIG_TIME', 'type':'numeric'},
  {'path':':CHANNEL_1', 'type':'signal'},
  {'path':':CHANNEL_2', 'type':'signal'}]

  parts.append({'path':':INIT_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','INIT',50,None),Method(None,'init',head))",
	'options':('no_write_shot',)})
  parts.append({'path':':STORE_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','STORE',50,None),Method(None,'store',head))",
	'options':('no_write_shot',)})


  def init(self):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
      hConn.request("GET", "/bazaar?start=scope+gen")
      hConn.getresponse()
    except:
      print('Cannot connect to '+self.ip_addr.data())
      raise mdsExceptions.TclFAILED_ESSENTIAL
    trigSource = self.trig_source.data()
    trigEdge = self.trig_edge.data()
    fullScale1 = self.full_scale_1.data()
    fullScale2 = self.full_scale_2.data()
    sleep(1)


    jsonStr = {'datasets':{'params':{'trig_mode':1}}}
    print(jsonStr)
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load trig_mode")
      raise mdsExceptions.TclFAILED_ESSENTIAL

    jsonStr = {'datasets':{'params':{'trig_source':int(trigSource)}}}
    print(jsonStr)
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load trig_source")
      raise mdsExceptions.TclFAILED_ESSENTIAL

    jsonStr = {'datasets':{'params':{'trig_edge': int(trigEdge)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load trig_edge")
      raise mdsExceptions.TclFAILED_ESSENTIAL


    jsonStr = {'datasets':{'params':{'gain_ch1': int(fullScale1)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load gain_ch1")
      raise mdsExceptions.TclFAILED_ESSENTIAL

    jsonStr = {'datasets':{'params':{'gain_ch2': int(fullScale2)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load gain_ch2")
      raise mdsExceptions.TclFAILED_ESSENTIAL
    return


  def trigger(self):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
    except:
      print('Cannot connect to '+self.ip_addr.data())
      raise mdsExceptions.TclFAILED_ESSENTIAL
    jsonStr = {'datasets':{'params':{'single_btn': 1}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot trigger device")
      raise mdsExceptions.TclFAILED_ESSENTIAL
    return

  def store(self):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
    except:
      print('Cannot connect to '+self.ip_addr.data())
      raise mdsExceptions.TclFAILED_ESSENTIAL
    try:
      hConn.request("GET", "/data")
      r = hConn.getresponse()
      jans = json.load(r)
    except:
      print('Cannot get data')
      raise mdsExceptions.TclFAILED_ESSENTIAL

    chan1 = jans['datasets']['g1'][0]['data']
    chan2 = jans['datasets']['g1'][1]['data']

    x1 = []
    y1 = []
    x2 = []
    y2 = []
    for i in range(0, len(chan1)):
       x1.append(chan1[i][0] * 1E-6)
       y1.append(chan1[i][1])
       x2.append(chan2[i][0] * 1E-6)
       y2.append(chan2[i][1])

    try :
	    triggerTime = self.trig_time.data()
    except:
        triggerTime = 0

    try:
        dim1 = Data.compile('$1 + $2', Float32Array(x1), Float32(triggerTime))
        data1 = Float32Array(y1)
        sig1 = Data.compile('build_signal($1,,$2)', data1, dim1)
        self.channel_1.putData(sig1)
    except:
        print('Cannot Save Channel 1')
        raise mdsExceptions.TclFAILED_ESSENTIAL

    try:
        dim2 = Data.compile('$1 + $2', Float32Array(x1), Float32(triggerTime))
        data2 = Float32Array(y2)
        sig2 = Data.compile('build_signal($1,,$2)', data2, dim2)
        self.channel_2.putData(sig2)
    except:
        print('Cannot Save Channel 2')
        raise mdsExceptions.TclFAILED_ESSENTIAL
    return
