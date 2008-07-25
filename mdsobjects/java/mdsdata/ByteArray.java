package mdsdata;

public class ByteArray extends ArrayData
{
    byte[] data;
    public static Data getData(byte data[])
    {
	return new ByteArray(data);
    }
    public static Data getData(byte data[], boolean isUnsigned)
    {
	return new ByteArray(data, isUnsigned);
    }
    
    public ByteArray(byte [] data)
    {
        super(data.length);
        dtype = Data.DTYPE_B;
        this.data = data;
    }
    public ByteArray(byte [] data, boolean isUnsigned)
    {
        super(data.length);
        if(isUnsigned)
            dtype = Data.DTYPE_BU;
        else
            dtype = Data.DTYPE_B;
        this.data = data;
    }
    public ByteArray(byte [] data, int dims[])
    {
        super(dims);
        dtype = Data.DTYPE_B;
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
