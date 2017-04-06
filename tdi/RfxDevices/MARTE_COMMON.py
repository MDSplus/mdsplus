# -*- coding: iso-8859-1 -*-
#MDSplus device superclass for MARTe applications
from MDSplus import mdsExceptions, Device, Event, Tree, Data
from os import environ
from time import sleep

class MARTE_COMMON(Device):
    """MARTe configuration"""
    parts=[{'path':':COMMENT', 'type':'text'},
      {'path':':ID', 'type':'numeric', 'value':0},
      {'path':':CONTROL', 'type':'text', 'value':'CONTROL'}]

    parts.append({'path':'.SIGNALS', 'type':'structure'})
    parts.append({'path':'.SIGNALS:NAMES', 'type':'text'})
#Maximim number of stored signals: 256
    for i in range(256):
      parts.append({'path':'.SIGNALS:SIGNAL_%03d'%(i+1), 'type':'structure'})
      parts.append({'path':'.SIGNALS:SIGNAL_%03d:NAME'%(i+1), 'type':'text'})
      parts.append({'path':'.SIGNALS:SIGNAL_%03d:DESCRIPTION'%(i+1), 'type':'text'})
      parts.append({'path':'.SIGNALS:SIGNAL_%03d:DATA'%(i+1), 'type':'signal'})
    del(i)
    parts.append({'path':':INIT_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','SEQ_INIT',50,None),Method(None,'init',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','SEQ_STORE',50,None),Method(None,'store',head))",
        'options':('no_write_shot',)})


    def getEventName(self):
      if environ.get("MARTE_EVENT") is None:
        return "MARTE"
      else:
        return environ["MARTE_EVENT"]

#init method will send a SETUP event with the required information to allow MDSInterface service retrieving parameter and signal information
    def init(self):
      eventStr = "SETUP " + self.tree.name + " "  + self.control.data() + " " + str(self.tree.shot) + " " + str(self.id.data()) + " "

      eventStr = eventStr + " " + str(self.params.nid)
      eventStr = eventStr + " " + str(self.wave_params.nid)
      eventStr = eventStr + " " + str(self.signals.nid)
      print(eventStr)
      Event.setevent(self.getEventName(), eventStr)
      sleep(3)
      return 1

#load method will send a LOAD event forcing reporting in MARTe confirguration the actual value of MDSplus parameters.
#GAM field MdsId will specify the target device for every GAM taking MDSplus parameters
    def load(self):
       eventStr = "LOAD"
       Event.setevent(self.getEventName(), eventStr)
       return 1

#Event transition requests
    def pre_req(self):
      eventStr = "PRE_REQ " + str(self.id.data())
      Event.setevent(self.getEventName(), eventStr)
      return 1

    def pulse_req(self):
      eventStr = "PULSE_REQ"
      Event.setevent(self.getEventName(), eventStr)
      return 1

    def post_req(self):
      eventStr = "POST_REQ"
      Event.setevent(self.getEventName(), eventStr)
      return 1

    def collection_complete(self):
      eventStr = "COLLECTION_COMPLETE"
      Event.setevent(self.getEventName(), eventStr)
      return 1

    def abort(self):
      eventStr = "ABORT"
      Event.setevent(self.getEventName(), eventStr)
      return 1

#force flushing of buffered data. Typially called after COLLECTION_COMPLETE event
    def store(self):
      eventStr = "STORE " +  str(self.id.data())
      print(eventStr)
      Event.setevent(self.getEventName(), eventStr)
      sleep(10)
      return 1
