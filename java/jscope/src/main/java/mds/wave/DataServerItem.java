package mds.wave;

import java.io.IOException;

import javax.swing.JFrame;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;

public class DataServerItem
{
	private static class NotConnectedDataProvider implements DataProvider
	{
		@Override
		public WaveData GetWaveData(String in, int row, int col, int index)
		{
			return null;
		}

		@Override
		public WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
		{
			return null;
		}

		@Override
		public void Dispose()
		{}

		@Override
		public int InquireCredentials(JFrame f, DataServerItem server_item)
		{
			return DataProvider.LOGIN_OK;
		}

		@Override
		public void SetArgument(String arg)
		{}

		@Override
		public boolean SupportsTunneling()
		{
			return false;
		}

		@Override
		public void SetEnvironment(String exp)
		{}

		@Override
		public void Update(String exp, long s)
		{}

		@Override
		public String GetString(String in, int row, int col, int index)
		{
			return "";
		}

		@Override
		public double GetFloat(String in, int row, int col, int index)
		{
			return Double.parseDouble(in);
		}

		@Override
		public long[] GetShots(String in, String experiment)
		{
			final long d[] = new long[1];
			d[0] = 0;
			return d;
		}

		@Override
		public String ErrorString()
		{
			return "Not Connected";
		}

		@Override
		public void AddUpdateEventListener(UpdateEventListener l, String event)
		{}

		@Override
		public void RemoveUpdateEventListener(UpdateEventListener l, String event)
		{}

		@Override
		public void AddConnectionListener(ConnectionListener l)
		{}

		@Override
		public void RemoveConnectionListener(ConnectionListener l)
		{}

		@Override
		public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
		{
			return null;
		}

		@Override
		public boolean isBusy()
		{ return false; }
	}

	private String name;
	private String argument;
	private String user;
	private String class_name;
	private final String browse_class;
	private final String browse_url;
	private String tunnel_port;
	public static final DataProvider NotConnected = new NotConnectedDataProvider();
	public static String knownProviders[] =
	{ "MdsDataProvider", "MdsDataProviderUdt", "MdsDataProviderAsync", "MdsDataProviderStreaming", "MdsDataProviderSsh",
			"MdsDataProviderLocal", "AsciiDataProvider", "AsdexDataProvider", "FtuDataProvider", "JetDataProvider",
			"JetMdsDataProvider", "JetDataProvider", "TsDataProvider", "TwuDataProvider", "UniversalDataProvider" };

	public boolean isNotConnected()
	{ return this.class_name == null; }

	public final String getName()
	{ return name; }

	public final String getArgument()
	{ return argument; }

	public final String getUser()
	{ return user; }

	public final String getClassName()
	{ return class_name; }

	public final String getBrowseUrl()
	{ return browse_url; }

	public final String getBrowseClass()
	{ return browse_class; }

	public final String getTunnelPort()
	{ return tunnel_port; }

	private static final String trimArg(final String arg)
	{
		if (arg == null)
			return null;
		final String trimmed = arg.trim();
		return trimmed.isEmpty() ? null : trimmed;
	}

	public DataServerItem(String user)
	{
		this(null, null, user, null, null, null, null);
	}

	public DataServerItem()
	{
		this("Not Connected", null, null, null, null, null, null);
	}

	public DataServerItem(String name, String argument, String user, String class_name, String browse_class,
			String browse_url, String tunnel_port)
	{
		this.name = trimArg(name);
		this.argument = trimArg(argument);
		this.user = trimArg(user);
		this.class_name = trimArg(class_name);
		this.browse_class = trimArg(browse_class);
		this.browse_url = trimArg(browse_url);
		this.tunnel_port = trimArg(tunnel_port);
	}

	@Override
	public String toString()
	{
		return name;
	}

	public boolean equals(DataServerItem dsi)
	{
		try
		{
			final boolean out = name.equals(dsi.name);
			// &&
			// argument.equals(dsi.argument) &&
			// class_name.equals(dsi.class_name) &&
			// browse_class.equals(dsi.browse_class);
			return out;
		}
		catch (final Exception exc)
		{
			return false;
		}
	}

	public boolean equals(String name)
	{
		return this.name.equals(name);
	}

	public void print()
	{
		System.out.println("name: " + this.name);
		System.out.println("argument: " + this.argument);
		System.out.println("user: " + this.user);
		System.out.println("class_name: " + this.getClassName());
		System.out.println("browse_class: " + this.name);
		System.out.println("tunnel_port: " + this.tunnel_port);
	}

	public void update(String name, String argument, String user, String class_name, String tunnel_port)
	{
		this.name = trimArg(name);
		this.argument = trimArg(argument);
		this.user = trimArg(user);
		this.class_name = trimArg(class_name);
		this.tunnel_port = trimArg(tunnel_port);
	}

	public DataProvider getProvider() throws Exception
	{
		if (this.isNotConnected())
			return NotConnected;
		try
		{
			Class<?> cl;
			if (class_name.contains("."))
				cl = Class.forName(class_name);
			else
				cl = Class.forName("mds.provider." + class_name);
			return (DataProvider) cl.newInstance();
		}
		catch (final Exception e)
		{
			throw (new Exception("Can't load data provider class: " + class_name + "\n" + e));
		}
	}
}
