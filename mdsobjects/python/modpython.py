# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

"""Use as a mod_python handler in apache.

Apache configuration example:

<Location "/mdsplusEvents/">
   SetHandler python-program
   PythonHandler MDSplus.modpython
   PythonPath "['/usr/local/myeventhandlers'] + sys.path"
   PythonDebug On

   PythonOption UDP_EVENTS yes
or
   PythonOption EVENT_SERVER host[:port]

</Location>

This configuration will handle requests such as:

http://mywebserver-host/mdsplusEvents/event-name
http://mywebserver-host/mdsplusEvents/event-name?timeout=5
http://mywebserver-host/mdsplusEvents/event-name?timeout=50&handler=myhandler

If no handler is used in the url then the default handler will be used.

The default handler does the following:

  If the event occurs before the timeout (default timeout is 60 seconds):
    returns a page of xml which looks like:
       <event><name>event-name</name><time>time-of-event</time></event> or
       <event><name>event-name</name><time>time-of-event</time><data>
              <data_bytes>[byte1,byte2,byte3,....,byte4]</data_bytes>
              <data_text>text_bytes</data_text></data>
       </event>
  If the event times out then returns an empty page with status of NO_RESPONSE(204)
  If there is an error during processing the event it returns a page of xml which
    looks like:
      <exception>text-of-exception</exception> and a status of BAD_REQUEST

If a handler is specified, a module of the name of handler specified will be
imported and the function called "handler" in that module will be called with
two arguments, the req and the MDSplus event instance in that order. The handler can use the req
object to write the response and the event object to get the event data. The
event-instance.exception will = None if no problem occurred during the event processing or
the exception string if an error occurred. The handler should return a valid
HTTP status if it handles the event otherwise it can return None to resume the
default processing. The following is a handler used on the C-Mod experiment
for monitoring the shot cycle state machine. Information about the state and
shot number is encoded in an event called CMOD_COUNTDOWN_EVENT.

from mod_python import apache
import time
import numpy
import sys,os
states=["starting", "standby", "cooldown", "test", "recool", "init", "check", "pulse", "abort"]
lastData=None
lastTime=time.time()

def handler(req,e):
    if e.exception is None:
        req.content_type="text/xml"
        data=e.getRaw().data()
        shot=int(data[range(4,8)].view(numpy.uint32).item())
        toState=states[int(data[1])]
        fromState=states[int(data[0])]
        req.write('<?xml version="1.0" encoding="ISO-8859-1" ?>')
        req.write("<cmodState><shot>%d</shot><toState>%s</toState><fromState>%s</fromState></cmodState>" % (shot,toState,fromState))
        return apache.OK

"""


from MDSplus import Event
import time
import os
from mod_python import apache
from cgi import parse_qs

class myevent(Event):
    def run(self):
        self.cancel()

def handler(req):
    try:
        opts=req.get_options()
        if "UDP_EVENTS" in opts:
            value=opts["UDP_EVENTS"].upper()
            if value=="YES":
                os.putenv('UDP_EVENTS','yes')
        if "EVENT_SERVER" in opts:
            os.putenv("mds_event_server",opts['EVENT_SERVER'])
    except:
        pass
    os.putenv('UDP_EVENTS','yes')
    event=req.path_info.rsplit('/')[-1]
    args=parse_qs(str(req.args))
    timeout=60
    try:
        timeout=int(args['timeout'][-1])
    except:
        pass
    try:
        event=args['event'][-1]
    except:
        pass
    if 'handler' in args:
        specialHandler=__import__(args['handler'][-1])
        if hasattr(specialHandler,'handler'):
            specialHandler=specialHandler.handler
        else:
            raise(Exception(str(dir(specialHandler))))
    else:
        specialHandler=None
        e=myevent(event,timeout)
        e.join()
    req.headers_out['Cache-Control']='no-store, no-cache, must-revalidate'
    req.headers_out['Pragma']='no-cache'
    req.content_type="text/xml"
    if specialHandler is not None:
        result = specialHandler(req,e)
        if result is not None:
            return result
    if e.exception is None:
        data=e.getRaw()
        t=time.ctime(e.time)
        req.write('<?xml version="1.0" encoding="ISO-8859-1" ?>'+"<event><name>%s</name><time>%s</time>" % (event,t))
        if data is not None:
            dtext=""
            for c in data:
                dtext = dtext+chr(int(c))
            req.write("<data><bytes>%s</bytes><text>%s</text></data>" % (data.tolist(),dtext))
        req.write("</event>")
        return apache.OK
    else:
        if 'Timeout' in str(e.exception):
            req.content_type="text/plain"
            return apache.HTTP_NO_CONTENT
        else:
            req.exception=True
            req.write('<?xml version="1.0" encoding="ISO-8859-1" ?>'+"\n<exception>%s</exception>" % (e.exception,))
            return apache.HTTP_BAD_REQUEST

