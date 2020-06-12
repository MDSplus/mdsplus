package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public final class Float64 extends FLOAT<Double>
{
	public static final Float64 D(final double value)
	{
		return new Float64(DTYPE.D, value);
	}

	public static final Float64 FT(final double value)
	{
		return new Float64(DTYPE.FT, value);
	}

	public static final Float64 G(final double value)
	{
		return new Float64(DTYPE.G, value);
	}

	public Float64()
	{
		this(0);
	}

	public Float64(final ByteBuffer b)
	{
		super(b);
	}

	public Float64(final double value)
	{
		this(DTYPE.DOUBLE, value);
	}

	protected Float64(final DTYPE dtype, final double value)
	{
		super(dtype, value);
	}

	@Override
	public final Float64 abs()
	{
		return new Float64(this.dtype(), Math.abs(this.getValue()));
	}

	@Override
	public final Float64 dfloat()
	{
		if (this.dtype() == DTYPE.D)
			return this;
		return Float64.D(this.getValue());
	}

	@Override
	public boolean equals(final Object obj)
	{
		if (!super.equals(obj))
			return false;
		final double tid = this.toDouble(), tad = ((Float64) obj).toDouble();
		if (tid == 0)
			return tad == 0;
		final double ttt = tad / tid * tad;
		return (ttt / tid - 2 * tad / tid + 1) / (ttt + tid) < 1e-13;
	}

	@Override
	public final Float64 ftfloat()
	{
		if (this.dtype() == DTYPE.FT)
			return this;
		return Float64.FT(this.getValue());
	}

	@Override
	public final Double getAtomic()
	{ return new Double(this.p.getDouble(0)); }

	@Override
	protected final byte getRankBits()
	{ return 0x07; }

	public final double getValue()
	{ return this.getBuffer().getDouble(0); }

	@Override
	public final Float64 gfloat()
	{
		if (this.dtype() == DTYPE.G)
			return this;
		return Float64.G(this.getValue());
	}

	@Override
	public final Uint64 inot()
	{
		return new Uint64(~this.getBuffer().getLong());
	}

	@Override
	public final Float64 neg()
	{
		return new Float64(this.dtype(), -this.getValue());
	}

	@Override
	public final Double parse(final String in)
	{
		return Double.valueOf(in);
	}

	public final void setValue(final double value)
	{
		this.b.putDouble(this.pointer(), value);
	}

	@Override
	public final StringDsc text()
	{
		return new StringDsc(String.format("%32.15E", this.getAtomic()).replace("E", this.getSuffix()));
	}
}
