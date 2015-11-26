from MDSplus import Device, Data, Float32, Float32Array, version
if version.ispy3:
    import http as httplib
else:
    import httplib
import json
from  time import sleep

class REDPYTADC(Device):
  print('REDPYTADC')
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


  def init(self, arg):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
      hConn.request("GET", "/bazaar?start=scope+gen")
      hConn.getresponse()
    except:
      print('Cannot connect to '+self.ip_addr.data())
      return 0
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
      return 0

    jsonStr = {'datasets':{'params':{'trig_source':int(trigSource)}}}
    print(jsonStr)
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load trig_source")
      return 0

    jsonStr = {'datasets':{'params':{'trig_edge': int(trigEdge)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load trig_edge")
      return 0


    jsonStr = {'datasets':{'params':{'gain_ch1': int(fullScale1)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load gain_ch1")
      return 0

    jsonStr = {'datasets':{'params':{'gain_ch2': int(fullScale2)}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot load gain_ch2")
      return 0
    return 1


  def trigger(self, arg):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
    except:
      print('Cannot connect to '+self.ip_addr.data())
      return 0
    jsonStr = {'datasets':{'params':{'single_btn': 1}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except:
      print("Cannot trigger device")
      return 0
    return 1

  def store(self, arg):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
    except:
      print('Cannot connect to '+self.ip_addr.data())
      return 0
    try:
      hConn.request("GET", "/data")
      r = hConn.getresponse()
      jans = json.load(r)
    except:
      print('Cannot get data')
      return 0

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
        return 0

    try:
        dim2 = Data.compile('$1 + $2', Float32Array(x1), Float32(triggerTime))
        data2 = Float32Array(y2)
        sig2 = Data.compile('build_signal($1,,$2)', data2, dim2)
        self.channel_2.putData(sig2)
    except:
        print('Cannot Save Channel 2')
        return 0
    return 1
