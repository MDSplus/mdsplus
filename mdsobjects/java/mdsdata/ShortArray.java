package mdsdata;

public class ShortArray extends ArrayData
{
    short[] data;
    public static Data getData(short [] data)
    {
	return new ShortArray(data);
    }
    
    public ShortArray(short [] data)
    {
        super(data.length);
        dtype = Data.DTYPE_W;
        this.data = data;
    }
    public ShortArray(short [] data, boolean isUnsigned)
    {
        super(data.length);
        if(isUnsigned)
            dtype = Data.DTYPE_WU;
        else
            dtype = Data.DTYPE_W;
        this.data = data;
    }
    public ShortArray(short [] data, int dims[])
    {
        super(dims);
        dtype = Data.DTYPE_W;
        this.data = data;
    }
    public byte[] getByteArray()
    {
        byte arr[] = new byte[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (byte)data[i];
        return arr;    
    }
    public short[] getShortArray()
    {
        short arr[] = new short[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (short)data[i];
        return arr;    
    }
    public int[] getIntArray()
    {
        int arr[] = new int[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (int)data[i];
        return arr;    
    }
    public long[] getLongArray()
    {
        long arr[] = new long[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (long)data[i];
        return arr;    
    }
    public float[] getFloatArray()
    {
        float arr[] = new float[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (float)data[i];
        return arr;    
    }
    public double[] getDoubleArray()
    {
        double arr[] = new double[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = (double)data[i];
        return arr;    
    }

}
