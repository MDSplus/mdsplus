//package jTraverser;

public class QuadData extends AtomicData
{
    long datum;
    public static Data getData(long datum, boolean unsigned)
    {
	return new QuadData(datum, unsigned);
    }
    public QuadData(long datum, boolean unsigned)
    {
	if(unsigned)
	    dtype = DTYPE_QU;
	else
	    dtype = DTYPE_Q;
	this.datum = datum;
    }
    public QuadData(long datum) { this(datum, false); }
    public int getInt() {return (int)datum; }
    public float getFloat() {return (float) datum; }
}