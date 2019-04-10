//package jTraverser;

public class SlopeData extends CompoundData
{
    public static Data getData() {return new SlopeData(); }
    public SlopeData() {dtype = DTYPE_SLOPE; }
    public SlopeData(Data slope, Data begin, Data end)
    {
	dtype = DTYPE_SLOPE;
	descs = new Data[3];
	descs[0] = slope;
	descs[1] = begin;
	descs[2] = end;
    }

    public Data getSlope()  {return descs[0]; }
    public Data getBegin()  {return descs[1]; }
    public Data getEnd()    {return descs[2]; }
}
