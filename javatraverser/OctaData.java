//package jTraverser;

public class OctaData extends AtomicData
{
    long datum[];
    public static Data getData(long datum[], boolean unsigned)
    {
	return new OctaData(datum, unsigned);
    }
    public OctaData(long datum[], boolean unsigned)
    {
	if(unsigned)
	    dtype = DTYPE_OU;
	else
	    dtype = DTYPE_O;
	this.datum = new long[2];
	this.datum[0] = datum[0];
	this.datum[1] = datum[1];
    }
    public OctaData(long datum[]) { this(datum, false); }
    public int getInt() {return (int)datum[0]; }
    public float getFloat() {return (float) datum[0]; }
}