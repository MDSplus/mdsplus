package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import java.nio.LongBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Float64;

public final class Float64Array extends FLOATArray<Double>
{
	public static final Float64Array D(final double... values)
	{
		return new Float64Array(DTYPE.D, values);
	}

	public static final Float64Array D(final int[] shape, final double... values)
	{
		return new Float64Array(DTYPE.D, values, shape);
	}

	public static final Float64Array FT(final double... values)
	{
		return new Float64Array(DTYPE.FT, values);
	}

	public static final Float64Array FT(final int[] shape, final double... values)
	{
		return new Float64Array(DTYPE.FT, values, shape);
	}

	public static final Float64Array G(final double... values)
	{
		return new Float64Array(DTYPE.G, values);
	}

	public static final Float64Array G(final int[] shape, final double... values)
	{
		return new Float64Array(DTYPE.G, values, shape);
	}

	public Float64Array(final ByteBuffer b)
	{
		super(b);
	}

	public Float64Array(final double... values)
	{
		this(DTYPE.DOUBLE, values);
	}

	public Float64Array(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	public Float64Array(final DTYPE dtype, final double[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	public Float64Array(final int[] shape, final double... values)
	{
		this(DTYPE.DOUBLE, values, shape);
	}

	@Override
	public final Float64Array abs()
	{
		final double[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = Math.abs(values[i]);
		return new Float64Array(values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return buf.putDouble(value);
	}

	@Override
	public final Float64Array dfloat()
	{
		if (this.dtype() == DTYPE.D)
			return this;
		return new Float64Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.D.toByte()));
	}

	@Override
	public final Float64Array ftfloat()
	{
		if (this.dtype() == DTYPE.FT)
			return this;
		return new Float64Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.FT.toByte()));
	}

	@Override
	public final Double getElement(final ByteBuffer b_in)
	{
		return new Double(b_in.getDouble());
	}

	@Override
	public Double getElement(final int i)
	{
		return new Double(this.p.getDouble(i * Double.BYTES));
	}

	@Override
	protected final byte getRankBits()
	{ return 0x07; }

	@Override
	public Float64 getScalar(final int idx)
	{
		return new Float64(this.getElement(idx).doubleValue());
	}

	@Override
	public final Float64Array gfloat()
	{
		if (this.dtype() == DTYPE.G)
			return this;
		return new Float64Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.G.toByte()));
	}

	@Override
	protected final Double[] initArray(final int size)
	{
		return new Double[size];
	}

	@Override
	public Uint64Array inot()
	{
		final LongBuffer lb = this.getBuffer().asLongBuffer();
		final long[] values = new long[this.arsize() / Double.BYTES];
		for (int i = 0; i < values.length; i++)
			values[i] = ~lb.get();
		return new Uint64Array(values);
	}

	@Override
	public final Float64Array neg()
	{
		final double[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = -values[i];
		return new Float64Array(values);
	}

	@Override
	public final Double parse(final String in)
	{
		return Double.valueOf(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Double value)
	{
		b.putDouble(value.doubleValue());
	}

	@Override
	protected final void setElement(final int i, final Double value)
	{
		this.p.putDouble(i * Double.BYTES, value.doubleValue());
	}

	public final double[] toArray()
	{
		final double[] values = new double[this.arsize() / Double.BYTES];
		this.getBuffer().asDoubleBuffer().get(values);
		return values;
	}
}
