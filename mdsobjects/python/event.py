from threading import Thread
import numpy as _N
import ctypes as _C
import os
import time

class Event(Thread):
    """Thread to wait for event"""

    def __del__(self):
        self.__active__=True

    def __init__(self,event):
        """Saves event name and starts wfevent thread
        @param event: name of event to monitor
        @type event: str
        """
        self.event=event
        Thread.__init__(self)
        self.__active__=True
        self.start()

    def __getattribute__(self,name):
        if name == "run":
            return super(Event,self).__getattribute__('_event_run')
        else:
            return super(Event,self).__getattribute__(name)

    def _event_run(self):
        from _mdsshr import MDSWfevent
        while self.__active__:
            self.raw=MDSWfevent(self.event)
            self.time=time.time()
            self.__class__.run(self)

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
        Event.seteventRaw(event,data.serialize())
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

