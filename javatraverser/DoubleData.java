package jTraverser;

public class DoubleData extends AtomicData
{
    double datum;
    public static Data getData(double datum)
    {
	return new DoubleData(datum);
    }
    public DoubleData(double datum)
    {
	dtype = DTYPE_DOUBLE;
	this.datum = datum;
    }
    public int getInt() {return (int)datum; }
    public float getFloat() {return (int)datum; }
}
