package mds.provider.mds;

import java.io.*;

import mds.connection.MdsConnection;

public class MdsConnectionSSH extends MdsConnection
{
	Process p;

	public MdsConnectionSSH()
	{}

	public MdsConnectionSSH(String provider)
	{
		this.provider = provider;
	}

	@Override
	public void connectToServer() throws IOException
	{
		host = getProviderHost();
		port = 0;
		user = "";
		ProcessBuilder pb = new ProcessBuilder("mdsip-client-ssh", host, "mdsip-server-ssh");
		try
		{
			final Process p = pb.start();
			dis = new BufferedInputStream(p.getInputStream());
			dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
		}
		catch (final Exception exc)
		{
			pb = new ProcessBuilder("mdsip-client-ssh.bat", host, "mdsip-server-ssh"); // Windows
			final Process p = pb.start();
			dis = new BufferedInputStream(p.getInputStream());
			dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
		}
		this.connected = true;
	}

	@Override
	public int DisconnectFromMds()
	{
		// Closing pipes crashes JVM!!!
		dis = null;
		dos = null;
		p.destroy();
		return super.DisconnectFromMds();
	}

	@Override
	public String getProvider()
	{ return "ssh"; }
}
