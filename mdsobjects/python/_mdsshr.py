def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import numpy as _N
import ctypes as _C

_ver=_mimport('version')
_array=_mimport('mdsarray')
_data=_mimport('mdsdata')
_Exceptions=_mimport('mdsExceptions')
_desc=_mimport('_descriptor')
_apd=_mimport('apd')


_mdsshr=_ver.load_library('MdsShr')
__MdsGetMsg=_mdsshr.MdsGetMsg
__MdsGetMsg.argtypes=[_C.c_int32]
__MdsGetMsg.restype=_C.c_char_p
__LibConvertDateString=_mdsshr.LibConvertDateString
__LibConvertDateString.argtypes=[_C.c_char_p,_C.POINTER(_C.c_ulonglong)]
__MDSWfeventTimed=_mdsshr.MDSWfeventTimed
__MDSWfeventTimed.argtypes=[_C.c_char_p,_C.c_int32,_C.c_void_p,_C.POINTER(_C.c_int32),_C.c_int32]
__MDSEventCan=_mdsshr.MDSEventCan
__MDSEventCan.argtypes=[_C.c_int32,]
__MDSEvent=_mdsshr.MDSEvent
__MDSEvent.argtypes=[_C.c_char_p,_C.c_int32,_C.c_void_p]

class MdsshrException(_Exceptions.MDSplusException):
    pass

class MdsInvalidEvent(MdsshrException):
    pass

class MdsTimeout(MdsshrException):
    pass

class MdsNoMoreEvents(MdsshrException):
    pass

def getenv(name):
    """get environment variable value
    @param name: name of environment variable
    @type name: str
    @return: value of environment variable or None if not defined
    @rtype: str or None
    """
    tl=_mdsshr.TranslateLogical
    tl.restype=_C.c_char_p
    try:
        ans=tl(_ver.tobytes(str(name)))
        if ans is not None:
            ans = _ver.tostr(ans)
    except:
        ans=""
    return ans

def setenv(name,value):
    """set environment variable
    @param name: name of the environment variable
    @type name: str
    @param value: value of the environment variable
    @type value: str
    """
    pe=_mdsshr.MdsPutEnv
    pe(_ver.tobytes("=".join([str(name),str(value)])))


def MDSEventCan(eventid):
    """Cancel an event callback
    @param eventid: event id returned from original eventAst call
    @type eventid: long
    """
    status=__MDSEventCan(eventid)
    if ((status & 1)==0):
        raise _Exceptions.statusToException(status)

def MDSWfeventTimed(event,timeout):
    buffer=_N.uint8(0).repeat(repeats=4096)
    numbytes=_C.c_int32(0)
    status=__MDSWfeventTimed(_ver.tobytes(event),len(buffer),buffer.ctypes.data,numbytes,timeout)
    if (status & 1) == 1:
        if numbytes.value == 0:
          return _array.Uint8Array([])
        else:
          return _array.makeArray(buffer[0:numbytes.value])
    elif (status == 0):
        raise MdsTimeout("Event %s timed out." % (_ver.tostr(event),))
    else:
        raise _Exceptions.statusToException(status)

def MDSEvent(event,buffer):
    status=__MDSEvent(_ver.tobytes(event),len(buffer),buffer.ctypes.data)
    if not ((status & 1) == 1):
        raise _Exceptions.statusToException(status)

def MdsGetMsg(status,default=None):
    status=int(status)
    if status==0 and not default is None:
        return default
    return _ver.tostr(__MdsGetMsg(status))

def MdsSerializeDscOut(desc):
    xd=_desc.descriptor_xd()
    if not isinstance(desc,_desc.descriptor):
        desc=_desc.descriptor(desc)
    status=_mdsshr.MdsSerializeDscOut(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise _Exceptions.statusToException(status)

def MdsSerializeDscIn(bytes):
    if len(bytes) == 0:  # short cut if setevent did not send array
        return _apd.List([])
    xd=_desc.descriptor_xd()
    status=_mdsshr.MdsSerializeDscIn(_C.c_void_p(bytes.ctypes.data),_C.pointer(xd))
    if (status & 1) == 1:
      return xd.value
    else:
      raise _Exceptions.statusToException(status)

def MdsDecompress(value):
    xd=_desc.descriptor_xd()
    status = _mdsshr.MdsDecompress(_C.pointer(value),_C.pointer(xd))
    if (status & 1) == 1:
        return xd.value
    else:
        raise _Exceptions.statusToException(status)


def MdsCopyDxXd(desc):
    xd=_desc.descriptor_xd()
    if not isinstance(desc,_desc.descriptor):
        desc=_desc.descriptor(desc)
    status=_mdsshr.MdsCopyDxXd(_C.pointer(desc),_C.pointer(xd))
    if (status & 1) == 1:
        return xd
    else:
        raise _Exceptions.statusToException(status)

#def MdsCompareXd(value1,value2):
#    return MdsShr.MdsCompareXd(_C.pointer(descriptor(value1)),_C.pointer(descriptor(value2)))

def MdsCompareXd(value1,value2):
    if not isinstance(value1,_desc.descriptor):
        value1=_desc.descriptor(value1)
    if not isinstance(value2,_desc.descriptor):
        value2=_desc.descriptor(value2)
    return _mdsshr.MdsCompareXd(_C.pointer(value1),_C.pointer(value2))

def MdsFree1Dx(value):
    _mdsshr.MdsFree1Dx(_C.pointer(value),_C.c_void_p(0))

def DateToQuad(date):
    ans=_C.c_ulonglong(0)
    status = __LibConvertDateString(_ver.tobytes(date),ans)
    if not (status & 1):
        raise MdsshrException("Cannot parse %s as date. Use dd-mon-yyyy hh:mm:ss.hh format or \"now\",\"today\",\"yesterday\"." % (date,))
    return _data.makeData(_N.uint64(ans.value))

try:  # should not be done
    __MDSQueueEvent=_mdsshr.MDSQueueEvent
    __MDSQueueEvent.argtypes=[_C.c_char_p,_C.POINTER(_C.c_int32)]
    __MDSGetEventQueue=_mdsshr.MDSGetEventQueue
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
        status = __MDSQueueEvent(_ver.tobytes(event),eventid)
        if status&1 == 1:
            return eventid.value
        else:
            raise MdsshrException("Error queuing the event %s, status=%d" % (event,status))

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
        dlen=_C.c_int32(0)
        bptr=_C.c_void_p(0)
        status=__MDSGetEventQueue(eventid,timeout,dlen,bptr)
        if status==1:
            if dlen.value>0:
                ans = _array.Uint8Array(_N.ndarray(shape=[dlen.value],buffer=_ver.buffer(_C.cast(bptr,_C.POINTER((_C.c_byte * dlen.value))).contents),dtype=_N.uint8))
                _mdsshr.MdsFree(bptr)
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
            raise MdsshrException("Unknown error - status=%d" % (status,))
except:
    print('error: _mdsshr.py,l.189')
