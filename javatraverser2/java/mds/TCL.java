/*
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
package mds;

import mds.data.descriptor_s.Pointer;

public final class TCL{
    private final Mds mds;

    public TCL(final Mds mds){
        this.mds = mds;
    }

    public final int doAction(final Pointer ctx, final int nid) throws MdsException {
        return this.mds.getInteger(ctx, new StringBuilder(37).append("TCL('do '//GetNci(").append(nid).append(",'PATH'))").toString());
    }

    public final String doMethod(final String devicepath, final String method, final String arg, final boolean override) throws MdsException {
        final StringBuilder cmd = new StringBuilder(128).append("DO/METHOD ");
        cmd.append(devicepath).append(' ').append(method);
        if(arg != null) cmd.append(" /ARG=\"").append(arg.replace("\"", "\"\"")).append("\"");
        if(override) cmd.append(" /OVERRIDE");
        return this.mds.tcl(cmd.toString());
    }

    public final String showAttribute() throws MdsException {
        return this.mds.tcl("show a");
    }

    public final String showCurrent(final String expt) throws MdsException {
        return this.mds.tcl("show c " + expt);
    }

    public final String showData(final String nodepath) throws MdsException {
        return this.mds.tcl("show d " + nodepath);
    }

    public final String showDatabase() throws MdsException {
        return this.mds.tcl("show db");
    }

    public final String showDefault() throws MdsException {
        return this.mds.tcl("show def");
    }

    public final String showVersion() throws MdsException {
        return this.mds.tcl("show v");
    }
}
