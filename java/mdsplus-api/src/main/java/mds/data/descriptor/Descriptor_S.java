package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_s.*;

/** Fixed-Length (static) Descriptor (1) **/
@SuppressWarnings("deprecation")
public abstract class Descriptor_S<T> extends Descriptor<T>
{
	public static final byte CLASS = 1;
	private static final boolean atomic = true;
	@SuppressWarnings("hiding")
	public static final short BYTES = Descriptor.BYTES;

	public static final Descriptor_S<?> deserialize(final ByteBuffer b) throws MdsException
	{
		switch (Descriptor.getDtype(b))
		{
		case NID:
			return new Nid(b);
		case BU:
			return new Uint8(b);
		case WU:
			return new Uint16(b);
		case LU:
			return new Uint32(b);
		case QU:
			return new Uint64(b);
		case OU:
			return new Uint128(b);
		case B:
			return new Int8(b);
		case W:
			return new Int16(b);
		case L:
			return new Int32(b);
		case Q:
			return new Int64(b);
		case O:
			return new Int128(b);
		case F:
		case FS:
			return new Float32(b);
		case FC:
		case FSC:
			return new Complex32(b);
		case D:
		case G:
		case FT:
			return new Float64(b);
		case DC:
		case GC:
		case FTC:
			return new Complex64(b);
		case T:
			return new StringDsc(b);
		case POINTER:
			return new Pointer(b);
		case IDENT:
			return new Ident(b);
		case PATH:
			return new Path(b);
		case Z:
			return Missing.NEW;
		case EVENT:
			return new Event(b);
		default:
			throw new MdsException(String.format("Unsupported dtype %s for class %s",
					DTYPE.getName(b.get(Descriptor._typB)), Descriptor.getDClassName(b.get(Descriptor._clsB))), 0);
		}
	}

	public Descriptor_S(final ByteBuffer b)
	{
		super(b);
		this.b.limit(this.pointer() + this.length());
	}

	public Descriptor_S(final DTYPE dtype, final ByteBuffer data)
	{
		this((short) data.limit(), dtype, data);
	}

	public Descriptor_S(final short length, final DTYPE dtype, final ByteBuffer value)
	{
		super(length, dtype, Descriptor_S.CLASS, value, Descriptor.BYTES, 0);
	}

	public Int8 bytes()
	{
		return new Int8(this.toByte());
	}

	public Uint8 byteu()
	{
		return new Uint8(this.toByte());
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		final T val = this.getAtomic();
		return pout.append(val == null ? "*" : val.toString());
	}

	public Float64 dfloat()
	{
		return Float64.D(this.toDouble());
	}

	public boolean equals(final Descriptor_S<?> dsca)
	{
		return this.getAtomic().equals(dsca.getAtomic());
	}

	public Float32 ffloat()
	{
		return Float32.F(this.toFloat());
	}

	public Float32 fsfloat()
	{
		return Float32.FS(this.toFloat());
	}

	public Float64 ftfloat()
	{
		return Float64.FT(this.toDouble());
	}

	@Override
	protected Descriptor<?> getData_(final DTYPE... omits) throws MdsException
	{
		return this;
	}

	@Override
	public int[] getShape()
	{
		return new int[0];
	}

	public Float64 gfloat()
	{
		return Float64.G(this.toDouble());
	}

	@Override
	public boolean isAtomic()
	{
		return Descriptor_S.atomic;
	}

	public Int32 longs()
	{
		return new Int32(this.toInt());
	}

	public Uint32 longu()
	{
		return new Uint32(this.toInt());
	}

	public Uint8 not()
	{
		return new Uint8(this.toByte() == 0 ? (byte) 1 : (byte) 0);
	}

	public Int128 octawords()
	{
		return new Int128(this.toLong());
	}

	public Uint128 octawordu()
	{
		return new Uint128(this.toLong());
	}

	public Int64 quadwords()
	{
		return new Int64(this.toLong());
	}

	public Uint64 quadwordu()
	{
		return new Uint64(this.toLong());
	}

