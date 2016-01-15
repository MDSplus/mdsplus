import xml.etree.ElementTree as ET
import sys,os

sevs = {'warning':0,'success':1,'error':2,'informational':3,'fatal':4}
faclist = []
facnums = {}
msglist = []


def gen_include(root,file,faclist,msglistm,f_test):
    pfaclist = []
    print file
    
    f_inc=open("../include/%sh" % file.lower()[0:-3],'w')
    f_py=open("../mdsobjects/python/mdsExceptions/%sExceptions.py" % file.lower()[0:-len("_messages.xml")],'w')
    f_inc.write("""
#pragma once
/*

 This header was generated using mdsshr/gen_device.py
 To add new status messages modify: 
     %s
 and then in mdsshr do:
     python gen_devices.py
*/

""" % file)
    f_py.write("""
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     %s
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException
""" % file)
    for f in root.getiterator('facility'):
        facnam = f.get('name')
        facnum = int(f.get('value'))
        if facnum in facnums:
            raise Exception("Reused facility value %d, in %s. Previously used in %s" % (facnum, file, facnums[facnum]))
        facnums[facnum]=file
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
""" % {'fac':facnam.capitalize(),'msgnam':msgnam.upper(),'status':msgn,'message':text})
            msglist.append({'msgnum':hex(msgn_nosev),'text':text,
                            'fac':facnam,'msgnam':msgnam,
                            'facabb':facabb})
    f_inc.close()
    f_py.close()

f_test=None
if len(sys.argv) > 1:
    f_test=open('testmsg.h','w');
f_getmsg=open('MdsGetStdMsg.c','w')

for root,dirs,files in os.walk('../'):
    for file in files:
        if file.endswith('messages.xml'):
            try:
                tree=ET.parse(root+'/'+file)
                root=tree.getroot()
                gen_include(root,file,faclist,msglist,f_test)
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
if f_test:
    f_test.close()
