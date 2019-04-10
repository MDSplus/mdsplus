//package jTraverser;

public class NidData extends IntData implements NodeId
{
    public static Data getData(int datum)
    {
	    return new NidData(datum);
    }
    public NidData()
    {
	    dtype = DTYPE_NID;
    }
    public NidData(int nid)
    {
	    dtype = DTYPE_NID;
	    datum = nid;
    }
    public int getInt()
    {
	return datum;
    }
    public boolean isResolved() {return true; }
    public void incrementNid()
    {
	datum++;
    }
    public boolean equals(Object obj)
    {
	if(!(obj instanceof NidData)) return false;
	return datum == ((NidData)obj).datum;
    }
}