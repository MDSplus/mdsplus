package mdsdata;

public class WindowData extends CompoundData
{
    public static Data getData()
    {
	return new WindowData();
    }
    public WindowData()
    {
        dtype = Data.DTYPE_WINDOW;
    }
    public WindowData(Data startIdx, Data endIdx, Data valueAt0)
    {
        dtype = Data.DTYPE_WINDOW;
        descs = new Data[3];
        descs[0] = startIdx;
        descs[1] = endIdx;
        descs[2] = valueAt0;
    }
    public Data getStartIdx() {return descs[0];}
    public Data getEndIdx() { return descs[1];}
    public Data getValueAt0() { return descs[2];}
    public void setStartIdx(Data startIdx) {descs[0] = startIdx;}
    public void setEndIdx(Data endIdx) {descs[1] = endIdx;}
    public void setValidation(Data valueAt0) {descs[2] = valueAt0;}
}
