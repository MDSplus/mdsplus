//package jTraverser;

public class OctaArray extends ArrayData
{
    long datum[];
    public static Data getData(long datum[], boolean unsigned)
    {
	return new OctaArray(datum, unsigned);
    }

    public OctaArray(long datum[], boolean unsigned)
    {
	length = datum.length/2;
	if(unsigned)
	    dtype = DTYPE_OU;
	else
	    dtype = DTYPE_O;
	this.datum = new long[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public OctaArray(long datum[]) { this(datum, false); }
    public int [] getIntArray()
    {
	int ris [] = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (int)datum[2*i];
	return ris;
    }
    public float [] getFloatArray()
    {
	float ris [] = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (float)datum[2*i];
	return ris;
    }
}