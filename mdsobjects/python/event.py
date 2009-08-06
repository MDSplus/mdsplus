from threading import Thread
import numpy as _N
import ctypes as _C
import os
import time

class Event(Thread):
    """Thread to wait for event"""

    def __init__(self,event):
        """Saves event name and starts wfevent thread
        @param event: name of event to monitor
        @type event: str
        """
        self.event=event
        Thread.__init__(self)
        self.start()

    def __getattribute__(self,name):
        if name == "run":
            return super(Event,self).__getattribute__('_event_run')
        else:
            return super(Event,self).__getattribute__(name)

    def _event_run(self):
        from _mdsshr import MDSWfevent
        while True:
            self.data=MDSWfevent(self.event)
            self.time=time.time()
            self.__class__.run(self)

    def setevent(event,buffer):
        from _mdsshr import MDSEvent
        MDSEvent(event,buffer)
    setevent=staticmethod(setevent)
