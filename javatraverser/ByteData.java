//package jTraverser;

public class ByteData extends AtomicData
{
    byte datum;
    public static Data getData(byte datum, boolean unsigned)
    {
	return new ByteData(datum, unsigned);
    }
    public ByteData(byte datum, boolean unsigned)
    {
	if(unsigned)
	    dtype = DTYPE_BU;
	else
	    dtype = DTYPE_B;
	this.datum = datum;
    }
    public ByteData(byte datum) { this(datum, false); }
    public int getInt() {return (int)datum; }
    public float getFloat() {return (float) datum; }
}