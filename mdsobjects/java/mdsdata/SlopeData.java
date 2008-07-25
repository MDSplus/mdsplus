package mdsdata;

public class SlopeData extends CompoundData
{
    public static Data getData()
    {
	return new SlopeData();
    }
    public SlopeData()
    {
        dtype = Data.DTYPE_SLOPE;
    }
    public SlopeData(Data slope, Data begin, Data ending)
    {
        dtype = Data.DTYPE_SLOPE;
        descs = new Data[3];
        descs[0] = slope;
        descs[1] = begin;
        descs[2] = ending;
    }
    public Data getSlope() {return descs[0];}
    public Data getBegin() { return descs[1];}
    public Data getEnding() { return descs[2];}
    public void setSlope(Data slope) {descs[0] = slope;}
    public void setBegin(Data begin) {descs[1] = begin;}
    public void setEnding(Data ending) {descs[2] = ending;}
}
