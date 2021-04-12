package mds.provider;

import mds.connection.MdsConnection;
import mds.provider.mds.MdsConnectionSSH;

public class MdsDataProviderSsh extends MdsDataProvider
{
	@Override
	public MdsConnection getConnection()
	{ return new MdsConnectionSSH(); }

	@Override
	public MdsConnection getConnection(String arg)
	{
		return new MdsConnectionSSH(arg);
	}
}
