package mdsdata;

public class LongArray extends ArrayData
{
    long[] data;
    
    public static Data getData(long [] data)
    {
	return new LongArray(data);
    }
    public static Data getData(long [] data, boolean isUnsigned)
    {
	return new LongArray(data, isUnsigned);
    }
    public LongArray(long [] data)
    {
        super(data.length);
        dtype = Data.DTYPE_Q;
        this.data = data;
    }
    public LongArray(long [] data, boolean isUnsigned)
    {
        super(data.length);
         if(isUnsigned)
            dtype = Data.DTYPE_QU;
        else
            dtype = Data.DTYPE_Q;
        this.data = data;
    }
    public LongArray(long [] data, int dims[])
    {
        super(dims);
        dtype = Data.DTYPE_Q;
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
