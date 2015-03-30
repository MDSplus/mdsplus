import ctypes as _C
from ctypes.util import find_library as _find_library
import numpy as _N
import os as _os
import sys as _sys

if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

def _load_library(name):
    if _sys.version_info[0]==2 and _sys.version_info[1]<5 and _os.name=='posix' and _sys.platform.startswith('linux'):
      return _C.CDLL('lib'+name+'.so')
    libnam=_find_library(name)
    if libnam is None:
        try:
            lib=_C.CDLL('lib'+name+'.so')
        except:
            try:
                lib=_C.CDLL('lib'+name+'.dylib')
            except:
                try:
                    lib=_C.CDLL(name+'.dll')
                except:
                    raise Exception("Error finding library: "+name)
    else:
        try:
            lib=_C.CDLL(libnam)
        except:
            try:
                lib=_C.CDLL(name)
            except:
                lib=_C.CDLL(_os.path.basename(libnam))
    return lib

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
        raise MdsException(MdsGetMsg(status))

def MDSWfeventTimed(event,timeout):
    _array=_mimport('mdsarray',1)
    buffer=_N.uint8(0).repeat(repeats=4096)
    numbytes=_C.c_int32(0)
    status=__MDSWfeventTimed(str.encode(event),len(buffer),buffer.ctypes.data,numbytes,timeout)
    if (status & 1) == 1:
        if numbytes.value == 0:
          return _array.Uint8Array([])
        else:
          return _array.makeArray(buffer[0:numbytes.value])
    elif (status == 0):
        raise MdsTimeout("Event %s timed out." % (str(event),))
    else:
        raise MdsException(MdsGetMsg(status))

def MDSEvent(event,buffer):
    status=__MDSEvent(str.encode(event),len(buffer),buffer.ctypes.data)
    if not ((status & 1) == 1):
        raise MdsException(MdsGetMsg(status))
    
def MdsGetMsg(status,default=None):
    status=int(status)
    if status==0 and not default is None:
        return default
    try:
        return __MdsGetMsg(status).decode()
    except:
        return __MdsGetMsg(status)

def MdsSerializeDscOut(desc):
    _desc=_mimport('_descriptor',1)
    xd=_desc.descriptor_xd()
    if not isinstance(desc,_desc.descriptor):
        desc=_desc.descriptor(desc)
    status=MdsShr.MdsSerializeDscOut(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise MdsException(MdsGetMsg(status))

def MdsSerializeDscIn(bytes):
    _desc=_mimport('_descriptor',1)
    xd=_desc.descriptor_xd()
    status=MdsShr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise MdsException(MdsGetMsg(status))

def MdsDecompress(value):
    _desc=_mimport('_descriptor',1)
    xd=_desc.descriptor_xd()
    status = MdsShr.MdsDecompress(_C.pointer(value),_C.pointer(xd))
    if (status & 1) == 1:
        return xd.value
    else:
        raise MdsException(MdsGetMsg(status))

def MdsCompareXd(value1,value2):
    return MdsShr.MdsCompareXd(_C.pointer(descriptor(value1)),_C.pointer(descriptor(value2)))


def MdsCopyDxXd(desc):
    _desc=_mimport('_descriptor',1)
    xd=_desc.descriptor_xd()
    if not isinstance(desc,_desc.descriptor):
        desc=_desc.descriptor(desc)
    status=MdsShr.MdsCopyDxXd(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
        return xd
    else:
        raise MdsException(MdsGetMsg(status))

def MdsCompareXd(value1,value2):
    _desc=_mimport('_descriptor',1)
    if not isinstance(value1,_desc.descriptor):
        value1=_desc.descriptor(value1)
    if not isinstance(value2,_desc.descriptor):
        value2=_desc.descriptor(value2)
    return MdsShr.MdsCompareXd(_C.pointer(value1),_C.pointer(value2))

def MdsFree1Dx(value):
    MdsShr.MdsFree1Dx(_C.pointer(value),_C.c_void_p(0))

def DateToQuad(date):
    _data=_mimport('mdsdata',1)
    ans=_C.c_ulonglong(0)
    status = __LibConvertDateString(date,ans)
    if not (status & 1):
        raise MdsException("Cannot parse %s as date. Use dd-mon-yyyy hh:mm:ss.hh format or \"now\",\"today\",\"yesterday\"." % (date,))
    return _data.makeData(_N.uint64(ans.value))

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
        status = __MDSQueueEvent(str.encode(event),eventid)
        if status&1 == 1:
            return eventid.value
        else:
            raise MdsException("Error queuing the event %s, status=%d" % (event,status))

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
        _array=_mimport('mdsarray',1)
        dlen=_C.c_int32(0)
        bptr=_C.c_void_p(0)
        status=__MDSGetEventQueue(eventid,timeout,dlen,bptr)
        if status==1:
            if dlen.value>0:
                ans = _array.Uint8Array(_N.ndarray(shape=[dlen.value],buffer=buffer(_C.cast(bptr,_C.POINTER((_C.c_byte * dlen.value))).contents),dtype=_N.uint8))
                MdsShr.MdsFree(bptr)
                return ans
            else:
                return _array.Uint8Array([])
        elif status==0:
            if timeout > 0:
                raise MdsTimeout("Timeout")
            else:
                raise MdsNoMoreEvents("No more events")
        elif status==2:
            raise MdsInvalidEvent("Invalid eventid")
        else:
            raise MdsException("Unknown error - status=%d" % (status,))
except:
    pass
