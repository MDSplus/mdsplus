package MDSplus;

/**
 * Array description for DTYPE_L.
 *
 * Constructors: Int32Array(int[])
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.39
 */
public class Int32Array extends Array
{
	int[] datum = new int[0];

	public Int32Array(int[] inDatum)
	{
		help = null;
		units = null;
		error = null;
		validation = null;
		clazz = CLASS_A;
		dtype = DTYPE_L;
		dims = new int[1];
		dims[0] = inDatum.length;
		datum = new int[inDatum.length];
		System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
	}

	public Int32Array(int[] inDatum, int[] dims) throws MdsException
	{
		this(inDatum, dims, null, null, null, null);
	}

	public Int32Array(int[] inDatum, int[] dims, Data help, Data units, Data error, Data validation) throws MdsException
	{
		super(dims, help, units, error, validation);
		clazz = CLASS_A;
		dtype = DTYPE_L;
		datum = new int[inDatum.length];
		System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
		setShape(dims);
	}

	private static int getTotSize(int arr[][])
	{
		int totSize = 0;
		for (int i = 0; i < arr.length; i++)
		{
			totSize += arr[i].length;
		}
		return totSize;
	}

	private static int[] copyToLinearized(int arr[][], int linArr[])
	{
		int currSize = 0;
		for (int i = 0; i < arr.length; i++)
		{
			System.arraycopy(arr[i], 0, linArr, currSize, arr[i].length);
			currSize += arr[i].length;
		}
		return linArr;
	}

	// NOTE same diemnsion assumed along sub arrays
	private static int[] getDims(int arr[][])
	{
		final int d[] = new int[2];
		d[0] = arr.length;
		d[1] = arr[0].length;
		return d;
	}

	public Int32Array(int[][] inDatum) throws MdsException
	{
		this(copyToLinearized(inDatum, new int[(getTotSize(inDatum))]), getDims(inDatum));
	}

	// NOTE same dimension assumed along sub arrays
	// Get dimension of tridimensional array
	private static int getTotSize(int arr[][][])
	{
		int totSize = 0;
		for (int i = 0; i < arr.length; i++)
		{
			for (int j = 0; j < arr[i].length; j++)
				totSize += arr[i][j].length;
		}
		return totSize;
	}

	// Row first copy of tridimensionalal array
	private static int[] copyToLinearized(int arr[][][], int linArr[])
	{
		int currSize = 0;
		for (int i = 0; i < arr.length; i++)
		{
			for (int j = 0; j < arr[i].length; j++)
			{
				System.arraycopy(linArr, currSize, arr[i][j], 0, arr[i][j].length);
				currSize += arr[i][j].length;
			}
		}
		return linArr;
	}

	// NOTE same diemnsion assumed along sub arrays
	private static int[] getDims(int arr[][][])
	{
		final int d[] = new int[3];
		d[0] = arr.length;
		d[1] = arr[0].length;
		d[2] = arr[0][0].length;
		return d;
	}

	public Int32Array(int[][][] inDatum) throws MdsException
	{
		this(copyToLinearized(inDatum, new int[(getTotSize(inDatum))]), getDims(inDatum));
	}

	@Override
	public boolean equals(Object obj)
	{
		if (!(obj instanceof Int32Array))
			return false;
		final Int32Array data = (Int32Array) obj;
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

	public static Data getData(int[] datum, int[] dims, Data help, Data units, Data error, Data validation)
	{
		try
		{
			return new Int32Array(datum, dims, help, units, error, validation);
		}
		catch (final Exception exc)
		{
			return null;
		}
	}

	@Override
	public Data getElementAt(int idx)
	{
		return new Int32(datum[idx]);
	}

	@Override
	public int getSize()
	{ return datum.length; }

	@Override
	protected Array getPortionAt(int startIdx, int[] newDims, int newSize) throws MdsException
	{
		final int newDatum[] = new int[newSize];
		System.arraycopy(datum, startIdx, newDatum, 0, newSize);
		return new Int32Array(newDatum, newDims);
	}

	@Override
	public void setElementAt(int idx, Data data) throws MdsException
	{
		datum[idx] = data.getInt();
	}

	@Override
	protected void setPortionAt(Array data, int startIdx, int size) throws MdsException
	{
		final int[] newDatum = data.getIntArray();
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
			retDatum[i] = datum[i];
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
	{ return getSize() * 4; }
}
