package MDSplus;

/**
 * Class description of DTYPE_B
 *
 * Constructors:Uint8(byte)
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.52
 */
public class Uint8 extends Scalar
{
	byte datum;

	public Uint8(byte datum)
	{
		this(datum, null, null, null, null);
	}

	public Uint8(byte datum, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_S;
		dtype = DTYPE_BU;
		this.datum = datum;
	}

	@Override
	public boolean equals(Object data)
	{
		if (!(data instanceof Uint8))
			return false;
		return ((Uint8) data).datum == datum;
	}

	public static Data getData(byte datum, Data help, Data units, Data error, Data validation)
	{
		return new Uint8(datum, help, units, error, validation);
	}

	@Override
	public byte getByte() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a short. Implemented at this class level by returning
	 * TDI data(WORD(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public short getShort() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a int. Implemented at this class level by returning
	 * TDI data(LONG(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public int getInt() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a long. Implemented at this class level by returning
	 * TDI data(QUADWORD(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public long getLong() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a float. Implemented at this class level by returning
	 * TDI data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public float getFloat() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a double. Implemented at this class level by returning
	 * TDI data(FT_FLOAT(this)). If data() fails or the returned class is not
	 * scalar, generated an exception.
	 */
	@Override
	public double getDouble() throws MdsException
	{ return datum; }

	/**
	 * Convert this data into a byte array. Implemented at this class level by
	 * returning TDI data(BYTE(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array.
	 */
	@Override
	public byte[] getByteArray() throws MdsException
	{
		return new byte[]
		{ datum };
	}

	/**
	 * Convert this data into a short array. Implemented at this class level by
	 * returning TDI data(WORD(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array.
	 */
	@Override
	public short[] getShortArray() throws MdsException
	{
		return new short[]
		{ datum };
	}

	/**
	 * Convert this data into a int array. Implemented at this class level by
	 * returning TDI data (LONG(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array.
	 */
	@Override
	public int[] getIntArray() throws MdsException
	{
		return new int[]
		{ datum };
	}

	/**
	 * Convert this data into a long array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1
	 * dimensional array using row-first ordering if a multidimensional array.
	 */
	@Override
	public long[] getLongArray() throws MdsException
	{
		return new long[]
		{ datum };
	}

	/**
	 * Convert this data into a float array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1
	 * dimensional array using row-first ordering if a multidimensional array.
	 */
	@Override
	public float[] getFloatArray() throws MdsException
	{
		return new float[]
		{ datum };
	}

	/**
	 * Convert this data into a long array. Implemented at this class level by
	 * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
	 * not array, generates an exception. In Java and C++ will return a 1
	 * dimensional array using row-first ordering if a multidimensional array.
	 */
	@Override
	public double[] getDoubleArray() throws MdsException
	{
		return new double[]
		{ datum };
	}

	@Override
	public int getSizeInBytes()
	{ return 1; }
}
