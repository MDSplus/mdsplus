package mds.jscope;

import mds.wavedisplay.MdsConnection;

class LocalDataProvider extends MdsDataProvider
{
    public MdsConnection getConnection()
    {
	return new MdsConnectionLocal();
    }
    public MdsConnection getConnection(String arg)
    {
	return new MdsConnectionLocal();
    }
}