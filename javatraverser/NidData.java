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
    public boolean isResolved() {return true; }
}