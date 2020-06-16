package mds.provider.mds;

import java.io.*;

import mds.connection.MdsConnection;

public class MdsConnectionLocal extends MdsConnection
{
	Process p;

	public MdsConnectionLocal()
	{
		this.provider = "local";
	}

	public MdsConnectionLocal(String provider)
	{
		this.provider = "local";
	}

	@Override
	public void connectToServer() throws IOException
	{
		host = "local";
		port = 0;
		user = "";
		ProcessBuilder pb = new ProcessBuilder("mdsip-client-local", "mdsip-client-local");
		try
		{
			final Process p = pb.start();
			dis = new BufferedInputStream(p.getInputStream());
			dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
		}
		catch (final Exception e)
		{
			pb = new ProcessBuilder("mdsip-client-local.bat", "mdsip-client-local"); // Windows
			final Process p = pb.start();
			dis = new BufferedInputStream(p.getInputStream());
			dos = new DataOutputStream(new BufferedOutputStream(p.getOutputStream()));
		}
		this.connected = true;
	}

	@Override
	public int DisconnectFromMds()
	{
		dis = null;
		dos = null;
		if (p != null)
			p.destroy();
		return super.DisconnectFromMds();
	}

	@Override
	public String getProvider()
	{ return "local"; }

	@Override
	public void setProvider(String provider)
	{}
}
