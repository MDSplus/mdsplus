package MDSplus;

/**
 * Array description for  DTYPE_WU
 *
 * Constructors:Uint16Array(short[])
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.52
 */
public class Uint16Array extends Array
{
    short [] datum;
    public Uint16Array(short[] inDatum)
    {
	help = null;
	units = null;
	error = null;
	validation = null;
	clazz = CLASS_A;
	dtype = DTYPE_WU;
	dims = new int[1];
	dims[0] = inDatum.length;
	datum = new short[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
    }
    public Uint16Array(short[] inDatum, int []dims) throws MdsException
    {
	this(inDatum, dims, null, null, null, null);
    }
    public Uint16Array(short[] inDatum, int []dims, Data help, Data units, Data error, Data validation) throws MdsException
    {
	super(dims, help, units, error, validation);
	clazz = CLASS_A;
	dtype = DTYPE_WU;
	datum = new short[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
	setShape(dims);
    }
    public boolean equals(Object obj)
    {
	if(!(obj instanceof Uint16Array))
	    return false;
	Uint16Array data = (Uint16Array)obj;
	if(data.dims.length != dims.length)
	    return false;
	for(int i = 0; i < dims.length; i++)
	    if(data.dims[i] != dims[i])
	        return false;
	for(int i = 0; i < datum.length; i++)
	    if(data.datum[i] != datum[i])
	        return false;
	return true;
    }

    public static Data getData(short []datum, int []dims, Data help, Data units, Data error, Data validation)
    {
	try {
	    return new Uint16Array(datum, dims, help, units, error, validation);
	}catch(Exception exc){return null;}
    }
    public Data getElementAt(int idx)
    {
	return new Uint16(datum[idx]);
    }

    public int getSize(){return datum.length;}
    protected Array getPortionAt(int startIdx, int []newDims, int newSize) throws MdsException
    {
	short newDatum[] = new short[newSize];
	System.arraycopy(datum, startIdx, newDatum, 0, newSize);
	return new Uint16Array(newDatum, newDims);
    }
    public void setElementAt(int idx, Data data) throws MdsException
    {
	datum[idx] = data.getShort();
    }
    protected  void setPortionAt(Array data, int startIdx, int size) throws MdsException
    {
	short [] newDatum = data.getShortArray();
	System.arraycopy(datum, startIdx, newDatum, 0, size);
    }
    public byte[] getByteArray()
    {
	byte[] retDatum = new byte[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (byte)datum[i];
	return retDatum;
    }
    public short[] getShortArray()
    {
	short[] retDatum = new short[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (short)datum[i];
	return retDatum;
    }
    public int[] getIntArray()
    {
	int[] retDatum = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (int)datum[i];
	return retDatum;
    }
    public long[] getLongArray()
    {
	long[] retDatum = new long[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (long)datum[i];
	return retDatum;
    }
    public float[] getFloatArray()
    {
	float[] retDatum = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (float)datum[i];
	return retDatum;
    }
    public double[] getDoubleArray()
    {
	double[] retDatum = new double[datum.length];
	for(int i = 0; i < datum.length; i++)
	    retDatum[i] = (double)datum[i];
	return retDatum;
    }
    public java.lang.String[] getStringArray() throws MdsException
    {
	throw new MdsException("Cannot convert byte array to string array");
    }
    public int getSizeInBytes() {return getSize() * 2;}

}