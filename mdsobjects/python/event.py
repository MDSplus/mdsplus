import time as _time
import threading as _threading
import ctypes as _C
import numpy as _N

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


_data=_mimport('mdsdata')
_array=_mimport('mdsarray')
_scalar=_mimport('mdsscalar')
_mdsshr=_mimport('_mdsshr')
_exceptions=_mimport('mdsExceptions')
_ver=_mimport('version')

class MdsshrException(_exceptions.MDSplusException):
    pass

class MdsInvalidEvent(MdsshrException):
    pass

class MdsTimeout(MdsshrException):
    pass
#
#############################################

#### Load Shared Libraries Referenced #######
#
_MdsShr=_ver.load_library('MdsShr')
#
#############################################

class Event(_threading.Thread):
    """Thread to wait for event

This class can be used as a superclass to define
some action to perform when a specified MDSplus
event occurs. For example:

<<<<<<<< test.py >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
from __init__ import *
class myEvent(Event):
    def run(self):
       print("Event %s occurred at %s with data: %s " % \
             (str(self.event), \
              str(self.qtime.date), \
              str(self.raw.deserialize())))
e=myEvent('eventname')
import time
time.sleep(5)
Event.setevent('eventname',Signal(42,None,100))
e.cancel()
<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

$ python test.py
Event eventname occurred at 28-JUL-2016 16:03:51.00 with data: Build_Signal(42, *, 100)

Note the run procedure occurs in a different thread so one should be careful
to prevent race conditions or interference between threads. If you were implementing
a daemon process to handle one or more named events you could add a call to the
join() method of the Event class for example:

from __init__ import *
class myEvent(Event):
    def run(self):
       if len(self.raw) == 0:
          self.cancel()
       print("Event %s occurred at %s with data: %s " % \
             (str(self.event), \
              str(self.qtime.date), \
              str(self.raw.deserialize())))
e=myEvent('eventname')
e.join()

This would print out the occurrence and data for the event called 'eventname' and
if that event is issued without any data the event instance would be canceled and
the e.join() would return exiting the problem.


"""

    def getData(self):
        """Return data transfered with the event.
        @rtype: Data
        """
        if len(self.raw) == 0:
            return None
        return _array.Array(self.getRaw()).deserialize()

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

    @staticmethod
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
            Event.seteventRaw(event,_data.Data(data).serialize())

    @staticmethod
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
        status=_MdsShr.MDSEvent(_ver.tobytes(event),_C.c_int32(len(buffer)),_C.c_void_p(buffer.ctypes.data))
        if not ((status & 1) == 1):
            raise _exceptions.statusToException(status)

    @staticmethod
    def wfeventRaw(event, timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        buffer=_N.uint8(0).repeat(repeats=4096)
        numbytes=_C.c_int32(0)
        status=_MdsShr.MDSWfeventTimed(_ver.tobytes(event),
                                       _C.c_int32(len(buffer)),
                                       _C.c_void_p(buffer.ctypes.data),
                                       _C.pointer(numbytes),
                                       _C.c_int32(timeout))
        if (status & 1) == 1:
            if numbytes.value == 0:
                return _scalar.Uint8([])
            else:
                return _array.Uint8Array(buffer[0:numbytes.value])
        elif (status == 0):
            raise MdsTimeout("Event %s timed out." % (_ver.tostr(event),))
        else:
            raise _exceptions.statusToException(status)

    @staticmethod
    def wfevent(event,timeout=0):
        """Wait for an event
        @param event: event name
        @rtype: Data
        """
        return Event.wfeventRaw(event, timeout).deserialize()

    @staticmethod
    def queueEvent(event):
        """Establish an event queue for an MDSplus event. Event occurrences will be monitored and accumulate
        until calls to MDSGetEventQueue retrieves the events occurences.
        @param event: Name of event to monitor
        @type event: str
        @return: eventid used in MDSGetEventQueue, and MDSEventCan
        @rtype: int
        """
        eventid=_C.c_int32(0)
        status = _MdsShr.MDSQueueEvent(_ver.tobytes(event),_C.pointer(eventid))
        if not status&1 == 1:
            raise _exceptions.statusToException(status)
        return eventid.value

    def getQueue(self):
        """Retrieve event occurrence.
        @param eventid: eventid returned from MDSQueueEvent function
        @type eventid: int
        @param timeout: Optional timeout. If greater than 0 an MdsTimeout exception will be raised if no event occurs
        within timeout seconds after function invokation. If timeout equals zero then this function will
        block until an event occurs. If timeout is less than zero this function will not wait for events
        and will either returned a queued event or raise MdsNoMoreEvents.
        @type timeout: int
        @return: event data
        @rtype: Uint8Array
        """
        dlen=_C.c_int32(0)
        bptr=_C.c_void_p(0)
        status=_MdsShr.MDSGetEventQueue(_C.c_int32(self.eventid),_C.c_int32(self.timeout),_C.pointer(dlen),_C.pointer(bptr))
        if status==1:
            if dlen.value>0:
                ans = _array.Uint8Array(_N.ndarray(shape=[dlen.value],buffer=_ver.buffer(_C.cast(bptr,_C.POINTER((_C.c_byte * dlen.value))).contents),dtype=_N.uint8))
                _MdsShr.MdsFree(bptr)
                return ans
            else:
                return _array.Uint8Array([])
        elif status==0:
            if self.timeout > 0:
                raise MdsTimeout("Timeout")
            else:
                raise _exceptions.MdsNoMoreEvents("No more events")
        elif status==2:
            raise MdsInvalidEvent("Invalid eventid")
        else:
            raise _exceptions.statusToException(status)

    def _event_run(self):
        while True:
            try:
                self.raw=self.getQueue()
                self.exception=None
            except MdsInvalidEvent:
                return
            except Exception as exc:
                self.exception=exc
            self.time=_time.time()
            self.qtime=_mdsshr.DateToQuad("now")
            self.subclass_run()
            _time.sleep(.01)

    def cancel(self):
        """Cancel this event instance. No further events will be processed for this instance.
        """
        status=_MdsShr.MDSEventCan(_C.c_int32(self.eventid))
        if ((status & 1)==0):
            raise _exceptions.statusToException(status)

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

