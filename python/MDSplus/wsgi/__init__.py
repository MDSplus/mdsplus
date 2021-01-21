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

"""Use as a mod_wsgi handler in apache.

This module provide access to MDSplus events and data and is designed for use with AJAX web based
applications using the XMLHttpRequest object.

A sample apache configuration to enable the use of this module would look something like:

WSGIScriptAlias /mdsplusWsgi /usr/local/cgi/mdsplus.wsgi

Where mdsplus.wsgi contains something like:

from MDSplus.mdsplus_wsgi import application

The mdsplus.wsgi file should configure the environment
initialization such as defining tree paths, UDP_EVENT settings etc.

e.g. :

    from MDSplus import setenv
    setenv('MDS_PATH',     '/usr/local/mdsplus/tdi')
    setenv('mytree_path',  '/mytree-directory-path')
    setenv('mytree2_path', '/mytree2-directory-path')
    setenv('UDP_EVENTS',   'YES')


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

    URL: http://mywebserver-host/mdsplusWsgi/treepath/treename

  getnid - Return node identifier

    URL: http://mywebserver-host/mdsplusWsgi/getNid/treename/shot-number?node=node-name-spec

       example: http://mywebserver-host/mdsplusWsgi/getNid/cmod/-1?node=\ip

"""


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


from MDSplus import Tree, TreeFOPENR
import os
import sys
import numpy
from cgi import parse_qs
for m in os.listdir(os.path.dirname(__file__)):
    if m.startswith("do") and m.endswith(".py"):
        _mimport(m[:-3])

try:
    glob = globals().__dict__
except:
    glob = globals()


class application:
    class Exception(Exception):
        pass

    @staticmethod
    def html_frame(title, body):
        return '<!DOCTYPE html>\n<title>%s</title>\n<html>\n<body>\n%s\n</body>\n</html>' % (title, body)

    @staticmethod
    def body_linebreak(input):
        return input.replace('\n', '<br/>\n')

    @staticmethod
    def table_header(entries):
        return '<tr><th align="left">%s</th></tr>' % ('</th><th align="left">'.join(map(str, entries)),)

    @staticmethod
    def table_row(entries, width=None):
        if width is None:
            return '<tr><td>%s</td></tr>' % ('</td><td>'.join(map(str, entries)),)
        return '<tr><td width=%d%% align="center">%s</td></tr>' % (width, ('</td><td width=%d%% align="center">' % width).join(map(str, entries)),)

    @classmethod
    def table_frame(cls, header, rows):
        head = cls.table_header(header)
        body = '\n'.join([cls.table_row(row) for row in rows])
        return '<table>\n%s\n%s\n</table>' % (head, body)

    @classmethod
    def table(title, header, rows):
        return cls.html_frame(title, table_frame(header, rows))

    @staticmethod
    def link(href, text):
        return '<a href="%s">%s</a>' % (href.replace('"', "&quot;"), text)

    def getReqURI(self):
        return self.environ["REQUEST_URI"].split('?', 2)[0]

    @staticmethod
    def doIndex(self):
        title = "MDSplus WSGI"
        service = self.getReqURI().strip("/")
        rows = []
        for k, v in glob.items():
            if not k.startswith("do"):
                continue
            try:
                link = v.example
            except AttributeError:
                link = "/"+k[2:].lower()
            link = "/%s%s" % (service, link)
            example = self.link(link, self.environ["HTTP_HOST"]+link)
            rows.append((k, example))
        rows.sort()
        #body   = self.table_frame(("name","value"),self.environ.items())
        body = self.table_frame(("module", "example"), rows)
        output = self.html_frame(title, body)
        return ('200 OK', [('Content-type', 'text/html')], output)

    def __init__(self, environ, start_response):
        self.environ = environ
        self.start = start_response
        self.args = parse_qs(self.environ['QUERY_STRING'], keep_blank_values=1)
        self.path_parts = self.environ['PATH_INFO'].split('/')[1:]
        if len(self.path_parts) > 0 and len(self.path_parts[0]) > 0:
            doername = 'do'+self.path_parts[0].capitalize()
            try:
                self.doer = glob[doername].__getattribute__(doername)
            except:
                self.doer = None
        else:
            self.doer = self.doIndex

    def __iter__(self):
        try:
            if self.doer is None:
                self.start('500 BAD_REQUEST', [('Content-type', 'text/plain')])
                output = "Unsupported request: '%s'" % (
                    "/".join(self.path_parts),)
            else:
                status, response_headers, output = self.doer(self)
                self.start(status, response_headers)
            yield output
        except self.Exception as e:
            self.start('500 BAD_REQUEST', [('Content-Type', 'text/html')])
            yield self.html_frame("ERROR", self.body_linebreak(str(e)))
        except Exception:
            import traceback
            self.start('500 BAD_REQUEST', [('Content-Type', 'text/html')])
            yield self.html_frame("EXCEPTION", '<PRE>\n%s\n</PRE>' % traceback.format_exc())

    def openTree(self, tree, shot):
        try:
            shot = int(shot)
        except Exception:
            raise Exception(
                "Invalid shot specified, must be an integer value: %s<br /><br />Error: %s" % (shot, sys.exc_info()))
        try:
            return Tree(tree, shot, "ReadOnly")
        except TreeFOPENR:
            shots = numpy.array(Tree.getShotDB(tree))
            lshots = len(shots)
            idx = numpy.searchsorted(shots, shot, side="left")
            raise self.Exception('\n'.join(["Shot not found; Shots nearby:",
                                            ', '.join(
                                                [str(shots[i]) for i in range(idx-5, idx) if i >= 0]),
                                            ', '.join([str(shots[i]) for i in range(idx, idx+5) if i < lshots])]))
        except:
            pass
        raise self.Exception("Error opening tree named %s for shot %d\nError: %s" % (
            tree, shot, sys.exc_info()))
