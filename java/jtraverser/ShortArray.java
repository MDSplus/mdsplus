//package jTraverser;

public class ShortArray extends ArrayData
{
    short datum[];
    public static Data getData(short datum[], boolean unsigned)
    {
	return new ShortArray(datum, unsigned);
    }
    public ShortArray(short datum[], boolean unsigned)
    {
	length = datum.length;
	if(unsigned)
	    dtype = DTYPE_WU;
	else
	    dtype = DTYPE_W;
	this.datum = new short[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public ShortArray(short datum[]) { this(datum, false); }
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