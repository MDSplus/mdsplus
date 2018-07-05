#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
netclient.py interface to client tcp socket with 
- sr() send/receive a command
Created on Sun Jan  8 12:36:38 2017

@author: pgm
"""


import socket
import re
import sys
import os



class Netclient:
    """connects and holds open a socket to defined port.
    
    Args:
        addr (str) : ip-address or dns name on network
        port (int) : server port number.
        
    """
    def receive_message(self, termex, maxlen=4096):
        """Read the information from the socket line at a time.
    
        Args:
            termex (str): regex defines line terminator
            maxlen (int): max read size
            
        Returns:
            string representing message        
        """        

        match = termex.search(self.buffer)
        while match == None:
            self.buffer += self.sock.recv(maxlen)        
            match = termex.search(self.buffer)
            
        rc = self.buffer[:match.start(1)]            
        self.buffer = self.buffer[match.end(1):]
        return rc
    
    trace = int(os.getenv("NETCLIENT_TRACE", "0"))   
                
    def __init__(self, addr, port) :
#        print("Netclient.init")
        self.buffer = ""
        self.__addr = addr
        self.__port = int(port)
        self.sock = socket.socket()
        if Netclient.trace:
            print("Netclient(%s, %d) connect" % (self.__addr, self.__port))
        self.sock.connect((self.__addr, self.__port))

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_value, traceback):
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
        except socket.error:
            pass
        self.sock.close()
            
    #@property
    def addr(self):
        return self.__addr
    
    #@property
    def port(self):
        return self.__port
    
    def __repr__(self):
        return 'Netclient(%s, %d)' % (self.__addr, self.__port)

class Logclient(Netclient):
    """Netclient optimised for logging, line by line"""
    def __init__(self, addr, port):
       Netclient.__init__(self,addr, port)
       self.termex = re.compile("(\r\n)")
       
    def poll(self):
        return self.receive_message(self.termex)


class Siteclient(Netclient):   
    """Netclient optimised for site service, may be multi-line response.
    
    Autodetects all knobs and holds them as properties for simple script-like
    set/get syntax.
    """
    knobs = {}
    prevent_autocreate = False
    pat = re.compile(r":")
    
    def sr(self, message):
        """send a command and receive a reply
        
        Args:
            message (str) : command (query) to send
            
        Returns:
            rx (str): response string
        """
        if (self.trace):
            print("%s >%s" % (repr(self), message.rstrip()))
        self.sock.send((message+"\n").encode())
        rx = self.receive_message(self.termex).rstrip()
        if self.show_responses and len(rx) > 1:
            print(rx)
        if (self.trace):
            print("%s <%s" % (repr(self), rx))
        return rx
 
    
    def build_knobs(self, knobstr):
# http://stackoverflow.com/questions/10967551/how-do-i-dynamically-create-properties-in-python
        self.knobs = dict((Siteclient.pat.sub(r"_", key), key) for key in knobstr.split())
        
    def help(self, regex = ".*"):
        """list available knobs, optionally filtered by regex.
        
        eg
        
        - help()  : list all
        - help("SIG) : list all knobs with SIG
        - help("SIG*FREQ") list all knobs SIG*FREQ        
        """
        regex = re.compile(regex)
        hr = []
        for key in sorted(self.knobs):
            if regex.match(key):
                hr.append(key)                
        return hr
            

    def __getattr__(self, name):
        if self.knobs == None:
            return object.__setattr__(self, name)
        if self.knobs.get(name) != None:
                return self.sr(self.knobs.get(name))
        else:
                msg = "'{0}' object has no attribute '{1}'"
                raise AttributeError(msg.format(type(self).__name__, name))

    def __setattr__(self, name, value):
        if self.knobs == None:
            return object.__setattr__(self, name, value)                 
        if self.knobs.get(name) != None:
            return self.sr("%s=%s" % (self.knobs.get(name), value))
        elif not self.prevent_autocreate or self.__dict__.get(name) != None:
            self.__dict__[name] = value
        else:
            msg = "'{0}' object has no attribute '{1}'"
            raise AttributeError(msg.format(type(self).__name__, name))          
                

    def get_knob(self, name):
        return self.__getattr__(name)
    def set_knob(self, name, value):
        return self.__setattr__(name, value)
    def __repr__(self):
        return 'Siteclient(%s, %d)' % (self.addr(), self.port())   
    
    trace = int(os.getenv("SITECLIENT_TRACE", "0"))
    
    def __init__(self, addr, port):
#        print("Siteclient.init")
        self.knobs = {}
        
        self.show_responses = False
        Netclient.__init__(self, addr, port) 
    # no more new props once set
        self.prevent_autocreate = False
        self.termex = re.compile(r"\n(acq400.[0-9]+ ([0-9]+) >)")
        self.trace = 1 if Siteclient.trace > 1 else 0
        self.sr("prompt on")
        self.build_knobs(self.sr("help"))
        self.trace = Siteclient.trace
        self.prevent_autocreate = True
        #self.show_responses = True


def run_unit_test():
    SERVER_ADDRESS = 'acq2106_066'
    SERVER_PORT=4233

    if len(sys.argv) > 1:
        SERVER_ADDRESS = sys.argv[1]
        if len(sys.argv) > 2:
            SERVER_PORT = int(sys.argv[2])

    print("create Netclient %s %d" %(SERVER_ADDRESS, SERVER_PORT))
    svc = Siteclient(SERVER_ADDRESS, SERVER_PORT)
    
    
    print("Model: %s" % (svc.MODEL))
    print("SITELIST: %s" % (svc.SITELIST))
    print("software_version: %s" % (svc.software_version))
    svc.trace = True
    print("spad1: %s" % (svc.spad1))
    svc.spad1 = "0x1234"
    print("spad1: %s" % (svc.spad1))
    svc.spad1 = "0x5678"
    print("spad1: %s" % (svc.spad1))
    svc.spad2 = "0x22222222"
    
    raise SystemExit
    for key in svc.knobs:
        cmd = svc.knobs[key]
        if cmd.startswith("help"):
            continue
        print("%s %s" % (cmd, svc.sr(cmd)))

    raise SystemExit
 
    while True:
        try:
            data = raw_input("Enter some data: ")
        except EOFError:
            print("\nOkay. Leaving. Bye")
            break

        print("Hello")

        if not data:
            print("Can't send empty string!")
            print("Ctrl-D [or Ctrl-Z on Windows] to exit")
            continue
        print("< %s" % (data))

        data += "\n"
        svc.send(data)
        data = svc.recv()
        
        print("Got this string from server:")
        print(data + '\n')

# excution starts here        
if __name__ == '__main__':
    run_unit_test




    
