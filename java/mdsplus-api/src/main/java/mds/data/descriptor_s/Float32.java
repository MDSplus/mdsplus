package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public final class Float32 extends FLOAT<Float>
{
	public static final Float32 F(final float value)
	{
		return new Float32(DTYPE.F, value);
	}

	public static final Float32 FS(final float value)
	{
		return new Float32(DTYPE.FS, value);
	}

	public Float32()
	{
		this(0);
	}

	public Float32(final ByteBuffer b)
	{
		super(b);
	}

	Float32(final DTYPE dtype, final float value)
	{
		super(dtype, value);
	}

	public Float32(final float value)
	{
		this(DTYPE.FLOAT, value);
	}

	@Override
	public final Float32 abs()
	{
		return new Float32(this.dtype(), Math.abs(this.getValue()));
	}

	@Override
	public boolean equals(final Object obj)
	{
		if (!super.equals(obj))
			return false;
		final float tid = this.toFloat(), tad = ((Float32) obj).toFloat();
		if (tid == 0)
			return tad == 0;
		final float ttt = tad / tid * tad;
		return (ttt / tid - 2 * tad / tid + 1) / (ttt + tid) < 1e-13;
	}

	@Override
	public final Float32 ffloat()
	{
		if (this.dtype() == DTYPE.F)
			return this;
		return Float32.F(this.getValue());
	}

	@Override
	public final Float32 fsfloat()
	{
		if (this.dtype() == DTYPE.FS)
			return this;
		return Float32.FS(this.getValue());
	}

	@Override
	public final Float getAtomic()
	{ return new Float(this.p.getFloat(0)); }

	@Override
	protected final byte getRankBits()
	{ return 0x03; }

	public final float getValue()
	{ return this.getBuffer().getFloat(0); }

	@Override
	public final Uint32 inot()
	{
		return new Uint32(~this.getBuffer().getInt());
	}

	@Override
	public final Float32 neg()
	{
		return new Float32(this.dtype(), -this.getValue());
	}

	@Override
	public final Float parse(final String in)
	{
		return Float.valueOf(in);
	}

	public final void setValue(final float value)
	{
		this.b.putFloat(this.pointer(), value);
	}

	@Override
	public final StringDsc text()
	{
		return new StringDsc(String.format("%16.5E", this.getAtomic()).replace("E", this.getSuffix()));
	}
}
