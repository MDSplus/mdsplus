package mdsdata;

public class RangeData extends CompoundData
{
    public static Data getData()
    {
	return new RangeData();
    }
    public RangeData()
    {
        dtype = Data.DTYPE_RANGE;
    }
    public RangeData(Data begin, Data ending, Data deltaval)
    {
        dtype = Data.DTYPE_RANGE;
        descs = new Data[3];
        descs[0] = begin;
        descs[1] = ending;
        descs[2] = deltaval;
    }
    public Data getBegin() {return descs[0];}
    public Data getEnding() { return descs[1];}
    public Data getDeltaval() { return descs[2];}
    public void setBegin(Data begin) {descs[0] = begin;}
    public void setHelp(Data ending) {descs[1] = ending;}
    public void setValidation(Data deltaval) {descs[2] = deltaval;}
}
