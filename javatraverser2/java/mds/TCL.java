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
