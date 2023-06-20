#!/usr/bin/env python
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


def add_c_header(f, filename=anyfile):
    f.write("/*")
    f.write(cpy_header)
    f.write("*/\n")
    f.write("/*%s\n" % ("*"*54))
    f.write(gen_header % filename)
    f.write("\n%s*/\n" % ("*"*54))


def add_py_header(f):
    for line in cpy_header.split('\n'):
        line = ('# %s' % line).strip()
        f.write('%s\n' % line)
    f.write('%s\n' % ("#"*56))
    for line in (gen_header % anyfile).split('\n'):
        f.write('#%s\n' % line)
    f.write('%s\n' % ("#"*56))


py_head = """

class MdsException(Exception):
  pass

class MDSplusException(MdsException):
  fac="MDSplus"
  statusDict={}
  severities=["W", "S", "E", "I", "F", "?", "?", "?"]
  def __new__(cls, *argv):
      if not argv or cls is not MDSplusException:
          return super(MDSplusException,cls).__new__(cls, *argv)
      status = int(argv[0])
      code   = status & -8
      if code in cls.statusDict:
          cls = cls.statusDict[code]
      else:
          cls = MDSplusUnknown
      return cls.__new__(cls, *argv)
  def __init__(self, status=None, message=None):
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
        if message:
            self.message = message
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
py_super_class = """

class %(fac)sException(MDSplusException):
  fac="%(fac)s"
"""
py_exc_class = """

class %(fac)s%(msgnam)s(%(fac)sException):%(depr)s
  status=%(status)d
  message="%(text)s"
  msgnam="%(msgnam)s"

MDSplusException.statusDict[%(msgnum)d] = %(fac)s%(msgnam)s
"""

inc_head = """
#ifndef _{base}_{ext}
#define _{base}_{ext}
#include <status.h>

"""

msg_head = """
#include <mdsplus/mdsconfig.h>

"""
msg_fun = """

EXPORT int MdsGetStdMsg(int status, const char **fac_out, const char **msgnam_out, const char **text_out) {
    int sts;
    switch (status & (-8)) {
"""
msg_case = """
/* %(fac)s%(msgnam)s */
      case 0%(msgnum)o:
        {static const char *text="%(text)s";
        static const char *msgnam="%(msgnam)s";
        *fac_out = FAC_%(facu)s;
        *msgnam_out = msgnam;
        *text_out = text;
        sts = 1;}
        break;
"""
msg_tail = """
    default: sts = 0;
  }
  if (sts == 0) {
    *fac_out=0;
    *msgnam_out=0;
    *text_out=0;
  }
  return sts;
}"""

jma_head = """package mds;

import java.awt.Color;
import java.io.IOException;
import javax.swing.JLabel;

