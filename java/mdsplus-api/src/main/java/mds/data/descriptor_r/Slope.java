package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_a.Float64Array;

/** depreciated **/
@Deprecated
public final class Slope extends Descriptor_R<Number>
{
	public Slope(final ByteBuffer b)
	{
		super(b);
	}

	public Slope(final Descriptor<?>... arguments)
	{
		super(DTYPE.SLOPE, null, arguments);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getBegin()
	{ return this.getDescriptor(1); }

	@Override
	public final Descriptor<?> getData_(final DTYPE... omits)
	{
		return new Float64Array(
				Range.range(this.getBegin().toDouble(), this.getEnding().toDouble(), this.getSlope().toDouble()));
	}

	public final Descriptor<?> getEnding()
	{ return this.getDescriptor(2); }

	@Override
	public final int[] getShape()
	{
		return new int[]
		{ (int) ((this.getEnding().toDouble() - this.getBegin().toDouble()) / this.getSlope().toDouble()) };
	}

	public final Descriptor<?> getSlope()
	{ return this.getDescriptor(0); }
}
