from MDSplus import Event
import time

def noCache(response_headers):
    response_headers.append(('Cache-Control','private,no-store, no-cache, must-revalidate, max-age=0,pre-check=0,post-check=0'))
    response_headers.append(('expires','Sat, 26 Jul 1997 09:00:00 GMT'))
    response_headers.append(('Pragma','no-cache'))

def doEvent(self):

    class myevent(Event):
        def run(self):
            self.cancel()

    status = '200 OK'
    response_headers=list()
    noCache(response_headers)
    output=''
    try:
        event=self.path_parts[1]
    except:
        raise Exception("No event string provided, use: /event/event-name-to-waitfor[?timeout=n-secs[&handler=handler]]")
    timeout=60
    try:
        timeout=int(self.args['timeout'][-1])
    except:
        pass
    if 'handler' in self.args:
        specialHandler=__import__(self.args['handler'][-1])
        if hasattr(specialHandler,'handler'):
            specialHandler=specialHandler.handler
        else:
            raise Exception("No handler function found in handler module")
    else:
        specialHandler=None
    e=myevent(event,timeout)
    e.join()
    if e.exception is None:
        if specialHandler is not None:
            status,sresponse_headers,output = specialHandler(e)
            for h in sresponse_headers:
              response_headers.append(h)
        else:
            t=time.ctime(e.time)
            data=e.getRaw()
            response_headers.append(('Content-type','text/xml'))
            output='<?xml version="1.0" encoding="ISO-8859-1" ?>'+"<event><name>%s</name><time>%s</time>" % (event,t)
            if data is not None:
              dtext=""
              for c in data:
                dtext = dtext+chr(int(c))
              output += "<data><bytes>%s</bytes><text>%s</text></data>" % (data.tolist(),dtext)
            output += "</event>"
            status = '200 OK'
    else:
        if 'Timeout' in str(e.exception):
            status = '204 NO_CONTENT'
        else:
            raise e.exception
    return status,response_headers,output

