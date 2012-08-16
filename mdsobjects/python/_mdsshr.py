import ctypes as _C
from ctypes.util import find_library as _find_library
import numpy as _N
import os,sys

def _load_library(name):
    if os.name == 'nt':
        return _C.CDLL(name)
    elif os.name == "posix" and sys.platform == "darwin":
        lib=_find_library(name)
        if lib is None:
            raise Exception,"Error finding library: "+name
        else:
            return _C.CDLL(lib)
    else:
        try:
            return _C.CDLL('lib'+name+'.so')
        
        except:
            return _C.CDLL('lib'+name+'.sl')
    raise Exception,"Error finding library: "+name

MdsShr=_load_library('MdsShr')
__MdsGetMsg=MdsShr.MdsGetMsg
__MdsGetMsg.argtypes=[_C.c_int32]
__MdsGetMsg.restype=_C.c_char_p
__LibConvertDateString=MdsShr.LibConvertDateString
__LibConvertDateString.argtypes=[_C.c_char_p,_C.POINTER(_C.c_ulonglong)]
__MDSWfeventTimed=MdsShr.MDSWfeventTimed
__MDSWfeventTimed.argtypes=[_C.c_char_p,_C.c_int32,_C.c_void_p,_C.POINTER(_C.c_int32),_C.c_int32]
__MDSEventCan=MdsShr.MDSEventCan
__MDSEventCan.argtypes=[_C.c_int32,]
__MDSEvent=MdsShr.MDSEvent
__MDSEvent.argtypes=[_C.c_char_p,_C.c_int32,_C.c_void_p]

class MdsException(Exception):
    pass

class MdsInvalidEvent(MdsException):
    pass

class MdsTimeout(MdsException):
    pass

class MdsNoMoreEvents(MdsException):
    pass

def MDSEventCan(eventid):
    """Cancel an event callback
    @param eventid: event id returned from original eventAst call
    @type eventid: long
    """
    status=__MDSEventCan(eventid)
    if ((status & 1)==0):
        raise MdsException,MdsGetMsg(status)

def MDSWfeventTimed(event,timeout):
    import numpy as _N
    from mdsarray import makeArray,Uint8Array
    buffer=_N.uint8(0).repeat(repeats=4096)
    numbytes=_C.c_int32(0)
    status=__MDSWfeventTimed(event,len(buffer),buffer.ctypes.data,numbytes,timeout)
    if (status & 1) == 1:
	if numbytes.value == 0:
	  return makeArray(Uint8Array([]))
        else:
          return makeArray(buffer[range(numbytes.value)])
    elif (status == 0):
        raise MdsTimeout,"Event %s timed out." % (str(event),)
    else:
        raise MdsException,MdsGetMsg(status)

def MDSEvent(event,buffer):
    status=__MDSEvent(event,len(buffer),buffer.ctypes.data)
    if not ((status & 1) == 1):
        raise MdsException,MdsGetMsg(status)
    
def MdsGetMsg(status,default=None):
    status=int(status)
    if status==0 and not default is None:
        return default
    return __MdsGetMsg(status)

def MdsSerializeDscOut(desc):
    from _descriptor import descriptor_xd,descriptor
    xd=descriptor_xd()
    if not isinstance(desc,descriptor):
        desc=descriptor(desc)
    status=MdsShr.MdsSerializeDscOut(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise MdsException,MdsGetMsg(status)

def MdsSerializeDscIn(bytes):
    from _descriptor import descriptor_xd
    xd=descriptor_xd()
    status=MdsShr.MdsSerializeDscIn(bytes.ctypes.data,_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise MdsException,MdsGetMsg(status)

def MdsDecompress(value):
    from _descriptor import descriptor_xd
    from mdsarray import makeArray
    xd=descriptor_xd()
    status = MdsShr.MdsDecompress(_C.pointer(value),_C.pointer(xd))
    if (status & 1) == 1:
        return makeArray(xd.value)
    else:
        raise MdsException,MdsGetMsg(status)

def MdsCopyDxXd(desc):
    from _descriptor import descriptor,descriptor_xd
    xd=descriptor_xd()
    if not isinstance(desc,descriptor):
        desc=descriptor(desc)
    status=MdsShr.MdsCopyDxXd(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
        return xd
    else:
        raise MdsException,MdsGetMsg(status)

def MdsCompareXd(value1,value2):
    from _descriptor import descriptor
    if not isinstance(value1,descriptor):
        value1=descriptor(value1)
    if not isinstance(value2,descriptor):
        value2=descriptor(value2)
    return MdsShr.MdsCompareXd(_C.pointer(value1),_C.pointer(value2))

def MdsFree1Dx(value):
    MdsShr.MdsFree1Dx(_C.pointer(value),_C.c_void_p(0))

def DateToQuad(date):
    from mdsdata import makeData
    ans=_C.c_ulonglong(0)
    status = __LibConvertDateString(date,ans)
    if not (status & 1):
        raise MdsException,"Cannot parse %s as date. Use dd-mon-yyyy hh:mm:ss.hh format or \"now\",\"today\",\"yesterday\"." % (date,)
    return makeData(_N.uint64(ans.value))

try:
    __MDSQueueEvent=MdsShr.MDSQueueEvent
    __MDSQueueEvent.argtypes=[_C.c_char_p,_C.POINTER(_C.c_int32)]
    __MDSGetEventQueue=MdsShr.MDSGetEventQueue
    __MDSGetEventQueue.argtypes=[_C.c_int32,_C.c_int32,_C.POINTER(_C.c_int32),_C.POINTER(_C.c_void_p)]
    def MDSQueueEvent(event):
        """Establish an event queue for an MDSplus event. Event occurrences will be monitored and accumulate
        until calls to MDSGetEventQueue retrieves the events occurences.
        @param event: Name of event to monitor
        @type event: str
        @return: eventid used in MDSGetEventQueue, and MDSEventCan
        @rtype: int
        """
        eventid=_C.c_int32(0)
        status = __MDSQueueEvent(event,eventid)
        if status&1 == 1:
            return eventid.value
        else:
            raise MdsException,"Error queuing the event %s, status=%d" % (event,status)

    def MDSGetEventQueue(eventid,timeout=0):
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
        from mdsarray import makeArray
        import numpy as _N
        dlen=_C.c_int32(0)
        bptr=_C.c_void_p(0)
        status=__MDSGetEventQueue(eventid,timeout,dlen,bptr)
        if status==1:
            if dlen.value>0:
                ans = makeArray(_N.array(_C.cast(bptr,_C.POINTER(_C.c_uint8 * dlen.value)).contents,dtype=_N.uint8))
                MdsShr.MdsFree(bptr)
                return ans
            else:
                return makeArray([])
        elif status==0:
            if timeout > 0:
                raise MdsTimeout,"Timeout"
            else:
                raise MdsNoMoreEvents,"No more events"
        elif status==2:
            raise MdsInvalidEvent,"Invalid eventid"
        else:
            raise MdsException,"Unknown error - status=%d" % (status,)
except:
    pass
