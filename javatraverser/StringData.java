//package jTraverser;

public class StringData extends AtomicData
{
    String datum;
    public static Data getData(String datum)
    {
	    return new StringData(datum);
    }
    public StringData() {}
    public StringData(String datum)
    {
	    dtype = DTYPE_T;
	    this.datum = datum;
    }
    public String getString() {return datum; }
}