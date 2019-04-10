//package jTraverser;

public class IdentData extends StringData
{
    public static Data getData(String datum)
    {
	    return new IdentData(datum);
    }
    public IdentData() {}
    public IdentData(String datum)
    {
	super(datum);
	    dtype = DTYPE_IDENT;
    }
    public String getString() {return datum; }
}