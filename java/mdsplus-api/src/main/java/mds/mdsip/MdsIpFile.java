package mds.mdsip;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.concurrent.TimeUnit;

import mds.mdsip.MdsIp.MdsIpIOStream;

public class MdsIpFile extends MdsIpIOStream
{
	static final MdsIpFile fromString(final String string) throws IOException
	{
		final String args[] = string.split(" ");
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
		return new MdsIpFile(args);
	}

	private final Process process;
	private final String args[];

	public MdsIpFile(final String... args) throws IOException
	{
		this.args = args;
		this.process = new ProcessBuilder(this.args).start();
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

	@Override
	public final String toString()
	{
		final StringBuilder sb = new StringBuilder();
		sb.append("MdsIpFile(");
		for (final String s : this.args)
			sb.append('"').append(s).append('"').append(", ");
		sb.setLength(sb.length() - 2);
		sb.append(')');
		return sb.toString();
	}
}
