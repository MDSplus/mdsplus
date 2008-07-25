package mdsdata;

public class ByteData extends ScalarData
{
    byte data;
    public static Data getData(byte data)
    {
	return new ByteData(data);
    }
    public static Data getData(byte data, boolean isUnsigned)
    {
	return new ByteData(data, isUnsigned);
    }
    public ByteData(byte data)
    {
        dtype = Data.DTYPE_B;
        this.data = data;
    }
    public ByteData(byte data, boolean isUnsigned)
    {
        if(isUnsigned)
            dtype = Data.DTYPE_BU;
        else
            dtype = Data.DTYPE_B;
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