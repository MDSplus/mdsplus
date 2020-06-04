package mds.jdispatcher;
import java.util.*;

import mds.connection.Descriptor;
import mds.connection.MdsConnection;

public class KillServer extends MdsConnection
{
	KillServer(final String server)
	{
		super(server);
	}

	public static void main(final String args[])
	{
		String serverIp = "";
		int serverPort = 0;

		try {
			serverIp = args[0];
			serverPort = Integer.parseInt(args[1]);
		}
		catch(final Exception exc)
		{
			System.err.println("Usage: java KillServer <ip>, <port>");
			System.exit(0);
		}

		try {
			final KillServer killServer = new KillServer(serverIp + ":"+serverPort);
			killServer.ConnectToMds(false);
			killServer.MdsValue("kill", new Vector<Descriptor>(), false);
			killServer.DisconnectFromMds();
		}
		catch(final Exception exc)
		{
			System.err.println("Cannot connect to server: " + serverIp + ":"+serverPort);
		}
		System.exit(0);
	}
}
