package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import mds.Mds;
import mds.MdsException;
import mds.data.*;
import mds.data.descriptor_a.Int8Array;
import mds.data.descriptor_r.*;
import mds.data.descriptor_s.*;
import mds.mdsip.Message;

/** DSC (24) **/
public abstract class Descriptor<T>
{
	static protected class FLAG
	{
		public static final boolean and(final FLAG flag, final boolean in)
		{
			if (flag != null)
				flag.flag = flag.flag && in;
			return in;
		}

		public static final boolean or(final FLAG flag, final boolean in)
		{
			if (flag != null)
				flag.flag = flag.flag || in;
			return in;
		}

		public static final boolean set(final FLAG flag, final boolean in)
		{
			if (flag != null)
				flag.flag = in;
			return in;
		}

		public static final boolean xor(final FLAG flag, final boolean in)
		{
			if (flag != null)
				flag.flag = flag.flag ^ in;
			return in;
		}

		public boolean flag = true;

		public FLAG()
		{
		}
	}

	public static final ByteOrder BYTEORDER = ByteOrder.nativeOrder();
	public static final short BYTES = 8;
	protected static final int _clsB = 3;
	protected static final int _lenS = 0;
	protected static final int _ptrI = 4;
	protected static final int _typB = 2;
	protected static final int DECO_NRM = 0;
	protected static final int DECO_STR = 1;
	protected static final int DECO_STRX = Descriptor.DECO_X | Descriptor.DECO_STR;
	protected static final int DECO_X = 2;
	protected static Mds mds_local = Mds.getLocal();
	protected static final byte P_ARG = 88;
	protected static final byte P_STMT = 96;
	protected static final byte P_SUBS = 0;

	private static final ByteBuffer Buffer(final byte[] buf, final boolean swap_little)
	{
		return Descriptor.Buffer(buf, swap_little ? ByteOrder.LITTLE_ENDIAN : ByteOrder.BIG_ENDIAN);
	}

	private static final ByteBuffer Buffer(final byte[] buf, final ByteOrder bo)
	{
		return ByteBuffer.wrap(buf).asReadOnlyBuffer().order(bo);
	}

	/** null safe decompile of the given Descriptor **/
	public static final String deco(final Descriptor<?> t)
	{
		return t == null ? "*" : t.decompile();
	}

	/**
	 * Returns the Descriptor deserialized from the given byte[] with native
	 * byte order (Descriptor.BYTEORDER)
	 **/
	public static final Descriptor<?> deserialize(final byte[] buf) throws MdsException
	{
		if (buf == null)
			return null;
		return Descriptor.deserialize(Descriptor.Buffer(buf, Descriptor.BYTEORDER));
	}

	/**
	 * Returns the Descriptor deserialized from the given byte[] with byte order
	 **/
	public static final Descriptor<?> deserialize(final byte[] buf, final boolean swap) throws MdsException
	{
		if (buf == null)
			return null;
		return Descriptor.deserialize(Descriptor.Buffer(buf, swap));
	}

	/** Returns the Descriptor deserialized from the given ByteBuffer **/
	public static Descriptor<?> deserialize(final ByteBuffer bi) throws MdsException
	{
		if (!bi.hasRemaining())
			return Missing.NEW;
		final ByteBuffer b = bi.slice().order(bi.order());
		switch (b.get(Descriptor._clsB))
		{
		case Descriptor_A.CLASS:
			return Descriptor_A.deserialize(b);
		case Descriptor_APD.CLASS:
			return Descriptor_APD.deserialize(b);
		case Descriptor_CA.CLASS:
			return Descriptor_CA.deserialize(b);
		case Descriptor_D.CLASS:
			return Descriptor_S.deserialize(b);
		case Descriptor_R.CLASS:
			return Descriptor_R.deserialize(b);
		case Descriptor_S.CLASS:
			return Descriptor_S.deserialize(b);
		case Descriptor_XD.CLASS:
			return Descriptor_XS.deserialize(b);
		case Descriptor_XS.CLASS:
			return Descriptor_XS.deserialize(b);
		}
		throw new MdsException(String.format("Unsupported class %s", Descriptor.getDClassName(b.get(Descriptor._clsB))),
				0);
	}

	/** null safe sloppy decompile of the given Descriptor **/
	public static final String Dsc2String(final Descriptor<?> t)
	{
		return t == null ? "*" : t.toString();
	}