public class MdsException extends IOException{
\tprivate static final long serialVersionUID = 1L;
\tpublic static final class MdsAbortException extends MdsException{
\t\tprivate static final long serialVersionUID = 1L;
\t\tpublic MdsAbortException(){
\t\t\tsuper("Transaction aborted", 0);
\t\t}
\t}
\tprivate static JLabel statusLabel = null;
"""
jma_type = "\tpublic static final int %(fac)10s%(msgnam)-30s= 0%(status)o;\n"
jma_fun = """
\tpublic static final String getMdsMessage(final int status) {
\t\tswitch(status){
"""
jma_case = '\t\t\tcase %(fac)s%(msgnam)s:\n\t\t\t\treturn "%%%(facu)s-%(sev)s-%(msgnam)s, %(text)s";\n'
jma_tail = """\t\t\tdefault:
\t\t\t\treturn "%MDSPLUS-?-UNKNOWN, Unknown exception " + status;
\t\t}
\t}

\tpublic final static void handleStatus(final int status) throws MdsException {
\t\tfinal String msg = MdsException.getMdsMessage(status);
\t\tfinal boolean success = (status & 1) == 1;
\t\tif(!success){
\t\t\tfinal MdsException exc = new MdsException(msg, status);
\t\t\tMdsException.stderr(null, exc);
\t\t\tthrow exc;
\t\t}
\t\tMdsException.stdout(msg);
\t}

\tpublic static final void setStatusLabel(final JLabel status) {
\t\tMdsException.statusLabel = status;
\t}

\tpublic static void stderr(final String line, final Exception exc) {
\t\tif(MdsException.statusLabel != null) MdsException.statusLabel.setForeground(Color.RED);
\t\tif(line == null){
\t\t\tif(exc == null){
\t\t\t\tif(MdsException.statusLabel != null) MdsException.statusLabel.setText("");
\t\t\t\treturn;
\t\t\t}
\t\t\tString msg = exc.getMessage();
\t\t\tif(msg == null) msg = exc.toString();
\t\t\tif(MdsException.statusLabel != null) MdsException.statusLabel.setText(msg);
\t\t\tSystem.err.println(msg);
\t\t}else if(exc == null){
\t\t\tif(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s", line));
\t\t\tSystem.err.println(String.format("%s", line));
\t\t}else{
\t\t\tString msg = exc.getMessage();
\t\t\tif(msg == null) msg = exc.toString();
\t\t\tif(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s (%s)", line, msg));
\t\t\tSystem.err.println(String.format("%s\\n%s", line, msg));
\t\t}
\t}

\tpublic static void stdout(final String line) {
\t\tif(MdsException.statusLabel == null) return;
\t\tMdsException.statusLabel.setForeground(Color.BLACK);
\t\tMdsException.statusLabel.setText(line);
\t}

\tprivate static final String parseMessage(final String message) {
\t\tfinal String[] parts = message.split(":", 2);
\t\treturn parts[parts.length - 1];
\t}
\tprivate final int status;

\tpublic MdsException(final int status){
\t\tthis(MdsException.getMdsMessage(status), status);
\t}

\tpublic MdsException(final String message){
\t\tsuper(MdsException.parseMessage(message));
\t\tint new_status = 0;
\t\ttry{
\t\t\tfinal String[] parts = message.split(":", 2);
\t\t\tif(parts.length > 1) new_status = Integer.parseInt(parts[0]);
\t\t}catch(final Exception exc){/**/}
\t\tthis.status = new_status;
\t}

\tpublic MdsException(final String header, final Exception e){
\t\tsuper(String.format("%s: %s", header, e.getMessage()));
\t\tthis.status = 0;
\t}

\tpublic MdsException(final String message, final int status){
\t\tsuper(message);
\t\tthis.status = status;
\t}

\tpublic final int getStatus() {
\t\treturn this.status;
\t}
}"""

# The status code is 32 bits, with these three fields:
#    16 bit facility code in high order bits,
#    13 bit message number, 
#     3 bit severity (low order bits).
#
# The severity scheme is similar to that used by VAX VMS.
# Every odd severity is a flavor of success ("status & 0x1" is easy check).
# Every even severity is a flavor of error.
#   W = warning = 0   -- bad
#   S = success = 1   -- OK
#   E = error = 2     -- bad
#   I = info = 3      -- OK
#   F = fatal = 4     -- bad
#   ? = unused = 5    -- OK
#   ? = unused = 6    -- bad
#   ? = internal = 7  -- OK (see usage in mdsshr/mdsshr_messages.xml)
#
# SsINTERNAL is merely a synonym for -1; it is not a status code.
# If the status variable is ever erroneously set to SsINTERNAL, note
# that the STATUS_OK macro will treat it as success (because the low-order
# bit is set).  See PR #2617 for details.

import xml.etree.ElementTree as ET
import sys
import os

sourcedir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
sevs = {'warning': 0, 'success': 1, 'error': 2,
        'info': 3, 'fatal': 4, 'internal': 7}
faclist = []
facnums = {}
msglist = []
severities = ["W", "S", "E", "I", "F", "?", "?", "?"]


def gen_include(root, filename, faclist, msglistm, f_test):
    pfaclist = ["MDSplus"]
    print(filename)
    with open("%s/include/%sh" % (sourcedir, filename[0:-3]), 'w') as f_inc:
        add_c_header(f_inc, filename)
        parts = filename.upper().split('.')
        f_inc.write(inc_head.format(base=parts[0],ext=parts[1]))
        for f in root.iter('facility'):
            facnam = f.get('name')
            facnum = int(f.get('value'))
            if facnum in facnums:
                raise Exception("Reused facility value %d, in %s. Previously used in %s" % (
                    facnum, filename, facnums[facnum]))
            facnums[facnum] = filename
            ffacnam = facnam
            faclist.append(facnam.upper())
            for status in f.iter('status'):
                facnam = ffacnam
                msgnam = status.get('name')
                msgnum = int(status.get('value'))
                sev = sevs[status.get('severity').lower()]
                msgn = (facnum << 16)+(msgnum << 3)+sev
                text = status.get('text', None)
                if not text:
                    raise Exception("missing or empty text: %s in %s." % (
                        facnam, filename))
                depr = status.get('deprecated', "0")
                sfacnam = status.get('facnam')
                facabb = status.get('facabb')
                if (sfacnam):
                    facnam = sfacnam
                if f_test and facnam != 'Mdsdcl':
                    f_test.write("printf(\"%(msg)s = %%0x, msgnum=%%d,\\n msg=%%s\\n\",%(msg)s,(%(msg)s&0xffff)>>3,MdsGetMsg(%(msg)s));\n" % {
                                 'msg': facnam+msgnam})
                msgnum = msgn & (-8)
                inc_line = "#define %-24s %s" % (facnam+msgnam, hex(msgn))
                try:
                    depr = bool(int(depr))
                    if depr:
                        text = '%s (deprecated)' % (text,)
                        inc_line = '%s // deprecated' % (inc_line,)
                        depr = '\n  """ This Exception is deprecated """'
                    else:
                        depr = ''
                except:
                    depr = "use %s%s" % (facnam, depr.upper())
                    text = '%s (deprecated: %s)' % (text, depr)
                    inc_line = '%s // deprecated: %s' % (inc_line, depr)
                    depr = '\n  """ This Exception is deprecated: %s """' % (
                        depr,)
                f_inc.write("%s\n" % (inc_line,))
                if (facabb):
                    facnam = facabb
                facu = facnam.upper()
                if (sfacnam or facabb) and facu not in faclist:
                    faclist.append(facu)
                msg = {'msgnum': msgnum, 'text': text.replace('"', '\\"'),
                       'fac': facnam, 'facu': facu, 'facabb': facabb, 'msgnam': msgnam,
                       'status': msgn, 'message': text, 'depr': depr,  'sev': severities[msgn & 7]}
                if not facnam in pfaclist:
                    pfaclist.append(facnam)
                msglist.append(msg)
        f_inc.write("#endif")


# gen_msglist():

f_test = None
if len(sys.argv) > 1:
    f_test = open('%s/testmsg.h' % sourcedir, 'w')
for root, dirs, files in os.walk(sourcedir):
    for filename in files:
        if not filename.endswith('messages.xml'):
            continue
        try:
            tree = ET.parse("%s/%s" % (root, filename)).getroot()
            gen_include(tree, filename.lower(), faclist, msglist, f_test)
        except Exception as e:
            print(e)
if f_test:
    f_test.close()

msglist = sorted(msglist, key=lambda item: item['msgnum'])

with open("%s/python/MDSplus/mdsExceptions.py" % sourcedir, 'w') as f_py:
    add_py_header(f_py)
    f_py.write(py_head)
    facs = set([])
    for msg in msglist:
        if not msg['fac'] in facs:
            if msg['fac'] != "MDSplus":
                f_py.write(py_super_class % msg)
            facs.add(msg['fac'])
        f_py.write(py_exc_class % msg)

with open('%s/mdsshr/MdsGetStdMsg.c' % sourcedir, 'w') as f_getmsg:
    add_c_header(f_getmsg)
    f_getmsg.write(msg_head)
    for facu in faclist:
        f_getmsg.write("static const char *FAC_%s = \"%s\";\n" % (facu, facu))
    f_getmsg.write(msg_fun)
    for msg in msglist:
        f_getmsg.write(msg_case % msg)
    f_getmsg.write(msg_tail)

with open("%s/java/mdsplus-api/src/main/java/mds/MdsException.java" % sourcedir, 'w') as f_jma:
    add_c_header(f_jma)
    f_jma.write(jma_head)
    for msg in msglist:
        f_jma.write(jma_type % msg)
    f_jma.write(jma_fun)
    for msg in msglist:
        f_jma.write(jma_case % msg)
    f_jma.write(jma_tail)