	@Override
	public BigInteger[] toBigIntegerArray()
	{
		final T val = this.getAtomic();
		if (val instanceof BigInteger)
			return new BigInteger[]
			{ (BigInteger) val };
		else if (val instanceof Number)
			return new BigInteger[]
			{ BigInteger.valueOf(((Number) val).longValue()) };
		else if (val instanceof String)
			return new BigInteger[]
			{ BigInteger.valueOf(Long.parseLong((String) val)) };
		else if (val instanceof float[])
			return new BigInteger[]
			{ BigInteger.valueOf((long) ((float[]) val)[0]) };
		else if (val instanceof double[])
			return new BigInteger[]
			{ BigInteger.valueOf((long) ((double[]) val)[0]) };
		else
			return new BigInteger[]
			{ BigInteger.ZERO };
	}

	@Override
	public byte[] toByteArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new byte[]
			{ ((Number) val).byteValue() };
		else if (val instanceof String)
			return new byte[]
			{ Byte.parseByte((String) val) };
		else if (val instanceof float[])
			return new byte[]
			{ (byte) ((float[]) val)[0] };
		else if (val instanceof double[])
			return new byte[]
			{ (byte) ((double[]) val)[0] };
		else
			return new byte[]
			{ 0 };
	}

	@Override
	public double[] toDoubleArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new double[]
			{ ((Number) val).doubleValue() };
		else if (val instanceof String)
			return new double[]
			{ Double.parseDouble((String) val) };
		else if (val instanceof float[])
			return new double[]
			{ ((float[]) val)[0] };
		else if (val instanceof double[])
			return new double[]
			{ ((double[]) val)[0] };
		else
			return new double[]
			{ Double.NaN };
	}

	@Override
	public float[] toFloatArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new float[]
			{ ((Number) val).floatValue() };
		else if (val instanceof String)
			return new float[]
			{ Float.parseFloat((String) val) };
		else if (val instanceof float[])
			return new float[]
			{ ((float[]) val)[0] };
		else if (val instanceof double[])
			return new float[]
			{ (float) ((double[]) val)[0] };
		else if (val instanceof double[])
			return new float[]
			{ (float) ((double[]) val)[0] };
		else
			return new float[]
			{ Float.NaN };
	}

	@Override
	public int[] toIntArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new int[]
			{ ((Number) val).intValue() };
		else if (val instanceof String)
			return new int[]
			{ Integer.parseInt((String) val) };
		else if (val instanceof float[])
			return new int[]
			{ (int) ((float[]) val)[0] };
		else if (val instanceof double[])
			return new int[]
			{ (int) ((double[]) val)[0] };
		else
			return new int[]
			{ 0 };
	}

	@Override
	public long[] toLongArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new long[]
			{ ((Number) val).longValue() };
		else if (val instanceof String)
			return new long[]
			{ Long.parseLong((String) val) };
		else if (val instanceof float[])
			return new long[]
			{ (long) ((float[]) val)[0] };
		else if (val instanceof double[])
			return new long[]
			{ (long) ((double[]) val)[0] };
		else
			return new long[]
			{ 0 };
	}

	@Override
	public short[] toShortArray()
	{
		final T val = this.getAtomic();
		if (val instanceof Number)
			return new short[]
			{ ((Number) val).shortValue() };
		else if (val instanceof String)
			return new short[]
			{ Short.parseShort((String) val) };
		else if (val instanceof float[])
			return new short[]
			{ (short) ((float[]) val)[0] };
		else if (val instanceof double[])
			return new short[]
			{ (short) ((double[]) val)[0] };
		else
			return new short[]
			{ 0 };
	}

	@Override
	public String[] toStringArray()
	{
		final T val = this.getAtomic();
		if (val instanceof String)
			return new String[]
			{ (String) val };
		else if (val instanceof Number)
			return new String[]
			{ val.toString() };
		else if (val instanceof float[])
			return new String[]
			{ Float.toString(((float[]) val)[0]) };
		else if (val instanceof double[])
			return new String[]
			{ Double.toString(((double[]) val)[0]) };
		else
			return new String[]
			{ "" };
	}

	public Int16 words()
	{
		return new Int16(this.toShort());
	}

	public Uint16 wordu()
	{
		return new Uint16(this.toShort());
	}
}
