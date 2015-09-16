import xml.etree.ElementTree as ET
import sys,os

sevs = {'warning':0,'success':1,'error':2,'informational':3,'fatal':4}
faclist = []
facnums = {}
msglist = []


def gen_include(root,file,faclist,msglistm,f_test):
    print file
    f_inc=open('../include/'+file.lower()[0:-3]+'h','w')
    f_inc.write("""

#ifndef %(filenam)s
#define %(filenam)s

""" % {'filenam':file[0:-4].upper()})
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
            msglist.append({'msgnum':hex(msgn_nosev),'text':text,
                            'fac':facnam,'msgnam':msgnam,
                            'facabb':facabb})
    f_inc.write("\n\n#endif\n")
    f_inc.close()

f_test=None
if len(sys.argv) > 1:
    f_test=open('testmsg.h','w');
f_getmsg=open('MdsGetStdMsg.c','w')
f_pyexcept=open('../mdsobjects/python/mdsExceptions.py','w')
f_pyexcept.write("""
_severity=["W", "S", "E", "I", "F", "?", "?", "?"]

""")
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
    if msg['fac'] not in facList:
        f_pyexcept.write(
"""
class %(fac)sException(Exception):
      pass
""" % msg)
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
    f_pyexcept.write("""
class %(fac)s%(msgnam)s(%(fac)sException):
    status=%(msgnum)s
    facility="%(fac)s"
    message="%(text)s"

    def __init__(self,status=None):
      if status is not None:
        self.status=status
      self.severity=_severity[self.status & 7]

    def __str__(self):
        return "%%%%%(facu)s-%%s-%(msgnam)s, %(text)s" %% self.severity
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
f_pyexcept.write("""

def statusToException(msgnum):
    edict={
%s
}
    try:
      return globals()[edict[msgnum & -8]](msgnum)
    except:
      return Exception("?-%%s-?: Unknown status return - %%s" %% (_severity[msgnum & 7],hex(msgnum)))
"""% "\n".join(exceptionDict))
f_pyexcept.close()
f_getmsg.close()
if f_test:
    f_test.close()
