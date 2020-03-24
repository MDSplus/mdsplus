//package jTraverser;

public class DimensionData extends CompoundData
{
    public static Data getData() {return new DimensionData(); }
    public DimensionData() {dtype = DTYPE_DIMENSION; }
    public DimensionData(Data window, Data axis)
    {
	dtype = DTYPE_DIMENSION;
	descs = new Data[2];
	descs[0] = window;
	descs[1] = axis;
    }
    public final Data getWindow() {return descs[0]; }
    public final Data getAxis()   {return descs[1]; }
}