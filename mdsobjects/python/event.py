import numpy as _N
import ctypes as _C
import os
import time
from mdsdata import makeData
from mdsscalar import Uint64
from threading import Thread
from _mdsshr import DateToQuad,MDSWfeventTimed,MDSEvent

class Event(Thread):
    """Thread to wait for event"""

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
        if buffer is None:
            from numpy import array
            buffer=array([])
        MDSEvent(event,buffer)
    seteventRaw=staticmethod(seteventRaw)

    def wfevent(event,timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        return MDSWfeventTimed(event,timeout).deserialize()
    wfevent=staticmethod(wfevent)


    def wfeventRaw(event,timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        return MDSWfeventTimed(event,timeout)
    wfeventRaw=staticmethod(wfeventRaw)

try:
    from _mdsshr import MDSQueueEvent,MDSGetEventQueue,MDSEventCan,MdsInvalidEvent
    def _event_run(self):
        while True:
            try:
                self.raw=self.getQueue(self.eventid,self.timeout)
                self.exception=None
            except MdsInvalidEvent:
                return
            except Exception:
                import sys
                self.exception=sys.exc_info()[1]
            self.time=time.time()
            self.qtime=DateToQuad("now")
            self.subclass_run()
            time.sleep(.01)
    Event._event_run=_event_run

    def cancel(self):
        """Cancel this event instance. No further events will be processed for this instance.
        """
        self.eventCan(self.eventid)
    Event.cancel=cancel

    def __init__(self,event,timeout=0):
        """Saves event name and starts wfevent thread
        @param event: name of event to monitor
        @type event: str
        """
        super(Event,self).__init__()
        self.event=event
        self.exception=None
        self.timeout=timeout
        self.eventid=self.queueEvent(event)
        self.subclass_run=self.run
        self.run=self._event_run
        self.setDaemon(True)
        self.start()
    Event.__init__=__init__
    
    Event.eventCan=staticmethod(MDSEventCan)
    Event.queueEvent=staticmethod(MDSQueueEvent)
    Event.getQueue=staticmethod(MDSGetEventQueue)
 
except:
    def _event_run(self):
        while self.__active__:
            self.exception=None
            try:
                self.raw=MDSWfeventTimed(self.event,self.timeout)
            except Exception:
                import sys
                self.exception=sys.exc_info()[1]
            if self.__active__:
                self.time=time.time()
                self.qtime=DateToQuad("now")
                self.subclass_run()
                time.sleep(.01)
    Event._event_run=_event_run

    def cancel(self):
        self.__active__=False
    Event.cancel=cancel

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
        self.run=self._event_run
        self.start()
    Event.__init__=__init__
