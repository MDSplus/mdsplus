package mds.connection;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Vector;
import java.util.zip.InflaterInputStream;

public class MdsMessage extends Object
{
	static final String EVENTASTREQUEST = "---EVENTAST---REQUEST---";
	static final String EVENTCANREQUEST = "---EVENTCAN---REQUEST---";
	private static final int HEADER_SIZE = 48;
	private static final byte JC_BIG_ENDIAN_MASK = (byte) 0x80;
	private static final byte JC_COMPRESSED = (byte) 0x20;
	// private static final byte JC_SENDCAPABILITIES = (byte) 0xF;
	private static final int JC_SUPPORTS_COMPRESSION = 0x8000;
	private static final byte JC_SWAP_ENDIAN_ON_SERVER_MASK = (byte) 0x40;
	private static final byte JCJAVA_CLIENT = (byte) ((byte) 3 | JC_BIG_ENDIAN_MASK | JC_SWAP_ENDIAN_ON_SERVER_MASK);
	private static byte NEXT_MSG_ID = 1;
	public byte body[];
	private final byte client_type;
	private boolean compressed = false;
	private final Vector<ConnectionListener> connectionListeners;
	public byte descr_idx;
	public final int dims[];
	public byte dtype;
	public short length;
	public byte message_id;
	public int msglen = 0;
	public byte nargs;
	public byte ndims;
	public int status;
	private boolean swap = false;

	public MdsMessage()
	{
		this((byte) 0, (byte) 0, (byte) 0, null, new byte[1], null);
		status = 1;
	}

	public MdsMessage(byte c)
	{
		this(c, null);
	}

	public MdsMessage(byte descr_idx, byte dtype, byte nargs, int dims[], byte body[])
	{
		this(descr_idx, dtype, nargs, dims, body, null);
	}

	public MdsMessage(byte descr_idx, byte dtype, byte nargs, int dims[], byte body[], Vector<ConnectionListener> v)
	{
		final int body_size = (body != null ? body.length : 0);
		msglen = HEADER_SIZE + body_size;
		status = 0;
		message_id = NEXT_MSG_ID;
		this.length = Descriptor.getDataSize(dtype, body);
		this.nargs = nargs;
		this.descr_idx = descr_idx;
		if (dims != null)
			ndims = (byte) ((dims.length > Descriptor.MAX_DIM) ? Descriptor.MAX_DIM : dims.length);
		else
			ndims = 0;
		this.dims = new int[Descriptor.MAX_DIM];
		for (int i = 0; i < Descriptor.MAX_DIM; i++)
			this.dims[i] = (dims != null && i < dims.length) ? dims[i] : 0;
		this.dtype = dtype;
		client_type = JCJAVA_CLIENT;
		this.body = body;
		connectionListeners = v;
	}

	public MdsMessage(byte c, Vector<ConnectionListener> v)
	{
		this((byte) 0, Descriptor.DTYPE_CSTRING, (byte) 1, null, new byte[]
		{ c }, v);
	}

	public MdsMessage(String s)
	{
		this(s, null);
	}

	public MdsMessage(String s, Vector<ConnectionListener> v)
	{
		this((byte) 0, Descriptor.DTYPE_CSTRING, (byte) 1, null, s.getBytes(), v);
	}

	protected int ByteToInt(byte b[], int idx)
	{
		int ch1, ch2, ch3, ch4;
		ch1 = (b[idx + 0] & 0xff) << 24;
		ch2 = (b[idx + 1] & 0xff) << 16;
		ch3 = (b[idx + 2] & 0xff) << 8;
		ch4 = (b[idx + 3] & 0xff) << 0;
		return ((ch1) + (ch2) + (ch3) + (ch4));
	}

	protected int ByteToIntSwap(byte b[], int idx)
	{
		int ch1, ch2, ch3, ch4;
		ch1 = (b[idx + 3] & 0xff) << 24;
		ch2 = (b[idx + 2] & 0xff) << 16;
		ch3 = (b[idx + 1] & 0xff) << 8;
		ch4 = (b[idx + 0] & 0xff) << 0;
		return ((ch1) + (ch2) + (ch3) + (ch4));
	}

