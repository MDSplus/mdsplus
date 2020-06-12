package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;

public abstract class MODIFIER extends Function
{
	public static final class AsIs extends MODIFIER
	{
		public AsIs(final ByteBuffer b)
		{
			super(b);
		}

		public AsIs(final Descriptor<?> arg)
		{
			super(OPC.OpcAsIs, arg);
		}
	}

	public static class In extends MODIFIER
	{
		public In(final ByteBuffer b)
		{
			super(b);
		}

		public In(final Descriptor<?> arg)
		{
			super(OPC.OpcIn, arg);
		}
	}

	public static class InOut extends MODIFIER
	{
		public InOut(final ByteBuffer b)
		{
			super(b);
		}

		public InOut(final Descriptor<?> arg)
		{
			super(OPC.OpcInOut, arg);
		}
	}

	public static class Optional extends MODIFIER
	{
		public Optional(final ByteBuffer b)
		{
			super(b);
		}

		public Optional(final Descriptor<?> arg)
		{
			super(OPC.OpcOptional, arg);
		}
	}

	public static class Out extends MODIFIER
	{
		public Out(final ByteBuffer b)
		{
			super(b);
		}

		public Out(final Descriptor<?> arg)
		{
			super(OPC.OpcOut, arg);
		}
	}

	public static class Private extends MODIFIER
	{
		public Private(final ByteBuffer b)
		{
			super(b);
		}

		public Private(final Descriptor<?> arg)
		{
			super(OPC.OpcPrivate, arg);
		}
	}

	public static class Public extends MODIFIER
	{
		public Public(final ByteBuffer b)
		{
			super(b);
		}

		public Public(final Descriptor<?> arg)
		{
			super(OPC.OpcPublic, arg);
		}
	}

	public static final boolean coversOpCode(final OPC opcode)
	{
		switch (opcode)
		{
		case OpcIn: /* input argument */
		case OpcInOut: /* input and output argument */
		case OpcOptional: /* optional argument */
		case OpcOut: /* output argument */
		case OpcPrivate: /* private ident */
		case OpcPublic: /* public ident */
			return true;
		default:
			return false;
		}
	}

	public static MODIFIER deserialize(final ByteBuffer b) throws MdsException
	{
		final OPC opcode = OPC.get(b.getShort(b.getInt(Descriptor._ptrI)));
		switch (opcode)
		{
		case OpcAsIs:
			return new AsIs(b);
		case OpcIn:
			return new In(b);
		case OpcInOut:
			return new InOut(b);
		case OpcOptional:
			return new Optional(b);
		case OpcOut:
			return new Out(b);
		case OpcPrivate:
			return new Public(b);
		case OpcPublic:
			return new Public(b);
		default:
			throw new MdsException(MdsException.TdiINV_OPC);
		}
	}

	public static final MODIFIER In(final Descriptor<?> ident)
	{
		return new In(ident);
	}

	public static final MODIFIER InOut(final Descriptor<?> ident)
	{
		return new InOut(ident);
	}

	public static final MODIFIER Optional(final Descriptor<?> ident)
	{
		return new Optional(ident);
	}

	public static final MODIFIER Out(final Descriptor<?> ident)
	{
		return new Out(ident);
	}

	public static final MODIFIER Private(final Descriptor<?> ident)
	{
		return new Private(ident);
	}

	public static final MODIFIER Public(final Descriptor<?> ident)
	{
		return new Public(ident);
	}

	private MODIFIER(final ByteBuffer b)
	{
		super(b);
	}

	private MODIFIER(final OPC opcode, final Descriptor<?> ident)
	{
		super(opcode, ident);
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		pout.append(this.getName()).append(' ');
		final Descriptor<?> ptr = this.getDescriptor(0);
		if (ptr.dtype() == DTYPE.T)
			pout.append(ptr.toString());
		else
			ptr.decompile(Descriptor.P_SUBS, pout, mode & ~Descriptor.DECO_X);
		return pout;
	}
}
