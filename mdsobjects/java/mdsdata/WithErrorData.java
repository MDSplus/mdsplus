package mdsdata;

public class WithErrorData extends CompoundData
{
    public static Data getData()
    {
	return new WithErrorData();
    }
    public WithErrorData()
    {
        dtype = Data.DTYPE_WITH_ERROR;
    }
    public WithErrorData(Data data, Data error)
    {
        dtype = Data.DTYPE_WITH_ERROR;
        descs = new Data[2];
        descs[0] = data;
        descs[1] = error;
    }
    public Data getDatum() {return descs[0];}
    public Data getError() { return descs[1];}
    public void setData(Data data) {descs[0] = data;}
    public void setError(Data error) {descs[1] = error;}
}
