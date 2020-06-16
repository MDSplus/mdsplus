package mds.connection;

/* $Id$ */
import java.io.*;

public class Descriptor
{
	public static final byte MAX_DIM = 8;
	public static final byte DTYPE_UBYTE = 2;
	public static final byte DTYPE_USHORT = 3;
	public static final byte DTYPE_ULONG = 4;
	public static final byte DTYPE_ULONGLONG = 5;
	public static final byte DTYPE_BYTE = 6;
	public static final byte DTYPE_SHORT = 7;
	public static final byte DTYPE_LONG = 8;
	public static final byte DTYPE_LONGLONG = 9;
	public static final byte DTYPE_FLOAT = 10;
	public static final byte DTYPE_DOUBLE = 11;
	public static final byte DTYPE_COMPLEX = 12;
	public static final byte DTYPE_COMPLEX_DOUBLE = 13;
	public static final byte DTYPE_CSTRING = 14;
	public static final byte DTYPE_EVENT = 99;
	public static final byte DTYPE_EVENT_NOTIFY = 99;
	public byte dtype;
	public short short_data[];
	public double double_data[];
	public float float_data[];
	public int int_data[];
	public byte byte_data[];
	public long long_data[];
	public String strdata;
	public String error = null;
	public int dims[];
	public int status;

	public Descriptor()
	{
		super();
	}

	public Descriptor(String error)
	{
		this.error = error;
	}

	public Descriptor(byte dtype, int dims[], byte byte_data[])
	{
		this.dtype = dtype;
		this.dims = dims;
		this.byte_data = byte_data;
	}

	public Descriptor(int dims[], byte byte_data[])
	{
		this.dtype = DTYPE_BYTE;
		this.dims = dims;
		this.byte_data = byte_data;
	}

	public Descriptor(int dims[], long long_data[])
	{
		this.dtype = DTYPE_LONGLONG;
		this.dims = dims;
		this.long_data = long_data;
	}

	public Descriptor(int dims[], float float_data[])
	{
		this.dtype = DTYPE_FLOAT;
		this.dims = dims;
		this.float_data = float_data;
	}

	public Descriptor(int dims[], int int_data[])
	{
		this.dtype = DTYPE_LONG;
		this.dims = dims;
		this.int_data = int_data;
	}

	public Descriptor(int dims[], short short_data[])
	{
		this.dtype = DTYPE_SHORT;
		this.dims = dims;
		this.short_data = short_data;
	}

	public Descriptor(int dims[], String strdata)
	{
		this.dtype = DTYPE_CSTRING;
		this.dims = dims;
		this.strdata = strdata;
	}

	public int getStatus()
	{ return status; }

	public int getInt()
	{ return int_data[0]; }

	public static byte[] dataToByteArray(Object o)
	{
		byte b[] = null;
		try
		{
			final ByteArrayOutputStream dosb = new ByteArrayOutputStream();
			final DataOutputStream dos = new DataOutputStream(dosb);
			if (o instanceof Short)
			{
				final short d = ((Short) o).shortValue();
				dos.writeShort(d);
				b = dosb.toByteArray();
			}
			if (o instanceof Integer)
			{
				final int d = ((Integer) o).intValue();
				dos.writeInt(d);
				b = dosb.toByteArray();
			}
			if (o instanceof Float)
			{
				final float d = ((Float) o).floatValue();
				dos.writeFloat(d);
				b = dosb.toByteArray();
			}
			if (o instanceof Double)
			{
				final double d = ((Double) o).doubleValue();
				dos.writeDouble(d);
				b = dosb.toByteArray();
			}
			if (o instanceof Long)
			{
				final long l = ((Long) o).longValue();
				dos.writeLong(l);
				b = dosb.toByteArray();
			}
			dos.close();
			return b;
		}
		catch (final IOException e)
		{}
		return null;
	}

	public byte[] dataToByteArray()
	{
		byte b[] = null;
		try
		{
			final ByteArrayOutputStream dosb = new ByteArrayOutputStream();
			final DataOutputStream dos = new DataOutputStream(dosb);
			switch (dtype)
			{
			case DTYPE_CSTRING:
				dos.close();
				if (strdata != null)
					return strdata.getBytes();
				else
					return byte_data;
			case DTYPE_UBYTE:
			case DTYPE_BYTE:
				dos.close();
				return byte_data;
			case DTYPE_USHORT:
			case DTYPE_SHORT:
				for (int i = 0; i < short_data.length; i++)
					dos.writeShort(short_data[i]);
				break;
			case DTYPE_ULONG:
			case DTYPE_LONG:
				for (int i = 0; i < int_data.length; i++)
					dos.writeInt(int_data[i]);
				break;
			case DTYPE_FLOAT:
				for (int i = 0; i < float_data.length; i++)
					dos.writeFloat(float_data[i]);
				break;
			case DTYPE_ULONGLONG:
			case DTYPE_LONGLONG:
				for (int i = 0; i < long_data.length; i++)
					dos.writeLong(long_data[i]);
				break;
			case DTYPE_DOUBLE:
				for (int i = 0; i < double_data.length; i++)
					dos.writeDouble(double_data[i]);
				break;
			}
			b = dosb.toByteArray();
			dos.close();
			return b;
		}
		catch (final IOException e)
		{}
		return null;
	}

	static public short getDataSize(byte type, byte[] body)
	{
		switch (type)
		{
		case DTYPE_CSTRING:
			return (short) body.length;
		case DTYPE_BYTE:
			return 1;
		case DTYPE_USHORT:
		case DTYPE_SHORT:
			return 2;
		case DTYPE_ULONG:
		case DTYPE_LONG:
		case DTYPE_FLOAT:
			return 4;
		case DTYPE_ULONGLONG:
		case DTYPE_LONGLONG:
		case DTYPE_DOUBLE:
			return 8;
		}
		return 0;
	}
}
