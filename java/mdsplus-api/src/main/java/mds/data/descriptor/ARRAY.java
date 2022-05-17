package mds.data.descriptor;

import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_a.*;

/** Array Descriptor **/
public abstract class ARRAY<T> extends Descriptor<T>
{
	public static final class aflags
	{
		/** if set, scale is a power-of-two, otherwise, -ten **/
		boolean binscale;
		/** if set, indicates the bounds block is present **/
		boolean bounds;
		/** if set, indicates the multipliers block is present **/
		boolean coeff;
		/** if set, indicates column-major order (FORTRAN) **/
		boolean column;
		/** if set, indicates the array can be re-dimensioned **/
		boolean redim;

		public aflags(final boolean binscale, final boolean redim, final boolean column, final boolean coeff,
				final boolean bounds)
		{
			this.binscale = binscale;
			this.redim = redim;
			this.column = column;
			this.coeff = coeff;
			this.bounds = bounds;
		}

		public aflags(final byte af)
		{
			this.binscale = (af & 0x08) > 0;
			this.redim = (af & 0x10) > 0;
			this.column = (af & 0x20) > 0;
			this.coeff = (af & 0x40) > 0;
			this.bounds = (af & 0x80) > 0;
		}

		public final byte toByte()
		{
			byte af = 0;
			if (this.binscale)
				af |= 0x08;
			if (this.redim)
				af |= 0x10;
			if (this.column)
				af |= 0x20;
			if (this.coeff)
				af |= 0x40;
			if (this.bounds)
				af |= 0x80;
			return af;
		}
	}

	public static final class bounds
	{
		public final int l;
		public final int u;

		public bounds(final int l, final int u)
		{
			this.l = l;
			this.u = u;
		}
	}

	public static final int _sclB = 8;
	public static final int _dgtsB = 9;
	public static final int _afsB = 10;
	public static final int _dmctB = 11;
	public static final int _aszI = 12;
	public static final int _a0I = 16;
	public static final int _dmsIa = 20;
	protected static final aflags f_array = new aflags(false, true, true, false, false);
	protected static final aflags f_bounds = new aflags(false, true, true, true, true);
	protected static final aflags f_coeff = new aflags(false, true, true, true, false);
	public static final byte MAX_DIMS = 8;
	public static final String ETC = " /*** etc. ***/)";

	/** Returns the ARRAY deserialized from the given ByteBuffer **/
	public static ARRAY<?> deserialize(final ByteBuffer bi) throws MdsException
	{
		if (!bi.hasRemaining())
			return EmptyArray.NEW;
		final ByteBuffer b = bi.slice().order(bi.order());
		switch (b.get(Descriptor._clsB))
		{
		case Descriptor_A.CLASS:
			return Descriptor_A.deserialize(b);
		case Descriptor_APD.CLASS:
			return Descriptor_APD.deserialize(b);
		case Descriptor_CA.CLASS:
			return Descriptor_CA.deserialize(b);
		}
		throw new MdsException(Descriptor.getDClassName(b.get(Descriptor._clsB)) + " is not an ARRAY", 0);
	}

	private static final short getLength(final int[] shape, final DTYPE dtype, final int size)
	{
		if (shape == null || shape.length == 0 || shape[0] == 0)
			return Descriptor.getDataSize(dtype, 0);
		int arrlen = shape[0];
		for (int i = 1; i < shape.length; i++)
			arrlen *= shape[i];
		return (short) (size / arrlen);
	}

	protected ARRAY(final ByteBuffer b)
	{
		super(b);
	}

	protected ARRAY(final DTYPE dtype, final byte dclass, final ByteBuffer byteBuffer, final int... shape)
	{
		super(ARRAY.getLength(shape, dtype, byteBuffer.limit()), dtype, dclass, byteBuffer,
				shape.length > 1 ? ARRAY._dmsIa + shape.length * Integer.BYTES : ARRAY._a0I, 0);
		this.b.put(ARRAY._afsB, (shape.length > 1 ? ARRAY.f_coeff : ARRAY.f_array).toByte());
		this.b.put(ARRAY._dmctB, (byte) shape.length);
		this.b.putInt(ARRAY._aszI, byteBuffer.limit());
		if (shape.length > 1)
		{
			this.b.putInt(ARRAY._a0I, this.pointer()).position(ARRAY._dmsIa);
			this.b.asIntBuffer().put(shape);
			this.b.position(0);
		}
	}

	/** (16,i) a0 **/
	public final int a0()
	{
		return this.b.getInt(ARRAY._a0I);
	}

	/** (10,b) array flags **/
	public final aflags aflags()
	{
		return new aflags(this.b.get(ARRAY._afsB));
	}

	/** (12,i) array size **/
	public final int arsize()
	{
		return this.b.getInt(ARRAY._aszI);
	}

	/** (20+dimct*4,2i) bounds **/
	public final bounds bounds(final int idx)
	{
		final int pos = 20 + (this.dimct() + idx * 2) * Integer.BYTES;
		return new bounds(this.b.get(pos), this.b.get(pos + Integer.BYTES));
	}

	public Int8Array bytes()
	{
		return new Int8Array(this.toByteArray());
	}

	public Uint8Array byteu()
	{
		return new Uint8Array(this.toByteArray());
	}

	public Float64Array dfloat()
	{
		return Float64Array.D(this.toDoubleArray());
	}

	/** (9,b) digits **/
	public final byte digits()
	{
		return this.b.get(ARRAY._dgtsB);
	}

	/** (11,b) dim count **/
	public final byte dimct()
	{
		return this.b.get(ARRAY._dmctB);
	}

	/** (20,i) dimensions **/
	public int dims(final int idx)
	{
		if (this.dimct() > 1)
			return this.b.getInt(ARRAY._dmsIa + idx * Integer.BYTES);
		return idx == 0 ? this.arsize() / this.length() : 0;
	}

	public Float32Array ffloat()
	{
		return Float32Array.F(this.toFloatArray());
	}

	public Float32Array fsfloat()
	{
		return Float32Array.FS(this.toFloatArray());
	}

	public Float64Array ftfloat()
	{
		return Float64Array.FT(this.toDoubleArray());
	}

	/** Returns the number of elements contained in the Array **/
	public final int getLength()
	{
		return this.arsize() / this.length();
		/*
		 * final int dimct = this.dimct(); if(dimct == 0) return 0; int
		 * array_length = 1; for(int i = 0; i < dimct; i++) array_length *=
		 * this.dims(i); return array_length;
		 */
	}

	@Override
	public final int[] getShape()
	{
		final int dimct = this.dimct();
		final int[] dims = new int[dimct];
		for (int i = 0; i < dimct; i++)
			dims[i] = this.dims(i);
		return dims;
	}

	public Float64Array gfloat()
	{
		return Float64Array.G(this.toDoubleArray());
	}

	public Int32Array longs()
	{
		return new Int32Array(this.toIntArray());
	}

	public Uint32Array longu()
	{
		return new Uint32Array(this.toIntArray());
	}

	public Int128Array octawords()
	{
		return new Int128Array(this.getShape(), this.toLongArray());
	}

	public Uint128Array octawordu()
	{
		return new Uint128Array(this.getShape(), this.toLongArray());
	}

	public Int64Array quadwords()
	{
		return new Int64Array(this.toLongArray());
	}

	public Uint64Array quadwordu()
	{
		return new Uint64Array(this.toLongArray());
	}

	/** (8,b) scale **/
	public final byte scale()
	{
		return this.b.get(ARRAY._sclB);
	}
}
