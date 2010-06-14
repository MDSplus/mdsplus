from threading import Thread
import numpy as _N
import ctypes as _C
import os
import time
from mdsdata import makeData

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
        from _mdsshr import MDSWfeventTimed
        while self.__active__:
            self.exception=None
            try:
                self.raw=MDSWfeventTimed(self.event,self.timeout)
            except Exception,e:
                self.exception=e
            if self.__active__:
                self.time=time.time()
                self.subclass_run()
    
    def getData(self):
        """Return data transfered with the event.
        @rtype: Data
        """
        from mdsarray import makeArray
        return makeArray(self.getRaw()).deserialize()

    def getRaw(self):
        """Return raw data transfered with the event.
        @rtype: numpy.uint8
        """
        return self.raw

    def getTime(self):
        """Return time of event in seconds since epoch
        rtype: float
        """
        return self.time
        

    def setevent(event,data):
        """Issue an MDSplus event
        @param event: event name
        @type event: str
        @param data: data to pass with event
        @type data: Data
        """
        Event.seteventRaw(event,makeData(data).serialize())
    setevent=staticmethod(setevent)
    
    def seteventRaw(event,buffer):
        """Issue an MDSplus event
        @param event: event name
        @type event: str
        @param buffer: data buffer
        @type buffer: numpy.uint8 array
        """
        from _mdsshr import MDSEvent
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
