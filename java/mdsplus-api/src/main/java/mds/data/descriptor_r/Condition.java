package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

/** depreciated **/
@Deprecated
public final class Condition extends Descriptor_R<Byte>
{
	public static final byte DEPENDENCY_AND = 10;
	public static final byte DEPENDENCY_OR = 11;
	public static final byte IGNORE_STATUS = 9;
	public static final byte IGNORE_UNDEFINED = 8;
	public static final byte NEGATE_CONDITION = 7;

	public Condition(final byte mode, final Descriptor<?> cond)
	{
		super(DTYPE.CONDITION, ByteBuffer.allocateDirect(Byte.BYTES).order(Descriptor.BYTEORDER).put(mode), cond);
	}

	public Condition(final ByteBuffer b)
	{
		super(b);
	}

	public Condition(final Descriptor<?>... arguments)
	{
		super(DTYPE.CONDITION, null, arguments);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	@Override
	public final Byte getAtomic()
	{
		switch (this.length())
		{
		case 1:
			return Byte.valueOf(this.p.get(0));
		case 2:
			return Byte.valueOf((byte) this.p.getShort(0));
		case 4:
			return Byte.valueOf((byte) this.p.getInt(0));
		default:
			return Byte.valueOf((byte) 0);
		}
	}

	public final Descriptor<?> getCondition()
	{ return this.getDescriptor(0); }

	@Override
	public final Descriptor<?> getData_(final DTYPE... omits)
	{
		return this.getCondition().getData(omits);
	}

	public final byte getModifier()
	{ return this.getAtomic().byteValue(); }
}