	protected final static DATA<?>[] getDATAs(final Descriptor<?>... args)
	{
		final DATA<?>[] data_args = new DATA[args.length];
		for (int i = 0; i < args.length; i++)
			data_args[i] = args[i].getDATA();
		return data_args;
	}

	/** Returns the element length of the given dtype **/
	public static final short getDataSize(final DTYPE bu, final int length)
	{
		switch (bu)
		{
		default:
		case T:
			return (short) length;
		case BU:
		case B:
			return 1;
		case WU:
		case W:
			return 2;
		case NID:
		case LU:
		case L:
		case F:
		case FS:
			return 4;
		case Q:
		case QU:
		case FC:
		case FSC:
		case D:
		case FT:
		case G:
			return 8;
		case O:
		case OU:
		case DC:
		case FTC:
		case GC:
			return 16;
		}
	}

	/** Returns the name of the given dclass **/
	public static final String getDClassName(final byte dclass)
	{
		switch (dclass)
		{
		case Descriptor_S.CLASS:
			return "CLASS_S";
		case Descriptor_D.CLASS:
			return "CLASS_D";
		case Descriptor_R.CLASS:
			return "CLASS_R";
		case Descriptor_A.CLASS:
			return "CLASS_A";
		case Descriptor_XS.CLASS:
			return "CLASS_XS";
		case Descriptor_XD.CLASS:
			return "CLASS_XD";
		case Descriptor_CA.CLASS:
			return "CLASS_CA";
		case Descriptor_APD.CLASS:
			return "CLASS_APD";
		default:
			return "CLASS_" + (dclass & 0xFF);
		}
	}

	public static DTYPE getDtype(final ByteBuffer b)
	{
		return DTYPE.get(b.get(Descriptor._typB));
	}

	public static final Descriptor<?> getLocal(final FLAG local, final Descriptor<?> dsc)
	{
		if (dsc == null || dsc.isLocal())
			return dsc;
		return dsc.getLocal(local);
	}

	protected final static Descriptor<?>[] getLocals(final FLAG local, final Descriptor<?>... args)
	{
		final Descriptor<?>[] local_args = new Descriptor<?>[args.length];
		for (int i = 0; i < args.length; i++)
			local_args[i] = Descriptor.getLocal(local, args[i]);
		return local_args;
	}

	public static final boolean isLocal(final Descriptor<?> dsc)
	{
		if (dsc == null)
			return true;
		return dsc.isLocal();
	}

	public static final boolean isMissing(final Descriptor<?> dsc)
	{
		return dsc == null || dsc == Missing.NEW;
	}

	public static final Descriptor<?> NEW(final Object obj) throws MdsException
	{
		if (obj == null)
			return Missing.NEW;
		if (obj instanceof String)
			return new StringDsc((String) obj);
		if (obj instanceof Number)
			return Descriptor.NEW(obj);
		throw new MdsException("Conversion form " + obj.getClass().getName() + " not yet implemented.");
	}

	/** Returns Descriptor contained in Message **/
	public static Descriptor<?> readMessage(final Message msg) throws MdsException
	{
		if (msg.getDType() == DTYPE.T)
			return new StringDsc(msg.getBody());
		return Descriptor_A.readMessage(msg);
	}

	public static final Int8 valueOf(final byte val)
	{
		return new Int8(val);
	}

	public static final Float64 valueOf(final double val)
	{
		return new Float64(val);
	}

	public static final Float32 valueOf(final float val)
	{
		return new Float32(val);
	}

	public static final Int32 valueOf(final int val)
	{
		return new Int32(val);
	}

	public static final Int64 valueOf(final long val)
	{
		return new Int64(val);
	}

	public static final NUMBER<? extends Number> valueOf(final Number value)
	{
		return Descriptor.valueOf(value, true);
	}

	public static final NUMBER<? extends Number> valueOf(final Number value, final boolean signed)
	{
		if (value instanceof Integer)
			return signed ? new Int32(value.intValue()) : new Uint32(value.intValue());
		if (value instanceof Long)
			return signed ? new Int64(value.longValue()) : new Uint64(value.longValue());
		if (value instanceof Float)
			return new Float32(value.floatValue());
		if (value instanceof Double)
			return new Float64(value.doubleValue());
		if (value instanceof Short)
			return signed ? new Int16(value.shortValue()) : new Uint16(value.shortValue());
		if (value instanceof Byte)
			return signed ? new Int8(value.byteValue()) : new Uint8(value.byteValue());
		if (value instanceof BigInteger)
			return signed ? new Int128((BigInteger) value) : new Uint128((BigInteger) value);
		return new Float32(value.floatValue());// default
	}

