package mds.mdsip;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.concurrent.TimeUnit;

import mds.mdsip.MdsIp.MdsIpIOStream;

public class MdsIpFile extends MdsIpIOStream
{
	private final Process process;

	public MdsIpFile(final String filepath) throws IOException
	{
		final String args[] = filepath.split(" ");
		for (int i = 0; i < args.length; i++)
		{
			try
			{
				args[i] = new URI(args[i]).getPath();
			}
			catch (final URISyntaxException e)
			{
				throw new IOException("Invalid URI: " + args[i]);
			}
		}
		this.process = new ProcessBuilder(args).start();
		this.dis = this.process.getInputStream();
		this.dos = this.process.getOutputStream();
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
			catch (final InterruptedException e)
			{
				this.process.destroy();
			}
		}
	}

	@Override
	public boolean isOpen()
	{
		return this.process.isAlive();
	}
}
