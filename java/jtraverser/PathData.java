//package jTraverser;

public class PathData extends StringData implements NodeId
{
    public static Data getData(String datum)
    {
	return new PathData(datum);
    }
    public PathData(String path)
    {
	datum = path;
	dtype = DTYPE_PATH;
    }
    public boolean isResolved() {return false;}
}
