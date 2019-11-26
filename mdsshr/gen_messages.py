cpy_header = """
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
"""

gen_header = """ This module was generated using mdsshr/gen_messages.py
 To add new status messages modify
 %s
 and then do:
     python mdsshr/gen_messages.py"""
anyfile = 'one of the "*_messages.xml" files'
def add_c_header(f,filename=anyfile):
    f.write("/*")
    f.write(cpy_header)
    f.write("*/\n")
    f.write("/*%s\n"%("*"*54))
    f.write(gen_header%filename)
    f.write("\n%s*/"%("*"*54))
def add_py_header(f):
    for line in cpy_header.split('\n'):
        line = ('# %s'%line).strip()
        f.write('%s\n'%line)
    f.write('%s\n'%("#"*56))
    for line in (gen_header%anyfile).split('\n'):
        f.write('#%s\n'%line)
    f.write('%s\n'%("#"*56))
py_header = """

class MdsException(Exception):
  pass

class MDSplusException(MdsException):
  fac="MDSplus"
  statusDict={}
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __new__(cls,*argv):
      if len(argv)==0 or cls is not MDSplusException:
          return super(MDSplusException,cls).__new__(cls,*argv)
      status = int(argv[0])
      code   = status & -8
      if code in cls.statusDict:
          cls = cls.statusDict[code]
      else:
          cls = MDSplusUnknown
      return cls.__new__(cls,*argv)
  def __init__(self,status=None,message=None):
    if isinstance(status,int):
      self.status = status
    else:
      if isinstance(status,str):
          message = status
      if not hasattr(self,'status'):
        self.status=PyUNHANDLED_EXCEPTION.status
        self.msgnam='Unknown'
        self.message='Unknown exception'
        self.fac='MDSplus'
    if message is not None:
        message = str(message)
        if len(message)>0:
            self.message = "%s:%s"%(self.message,message)
    self.severity=self.severities[self.status & 7]
    super(Exception,self).__init__(self.message)

  def __str__(self):
    return "%%%s-%s-%s, %s" % (self.fac.upper(),
                               self.severity,
                               self.msgnam,
                               self.message)

class MDSplusUnknown(MDSplusException):
  fac="MDSplus"
  msgnam="Unknown"
  def __init__(self,status):
    self.status=status
    self.severity=self.severities[self.status & 7]
    self.message="Operation returned unknown status value: %s" % str(status)
  def __repr__(self):
    return 'MDSplusUnknown(%s)'%(str(self.status),)

def statusToException(status):
    return MDSplusException(status)

def checkStatus(status,ignore=tuple(),message=None):
    if (status & 1)==0:
        exception = MDSplusException(status,message)
        if isinstance(exception, ignore):
            print(exception.message)
        else:
            raise exception
"""
py_new_file = """
########################### generated from %s ########################

"""
py_exc_class = """

class %(fac)s%(msgnam)s(%(fac)sException):%(depr)s
  status=%(status)d
  message="%(message)s"
  msgnam="%(msgnam)s"

MDSplusException.statusDict[%(msgn_nosev)d] = %(fac)s%(msgnam)s
"""

inc_header = """
#pragma once
#include <status.h>

"""


msg_header = """
#include <mdsplus/mdsconfig.h>

"""
msg_fun_head = """

EXPORT int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out) {
    int sts;
    switch (status & (-8)) {
"""
msg_case = """
/* %(fac)s%(msgnam)s */
      case %(msgnum)s:
        {static const char *text="%(text)s";
        static const char *msgnam="%(msgnam)s";
        *fac_out = FAC_%(facu)s;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;
"""
msg_default = """
    default: sts = 0;
  }
  if (sts == 0) {
    *fac_out=0;
    *msgnam_out=0;
    *text_out=0;
  }
  return sts;
}"""


import xml.etree.ElementTree as ET
import sys,os

sourcedir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sevs = {'warning':0,'success':1,'error':2,'info':3,'fatal':4,'internal':7}
faclist = []
facnums = {}
msglist = []


