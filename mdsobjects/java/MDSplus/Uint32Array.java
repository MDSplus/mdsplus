package MDSplus;

/**
 * Array description for  DTYPE_LU
 *
 * Constructors:Uint32Array(int[])
 * @author manduchi
 * @version 1.0
 * @updated 30-mar-2009 13.44.52
 */
public class Uint32Array extends Array
{
    int [] datum;
    public Uint32Array(int[] inDatum)
    {
	help = null;
	units = null;
	error = null;
	validation = null;
	clazz = CLASS_A;
	dtype = DTYPE_LU;
	dims = new int[1];
	dims[0] = inDatum.length;
	datum = new int[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
    }
    public Uint32Array(int[] inDatum, int []dims) throws MdsException
    {
	this(inDatum, dims, null, null, null, null);
    }
    public Uint32Array(int[] inDatum, int []dims, Data help, Data units, Data error, Data validation) throws MdsException
    {
	super(dims, help, units, error, validation);
	clazz = CLASS_A;
	dtype = DTYPE_LU;
	datum = new int[inDatum.length];
	System.arraycopy(inDatum, 0, datum, 0, inDatum.length);
	setShape(dims);
    }
    public boolean equals(Object obj)
    {
	if(!(obj instanceof Uint32Array))
	    return false;
	Uint32Array data = (Uint32Array)obj;
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


    public static Data getData(int []datum, int []dims, Data help, Data units, Data error, Data validation)
    {
	try {
	    return new Uint32Array(datum, dims, help, units, error, validation);
	}catch(Exception exc){return null;}
    }
    public Data getElementAt(int idx)
    {
	return new Uint32(datum[idx]);
    }

    public int getSize(){return datum.length;}
    protected Array getPortionAt(int startIdx, int []newDims, int newSize) throws MdsException
    {
	int newDatum[] = new int[newSize];
	System.arraycopy(datum, startIdx, newDatum, 0, newSize);
	return new Uint32Array(newDatum, newDims);
    }
    public void setElementAt(int idx, Data data) throws MdsException
    {
	datum[idx] = data.getInt();
    }
    protected  void setPortionAt(Array data, int startIdx, int size) throws MdsException
    {
	int [] newDatum = data.getIntArray();
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
    public int getSizeInBytes() {return getSize() * 4;}

}