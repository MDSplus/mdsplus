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
import os

targetdir = os.path.dirname(os.path.realpath(__file__))
sourcedir = "%s\\mdsplus"%os.path.dirname(os.path.dirname(targetdir))
sevs = {'warning':0,'success':1,'error':2,'info':3,'fatal':4,'internal':7}
faclist = []
facnums = {}
msglist = []
caselist = []


with open("%s/java/mds/MdsException.java"%targetdir,'w') as jfile:
    jfile.write("""/*
* This module was generated using mdsshr/gen_device.py
* To add new status messages modify one of the
* "xxxx_messages.xml files (or add a new one)
* and then in mdsshr do:
*     python gen_devices.py
*/
package mds;

import java.awt.Color;
import java.io.IOException;
import javax.swing.JLabel;

@SuppressWarnings("serial")
public class MdsException extends IOException{
    public static final class MdsAbortException extends MdsException{
        public MdsAbortException(){
            super("Transaction aborted", 0);
        }
    }
    private static JLabel   statusLabel                      = null;
""")

    xmllist = {}
    for root,dirs,files in os.walk(sourcedir):
        for filename in files:
            if filename.endswith('messages.xml'):
                xmllist[filename.lower()] = "%s/%s"%(root,filename)

    for filename,filepath in xmllist.items():
        try:
            tree=ET.parse(filepath)
            root=tree.getroot()
            pfaclist = []
            for f in root.iter('facility'):
                sfacnam = f.get('name')
                facnum = int(f.get('value'))
                if facnum in facnums:
                    raise Exception("Reused facility value %d, in %s. Previously used in %s" % (facnum, filename, facnums[facnum]))
                facnums[facnum]=filename
                faclist.append(sfacnam.upper())
                for status in f.iter('status'):
                    facnam = status.get('facnam')
                    facabb = status.get('facabb')
                    if (facabb):     facnam=facabb
                    elif not facnam: facnam=sfacnam
                    msgnam = facnam+status.get('name')
                    msgnum = int(status.get('value'))
                    sev = sevs[status.get('severity').lower()]
                    msgn = (facnum << 16)+(msgnum << 3)+sev
                    text = status.get('text')
                    if not msgnam in pfaclist:
                        pfaclist.append(msgnam)
                        jfile.write("    public static final int %-33s=%10d;\n"%(msgnam,msgn))
                        caselist.append((msgnam,text))
        except Exception,e:
            print e
    jfile.write("""
    public static final String getMdsMessage(final int status) {
        switch(status){
            default:
                return "%MDSPLUS-?-UNKNOWN, Unknown exception " + status;
""")
    for case in caselist:
        jfile.write('            case %s:\n                return "%s";\n'%case)
    jfile.write("""        }
    }

    public final static void handleStatus(final int status) throws MdsException {
        final String msg = MdsException.getMdsMessage(status);
        final boolean success = (status & 1) == 1;
        if(!success){
            final MdsException exc = new MdsException(msg, status);
            MdsException.stderr(null, exc);
            throw exc;
        }
        MdsException.stdout(msg);
    }

    private static final String parseMessage(final String message) {
        final String[] parts = message.split(":", 2);
        return parts[parts.length - 1];
    }

    public static final void setStatusLabel(final JLabel status) {
        MdsException.statusLabel = status;
    }

    public static void stderr(final String line, final Exception exc) {
        if(MdsException.statusLabel != null) MdsException.statusLabel.setForeground(Color.RED);
        if(line == null){
            if(exc == null){
                if(MdsException.statusLabel != null) MdsException.statusLabel.setText("");
                return;
            }
            String msg = exc.getMessage();
            if(msg == null) msg = exc.toString();
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(msg);
            System.err.println(msg);
        }else if(exc == null){
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s", line));
            System.err.println(String.format("%s", line));
        }else{
            String msg = exc.getMessage();
            if(msg == null) msg = exc.toString();
            if(MdsException.statusLabel != null) MdsException.statusLabel.setText(String.format("E:%s (%s)", line, msg));
            System.err.println(String.format("%s\\n%s", line, msg));
        }
    }

    public static void stdout(final String line) {
        if(MdsException.statusLabel == null) return;
        MdsException.statusLabel.setForeground(Color.BLACK);
        MdsException.statusLabel.setText(line);
    }
    private final int status;

    public MdsException(final int status){
        this(MdsException.getMdsMessage(status), status);
    }

    public MdsException(final String message){
        super(MdsException.parseMessage(message));
        int new_status = 0;
        try{
            final String[] parts = message.split(":", 2);
            if(parts.length > 1) new_status = Integer.parseInt(parts[0]);
        }catch(final Exception exc){/**/}
        this.status = new_status;
    }

    public MdsException(final String header, final Exception e){
        super(String.format("%s: %s", header, e.getMessage()));
        this.status = 0;
    }

    public MdsException(final String message, final int status){
        super(message);
        this.status = status;
    }

    public final int getStatus() {
        return this.status;
    }
}
""")