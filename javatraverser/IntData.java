//package jTraverser;

public class IntData extends AtomicData
{
    int datum;
    public static Data getData(int datum, boolean unsigned)
	{return new IntData(datum, unsigned);}
    public IntData() {}
    public IntData(int datum, boolean unsigned)
    {
	if(unsigned)
	    dtype = DTYPE_LU;
	else
	    dtype = DTYPE_L;
	this.datum = datum;
    }
    public IntData(int datum) { this(datum, false); }
    public int getInt() {return datum; }
    public float getFloat() {return (float) datum; }
}