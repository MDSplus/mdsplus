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
				provider.SetArgument(currIp);
				dataProviderH.put(currIp, provider);
			}
			catch (final Exception exc)
			{
				return null;
			}
		}
		return provider;
	}

	private boolean isDefault(String in)
	{
		return (!in.startsWith("//"));
	}

	private String getExperiment(String in)
	{
		final String subStr1 = in.trim().substring(2);
		final int pos1 = subStr1.indexOf("/");
		final String subStr2 = subStr1.substring(pos1 + 1);
		final int pos2 = subStr2.indexOf("/");
		return subStr2.substring(0, pos2);
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

	public UniversalDataProvider()
	{
		defaultProvider = new MdsDataProvider();
	}

	@Override
	public void SetArgument(String arg)
	{
		try
		{
			defaultProvider.SetArgument(arg);
		}
		catch (final Exception exc)
		{
			defaultProvider = null;
		}
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
	public WaveData GetWaveData(String in, int row, int col, int index)
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
				currProvider.Update(getExperiment(in), getShot(in));
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
		try
		{
			return currProvider.GetWaveData(getExpr(in), row, col, index);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
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
				currProvider.Update(getExperiment(in_y), getShot(in_y));
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
		try
		{
			return currProvider.GetWaveData(getExpr(in_y), getExpr(in_x), row, col, index);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return null;
		}
	}

	@Override
	public void Dispose()
	{
		final Enumeration<MdsDataProvider> en = dataProviderH.elements();
		while (en.hasMoreElements())
			en.nextElement().Dispose();
	}

	@Override
	public int InquireCredentials(JFrame f, DataServerItem server_item)
	{
		return 1;
	}

	@Override
	public boolean SupportsTunneling()
	{
		return false;
	}

	@Override
	public void SetEnvironment(String exp)
	{
		final Enumeration<MdsDataProvider> en = dataProviderH.elements();
		while (en.hasMoreElements())
		{
			try
			{
				en.nextElement().SetEnvironment(exp);
			}
			catch (final Exception exc)
			{}
		}
	}

	@Override
	public void Update(String exp, long s)
	{
		defaultProvider.Update(exp, s);
	}

	@Override
	public String GetString(String in, int row, int col, int index)
	{
		try
		{
			return defaultProvider.GetString(in, row, col, index);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public double GetFloat(String in, int row, int col, int index)
	{
		try
		{
			return defaultProvider.GetFloat(in, row, col, index);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return 0;
		}
	}

	@Override
	public long[] GetShots(String in, String experiment)
	{
		try
		{
			return defaultProvider.GetShots(in, experiment);
		}
		catch (final Exception exc)
		{
			error = "" + exc;
			return null;
		}
	}

	@Override
	public String ErrorString()
	{
		return error;
	}

	@Override
	public void AddUpdateEventListener(UpdateEventListener l, String event) throws IOException
	{
		defaultProvider.AddUpdateEventListener(l, event);
	}

	@Override
	public void RemoveUpdateEventListener(UpdateEventListener l, String event) throws IOException
	{
		defaultProvider.RemoveUpdateEventListener(l, event);
	}

	@Override
	public void AddConnectionListener(ConnectionListener l)
	{
		defaultProvider.AddConnectionListener(l);
	}

	@Override
	public void RemoveConnectionListener(ConnectionListener l)
	{
		defaultProvider.RemoveConnectionListener(l);
	}

	@Override
	public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		return null;
	}

	@Override
	public boolean isBusy()
	{ return false; }
}