	public static final Int16 valueOf(final short val)
	{
		return new Int16(val);
	}

	public static final Descriptor<?> valueOf(final String val)
	{
		return val == null ? Missing.NEW : new StringDsc(val);
	}

	protected final ByteBuffer b, p;
	protected Mds mds;
	protected TREE tree;
	protected Descriptor<?> VALUE;
	private boolean islocal = false;

	public Descriptor(final ByteBuffer b)
	{
		this.tree = TREE.getActiveTree();
		this.mds = this.tree == null ? Mds.getActiveMds() : this.getMds();
		this.b = b.slice().order(b.order());
		this.p = ((ByteBuffer) this.b.duplicate().position(this.pointer() == 0 ? this.b.limit() : this.pointer()))
				.slice().order(this.b.order());
	}

	public Descriptor(final short length, final DTYPE dtype, final byte dclass, final ByteBuffer data,
			final int pointer, int size)
	{
		this.tree = TREE.getActiveTree();
		this.mds = this.tree == null ? Mds.getActiveMds() : this.getMds();
		size += pointer;
		if (data != null)
			size += data.remaining();
		this.b = ByteBuffer.allocateDirect(size).order(Descriptor.BYTEORDER);
		this.b.putShort(Descriptor._lenS, length);
		this.b.put(Descriptor._typB, (byte) dtype.ordinal());
		this.b.put(Descriptor._clsB, dclass);
		if (data == null)
			this.b.putInt(Descriptor._ptrI, 0);
		else
		{
			this.b.putInt(Descriptor._ptrI, pointer);
			((ByteBuffer) this.b.position(pointer)).put(data.slice()).rewind();
		}
		this.p = ((ByteBuffer) this.b.duplicate().position(this.pointer() == 0 ? this.b.limit() : this.pointer()))
				.slice().order(this.b.order());
	}

	public Function as_is()
	{
		return Function.AS_IS(this);
	}

	/** (3,b) descriptor class code **/
	public final byte dclass()
	{
		return this.b.get(Descriptor._clsB);
	}

