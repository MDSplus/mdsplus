package mdsdata;

public class DoubleData extends ScalarData
{
    double data;
    public static Data getData(double data)
    {
	return new DoubleData(data);
    }
    public DoubleData(double data)
    {
        dtype = Data.DTYPE_DOUBLE;
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