package mdsdata;

public class NidData extends ScalarData
{
    int data;
    public static Data getData(int data)
    {
	return new NidData(data);
    }
    public NidData(int data)
    {
        dtype = Data.DTYPE_NID;
        this.data = data;
    }

    public int getInt()
    {
        return data;
    }
}