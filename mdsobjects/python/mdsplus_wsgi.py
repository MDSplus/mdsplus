from MDSplus import *
import time
import os
import sys

from cgi import parse_qs

"""Use as a mod_wsgi handler in apache.

This module provide access to MDSplus events and data and is designed for use with AJAX web based
applications using the XMLHttpRequest object.

A sample apache configuration to enable the use of this module would look something like:

WSGIScriptAlias /mdsplusWsgi /usr/local/cgi/mdsplus.wsgi

Where mdsplus.wsgi contains something like:

from MDSplus.mdsplus_wsgi import application

The application class in this module will add the directory where the mdsplus.wsgi file resides to the python
path and then try to import a module named mdsplus_wsgi_config. If you provide a file called
mdsplus_wsgi_config.py in the same directory as the mdsplus.wsgi then you can perform some environment
initialization such as defining tree paths, UDP_EVENT settings etc.

As sample mdsplus_wsgi_config.py file might contain:

    import os
    os.environ['MDS_PATH']='/usr/local/mdsplus/tdi'
    os.environ['mytree_path']='/mytree-directory-path'
    os.environ['mytree2_path']='/mytree2-directory-path'
    os.environ['UDP_EVENTS']='YES'


Once the mdsplus.wsgi is configured the web server will serve requests with the following format:

http://mywebserver-host/mdsplusWsgi/request-type/[param0[/param1[.../paramn]]?query-options

Currently the following request-types are supported:

  event - Wait for an MDSplus event and produce either a xml document or hand the event info over to a python
          handler to format a custom response.

          URL: http://mywebserver-host/mdsplusWsgi/event/event-name[?timeout=seconds[&handler=handler-module]]

    If no handler is specified then the default handler will be used.

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
         <exception>text-of-exception</exception>

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

       import time
       import numpy
       import sys,os
       states=["starting", "standby", "cooldown", "test", "recool", "init", "check", "pulse", "abort"]
       lastData=None
       lastTime=time.time()

       def handler(e):
         if e.exception is None:
           response_headers=list()
           response_headers.append(('Content-type','text/xml'))
           status='200 OK'
           data=e.getRaw()
           shot=int(data[range(4,8)].view(numpy.uint32).item())
           toState=states[int(data[1])]
           fromState=states[int(data[0])]
           output='<?xml version="1.0" encoding="ISO-8859-1" ?>\n'
           output += '<cmodState>\n  <shot>%d</shot>\n  <toState>%s</toState>\n  <fromState>%s</fromState>\n</cmodState>" % (shot,toState,fromState))
           return (reponse_headers,status,output)

  1darray - Evaluate an expression and return a 1-D array of numeric values

    URL: http://mywebserver-host/mdsplusWsgi/1darray/[tree-name/shot-number]?expr=expression

    This request type will result in binary data returned. Request headers returned returned include one or more of
    DTYPE (i.e. "Float32Array"), LENGTH, TREE, SHOT, ERROR.

    In javascript you would read the data using code similar to:

        var a = eval('new '+req.getResponseHeader('DTYPE')+'(req.response)');

        NOTE: Typed arrays in javascript may not be supported in all browsers yet. 
                 
    If there is an error then there will be a request header returned called "ERROR" and its value is the error message.
    
  1dsignal - Evaluate an expression and resturn a 1-D array of x values followed by a 1-D array of y values.

     URL: http://mywebserver-host/mdsplusWsgi/1dsignal/[tree-name/shot-number]?expr=expression

     Similar to the array mode but request headers returned include XDTYPE,XLENGTH,YDTYPE,YLENGTH. The contents returned
     consist of the x axis followed by the y axis. In javascript you would read the data using something like:
                        
        var xlength=req.getResponseHeader('XLENGTH')
        var x = eval('new '+req.getResponseHeader('XDTYPE')+'(req.response,xlength)');
        var y = eval('new '+req.getResponseHeader('YDTYPE')+'(req.response,xlength*x.BYTES_PER_ELEMENT)');
    
        NOTE: Typed arrays in javascript may not be supported in all browsers yet.

  treepath - Return tree_path environment variable

    URL: http://mywebserver-host/mdsplusWsgi/treename

  getnid - Return node identifier

    URL: http://mywebserver-host/mdsplusWsgi/treename/shot-number?node=node-name-spec

       example: http://mywebserver-host/mdsplusWsgi/cmod/-1?node=\ip

"""


