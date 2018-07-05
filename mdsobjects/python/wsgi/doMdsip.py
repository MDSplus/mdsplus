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

import struct
import select
from subprocess import Popen
from tempfile import mkdtemp
import os,sys

class DisconnectException(Exception):
    pass

def getheader(fd):
    header=fd.read(48)
    #print "getheader got %d bytes from %s" % (len(header),str(fd))
    if header=='bye'.ljust(48,'-'):
        raise DisconnectException('Disconnect')
    if len(header)==0:
        #print 'got 0 bytes from %s' % (str(fd),)
        return ('',0,0,0)
    else:
        hdr=struct.unpack('Iihbbbbbbiiiiiiii',header)
        nbytes=hdr[0]-48
        nargs=hdr[3]
        idx=hdr[4]
    return (header,nbytes,nargs,idx)

def getbytes(fd,num):          
    bytes=''
    while len(bytes) < num:
        b=fd.read(num-len(bytes))
        if len(b) == 0:
            raise Exception('short read when getting %d bytes' % (num-len(bytes),))
        else:
            bytes=bytes+b
    return bytes

def getmsg(fd,timeout=None):
    if timeout is None:
        msg=''
        nargs=1000
        idx=-1
        while idx <= (nargs-1):
            header,numbytes,nargs,idx=getheader(fd)
            msg=msg+header+getbytes(fd,numbytes)
    else:
        s=select.select([fd],[],[],timeout)
        if fd in s[0]:
            header,numbytes,nargs,idx=getheader(fd)
            msg = header+getbytes(fd,numbytes)
        else:
            raise Exception('Getmsg timeout')
    return msg

def disconnectMdsip(fifodir):
    #print "Disconnecting"
    try:
        sys.stdout.flush()
        fifo_out=open(fifodir+'/in','r+b')
        fifo_out.write('bye'.ljust(48,'-'))
        fifo_out.flush()
        fifo_out.close()
    except Exception:
        print('got exception shutting down:',sys.exec_info())
        sys.stdout.flush()
    raise Exception('mdsip disconnect')

def doMdsip(self):
    ans=('500 BAD_REQUEST',[('Content-type','text/text')],'unknown error')
    op=self.path_parts[1].lower()
    if op == 'connect':
        tmpdir=mkdtemp(prefix='mdsip_http_')
        if 'MDSPLUS_DIR' in os.environ:
          mdsplus_dir=os.environ['MDSPLUS_DIR']
        else:
          mdsplus_dir='/usr/local/mdsplus'
        p=Popen('setsid %s/bin/mdsip-server-http %s' % (mdsplus_dir,tmpdir),preexec_fn=os.setsid,shell=True)
        if p.wait() == 0:
            ans = ('200 OK',[('Content-type','text/text')],tmpdir)
        else:
            raise Exception('Error running mdsip-server-http process')
    elif op == 'msg':
        if 'HTTP_TMPDIR' in self.environ:
            fifodir=self.environ['HTTP_TMPDIR']
        else:
            raise Exception('No temp dir provided in request')
        sys.stdout=open('%s/mdsip-wsgi.log' % (fifodir,),'a')
        if 'HTTP_FINISHED' in self.environ:
            disconnectMdsip(fifodir)
        msg=''
        #print 'getting msg'
        sys.stdout.flush()
        sys.stdout.flush()
        try:
            msg=getmsg(self.environ['wsgi.input'])
        except DisconnectException:
            disconnectMdsip(fifodir)
        #print 'got message of %d bytes' % (len(msg),)
        #sys.stdout.flush()
        nofifo=True
        tries=0
        #print 'opening fifo in'
        #sys.stdout.flush()
        while nofifo:
            try:
                fifo_out=open(fifodir+'/in','r+b')
                nofifo=False
            except:
                from time import sleep
                tries=tries+1
                if tries > 10:
                    raise Exception('Cannot access server fifo file')
                sleep(1)
        #print 'opened input fifo'
        #print 'writing msg'
        #sys.stdout.flush()
        fifo_out.write(msg)
        #print 'done writing msg'
        fifo_out.flush()
        fifo_out.close()
        #print 'getting response'
        #sys.stdout.flush()
        msg=''
        while len(msg) == 0:
            fifo_in=os.fdopen(os.open(fifodir+'/out',os.O_RDONLY|os.O_NONBLOCK))
            msg=getmsg(fifo_in,3600)
            #print 'got %d byte response' % (len(msg),)
            sys.stdout.flush()
            if len(msg) == 0:
                from time import sleep
                sleep(1)
                fifo_in.close()
        ans = ('200 OK',[('Content-type','application/octet-stream')],msg)
        #print 'Done transaction'
        #sys.stdout.flush()
        sys.stdout.close()
    else:
        raise Exception('Invalid mdsip operation specified')
    return ans

