//package jTraverser;

public class DoubleData extends AtomicData
{
    double datum;
    int flags = DTYPE_DOUBLE;
    public static Data getData(double datum, int flags)
    {
	    return new DoubleData(datum, flags);
    }
    public DoubleData(double datum) {this(datum, DTYPE_DOUBLE);}
    public DoubleData(double datum, int flags)
    {
	    dtype = DTYPE_DOUBLE;
	    this.datum = datum;
	    this.flags = flags;
    }
    public int getInt() {return (int)datum; }
    public float getFloat() {return (float)datum; }
    public double getDouble() {return datum; }
    public int getFlags() {return flags;}
}
