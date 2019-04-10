package MDSplus;

/**
 * Array description for  DTYPE_T .
 *
 * Will handle arrays of string like TDI, i.e. padding to the longest string, for
 * C++, in order to retain compatibility with CLASS_A, DTYPE_Tdescriptors.
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.42
 */
public class StringArray extends Array
{
    java.lang.String [] datum;
    public StringArray(java.lang.String[] inDatum)
    {
	help = null;
	units = null;
	error = null;
	validation = null;
	clazz = CLASS_A;
	dtype = DTYPE_T;
	dims = new int[1];
	dims[0] = inDatum.length;
	datum = new java.lang.String[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
    }
    public StringArray(java.lang.String[] inDatum, int []dims) throws MdsException
    {
	this(inDatum, dims, null, null, null, null);
    }
    public StringArray(java.lang.String[] inDatum, int []dims, Data help, Data units, Data error, Data validation) throws MdsException
    {
	super(dims, help, units, error, validation);
	clazz = CLASS_A;
	dtype = DTYPE_T;
//        if(inDatum.length != getSize())
//            throw new MdsException("Invalid array size in Uint8 Array constructor");
	datum = new java.lang.String[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
	setShape(dims);
    }
    public boolean equals(Object obj)
    {
	if(!(obj instanceof StringArray))
	    return false;
	StringArray data = (StringArray)obj;
	if(data.dims.length != dims.length)
	    return false;
	for(int i = 0; i < dims.length; i++)
	    if(data.dims[i] != dims[i])
	        return false;
	for(int i = 0; i < datum.length; i++)
	    if(!data.datum[i].equals(datum[i]))
	        return false;
	return true;
    }


    public static Data getData(java.lang.String []datum, int []dims, Data help, Data units, Data error, Data validation)
    {
	try {
	    return new StringArray(datum, dims, help, units, error, validation);
	}catch(Exception exc)
	{
	    return null;
	}
    }
    public Data getElementAt(int idx)
    {
	return new String(datum[idx]);
    }

    public int getSize(){return datum.length;}
    protected Array getPortionAt(int startIdx, int []newDims, int newSize) throws MdsException
    {
	java.lang.String newDatum[] = new java.lang.String[newSize];
	System.arraycopy(datum, startIdx, newDatum, 0, newSize);
	return new StringArray(newDatum, newDims);
    }
    public void setElementAt(int idx, Data data) throws MdsException
    {
	datum[idx] = data.getString();
    }
    protected  void setPortionAt(Array data, int startIdx, int size) throws MdsException
    {
	java.lang.String [] newDatum = data.getStringArray();
	System.arraycopy(datum, startIdx, newDatum, 0, size);
    }
    public byte[] getByteArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to byte array");
    }
    public short[] getShortArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to short array");
    }
    public int[] getIntArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to int array");
    }
    public long[] getLongArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to long array");
    }
    public float[] getFloatArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to float array");
    }
    public double[] getDoubleArray() throws MdsException
    {
	throw new MdsException("Cannot convert string array to double array");
    }
    public java.lang.String[] getStringArray()
    {
	java.lang.String retStrings[] = new java.lang.String[datum.length];
	System.arraycopy(datum, 0, retStrings, 0, datum.length);
	return retStrings;
    }
    public int getSizeInBytes()
    {
	//find longest string
	int maxLen = 0;
	for(int i = 0; i < datum.length; i++)
	    if(datum[i] != null && datum[i].length() > maxLen)
	        maxLen = datum[i].length();
	return datum.length * maxLen;
    }
 }