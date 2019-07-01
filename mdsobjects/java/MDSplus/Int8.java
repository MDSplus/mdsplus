package MDSplus;

/**
 * Class description of DTYPE_B
 *
 * Constructors:Int8(byte)
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.39
 */
public class Int8 extends Scalar
{
    byte datum;
    public Int8(byte datum)
    {
	this(datum, null, null, null, null);
    }
    public Int8(byte datum, Data help, Data units, Data error, Data validation)
    {
	super(help, units, error, validation);
	clazz = CLASS_S;
	dtype = DTYPE_B;
	this.datum = datum;
    }
    public boolean equals(Object data)
    {
	if(!(data instanceof Int8))
	    return false;
	return((Int8)data).datum == datum;
    }
    public static Data getData(byte datum, Data help, Data units, Data error, Data validation)
    {
	return new Int8(datum, help, units, error, validation);
    }

    public  byte getByte() throws MdsException
    {
	return datum;
    }

    /**
     * Convert this data into a short. Implemented at this class level by returning TDI
     * data(WORD(this)).If data() fails or the returned class is not scalar, generated
     * an exception.
     */
    public  short getShort() throws MdsException
    {
	return (short)datum;
    }

    /**
     * Convert this data into a int. Implemented at this class level by returning TDI
     * data(LONG(this)).If data() fails or the returned class is not scalar, generated
     * an exception.
     */
    public  int getInt() throws MdsException
    {
	return (int)datum;
    }


    /**
     * Convert this data into a long. Implemented at this class level by returning TDI
     * data(QUADWORD(this)).If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  long getLong() throws MdsException
    {
	return (long)datum;
    }

    /**
     * Convert this data into a float. Implemented at this class level by returning TDI
     * data(F_FLOAT(this)).If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  float getFloat()throws MdsException
    {
	return (float)datum;
    }

    /**
     * Convert this data into a double. Implemented at this class level by returning TDI
     * data(FT_FLOAT(this)). If data() fails or the returned class is not scalar,
     * generated an exception.
     */
    public  double getDouble()throws MdsException
    {
	return (double)datum;
    }


    /**
     * Convert this data into a byte array. Implemented at this class level by
     * returning TDI data(BYTE(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  byte[] getByteArray() throws MdsException
    {
	return new byte[]{datum};
    }


    /**
     * Convert this data into a short array. Implemented at this class level by
     * returning TDI data(WORD(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  short[] getShortArray()throws MdsException
    {
	return new short[]{(short)datum};
    }


    /**
     * Convert this data into a int array. Implemented at this class level by
     * returning TDI data (LONG(this)). If data() fails or the returned class is not
     * array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  int[] getIntArray()throws MdsException
    {
	return new int[]{(int)datum};
    }

    /**
     * Convert this data into a long array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  long[] getLongArray()throws MdsException
    {
	return new long[]{(long)datum};
    }

    /**
     * Convert this data into a float array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  float[] getFloatArray()throws MdsException
    {
	return new float[]{(float)datum};
    }

    /**
     * Convert this data into a long array. Implemented at this class level by
     * returning TDI data(QUADWORD(this)). If data() fails or the returned class is
     * not array, generates an exception. In Java and C++ will return a 1 dimensional
     * array using row-first ordering if a multidimensional array.
     */
    public  double[] getDoubleArray()throws MdsException
    {
	return new double[]{(double)datum};
    }
    public int getSizeInBytes() {return 1;}

}