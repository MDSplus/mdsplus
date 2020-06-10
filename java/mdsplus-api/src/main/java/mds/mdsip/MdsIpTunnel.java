package mds.mdsip;

import java.io.*;

import mds.mdsip.MdsIp.MdsIpIOStream;

public class MdsIpTunnel extends MdsIpIOStream
{
	private final Process process;
	protected final InputStream dis;
	protected final OutputStream dos;

	public MdsIpTunnel() throws IOException
	{
		process = new ProcessBuilder("mdsip", "-P", "tunnel").start();
		dis = this.process.getInputStream();
		dos = this.process.getOutputStream();
	}

	@Override
	public void close() throws IOException
	{
		if (this.isOpen())
			this.process.destroy();
	}

	@Override
	public boolean isOpen()
	{ return this.process.isAlive(); }
}
