package mds.mdsip;

import java.io.IOException;
import java.util.concurrent.TimeUnit;

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
		{
			this.dis.close();
			this.dos.close();
			try
			{
				if (!this.process.waitFor(1000, TimeUnit.MILLISECONDS))
					this.process.destroy();
			}
			catch (InterruptedException e)
			{
				this.process.destroy();
			}
		}
	}

	@Override
	public boolean isOpen()
	{ return this.process.isAlive(); }
}
