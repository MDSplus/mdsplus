package mds.provider;

import mds.connection.MdsConnection;
import mds.provider.mds.MdsConnectionUdt;

/**
 *
 * @author manduchi
 */
public class MdsDataProviderUdt extends MdsDataProvider
{
	@Override
	protected MdsConnection getConnection()
	{ return new MdsConnectionUdt(); }
}
