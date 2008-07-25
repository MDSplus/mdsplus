package mdsdata;

public class LongData extends ScalarData
{
    long data;
    public static Data getData(long data)
    {
	return new LongData(data);
    }
    public static Data getData(long data, boolean isUnsigned)
    {
	return new LongData(data, isUnsigned);
    }
    public LongData(long data)
    {
        dtype = Data.DTYPE_Q;
        this.data = data;
    }
    public LongData(long data, boolean isUnsigned)
    {
        if(isUnsigned)
            dtype = Data.DTYPE_QU;
        else
            dtype = Data.DTYPE_Q;
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