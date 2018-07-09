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
    f_inc=open("%s/include/%sh" % (sourcedir,filename[0:-3]),'w')
    f_inc.write(
"""/*
 This header was generated using mdsshr/gen_messages.py
 To add new status messages modify:
     %s
 and then in mdsshr do:
     python gen_messages.py
*/
#pragma once
#include <status.h>

""" % filename)
    f_py.write("""
########################### generated from %s ########################

""" % filename)
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
            sfacnam = status.get('facnam')
            facabb = status.get('facabb')
            if (sfacnam):
                facnam=sfacnam
            if f_test and facnam != 'Mdsdcl':
                f_test.write("printf(\"%(msg)s = %%0x, msgnum=%%d,\\n msg=%%s\\n\",%(msg)s,(%(msg)s&0xffff)>>3,MdsGetMsg(%(msg)s));\n" % {'msg':facnam+msgnam})
            msgn_nosev = msgn & (-8)
            f_inc.write("#define %-24s %s\n" % (facnam+msgnam,hex(msgn)))
            if (facabb):
                facnam=facabb
            facu = facnam.upper()
            if (sfacnam or facabb) and facu not in faclist:
                    faclist.append(facu)
            msg = {'msgnum':hex(msgn_nosev),'text':text,
                   'fac':facnam,'facu':facu,'facabb':facabb,'msgnam':msgnam,
                   'status':msgn,'message':text,'msgn_nosev':msgn_nosev}
            if not facnam in pfaclist:
                pfaclist.append(facnam)
                f_py.write("""

class %(fac)sException(MDSplusException):
  fac="%(fac)s"
""" % {'fac':facnam})
            msglist.append(msg)
            f_py.write("""

class %(fac)s%(msgnam)s(%(fac)sException):
  status=%(status)d
  message="%(message)s"
  msgnam="%(msgnam)s"

MDSplusException.statusDict[%(msgn_nosev)d] = %(fac)s%(msgnam)s
""" % msg)
    f_inc.close()

f_py=open("%s/mdsobjects/python/mdsExceptions.py"%sourcedir,'w')
f_py.write("""# 
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


########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify one of the
# "xxxx_messages.xml files (or add a new one)
# and then in mdsshr do:
#     python gen_devices.py
########################################################

class MDSplusException(Exception):
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
  def __init__(self,status=None):
    if isinstance(status,int):
      self.status=status
    if not hasattr(self,'status'):
      self.status=PyUNHANDLED_EXCEPTION.status
      self.msgnam='Unknown'
      self.message='Unknown exception'
      self.fac='MDSplus'
    if isinstance(status,str):
      self.message = status
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

def checkStatus(status,ignore=tuple()):
    if (status & 1)==0:
        exception = MDSplusException(status)
        if isinstance(exception, ignore):
            print(exception.message)
        else:
            raise MDSplusException(status)
""")

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

f_getmsg=open('%s/mdsshr/MdsGetStdMsg.c'%sourcedir,'w')
exceptionDict=[]
f_getmsg.write("""/*
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
*/
#include <mdsplus/mdsconfig.h>

""");
for facu in faclist:
    f_getmsg.write("static const char *FAC_%s = \"%s\";\n" % (facu,facu))
f_getmsg.write("""

EXPORT int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out) {
    int sts;
    switch (status & (-8)) {
""")

for msg in msglist:
    f_getmsg.write("""
/* %(fac)s%(msgnam)s */
      case %(msgnum)s:
        {static const char *text="%(text)s";
        static const char *msgnam="%(msgnam)s";
        *fac_out = FAC_%(facu)s;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;
""" % msg)
    exceptionDict.append("%(msgnum)s:\"%(fac)s%(msgnam)s\"," % msg)
f_getmsg.write("""
    default: sts = 0;
  }
  if (sts == 0) {
    *fac_out=0;
    *msgnam_out=0;
    *text_out=0;
  }
  return sts;
}""")

f_getmsg.close()
f_py.close()
if f_test:
    f_test.close()
