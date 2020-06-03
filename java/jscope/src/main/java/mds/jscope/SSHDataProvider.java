package mds.jscope;

import mds.wavedisplay.MdsConnection;

class SSHDataProvider extends MdsDataProvider
{
    public MdsConnection getConnection()
    {
	return new MdsConnectionSSH();
    }
    public MdsConnection getConnection(String arg)
    {
	return new MdsConnectionSSH(arg);
    }
}