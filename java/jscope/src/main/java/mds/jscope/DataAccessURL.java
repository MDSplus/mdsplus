package mds.jscope;

import java.io.IOException;
import java.util.Vector;

import mds.provider.access.MdsAccess;
import mds.provider.access.TwuAccess;
import mds.wavedisplay.*;

class DataAccessURL
{
	static Vector<DataAccess> dataAccessVector = new Vector<>();

	static public void addProtocol(DataAccess dataAccess)
	{
		dataAccessVector.addElement(dataAccess);
	}

	static
	{
		DataAccessURL.addProtocol(new MdsAccess());
		DataAccessURL.addProtocol(new TwuAccess());
	}

	static public Signal getSignal(String url) throws IOException
	{
		return getSignal(url, null, null);
	}

	static public Signal getSignal(String url, String passwd) throws IOException
	{
		return getSignal(url, null, passwd);
	}

	static public DataAccess getDataAccess(String url) throws IOException
	{
		for (final DataAccess da : dataAccessVector)
			if (da.supports(url))
				return da;
		throw (new IOException("Protocol not recognized"));
	}

	static public Signal getSignal(String url, String name, String passwd) throws IOException
	{
		DataAccess da = null;
		if ((da = getDataAccess(url)) != null)
		{
			da.setPassword(passwd);
			final Signal s = da.getSignal(url);
			if (s == null && da.getError() == null)
				throw (new IOException("Incorrect password or read signal error"));
			if (da.getError() == null)
			{
				if (name == null)
					name = s.getName();
				if (name == null)
					name = da.getSignalName() + " " + da.getShot();
				else
					name = name + " " + da.getShot();
				s.setName(name);
				return s;
			}
			else
			{
				throw (new IOException(da.getError()));
			}
		}
		return null;
	}

	static public void getImages(String url, Frames f) throws Exception
	{
		getImages(url, null, null, f);
	}

	static public void getImages(String url, String passwd, Frames f) throws Exception
	{
		getImages(url, null, passwd, f);
	}

	static public void getImages(String url, String name, String passwd, Frames f) throws Exception
	{
		DataAccess da = null;
		if ((da = getDataAccess(url)) != null || f == null)
		{
			da.setPassword(passwd);
			final FrameData fd = da.getFrameData(url);
			if (fd == null && da.getError() == null)
				throw (new IOException("Incorrect password or read images error"));
			f.SetFrameData(fd);
			f.setName(da.getSignalName());
			if (da.getError() != null)
			{
				throw (new IOException(da.getError()));
			}
		}
		else
			throw (new IOException("Protocol not recognized"));
	}

	static public void close()
	{
		for (final DataAccess da : dataAccessVector)
			if (da != null)
				da.close();
	}

	static public int getNumProtocols()
	{ return dataAccessVector.size(); }
}
