//package jTraverser;

public class FloatData extends AtomicData
{
    float datum;
    public static Data getData(float datum)
    {
	return new FloatData(datum);
    }
    public FloatData(float datum)
    {
	dtype = DTYPE_FLOAT;
	this.datum = datum;
    }
    public int getInt() {return (int)datum; }
    public float getFloat() {return datum; }
}