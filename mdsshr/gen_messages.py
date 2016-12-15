import xml.etree.ElementTree as ET
import sys,os

sourcedir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sevs = {'warning':0,'success':1,'error':2,'informational':3,'fatal':4}
faclist = []
facnums = {}
msglist = []


def gen_include(root,filename,faclist,msglistm,f_test):
    pfaclist = []
    print filename
    f_inc=open("%s/include/%sh" % (sourcedir,filename[0:-3]),'w')
    f_inc.write("""
#pragma once
/*

 This header was generated using mdsshr/gen_messages.py
 To add new status messages modify:
     %s
 and then in mdsshr do:
     python gen_messages.py
*/

""" % filename)
    f_py.write("""

########################### generated from %s ########################

""" % filename)
    for f in root.getiterator('facility'):
        facnam = f.get('name')
        facnum = int(f.get('value'))
        if facnum in facnums:
            raise Exception("Reused facility value %d, in %s. Previously used in %s" % (facnum, filename, facnums[facnum]))
        facnums[facnum]=filename
        ffacnam = facnam
        faclist.append(facnam)
        for status in f.getiterator('status'):
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
            f_inc.write("#define %s%-20s %s\n" % (facnam,msgnam,hex(msgn)))
            if (facabb):
                facnam=facabb
            if (sfacnam or facabb) and facnam not in faclist:
                    faclist.append(facnam)
            if not facnam in pfaclist:
                pfaclist.append(facnam)
                f_py.write("""

class %(fac)sException(MDSplusException):
  fac="%(fac)s"
""" % {'fac':facnam.capitalize()})
            f_py.write("""

class %(fac)s%(msgnam)s(%(fac)sException):
  status=%(status)d
  message="%(message)s"
  msgnam="%(msgnam)s"

MDSplusException.statusDict[%(msgn_nosev)s] = %(fac)s%(msgnam)s
""" % {'fac':facnam.capitalize(),'msgnam':msgnam.upper(),'status':msgn,'message':text,'msgn_nosev':msgn_nosev})
            msglist.append({'msgnum':hex(msgn_nosev),'text':text,
                            'fac':facnam,'msgnam':msgnam,
                            'facabb':facabb})
    f_inc.close()

f_py=open("%s/mdsobjects/python/mdsExceptions.py"%sourcedir,'w')
f_py.write("""
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify one of the
# "xxxx_messages.xml files (or add a new one)
# and then in mdsshr do:
#     python gen_devices.py
########################################################

class MDSplusException(Exception):
  statusDict={}
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __new__(cls,*argv):
      if len(argv)==0 or cls is not MDSplusException:
          return super(MDSplusException,cls).__new__(cls,*argv)
      status = int(argv[0])
      code   = status & -8
      if code in cls.statusDict:
          cls = cls.statusDict[code]
      elif status == MDSplusError.status:
          cls = MDSplusError
      elif status == MDSplusSuccess.status:
          cls = MDSplusSuccess
      else:
          cls = MDSplusUnknown
      return cls.__new__(cls,*argv)
  def __init__(self,status=None):
    if isinstance(status,int):
      self.status=status
    if not hasattr(self,'status'):
      self.status=-2
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

class MDSplusError(MDSplusException):
  fac="MDSplus"
  severity="E"
  msgnam="Error"
  message="Failure to complete operation"
  status=-8|2  # serverity E
  def __init__(*args): pass

class MDSplusSuccess(MDSplusException):
  fac="MDSplus"
  severity="S"
  msgnam="Success"
  message="Successful execution"
  status=1
  def __init__(*args): pass

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

def checkStatus(status,ignore=[]):
    if (status & 1)==0:
        exception = MDSplusException(status)
        if exception in ignore:
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
f_getmsg=open('%s/mdsshr/MdsGetStdMsg.c'%sourcedir,'w')
for filename,filepath in xmllist.items():
    try:
        tree=ET.parse(filepath)
        root=tree.getroot()
        gen_include(root,filename,faclist,msglist,f_test)
    except Exception,e:
        print e

for fac in faclist:
    f_getmsg.write("static const char *FAC_%s = \"%s\";\n" % (fac,fac.upper()))
f_getmsg.write("""

int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out) {
    int sts;
    switch (status & (-8)) {
""")
exceptionDict=[]
facList=[]
for msg in msglist:
        facList.append(msg['fac'])


for msg in msglist:
    msg['facu']=msg['fac'].upper()
    f_getmsg.write("""
/* %(fac)s%(msgnam)s */
      case %(msgnum)s:
        {static const char *text="%(text)s";
        static const char *msgnam="%(msgnam)s";
        *fac_out = FAC_%(fac)s;
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
