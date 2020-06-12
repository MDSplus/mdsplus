package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Call extends Descriptor_R<Byte>
{
	public static final class def_cat
	{
		public static final short TdiCAT_B = 0x0300;
		public static final short TdiCAT_BU = 0x0100;
		public static final short TdiCAT_COMPLEX = 0x1000;
		public static final short TdiCAT_D = ((short) 0x8000 | def_cat.TdiCAT_FLOAT | 7);
		public static final short TdiCAT_F = ((short) 0x8000 | def_cat.TdiCAT_FLOAT | 3);
		public static final short TdiCAT_FC = (def_cat.TdiCAT_COMPLEX | def_cat.TdiCAT_F);
		public static final short TdiCAT_FLOAT = 0x0700;
		public static final short TdiCAT_LENGTH = 0x00ff;
		public static final short TdiCAT_SIGNED = ((short) 0x8000 | def_cat.TdiCAT_B | def_cat.TdiCAT_LENGTH);
		public static final short TdiCAT_UNSIGNED = ((short) 0x8000 | def_cat.TdiCAT_BU | def_cat.TdiCAT_LENGTH);
		public static final short TdiCAT_WIDE_EXP = 0x0800;
		public final short cat; /* category code */
		public final byte digits; /* size of text conversion */
		public final String fname; /* exponent name for floating decompile */
		public final byte length; /* size in bytes */
		public final String name; /* text for decompile */

		public def_cat(final String name, final short cat, final byte length, final byte digits, final String fname)
		{
			this.name = name;
			this.cat = cat;
			this.length = length;
			this.digits = digits;
			this.fname = fname;
		}
	}

	private static final def_cat[] tdiDefCat = new def_cat[]
	{ //
			new def_cat("MISSING", (short) 0, (byte) 0, (byte) 0, null), // 0
			new def_cat("V", (short) 0x7fff, (byte) 0, (byte) 0, null), // 1
			new def_cat("BU", (short) 0x8100, (byte) 1, (byte) 4, null), // 2
			new def_cat("WU", (short) 0x8101, (byte) 2, (byte) 8, null), // 3
			new def_cat("LU", (short) 0x8103, (byte) 4, (byte) 12, null), // 4
			new def_cat("QU", (short) 0x8107, (byte) 8, (byte) 20, null), // 5
			new def_cat("B", (short) 0x8300, (byte) 1, (byte) 4, null), // 6
			new def_cat("W", (short) 0x8301, (byte) 2, (byte) 8, null), // 7
			new def_cat("L", (short) 0x8303, (byte) 4, (byte) 12, null), // 8
			new def_cat("Q", (short) 0x8307, (byte) 8, (byte) 20, null), // 9
			new def_cat("F", (short) 0x8703, (byte) 4, (byte) 16, "F"), // 10
			new def_cat("D", (short) 0x8707, (byte) 8, (byte) 24, "V"), // 11
			new def_cat("FC", (short) 0x9703, (byte) 8, (byte) 32, "F"), // 12
			new def_cat("DC", (short) 0x9707, (byte) 16, (byte) 48, "V"), // 13
			new def_cat("T", (short) 0x8000, (byte) 0, (byte) 0, null), // 14
			new def_cat("NU", (short) 0x7fff, (byte) 0, (byte) 0, null), // 15
			new def_cat("NL", (short) 0x7fff, (byte) 0, (byte) 0, null), // 16
			new def_cat("NLO", (short) 0x7fff, (byte) 0, (byte) 0, null), // 17
			new def_cat("NR", (short) 0x7fff, (byte) 0, (byte) 0, null), // 18
			new def_cat("NRO", (short) 0x7fff, (byte) 0, (byte) 0, null), // 19
			new def_cat("NZ", (short) 0x7fff, (byte) 0, (byte) 0, null), // 20
			new def_cat("NP", (short) 0x7fff, (byte) 0, (byte) 0, null), // 21
			new def_cat("ZI", (short) 0x7fff, (byte) 0, (byte) 0, null), // 22
			new def_cat("ZEM", (short) 0x7fff, (byte) 2, (byte) 0, null), // 23
			new def_cat("DSC", (short) 0x7fff, (byte) 8, (byte) 0, null), // 24
			new def_cat("OU", (short) 0x810f, (byte) 16, (byte) 36, null), // 25
			new def_cat("O", (short) 0x830f, (byte) 16, (byte) 36, null), // 26
			new def_cat("G", (short) 0x8707, (byte) 8, (byte) 24, "G"), // 27
			new def_cat("H", (short) 0x8f0f, (byte) 16, (byte) 40, "H"), // 28
			new def_cat("GC", (short) 0x9707, (byte) 16, (byte) 48, "G"), // 29
			new def_cat("HC", (short) 0x9f0f, (byte) 0, (byte) 0, null), // 30
			new def_cat("CIT", (short) 0x7fff, (byte) 10, (byte) 0, null), // 31
			new def_cat("BPV", (short) 0x7fff, (byte) 8, (byte) 0, null), // 32
			new def_cat("BLV", (short) 0x7fff, (byte) 8, (byte) 0, null), // 33
			new def_cat("VU", (short) 0x7fff, (byte) 0, (byte) 0, null), // 34
			new def_cat("ADT", (short) 0x7fff, (byte) 8, (byte) 23, null), // 35
			new def_cat("36", (short) 0x7fff, (byte) 0, (byte) 0, null), // 36
			new def_cat("VT", (short) 0x7fff, (byte) 0, (byte) 0, null), // 37
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 38
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 39
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 40
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 41
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 42
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 43
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 44
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 45
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 46
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 47
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 48
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 49
			new def_cat("?", (short) 0x7fff, (byte) 0, (byte) 0, null), // 50
			new def_cat("P", (short) 0x7fff, (byte) 8, (byte) 36, null), // 51
			new def_cat("FS", (short) 0x8703, (byte) 4, (byte) 16, "E"), // 52
			new def_cat("FT", (short) 0x8707, (byte) 8, (byte) 32, "D"), // 53
			new def_cat("FSC", (short) 0x9703, (byte) 8, (byte) 24, "E"), // 54
			new def_cat("FTC", (short) 0x9707, (byte) 16, (byte) 48, "D"),// 55
	};

	public Call(final ByteBuffer b)
	{
		super(b);
	}

	public Call(final Descriptor<?>... arguments)
	{
		super(DTYPE.CALL, null, arguments);
	}

	public Call(final DTYPE type, final Descriptor<?> image, final Descriptor<?> routine, final Descriptor<?>... args)
	{
		super(DTYPE.CALL, ByteBuffer.allocateDirect(Byte.BYTES).order(Descriptor.BYTEORDER).put(type.toByte()), args,
				image, routine);
	}

	public Call(final DTYPE type, final String image, final String routine, final Descriptor<?>... args)
	{
		this(type, Descriptor.valueOf(image), Descriptor.valueOf(routine), args);
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		if (this.getImage().dtype() != DTYPE.T || this.getRoutine().dtype() != DTYPE.T)
			return super.decompile(prec, pout, mode);
		pout.append(this.getImage()).append("->").append(this.getRoutine());
		if (this.length() > 0)
		{
			pout.append(':');
			final int dType = this.getAtomic().intValue() & 0xff;
			if (dType < Call.tdiDefCat.length)
				pout.append(Call.tdiDefCat[dType].name);
			else if (this.dtype() == DTYPE.NID)
				pout.append("Nid");
			else if (this.dtype() == DTYPE.PATH)
				pout.append("Path");
			else
				pout.append("%Unknown%");
		}
		this.addArguments(2, "(", ")", pout, mode);
		return pout;
	}

	public final Descriptor<?> getArguments(final int idx)
	{
		return this.getDescriptor(2 + idx);
	}

	@Override
	public final Byte getAtomic()
	{ return Byte.valueOf(this.p.get(0)); }

	public final Descriptor<?> getImage()
	{ return this.getDescriptor(0); }

	public final Descriptor<?> getRoutine()
	{ return this.getDescriptor(1); }
}
