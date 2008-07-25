package mdsdata;

public class FloatData extends ScalarData
{
    float data;
    public static Data getData(float data)
    {
	return new FloatData(data);
    }
    public FloatData(float data)
    {
        dtype = Data.DTYPE_FLOAT;
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