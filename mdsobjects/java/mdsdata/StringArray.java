package mdsdata;

public class StringArray extends ArrayData
{
    String[] data;
    
    public static Data getData(String []data)
    {
	return new StringArray(data);
    }
    public StringArray(String [] data)
    {
        super(data.length);
        dtype = Data.DTYPE_T;
        this.data = data;
    }

    public String[] getStringArray()
    {
        String arr[] = new String[data.length];
        for(int i = 0; i < data.length; i++)
            arr[i] = data[i];
        return arr;    
    }
}
