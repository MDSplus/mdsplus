//package jTraverser;

public class FloatData extends AtomicData
{
    float datum;
    int flags = DTYPE_FLOAT;
    public static Data getData(float datum, int flags)
    {
	    return new FloatData(datum, flags);
    }
    public FloatData(float datum){this(datum, DTYPE_FLOAT);}
    public FloatData(float datum, int flags)
    {
	    dtype = DTYPE_FLOAT;
	    this.datum = datum;
	    this.flags = flags;
    }
    public int getInt() {return (int)datum; }
    public float getFloat() {return datum; }
    public int getFlags() {return flags;}
}