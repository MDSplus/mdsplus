//package jTraverser;

public class DoubleArray extends ArrayData
{
    double datum[];
    int flags = DTYPE_DOUBLE;
    public static Data getData(double datum[], int flags)
    {
	    return new DoubleArray(datum, flags);
    }
    public DoubleArray(double datum[]){this(datum, DTYPE_DOUBLE);}
    public DoubleArray(double datum[], int flags)
    {
	    length = datum.length;
	    dtype = DTYPE_DOUBLE;
	    this.datum = new double[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        this.datum[i] = datum[i];
	    this.flags = flags;
    }
    public int [] getIntArray()
    {
	    int ris [] = new int[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        ris[i] = (int)datum[i];
	    return ris;
    }
    public float [] getFloatArray()
    {
	    float ris [] = new float[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        ris[i] = (float)datum[i];
	    return ris;
    }
    public int getFlags(){return flags;}
}