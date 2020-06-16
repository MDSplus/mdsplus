package mds.provider;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import javax.swing.JFrame;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;
import mds.wave.*;

public class UniversalDataProvider implements DataProvider
{
	private String error;
	private MdsDataProvider defaultProvider;
	private final Hashtable<String, MdsDataProvider> dataProviderH = new Hashtable<>();

	public UniversalDataProvider()
	{
		defaultProvider = new MdsDataProvider();
	}

	@Override
	public void addConnectionListener(ConnectionListener l)
	{
		defaultProvider.addConnectionListener(l);
	}

	@Override
	public void addUpdateEventListener(UpdateEventListener l, String event) throws IOException
	{
		defaultProvider.addUpdateEventListener(l, event);
	}

	@Override
	public void close()
	{
		final Enumeration<MdsDataProvider> en = dataProviderH.elements();
		while (en.hasMoreElements())
			en.nextElement().close();
	}

	@Override
	public String getError()
	{ return error; }

	private String getExperiment(String in)
	{
		final String subStr1 = in.trim().substring(2);
		final int pos1 = subStr1.indexOf("/");
		final String subStr2 = subStr1.substring(pos1 + 1);
		final int pos2 = subStr2.indexOf("/");
		return subStr2.substring(0, pos2);
	}

	private String getExpr(String spec)
	{
		if (!spec.startsWith("//"))
			return spec;
		final String subStr1 = spec.trim().substring(2);
		final int pos1 = subStr1.indexOf("/");
		final String subStr2 = subStr1.substring(pos1 + 1);
		final int pos2 = subStr2.indexOf("/");
		final String subStr3 = subStr2.substring(pos2 + 1);
		final int pos3 = subStr3.indexOf("/");
		return subStr3.substring(pos3 + 1);
	}

	@Override
	public double getFloat(String in, int row, int col, int index)
	{
		try
		{
			return defaultProvider.getFloat(in, row, col, index);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return 0;
		}
	}

	@Override
	public FrameData getFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		return null;
	}

	private MdsDataProvider getProvider(String ip)
	{
		if (!ip.startsWith("//"))
			return defaultProvider;
		final int pos = ip.substring(2).indexOf("/");
		final String currIp = ip.substring(2, pos + 2);
		MdsDataProvider provider = dataProviderH.get(currIp);
		if (provider == null)
		{
			provider = new MdsDataProvider();
			try
			{
				provider.setArgument(currIp);
				dataProviderH.put(currIp, provider);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}
		return provider;
	}

	private int getShot(String in) throws Exception
	{
		final String subStr1 = in.trim().substring(2);
		final int pos1 = subStr1.indexOf("/");
		final String subStr2 = subStr1.substring(pos1 + 1);
		final int pos2 = subStr2.indexOf("/");
		final String subStr3 = subStr2.substring(pos2 + 1);
		final int pos3 = subStr3.indexOf("/");
		final String subStr4 = subStr3.substring(0, pos3);
		return Integer.parseInt(subStr4);
	}

	@Override
	public long[] getShots(String in, String experiment)
	{
		try
		{
			return defaultProvider.getShots(in, experiment);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return null;
		}
	}

	@Override
	public String getString(String in, int row, int col, int index)
	{
		try
		{
			return defaultProvider.getString(in, row, col, index);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public WaveData getWaveData(String in, int row, int col, int index)
	{
		final MdsDataProvider currProvider = getProvider(in);
		if (currProvider == null)
		{
			error = "Missing default provider";
			return null;
		}
		if (!isDefault(in))
		{
			try
			{
				currProvider.update(getExperiment(in), getShot(in));
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
		try
		{
			return currProvider.getWaveData(getExpr(in), row, col, index);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public WaveData getWaveData(String in_y, String in_x, int row, int col, int index)
	{
		final MdsDataProvider currProvider = getProvider(in_y);
		if (currProvider == null)
		{
			error = "Missing default provider";
			return null;
		}
		if (!isDefault(in_y))
		{
			try
			{
				currProvider.update(getExperiment(in_y), getShot(in_y));
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
		try
		{
			return currProvider.getWaveData(getExpr(in_y), getExpr(in_x), row, col, index);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return null;
		}
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		return 1;
	}

	@Override
	public boolean isBusy()
	{ return false; }

	private boolean isDefault(String in)
	{
		return (!in.startsWith("//"));
	}

	@Override
	public void removeConnectionListener(ConnectionListener l)
	{
		defaultProvider.removeConnectionListener(l);
	}

	@Override
	public void removeUpdateEventListener(UpdateEventListener l, String event) throws IOException
	{
		defaultProvider.removeUpdateEventListener(l, event);
	}

	@Override
	public void setArgument(String arg)
	{
		try
		{
			defaultProvider.setArgument(arg);
		}
		catch (final Exception exc)
		{
			defaultProvider = null;
		}
	}

	@Override
	public void setEnvironment(String exp)
	{
		final Enumeration<MdsDataProvider> en = dataProviderH.elements();
		while (en.hasMoreElements())
		{
			try
			{
				en.nextElement().setEnvironment(exp);
			}
			catch (final Exception exc)
			{}
		}
	}

	@Override
	public boolean supportsTunneling()
	{
		return false;
	}

	@Override
	public void update(String exp, long s)
	{
		defaultProvider.update(exp, s);
	}
}
