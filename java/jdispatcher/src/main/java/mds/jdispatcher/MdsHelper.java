package mds.jdispatcher;

import java.util.*;
import java.io.*;
import java.nio.ByteBuffer;

class MdsHelper {
	private static Hashtable<String, Integer> name_to_id = new Hashtable<String, Integer>();
	private static Hashtable<Integer, String> id_to_name = new Hashtable<Integer, String>();
	private static String dispatcher_ip = null;
	private static int dispatcherPort = 0;
	static String experiment = null;
	private static Hashtable<String, String> dispatch;
	private static final String defaultFileName = "jDispatcher.properties";

	private static final Properties tryOpen(final Vector<String> paths, final Vector<String> names) {
		paths.add(null);
		names.add(defaultFileName);
		final Properties properties = new Properties();
		for (final String name : names) {
			for (final String path : paths) {
				try {
					try (final InputStream stream = new FileInputStream(new File(path, name))) {
						properties.load(stream);
					}
					return properties;
				} catch (IOException next) {
					continue;
				}
			}
		}
		return null;
	}

	static Properties initialization(String... args) {
		experiment = args.length > 0 ? args[0] : "test";
		final Properties properties;
		if (args.length > 1) {
			properties = new Properties();
			try {
				try (final InputStream stream = new FileInputStream(args[1])) {
					properties.load(stream);
				}
			} catch (IOException e) {
				System.err.println("Cannot open specified jDispatcher properties file " + args[1]);
				return null;
			}
		} else {
			final Vector<String> paths = new Vector<String>();
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
			if (System.getProperty("os.name").startsWith("Windows")) { // Windows specific paths

			} else { // unix specific paths
				final File etc = new File("/etc/jdispatcher");
				if (etc.exists())
					paths.add(etc.getPath());
			}
			final Vector<String> names = new Vector<String>();
			if (experiment != null)
				names.add("jDispatcher_" + experiment.toLowerCase() + ".properties");

			properties = tryOpen(paths, names);
		}
		if (properties == null) {
			System.err.println("Cannot open jDispatcher properties files");
			return null;
		}
		for (int i = 1;; i++) {
			final String phase_name = properties.getProperty("jDispatcher.phase_" + i + ".name");
			final String phase_id = properties.getProperty("jDispatcher.phase_" + i + ".id");
			if (phase_name == null || phase_id == null)
				break;
			id_to_name.put(new Integer(phase_id), phase_name);
			name_to_id.put(phase_name, new Integer(phase_id));
		}
		for (int i = 1;; i++) {
			final String name = properties.getProperty("jDispatcher.dispatch_" + i + ".name");
			final String cmd = properties.getProperty("jDispatcher.dispatch_" + i + ".cmd");
			if (name == null || cmd == null)
				break;
			dispatch.put(name, cmd);
		}
		dispatcher_ip = properties.getProperty("jDispatcher.dispatcher_ip");
		dispatcherPort = Integer.parseInt(properties.getProperty("jDispatcher.port"));
		return properties;
	}

	public static void setExperiment(String exp) {
		System.out.println("MdsHelper set experiment " + exp);
		experiment = exp;
	}

	public static void generateEvent(String event, int shot) {
		byte[] shotBytes = ByteBuffer.allocate(4).putInt(shot).array();
		MDSplus.Event.setEventRaw(event, shotBytes);
	}

	public static String getErrorString(int status) {
		return MDSplus.Data.getMdsMsg(status);
	}

	// public static String getErrorString(int status){return "Error message not yet
	// implemented"; }
	public static int toPhaseId(String phase_name) {
		try {
			return (name_to_id.get(phase_name)).intValue();
		} catch (Exception exc) {
			return -1;
		}
	}

	public static String toPhaseName(int phase_id) {
		return id_to_name.get(new Integer(phase_id));
	}

	public static String getDispatcher() {
		return dispatcher_ip;
	}

	public static int getDispatcherPort() {
		return dispatcherPort;
	}
}