	protected short ByteToShort(byte b[], int idx)
	{
		short ch1, ch2;
		ch1 = (short) ((b[idx + 0] & 0xff) << 8);
		ch2 = (short) ((b[idx + 1] & 0xff) << 0);
		return (short) ((ch1) + (ch2));
	}

	protected short ByteToShortSwap(byte b[], int idx)
	{
		short ch1, ch2;
		ch1 = (short) ((b[idx + 1] & 0xff) << 8);
		ch2 = (short) ((b[idx + 0] & 0xff) << 0);
		return (short) ((ch1) + (ch2));
	}

	synchronized protected void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (connectionListeners != null)
			for (final ConnectionListener listener : connectionListeners)
				listener.processConnectionEvent(e);
	}

	protected void Flip(byte bytes[], int size)
	{
		int i;
		byte b;
		for (i = 0; i < bytes.length; i += size)
		{
			if (size == 2)
			{
				b = bytes[i];
				bytes[i] = bytes[i + 1];
				bytes[i + 1] = b;
			}
			else if (size == 4)
			{
				b = bytes[i];
				bytes[i] = bytes[i + 3];
				bytes[i + 3] = b;
				b = bytes[i + 1];
				bytes[i + 1] = bytes[i + 2];
				bytes[i + 2] = b;
			}
		}
	}

	private ByteBuffer getWrappedBody()
	{ return ByteBuffer.wrap(body).order(swap ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN); }

	protected final boolean IsRoprand(byte arr[], int idx)
	{
		return (arr[idx] == 0 && arr[idx + 1] == 0 && arr[idx + 2] == -128 && arr[idx + 3] == 0);
	}

	protected synchronized void ReadBuf(byte buf[], InputStream dis) throws IOException
	{
		ConnectionEvent e;
		int bytes_to_read = buf.length, read_bytes = 0, curr_offset = 0;
		boolean send = false;
		if (bytes_to_read > 2000)
		{
			send = true;
			e = new ConnectionEvent(this, buf.length, curr_offset);
			dispatchConnectionEvent(e);
		}
		while (bytes_to_read > 0)
		{
			read_bytes = dis.read(buf, curr_offset, bytes_to_read);
			if (read_bytes < 0)
				throw new IOException("Read Operation Failed");
			curr_offset += read_bytes;
			bytes_to_read -= read_bytes;
			if (send)
			{
				e = new ConnectionEvent(this, buf.length, curr_offset);
				dispatchConnectionEvent(e);
			}
		}
	}

	protected synchronized byte[] ReadCompressedBuf(InputStream dis) throws IOException
	{
		int bytes_to_read, read_bytes = 0, curr_offset = 0;
		byte out[];
		final byte b4[] = new byte[4];
		ReadBuf(b4, dis);
		bytes_to_read = ToInt(b4) - HEADER_SIZE;
		out = new byte[bytes_to_read];
		final InflaterInputStream zis = new InflaterInputStream(dis);
		while (bytes_to_read > 0)
		{
			read_bytes = zis.read(out, curr_offset, bytes_to_read);
			curr_offset += read_bytes;
			bytes_to_read -= read_bytes;
		}
		// remove EOF
		final byte pp[] = new byte[1];
		while (zis.available() == 1)
		{
			zis.read(pp);
		}
		return out;
	}

	public synchronized void Receive(InputStream dis) throws IOException
	{
		final byte header_b[] = new byte[16 + Descriptor.MAX_DIM * 4];
		int c_type = 0;
		int idx = 0;
		ReadBuf(header_b, dis);
		c_type = header_b[14];
		swap = ((c_type & JC_BIG_ENDIAN_MASK) != JC_BIG_ENDIAN_MASK);
		compressed = ((c_type & JC_COMPRESSED) == JC_COMPRESSED);
		if (swap)
		{
			msglen = ByteToIntSwap(header_b, 0);
			idx = 4;
			status = ByteToIntSwap(header_b, idx);
			idx += 4;
			length = ByteToShortSwap(header_b, idx);
			idx += 2;
		}
		else
		{
			msglen = ByteToInt(header_b, 0);
			idx = 4;
			status = ByteToInt(header_b, idx);
			idx += 4;
			length = ByteToShort(header_b, idx);
			idx += 2;
		}
		nargs = header_b[idx++];
		descr_idx = header_b[idx++];
		message_id = header_b[idx++];
		dtype = header_b[idx++];
		c_type = header_b[idx++];
		ndims = header_b[idx++];
		if (swap)
		{
			for (int i = 0, j = idx; i < Descriptor.MAX_DIM; i++, j += 4)
			{
				dims[i] = ByteToIntSwap(header_b, j);
			}
		}
		else
		{
			for (int i = 0, j = idx; i < Descriptor.MAX_DIM; i++, j += 4)
			{
				dims[i] = ByteToInt(header_b, j);
			}
		}
		if (msglen > HEADER_SIZE)
		{
			if (compressed)
			{
				body = ReadCompressedBuf(dis);
			}
			else
			{
				body = new byte[msglen - HEADER_SIZE];
				ReadBuf(body, dis);
			}
		}
		else
			body = new byte[0];
	}

	public synchronized void Send(DataOutputStream dos) throws IOException
	{
		dos.writeInt(msglen);
		dos.writeInt(status);
		dos.writeShort(length);
		dos.writeByte(nargs);
		dos.writeByte(descr_idx);
		dos.writeByte(message_id);
		dos.writeByte(dtype);
		dos.writeByte(client_type);
		dos.writeByte(ndims);
		for (int i = 0; i < Descriptor.MAX_DIM; i++)
			dos.writeInt(dims[i]);
		dos.write(body, 0, body.length);
		dos.flush();
		if (descr_idx == (nargs - 1))
			if (++NEXT_MSG_ID == 0)
				NEXT_MSG_ID = 1;
	}

	public double[] ToDoubleArray() throws IOException
	{
		final double out[] = new double[body.length / 8];
		getWrappedBody().asDoubleBuffer().get(out);
		return out;
	}

	protected float ToFloat(byte bytes[]) throws IOException
	{
		if (swap)
			Flip(bytes, 4);
		final ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
		final DataInputStream dis = new DataInputStream(bis);
		return dis.readFloat();
	}

	public float[] ToFloatArray() throws IOException
	{
		final float out[] = new float[body.length / 4];
		getWrappedBody().asFloatBuffer().get(out);
		return out;
	}

	protected int ToInt(byte bytes[]) throws IOException
	{
		if (swap)
			Flip(bytes, 4);
		final ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
		final DataInputStream dis = new DataInputStream(bis);
		return dis.readInt();
	}

	public int[] ToIntArray() throws IOException
	{
		final int out[] = new int[body.length / 4];
		getWrappedBody().asIntBuffer().get(out);
		return out;
	}

	public long[] ToLongArray() throws IOException
	{
		final long out[] = new long[body.length / 8];
		getWrappedBody().asLongBuffer().get(out);
		return out;
	}

	protected short ToShort(byte bytes[]) throws IOException
	{
		if (swap)
			Flip(bytes, 2);
		final ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
		final DataInputStream dis = new DataInputStream(bis);
		return dis.readShort();
	}

	public short[] ToShortArray() throws IOException
	{
		final short out[] = new short[body.length / 2];
		getWrappedBody().asShortBuffer().get(out);
		return out;
	}

	public String ToString()
	{
		return new String(body);
	}

	public long[] ToUIntArray() throws IOException
	{
		final int[] signed = ToIntArray();
		final long[] unsigned = new long[signed.length];
		for (int i = 0; i < signed.length; i++)
			unsigned[i] = (signed[i]) & 0xffffffffL;
		return unsigned;
	}

	public int[] ToUShortArray() throws IOException
	{
		final short[] shorts = ToShortArray();
		final int[] unsigned = new int[shorts.length];
		for (int i = 0; i < shorts.length; i++)
			unsigned[i] = shorts[i] & 0xffff;
		return unsigned;
	}

	public void useCompression(boolean use_cmp)
	{
		status = (use_cmp ? JC_SUPPORTS_COMPRESSION | 5 : 0);
	}
}
