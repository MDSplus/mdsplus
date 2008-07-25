package mdsdata;

public class DimensionData extends CompoundData
{
    public static Data getData(byte data)
    {
	return new DimensionData();
    }
    public DimensionData()
    {
        dtype = Data.DTYPE_DIMENSION;
    }
    public DimensionData(Data window, Data axis)
    {
        dtype = Data.DTYPE_DIMENSION;
        descs = new Data[2];
        descs[0] = window;
        descs[1] = axis;
    }
    public Data getWindow() {return descs[0];}
    public Data getAxis() { return descs[1];}
    public void setWindow(Data window) {descs[0] = window;}
    public void setAxis(Data axis) {descs[1] = axis;}
}
