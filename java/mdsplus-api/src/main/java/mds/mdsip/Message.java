package mds.mdsip;

import java.io.IOException;
import java.io.InputStream;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.charset.StandardCharsets;
import java.util.Set;
import java.util.zip.InflaterInputStream;

import mds.MdsException;
import mds.TransferEventListener;
import mds.data.DTYPE;
import mds.data.descriptor.ARRAY;
import mds.data.descriptor.Descriptor;
import mds.mdsip.MdsIp.Connection;

public final class Message extends Object
{
	private static class CompressedChannel implements ReadableByteChannel
	{
		final byte buf[] = new byte[1024];
		final ReadableByteChannel rbc;
		final InflaterInputStream zis;

		public CompressedChannel(final ReadableByteChannel rbc)
		{
			this.rbc = rbc;
			final InputStream is = new InputStream()
			{
				ByteBuffer bb = ByteBuffer.allocate(1);

				@Override
				public int read() throws IOException
				{
					CompressedChannel.this.rbc.read(this.bb);
					return this.bb.get();
				}
			};
			this.zis = new InflaterInputStream(is);
		}

		@Override
		public void close() throws IOException
		{
			this.zis.skip(1);
		}

		@Override
		public boolean isOpen()
		{
			return this.rbc.isOpen();
		}

		@Override
		public int read(final ByteBuffer dst) throws IOException
		{
			final int read = this.zis.read(this.buf, 0, dst.remaining());
			dst.put(this.buf, 0, read);
			return read;
		}
	}

	public static final byte BIG_ENDIAN_MASK = (byte) 0x80;
	private static final byte COMPRESSED = (byte) 0x20;
	protected static final String EVENTASTREQUEST = "---EVENTAST---REQUEST---";
	protected static final String EVENTCANREQUEST = "---EVENTCAN---REQUEST---";
	static final public int HEADER_CTYPE_B = 14;
	static final public int HEADER_DIM0_I = 16;
	static final public int HEADER_DSCIDX_B = 11;
	static final public int HEADER_DTYPE_B = 13;
	static final public int HEADER_LENGTH_S = 8;
	static final public int HEADER_MSGIDX_B = 12;
	static final public int HEADER_MSGLEN_I = 0;
	static final public int HEADER_NARGS_B = 10;
	static final public int HEADER_NDIMS_B = 15;
	static final public int HEADER_SIZE = 48;
	static final public int HEADER_STATUS_I = 4;
	public static final byte JAVA_CLIENT = (byte) (Message.JAVA_CLIENT_LITTLE | Message.BIG_ENDIAN_MASK);
	private static final byte JAVA_CLIENT_LITTLE = (byte) 0x3;
	// private static final byte SENDCAPABILITIES = (byte)0xF;
	private static final int SUPPORTS_COMPRESSION = 0x8000;

	private static final synchronized void dispatchTransferEvent(final Set<TransferEventListener> mdslisteners,
			final ReadableByteChannel dis, final String info, final int read, final int to_read)
	{
		if (mdslisteners != null)
			for (final TransferEventListener listener : mdslisteners)
				listener.handleTransferEvent(dis, info, read, to_read);
	}

	protected static boolean isRoprand(final byte arr[], final int idx)
	{
		return (arr[idx] == 0 && arr[idx + 1] == 0 && arr[idx + 2] == -128 && arr[idx + 3] == 0);
	}

	private static final ByteBuffer readBuf(final int bytes_to_read, final ReadableByteChannel dis,
			final Set<TransferEventListener> mdslisteners, long abstimeout) throws IOException
	{
		final ByteBuffer buf = ByteBuffer.allocateDirect(bytes_to_read);
		final boolean event = (bytes_to_read > 2000);
		if (abstimeout == 0)
			abstimeout = System.currentTimeMillis() + 3_000;
		try
		{
			while (buf.hasRemaining())
			{
				final int read = dis.read(buf);
				if (read == -1)
					throw new SocketException("connection lost");
				else if (read == 0)
				{
					if (abstimeout > 0 && System.currentTimeMillis() > abstimeout)
						throw new SocketException("connection timeout");
					continue;
				}
				if (event)
					Message.dispatchTransferEvent(mdslisteners, dis, null, bytes_to_read, buf.position());
			}
		}
		catch (final IOException e)
		{
			System.err.printf("Connection lost: %s\n", e.getMessage());
			Message.dispatchTransferEvent(mdslisteners, dis, e.getMessage(), 0, 0);
			throw e;
		}
		buf.rewind();
		return buf;
	}

