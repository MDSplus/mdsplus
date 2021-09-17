package mds.mdsip;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.StandardSocketOptions;
import java.nio.ByteBuffer;
import java.nio.channels.*;

import mds.mdsip.MdsIp.Connection;

public class MdsIpTcp extends Connection
{
	private static final int DEFAULT_PORT = 8000;

	static public Connection fromString(final String string) throws IOException
	{
		final String host_port[] = string.split(":", 2);
		final int port = (host_port.length > 1) ? Integer.parseInt(host_port[1]) : MdsIpTcp.DEFAULT_PORT;
		return new MdsIpTcp(host_port[0], port);
	}

	private final SelectionKey select_out;
	private final SelectionKey select_in;
	private final SocketChannel socket;

	public MdsIpTcp(final String host, final int port) throws IOException
	{
		this.socket = SocketChannel.open();
		try
		{
			this.socket.configureBlocking(false);
			this.socket.setOption(StandardSocketOptions.SO_KEEPALIVE, Boolean.TRUE);
			this.select_out = this.socket.register(Selector.open(), SelectionKey.OP_CONNECT);
			this.select_in = this.socket.register(Selector.open(), SelectionKey.OP_READ);
			this.socket.connect(new InetSocketAddress(host, port));
		}
		catch (final IOException e)
		{
			this.socket.close();
			throw e;
		}
	}

	@Override
	final public void close() throws IOException
	{
		this.socket.close();
		synchronized (this.select_out)
		{
			synchronized (this.select_in)
			{
				this.select_in.cancel();
				this.select_out.cancel();
			}
		}
	}

	final private boolean ensure_open() throws IOException
	{
		if (!this.select_out.isValid())
			return false;
		while (this.select_out.interestOps() == SelectionKey.OP_CONNECT)
		{
			if (this.select_out.selector().select(Connection.internal_timeout) > 0 && this.select_out.isConnectable())
			{
				synchronized (this.select_out)
				{
					if (!(this.select_out.isValid() && this.socket.finishConnect()))
						return false;
					this.select_out.interestOps(SelectionKey.OP_WRITE);
				}
			}
		}
		return true;
	}

	@Override
	final public boolean isOpen()
	{
		return this.select_out.isValid();
	}

	@Override
	final public int read(final ByteBuffer buffer) throws IOException
	{
		synchronized (this.select_in)
		{
			if (!this.ensure_open())
				return -1;
			final int rem = buffer.remaining();
			if (rem == 0)
				return 0;
			if (this.select_in.selector().select(Connection.internal_timeout) >= 0 && this.select_in.isReadable())
			{
				try
				{
					final int read = this.socket.read(buffer);
					if (read == -1)
						return read;
				}
				catch (final ClosedChannelException e)
				{
					return -1;
				}
			}
			return rem - buffer.remaining();
		}
	}

	@Override
	final public int write(final ByteBuffer buffer) throws IOException
	{
		synchronized (this.select_out)
		{
			if (!this.ensure_open())
				return -1;
			final int rem = buffer.remaining();
			if (rem == 0)
				return 0;
			if (this.select_out.selector().select(Connection.internal_timeout) >= 0 && this.select_out.isWritable())
			{
				if (!this.select_out.isValid())
					return -1;
				final int sent = this.socket.write(buffer);
				if (sent == -1)
					return sent;
			}
			return rem - buffer.remaining();
		}
	}
}
