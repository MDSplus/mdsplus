package mdsdata;

public class ShortData extends ScalarData
{
    short data;
    public static Data getData(short data)
    {
	return new ShortData(data);
    }
    public ShortData(short data)
    {
        dtype = Data.DTYPE_W;
        this.data = data;
    }
    public ShortData(short data, boolean isUnsigned)
    {
        if(isUnsigned)
            dtype = Data.DTYPE_WU;
        else
            dtype = Data.DTYPE_W;
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