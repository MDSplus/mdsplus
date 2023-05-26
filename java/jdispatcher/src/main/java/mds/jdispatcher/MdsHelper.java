package mds.jdispatcher;

import java.io.*;
import java.nio.ByteBuffer;
import java.util.*;

class MdsHelper
{
	static final class DispatchCmd
	{
		final String name;
		final String cmd;

		DispatchCmd(String name, String cmd)
		{
			this.name = name;
			this.cmd = cmd;
		}
	}

	private final static Hashtable<String, Integer> name_to_id = new Hashtable<>();
	private final static Hashtable<Integer, String> id_to_name = new Hashtable<>();
	private final static String DEFAULT_FILENAME = "jDispatcher.properties";
	static String experiment = null;
	private static String dispatcher_ip = null;
	private static int dispatcherPort = 0;
	private static Properties properties = null;
	private static final String DISPATCH_FMRT = "jDispatcher.dispatch_%d.%s";
	private static final String SERVER_FMRT = "jDispatcher.server_%d.%s";

	public static void generateEvent(String event, int shot)
	{
		final byte[] shotBytes = ByteBuffer.allocate(4).putInt(shot).array();
		MDSplus.Event.setEventRaw(event, shotBytes);
	}

	static final Vector<DispatchCmd> getDispatch()
	{
		final Vector<DispatchCmd> dispatch = new Vector<>();
		for (int i = 1;; i++)
		{
			final String name = properties.getProperty(String.format(DISPATCH_FMRT, i, "name"));
			final String cmd = properties.getProperty(String.format(DISPATCH_FMRT, i, "cmd"));
			if (name == null || cmd == null)
				break;
			dispatch.add(new DispatchCmd(name.trim(), cmd.trim()));
		}
		return dispatch;
	}

	public static String getDispatcher()
	{ return dispatcher_ip; }

	public static int getDispatcherPort()
	{ return dispatcherPort; }

	public static String getErrorString(int status)
	{
		return MDSplus.Data.getMdsMsg(status);
	}

	public static Vector<ServerInfo> getServers()
	{
		final Vector<ServerInfo> servers = new Vector<>();
		for (int i = 1;; i++)
		{
			String prop = properties.getProperty(String.format(SERVER_FMRT, i, "class"));
			if (prop == null)
				break;
                        final String server_class = prop.trim();

			prop = properties.getProperty(String.format(SERVER_FMRT, i, "address"));
			if (prop == null)
				break;
			final String server_ip = prop.trim();

			final String server_subtree = properties.getProperty(String.format(SERVER_FMRT, i, "subtree"));
			final String javasvr = properties.getProperty(String.format(SERVER_FMRT, i, "use_jserver"));
			final boolean useJavaServer = javasvr == null || javasvr.equals("true");
			int watchdogPort;
			try
			{
				watchdogPort = Integer.parseInt(properties.getProperty(String.format(SERVER_FMRT, i, "watchdog_port")));
			}
			catch (final Exception exc)
			{
				watchdogPort = -1;
			}
			final String startScript = properties.getProperty(String.format(SERVER_FMRT, i, "start_script"));
			final String stopScript = properties.getProperty(String.format(SERVER_FMRT, i, "stop_script"));

			final String showtermScript = properties.getProperty(String.format(SERVER_FMRT, i, "showterm_script"));
			final String showlogScript = properties.getProperty(String.format(SERVER_FMRT, i, "showlog_script"));

			final ServerInfo srvInfo = new ServerInfo(server_class, server_ip, server_subtree, useJavaServer,
					watchdogPort, startScript, stopScript, showtermScript, showlogScript);
			servers.add(srvInfo);
		}
		return servers;
	}

	static Properties initialization(String... args)
	{
		experiment = args.length > 0 ? args[0] : "test";
		if (args.length > 1 && !args[1].trim().isEmpty())
		{
			properties = new Properties();
			try
			{
				try (final InputStream stream = new FileInputStream(args[1]))
				{
					properties.load(stream);
				}
			}
			catch (final IOException e)
			{
				System.err.println("Cannot open specified jDispatcher properties file " + args[1]);
				return null;
			}
		}
		else
		{
			final Vector<String> paths = new Vector<>();
			{// add mdsplus_dir
				final String path = System.getenv("MDSPLUS_DIR");
				if (path != null)
					paths.add(new File(path, "local").getPath());
			}
			{// add home
				final String path = System.getProperty("user.home");
				if (path != null)
					paths.add(new File(path, "jdispatcher").getPath());
			}
			if (System.getProperty("os.name").startsWith("Windows"))
			{ // Windows specific paths
			}
			else
			{ // unix specific paths
				final File etc = new File("/etc/jdispatcher");
				if (etc.exists())
					paths.add(etc.getPath());
			}
			final Vector<String> names = new Vector<>();
			if (experiment != null)
				names.add("jDispatcher_" + experiment.toLowerCase() + ".properties");
			properties = tryOpen(paths, names);
		}
		if (properties == null)
		{
			System.err.println("Cannot open jDispatcher properties files");
			return null;
		}
		for (int i = 1;; i++)
		{
			final String phase_name = properties.getProperty("jDispatcher.phase_" + i + ".name");
			final String phase_id = properties.getProperty("jDispatcher.phase_" + i + ".id");
			if (phase_name == null || phase_id == null)
				break;
			id_to_name.put(new Integer(phase_id), phase_name);
			name_to_id.put(phase_name, new Integer(phase_id));
		}
		dispatcher_ip = properties.getProperty("jDispatcher.dispatcher_ip");
		dispatcherPort = Integer.parseInt(properties.getProperty("jDispatcher.port"));
		return properties;
	}

	public static int toPhaseId(String phase_name)
	{
		try
		{
			return (name_to_id.get(phase_name)).intValue();
		}
		catch (final Exception exc)
		{
			return -1;
		}
	}

	public static String toPhaseName(int phase_id)
	{
		return id_to_name.get(new Integer(phase_id));
	}

	private static final Properties tryOpen(final Vector<String> paths, final Vector<String> names)
	{
		paths.add(null);
		names.add(DEFAULT_FILENAME);
		final Properties properties = new Properties();
		for (final String name : names)
		{
			for (final String path : paths)
			{
				try
				{
					try (final InputStream stream = new FileInputStream(new File(path, name)))
					{
						properties.load(stream);
					}
					return properties;
				}
				catch (final IOException next)
				{
					continue;
				}
			}
		}
		try
		{
			try (final InputStream stream = MdsHelper.class.getClassLoader()
					.getResourceAsStream("mds/jdispatcher/" + DEFAULT_FILENAME))
			{
				properties.load(stream);
				return properties;
			}
		}
		catch (final IOException next)
		{}
		return null;
	}
}
