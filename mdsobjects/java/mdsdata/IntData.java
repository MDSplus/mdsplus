package mdsdata;

public class IntData extends ScalarData
{
    int data;
    public static Data getData(int data)
    {
	return new IntData(data);
    }
    public static Data getData(int data, boolean isUnsigned)
    {
	return new IntData(data, isUnsigned);
    }
    public IntData(int data)
    {
        dtype = Data.DTYPE_L;
        this.data = data;
    }
    public IntData(int data, boolean isUnsigned)
    {
        if(isUnsigned)
            dtype = Data.DTYPE_LU;
        else
            dtype = Data.DTYPE_L;
        this.data = data;
    }
    public byte getByte()
    {
        return (byte)data;
    }
    public short getShort()
    {
        return (short)data;
    }
    public int getInt()
    {
        return (int)data;
    }
    public long getLong()
    {
        return (long)data;
    }
    public float getFloat()
    {
        return (float)data;
    }
    public double getDouble()
    {
        return (double)data;
    }

    public String getString()
    {
        return ""+data;
    }
}