def gen_include(root,filename,faclist,msglistm,f_test):
    pfaclist = ["MDSplus"]
    print filename
    f_py.write(py_new_file % filename)
    with open("%s/include/%sh" % (sourcedir,filename[0:-3]),'w') as f_inc:
        add_c_header(f_inc,filename)
        f_inc.write(inc_header)
        for f in root.iter('facility'):
            facnam = f.get('name')
            facnum = int(f.get('value'))
            if facnum in facnums:
                raise Exception("Reused facility value %d, in %s. Previously used in %s" % (facnum, filename, facnums[facnum]))
            facnums[facnum]=filename
            ffacnam = facnam
            faclist.append(facnam.upper())
            for status in f.iter('status'):
                facnam = ffacnam
                msgnam = status.get('name')
                msgnum = int(status.get('value'))
                sev = sevs[status.get('severity').lower()]
                msgn = (facnum << 16)+(msgnum << 3)+sev
                text = status.get('text')
                depr = status.get('deprecated',"0")
                sfacnam = status.get('facnam')
                facabb = status.get('facabb')
                if (sfacnam):
                    facnam=sfacnam
                if f_test and facnam != 'Mdsdcl':
                    f_test.write("printf(\"%(msg)s = %%0x, msgnum=%%d,\\n msg=%%s\\n\",%(msg)s,(%(msg)s&0xffff)>>3,MdsGetMsg(%(msg)s));\n" % {'msg':facnam+msgnam})
                msgn_nosev = msgn & (-8)
                inc_line = "#define %-24s %s" % (facnam+msgnam,hex(msgn))
                try:
                    depr = bool(int(depr))
                    if depr:
                        text = '%s (deprecated)'%(text,)
                        inc_line = '%s // deprecated'%(inc_line,)
                        depr = '''
  """ This Exception is deprecated """'''
                    else:
                        depr = ''
                except:
                    depr = "use %s%s"%(facnam,depr.upper())
                    text = '%s (deprecated: %s)'%(text,depr)
                    inc_line = '%s // deprecated: %s'%(inc_line,depr)
                    depr = '''
  """ This Exception is deprecated: %s """'''%(depr,)
                f_inc.write("%s\n"%(inc_line,))
                if (facabb):
                    facnam=facabb
                facu = facnam.upper()
                if (sfacnam or facabb) and facu not in faclist:
                    faclist.append(facu)
                msg = {'msgnum':hex(msgn_nosev),'text':text,
                       'fac':facnam,'facu':facu,'facabb':facabb,'msgnam':msgnam,
                       'status':msgn,'message':text,'msgn_nosev':msgn_nosev,'depr':depr}
                if not facnam in pfaclist:
                    pfaclist.append(facnam)
                    f_py.write("""

class %(fac)sException(MDSplusException):
  fac="%(fac)s"
""" % {'fac':facnam})
                msglist.append(msg)
                f_py.write(py_exc_class % msg)

with open("%s/python/MDSplus/mdsExceptions.py"%sourcedir,'w') as f_py:
    add_py_header(f_py)
    f_py.write(py_header)
    xmllist = {}
    for root,dirs,files in os.walk(sourcedir):
        for filename in files:
            if filename.endswith('messages.xml'):
                xmllist[filename.lower()] = "%s/%s"%(root,filename)

    f_test=None
    if len(sys.argv) > 1:
        f_test=open('%s/mdsshr/testmsg.h'%sourcedir,'w');
    for filename,filepath in xmllist.items():
        try:
            tree=ET.parse(filepath)
            root=tree.getroot()
            gen_include(root,filename,faclist,msglist,f_test)
        except Exception,e:
            print e
    exceptionDict=[]
    with open('%s/mdsshr/MdsGetStdMsg.c'%sourcedir,'w') as f_getmsg:
        add_c_header(f_getmsg)
        f_getmsg.write(msg_header);
        for facu in faclist:
            f_getmsg.write("static const char *FAC_%s = \"%s\";\n" % (facu,facu))
        f_getmsg.write(msg_fun_head)
        for msg in msglist:
            f_getmsg.write(msg_case % msg)
            exceptionDict.append("%(msgnum)s:\"%(fac)s%(msgnam)s\"," % msg)
        f_getmsg.write(msg_default)

if f_test:
    f_test.close()
