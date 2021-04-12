package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Float32;

public final class Float32Array extends FLOATArray<Float>
{
	public static final Float32Array F(final float... values)
	{
		return new Float32Array(DTYPE.F, values);
	}

	public static final Float32Array F(final int shape[], final float... values)
	{
		return new Float32Array(DTYPE.F, values, shape);
	}

	public static final Float32Array FS(final float... values)
	{
		return new Float32Array(DTYPE.FS, values);
	}

	public static final Float32Array FS(final int shape[], final float... values)
	{
		return new Float32Array(DTYPE.FS, values, shape);
	}

	public Float32Array(final ByteBuffer b)
	{
		super(b);
	}

	public Float32Array(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	private Float32Array(final DTYPE dtype, final float[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	public Float32Array(final float... values)
	{
		super(DTYPE.FLOAT, values);
	}

	public Float32Array(final int shape[], final float... values)
	{
		super(DTYPE.FLOAT, values, shape);
	}

	@Override
	public final Float32Array abs()
	{
		final float[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = Math.abs(values[i]);
		return new Float32Array(values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return buf.putFloat((float) value);
	}

	@Override
	public final Float32Array ffloat()
	{
		if (this.dtype() == DTYPE.F)
			return this;
		return new Float32Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.F.toByte()));
	}

	@Override
	public final Float32Array fsfloat()
	{
		if (this.dtype() == DTYPE.FS)
			return this;
		return new Float32Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.FS.toByte()));
	}

	@Override
	public final Float getElement(final ByteBuffer b_in)
	{
		return new Float(b_in.getFloat());
	}

	@Override
	public final Float getElement(final int i)
	{
		return new Float(this.p.getFloat(i * Float.BYTES));
	}

	@Override
	protected byte getRankBits()
	{ return 0x03; }

	@Override
	public Float32 getScalar(final int idx)
	{
		return new Float32(this.getElement(idx).floatValue());
	}

	@Override
	protected final Float[] initArray(final int size)
	{
		return new Float[size];
	}

	@Override
	public Uint32Array inot()
	{
		final IntBuffer ib = this.getBuffer().asIntBuffer();
		final int[] values = new int[this.arsize() / Double.BYTES];
		for (int i = 0; i < values.length; i++)
			values[i] = ~ib.get();
		return new Uint32Array(values);
	}

	@Override
	public final Float32Array neg()
	{
		final float[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = -values[i];
		return new Float32Array(values);
	}

	@Override
	public final Float parse(final String in)
	{
		return Float.valueOf(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Float value)
	{
		b.putFloat(value.floatValue());
	}

	@Override
	protected final void setElement(final int i, final Float value)
	{
		this.p.putFloat(i * Float.BYTES, value.floatValue());
	}

	public final float[] toArray()
	{
		final float[] values = new float[this.arsize() / Float.BYTES];
		this.getBuffer().asFloatBuffer().get(values);
		return values;
	}
}
