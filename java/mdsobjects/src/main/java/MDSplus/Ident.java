package MDSplus;

/**
 * Class description of DTYPE_IDENT. It describes a TDI variable. When
 * evaluated, it returns the content of this variable. Constructor:Ident(string)
 *
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.37
 */
public class Ident extends Data
{
	java.lang.String datum;

	public Ident(java.lang.String datum)
	{
		this(datum, null, null, null, null);
	}

	public Ident(java.lang.String datum, Data help, Data units, Data error, Data validation)
	{
		super(help, units, error, validation);
		clazz = CLASS_S;
		dtype = DTYPE_IDENT;
		this.datum = datum;
	}

	public static Data getData(java.lang.String datum, Data help, Data units, Data error, Data validation)
	{
		return new Ident(datum, help, units, error, validation);
	}

	@Override
	public byte getByte() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a byte");
	}

	/**
	 * Convert this data into a short. Implemented at this class level by returning
	 * TDI data(WORD(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public short getShort() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a short");
	}

	/**
	 * Convert this data into a int. Implemented at this class level by returning
	 * TDI data(LONG(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public int getInt() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a int");
	}

	/**
	 * Convert this data into a long. Implemented at this class level by returning
	 * TDI data(QUADWORD(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public long getLong() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a long");
	}

	/**
	 * Convert this data into a float. Implemented at this class level by returning
	 * TDI data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
	 * generated an exception.
	 */
	@Override
	public float getFloat() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a float");
	}

	/**
	 * Convert this data into a double. Implemented at this class level by returning
	 * TDI data(FT_FLOAT(this)). If data() fails or the returned class is not
	 * scalar, generated an exception.
	 */
	@Override
	public double getDouble() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a double");
	}

	/**
	 * Convert this data into a byte array. Implemented at this class level by
	 * returning TDI data(BYTE(this)). If data() fails or the returned class is not
	 * array, generates an exception. In Java and C++ will return a 1 dimensional
	 * array using row-first ordering if a multidimensional array.
	 */
	@Override
	public byte[] getByteArray() throws MdsException
	{
		throw new MdsException("Cannot convert a Ident to a byte array");
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
		throw new MdsException("Cannot convert a Ident to a short array");
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
		throw new MdsException("Cannot convert a Ident to a int array");
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
		throw new MdsException("Cannot convert a Ident to a long array");
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
		throw new MdsException("Cannot convert a Ident to a float array");
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
		throw new MdsException("Cannot convert a Ident to a double array");
	}

	@Override
	public java.lang.String getString()
	{ return datum; }

	public Ident()
	{}

	@Override
	public void finalize() throws Throwable
	{
		super.finalize();
	}
}
