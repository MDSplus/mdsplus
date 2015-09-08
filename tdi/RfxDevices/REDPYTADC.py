from MDSplus import *
from ctypes import *
import httplib
import json

class REDPYTADC(Device):
  parts=[{'path':':IP_ADDR', 'type':'text'},{'path':':COMMENT', 'type':'text'},
  {'path':':ACQ_TIME', 'type':'numeric', 'value':10E3},
  {'path':':TRIG_MODE', 'type':'text', 'value':'INTERNAL'},
  {'path':':FULL_SCALE', 'type':'text', 'value':'12V'},
  {'path':':FREQUENCY', 'type':'numeric', 'value':1E4},
  {'path':':TRIG_TIME', 'type':'numeric'},
  {'path':':PTS', 'type':'numeric'},
  {'path':':CHANNEL_1', 'type':'signal'},
  {'path':':CHANNEL_2', 'type':'signal'}]

  parts.append({'path':':INIT_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','INIT',50,None),Method(None,'init',head))",
	'options':('no_write_shot',)})
  parts.append({'path':':STORE_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('SERVER','STORE',50,None),Method(None,'store',head))",
	'options':('no_write_shot',)})

  print 'REDPYTADC activated'
  
  
  def init(self, arg):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
      hConn.request("GET", "/bazaar?start=scope+gen")
      hConn.getresponse()
    except: 
      print 'Cannot connect to '+self.ip_addr.data()
      return 0
    tEnd = self.acq_time.data()/1000.
    trigSourceDict = {'CHAN_A':0, 'CHAN_B':1, 'EXTERNAL':2}
    fullScaleDict = {'12V':1, '5V':0}
    trigSourceCode = trigModeDict[self.trig_mode(data)]
    fullScaleCode = fullScaleDict[self.full_scale.data()]
    jsonStr = {'datasets':{'params':{'xmin':0}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load xmin"
      return 0
	
    jsonStr = {'datasets':{'params':{'xmax':tEnd}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load xmax"
      return 0
	
    jsonStr = {'datasets':{'params':{'trig_mode':2}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load trig_mode"
      return 0
	
    jsonStr = {'datasets':{'params':{'trig_source': trigSourceCode}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load trig_mode"
      return 0
	
    jsonStr = {'datasets':{'params':{'trig_edge': 0}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load trig_edge"
      return 0
	
    jsonStr = {'datasets':{'params':{'time_range': int(self.frequency.data())}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load time_range"
      return 0
	
    jsonStr = {'datasets':{'params':{'gain_ch1': fullScaleCode}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load tgain_ch1"
      return 0
	
    jsonStr = {'datasets':{'params':{'gain_ch2': fullScaleCode}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot load gain_ch2"
      return 0
	
  def trigger(self, arg):
    try:
      hConn = httplib.HTTPConnection(self.ip_addr.data())
      hConn.getresponse()
    except: 
      print 'Cannot connect to '+self.ip_addr.data()
      return 0
    jsonStr = {'datasets':{'params':{'single_btn': 1}}}
    try:
      hConn.request("POST", "/data", json.dumps(jsonStr))
      hConn.getresponse()
    except: 
      print "Cannot trigger device"
      return 0
  
  