	/** Returns the decompiled string **/
	public final String decompile()
	{
		return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_NRM).toString();
	}

	/** Core method for decompiling (dummy) **/
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		pout.append("<Descriptor(");
		pout.append(this.length() & 0xFFFF).append(',');
		pout.append(this.dtype().ordinal()).append(',');
		pout.append(this.dclass() & 0xFF).append(',');
		pout.append(this.pointer() & 0xFFFFFFFFl);
		return pout.append(")>");
	}

	/** Returns the decompiled string with first level indentation **/
	public final String decompileX()
	{
		return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_X).toString();
	}

	public final Descriptor<?> doAsTask() throws MdsException
	{
		return this.mds.getAPI().tdiDoTask(this.getTree(), this);
	}

	/** (2,b) data type code **/
	public final DTYPE dtype()
	{
		return Descriptor.getDtype(this.b);
	}

	@Override
	public boolean equals(final Object obj)
	{
		if (this.getClass() != obj.getClass())
			return false;
		final Descriptor<?> that = (Descriptor<?>) obj;
		if (this.length() != that.length())
			return false;
		if (this.dclass() != that.dclass())
			return false;
		if (this.dtype() != that.dtype())
			return false;
		if (this.pointer() != that.pointer())
			return false;
		return true;
	}

	/** Evaluates Descriptor remotely and returns result Descriptor **/
	public Descriptor<?> evaluate()
	{
		if (this instanceof DATA)
			return this;
		try
		{
			if (this.isLocal())
				return this.evaluate_lib();
			return this.mds.getAPI().tdiEvaluate(this.tree, this).getData();
		}
		catch (final MdsException e)
		{
			return Missing.NEW;
		}
	}

	public Descriptor<?> evaluate_lib() throws MdsException
	{
		if (this instanceof DATA)
			return this;
		if (this.use_mds_local())
			return Descriptor.mds_local.getAPI().tdiEvaluate(null, this.getLocal()).getData();
		return this.mds.getAPI().tdiEvaluate(this.tree, this).getData();
	}

	/** Returns the value<T> of the body directed to by pointer **/
	public abstract T getAtomic();

	/** Returns the value as raw ByteBuffer **/
	public ByteBuffer getBuffer()
	{
		return this.p.asReadOnlyBuffer().order(this.p.order());
	}

	/**
	 * Returns the data of the Descriptor, i.e. DATA($THIS)
	 *
	 * @throws MdsException
	 **/
	public Descriptor<?> getData(final DTYPE... omits)
	{
		try
		{
			if (this instanceof DATA)
				return this;
			if (this.use_mds_local())
				return Descriptor.mds_local.getDescriptor(this.tree, "DATA(($;))", this.getLocal());
			return this.getData_(omits);
		}
		catch (final MdsException e)
		{
			return Missing.NEW;
		}
	}

	public final DATA<?> getDATA()
	{
		return (DATA<?>) this.getData();
	}

	@SuppressWarnings("static-method")
	protected Descriptor<?> getData_(final DTYPE... omits) throws MdsException
	{
		throw DATA.dataerror;
	}

	public Descriptor_A<?> getDataA() throws MdsException
	{
		final Descriptor<?> dsc = this.getData().toDescriptor();
		if (dsc instanceof Descriptor_A)
			return (Descriptor_A<?>) dsc;
		throw new MdsException(MdsException.TdiINVDTYDSC);
	}

	public Descriptor<?> getDataD()
	{
		return this.getData().toDescriptor();
	}

	public Descriptor_S<?> getDataS() throws MdsException
	{
		final Descriptor<?> dsc = this.getData().toDescriptor();
		if (dsc instanceof Descriptor_S)
			return (Descriptor_S<?>) dsc;
		throw new MdsException(MdsException.TdiNOT_NUMBER);
	}

	/** Returns the dclass name of the Descriptor **/
	public final String getDClassName()
	{
		return Descriptor.getDClassName(this.dclass());
	}

	/** Returns the value cast to Descriptor **/
	public final Descriptor<?> getDescriptor() throws MdsException
	{
		return Descriptor.deserialize(this.getBuffer());
	}

	public Descriptor<?> getDimension()
	{
		return this.getDimension(0);
	}

	public Descriptor<?> getDimension(final int idx)
	{
		final Descriptor<?> dsc = this.getData(DTYPE.SIGNAL);
		if (dsc instanceof Signal)
			return dsc.getDimension(idx);
		return null;
	}

	/** Returns the dtype name of the Descriptor **/
	public String getDTypeName()
	{
		return this.dtype().label;
	}

	public Descriptor<?> getHelp()
	{
		final Descriptor<?> dsc = this.getData(DTYPE.PARAM);
		if (dsc instanceof Param)
			return dsc.getHelp();
		return null;
	}

	final public Descriptor<?> getLocal()
	{
		if (this.islocal)
			return this;
		return this.getLocal(null);
	}

	/**
	 * Returns the local version of the Descriptor, i.e. it will download all
	 * required data
	 *
	 * @throws MdsException
	 **/
	public Descriptor<?> getLocal(final FLAG local)
	{
		if (this.isLocal())
			return this;
		return this.getLocal_(local);
	}

	public Descriptor<?> getLocal_(final FLAG local)
	{
		FLAG.set(local, false);
		final Descriptor<?> eval = this.evaluate();
		return eval.getLocal();
	}

	protected Mds getMds()
	{
		return this.getTree().getMds();
	}

	public Descriptor<?> getRaw()
	{
		final Descriptor<?> dsc = this.getData(DTYPE.SIGNAL);
		if (dsc instanceof Signal)
			return dsc.getRaw();
		return null;
	}

	/** Returns the shape of the Descriptor, i.e. SHAPE($THIS) **/
	public abstract int[] getShape();

	/** Returns the total size of the backing buffer in bytes **/
	public final int getSize()
	{
		return this.b.limit();
	}

	public final TREE getTree()
	{
		return this.tree;
	}

	public Descriptor<?> getUnits()
	{
		final Descriptor<?> dsc = this.getData(DTYPE.WITH_UNITS);
		if (dsc instanceof With_Units)
			return dsc.getUnits();
		return null;
	}

	@Override
	public int hashCode()
	{
		return super.hashCode();
	}

	@SuppressWarnings("static-method")
	public boolean isAtomic()
	{
		return false;
	}

	public boolean isLocal()
	{
		return this.islocal;
	}

	/** (0,s) specific length typically a 16-bit (unsigned) length **/
	public final int length()
	{
		return this.b.getShort(Descriptor._lenS) & 0xFFFF;
	}

	/** (4,i) address of first byte of data element **/
	public final int pointer()
	{
		return this.b.getInt(Descriptor._ptrI);
	}

	/** Returns serialized byte stream as ByteBuffer **/
	public final ByteBuffer serialize()
	{
		return this.b.asReadOnlyBuffer().order(this.b.order());
	}

	/** Returns serialized byte stream as byte[] **/
	public final byte[] serializeArray()
	{
		if (!this.b.isDirect() && !this.b.isReadOnly() && this.b.arrayOffset() == 0)
			return this.b.array();
		return this.serializeArray_copy();
	}

	/** Returns serialized byte stream as byte[] **/
	public final byte[] serializeArray_copy()
	{
		final ByteBuffer bs = this.serialize();
		return ByteBuffer.allocate(bs.limit()).put(bs).array();
	}

	/** Returns serialized byte stream as Descriptor **/
	public final Int8Array serializeDsc()
	{
		return new Int8Array(this.serializeArray());
	}

	public Descriptor<?> setLocal()
	{
		this.islocal = true;
		return this;
	}

	public Descriptor<?> setMds(final Mds mds)
	{
		this.mds = mds;
		return this;
	}

	public Descriptor<?> setTree(final TREE tree)
	{
		this.tree = tree;
		if (this.tree != null)
			this.mds = this.getMds();
		return this;
	}

	/** Returns value as BigInteger **/
	public BigInteger toBigInteger()
	{
		return this.toBigIntegerArray()[0];
	}

	/** Returns value as BigInteger[] **/
	public abstract BigInteger[] toBigIntegerArray();

	/** Returns value as boolean **/
	public boolean toBool()
	{
		return (this.toByte() & 1) == 1;
	}

	/** Returns value as byte **/
	public byte toByte()
	{
		return this.toByteArray()[0];
	}

	/** Returns value as byte[] **/
	public abstract byte[] toByteArray();

	public final Descriptor<T> toDescriptor()
	{
		return this;
	}

	/** Returns value as double **/
	public double toDouble()
	{
		return this.toDoubleArray()[0];
	}

	/** Returns value as double[] **/
	public abstract double[] toDoubleArray();

	/** Returns value as float **/
	public float toFloat()
	{
		return this.toFloatArray()[0];
	}

	/** Returns value as float[] **/
	public abstract float[] toFloatArray();

	/** Returns value as int **/
	public int toInt()
	{
		return this.toIntArray()[0];
	}

	/** Returns value as int[] **/
	public abstract int[] toIntArray();

	/** Returns value as long **/
	public long toLong()
	{
		return this.toLongArray()[0];
	}

	/** return value as long[] **/
	public abstract long[] toLongArray();

	/**
	 * Returns the MdsIp Message representing this Descriptor
	 *
	 * @param mid
	 * @throws MdsException
	 **/
	public Message toMessage(final byte descr_idx, final byte n_args, final byte mid) throws MdsException
	{
		final Descriptor<?> data = this.getData();
		return new Message(descr_idx, data.dtype(), n_args, data.getShape(), data.getBuffer(), mid);
	}

	/** Returns value as short **/
	public short toShort()
	{
		return this.toShortArray()[0];
	}

	/** Returns value as short[] **/
	public abstract short[] toShortArray();

	@Override
	/** Returns a sloppy decompiled string **/
	public String toString()
	{
		try
		{
			return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_STR).toString();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
			return Descriptor.getDClassName(this.dclass()) + " " + this.getDTypeName();
		}
	}

	/** Returns value as String[] **/
	public abstract String[] toStringArray();

	/** Returns a sloppy decompiled string with first level indentation **/
	public final String toStringX()
	{
		return this.decompile(Descriptor.P_STMT, new StringBuilder(1024), Descriptor.DECO_STRX).toString();
	}

	@SuppressWarnings("static-method")
	protected final boolean use_mds_local()
	{
		return (Descriptor.mds_local != null && Descriptor.mds_local.isReady() == null);
	}

	public Descriptor<?> VALUE()
	{
		return this.VALUE.VALUE();
	}

	public final Descriptor<?> VALUE(final Descriptor<?> value)
	{
		this.VALUE = value;
		return this;
	}
}
