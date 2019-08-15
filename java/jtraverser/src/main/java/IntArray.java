//package jTraverser;

public class IntArray extends ArrayData
{
    int datum[];
    public static Data getData(int datum[], boolean unsigned)
    {
	return new IntArray(datum, unsigned);
    }
    public IntArray(int datum[], boolean unsigned)
    {
	length = datum.length;
	if(unsigned)
	    dtype = DTYPE_LU;
	else
	    dtype = DTYPE_L;
	this.datum = new int[datum.length];
	for(int i = 0; i < datum.length; i++)
	    this.datum[i] = datum[i];
    }
    public IntArray(int datum[]) { this(datum, false); }
    public int [] getIntArray() {return datum; }
    public float [] getFloatArray()
    {
	float ris [] = new float[datum.length];
	for(int i = 0; i < datum.length; i++)
	    ris[i] = (float)datum[i];
	return ris;
    }
}