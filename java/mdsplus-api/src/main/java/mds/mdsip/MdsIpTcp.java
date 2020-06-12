package mds.mdsip;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.StandardSocketOptions;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import mds.mdsip.MdsIp.Connection;

public class MdsIpTcp implements Connection
{
	private final SelectionKey select_out;
	private final SelectionKey select_in;
	private final SocketChannel socket;

	public MdsIpTcp(final String host, final int port) throws IOException
	{
		socket = SocketChannel.open();
		try
		{
			socket.configureBlocking(false);
			socket.setOption(StandardSocketOptions.SO_KEEPALIVE, Boolean.TRUE);
			this.select_out = socket.register(Selector.open(), SelectionKey.OP_CONNECT);
			this.select_in = socket.register(Selector.open(), SelectionKey.OP_READ);
			socket.connect(new InetSocketAddress(host, port));
		}
		catch (final IOException e)
		{
			socket.close();
			throw e;
		}
	}

	@Override
	final public void close() throws IOException
	{
		synchronized (select_out)
		{
			synchronized (select_in)
			{
				select_out.cancel();
				select_in.cancel();
				socket.close();
			}
		}
	}

	final private boolean ensure_open() throws IOException
	{
		if (!select_out.isValid())
			return false;
		while (select_out.interestOps() == SelectionKey.OP_CONNECT)
		{
			if (select_out.selector().select(internal_timeout) > 0 && select_out.isConnectable())
			{
				synchronized (select_out)
				{
					if (!(select_out.isValid() && socket.finishConnect()))
						return false;
					select_out.interestOps(SelectionKey.OP_WRITE);
				}
			}
		}
		return true;
	}

	@Override
	final public boolean isOpen()
	{ return select_out.isValid(); }

	@Override
	final public int read(ByteBuffer buffer) throws IOException
	{
		synchronized (select_in)
		{
			if (!this.ensure_open())
				return -1;
			final int rem = buffer.remaining();
			if (rem == 0)
				return 0;
			if (select_in.selector().select(internal_timeout) >= 0 && select_in.isReadable())
			{
				final int read = socket.read(buffer);
				if (read == -1)
					return read;
			}
			return rem - buffer.remaining();
		}
	}

	@Override
	final public int write(final ByteBuffer buffer) throws IOException
	{
		synchronized (select_out)
		{
			if (!this.ensure_open())
				return -1;
			final int rem = buffer.remaining();
			if (rem == 0)
				return 0;
			if (select_out.selector().select(internal_timeout) >= 0 && select_out.isWritable())
			{
				if (!select_out.isValid())
					return -1;
				final int sent = socket.write(buffer);
				if (sent == -1)
					return sent;
			}
			return rem - buffer.remaining();
		}
	}
}