	protected static final ByteBuffer readCompressedBuf(final ReadableByteChannel rbc, final ByteOrder byteOrder,
			final Set<TransferEventListener> listeners) throws IOException
	{
		final ByteBuffer unflattened_length = ByteBuffer.allocate(Integer.BYTES).order(byteOrder);
		rbc.read(unflattened_length);
		final int bytes_to_read = unflattened_length.getInt(0) - Message.HEADER_SIZE;
		try (CompressedChannel cc = new Message.CompressedChannel(rbc);)
		{
			return Message.readBuf(bytes_to_read, cc, listeners, 0);
		}
	}

	public final static Message receive(final Connection con, final Set<TransferEventListener> listeners,
			final int timeout) throws IOException
	{
		final long abstimeout = timeout >= 0 ? System.currentTimeMillis() + timeout : -1;
		final ByteBuffer header = Message.readBuf(Message.HEADER_SIZE, con, null, abstimeout);
		con.checkMsgIdx(header.get(Message.HEADER_MSGIDX_B));
		if (header.get(Message.HEADER_NDIMS_B) > ARRAY.MAX_DIMS)
			throw new IOException("Invalid Message: MAX_DIMS");
		final byte c_type = header.get(Message.HEADER_CTYPE_B);
		if ((c_type & Message.BIG_ENDIAN_MASK) == 0)
			header.order(ByteOrder.LITTLE_ENDIAN);
		final int msglen = header.getInt(Message.HEADER_MSGLEN_I);
		final ByteBuffer body;
		if (msglen > Message.HEADER_SIZE)
		{
			if ((c_type & Message.COMPRESSED) != 0)
				body = Message.readCompressedBuf(con, header.order(), listeners);
			else
				body = Message.readBuf(msglen - Message.HEADER_SIZE, con, listeners, 0);
		}
		else if (msglen < Message.HEADER_SIZE)
		{
			throw new IOException("Invalid Message: HEADER_SIZE");
		}
		else
			body = ByteBuffer.allocateDirect(0);
		body.order(header.order());
		final Message msg = new Message(header, body);
		return msg;
	}

	private final ByteBuffer body;
	private final ByteBuffer header;

	public Message(final byte mid)
	{
		this('\0', mid);
		this.verify();
	}

	public Message(final byte descr_idx, final DTYPE dtype, final byte nargs, final int dims[], final byte ndims_in,
			final ByteBuffer body, final byte client_type, final int status, final byte mid)
	{
		this.header = ByteBuffer.allocateDirect(Message.HEADER_SIZE);
		this.body = body.slice();
		if ((client_type & Message.BIG_ENDIAN_MASK) == 0)
		{
			this.header.order(ByteOrder.LITTLE_ENDIAN);
			this.body.order(ByteOrder.LITTLE_ENDIAN);
		}
		final int body_size = body.remaining();
		final ByteBuffer h = this.getHeader();// 4
		final byte ndims = (ndims_in > ARRAY.MAX_DIMS) ? ARRAY.MAX_DIMS : ndims_in;
		h.putInt(Message.HEADER_SIZE + body_size).putInt(status);// 8
		h.putShort(Descriptor.getDataSize(dtype, body_size)).put(nargs).put(descr_idx);// 12
		h.put(mid).put(dtype.toByte()).put(client_type).put(ndims);// 16
		if (ndims > 0)
			h.asIntBuffer().put(dims, 0, ndims);
	}

	public Message(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final byte ndims,
			final ByteBuffer body, final int status, final byte mid)
	{
		this(descr_idx, bu, nargs, dims, ndims, body, (body == null || body.order() == ByteOrder.BIG_ENDIAN)
				? Message.JAVA_CLIENT : Message.JAVA_CLIENT_LITTLE, status, mid);
	}

	public Message(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final ByteBuffer body,
			final byte mid)
	{
		this(descr_idx, bu, nargs, dims, (dims == null) ? (byte) 0 : (byte) dims.length, body, 0, mid);
	}

	public Message(final byte descr_idx, final DTYPE bu, final byte nargs, final int dims[], final String body,
			final byte mid)
	{
		this(descr_idx, bu, nargs, dims, StandardCharsets.UTF_8.encode(body).order(Descriptor.BYTEORDER), mid);
	}

	public Message(final ByteBuffer header, final ByteBuffer body)
	{
		this.header = header.slice().order(header.order());
		this.body = body.slice().order(header.order());
	}

	protected Message(final char c, final byte mid)
	{
		this(String.valueOf(c), mid);
	}

	protected Message(final String s, final byte mid)
	{
		this((byte) 0, DTYPE.T, (byte) 1, null, s, mid);
	}

	public final byte[] asByteArray()
	{
		final ByteBuffer b = this.getBody();
		return b.array();
	}

