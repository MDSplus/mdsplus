def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import time as _time
import threading as _threading
import sys as _sys

_data=_mimport('mdsdata')
_array=_mimport('mdsarray')
_mdsshr=_mimport('_mdsshr')

class Event(_threading.Thread):
    """Thread to wait for event"""

    def getData(self):
        """Return data transfered with the event.
        @rtype: Data
        """
        if len(self.raw) == 0:
            return None
        return _array.makeArray(self.getRaw()).deserialize()

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
            Event.seteventRaw(event,_data.makeData(data).serialize())
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
        _mdsshr.MDSEvent(event,buffer)
    seteventRaw=staticmethod(seteventRaw)

    def wfevent(event,timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        return _mdsshr.MDSWfeventTimed(event,timeout).deserialize()
    wfevent=staticmethod(wfevent)


    def wfeventRaw(event,timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        return _mdsshr.MDSWfeventTimed(event,timeout)
    wfeventRaw=staticmethod(wfeventRaw)

try:
    MDSQueueEvent=_mdsshr.MDSQueueEvent
    MDSGetEventQueue=_mdsshr.MDSGetEventQueue
    MDSEventCan=_mdsshr.MDSEventCan
    MDSInvalidEvent=_mdsshr.MdsInvalidEvent
    def _event_run(self):
        while True:
            try:
                self.raw=self.getQueue(self.eventid,self.timeout)
                self.exception=None
            except MDSInvalidEvent:
                return
            except:
                self.exception=_sys.exc_info()[1]
            self.time=_time.time()
            self.qtime=_mdsshr.DateToQuad("now")
            self.subclass_run()
            _time.sleep(.01)
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
                self.raw=_mdsshr.MDSWfeventTimed(self.event,self.timeout)
            except Exception:
                import sys
                self.exception=sys.exc_info()[1]
            if self.__active__:
                self.time=_time.time()
                self.qtime=_mdsshr.DateToQuad("now")
                self.subclass_run()
                _time.sleep(.01)
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
