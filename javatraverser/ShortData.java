//package jTraverser;

public class ShortData extends AtomicData
{
    short datum;
    public static Data getData(short datum, boolean unsigned)
    {
	return new ShortData(datum, unsigned);
    }
    public ShortData(short datum, boolean unsigned)
    {
	if(unsigned)
	    dtype = DTYPE_WU;
	else
	    dtype = DTYPE_W;
	this.datum = datum;
    }
    public ShortData(short datum) { this(datum, false); }
    public int getInt() {return (int )datum; }
    public float getFloat() {return (float) datum; }
}