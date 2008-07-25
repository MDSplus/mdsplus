package mdsdata;

public class StringData extends ScalarData
{
    String data;
    public static Data getData(String data)
    {
	return new StringData(data);
    }
    public StringData(String data)
    {
        dtype = Data.DTYPE_T;
        this.data = data;
    }

    public String getString()
    {
        return data;
    }
}