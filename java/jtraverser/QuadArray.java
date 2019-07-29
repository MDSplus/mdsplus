//package jTraverser;

public class QuadArray extends ArrayData
{
    long datum[];
    public static Data getData(long datum[], boolean unsigned)
    {
	return new QuadArray(datum, unsigned);
    }
    public QuadArray(long datum[], boolean unsigned)
    {
	length = datum.length;
	if(unsigned)
	    dtype = DTYPE_QU;
	else
	    dtype = DTYPE_Q;
	this.datum = new long[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public QuadArray(long datum[]) { this(datum, false); }
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
}