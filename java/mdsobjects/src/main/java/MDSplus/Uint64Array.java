package MDSplus;

/**
 * Array description for DTYPE_QU
 *
 * Constructors:Uint64Array(long[])
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.52
 */
public class Uint64Array extends Array
{
	long[] datum;

	public Uint64Array(long[] inDatum)
	{
		help = null;
		units = null;
		error = null;
		validation = null;
		clazz = CLASS_A;
		dtype = DTYPE_QU;
		dims = new int[1];
		dims[0] = inDatum.length;
		datum = new long[inDatum.length];
		System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
	}

	public Uint64Array(long[] inDatum, int[] dims) throws MdsException
	{
		this(inDatum, dims, null, null, null, null);
	}

	public Uint64Array(long[] inDatum, int[] dims, Data help, Data units, Data error, Data validation)
			throws MdsException
	{
		super(dims, help, units, error, validation);
		clazz = CLASS_A;
		dtype = DTYPE_QU;
		datum = new long[inDatum.length];
		System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
		setShape(dims);
	}

	@Override
	public boolean equals(Object obj)
	{
		if (!(obj instanceof Uint64Array))
			return false;
		final Uint64Array data = (Uint64Array) obj;
		if (data.dims.length != dims.length)
			return false;
		for (int i = 0; i < dims.length; i++)
			if (data.dims[i] != dims[i])
				return false;
		for (int i = 0; i < datum.length; i++)
			if (data.datum[i] != datum[i])
				return false;
		return true;
	}

	public static Data getData(long[] datum, int[] dims, Data help, Data units, Data error, Data validation)
	{
		try
		{
			return new Uint64Array(datum, dims, help, units, error, validation);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public Data getElementAt(int idx)
	{
		return new Uint64(datum[idx]);
	}

	@Override
	public int getSize()
	{ return datum.length; }

	@Override
	protected Array getPortionAt(int startIdx, int[] newDims, int newSize) throws MdsException
	{
		final long newDatum[] = new long[newSize];
		System.arraycopy(datum, startIdx, newDatum, 0, newSize);
		return new Uint64Array(newDatum, newDims);
	}

	@Override
	public void setElementAt(int idx, Data data) throws MdsException
	{
		datum[idx] = data.getLong();
	}

	@Override
	protected void setPortionAt(Array data, int startIdx, int size) throws MdsException
	{
		final long[] newDatum = data.getLongArray();
		System.arraycopy(datum, startIdx, newDatum, 0, size);
	}

	@Override
	public byte[] getByteArray()
	{
		final byte[] retDatum = new byte[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = (byte) datum[i];
		return retDatum;
	}

	@Override
	public short[] getShortArray()
	{
		final short[] retDatum = new short[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = (short) datum[i];
		return retDatum;
	}

	@Override
	public int[] getIntArray()
	{
		final int[] retDatum = new int[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = (int) datum[i];
		return retDatum;
	}

	@Override
	public long[] getLongArray()
	{
		final long[] retDatum = new long[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = datum[i];
		return retDatum;
	}

	@Override
	public float[] getFloatArray()
	{
		final float[] retDatum = new float[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = datum[i];
		return retDatum;
	}

	@Override
	public double[] getDoubleArray()
	{
		final double[] retDatum = new double[datum.length];
		for (int i = 0; i < datum.length; i++)
			retDatum[i] = datum[i];
		return retDatum;
	}

	@Override
	public java.lang.String[] getStringArray() throws MdsException
	{
		throw new MdsException("Cannot convert byte array to string array");
	}

	@Override
	public int getSizeInBytes()
	{ return getSize() * 8; }
}