	protected final double[] asDoubleArray()
	{
		final ByteBuffer b = this.getBody();
		final double[] out = new double[b.position(0).remaining() / Double.BYTES];
		b.asDoubleBuffer().get(out);
		return out;
	}

	protected final float[] asFloatArray()
	{
		final ByteBuffer b = this.getBody();
		final float out[] = new float[b.position(0).remaining() / Float.BYTES];
		b.asFloatBuffer().get(out);
		return out;
	}

	public final int[] asIntArray()
	{
		final ByteBuffer b = this.getBody();
		final int out[] = new int[b.position(0).remaining() / Integer.BYTES];
		b.asIntBuffer().get(out);
		return out;
	}

	public final long[] asLongArray()
	{
		final ByteBuffer b = this.getBody();
		final long out[] = new long[b.position(0).remaining() / Long.BYTES];
		b.asLongBuffer().get(out);
		return out;
	}

	public final short[] asShortArray()
	{
		final ByteBuffer b = this.getBody();
		final short out[] = new short[b.position(0).remaining() / Short.BYTES];
		b.asShortBuffer().get(out);
		return out;
	}

	public final String asString()
	{
		return StandardCharsets.UTF_8.decode(this.getBody()).toString();
	}

	public final long[] asUIntArray()
	{
		final ByteBuffer b = this.getBody();
		final long out[] = new long[b.position(0).remaining() / Integer.BYTES];
		for (int i = 0; i < out.length; i++)
			out[i] = b.getInt() & 0xFFFFFFFFl;
		return out;
	}

	public final int[] asUShortArray()
	{
		final ByteBuffer b = this.getBody();
		final int out[] = new int[b.position(0).remaining() / Short.BYTES];
		for (int i = 0; i < out.length; i++)
			out[i] = b.getShort() & 0xFFFF;
		return out;
	}

	public final ByteBuffer getBody()
	{
		return this.body.duplicate().order(this.body.order());
	}

	public final byte getCType()
	{
		return (byte) (this.header.get(Message.HEADER_CTYPE_B) & 0x1F);
	}

	public final int getDescIdx()
	{
		return this.header.get(Message.HEADER_DSCIDX_B) & 0xFF;
	}

	public final int[] getDims()
	{
		final int dims[] = new int[ARRAY.MAX_DIMS];
		final ByteBuffer h = this.getHeader();
		h.position(Message.HEADER_DIM0_I);
		h.asIntBuffer().get(dims, 0, this.getNDims());
		return dims;
	}

	public final DTYPE getDType()
	{
		return DTYPE.get(this.header.get(Message.HEADER_DTYPE_B));
	}

	public final ByteBuffer getHeader()
	{
		return this.header.duplicate().order(this.header.order());
	}

	public final int getLength()
	{
		return this.header.getShort(Message.HEADER_LENGTH_S) & 0xFFFF;
	}

	public final int getMsgIdx()
	{
		return this.header.get(Message.HEADER_MSGIDX_B) & 0xFF;
	}

	public final int getMsgLen()
	{
		return this.header.getInt(Message.HEADER_MSGLEN_I);
	}

	public final int getNArgs()
	{
		return this.header.get(Message.HEADER_NARGS_B) & 0xFF;
	}

	public final int getNDims()
	{
		return this.header.get(Message.HEADER_NDIMS_B);
	}

	public final int getStatus()
	{
		return this.header.getInt(Message.HEADER_STATUS_I);
	}

	public final boolean isLittleEndian()
	{
		return ((this.getCType() & Message.BIG_ENDIAN_MASK) == 0);
	}

	public final void send(final WritableByteChannel wbc) throws MdsException
	{
		try
		{
			synchronized (wbc)
			{
				final ByteBuffer h = this.getHeader();
				while (h.hasRemaining())
					if (wbc.write(h) < 0)
						throw new IOException("failed to write");
				final ByteBuffer b = this.getBody();
				while (b.hasRemaining())
				{
					if (wbc.write(b) < 0)
						throw new IOException("failed to write");
				}
				// wbc.flush(); TODO: interface?
			}
		}
		catch (final IOException e)
		{
			throw new MdsException(e.getMessage());
		}
	}

	@Override
	public final String toString()
	{
		try
		{
			return Descriptor.readMessage(this).toString();
		}
		catch (final Exception e)
		{
			return this.getDType().label + "(" + this.getMsgLen() + "Bytes)";
		}
	}

	protected final void useCompression(final boolean use_cmp)
	{
		this.header.putInt(Message.HEADER_STATUS_I, (use_cmp ? Message.SUPPORTS_COMPRESSION | 5 : 0));
	}

	public final void verify()
	{
		this.header.putInt(Message.HEADER_STATUS_I, this.header.getInt(Message.HEADER_STATUS_I) | 1);
	}
}
