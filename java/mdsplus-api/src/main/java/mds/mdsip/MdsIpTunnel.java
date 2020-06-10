package mds.mdsip;

import java.io.IOException;

import mds.mdsip.MdsIp.MdsIpIOStream;

public class MdsIpTunnel extends MdsIpIOStream
{
	private final Process process;

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
