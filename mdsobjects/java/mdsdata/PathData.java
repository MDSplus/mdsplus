package mdsdata;

public class PathData extends ScalarData
{
    String data;
    public static Data getData(String data)
    {
	return new PathData(data);
    }
    public PathData(String data)
    {
        dtype = Data.DTYPE_PATH;
        this.data = data;
    }

    public String getString()
    {
        return data;
    }
}