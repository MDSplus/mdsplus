//package jTraverser;

public class RangeData extends CompoundData
{
    public static Data getData() {return new RangeData(); }
    public RangeData() {dtype = DTYPE_RANGE; }
    public RangeData(Data begin, Data end, Data delta)
    {
	dtype = DTYPE_RANGE;
	descs = new Data[3];
	descs[0] = begin;
	descs[1] = end;
	descs[2] = delta;
    }

    public Data getBegin()  {return descs[0]; }
    public Data getEnd()    {return descs[1]; }
    public Data getDelta()  {return descs[2]; }
}