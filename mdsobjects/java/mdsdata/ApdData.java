package mdsdata;

public class ApdData extends Data
{
    Data descs[];
    public static Data getData(Data data[])
    {
	return new ApdData(data);
    }
    
    public ApdData() 
    {
        clazz = Data.CLASS_APD;
        dtype = Data.DTYPE_DSC;
    }
    public ApdData(Data descs[])
    {
        clazz = Data.CLASS_APD;
        dtype = Data.DTYPE_DSC;
        this.descs = descs;
    }
    
    public Data[] getDescArray()
    {
        Data[] retData = new Data[descs.length];
        for(int i = 0; i < descs.length; i++)
            retData[i] = descs[i];
        return retData;
    }
}