class application:

    def __init__(self, environ, start_response):
    	sys.path.insert(0,os.path.dirname(environ['SCRIPT_FILENAME']))
    	try:
	  import mdsplus_wsgi_config
	except:
	  pass
        self.environ = environ
        self.start = start_response
        self.tree=None
        self.shot=None
	self.args=parse_qs(self.environ['QUERY_STRING'],keep_blank_values=1)
	self.path_parts=self.environ['PATH_INFO'].split('/')[1:]
        doername='do'+self.path_parts[0].capitalize()
	if doername in vars(application):
          self.doer=vars(application)[doername]
        else:
          self.doer=None

    def __iter__(self):
      try:
        if self.doer is None:
          status = '500 BAD_REQUEST'
          response_headers=[('Content-type','text/text')]
	  self.start('500 BAD_REQUEST',[('Content-type','text/text')])
          output="Unsupported request type: "+self.path_parts[0]
        else:
          status, response_headers,output = self.doer(self)
	self.start(status,response_headers)
	yield output
      except Exception,e:
        self.start('500 BAD REQUEST',[('Content-Type','text/xml')])
	yield '<?xml version="1.0" encoding="ISO-8859-1" ?>'+"\n<exception>%s</exception>" % (str(e),) 

    def doEvent(self):

        class myevent(Event):
            def run(self):
                self.cancel()

        status = '200 OK'
    	response_headers=list()
        response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
        response_headers.append(('Pragma','no-cache'))
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

    def do1darray(self):
        if len(self.path_parts) > 2:
           self.openTree(self.path_parts[1],self.path_parts[2])
        expr=self.args['expr'][-1]
        try:
           a=makeData(Data.execute(expr).data())
        except Exception,e:
           raise Exception("Error evaluating expression: '%s', error: %s" % (expr,e))
    	response_headers=list()
        response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
        response_headers.append(('Pragma','no-cache'))
        response_headers.append(('DTYPE',a.__class__.__name__))
        response_headers.append(('LENGTH',str(len(a))))
        if self.tree is not None:
           response_headers.append(('TREE',self.tree))
           response_headers.append(('SHOT',self.shot))
        output=str(a.data().data)
        status = '200 OK'
	return (status, response_headers, output)

    def do1dsignal(self):
        if len(self.path_parts) > 2:
           self.openTree(self.path_parts[1],self.path_parts[2])
        expr=self.args['expr'][-1]
        try:
           sig=Data.execute(expr)
           y=makeData(sig.data())
           x=makeData(sig.dim_of().data())
        except Exception,e:
           raise Exception("Error evaluating expression: '%s', error: %s" % (expr,e))
    	response_headers=list()
        response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
        response_headers.append(('Pragma','no-cache'))
        response_headers.append(('XDTYPE',x.__class__.__name__))
        response_headers.append(('YDTYPE',y.__class__.__name__))
        response_headers.append(('XLENGTH',str(len(x))))
        response_headers.append(('YLENGTH',str(len(y))))
        if self.tree is not None:
           response_headers.append(('TREE',self.tree))
           response_headers.append(('SHOT',self.shot))
        output=str(x.data().data)+str(y.data().data)
        status = '200 OK'
	return (status, response_headers, output)

    def doTreepath(self):
        tree=self.path_parts[1].lower()
        if tree+'_path' in os.environ:
          return ('200 OK', [('Content-type','text/text')],os.environ[tree+'_path'])
        else:
          return ('400 BAD_REQUEST',[('Content-type','text/text')],'No path defined for '+tree)

    def doGetnid(self):
        self.openTree(self.path_parts[1],self.path_parts[2])
        output=str(self.treeObj.getNode(self.args['node'][-1]).nid)
        return ('200 OK',[('Content-type','text/text')],output)

    def openTree(self,tree,shot):
        try:
            shot=int(shot)
    	except Exception,e:
            raise Exception("Invalid shot specified, must be an integer value: %s<br /><br />Error: %s" % (shot,e))
        try:
	    t=Tree(tree,shot)
            self.tree=t.tree
            self.shot=str(t.shot)
            self.treeObj=t
        except Exception,e:
            raise Exception("Error opening tree named %s for shot %d<br /><br />Error: %s" % (tree,shot,e))

    def doImage(self):
        if len(self.path_parts) > 2:
           self.openTree(self.path_parts[1],self.path_parts[2])
        expr=self.args['expr'][-1]
        try:
           d=Data.execute(expr)
           try:
             im=d.getImage()
           except:
             raise Exception("Expression does not evaluate to an image type")
           if im.format == "MPEG":
             response_headers=[('Content-type','video/mpeg'),('Content-Disposition','inline; filename="%s.mpeg"' % (expr,))]
           elif im.format == "GIF":
             response_headers=[('Content-type','image/gif'),('Content-Disposition','inline; filename="%s.gif"' % (expr,))]
           elif im.format == "JPEG":
             response_headers=[('Content-type','image/jpeg'),('Content-Disposition','inline; filename="%s.jpeg"' % (expr,))]
           else:
             raise Exception("not an known image type")
           output=str(d.getData().data().data)
        except Exception,e:
           raise Exception("Error evaluating expression: '%s', error: %s" % (expr,e))
        status = '200 OK'
        return (status, response_headers, output)


