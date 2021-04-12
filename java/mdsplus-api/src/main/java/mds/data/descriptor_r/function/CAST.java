package mds.data.descriptor_r.function;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Float64Array;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.Float64;
import mds.data.descriptor_s.Missing;

public abstract class CAST extends Function
{
	public static final class Byte extends CAST
	{
		public Byte(final ByteBuffer b)
		{
			super(b);
		}

		public Byte(final Descriptor<?> arg)
		{
			super(OPC.OpcByte, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.bytes();
		}
	}

	public static final class Byte_Unsigned extends CAST
	{
		public Byte_Unsigned(final ByteBuffer b)
		{
			super(b);
		}

		public Byte_Unsigned(final Descriptor<?> arg)
		{
			super(OPC.OpcByteUnsigned, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.byteu();
		}
	}

	public static final class D_Complex extends CAST
	{
		public D_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public D_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcDComplex, arg);
		}
	}

	public static final class D_Float extends CAST
	{
		public D_Float(final ByteBuffer b)
		{
			super(b);
		}

		public D_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcDFloat, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.dfloat();
		}
	}

	public static final class Dble extends CAST
	{
		public Dble(final ByteBuffer b)
		{
			super(b);
		}

		public Dble(final Descriptor<?> arg)
		{
			super(OPC.OpcDble, arg);
		}
	}

	public static final class F_Complex extends CAST
	{
		public F_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public F_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcFComplex, arg);
		}
	}

	public static final class F_Float extends CAST
	{
		public F_Float(final ByteBuffer b)
		{
			super(b);
		}

		public F_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcFFloat, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.ffloat();
		}
	}

	public static final class Float extends CAST
	{
		public Float(final ByteBuffer b)
		{
			super(b);
		}

		public Float(final Descriptor<?> arg)
		{
			super(OPC.OpcFloat, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			if (data.length() > 4)
			{
				if (data instanceof Descriptor_A)
					return new Float64Array(data.getShape(), data.toDoubleArray());
				return new Float64(data.toDouble());
			}
			if (data instanceof Descriptor_A)
				return new Float32Array(data.getShape(), data.toFloatArray());
			return new Float32(data.toFloat());
		}
	}

	public static final class FS_Complex extends CAST
	{
		public FS_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public FS_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcFS_complex, arg);
		}
	}

	public static final class FS_Float extends CAST
	{
		public FS_Float(final ByteBuffer b)
		{
			super(b);
		}

		public FS_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcFS_float, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.fsfloat();
		}
	}

	public static final class FT_Complex extends CAST
	{
		public FT_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public FT_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcFT_complex, arg);
		}
	}

	public static final class FT_Float extends CAST
	{
		public FT_Float(final ByteBuffer b)
		{
			super(b);
		}

		public FT_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcFT_float, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.ftfloat();
		}
	}

	public static final class G_Complex extends CAST
	{
		public G_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public G_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcGComplex, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.gfloat();
		}
	}

	public static final class G_Float extends CAST
	{
		public G_Float(final ByteBuffer b)
		{
			super(b);
		}

		public G_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcGFloat, arg);
		}
	}

	public static final class H_Complex extends CAST
	{
		public H_Complex(final ByteBuffer b)
		{
			super(b);
		}

		public H_Complex(final Descriptor<?> arg)
		{
			super(OPC.OpcHComplex, arg);
		}
	}

	public static final class H_Float extends CAST
	{
		public H_Float(final ByteBuffer b)
		{
			super(b);
		}

		public H_Float(final Descriptor<?> arg)
		{
			super(OPC.OpcHFloat, arg);
		}
	}

	public static final class Long extends CAST
	{
		public Long(final ByteBuffer b)
		{
			super(b);
		}

		public Long(final Descriptor<?> arg)
		{
			super(OPC.OpcLong, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.longs();
		}
	}

	public static final class Long_Unsigned extends CAST
	{
		public Long_Unsigned(final ByteBuffer b)
		{
			super(b);
		}

		public Long_Unsigned(final Descriptor<?> arg)
		{
			super(OPC.OpcLongUnsigned, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.longu();
		}
	}

	public static final class Octaword extends CAST
	{
		public Octaword(final ByteBuffer b)
		{
			super(b);
		}

		public Octaword(final Descriptor<?> arg)
		{
			super(OPC.OpcOctaword, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.octawords();
		}
	}

	public static final class Octaword_Unsigned extends CAST
	{
		public Octaword_Unsigned(final ByteBuffer b)
		{
			super(b);
		}

		public Octaword_Unsigned(final Descriptor<?> arg)
		{
			super(OPC.OpcOctawordUnsigned, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.octawordu();
		}
	}

	public static final class Quadword extends CAST
	{
		public Quadword(final ByteBuffer b)
		{
			super(b);
		}

		public Quadword(final Descriptor<?> arg)
		{
			super(OPC.OpcQuadword, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.quadwords();
		}
	}

	public static final class Quadword_Unsigned extends CAST
	{
		public Quadword_Unsigned(final ByteBuffer b)
		{
			super(b);
		}

		public Quadword_Unsigned(final Descriptor<?> arg)
		{
			super(OPC.OpcQuadwordUnsigned, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.quadwordu();
		}
	}

	public static final class Text extends CAST
	{
		public Text(final ByteBuffer b)
		{
			super(b);
		}

		public Text(final Descriptor<?> arg)
		{
			super(OPC.OpcText, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.text();
		}
	}

	public static final class Word extends CAST
	{
		public Word(final ByteBuffer b)
		{
			super(b);
		}

		public Word(final Descriptor<?> arg)
		{
			super(OPC.OpcWord, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.words();
		}
	}

	public static final class Word_Unsigned extends CAST
	{
		public Word_Unsigned(final ByteBuffer b)
		{
			super(b);
		}

		public Word_Unsigned(final Descriptor<?> arg)
		{
			super(OPC.OpcWordUnsigned, arg);
		}

		@Override
		protected final Descriptor<?> evaluate(final DATA<?> data) throws MdsException
		{
			return data.wordu();
		}
	}

	public static final boolean coversOpCode(final OPC opcode)
	{
		switch (opcode)
		{
		default:
			return false;
		case OpcText:
		case OpcDble:
		case OpcByte:
		case OpcByteUnsigned:
		case OpcWord:
		case OpcWordUnsigned:
		case OpcLong:
		case OpcLongUnsigned:
		case OpcQuadword:
		case OpcQuadwordUnsigned:
		case OpcOctaword:
		case OpcOctawordUnsigned:
		case OpcFloat:
		case OpcDFloat:
		case OpcFFloat:
		case OpcFS_float:
		case OpcFT_float:
		case OpcGFloat:
		case OpcHFloat:
		case OpcDComplex:
		case OpcFComplex:
		case OpcFS_complex:
		case OpcFT_complex:
		case OpcGComplex:
		case OpcHComplex:
			return true;
		}
	}

	public static CAST deserialize(final ByteBuffer b) throws MdsException
	{
		final OPC opcode = OPC.get(b.getShort(b.getInt(Descriptor._ptrI)));
		switch (opcode)
		{
		case OpcText:
			return new Text(b);
		case OpcDble:
			return new Dble(b);
		case OpcByte:
			return new Byte(b);
		case OpcByteUnsigned:
			return new Byte_Unsigned(b);
		case OpcWord:
			return new Word(b);
		case OpcWordUnsigned:
			return new Word_Unsigned(b);
		case OpcLong:
			return new Long(b);
		case OpcLongUnsigned:
			return new Long_Unsigned(b);
		case OpcQuadword:
			return new Quadword(b);
		case OpcQuadwordUnsigned:
			return new Quadword_Unsigned(b);
		case OpcOctaword:
			return new Octaword(b);
		case OpcOctawordUnsigned:
			return new Octaword_Unsigned(b);
		case OpcFloat:
			return new Float(b);
		case OpcDFloat:
			return new D_Float(b);
		case OpcFFloat:
			return new F_Float(b);
		case OpcFS_float:
			return new FS_Float(b);
		case OpcFT_float:
			return new FT_Float(b);
		case OpcGFloat:
			return new G_Float(b);
		case OpcHFloat:
			return new H_Float(b);
		case OpcDComplex:
			return new Octaword(b);
		case OpcFComplex:
			return new Octaword(b);
		case OpcFS_complex:
			return new Octaword(b);
		case OpcFT_complex:
			return new Octaword(b);
		case OpcGComplex:
			return new Octaword(b);
		case OpcHComplex:
			return new Octaword(b);
		default:
			throw new MdsException(MdsException.TdiINV_OPC);
		}
	}

	protected CAST(final ByteBuffer b)
	{
		super(b);
	}

	private CAST(final OPC opcode, final Descriptor<?> arg)
	{
		super(opcode, arg);
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		pout.append(this.getName());
		this.addArguments(0, "(", ")", pout, mode);
		return pout;
	}

	@Override
	public final Descriptor<?> evaluate()
	{
		try
		{
			return this.evaluate(this.getArgument(0).getDATA());
		}
		catch (final MdsException e)
		{
			return Missing.NEW;
		}
	}

	protected Descriptor<?> evaluate(final DATA<?> data) throws MdsException
	{
		return this.mds.getDescriptor(this.getName(), this.getArguments());
	}
}
