package mds.wave;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.*;

import javax.swing.JFrame;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;

public class DataServerItem
{
	public static final DataProvider NotConnected = new DataProvider()
	{
		@Override
		public void addConnectionListener(ConnectionListener l)
		{}

		@Override
		public void addUpdateEventListener(UpdateEventListener l, String event)
		{}

		@Override
		public void close()
		{}

		@Override
		public String getError()
		{ return "Not Connected"; }

		@Override
		public double getFloat(String in, int row, int col, int index)
		{
			return Double.parseDouble(in);
		}

		@Override
		public FrameData getFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
		{
			return null;
		}

		@Override
		public long[] getShots(String in, String experiment)
		{
			final long d[] = new long[1];
			d[0] = 0;
			return d;
		}

		@Override
		public String getString(String in, int row, int col, int index)
		{
			return "";
		}

		@Override
		public WaveData getWaveData(String in, int row, int col, int index)
		{
			return null;
		}

		@Override
		public WaveData getWaveData(String in_y, String in_x, int row, int col, int index)
		{
			return null;
		}

		@Override
		public int inquireCredentials(JFrame f, DataServerItem server_item)
		{
			return DataProvider.LOGIN_OK;
		}

		@Override
		public boolean isBusy()
		{ return false; }

		@Override
		public void removeConnectionListener(ConnectionListener l)
		{}

		@Override
		public void removeUpdateEventListener(UpdateEventListener l, String event)
		{}

		@Override
		public void setArgument(String arg)
		{}

		@Override
		public void setEnvironment(String exp)
		{}

		@Override
		public boolean supportsTunneling()
		{
			return false;
		}

		@Override
		public void update(String exp, long s)
		{}
	};

	public static final Set<Class<?>> knownProviders = getClasses("mds.provider", DataProvider.class);

	public static final Set<Class<?>> getClasses(String packageName, Class<?> type)
	{
		final Set<Class<?>> classes = new HashSet<>();
		try
		{
			final ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
			assert classLoader != null;
			final Enumeration<URL> resources = classLoader.getResources(packageName.replace('.', '/'));
			final File directory = new File(resources.nextElement().getFile());
			if (directory.exists())
			{
				final File[] files = directory.listFiles();
				for (final File file : files)
				{
					final String name = file.getName();
					if (!name.endsWith(".class") || name.contains("$"))
						continue;
					try
					{
						classes.add(Class
								.forName(packageName + '.' + file.getName().substring(0, file.getName().length() - 6))
								.asSubclass(type));
					}
					catch (final Exception ignore)
					{}
				}
			}
		}
		catch (final IOException e)
		{
			e.printStackTrace();
		}
		return classes;
	}

	private static final String trimArg(final String arg)
	{
		if (arg == null)
			return null;
		final String trimmed = arg.trim();
		return trimmed.isEmpty() ? null : trimmed;
	}

	private String argument;
	private final String browse_class;
	private final String browse_url;
	private String class_name;
	private String name;
	private String tunnel_port;
	private String user;

	public DataServerItem()
	{
		this("Not Connected", null, null, null, null, null, null);
	}

	public DataServerItem(String user)
	{
		this(null, null, user, null, null, null, null);
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

	public final String getArgument()
	{ return argument; }

	public final String getBrowseClass()
	{ return browse_class; }

	public final String getBrowseUrl()
	{ return browse_url; }

	public final String getClassName()
	{ return class_name; }

	public final String getName()
	{ return name; }

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

	public final String getTunnelPort()
	{ return tunnel_port; }

	public final String getUser()
	{ return user; }

	public boolean isNotConnected()
	{ return this.class_name == null; }

	public void print()
	{
		System.out.println("name: " + this.name);
		System.out.println("argument: " + this.argument);
		System.out.println("user: " + this.user);
		System.out.println("class_name: " + this.getClassName());
		System.out.println("browse_class: " + this.name);
		System.out.println("tunnel_port: " + this.tunnel_port);
	}

	@Override
	public String toString()
	{
		return name;
	}

	public void update(String name, String argument, String user, String class_name, String tunnel_port)
	{
		this.name = trimArg(name);
		this.argument = trimArg(argument);
		this.user = trimArg(user);
		this.class_name = trimArg(class_name);
		this.tunnel_port = trimArg(tunnel_port);
	}
}
