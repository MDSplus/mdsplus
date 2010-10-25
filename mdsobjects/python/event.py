from threading import Thread
import numpy as _N
import ctypes as _C
import os
import time
from mdsdata import makeData
from _mdsshr import DateToQuad

class Event(Thread):
    """Thread to wait for event"""

    def cancel(self):
        self.__active__=False

    def __init__(self,event,timeout=0):
        """Saves event name and starts wfevent thread
        @param event: name of event to monitor
        @type event: str
        """
        self.event=event
        self.exception=None
        self.timeout=timeout
        super(Event,self).__init__()
        self.setDaemon(True)
        self.__active__=True
        self.subclass_run=self.run
        self.run=self.event_run
	self.start()

    def event_run(self):
        from _mdsshr import MDSWfeventTimed,DateToQuad
        from numpy import uint64
        while self.__active__:
            self.exception=None
            try:
                self.raw=MDSWfeventTimed(self.event,self.timeout)
            except Exception,e:
                self.exception=e
            if self.__active__:
                self.time=time.time()
                self.subclass_run()
                qtime=DateToQuad("now")
                self.qtime=makeData(uint64(qtime))
    
    def getData(self):
        """Return data transfered with the event.
        @rtype: Data
        """
	if len(self.raw) == 0:
	  return None
        from mdsarray import makeArray
        return makeArray(self.getRaw()).deserialize()

    def getRaw(self):
        """Return raw data transfered with the event.
        @rtype: numpy.uint8
        """
        if len(self.raw) == 0:
		return None
        return self.raw

    def getTime(self):
        """Return time of event in seconds since epoch
        rtype: float
        """
        return self.time

    def getQTime(self):
	"""Return quadword time when the event last occurred
	rtype: Uint64
        """
	return self.qtime
        
    def getName(self):
	"""Return the name of the event
	rtype: str
	"""
	return self.event

    def setevent(event,data=None):
        """Issue an MDSplus event
        @param event: event name
        @type event: str
        @param data: data to pass with event
        @type data: Data
        """
        if data is None:
          Event.seteventRaw(event,None)
        else:
          Event.seteventRaw(event,makeData(data).serialize())
    setevent=staticmethod(setevent)
    
    def seteventRaw(event,buffer=None):
        """Issue an MDSplus event
        @param event: event name
        @type event: str
        @param buffer: data buffer
        @type buffer: numpy.uint8 array
        """
        from _mdsshr import MDSEvent
        if buffer is None:
          from numpy import array
          buffer=array([])
        MDSEvent(event,buffer)
    seteventRaw=staticmethod(seteventRaw)

    def wfevent(event):
	"""Wait for an event
	@param event: event name
	@rtype: Data
	"""
	class wfevent_handler(Event):
            def run(self):
                self.cancel()

        c=wfevent_handler(event)
        c.join()
        return c.getData()
    wfevent=staticmethod(wfevent)

    def wfeventRaw(event):
	"""Wait for an event
	@param event: event name
	@rtype: Data
	"""
	class wfevent_handler(Event):
            def run(self):
                self.cancel()

        c=wfevent_handler(event)
        c.join()
        return c.getRaw()
    wfeventRaw=staticmethod(wfeventRaw)
