//package jTraverser;

public class StringArray extends ArrayData
{
    String datum[];
    public static Data getData(String datum[])
    {
	    return new StringArray(datum);
    }
    public StringArray(String datum[])
    {
	    length = datum.length;
	    dtype = DTYPE_T;
	    this.datum = new String[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        this.datum[i] = new String(datum[i]);
    }
    public String [] getStringArray()
    {
	    String ris [] = new String[datum.length];
	    for(int i = 0; i < datum.length; i++)
	        ris[i] = new String(datum[i]);
	    return ris;
    }
}
