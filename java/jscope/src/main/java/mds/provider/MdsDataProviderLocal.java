package mds.provider;

import mds.connection.MdsConnection;
import mds.provider.mds.MdsConnectionLocal;

public class MdsDataProviderLocal extends MdsDataProvider
{
	@Override
	public MdsConnection getConnection()
	{ return new MdsConnectionLocal(); }

	@Override
	public MdsConnection getConnection(String arg)
	{
		return this.getConnection();
	}
}
