package mdsdata;

public class IdentData extends ScalarData
{
    String data;
    public static Data getData(String data)
    {
	return new IdentData(data);
    }
    public IdentData(String data)
    {
        dtype = Data.DTYPE_IDENT;
        this.data = data;
    }

    public String getString()
    {
        return data;
    }
}